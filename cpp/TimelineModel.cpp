#include "TimelineModel.h"
#include <QDebug>
#include <QRegularExpression>
#include <matrix-client-library/EventAccessors.h>
#include <QFontMetrics>

TimelineModel::~TimelineModel(){
    if(_timeline) {
        disconnect(_timeline, &Timeline::newEventsStored, nullptr, nullptr);
        disconnect(_timeline, &Timeline::typingUsersChanged, nullptr, nullptr);
        _timeline = nullptr;
    }
}

TimelineModel::TimelineModel(const QString roomID , QObject *parent ): QAbstractListModel(parent) {
    _timeline = Client::instance()->timeline(roomID);
    if(_timeline){
        connect(_timeline, &Timeline::newEventsStored, [&](int from, int len){
            add(_timeline->getEvents(from, len));
        });

        connect(_timeline, &Timeline::typingUsersChanged,[&](const QStringList &users){
            QString text = "";
            if(users.size()){
                for(int i = 0; i<users.size(); i++){
                    text += _timeline->displayName(users[i]);
                    if( (i+1) < users.size())
                        text += " and ";
                }
                if(users.size() > 1)
                    text += " are ";
                else 
                    text += " is ";
                text +=  " typing ...";
            }
            emit typingUsersChanged(text);
        });

        add(_timeline->getEvents(0,_timeline->eventSize()));
    }
}

void TimelineModel::add(const QVector<DescInfo> &items){
    for(auto const &e: items){
        TimelineItem item(e.event_id, _timeline->displayName(e.userid), e.body, e.descriptiveTime, e.timestamp, e.isLocal);
        add(item);
    }
}

int TimelineModel::rowCount(const QModelIndex &parent) const
{
    (void)parent;
    return _timelineItems.count();
}

// QVariant TimelineModel::data(const QModelIndex &index, int role) const
// {
//     if (!index.isValid() || index.row() >= _timelineItems.size())
//         return QVariant();

//     const TimelineItem room = _timelineItems.at(index.row()); // TODO
//     if (role == idRole)
//         return room.id();
//     else if (role == senderIdRole)
//         return room.senderId();
//     else if (role == bodyRole)
//         return room.body();
//     else if (role == descriptiveTimeRole)
//         return room.descriptiveTime();
//     else if (role == timestampRole)
//         return room.timestamp();
//     else if (role == isLocalRole)
//         return room.isLocal();
//     return QVariant();
// }

QVariant TimelineModel::headerData(int section, Qt::Orientation orientation,
                                     int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        return QString("Column %1").arg(section);
    else
        return QString("Row %1").arg(section);
}

Qt::ItemFlags TimelineModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

// QHash<int, QByteArray> TimelineModel::roleNames() const {
//     QHash<int, QByteArray> roles;
//     roles[idRole] = "id";
//     roles[senderIdRole] = "senderId";
//     roles[bodyRole] = "body";
//     roles[descriptiveTimeRole] = "descriptiveTime";
//     roles[timestampRole] = "timestamp";
//     roles[isLocalRole] = "isLocal";
//     return roles;
// }
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
    };

    return roles;
}

int TimelineModel::timelineIdToIndex(const QString &roomid){
    for (int i = 0; i < (int)_timelineItems.size(); i++) {
        if (_timelineItems[i].id() == roomid)
            return i;
    }
    return -1;
}

bool TimelineModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    (void)parent;
    beginRemoveRows(QModelIndex(), position, position+rows-1);
    for (int row = 0; row < rows; ++row) {
        _timelineItems.removeAt(position);
        _messageIds.removeAt(position);
    }
    endRemoveRows();
    return true;
}

void TimelineModel::add(TimelineItem &item){
    // if(_messageIds.contains(item.id())){
    //     auto idx = roomidToIndex(item.id());
    //     if(_timelineItems.at(idx).invite() && !item.invite()){
    //         setData(index(idx), false, inviteRole);
    //     }
    // } else 
    if(!_messageIds.contains(item.id())) {
        // add new room [room events]
        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        _timelineItems << item;
        _messageIds << item.id();
        endInsertRows();
        qDebug() << "Added to Timeline :" <<  item.toString();
    }
}

