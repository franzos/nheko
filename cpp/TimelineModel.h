#ifndef TIME_LINE_MODE_H
#define TIME_LINE_MODE_H

#include <QAbstractListModel>
#include <QObject>
#include <QStringList>
#include <matrix-client-library/Client.h>
#include <QQuickTextDocument>
#include "TimelineItem.h"


namespace qml_mtx_events {
Q_NAMESPACE

enum EventType
{
    // Unsupported event
    Unsupported,
    /// m.room_key_request
    KeyRequest,
    /// m.reaction,
    Reaction,
    /// m.room.aliases
    Aliases,
    /// m.room.avatar
    Avatar,
    /// m.call.invite
    CallInvite,
    /// m.call.answer
    CallAnswer,
    /// m.call.hangup
    CallHangUp,
    /// m.call.candidates
    CallCandidates,
    /// m.room.canonical_alias
    CanonicalAlias,
    /// m.room.create
    RoomCreate,
    /// m.room.encrypted.
    Encrypted,
    /// m.room.encryption.
    Encryption,
    /// m.room.guest_access
    RoomGuestAccess,
    /// m.room.history_visibility
    RoomHistoryVisibility,
    /// m.room.join_rules
    RoomJoinRules,
    /// m.room.member
    Member,
    /// m.room.name
    Name,
    /// m.room.power_levels
    PowerLevels,
    /// m.room.tombstone
    Tombstone,
    /// m.room.topic
    Topic,
    /// m.room.redaction
    Redaction,
    /// m.room.pinned_events
    PinnedEvents,
    // m.sticker
    Sticker,
    // m.tag
    Tag,
    // m.widget
    Widget,
    /// m.room.message
    AudioMessage,
    EmoteMessage,
    FileMessage,
    ImageMessage,
    LocationMessage,
    NoticeMessage,
    TextMessage,
    VideoMessage,
    Redacted,
    UnknownMessage,
    KeyVerificationRequest,
    KeyVerificationStart,
    KeyVerificationMac,
    KeyVerificationAccept,
    KeyVerificationCancel,
    KeyVerificationKey,
    KeyVerificationDone,
    KeyVerificationReady,
    //! m.image_pack, currently im.ponies.room_emotes
    ImagePackInRoom,
    //! m.image_pack, currently im.ponies.user_emotes
    ImagePackInAccountData,
    //! m.image_pack.rooms, currently im.ponies.emote_rooms
    ImagePackRooms,
    // m.space.parent
    SpaceParent,
    // m.space.child
    SpaceChild,
};
Q_ENUM_NS(EventType)
mtx::events::EventType fromRoomEventType(qml_mtx_events::EventType);
qml_mtx_events::EventType
toRoomEventType(mtx::events::EventType e);

enum EventState
{
    //! The plaintext message was received by the server.
    Received,
    //! At least one of the participants has read the message.
    Read,
    //! The client sent the message. Not yet received.
    Sent,
    //! When the message is loaded from cache or backfill.
    Empty,
};
Q_ENUM_NS(EventState)
}

class TimelineModel : public QAbstractListModel{
    Q_OBJECT

    Q_PROPERTY(int rowCount READ rowCount NOTIFY rowCountChanged)
public:
    enum TimelineItemRoles {
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
    };
    Q_ENUM(TimelineItemRoles);

    TimelineModel(const QString roomID = "", QObject *parent = 0);
    ~TimelineModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant data(const mtx::events::collections::TimelineEvents &event, int role) const;
    Q_INVOKABLE QVariant dataById(const QString &id, int role, const QString &relatedTo);
    // bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QVariant headerData(int section, Qt::Orientation orientation,int role = Qt::DisplayRole) const override;
    bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;
    int  timelineIdToIndex(const QString &roomid);
    Q_INVOKABLE QString escapeEmoji(QString str) const;
    Q_INVOKABLE QString htmlEscape(QString str) const { return str.toHtmlEscaped(); }
    Q_INVOKABLE QString formatDateSeparator(QDate date) const;
    Q_INVOKABLE void fixImageRendering(QQuickTextDocument *t, QQuickItem *i);
    Q_INVOKABLE QColor userColor(QString id, QColor background);
    Q_INVOKABLE void focusMessageInput();
    Q_INVOKABLE QString avatarUrl(const QString &id) const;

protected:
    QHash<int, QByteArray> roleNames() const;

public slots:
    void add(TimelineItem &item);
    void add(QList<TimelineItem> &items);
    void remove(const QStringList &ids);
    void send(const QString &message);

signals:
    void rowCountChanged();
    void typingUsersChanged(const QString &text);
    void focusInput();

private:
    void add(const QVector<DescInfo> &items);
    QList<TimelineItem> _timelineItems;
    QStringList _messageIds;
    Timeline *_timeline = nullptr;
    QHash<QPair<QString, quint64>, QColor> userColors;
};
#endif
