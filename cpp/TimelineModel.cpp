// SPDX-FileCopyrightText: 2021 Nheko Contributors
// SPDX-FileCopyrightText: 2022 Nheko Contributors
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TimelineModel.h"

#include <algorithm>
#include <thread>
#include <type_traits>

#include <QCache>
#include <QClipboard>
#include <QDesktopServices>
#include <QFileDialog>
#include <QGuiApplication>
#include <QMimeDatabase>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QQmlEngine>
#include <QVariant>
#include <utility>

#include <matrix-client-library/Config.h>
#include <matrix-client-library/EventAccessors.h>

#include "ui/emoji/EmojiModel.h"
#include "ui/CombinedImagePackModel.h"
#include "CompletionProxyModel.h"
#include "RoomsModel.h"
#include "MemberList.h"
#include "UsersModel.h"
#include "GlobalObject.h"

Q_DECLARE_METATYPE(QModelIndex)

namespace std {
inline uint // clazy:exclude=qhash-namespace
qHash(const std::string &key, uint seed = 0)
{
    return qHash(QByteArray::fromRawData(key.data(), (int)key.length()), seed);
}
}

TimelineModel::TimelineModel(
    QString room_id, QObject *parent)
  : QAbstractListModel(parent)
  , room_id_(std::move(room_id))
  , _timeline(Client::instance()->timeline(room_id_))
{
    events = _timeline->events();
    lastMessage_.timestamp = 0;

    // if (auto create =
    //       cache::client()->getStateEvent<mtx::events::state::Create>(room_id_.toStdString()))
    //     this->isSpace_ = create->content.type == mtx::events::state::room_type::space;
    this->isEncrypted_ = cache::isRoomEncrypted(room_id_.toStdString());

    // this connection will simplify adding the plainRoomNameChanged() signal everywhere that it
    // needs to be
    connect(_timeline, &Timeline::roomNameChanged, this, &TimelineModel::plainRoomNameChanged);
    connect(_timeline, &Timeline::roomNameChanged, this, &TimelineModel::roomNameChanged);
    connect(_timeline, &Timeline::roomTopicChanged, this, &TimelineModel::roomTopicChanged);

    // connect(
    //   this,
    //   &TimelineModel::redactionFailed,
    //   this,
    //   [](const QString &msg) { emit ChatPage::instance()->showNotification(msg); },
    //   Qt::QueuedConnection);

    connect(this, &TimelineModel::dataAtIdChanged, this, [this](const QString &id) {
        relatedEventCacheBuster++;

        auto idx = idToIndex(id);
        if (idx != -1) {
            auto pos = index(idx);
            nhlog::ui()->debug("data changed at {}", id.toStdString());
            emit dataChanged(pos, pos);
        } else {
            nhlog::ui()->debug("id not found {}", id.toStdString());
        }
    });
    connect(events, &EventStore::dataChanged, this, [this](int from, int to) {
        relatedEventCacheBuster++;
        nhlog::ui()->debug(
          "data changed {} to {}", events->size() - to - 1, events->size() - from - 1);
        emit dataChanged(index(events->size() - to - 1, 0), index(events->size() - from - 1, 0));
    });
    connect(events, &EventStore::beginInsertRows, this, [this](int from, int to) {
        int first = events->size() - to;
        int last  = events->size() - from;
        if (from >= events->size()) {
            int batch_size = to - from;
            first += batch_size;
            last += batch_size;
        } else {
            first -= 1;
            last -= 1;
        }
        nhlog::ui()->debug("begin insert from {} to {}", first, last);
        beginInsertRows(QModelIndex(), first, last);
    });
    connect(events, &EventStore::endInsertRows, this, [this]() { endInsertRows(); });
    connect(events, &EventStore::beginResetModel, this, [this]() { beginResetModel(); });
    connect(events, &EventStore::endResetModel, this, [this]() { endResetModel(); });
    connect(events, &EventStore::fetchedMore, this, [this]() { setPaginationInProgress(false); });
    // connect(events,
    //         &EventStore::startDMVerification,
    //         this,
    //         [this](const mtx::events::RoomEvent<mtx::events::msg::KeyVerificationRequest> &msg) {
    //             ChatPage::instance()->receivedRoomDeviceVerificationRequest(msg, this);
    //         });
    // When a message is sent, check if the current edit/reply relates to that message,
    // and update the event_id so that it points to the sent message and not the pending one.
    connect(
      events,
      &EventStore::messageSent,
      this,
      [this](const std::string &txn_id, const std::string &event_id) {
          if (edit_.toStdString() == txn_id) {
              edit_ = QString::fromStdString(event_id);
              emit editChanged(edit_);
          }
          if (reply_.toStdString() == txn_id) {
              reply_ = QString::fromStdString(event_id);
              emit replyChanged(reply_);
          }
      },
      Qt::QueuedConnection);

    connect(this, &TimelineModel::encryptionChanged, this, &TimelineModel::trustlevelChanged);
    connect(this, &TimelineModel::roomMemberCountChanged, this, &TimelineModel::trustlevelChanged);
    // connect(
    //   cache::client(), &Cache::verificationStatusChanged, this, &TimelineModel::trustlevelChanged);
    connect(_timeline, &Timeline::typingUsersChanged, this, &TimelineModel::updateTypingUsers);
    showEventTimer.callOnTimeout(this, &TimelineModel::scrollTimerEvent);

    // connect(this, &TimelineModel::newState, this, [this](mtx::responses::StateEvents events_) {
    //     cache::client()->updateState(room_id_.toStdString(), events_);
    //     this->syncState({std::move(events_.events)});
    // });
}

QHash<int, QByteArray>
TimelineModel::roleNames() const
{
    static QHash<int, QByteArray> roles{
      {Type, "type"},
      {TypeString, "typeString"},
      {IsOnlyEmoji, "isOnlyEmoji"},
      {Body, "body"},
      {FormattedBody, "formattedBody"},
      {PreviousMessageUserId, "previousMessageUserId"},
      {IsSender, "isSender"},
      {UserId, "userId"},
      {UserName, "userName"},
      {PreviousMessageDay, "previousMessageDay"},
      {PreviousMessageIsStateEvent, "previousMessageIsStateEvent"},
      {Day, "day"},
      {Timestamp, "timestamp"},
      {Url, "url"},
      {ThumbnailUrl, "thumbnailUrl"},
      {Duration, "duration"},
      {Blurhash, "blurhash"},
      {Filename, "filename"},
      {Filesize, "filesize"},
      {MimeType, "mimetype"},
      {OriginalHeight, "originalHeight"},
      {OriginalWidth, "originalWidth"},
      {ProportionalHeight, "proportionalHeight"},
      {EventId, "eventId"},
      {State, "status"},
      {IsEdited, "isEdited"},
      {IsEditable, "isEditable"},
      {IsEncrypted, "isEncrypted"},
      {IsStateEvent, "isStateEvent"},
      {Trustlevel, "trustlevel"},
      {EncryptionError, "encryptionError"},
      {ReplyTo, "replyTo"},
      {Reactions, "reactions"},
      {RoomId, "roomId"},
      {RoomName, "roomName"},
      {RoomTopic, "roomTopic"},
      {CallType, "callType"},
      {Dump, "dump"},
      {RelatedEventCacheBuster, "relatedEventCacheBuster"},
      {GeoUri, "geoUri"},
    };

    return roles;
}
int
TimelineModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return this->events->size();
}

QVariantMap
TimelineModel::getDump(const QString &eventId, const QString &relatedTo) const
{
    if (auto event = events->get(eventId.toStdString(), relatedTo.toStdString()))
        return data(*event, Dump).toMap();
    return {};
}