// bool TimelineModel::setData(const QModelIndex &index, const QVariant &value, int role)
// {
//     if (data(index, role) != value && index.isValid()) {
//         TimelineItem item = _timelineItems.at(index.row());

//         switch (role) {
//         case idRole:
//             item.setId(value.toString());
//             break;
//         case senderIdRole:
//             item.setSenderId(value.toString());
//             break;
//         case bodyRole:
//             item.setBody(value.toString());
//             break;
//         case descriptiveTimeRole:
//             item.setDescriptiveTime(value.toString());
//             break;
//         case timestampRole:
//             item.setTimestamp(value.toUInt());
//             break;
//         case isLocalRole:
//             item.setLocal(value.toBool());
//             break;
//         default:
//             return false;
//         }

//         _timelineItems.replace(index.row(), item);

//         emit dataChanged(index, index, QVector<int>() << role);
//         return true;
//     }
//     return false;
// }

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
        return QVariant(_timeline->displayName(QString::fromStdString(acc::sender(event))));

    case Day: {
        QDateTime prevDate = origin_server_ts(event);
        prevDate.setTime(QTime());
        return {prevDate.toMSecsSinceEpoch()};
    }
    case Timestamp:
        return QVariant(origin_server_ts(event));
    // case Type:
    //     return {toRoomEventType(event)};
    // case TypeString:
    //     return QVariant(toRoomEventTypeString(event));
    // case IsOnlyEmoji: {
    //     QString qBody = QString::fromStdString(body(event));

    //     QVector<uint> utf32_string = qBody.toUcs4();
    //     int emojiCount             = 0;

    //     for (auto &code : utf32_string) {
    //         if (utils::codepointIsEmoji(code)) {
    //             emojiCount++;
    //         } else {
    //             return {0};
    //         }
    //     }

    //     return {emojiCount};
    // }
    case Body:
        return QVariant(escapeEmoji(QString::fromStdString(body(event)).toHtmlEscaped()));
    case FormattedBody: {
        const static QRegularExpression replyFallback(
          QStringLiteral("<mx-reply>.*</mx-reply>"),
          QRegularExpression::DotMatchesEverythingOption);

        auto ascent = QFontMetrics(QString("default")).ascent();

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

        return QVariant(
          utils::replaceEmoji(utils::linkifyMessage(utils::escapeBlacklistedHtml(formattedBody_))));
    }
    case Url:
        return QVariant(QString::fromStdString(url(event)));
    case ThumbnailUrl:
        return QVariant(QString::fromStdString(thumbnail_url(event)));
    // case Duration:
    //     return QVariant(static_cast<qulonglong>(duration(event)));
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
    // case State: {
    //     auto id             = QString::fromStdString(event_id(event));
    //     auto containsOthers = [](const auto &vec) {
    //         for (const auto &e : vec)
    //             if (e.second != http::client()->user_id().to_string())
    //                 return true;
    //         return false;
    //     };

    //     // only show read receipts for messages not from us
    //     if (acc::sender(event) != http::client()->user_id().to_string())
    //         return qml_mtx_events::Empty;
    //     else if (!id.isEmpty() && id[0] == 'm')
    //         return qml_mtx_events::Sent;
    //     else if (read.contains(id) || containsOthers(cache::readReceipts(id, room_id_)))
    //         return qml_mtx_events::Read;
    //     else
    //         return qml_mtx_events::Received;
    // }
    // case IsEdited:
    //     return {relations(event).replaces().has_value()};
    case IsEditable:
        return {!is_state_event(event) &&
                mtx::accessors::sender(event) == http::client()->user_id().to_string()};
    case IsEncrypted: {
        auto encrypted_event = _timeline->events()->get(event_id(event), "", false);
        return encrypted_event &&
               std::holds_alternative<mtx::events::EncryptedEvent<mtx::events::msg::Encrypted>>(
                 *encrypted_event);
    }
    case IsStateEvent: {
        return is_state_event(event);
    }

    // case Trustlevel: {
    //     auto encrypted_event = events.get(event_id(event), "", false);
    //     if (encrypted_event) {
    //         if (auto encrypted =
    //               std::get_if<mtx::events::EncryptedEvent<mtx::events::msg::Encrypted>>(
    //                 &*encrypted_event)) {
    //             return olm::calculate_trust(
    //               encrypted->sender,
    //               MegolmSessionIndex(room_id_.toStdString(), encrypted->content));
    //         }
    //     }
    //     return crypto::Trust::Unverified;
    // }

    case EncryptionError:
        return _timeline->events()->decryptionError(event_id(event));

    // case ReplyTo:
    //     return QVariant(QString::fromStdString(relations(event).reply_to().value_or("")));
    // case Reactions: {
    //     auto id = relations(event).replaces().value_or(event_id(event));
    //     return QVariant::fromValue(events.reactions(id));
    // }
    case RoomId:
        return QVariant(_timeline->id());
    case RoomName:
        return QVariant(
          escapeEmoji(QString::fromStdString(room_name(event)).toHtmlEscaped()));
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

        return QVariant(m);
    }
    // case RelatedEventCacheBuster:
    //     return relatedEventCacheBuster;
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
    if (index.row() + 1 == rowCount())// && !m_paginationInProgress)
        const_cast<TimelineModel *>(this)->fetchMore(index);

    auto event = _timeline->events()->get(rowCount() - index.row() - 1);

    if (!event)
        return "";

    if (role == PreviousMessageDay || role == PreviousMessageUserId ||
        role == PreviousMessageIsStateEvent) {
        int prevIdx = rowCount() - index.row() - 2;
        if (prevIdx < 0)
            return {};
        auto tempEv = _timeline->events()->get(prevIdx);
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
    if (auto event = _timeline->events()->get(id.toStdString(), relatedTo.toStdString()))
        return data(*event, role);
    return {};
}

