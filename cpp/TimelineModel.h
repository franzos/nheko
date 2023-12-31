// SPDX-FileCopyrightText: 2021 Nheko Contributors
// SPDX-FileCopyrightText: 2022 Nheko Contributors
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAbstractListModel>
#include <QColor>
#include <QDate>
#include <QHash>
#include <QSet>
#include <QTimer>
#include <QVariant>
#include <QQuickTextDocument>
#include <mtxclient/http/errors.hpp>

#include <matrix-client-library/Client.h>
#include <matrix-client-library/CacheStructs.h>
#include <matrix-client-library/timeline/EventStore.h>
#include <matrix-client-library/timeline/Timeline.h>
#include <matrix-client-library/timeline/Permissions.h>
#include <matrix-client-library/UserProfile.h>

#include "ui/InputBar.h"
#include "ui/RoomSettings.h"
#include "ReadReceiptsModel.h"
#include "MemberList.h"
#include "InviteesModel.h"

namespace mtx::http {
using RequestErr = const std::optional<mtx::http::ClientError> &;
}

namespace mtx::responses {
struct Timeline;
struct Messages;
struct ClaimKeys;
}
struct RelatedInfo;

class TimelineModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(QString scrollTarget READ scrollTarget NOTIFY scrollTargetChanged)
    Q_PROPERTY(QString reply READ reply WRITE setReply NOTIFY replyChanged RESET resetReply)
    Q_PROPERTY(QString edit READ edit WRITE setEdit NOTIFY editChanged RESET resetEdit)
    Q_PROPERTY(bool paginationInProgress READ paginationInProgress NOTIFY paginationInProgressChanged)
    Q_PROPERTY(QString roomId READ roomId CONSTANT)
    Q_PROPERTY(QString roomName READ roomName NOTIFY roomNameChanged)
    Q_PROPERTY(QString plainRoomName READ plainRoomName NOTIFY plainRoomNameChanged)
    Q_PROPERTY(QString roomAvatarUrl READ roomAvatarUrl NOTIFY roomAvatarUrlChanged)
    Q_PROPERTY(QString roomTopic READ roomTopic NOTIFY roomTopicChanged)
    Q_PROPERTY(QStringList pinnedMessages READ pinnedMessages NOTIFY pinnedMessagesChanged)
    Q_PROPERTY(QStringList widgetLinks READ widgetLinks NOTIFY widgetLinksChanged)
    Q_PROPERTY(int roomMemberCount READ roomMemberCount NOTIFY roomMemberCountChanged)
    Q_PROPERTY(bool isEncrypted READ isEncrypted NOTIFY encryptionChanged)
    Q_PROPERTY(bool isSpace READ isSpace CONSTANT)
    // Q_PROPERTY(int trustlevel READ trustlevel NOTIFY trustlevelChanged)
    Q_PROPERTY(bool isDirect READ isDirect NOTIFY isDirectChanged)
    Q_PROPERTY(
      QString directChatOtherUserId READ directChatOtherUserId NOTIFY directChatOtherUserIdChanged)
    Q_PROPERTY(InputBar *input READ input CONSTANT)
    Q_PROPERTY(Permissions *permissions READ permissions NOTIFY permissionsChanged)

