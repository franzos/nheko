#ifndef ROOM_LIST_ITEM_H
#define ROOM_LIST_ITEM_H

#include <QObject>
#include <matrix-client-library/Client.h>

class RoomInformation :public QObject {
Q_OBJECT
public:
    RoomInformation(){};
    RoomInformation(const QString &id, const RoomInfo &roomInfo, int unreadCount):
        _id(id), 
        _unreadCount(unreadCount),
        _roominfo(roomInfo){};
    
    Q_INVOKABLE QString     id() {return _id;};
    Q_INVOKABLE QString     name() {return _roominfo.name;};
    Q_INVOKABLE QString     avatar() {return _roominfo.avatar_url;};
    Q_INVOKABLE bool        invite() {return _roominfo.is_invite;};
    Q_INVOKABLE QString     lastmessage() {return _lastmessage;};
    Q_INVOKABLE int         memberCount() {return _roominfo.member_count;};
    Q_INVOKABLE QString     topic() {return _roominfo.topic;};
    Q_INVOKABLE QString     version() {return _roominfo.version;};
    Q_INVOKABLE bool        guestAccess() {return _roominfo.guest_access;};
    Q_INVOKABLE int         unreadCount() {return _unreadCount;};
    Q_INVOKABLE uint64_t    timestamp() {return _timestamp;};

    void setLastMessage(const QString &lastmessage) {_lastmessage = lastmessage;};
    void setUnreadCount(const int &unreadCount) {_unreadCount = unreadCount;};
    void setTimestamp  (const uint64_t ts) {_timestamp = ts;};
    void update() {
        if(invite()){
            auto newRoomInfo = cache::client()->invite(_id.toStdString());
            if (newRoomInfo){
                _roominfo = *newRoomInfo;
                return;
            }
        }
        _roominfo = Client::instance()->roomInfo(_id);
    };
private:
    QString     _id             = "";
    QString     _lastmessage    = "";
    int         _unreadCount    = 0;
    uint64_t    _timestamp     = 0;
    RoomInfo    _roominfo;
};

class RoomListItem{
public: 
    RoomListItem(const QString &id, const RoomInfo &roomInfo, int unreadCount = 0);

    QString id() const;
    QString name() const;
    QString avatar() const;
    QString lastMessage() const;
    void setLastMessage(const QString &message);
    bool invite() const;    
    int  unreadCount() const;
    void setUnreadCount(int unreadCount);
    int  memberCount() const;
    QString topic() const;
    QString version() const;
    bool guestAccess() const;
    uint64_t timestamp() const;
    void setTimestamp(uint64_t ts) const;
    RoomInformation *roomInformation() const;
    Timeline *timeline();
    QString toString();

private:
    RoomInformation *_roomInformation = nullptr;
};
#endif // ROOM_LIST_ITEM_H