void TimelineModel::add(QList<TimelineItem> &rooms){
    if(rooms.size()){
        for(auto &r: rooms){
            add(r);
        }
    }
}

void TimelineModel::remove(const QStringList &ids){
    for(auto const &id: ids){
        auto idx = timelineIdToIndex(id);
        if (idx != -1) {
            if(removeRows(idx,1)){
                qDebug() << "Removed from Timeline: " << id;
            }
        }
    }
}

void TimelineModel::send(const QString &message){
    if(_timeline){
        _timeline->sendMessage(message);
    }
}

QString TimelineModel::escapeEmoji(QString str) const{
    return _timeline->escapeEmoji(str);
}


QString TimelineModel::formatDateSeparator(QDate date) const {
    auto now = QDateTime::currentDateTime();

    QString fmt = QLocale::system().dateFormat(QLocale::LongFormat);

    if (now.date().year() == date.year()) {
        QRegularExpression rx(QStringLiteral("[^a-zA-Z]*y+[^a-zA-Z]*"));
        fmt = fmt.remove(rx);
    }

    return date.toString(fmt);
}

void TimelineModel::fixImageRendering(QQuickTextDocument *t, QQuickItem *i)
{
    if (t) {
        QObject::connect(t->textDocument(), SIGNAL(imagesLoaded()), i, SLOT(updateWholeDocument()));
    }
}

QColor TimelineModel::userColor(QString id, QColor background)
{
    QPair<QString, quint64> idx{id, background.rgba64()};
    if (!userColors.contains(idx))
        userColors.insert(idx, QColor(utils::generateContrastingHexColor(id, background)));
    return userColors.value(idx);
}

void TimelineModel::focusMessageInput()
{
    emit focusInput();
}


QString TimelineModel::avatarUrl(const QString &id) const {
    return _timeline->avatarUrl(id);
}