public:
    explicit TimelineModel(//TimelineViewManager *manager,
                           QString room_id,
                           QObject *parent = nullptr);

    enum Roles
    {
        Type,
        TypeString,
        IsOnlyEmoji,
        Body,
        FormattedBody,
        PreviousMessageUserId,
        IsSender,
        UserId,
        UserName,
        PreviousMessageDay,
        PreviousMessageIsStateEvent,
        Day,
        Timestamp,
        Url,
        ThumbnailUrl,
        Duration,
        Blurhash,
        Filename,
        Filesize,
        MimeType,
        OriginalHeight,
        OriginalWidth,
        ProportionalHeight,
        EventId,
        State,
        IsEdited,
        IsEditable,
        IsEncrypted,
        IsStateEvent,
        Trustlevel,
        EncryptionError,
        ReplyTo,
        Reactions,
        RoomId,
        RoomName,
        RoomTopic,
        CallType,
        Dump,
        RelatedEventCacheBuster,
        GeoUri,
    };
    Q_ENUM(Roles);

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant data(const mtx::events::collections::TimelineEvents &event, int role) const;
    Q_INVOKABLE QVariant dataById(const QString &id, int role, const QString &relatedTo);

    bool canFetchMore(const QModelIndex &) const override;
    void fetchMore(const QModelIndex &) override;

    Q_INVOKABLE QString getBareRoomLink(const QString & );
    Q_INVOKABLE QString getRoomVias(const QString & );
    Q_INVOKABLE QString displayName(const QString &id) const;
    Q_INVOKABLE QString avatarUrl(const QString &id) const;
    Q_INVOKABLE QString formatDateSeparator(QDate date) const;
    Q_INVOKABLE QString formatTypingUsers(const QStringList &users, const QColor &bg);
    Q_INVOKABLE bool showAcceptKnockButton(const QString &id);
    Q_INVOKABLE void acceptKnock(const QString &id);
    Q_INVOKABLE QString formatMemberEvent(const QString &id);
    Q_INVOKABLE QString formatJoinRuleEvent(const QString &id);
    Q_INVOKABLE QString formatHistoryVisibilityEvent(const QString &id);
    Q_INVOKABLE QString formatGuestAccessEvent(const QString &id);
    Q_INVOKABLE QString formatPowerLevelEvent(const QString &id);
    Q_INVOKABLE QString formatImagePackEvent(const QString &id);
    Q_INVOKABLE QString formatPolicyRule(const QString &id);
    Q_INVOKABLE QVariantMap formatRedactedEvent(const QString &id);

    Q_INVOKABLE void viewRawMessage(const QString &id);
    Q_INVOKABLE void forwardMessage(const QString &eventId, QString roomId);
    Q_INVOKABLE void viewDecryptedRawMessage(const QString &id);
    Q_INVOKABLE void openUserProfile(QString userid);
    Q_INVOKABLE void editAction(QString id);
    Q_INVOKABLE void replyAction(const QString &id);
    Q_INVOKABLE void unpin(const QString &id);
    Q_INVOKABLE void pin(const QString &id);
    Q_INVOKABLE void showReadReceipts(QString id);
    Q_INVOKABLE void redactEvent(const QString &id, const QString &reason = "");
    Q_INVOKABLE void redactAllFromUser(const QString &userid, const QString &reason = "");
    Q_INVOKABLE int idToIndex(const QString &id) const;
    Q_INVOKABLE QString indexToId(int index) const;
    Q_INVOKABLE void openMedia(const QString &eventId);
    Q_INVOKABLE void cacheMedia(const QString &eventId);
    Q_INVOKABLE bool saveMedia(const QString &eventId) const;
    Q_INVOKABLE void showEvent(QString eventId);
    Q_INVOKABLE void copyLinkToEvent(const QString &eventId);
    Q_INVOKABLE QColor userColor(QString id, QColor background);
    Q_INVOKABLE QString escapeEmoji(QString str) const;
    Q_INVOKABLE QString htmlEscape(QString str) const { return str.toHtmlEscaped(); }
    Q_INVOKABLE void fixImageRendering(QQuickTextDocument *t, QQuickItem *i);
    Q_INVOKABLE void markEventsAsRead(const QString &event_id);
    Q_INVOKABLE void focusMessageInput();
    Q_INVOKABLE void openRoomMembers();
    Q_INVOKABLE void openRoomSettings();
    Q_INVOKABLE void openInviteUsers();
    Q_INVOKABLE void openImageOverlay(QString mxcUrl,
                                      QString eventId,
                                      double originalWidth,
                                      double proportionalHeight);

    void
    cacheMedia(const QString &eventId, const std::function<void(const QString filename)> &callback);
    Q_INVOKABLE void sendReset()
    {
        beginResetModel();
        endResetModel();
    }

    Q_INVOKABLE void requestKeyForEvent(const QString &id);

    std::vector<::Reaction> reactions(const std::string &event_id)
    {
        auto list = events->reactions(event_id);
        std::vector<::Reaction> vec;
        vec.reserve(list.size());
        for (const auto &r : list)
            vec.push_back(r.value<Reaction>());
        return vec;
    }

    void updateLastMessage();
    RelatedInfo relatedInfo(const QString &id);

    DescInfo lastMessage() const { return lastMessage_; }
    bool isSpace() const { return isSpace_; }
    bool isEncrypted() const { return isEncrypted_; }
    // crypto::Trust trustlevel() const;
    int roomMemberCount() const;
    bool isDirect() const { return roomMemberCount() <= 2; }
    QString directChatOtherUserId() const;

    std::optional<mtx::events::collections::TimelineEvents> eventById(const QString &id)
    {
        auto e = events->get(id.toStdString(), "");
        if (e)
            return *e;
        else
            return std::nullopt;
    }

