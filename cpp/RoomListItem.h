#ifndef ROOM_LIST_ITEM_H
#define ROOM_LIST_ITEM_H

#include <QObject>
#include <matrix-client-library/Client.h>

class RoomInformation :public QObject {
Q_OBJECT
public:
    RoomInformation(){};
    RoomInformation(const QString &id, const QString &name, const QString &avatar, bool invite, int unreadCount):
        _id(id), _name(name), _avatar(avatar), _invite(invite), _unreadCount(unreadCount){};
    RoomInformation(const QString &id, int unreadCount):
        _id(id), 
        _name(Client::instance()->roomInfo(_id).name),
        _unreadCount(unreadCount) {};
    
    Q_INVOKABLE QString id() {return _id;};
    Q_INVOKABLE QString name() {return _name;};
    Q_INVOKABLE QString avatar() {return Client::instance()->roomInfo(_id).avatar_url;};
    Q_INVOKABLE bool    invite() {return Client::instance()->roomInfo(_id).is_invite;};
    Q_INVOKABLE QString lastmessage() {return _lastmessage;};
    Q_INVOKABLE int     memberCount() {return Client::instance()->roomInfo(_id).member_count;};
    Q_INVOKABLE QString topic() {return Client::instance()->roomInfo(_id).topic;};
    Q_INVOKABLE QString version() {return Client::instance()->roomInfo(_id).version;};
    Q_INVOKABLE bool    guestAccess() {return Client::instance()->roomInfo(_id).guest_access;};
    Q_INVOKABLE int     unreadCount() {return _unreadCount;};

    void setId(const QString &id) {_id = id;};
    void setName(const QString &name) {_name = name;};
    void setAvatar(const QString &avatar) {_avatar = avatar;};
    void setInvite(const bool &invite) {_invite = invite;};
    void setLastMessage(const QString &lastmessage) {_lastmessage = lastmessage;};
    void setMemberCount(const int &memberCount) {_memberCount = memberCount;};
    void setTopic(const QString &topic) {_topic = topic;};
    void setVersion(const QString &version) {_version = version;};
    void setGuestAccess(const bool &guestAccess) {_guestAccess = guestAccess;};
    void setUnreadCount(const int &unreadCount) {_unreadCount = unreadCount;};

private:
    QString _id             = "";
    QString _name           = "";
    QString _avatar         = "";
    bool    _invite         = false;
    QString _lastmessage    = "";
    int     _memberCount    = 0;
    QString _topic          = "";
    QString _version        = "";
    bool    _guestAccess    = false;
    int     _unreadCount    = 0;
};

class RoomListItem{
public: 
    RoomListItem(const QString &id, const QString &name, const QString &avatar, bool invite, int unreadCount = 0);
    RoomListItem(const QString &id, int unreadCount = 0);

    QString id() const;
    void setId(const QString &id);

    QString name() const;
    void setName(const QString &name);

    QString avatar() const;
    void setAvatar(const QString &avatar);

    QString lastMessage() const;
    void setLastMessage(const QString &message);
        
    bool    invite() const;
    void setInvite(bool invite);
    
    int    unreadCount() const;
    void setUnreadCount(int unreadCount);
    
    int memberCount() const;
    void setMemberCount(int memberCount);

    QString topic() const;
    void setTopic(const QString &topic);

    QString version() const;
    void setVersion(const QString &version);

    bool guestAccess() const;
    void setGuestAccess(bool access);

    RoomInformation *roomInformation() const;
    Timeline *timeline();

    QString toString();

private:
    RoomInformation *_roomInformation = nullptr;
};
#endif // ROOM_LIST_ITEM_H