namespace {
struct RoomEventType
{
    template<class T>
    qml_mtx_events::EventType operator()(const mtx::events::Event<T> &e)
    {
        return qml_mtx_events::toRoomEventType(e.type);
    }
    qml_mtx_events::EventType operator()(const mtx::events::Event<mtx::events::msg::Audio> &)
    {
        return qml_mtx_events::EventType::AudioMessage;
    }
    qml_mtx_events::EventType operator()(const mtx::events::Event<mtx::events::msg::Emote> &)
    {
        return qml_mtx_events::EventType::EmoteMessage;
    }
    qml_mtx_events::EventType operator()(const mtx::events::Event<mtx::events::msg::File> &)
    {
        return qml_mtx_events::EventType::FileMessage;
    }
    qml_mtx_events::EventType operator()(const mtx::events::Event<mtx::events::msg::Image> &)
    {
        return qml_mtx_events::EventType::ImageMessage;
    }
    qml_mtx_events::EventType operator()(const mtx::events::Event<mtx::events::msg::Notice> &)
    {
        return qml_mtx_events::EventType::NoticeMessage;
    }
    qml_mtx_events::EventType operator()(const mtx::events::Event<mtx::events::msg::Text> &)
    {
        return qml_mtx_events::EventType::TextMessage;
    }
    qml_mtx_events::EventType operator()(const mtx::events::Event<mtx::events::msg::Video> &)
    {
        return qml_mtx_events::EventType::VideoMessage;
    }
    qml_mtx_events::EventType
    operator()(const mtx::events::Event<mtx::events::msg::KeyVerificationRequest> &)
    {
        return qml_mtx_events::EventType::KeyVerificationRequest;
    }
    qml_mtx_events::EventType
    operator()(const mtx::events::Event<mtx::events::msg::KeyVerificationStart> &)
    {
        return qml_mtx_events::EventType::KeyVerificationStart;
    }
    qml_mtx_events::EventType
    operator()(const mtx::events::Event<mtx::events::msg::KeyVerificationMac> &)
    {
        return qml_mtx_events::EventType::KeyVerificationMac;
    }
    qml_mtx_events::EventType
    operator()(const mtx::events::Event<mtx::events::msg::KeyVerificationAccept> &)
    {
        return qml_mtx_events::EventType::KeyVerificationAccept;
    }
    qml_mtx_events::EventType
    operator()(const mtx::events::Event<mtx::events::msg::KeyVerificationReady> &)
    {
        return qml_mtx_events::EventType::KeyVerificationReady;
    }
    qml_mtx_events::EventType
    operator()(const mtx::events::Event<mtx::events::msg::KeyVerificationCancel> &)
    {
        return qml_mtx_events::EventType::KeyVerificationCancel;
    }
    qml_mtx_events::EventType
    operator()(const mtx::events::Event<mtx::events::msg::KeyVerificationKey> &)
    {
        return qml_mtx_events::EventType::KeyVerificationKey;
    }
    qml_mtx_events::EventType
    operator()(const mtx::events::Event<mtx::events::msg::KeyVerificationDone> &)
    {
        return qml_mtx_events::EventType::KeyVerificationDone;
    }
    qml_mtx_events::EventType operator()(const mtx::events::Event<mtx::events::msg::Redacted> &)
    {
        return qml_mtx_events::EventType::Redacted;
    }
    qml_mtx_events::EventType operator()(const mtx::events::Event<mtx::events::msg::CallInvite> &)
    {
        return qml_mtx_events::EventType::CallInvite;
    }
    qml_mtx_events::EventType operator()(const mtx::events::Event<mtx::events::msg::CallAnswer> &)
    {
        return qml_mtx_events::EventType::CallAnswer;
    }
    qml_mtx_events::EventType operator()(const mtx::events::Event<mtx::events::msg::CallHangUp> &)
    {
        return qml_mtx_events::EventType::CallHangUp;
    }
    qml_mtx_events::EventType
    operator()(const mtx::events::Event<mtx::events::msg::CallCandidates> &)
    {
        return qml_mtx_events::EventType::CallCandidates;
    }
    // ::EventType::Type operator()(const Event<mtx::events::msg::Location> &e) { return
    // ::EventType::LocationMessage; }
};
}