QVariant
TimelineModel::data(const mtx::events::collections::TimelineEvents &event, int role) const
{
    using namespace mtx::accessors;
    namespace acc = mtx::accessors;

    switch (role) {
    case IsSender:
        return {acc::sender(event) == http::client()->user_id().to_string()};
    case UserId:
        return QVariant(QString::fromStdString(acc::sender(event)));
    case UserName:
        return QVariant(displayName(QString::fromStdString(acc::sender(event))));

    case Day: {
        QDateTime prevDate = origin_server_ts(event);
        prevDate.setTime(QTime());
        return {prevDate.toMSecsSinceEpoch()};
    }
    case Timestamp:
        return QVariant(origin_server_ts(event));
    case Type:
        return {qml_mtx_events::toRoomEventType(event)};
    case TypeString:
        return QVariant(qml_mtx_events::toRoomEventTypeString(event));
    case IsOnlyEmoji: {
        QString qBody = QString::fromStdString(body(event));

        QVector<uint> utf32_string = qBody.toUcs4();
        int emojiCount             = 0;

        for (auto &code : utf32_string) {
            if (utils::codepointIsEmoji(code)) {
                emojiCount++;
            } else {
                return {0};
            }
        }

        return {emojiCount};
    }
    case Body:
        return QVariant(utils::replaceEmoji(QString::fromStdString(body(event)).toHtmlEscaped()));
    case FormattedBody: {
        const static QRegularExpression replyFallback(
          QStringLiteral("<mx-reply>.*</mx-reply>"),
          QRegularExpression::DotMatchesEverythingOption);

        auto ascent = QFontMetrics(QFont("default")).ascent();

        bool isReply = utils::isReply(event);

        auto formattedBody_ = QString::fromStdString(formatted_body(event));
        if (formattedBody_.isEmpty()) {
            auto body_ = QString::fromStdString(body(event));

            if (isReply) {
                while (body_.startsWith(QLatin1String("> ")))
                    body_ = body_.right(body_.size() - body_.indexOf('\n') - 1);
                if (body_.startsWith('\n'))
                    body_ = body_.right(body_.size() - 1);
            }
            formattedBody_ = body_.toHtmlEscaped().replace('\n', QLatin1String("<br>"));
        } else {
            if (isReply)
                formattedBody_ = formattedBody_.remove(replyFallback);
        }
        formattedBody_ = utils::escapeBlacklistedHtml(formattedBody_);

        // TODO(Nico): Don't parse html with a regex
        const static QRegularExpression matchIsImg(QStringLiteral("<img [^>]+>"));
        auto itIsImg = matchIsImg.globalMatch(formattedBody_);
        while (itIsImg.hasNext()) {
            // The current <img> tag.
            const QString curImg = itIsImg.next().captured(0);
            // The replacement for the current <img>.
            auto imgReplacement = curImg;

            // Construct image parameters later used by MxcImageProvider.
            QString imgParams;
            if (curImg.contains(QLatin1String("height"))) {
                const static QRegularExpression matchImgHeight(
                  QStringLiteral("height=([\"\']?)(\\d+)([\"\']?)"));
                // Make emoticons twice as high as the font.
                if (curImg.contains(QLatin1String("data-mx-emoticon"))) {
                    imgReplacement =
                      imgReplacement.replace(matchImgHeight, "height=\\1%1\\3").arg(ascent * 2);
                }
                const auto height = matchImgHeight.match(imgReplacement).captured(2).toInt();
                imgParams         = QStringLiteral("?scale&height=%1").arg(height);
            }

            // Replace src in current <img>.
            const static QRegularExpression matchImgUri(QStringLiteral("src=\"mxc://([^\"]*)\""));
            imgReplacement.replace(matchImgUri,
                                   QStringLiteral(R"(src="image://mxcImage/\1%1")").arg(imgParams));
            // Same regex but for single quotes around the src
            const static QRegularExpression matchImgUri2(QStringLiteral("src=\'mxc://([^\']*)\'"));
            imgReplacement.replace(matchImgUri2,
                                   QStringLiteral("src=\'image://mxcImage/\\1%1\'").arg(imgParams));

            // Replace <img> in formattedBody_ with our new <img>.
            formattedBody_.replace(curImg, imgReplacement);
        }

        return QVariant(utils::replaceEmoji(utils::linkifyMessage(formattedBody_)));
    }
    case Url:
        return QVariant(QString::fromStdString(url(event)));
    case ThumbnailUrl:
        return QVariant(QString::fromStdString(thumbnail_url(event)));
    case Duration:
        return QVariant(static_cast<qulonglong>(duration(event)));
    case Blurhash:
        return QVariant(QString::fromStdString(blurhash(event)));
    case Filename:
        return QVariant(QString::fromStdString(filename(event)));
    case Filesize:
        return QVariant(utils::humanReadableFileSize(filesize(event)));
    case MimeType:
        return QVariant(QString::fromStdString(mimetype(event)));
    case OriginalHeight:
        return QVariant(qulonglong{media_height(event)});
    case OriginalWidth:
        return QVariant(qulonglong{media_width(event)});
    case ProportionalHeight: {
        auto w = media_width(event);
        if (w == 0)
            w = 1;

        double prop = media_height(event) / (double)w;

        return {prop > 0 ? prop : 1.};
    }
    case EventId: {
        if (auto replaces = relations(event).replaces())
            return QVariant(QString::fromStdString(replaces.value()));
        else
            return QVariant(QString::fromStdString(event_id(event)));
    }
    case State: {
        auto id             = QString::fromStdString(event_id(event));
        auto containsOthers = [](const auto &vec) {
            for (const auto &e : vec)
                if (e.second != http::client()->user_id().to_string())
                    return true;
            return false;
        };

        // only show read receipts for messages not from us
        if (acc::sender(event) != http::client()->user_id().to_string())
            return qml_mtx_events::Empty;
        else if (!id.isEmpty() && id[0] == 'm')
            return qml_mtx_events::Sent;
        else if (read.contains(id) || containsOthers(cache::readReceipts(id, room_id_)))
            return qml_mtx_events::Read;
        else
            return qml_mtx_events::Received;
    }
    case IsEdited:
        return {relations(event).replaces().has_value()};
    case IsEditable:
        return {!is_state_event(event) &&
                mtx::accessors::sender(event) == http::client()->user_id().to_string()};
    case IsEncrypted: {
        auto encrypted_event = events->get(event_id(event), "", false);
        return encrypted_event &&
               std::holds_alternative<mtx::events::EncryptedEvent<mtx::events::msg::Encrypted>>(
                 *encrypted_event);
    }
    case IsStateEvent: {
        return is_state_event(event);
    }

    case Trustlevel: {
        auto encrypted_event = events->get(event_id(event), "", false);
        if (encrypted_event) {
            if (auto encrypted =
                  std::get_if<mtx::events::EncryptedEvent<mtx::events::msg::Encrypted>>(
                    &*encrypted_event)) {
                return olm::calculate_trust(
                  encrypted->sender,
                  MegolmSessionIndex(room_id_.toStdString(), encrypted->content));
            }
        }
        return crypto::Trust::Unverified;
    }

    case EncryptionError:
        return events->decryptionError(event_id(event));

    case ReplyTo:
        return QVariant(QString::fromStdString(relations(event).reply_to().value_or("")));
    case Reactions: {
        auto id = relations(event).replaces().value_or(event_id(event));
        return QVariant::fromValue(events->reactions(id));
    }
    case RoomId:
        return QVariant(room_id_);
    case RoomName:
        return QVariant(
          utils::replaceEmoji(QString::fromStdString(room_name(event)).toHtmlEscaped()));
    case RoomTopic:
        return QVariant(utils::replaceEmoji(
          utils::linkifyMessage(QString::fromStdString(room_topic(event))
                                  .toHtmlEscaped()
                                  .replace(QLatin1String("\n"), QLatin1String("<br>")))));
    case CallType:
        return QVariant(QString::fromStdString(call_type(event)));
    case Dump: {
        QVariantMap m;
        auto names = roleNames();

        m.insert(names[Type], data(event, static_cast<int>(Type)));
        m.insert(names[TypeString], data(event, static_cast<int>(TypeString)));
        m.insert(names[IsOnlyEmoji], data(event, static_cast<int>(IsOnlyEmoji)));
        m.insert(names[Body], data(event, static_cast<int>(Body)));
        m.insert(names[FormattedBody], data(event, static_cast<int>(FormattedBody)));
        m.insert(names[IsSender], data(event, static_cast<int>(IsSender)));
        m.insert(names[UserId], data(event, static_cast<int>(UserId)));
        m.insert(names[UserName], data(event, static_cast<int>(UserName)));
        m.insert(names[Day], data(event, static_cast<int>(Day)));
        m.insert(names[Timestamp], data(event, static_cast<int>(Timestamp)));
        m.insert(names[Url], data(event, static_cast<int>(Url)));
        m.insert(names[ThumbnailUrl], data(event, static_cast<int>(ThumbnailUrl)));
        m.insert(names[Duration], data(event, static_cast<int>(Duration)));
        m.insert(names[Blurhash], data(event, static_cast<int>(Blurhash)));
        m.insert(names[Filename], data(event, static_cast<int>(Filename)));
        m.insert(names[Filesize], data(event, static_cast<int>(Filesize)));
        m.insert(names[MimeType], data(event, static_cast<int>(MimeType)));
        m.insert(names[OriginalHeight], data(event, static_cast<int>(OriginalHeight)));
        m.insert(names[OriginalWidth], data(event, static_cast<int>(OriginalWidth)));
        m.insert(names[ProportionalHeight], data(event, static_cast<int>(ProportionalHeight)));
        m.insert(names[EventId], data(event, static_cast<int>(EventId)));
        m.insert(names[State], data(event, static_cast<int>(State)));
        m.insert(names[IsEdited], data(event, static_cast<int>(IsEdited)));
        m.insert(names[IsEditable], data(event, static_cast<int>(IsEditable)));
        m.insert(names[IsEncrypted], data(event, static_cast<int>(IsEncrypted)));
        m.insert(names[IsStateEvent], data(event, static_cast<int>(IsStateEvent)));
        m.insert(names[ReplyTo], data(event, static_cast<int>(ReplyTo)));
        m.insert(names[RoomName], data(event, static_cast<int>(RoomName)));
        m.insert(names[RoomTopic], data(event, static_cast<int>(RoomTopic)));
        m.insert(names[CallType], data(event, static_cast<int>(CallType)));
        m.insert(names[EncryptionError], data(event, static_cast<int>(EncryptionError)));
        m.insert(names[GeoUri], data(event, static_cast<int>(GeoUri)));
        return QVariant(m);
    }
    case RelatedEventCacheBuster:
        return relatedEventCacheBuster;
    case GeoUri:
        return QVariant(QString::fromStdString(geoUri(event)));
    default:
        return {};
    }
}

QVariant
TimelineModel::data(const QModelIndex &index, int role) const
{
    using namespace mtx::accessors;
    namespace acc = mtx::accessors;
    if (index.row() < 0 && index.row() >= rowCount())
        return {};

    // HACK(Nico): fetchMore likes to break with dynamically sized delegates and reuseItems
    if (index.row() + 1 == rowCount() && !m_paginationInProgress)
        const_cast<TimelineModel *>(this)->fetchMore(index);

    auto event = events->get(rowCount() - index.row() - 1);

    if (!event)
        return "";

    if (role == PreviousMessageDay || role == PreviousMessageUserId ||
        role == PreviousMessageIsStateEvent) {
        int prevIdx = rowCount() - index.row() - 2;
        if (prevIdx < 0)
            return {};
        auto tempEv = events->get(prevIdx);
        if (!tempEv)
            return {};
        if (role == PreviousMessageUserId)
            return data(*tempEv, UserId);
        else if (role == PreviousMessageDay)
            return data(*tempEv, Day);
        else
            return data(*tempEv, IsStateEvent);
    }

    return data(*event, role);
}

QVariant
TimelineModel::dataById(const QString &id, int role, const QString &relatedTo)
{
    if (auto event = events->get(id.toStdString(), relatedTo.toStdString()))
        return data(*event, role);
    return {};
}

bool
TimelineModel::canFetchMore(const QModelIndex &) const
{
    if (!events->size())
        return true;
    if (auto first = events->get(0);
        first &&
        !std::holds_alternative<mtx::events::StateEvent<mtx::events::state::Create>>(*first))
        return true;
    else

        return false;
}

void
TimelineModel::setPaginationInProgress(const bool paginationInProgress)
{
    if (m_paginationInProgress == paginationInProgress) {
        return;
    }

    m_paginationInProgress = paginationInProgress;
    emit paginationInProgressChanged(m_paginationInProgress);
}

void
TimelineModel::fetchMore(const QModelIndex &)
{
    if (m_paginationInProgress) {
        nhlog::ui()->warn("Already loading older messages");
        return;
    }

    setPaginationInProgress(true);

    events->fetchMore();
}

void TimelineModel::updateLastMessage() {
    _timeline->updateLastMessage();
}

void
TimelineModel::setCurrentIndex(int index)
{
    auto oldIndex = idToIndex(currentId);
    currentId     = indexToId(index);
    if (index != oldIndex)
        emit currentIndexChanged(index);

    // if (MainWindow::instance() != QGuiApplication::focusWindow())
    //     return;

    if (!currentId.startsWith('m')) {
        auto oldReadIndex =
          cache::getEventIndex(roomId().toStdString(), currentReadId.toStdString());
        auto nextEventIndexAndId =
          cache::lastInvisibleEventAfter(roomId().toStdString(), currentId.toStdString());

        if (nextEventIndexAndId && (!oldReadIndex || *oldReadIndex < nextEventIndexAndId->first)) {
            readEvent(nextEventIndexAndId->second);
            currentReadId = QString::fromStdString(nextEventIndexAndId->second);
        }
    }
}

void
TimelineModel::readEvent(const std::string &id)
{
    http::client()->read_event(
      room_id_.toStdString(),
      id,
      [this](mtx::http::RequestErr err) {
          if (err) {
              nhlog::net()->warn(
                "failed to read_event ({}, {})", room_id_.toStdString(), currentId.toStdString());
          }
      }, false);
    //   !UserSettings::instance()->readReceipts());
}

QString TimelineModel::displayName(const QString &id) const {
    return _timeline->displayName(id);
}

QString TimelineModel::avatarUrl(const QString &id) const {
    return _timeline->avatarUrl(id);
}

QString
TimelineModel::formatDateSeparator(QDate date) const
{
    auto now = QDateTime::currentDateTime();

    QString fmt = QLocale::system().dateFormat(QLocale::LongFormat);

    if (now.date().year() == date.year()) {
        QRegularExpression rx(QStringLiteral("[^a-zA-Z]*y+[^a-zA-Z]*"));
        fmt = fmt.remove(rx);
    }

    return date.toString(fmt);
}

void TimelineModel::viewRawMessage(const QString &id) {
    auto e = _timeline->viewRawMessage(id);
    if (e.isEmpty())
        return;
    emit showRawMessageDialog(e);
}

void TimelineModel::forwardMessage(const QString &eventId, QString roomId) {
    _timeline->forwardMessage(eventId, roomId);
}

void TimelineModel::viewDecryptedRawMessage(const QString &id) {
    auto e = _timeline->viewDecryptedRawMessage(id);
    if (e.isEmpty())
        return;
    emit showRawMessageDialog(e);
}

void
TimelineModel::openUserProfile(QString userid)
{
    UserProfile *userProfile = new UserProfile(room_id_, std::move(userid),this);
    connect(this, &TimelineModel::roomAvatarUrlChanged, userProfile, &UserProfile::updateAvatarUrl);
    emit openProfile(userProfile);
}

void
TimelineModel::replyAction(const QString &id)
{
    setReply(id);
}

void TimelineModel::unpin(const QString &id) {
    _timeline->unpin(id);
}

void TimelineModel::pin(const QString &id) {
    _timeline->pin(id);
}

void
TimelineModel::editAction(QString id)
{
    setEdit(id);
}

RelatedInfo
TimelineModel::relatedInfo(const QString &id)
{
    auto event = events->get(id.toStdString(), "");
    if (!event)
        return {};

    return utils::stripReplyFallbacks(*event, id.toStdString(), room_id_);
}

void
TimelineModel::showReadReceipts(QString id)
{
    emit openReadReceiptsDialog(new ReadReceiptsProxy{id, roomId(), this});
}

void
TimelineModel::redactAllFromUser(const QString &userid, const QString &reason)
{
    auto user = userid.toStdString();
    std::vector<QString> toRedact;
    for (auto it = events->size() - 1; it >= 0; --it) {
        auto event = events->get(it, false);
        if (event && mtx::accessors::sender(*event) == user &&
            !std::holds_alternative<mtx::events::RoomEvent<mtx::events::msg::Redacted>>(*event)) {
            toRedact.push_back(QString::fromStdString(mtx::accessors::event_id(*event)));
        }
    }

    for (const auto &e : toRedact) {
        redactEvent(e, reason);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void
TimelineModel::redactEvent(const QString &id, const QString &reason)
{
    if (!id.isEmpty()) {
        auto edits = events->edits(id.toStdString());
        http::client()->redact_event(
          room_id_.toStdString(),
          id.toStdString(),
          [this, id](const mtx::responses::EventId &, mtx::http::RequestErr err) {
              if (err) {
                  emit redactionFailed(tr("Message redaction failed: %1")
                                         .arg(QString::fromStdString(err->matrix_error.error)));
                  return;
              }

              emit dataAtIdChanged(id);
          },
          reason.toStdString());

        // redact all edits to prevent leaks
        for (const auto &e : edits) {
            const auto &id_ = mtx::accessors::event_id(e);
            http::client()->redact_event(
              room_id_.toStdString(),
              id_,
              [this, id, id_](const mtx::responses::EventId &, mtx::http::RequestErr err) {
                  if (err) {
                      emit redactionFailed(tr("Message redaction failed: %1")
                                             .arg(QString::fromStdString(err->matrix_error.error)));
                      return;
                  }

                  emit dataAtIdChanged(id);
              },
              reason.toStdString());
        }
    }
}

int
TimelineModel::idToIndex(const QString &id) const
{
    if (id.isEmpty())
        return -1;

    auto idx = events->idToIndex(id.toStdString());
    if (idx)
        return events->size() - *idx - 1;
    else
        return -1;
}

QString
TimelineModel::indexToId(int index) const
{
    auto id = events->indexToId(events->size() - index - 1);
    return id ? QString::fromStdString(*id) : QLatin1String("");
}


void
TimelineModel::openMedia(const QString &eventId)
{
    cacheMedia(eventId, [](const QString &filename) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(filename));
    });
}

bool
TimelineModel::saveMedia(const QString &eventId) const
{
    mtx::events::collections::TimelineEvents *event = events->get(eventId.toStdString(), "");
    if (!event)
        return false;

    QString mxcUrl           = QString::fromStdString(mtx::accessors::url(*event));
    QString originalFilename = QString::fromStdString(mtx::accessors::filename(*event));
    QString mimeType         = QString::fromStdString(mtx::accessors::mimetype(*event));

    auto encryptionInfo = mtx::accessors::file(*event);

    qml_mtx_events::EventType eventType = qml_mtx_events::toRoomEventType(*event);

    QString dialogTitle;
    if (eventType == qml_mtx_events::EventType::ImageMessage) {
        dialogTitle = tr("Save image");
    } else if (eventType == qml_mtx_events::EventType::VideoMessage) {
        dialogTitle = tr("Save video");
    } else if (eventType == qml_mtx_events::EventType::AudioMessage) {
        dialogTitle = tr("Save audio");
    } else {
        dialogTitle = tr("Save file");
    }

    const QString filterString = QMimeDatabase().mimeTypeForName(mimeType).filterString();
    const QString downloadsFolder =
      QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    const QString openLocation = downloadsFolder + "/" + originalFilename;

    const auto filename = GlobalObject::getSaveFileName(dialogTitle, openLocation, originalFilename, filterString);

    if (filename.isEmpty())
        return false;

    const auto url = mxcUrl.toStdString();

    http::client()->download(url,
                             [filename, url, encryptionInfo](const std::string &data,
                                                             const std::string &,
                                                             const std::string &,
                                                             mtx::http::RequestErr err) {
                                 if (err) {
                                     nhlog::net()->warn("failed to retrieve image {}: {} {}",
                                                        url,
                                                        err->matrix_error.error,
                                                        static_cast<int>(err->status_code));
                                     return;
                                 }

                                 try {
                                     auto temp = data;
                                     if (encryptionInfo)
                                         temp = mtx::crypto::to_string(
                                           mtx::crypto::decrypt_file(temp, encryptionInfo.value()));

                                     QFile file(filename);

                                     if (!file.open(QIODevice::WriteOnly))
                                         return;

                                     file.write(QByteArray(temp.data(), (int)temp.size()));
                                     file.close();

                                     return;
                                 } catch (const std::exception &e) {
                                     nhlog::ui()->warn("Error while saving file to: {}", e.what());
                                 }
                             });
    return true;
}

void
TimelineModel::cacheMedia(const QString &eventId,
                          const std::function<void(const QString)> &callback)
{
    mtx::events::collections::TimelineEvents *event = events->get(eventId.toStdString(), "");
    if (!event)
        return;

    QString mxcUrl   = QString::fromStdString(mtx::accessors::url(*event));
    QString mimeType = QString::fromStdString(mtx::accessors::mimetype(*event));

    auto encryptionInfo = mtx::accessors::file(*event);

    // If the message is a link to a non mxcUrl, don't download it
    if (!mxcUrl.startsWith(QLatin1String("mxc://"))) {
        emit mediaCached(mxcUrl, mxcUrl);
        return;
    }

    QString suffix = QMimeDatabase().mimeTypeForName(mimeType).preferredSuffix();

    const auto url  = mxcUrl.toStdString();
    const auto name = QString(mxcUrl).remove(QStringLiteral("mxc://"));
    QFileInfo filename(
      QStringLiteral("%1/%2.%3")
        .arg(GlobalObject::instance()->mediaCachePath(), name, suffix));
    if (QDir::cleanPath(name) != name) {
        nhlog::net()->warn("mxcUrl '{}' is not safe, not downloading file", url);
        return;
    }

    QDir().mkpath(filename.path());

    if (filename.isReadable()) {
#if defined(Q_OS_WIN)
        emit mediaCached(mxcUrl, filename.filePath());
#else
        emit mediaCached(mxcUrl, "file://" + filename.filePath());
#endif
        if (callback) {
            callback(filename.filePath());
        }
        return;
    }

    http::client()->download(
      url,
      [this, callback, mxcUrl, filename, url, encryptionInfo](const std::string &data,
                                                              const std::string &,
                                                              const std::string &,
                                                              mtx::http::RequestErr err) {
          if (err) {
              nhlog::net()->warn("failed to retrieve image {}: {} {}",
                                 url,
                                 err->matrix_error.error,
                                 static_cast<int>(err->status_code));
              return;
          }

          try {
              auto temp = data;
              if (encryptionInfo)
                  temp =
                    mtx::crypto::to_string(mtx::crypto::decrypt_file(temp, encryptionInfo.value()));

              QFile file(filename.filePath());

              if (!file.open(QIODevice::WriteOnly))
                  return;

              file.write(QByteArray(temp.data(), (int)temp.size()));
              file.close();

              if (callback) {
                  callback(filename.filePath());
              }
          } catch (const std::exception &e) {
              nhlog::ui()->warn("Error while saving file to: {}", e.what());
          }

#if defined(Q_OS_WIN)
          emit mediaCached(mxcUrl, filename.filePath());
#else
          emit mediaCached(mxcUrl, "file://" + filename.filePath());
#endif
      });
}

void
TimelineModel::cacheMedia(const QString &eventId)
{
    cacheMedia(eventId, nullptr);
}

void
TimelineModel::showEvent(QString eventId)
{
    using namespace std::chrono_literals;
    // Direct to eventId
    if (eventId[0] == '$') {
        int idx = idToIndex(eventId);
        if (idx == -1) {
            nhlog::ui()->warn("Scrolling to event id {}, failed - no known index",
                              eventId.toStdString());
            return;
        }
        eventIdToShow = eventId;
        emit scrollTargetChanged();
        showEventTimer.start(50ms);
        return;
    }
    // to message index
    eventId       = indexToId(eventId.toInt());
    eventIdToShow = eventId;
    emit scrollTargetChanged();
    showEventTimer.start(50ms);
    return;
}

void
TimelineModel::eventShown()
{
    eventIdToShow.clear();
    emit scrollTargetChanged();
}

QString
TimelineModel::scrollTarget() const
{
    return eventIdToShow;
}

void
TimelineModel::scrollTimerEvent()
{
    if (eventIdToShow.isEmpty() || showEventTimerCounter > 3) {
        showEventTimer.stop();
        showEventTimerCounter = 0;
    } else {
        emit scrollToIndex(idToIndex(eventIdToShow));
        showEventTimerCounter++;
    }
}

void
TimelineModel::requestKeyForEvent(const QString &id)
{
    auto encrypted_event = events->get(id.toStdString(), "", false);
    if (encrypted_event) {
        if (auto ev = std::get_if<mtx::events::EncryptedEvent<mtx::events::msg::Encrypted>>(
              encrypted_event))
            events->requestSession(*ev, true);
    }
}

QString
TimelineModel::getBareRoomLink(const QString &roomId)
{
    auto alias = _timeline->getRoomAliases();
    QString room;
    if (alias) {
        room = QString::fromStdString(alias->alias);
        if (room.isEmpty() && !alias->alt_aliases.empty()) {
            room = QString::fromStdString(alias->alt_aliases.front());
        }
    }

    if (room.isEmpty())
        room = roomId;

    return QStringLiteral("https://matrix.to/#/%1").arg(QString(QUrl::toPercentEncoding(room)));
}

QString
TimelineModel::getRoomVias(const QString &roomId)
{
    QStringList vias;

    for (const auto &m : utils::roomVias(roomId.toStdString())) {
        if (vias.size() >= 4)
            break;

        QString server =
          QStringLiteral("via=%1").arg(QString(QUrl::toPercentEncoding(QString::fromStdString(m))));

        if (!vias.contains(server))
            vias.push_back(server);
    }

    return vias.join("&");
}

void
TimelineModel::copyLinkToEvent(const QString &eventId)
{
    auto link = QStringLiteral("%1/%2?%3")
                  .arg(getBareRoomLink(room_id_),
                       QString(QUrl::toPercentEncoding(eventId)),
                       getRoomVias(room_id_));
    QGuiApplication::clipboard()->setText(link);
}

QString
TimelineModel::formatTypingUsers(const QStringList &users, const QColor &bg)
{
    QString temp = (users.size() > 1)?"%1 and %2 are typing.":"%1%2 is typing.";

    if (users.empty()) {
        return {};
    }

    QStringList uidWithoutLast;

    auto formatUser = [this, bg](const QString &user_id) -> QString {
        auto uncoloredUsername = utils::replaceEmoji(displayName(user_id));
        QString prefix = 
          QStringLiteral("<font color=\"%1\">").arg(userColor(user_id, bg).name());

        // color only parts that don't have a font already specified
        QString coloredUsername;
        int index = 0;
        do {
            auto startIndex = uncoloredUsername.indexOf(QLatin1String("<font"), index);

            if (startIndex - index != 0)
                coloredUsername +=
                  prefix + uncoloredUsername.mid(index, startIndex > 0 ? startIndex - index : -1) +
                  QStringLiteral("</font>");

            auto endIndex = uncoloredUsername.indexOf(QLatin1String("</font>"), startIndex);
            if (endIndex > 0)
                endIndex += sizeof("</font>") - 1;

            if (endIndex - startIndex != 0)
                coloredUsername +=
                  QStringView(uncoloredUsername).mid(startIndex, endIndex - startIndex);

            index = endIndex;
        } while (index > 0 && index < uncoloredUsername.size());

        return coloredUsername;
    };

    uidWithoutLast.reserve(static_cast<int>(users.size()));
    for (size_t i = 0; i + 1 < (unsigned int)users.size(); i++) {
        uidWithoutLast.append(formatUser(users[i]));
    }
    
    return temp.arg(uidWithoutLast.join(QStringLiteral(", ")), formatUser(users.back()));
}

QString
TimelineModel::formatJoinRuleEvent(const QString &id)
{
    mtx::events::collections::TimelineEvents *e = events->get(id.toStdString(), "");
    if (!e)
        return {};

    auto event = std::get_if<mtx::events::StateEvent<mtx::events::state::JoinRules>>(e);
    if (!event)
        return {};

    QString user = QString::fromStdString(event->sender);
    QString name = utils::replaceEmoji(displayName(user));

    switch (event->content.join_rule) {
    case mtx::events::state::JoinRule::Public:
        return tr("%1 opened the room to the public.").arg(name);
    case mtx::events::state::JoinRule::Invite:
        return tr("%1 made this room require and invitation to join.").arg(name);
    case mtx::events::state::JoinRule::Knock:
        return tr("%1 allowed to join this room by knocking.").arg(name);
    case mtx::events::state::JoinRule::Restricted: {
        QStringList rooms;
        for (const auto &r : event->content.allow) {
            if (r.type == mtx::events::state::JoinAllowanceType::RoomMembership)
                rooms.push_back(QString::fromStdString(r.room_id));
        }
        return tr("%1 allowed members of the following rooms to automatically join this "
                  "room: %2")
          .arg(name, rooms.join(QStringLiteral(", ")));
    }
    default:
        // Currently, knock and private are reserved keywords and not implemented in Matrix.
        return {};
    }
}

QString
TimelineModel::formatGuestAccessEvent(const QString &id)
{
    mtx::events::collections::TimelineEvents *e = events->get(id.toStdString(), "");
    if (!e)
        return {};

    auto event = std::get_if<mtx::events::StateEvent<mtx::events::state::GuestAccess>>(e);
    if (!event)
        return {};

    QString user = QString::fromStdString(event->sender);
    QString name = utils::replaceEmoji(displayName(user));

    switch (event->content.guest_access) {
    case mtx::events::state::AccessState::CanJoin:
        return tr("%1 made the room open to guests.").arg(name);
    case mtx::events::state::AccessState::Forbidden:
        return tr("%1 has closed the room to guest access.").arg(name);
    default:
        return {};
    }
}

QString
TimelineModel::formatHistoryVisibilityEvent(const QString &id)
{
    mtx::events::collections::TimelineEvents *e = events->get(id.toStdString(), "");
    if (!e)
        return {};

    auto event = std::get_if<mtx::events::StateEvent<mtx::events::state::HistoryVisibility>>(e);

    if (!event)
        return {};

    QString user = QString::fromStdString(event->sender);
    QString name = utils::replaceEmoji(displayName(user));

    switch (event->content.history_visibility) {
    case mtx::events::state::Visibility::WorldReadable:
        return tr("%1 made the room history world readable. Events may be now read by "
                  "non-joined people.")
          .arg(name);
    case mtx::events::state::Visibility::Shared:
        return tr("%1 set the room history visible to members from this point on.").arg(name);
    case mtx::events::state::Visibility::Invited:
        return tr("%1 set the room history visible to members since they were invited.").arg(name);
    case mtx::events::state::Visibility::Joined:
        return tr("%1 set the room history visible to members since they joined the room.")
          .arg(name);
    default:
        return {};
    }
}

QString
TimelineModel::formatPowerLevelEvent(const QString &id)
{
    mtx::events::collections::TimelineEvents *e = events->get(id.toStdString(), "");
    if (!e)
        return {};

    auto event = std::get_if<mtx::events::StateEvent<mtx::events::state::PowerLevels>>(e);
    if (!event)
        return QString();

    mtx::events::StateEvent<mtx::events::state::PowerLevels> *prevEvent = nullptr;
    if (!event->unsigned_data.replaces_state.empty()) {
        auto tempPrevEvent = events->get(event->unsigned_data.replaces_state, event->event_id);
        if (tempPrevEvent) {
            prevEvent =
              std::get_if<mtx::events::StateEvent<mtx::events::state::PowerLevels>>(tempPrevEvent);
        }
    }

    QString user        = QString::fromStdString(event->sender);
    QString sender_name = utils::replaceEmoji(displayName(user));
    // Get the rooms levels for redactions and powerlevel changes to determine "Administrator" and
    // "Moderator" powerlevels.
    auto administrator_power_level = event->content.state_level("m.room.power_levels");
    auto moderator_power_level     = event->content.redact;
    auto default_powerlevel        = event->content.users_default;
    if (!prevEvent)
        return tr("%1 has changed the room's permissions.").arg(sender_name);

    auto calc_affected = [&event,
                          &prevEvent](int64_t newPowerlevelSetting) -> std::pair<QStringList, int> {
        QStringList affected{};
        auto numberOfAffected = 0;
        // We do only compare to people with explicit PL. Usually others are not going to be
        // affected either way and this is cheaper to iterate over.
        for (auto const &[mxid, currentPowerlevel] : event->content.users) {
            if (currentPowerlevel == newPowerlevelSetting &&
                prevEvent->content.user_level(mxid) < newPowerlevelSetting) {
                numberOfAffected++;
                if (numberOfAffected <= 2) {
                    affected.push_back(QString::fromStdString(mxid));
                }
            }
        }
        return {affected, numberOfAffected};
    };

    QStringList resultingMessage{};
    // These affect only a few people. Therefor we can print who is affected.
    if (event->content.kick != prevEvent->content.kick) {
        auto default_message = tr("%1 has changed the room's kick powerlevel from %2 to %3.")
                                 .arg(sender_name)
                                 .arg(prevEvent->content.kick)
                                 .arg(event->content.kick);

        // We only calculate affected users if we change to a level above the default users PL
        // to not accidentally have a DoS vector
        if (event->content.kick > default_powerlevel) {
            auto [affected, number_of_affected] = calc_affected(event->content.kick);

            if (number_of_affected != 0) {
                auto true_affected_rest = number_of_affected - affected.size();
                if (number_of_affected > 1) {
                    resultingMessage.append(
                      default_message + QStringLiteral(" ") +
                      tr("%n member(s) can now kick room members.", nullptr, true_affected_rest));
                } else if (number_of_affected == 1) {
                    resultingMessage.append(
                      default_message + QStringLiteral(" ") +
                      tr("%1 can now kick room members.")
                        .arg(utils::replaceEmoji(displayName(affected.at(0)))));
                }
            } else {
                resultingMessage.append(default_message);
            }
        } else {
            resultingMessage.append(default_message);
        }
    }

    if (event->content.redact != prevEvent->content.redact) {
        auto default_message = tr("%1 has changed the room's redact powerlevel from %2 to %3.")
                                 .arg(sender_name)
                                 .arg(prevEvent->content.redact)
                                 .arg(event->content.redact);

        // We only calculate affected users if we change to a level above the default users PL
        // to not accidentally have a DoS vector
        if (event->content.redact > default_powerlevel) {
            auto [affected, number_of_affected] = calc_affected(event->content.redact);

            if (number_of_affected != 0) {
                auto true_affected_rest = number_of_affected - affected.size();
                if (number_of_affected > 1) {
                    resultingMessage.append(default_message + QStringLiteral(" ") +
                                            tr("%n member(s) can now redact room messages.",
                                               nullptr,
                                               true_affected_rest));
                } else if (number_of_affected == 1) {
                    resultingMessage.append(
                      default_message + QStringLiteral(" ") +
                      tr("%1 can now redact room messages.")
                        .arg(utils::replaceEmoji(displayName(affected.at(0)))));
                }
            } else {
                resultingMessage.append(default_message);
            }
        } else {
            resultingMessage.append(default_message);
        }
    }

    if (event->content.ban != prevEvent->content.ban) {
        auto default_message = tr("%1 has changed the room's ban powerlevel from %2 to %3.")
                                 .arg(sender_name)
                                 .arg(prevEvent->content.ban)
                                 .arg(event->content.ban);

        // We only calculate affected users if we change to a level above the default users PL
        // to not accidentally have a DoS vector
        if (event->content.ban > default_powerlevel) {
            auto [affected, number_of_affected] = calc_affected(event->content.ban);

            if (number_of_affected != 0) {
                auto true_affected_rest = number_of_affected - affected.size();
                if (number_of_affected > 1) {
                    resultingMessage.append(
                      default_message + QStringLiteral(" ") +
                      tr("%n member(s) can now ban room members.", nullptr, true_affected_rest));
                } else if (number_of_affected == 1) {
                    resultingMessage.append(
                      default_message + QStringLiteral(" ") +
                      tr("%1 can now ban room members.")
                        .arg(utils::replaceEmoji(displayName(affected.at(0)))));
                }
            } else {
                resultingMessage.append(default_message);
            }
        } else {
            resultingMessage.append(default_message);
        }
    }

    if (event->content.state_default != prevEvent->content.state_default) {
        auto default_message =
          tr("%1 has changed the room's state_default powerlevel from %2 to %3.")
            .arg(sender_name)
            .arg(prevEvent->content.state_default)
            .arg(event->content.state_default);

        // We only calculate affected users if we change to a level above the default users PL
        // to not accidentally have a DoS vector
        if (event->content.state_default > default_powerlevel) {
            auto [affected, number_of_affected] = calc_affected(event->content.kick);

            if (number_of_affected != 0) {
                auto true_affected_rest = number_of_affected - affected.size();
                if (number_of_affected > 1) {
                    resultingMessage.append(
                      default_message + QStringLiteral(" ") +
                      tr("%n member(s) can now send state events.", nullptr, true_affected_rest));
                } else if (number_of_affected == 1) {
                    resultingMessage.append(
                      default_message + QStringLiteral(" ") +
                      tr("%1 can now send state events.")
                        .arg(utils::replaceEmoji(displayName(affected.at(0)))));
                }
            } else {
                resultingMessage.append(default_message);
            }
        } else {
            resultingMessage.append(default_message);
        }
    }

    // These affect potentially the whole room. We there for do not calculate who gets affected
    // by this to prevent huge lists of people.
    if (event->content.invite != prevEvent->content.invite) {
        resultingMessage.append(tr("%1 has changed the room's invite powerlevel from %2 to %3.")
                                  .arg(sender_name,
                                       QString::number(prevEvent->content.invite),
                                       QString::number(event->content.invite)));
    }

    if (event->content.events_default != prevEvent->content.events_default) {
        if ((event->content.events_default > default_powerlevel) &&
            prevEvent->content.events_default <= default_powerlevel) {
            resultingMessage.append(
              tr("%1 has changed the room's events_default powerlevel from %2 to %3. New "
                 "users can now not send any events.")
                .arg(sender_name,
                     QString::number(prevEvent->content.events_default),
                     QString::number(event->content.events_default)));
        } else if ((event->content.events_default < prevEvent->content.events_default) &&
                   (event->content.events_default < default_powerlevel) &&
                   (prevEvent->content.events_default > default_powerlevel)) {
            resultingMessage.append(
              tr("%1 has changed the room's events_default powerlevel from %2 to %3. New "
                 "users can now send events that are not otherwise restricted.")
                .arg(sender_name,
                     QString::number(prevEvent->content.events_default),
                     QString::number(event->content.events_default)));
        } else {
            resultingMessage.append(
              tr("%1 has changed the room's events_default powerlevel from %2 to %3.")
                .arg(sender_name,
                     QString::number(prevEvent->content.events_default),
                     QString::number(event->content.events_default)));
        }
    }

    // Compare if a Powerlevel of a user changed
    for (auto const &[mxid, powerlevel] : event->content.users) {
        auto nameOfChangedUser = utils::replaceEmoji(displayName(QString::fromStdString(mxid)));
        if (prevEvent->content.user_level(mxid) != powerlevel) {
            if (powerlevel >= administrator_power_level) {
                resultingMessage.append(tr("%1 has made %2 an administrator of this room.")
                                          .arg(sender_name, nameOfChangedUser));
            } else if (powerlevel >= moderator_power_level &&
                       powerlevel > prevEvent->content.user_level(mxid)) {
                resultingMessage.append(tr("%1 has made %2 a moderator of this room.")
                                          .arg(sender_name, nameOfChangedUser));
            } else if (powerlevel >= moderator_power_level &&
                       powerlevel < prevEvent->content.user_level(mxid)) {
                resultingMessage.append(tr("%1 has downgraded %2 to moderator of this room.")
                                          .arg(sender_name, nameOfChangedUser));
            } else {
                resultingMessage.append(tr("%1 has changed the powerlevel of %2 from %3 to %4.")
                                          .arg(sender_name,
                                               nameOfChangedUser,
                                               QString::number(prevEvent->content.user_level(mxid)),
                                               QString::number(powerlevel)));
            }
        }
    }

    // Handle added/removed/changed event type
    for (auto const &[event_type, powerlevel] : event->content.events) {
        auto prev_not_present =
          prevEvent->content.events.find(event_type) == prevEvent->content.events.end();

        if (prev_not_present || prevEvent->content.events.at(event_type) != powerlevel) {
            if (powerlevel >= administrator_power_level) {
                resultingMessage.append(tr("%1 allowed only administrators to send \"%2\".")
                                          .arg(sender_name, QString::fromStdString(event_type)));
            } else if (powerlevel >= moderator_power_level) {
                resultingMessage.append(tr("%1 allowed only moderators to send \"%2\".")
                                          .arg(sender_name, QString::fromStdString(event_type)));
            } else if (powerlevel == default_powerlevel) {
                resultingMessage.append(tr("%1 allowed everyone to send \"%2\".")
                                          .arg(sender_name, QString::fromStdString(event_type)));
            } else if (prev_not_present) {
                resultingMessage.append(
                  tr("%1 has changed the powerlevel of event type \"%2\" from the default to %3.")
                    .arg(sender_name,
                         QString::fromStdString(event_type),
                         QString::number(powerlevel)));
            } else {
                resultingMessage.append(
                  tr("%1 has changed the powerlevel of event type \"%2\" from %3 to %4.")
                    .arg(sender_name,
                         QString::fromStdString(event_type),
                         QString::number(prevEvent->content.events.at(event_type)),
                         QString::number(powerlevel)));
            }
        }
    }

    if (!resultingMessage.isEmpty()) {
        return resultingMessage.join("<br/>");
    } else {
        return tr("%1 has changed the room's permissions.").arg(sender_name);
    }
}

QString
TimelineModel::formatImagePackEvent(const QString &id)
{
    mtx::events::collections::TimelineEvents *e = events->get(id.toStdString(), "");
    if (!e)
        return {};

    auto event = std::get_if<mtx::events::StateEvent<mtx::events::msc2545::ImagePack>>(e);
    if (!event)
        return {};

    mtx::events::StateEvent<mtx::events::msc2545::ImagePack> *prevEvent = nullptr;
    if (!event->unsigned_data.replaces_state.empty()) {
        auto tempPrevEvent = events->get(event->unsigned_data.replaces_state, event->event_id);
        if (tempPrevEvent) {
            prevEvent =
              std::get_if<mtx::events::StateEvent<mtx::events::msc2545::ImagePack>>(tempPrevEvent);
        }
    }

    const auto &newImages = event->content.images;
    const auto oldImages  = prevEvent ? prevEvent->content.images : decltype(newImages){};

    auto ascent = QFontMetrics(QFont("default")).ascent();

    auto calcChange = [ascent](const std::map<std::string, mtx::events::msc2545::PackImage> &newI,
                               const std::map<std::string, mtx::events::msc2545::PackImage> &oldI) {
        QStringList added;
        for (const auto &[shortcode, img] : newI) {
            if (!oldI.count(shortcode))
                added.push_back(QStringLiteral("<img data-mx-emoticon height=%1 src=\"%2\"> (~%3)")
                                  .arg(ascent)
                                  .arg(QString::fromStdString(img.url)
                                         .replace("mxc://", "image://mxcImage/")
                                         .toHtmlEscaped(),
                                       QString::fromStdString(shortcode)));
        }
        return added;
    };

    auto added   = calcChange(newImages, oldImages);
    auto removed = calcChange(oldImages, newImages);

    auto sender       = utils::replaceEmoji(displayName(QString::fromStdString(event->sender)));
    const auto packId = [&event]() -> QString {
        if (event->content.pack && !event->content.pack->display_name.empty()) {
            return event->content.pack->display_name.c_str();
        } else if (!event->state_key.empty()) {
            return event->state_key.c_str();
        }
        return tr("(empty)");
    }();

    QString msg;

    if (!removed.isEmpty()) {
        msg = tr("%1 removed the following images from the pack %2:<br>%3")
                .arg(sender, packId, removed.join(", "));
    }
    if (!added.isEmpty()) {
        if (!msg.isEmpty())
            msg += "<br>";
        msg += tr("%1 added the following images to the pack %2:<br>%3")
                 .arg(sender, packId, added.join(", "));
    }

    if (msg.isEmpty())
        return tr("%1 changed the sticker and emotes in this room.").arg(sender);
    else
        return msg;
}

QString
TimelineModel::formatPolicyRule(const QString &id)
{
    auto idStr                                  = id.toStdString();
    mtx::events::collections::TimelineEvents *e = events->get(idStr, "");
    if (!e)
        return {};

    auto qsHtml = [](const std::string &s) { return QString::fromStdString(s).toHtmlEscaped(); };
    constexpr std::string_view unstable_ban = "org.matrix.mjolnir.ban";

    if (auto userRule =
          std::get_if<mtx::events::StateEvent<mtx::events::state::policy_rule::UserRule>>(e)) {
        auto sender = utils::replaceEmoji(displayName(QString::fromStdString(userRule->sender)));
        if (userRule->content.entity.empty() ||
            (userRule->content.recommendation !=
               mtx::events::state::policy_rule::recommendation::ban &&
             userRule->content.recommendation != unstable_ban)) {
            while (userRule->content.entity.empty() &&
                   !userRule->unsigned_data.replaces_state.empty()) {
                auto temp = events->get(userRule->unsigned_data.replaces_state, idStr);
                if (!temp)
                    break;
                if (auto tempRule = std::get_if<
                      mtx::events::StateEvent<mtx::events::state::policy_rule::UserRule>>(temp))
                    userRule = tempRule;
                else
                    break;
            }

            return tr("%1 disabled the rule to ban users matching %2.")
              .arg(sender, qsHtml(userRule->content.entity));
        } else {
            return tr("%1 added a rule to ban users matching %2 for '%3'.")
              .arg(sender, qsHtml(userRule->content.entity), qsHtml(userRule->content.reason));
        }
    } else if (auto roomRule =
                 std::get_if<mtx::events::StateEvent<mtx::events::state::policy_rule::RoomRule>>(
                   e)) {
        auto sender = utils::replaceEmoji(displayName(QString::fromStdString(roomRule->sender)));
        if (roomRule->content.entity.empty() ||
            (roomRule->content.recommendation !=
               mtx::events::state::policy_rule::recommendation::ban &&
             roomRule->content.recommendation != unstable_ban)) {
            while (roomRule->content.entity.empty() &&
                   !roomRule->unsigned_data.replaces_state.empty()) {
                auto temp = events->get(roomRule->unsigned_data.replaces_state, idStr);
                if (!temp)
                    break;
                if (auto tempRule = std::get_if<
                      mtx::events::StateEvent<mtx::events::state::policy_rule::RoomRule>>(temp))
                    roomRule = tempRule;
                else
                    break;
            }
            return tr("%1 disabled the rule to ban rooms matching %2.")
              .arg(sender, qsHtml(roomRule->content.entity));
        } else {
            return tr("%1 added a rule to ban rooms matching %2 for '%3'.")
              .arg(sender, qsHtml(roomRule->content.entity), qsHtml(roomRule->content.reason));
        }
    } else if (auto serverRule =
                 std::get_if<mtx::events::StateEvent<mtx::events::state::policy_rule::ServerRule>>(
                   e)) {
        auto sender = utils::replaceEmoji(displayName(QString::fromStdString(serverRule->sender)));
        if (serverRule->content.entity.empty() ||
            (serverRule->content.recommendation !=
               mtx::events::state::policy_rule::recommendation::ban &&
             serverRule->content.recommendation != unstable_ban)) {
            while (serverRule->content.entity.empty() &&
                   !serverRule->unsigned_data.replaces_state.empty()) {
                auto temp = events->get(serverRule->unsigned_data.replaces_state, idStr);
                if (!temp)
                    break;
                if (auto tempRule = std::get_if<
                      mtx::events::StateEvent<mtx::events::state::policy_rule::ServerRule>>(temp))
                    serverRule = tempRule;
                else
                    break;
            }
            return tr("%1 disabled the rule to ban servers matching %2.")
              .arg(sender, qsHtml(serverRule->content.entity));
        } else {
            return tr("%1 added a rule to ban servers matching %2 for '%3'.")
              .arg(sender, qsHtml(serverRule->content.entity), qsHtml(serverRule->content.reason));
        }
    }

    return {};
}

QVariantMap
TimelineModel::formatRedactedEvent(const QString &id)
{
    QVariantMap pair{{"first", ""}, {"second", ""}};
    mtx::events::collections::TimelineEvents *e = events->get(id.toStdString(), "");
    if (!e)
        return pair;

    auto event = std::get_if<mtx::events::RoomEvent<mtx::events::msg::Redacted>>(e);
    if (!event)
        return pair;

    QString dateTime = QDateTime::fromMSecsSinceEpoch(event->origin_server_ts).toString();
    QString reason   = QLatin1String("");
    auto because     = event->unsigned_data.redacted_because;
    // User info about who actually sent the redacted event.
    QString redactedUser;
    QString redactedName;

    if (because.has_value()) {
        redactedUser = QString::fromStdString(because->sender).toHtmlEscaped();
        redactedName = utils::replaceEmoji(displayName(redactedUser));
        reason       = QString::fromStdString(because->content.reason).toHtmlEscaped();
    }

    if (reason.isEmpty()) {
        pair[QStringLiteral("first")] = tr("Removed by %1").arg(redactedName);
        pair[QStringLiteral("second")] =
          tr("%1 (%2) removed this message at %3").arg(redactedName, redactedUser, dateTime);
    } else {
        pair[QStringLiteral("first")]  = tr("Removed by %1 because: %2").arg(redactedName, reason);
        pair[QStringLiteral("second")] = tr("%1 (%2) removed this message at %3\nReason: %4")
                                           .arg(redactedName, redactedUser, dateTime, reason);
    }

    return pair;
}

void
TimelineModel::acceptKnock(const QString &id)
{
    mtx::events::collections::TimelineEvents *e = events->get(id.toStdString(), "");
    if (!e)
        return;

    auto event = std::get_if<mtx::events::StateEvent<mtx::events::state::Member>>(e);
    if (!event)
        return;

    if (!_timeline->permissions()->canInvite())
        return;

    if (cache::isRoomMember(event->state_key, room_id_.toStdString()))
        return;

    using namespace mtx::events::state;
    if (event->content.membership != Membership::Knock)
        return;

    Client::instance()->inviteUser(QString::fromStdString(event->state_key), QLatin1String(""), QLatin1String("Knock"));
}

bool
TimelineModel::showAcceptKnockButton(const QString &id)
{
    mtx::events::collections::TimelineEvents *e = events->get(id.toStdString(), "");
    if (!e)
        return false;

    auto event = std::get_if<mtx::events::StateEvent<mtx::events::state::Member>>(e);
    if (!event)
        return false;

    if (!_timeline->permissions()->canInvite())
        return false;

    if (cache::isRoomMember(event->state_key, room_id_.toStdString()))
        return false;

    using namespace mtx::events::state;
    return event->content.membership == Membership::Knock;
}

QString
TimelineModel::formatMemberEvent(const QString &id)
{
    mtx::events::collections::TimelineEvents *e = events->get(id.toStdString(), "");
    if (!e)
        return {};

    auto event = std::get_if<mtx::events::StateEvent<mtx::events::state::Member>>(e);
    if (!event)
        return {};

    mtx::events::StateEvent<mtx::events::state::Member> *prevEvent = nullptr;
    if (!event->unsigned_data.replaces_state.empty()) {
        auto tempPrevEvent = events->get(event->unsigned_data.replaces_state, event->event_id);
        if (tempPrevEvent) {
            prevEvent =
              std::get_if<mtx::events::StateEvent<mtx::events::state::Member>>(tempPrevEvent);
        }
    }

    QString user = QString::fromStdString(event->state_key);
    QString name = utils::replaceEmoji(displayName(user));
    QString rendered;
    QString sender     = QString::fromStdString(event->sender);
    QString senderName = utils::replaceEmoji(displayName(sender));

    // see table https://matrix.org/docs/spec/client_server/latest#m-room-member
    using namespace mtx::events::state;
    switch (event->content.membership) {
    case Membership::Invite:
        rendered = tr("%1 invited %2.").arg(senderName, name);
        break;
    case Membership::Join:
        if (prevEvent && prevEvent->content.membership == Membership::Join) {
            QString oldName = utils::replaceEmoji(
              QString::fromStdString(prevEvent->content.display_name).toHtmlEscaped());

            bool displayNameChanged =
              prevEvent->content.display_name != event->content.display_name;
            bool avatarChanged = prevEvent->content.avatar_url != event->content.avatar_url;

            if (displayNameChanged && avatarChanged)
                rendered = tr("%1 has changed their avatar and changed their "
                              "display name to %2.")
                             .arg(oldName, name);
            else if (displayNameChanged)
                rendered = tr("%1 has changed their display name to %2.").arg(oldName, name);
            else if (avatarChanged)
                rendered = tr("%1 changed their avatar.").arg(name);
            else
                rendered = tr("%1 changed some profile info.").arg(name);
            // the case of nothing changed but join follows join shouldn't happen, so
            // just show it as join
        } else {
            if (event->content.join_authorised_via_users_server.empty())
                rendered = tr("%1 joined.").arg(name);
            else
                rendered =
                  tr("%1 joined via authorisation from %2's server.")
                    .arg(name,
                         QString::fromStdString(event->content.join_authorised_via_users_server));
        }
        break;
    case Membership::Leave:
        if (!prevEvent || prevEvent->content.membership == Membership::Join) {
            if (event->state_key == event->sender)
                rendered = tr("%1 left the room.").arg(name);
            else
                rendered = tr("%2 kicked %1.").arg(name, senderName);
        } else if (prevEvent->content.membership == Membership::Invite) {
            if (event->state_key == event->sender)
                rendered = tr("%1 rejected their invite.").arg(name);
            else
                rendered = tr("%2 revoked the invite to %1.").arg(name, senderName);
        } else if (prevEvent->content.membership == Membership::Ban) {
            rendered = tr("%2 unbanned %1.").arg(name, senderName);
        } else if (prevEvent->content.membership == Membership::Knock) {
            if (event->state_key == event->sender)
                rendered = tr("%1 redacted their knock.").arg(name);
            else
                rendered = tr("%2 rejected the knock from %1.").arg(name, senderName);
        } else
            return tr("%1 left after having already left!",
                      "This is a leave event after the user already left and shouldn't "
                      "happen apart from state resets")
              .arg(name);
        break;

    case Membership::Ban:
        rendered = tr("%1 banned %2").arg(senderName, name);
        break;
    case Membership::Knock:
        rendered = tr("%1 knocked.").arg(name);
        break;
    }

    if (event->content.reason != "") {
        rendered += " " + tr("Reason: %1").arg(QString::fromStdString(event->content.reason));
    }

    return rendered;
}

void
TimelineModel::setEdit(const QString &newEdit)
{
    if (newEdit.isEmpty()) {
        resetEdit();
        return;
    }

    if (edit_.isEmpty()) {
        this->textBeforeEdit  = input()->text();
        this->replyBeforeEdit = reply_;
        nhlog::ui()->debug("Stored: {}", textBeforeEdit.toStdString());
    }

    auto quoted = [](QString in) { return in.replace("[", "\\[").replace("]", "\\]"); };

    if (edit_ != newEdit) {
        auto ev = events->get(newEdit.toStdString(), "");
        if (ev && mtx::accessors::sender(*ev) == http::client()->user_id().to_string()) {
            auto e = *ev;
            setReply(QString::fromStdString(mtx::accessors::relations(e).reply_to().value_or("")));

            auto msgType = mtx::accessors::msg_type(e);
            if (msgType == mtx::events::MessageType::Text ||
                msgType == mtx::events::MessageType::Notice ||
                msgType == mtx::events::MessageType::Emote) {
                auto relInfo  = relatedInfo(newEdit);
                auto editText = relInfo.quoted_body;

                if (!relInfo.quoted_formatted_body.isEmpty()) {
                    auto matches =
                      conf::strings::matrixToLink.globalMatch(relInfo.quoted_formatted_body);
                    std::map<QString, QString> reverseNameMapping;
                    while (matches.hasNext()) {
                        auto m                            = matches.next();
                        reverseNameMapping[m.captured(2)] = m.captured(1);
                    }

                    for (const auto &[user, link] : reverseNameMapping) {
                        // TODO(Nico): html unescape the user name
                        editText.replace(user, QStringLiteral("[%1](%2)").arg(quoted(user), link));
                    }
                }

                if (msgType == mtx::events::MessageType::Emote)
                    input()->setText("/me " + editText);
                else
                    input()->setText(editText);
            } else {
                input()->setText(QLatin1String(""));
            }

            edit_ = newEdit;
        } else {
            resetReply();

            input()->setText(QLatin1String(""));
            edit_ = QLatin1String("");
        }
        emit editChanged(edit_);
    }
}

void
TimelineModel::resetEdit()
{
    if (!edit_.isEmpty()) {
        edit_ = QLatin1String("");
        emit editChanged(edit_);
        nhlog::ui()->debug("Restoring: {}", textBeforeEdit.toStdString());
        input()->setText(textBeforeEdit);
        textBeforeEdit.clear();
        if (replyBeforeEdit.isEmpty())
            resetReply();
        else
            setReply(replyBeforeEdit);
        replyBeforeEdit.clear();
    }
}

void
TimelineModel::resetState()
{
    http::client()->get_state(
      room_id_.toStdString(),
      [this](const mtx::responses::StateEvents &events_, mtx::http::RequestErr e) {
          if (e) {
              nhlog::net()->error("Failed to retrieve current room state: {}", *e);
              return;
          }

          emit newState(events_);
      });
}

QString
TimelineModel::roomName() const
{
    auto info = cache::getRoomInfo({room_id_.toStdString()});

    if (!info.count(room_id_))
        return {};
    else
        return utils::replaceEmoji((info[room_id_].name).toHtmlEscaped());
}

QString
TimelineModel::plainRoomName() const
{
    auto info = cache::getRoomInfo({room_id_.toStdString()});

    if (!info.count(room_id_))
        return {};
    else
        return (info[room_id_].name);
}

QString
TimelineModel::roomAvatarUrl() const
{
    auto info = cache::getRoomInfo({room_id_.toStdString()});

    if (!info.count(room_id_))
        return {};
    else
        return (info[room_id_].avatar_url);
}

QString
TimelineModel::roomTopic() const
{
    auto info = cache::getRoomInfo({room_id_.toStdString()});

    if (!info.count(room_id_))
        return {};
    else
        return utils::replaceEmoji(
          utils::linkifyMessage((info[room_id_].topic).toHtmlEscaped()));
}

QStringList TimelineModel::pinnedMessages() const {
    return _timeline->pinnedMessages();
}

QStringList
TimelineModel::widgetLinks() const
{
    // auto evs =
    //   cache::client()->getStateEventsWithType<mtx::events::state::Widget>(room_id_.toStdString());
    // auto evs2 = cache::client()->getStateEventsWithType<mtx::events::state::Widget>(
    //   room_id_.toStdString(), mtx::events::EventType::Widget);
    // evs.insert(
    //   evs.end(), std::make_move_iterator(evs2.begin()), std::make_move_iterator(evs2.end()));

    // if (evs.empty())
    //     return {};

    QStringList list;

    // auto user = utils::localUser();
    // auto av   = QUrl::toPercentEncoding(
    //   QString::fromStdString(http::client()->mxc_to_download_url(avatarUrl(user).toStdString())));
    // auto disp  = QUrl::toPercentEncoding(displayName(user));
    // auto theme = "system";//UserSettings::instance()->theme();
    // if (theme == QStringLiteral("system"))
    //     theme.clear();
    // user = QUrl::toPercentEncoding(user);

    // list.reserve(evs.size());
    // for (const auto &p : evs) {
    //     auto url = QString::fromStdString(p.content.url);
    //     for (const auto &[k, v] : p.content.data)
    //         url.replace("$" + QString::fromStdString(k),
    //                     QUrl::toPercentEncoding(QString::fromStdString(v)));

    //     url.replace("$matrix_user_id", user);
    //     url.replace("$matrix_room_id", QUrl::toPercentEncoding(room_id_));
    //     url.replace("$matrix_display_name", disp);
    //     url.replace("$matrix_avatar_url", av);

    //     url.replace("$matrix_widget_id",
    //                 QUrl::toPercentEncoding(QString::fromStdString(p.content.id)));

    //     // url.replace("$matrix_client_theme", theme);
    //     url.replace("$org.matrix.msc2873.client_theme", theme);
    //     url.replace("$org.matrix.msc2873.client_id", "im.nheko");

    //     // compat with some widgets, i.e. FOSDEM
    //     url.replace("$theme", theme);

    //     url = QUrl::toPercentEncoding(url, "/:@?#&=%");

    //     list.push_back(
    //       QLatin1String("<a href='%1'>%2</a>")
    //         .arg(url,
    //              QString::fromStdString(p.content.name.empty() ? p.state_key : p.content.name)
    //                .toHtmlEscaped()));
    // }

    return list;
}

// crypto::Trust
// TimelineModel::trustlevel() const
// {
//     if (!isEncrypted_)
//         return crypto::Trust::Unverified;

//     return cache::client()->roomVerificationStatus(room_id_.toStdString());
// }

int TimelineModel::roomMemberCount() const {
    return _timeline->roomMemberCount();
}

QString TimelineModel::directChatOtherUserId() const {
    return _timeline->directChatOtherUserId();
}

QColor TimelineModel::userColor(QString id, QColor background)
{
    QPair<QString, quint64> idx{id, background.rgba64()};
    if (!userColors.contains(idx))
        userColors.insert(idx, QColor(utils::generateContrastingHexColor(id, background)));
    return userColors.value(idx);
}

QString
TimelineModel::escapeEmoji(QString str) const
{
    return utils::replaceEmoji(str);
}

void
TimelineModel::fixImageRendering(QQuickTextDocument *t, QQuickItem *i)
{
    if (t) {
        QObject::connect(t->textDocument(), SIGNAL(imagesLoaded()), i, SLOT(updateWholeDocument()));
    }
}


QObject *
TimelineModel::completerFor(QString completerName, QString roomId)
{
    if (completerName == QLatin1String("user")) {
        auto userModel = new UsersModel(roomId.toStdString());
        auto proxy     = new CompletionProxyModel(userModel);
        userModel->setParent(proxy);
        return proxy;
    } else if (completerName == QLatin1String("emoji")) {
        auto emojiModel = new emoji::EmojiModel();
        auto proxy      = new CompletionProxyModel(emojiModel);
        emojiModel->setParent(proxy);
        return proxy;
    } else if (completerName == QLatin1String("allemoji")) {
        auto emojiModel = new emoji::EmojiModel();
        auto proxy      = new CompletionProxyModel(emojiModel, 1, static_cast<size_t>(-1) / 4);
        emojiModel->setParent(proxy);
        return proxy;
    } else if (completerName == QLatin1String("room")) {
        auto roomModel = new RoomsModel(false);
        auto proxy     = new CompletionProxyModel(roomModel, 4);
        roomModel->setParent(proxy);
        return proxy;
    } else if (completerName == QLatin1String("roomAliases")) {
        auto roomModel = new RoomsModel(false);
        auto proxy     = new CompletionProxyModel(roomModel);
        roomModel->setParent(proxy);
        return proxy;
    } else if (completerName == QLatin1String("stickers")) {
        auto stickerModel = new CombinedImagePackModel(roomId.toStdString(), true);
        auto proxy        = new CompletionProxyModel(stickerModel, 1, static_cast<size_t>(-1) / 4);
        stickerModel->setParent(proxy);
        return proxy;
    } else if (completerName == QLatin1String("customEmoji")) {
        auto stickerModel = new CombinedImagePackModel(roomId.toStdString(), false);
        auto proxy        = new CompletionProxyModel(stickerModel);
        stickerModel->setParent(proxy);
        return proxy;
    }
    return nullptr;
}

void TimelineModel::markEventsAsRead(const QString &event_id){
    _timeline->markEventsAsRead(QStringList() << event_id);
}


void TimelineModel::focusMessageInput() {
    emit focusInput();
}

void
TimelineModel::openRoomMembers( )
{ 
    MemberList *memberList = new MemberList(_timeline->id());
    QQmlEngine::setObjectOwnership(memberList, QQmlEngine::JavaScriptOwnership);
    emit openRoomMembersDialog(memberList);
}

void
TimelineModel::openRoomSettings()
{
    RoomSettings *settings = new RoomSettings(room_id_);
    connect(this,
            &TimelineModel::roomAvatarUrlChanged,
            settings,
            &RoomSettings::avatarChanged);
    QQmlEngine::setObjectOwnership(settings, QQmlEngine::JavaScriptOwnership);
    emit openRoomSettingsDialog(settings);
}

void
TimelineModel::openInviteUsers()
{
    InviteesModel *model = new InviteesModel{};
    connect(model, &InviteesModel::accept, this, [this, model]() {
        for(auto const& u: model->mxids()){
            Client::instance()->inviteUser(this->room_id_,u,"Send invitation");
        }
    });
    QQmlEngine::setObjectOwnership(model, QQmlEngine::JavaScriptOwnership);
    emit openInviteUsersDialog(model);
}


void
TimelineModel::openImageOverlay(QString mxcUrl,
                                QString eventId,
                                double originalWidth,
                                double proportionalHeight)
{
    if (mxcUrl.isEmpty()) {
        return;
    }

    emit showImageOverlay(eventId, mxcUrl, originalWidth, proportionalHeight);
}