public slots:
    void setCurrentIndex(int index);
    int currentIndex() const { return idToIndex(currentId); }
    void eventShown();
    QVariantMap getDump(const QString &eventId, const QString &relatedTo) const;
    Timeline *timeline() { return _timeline; };
    void updateTypingUsers(const QStringList &users)
    {
        emit typingUsersChanged(users);
    }
    QStringList typingUsers() const { return _timeline->typingUsers(); }
    bool paginationInProgress() const { return m_paginationInProgress; }
    QString reply() const { return reply_; }
    void setReply(const QString &newReply)
    {
        if (edit_.startsWith('m'))
            return;
        if (reply_ != newReply) {
            reply_ = newReply;
            emit replyChanged(reply_);
        }
    }
    void resetReply()
    {
        if (!reply_.isEmpty()) {
            reply_ = QLatin1String("");
            emit replyChanged(reply_);
        }
    }
    QString edit() const { return edit_; }
    void setEdit(const QString &newEdit);
    void resetEdit();
    void setDecryptDescription(bool decrypt) { decryptDescription = decrypt; }
    void clearTimeline() { events->clearTimeline(); }
    void resetState();

    QString roomName() const;
    QString plainRoomName() const;
    QString roomTopic() const;
    QStringList pinnedMessages() const;
    QStringList widgetLinks() const;
    InputBar *input() { return &input_; }
    Permissions *permissions() { return _timeline->permissions(); }
    QString roomAvatarUrl() const;
    QString roomId() const { return room_id_; }

    bool hasMentions() const { return highlight_count > 0; }
    int notificationCount() const { return notification_count; }

    QString scrollTarget() const;
    QObject *completerFor(QString completerName, QString roomId = QLatin1String(QLatin1String("")));

private slots:
    void scrollTimerEvent();

signals:
    void dataAtIdChanged(QString id);
    void currentIndexChanged(int index);
    void redactionFailed(QString id);
    void mediaCached(QString mxcUrl, QString cacheUrl);
    void typingUsersChanged(const QStringList &users);
    void replyChanged(QString reply);
    void editChanged(QString reply);
    void openReadReceiptsDialog(ReadReceiptsProxy *rr);
    void showRawMessageDialog(QString rawMessage);
    void paginationInProgressChanged(const bool);
    void newCallEvent(const mtx::events::collections::TimelineEvents &event);
    void scrollToIndex(int index);
    void focusInput();
    void openRoomMembersDialog(MemberList *members);
    void openRoomSettingsDialog(RoomSettings *settings);
    void openProfile(UserProfile *profile);
    void openInviteUsersDialog(InviteesModel *invitees);
    void showImageOverlay(QString eventId,
                          QString url,
                          double originalWidth,
                          double proportionalHeight);
                          
    void lastMessageChanged();
    void notificationsChanged();

    void newState(mtx::responses::StateEvents events);

    void newMessageToSend(mtx::events::collections::TimelineEvents event);
    void addPendingMessageToStore(mtx::events::collections::TimelineEvents event);

    void encryptionChanged();
    void trustlevelChanged();
    void roomNameChanged();
    void plainRoomNameChanged();
    void roomTopicChanged();
    void pinnedMessagesChanged();
    void widgetLinksChanged();
    void roomAvatarUrlChanged();
    void roomMemberCountChanged();
    void isDirectChanged();
    void directChatOtherUserIdChanged();
    void permissionsChanged();

    void scrollTargetChanged();

private:
    void readEvent(const std::string &id);

    void setPaginationInProgress(const bool paginationInProgress);

    QString room_id_;

    QSet<QString> read;

    EventStore *events;

    QString currentId, currentReadId;
    QString reply_, edit_;
    QString textBeforeEdit, replyBeforeEdit;

    InputBar input_{this};

    QTimer showEventTimer{this};
    QString eventIdToShow;
    int showEventTimerCounter = 0;

    DescInfo lastMessage_{};

    // friend struct SendMessageVisitor;

    uint64_t notification_count = 0, highlight_count = 0;

    unsigned int relatedEventCacheBuster = 0;

    bool decryptDescription     = true;
    bool m_paginationInProgress = false;
    bool isSpace_               = false;
    bool isEncrypted_           = false;
    QHash<QPair<QString, quint64>, QColor> userColors;
    Timeline *_timeline = nullptr;
};