qml_mtx_events::EventType
qml_mtx_events::toRoomEventType(mtx::events::EventType e)
{
    using mtx::events::EventType;
    switch (e) {
    case EventType::RoomKeyRequest:
        return qml_mtx_events::EventType::KeyRequest;
    case EventType::Reaction:
        return qml_mtx_events::EventType::Reaction;
    case EventType::RoomAliases:
        return qml_mtx_events::EventType::Aliases;
    case EventType::RoomAvatar:
        return qml_mtx_events::EventType::Avatar;
    case EventType::RoomCanonicalAlias:
        return qml_mtx_events::EventType::CanonicalAlias;
    case EventType::RoomCreate:
        return qml_mtx_events::EventType::RoomCreate;
    case EventType::RoomEncrypted:
        return qml_mtx_events::EventType::Encrypted;
    case EventType::RoomEncryption:
        return qml_mtx_events::EventType::Encryption;
    case EventType::RoomGuestAccess:
        return qml_mtx_events::EventType::RoomGuestAccess;
    case EventType::RoomHistoryVisibility:
        return qml_mtx_events::EventType::RoomHistoryVisibility;
    case EventType::RoomJoinRules:
        return qml_mtx_events::EventType::RoomJoinRules;
    case EventType::RoomMember:
        return qml_mtx_events::EventType::Member;
    case EventType::RoomMessage:
        return qml_mtx_events::EventType::UnknownMessage;
    case EventType::RoomName:
        return qml_mtx_events::EventType::Name;
    case EventType::RoomPowerLevels:
        return qml_mtx_events::EventType::PowerLevels;
    case EventType::RoomTopic:
        return qml_mtx_events::EventType::Topic;
    case EventType::RoomTombstone:
        return qml_mtx_events::EventType::Tombstone;
    case EventType::RoomRedaction:
        return qml_mtx_events::EventType::Redaction;
    case EventType::RoomPinnedEvents:
        return qml_mtx_events::EventType::PinnedEvents;
    case EventType::Sticker:
        return qml_mtx_events::EventType::Sticker;
    case EventType::Tag:
        return qml_mtx_events::EventType::Tag;
    case EventType::SpaceParent:
        return qml_mtx_events::EventType::SpaceParent;
    case EventType::SpaceChild:
        return qml_mtx_events::EventType::SpaceChild;
    case EventType::Unsupported:
        return qml_mtx_events::EventType::Unsupported;
    default:
        return qml_mtx_events::EventType::UnknownMessage;
    }
}

qml_mtx_events::EventType
toRoomEventType(const mtx::events::collections::TimelineEvents &event)
{
    return std::visit(RoomEventType{}, event);
}

QString
toRoomEventTypeString(const mtx::events::collections::TimelineEvents &event)
{
    return std::visit([](const auto &e) { return QString::fromStdString(to_string(e.type)); },
                      event);
}

mtx::events::EventType
qml_mtx_events::fromRoomEventType(qml_mtx_events::EventType t)
{
    switch (t) {
    // Unsupported event
    case qml_mtx_events::Unsupported:
        return mtx::events::EventType::Unsupported;

    /// m.room_key_request
    case qml_mtx_events::KeyRequest:
        return mtx::events::EventType::RoomKeyRequest;
    /// m.reaction:
    case qml_mtx_events::Reaction:
        return mtx::events::EventType::Reaction;
    /// m.room.aliases
    case qml_mtx_events::Aliases:
        return mtx::events::EventType::RoomAliases;
    /// m.room.avatar
    case qml_mtx_events::Avatar:
        return mtx::events::EventType::RoomAvatar;
    /// m.call.invite
    case qml_mtx_events::CallInvite:
        return mtx::events::EventType::CallInvite;
    /// m.call.answer
    case qml_mtx_events::CallAnswer:
        return mtx::events::EventType::CallAnswer;
    /// m.call.hangup
    case qml_mtx_events::CallHangUp:
        return mtx::events::EventType::CallHangUp;
    /// m.call.candidates
    case qml_mtx_events::CallCandidates:
        return mtx::events::EventType::CallCandidates;
    /// m.room.canonical_alias
    case qml_mtx_events::CanonicalAlias:
        return mtx::events::EventType::RoomCanonicalAlias;
    /// m.room.create
    case qml_mtx_events::RoomCreate:
        return mtx::events::EventType::RoomCreate;
    /// m.room.encrypted.
    case qml_mtx_events::Encrypted:
        return mtx::events::EventType::RoomEncrypted;
    /// m.room.encryption.
    case qml_mtx_events::Encryption:
        return mtx::events::EventType::RoomEncryption;
    /// m.room.guest_access
    case qml_mtx_events::RoomGuestAccess:
        return mtx::events::EventType::RoomGuestAccess;
    /// m.room.history_visibility
    case qml_mtx_events::RoomHistoryVisibility:
        return mtx::events::EventType::RoomHistoryVisibility;
    /// m.room.join_rules
    case qml_mtx_events::RoomJoinRules:
        return mtx::events::EventType::RoomJoinRules;
    /// m.room.member
    case qml_mtx_events::Member:
        return mtx::events::EventType::RoomMember;
    /// m.room.name
    case qml_mtx_events::Name:
        return mtx::events::EventType::RoomName;
    /// m.room.power_levels
    case qml_mtx_events::PowerLevels:
        return mtx::events::EventType::RoomPowerLevels;
    /// m.room.tombstone
    case qml_mtx_events::Tombstone:
        return mtx::events::EventType::RoomTombstone;
    /// m.room.topic
    case qml_mtx_events::Topic:
        return mtx::events::EventType::RoomTopic;
    /// m.room.redaction
    case qml_mtx_events::Redaction:
        return mtx::events::EventType::RoomRedaction;
    /// m.room.pinned_events
    case qml_mtx_events::PinnedEvents:
        return mtx::events::EventType::RoomPinnedEvents;
    /// m.widget
    case qml_mtx_events::Widget:
        return mtx::events::EventType::Widget;
    // m.sticker
    case qml_mtx_events::Sticker:
        return mtx::events::EventType::Sticker;
    // m.tag
    case qml_mtx_events::Tag:
        return mtx::events::EventType::Tag;
    // m.space.parent
    case qml_mtx_events::SpaceParent:
        return mtx::events::EventType::SpaceParent;
    // m.space.child
    case qml_mtx_events::SpaceChild:
        return mtx::events::EventType::SpaceChild;
    /// m.room.message
    case qml_mtx_events::AudioMessage:
    case qml_mtx_events::EmoteMessage:
    case qml_mtx_events::FileMessage:
    case qml_mtx_events::ImageMessage:
    case qml_mtx_events::LocationMessage:
    case qml_mtx_events::NoticeMessage:
    case qml_mtx_events::TextMessage:
    case qml_mtx_events::VideoMessage:
    case qml_mtx_events::Redacted:
    case qml_mtx_events::UnknownMessage:
    case qml_mtx_events::KeyVerificationRequest:
    case qml_mtx_events::KeyVerificationStart:
    case qml_mtx_events::KeyVerificationMac:
    case qml_mtx_events::KeyVerificationAccept:
    case qml_mtx_events::KeyVerificationCancel:
    case qml_mtx_events::KeyVerificationKey:
    case qml_mtx_events::KeyVerificationDone:
    case qml_mtx_events::KeyVerificationReady:
        return mtx::events::EventType::RoomMessage;
        //! m.image_pack, currently im.ponies.room_emotes
    case qml_mtx_events::ImagePackInRoom:
        return mtx::events::EventType::ImagePackInRoom;
    //! m.image_pack, currently im.ponies.user_emotes
    case qml_mtx_events::ImagePackInAccountData:
        return mtx::events::EventType::ImagePackInAccountData;
    //! m.image_pack.rooms, currently im.ponies.emote_rooms
    case qml_mtx_events::ImagePackRooms:
        return mtx::events::EventType::ImagePackRooms;
    default:
        return mtx::events::EventType::Unsupported;
    };
}