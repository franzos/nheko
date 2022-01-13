#include <QDebug>
#include "RoomListItem.h"

RoomListItem::RoomListItem( const QString &id, 
                            const QString &name, 
                            const QString &avatar, 
                            bool invite, 
                            int unreadCount):
    _id(id), _name(name), _avatar(avatar), _invite(invite), _unreadCount(unreadCount)
    {}

QString RoomListItem::id() const{
    return _id;
}

void RoomListItem::setId(const QString &id){
    if (id != _id) {
        _id = id;
    }
}

QString RoomListItem::name() const {
    return _name;
}

void RoomListItem::setName(const QString &name){
    if (name != _name) {
        _name = name;
    }
}

QString RoomListItem::avatar() const {
    return _avatar;
}

void RoomListItem::setAvatar(const QString &avatar){
    if (avatar != _avatar) {
        _avatar= avatar;
    }
}

QString RoomListItem::lastMessage() const {
    return _lastmessage;
}

void RoomListItem::setLastMessage(const QString &message){
    if (message != _lastmessage) {
        _lastmessage= message;
    }
}

bool    RoomListItem::invite() const{
    return _invite;
}

void RoomListItem::setInvite(bool invite){
    if (invite != _invite) {
        _invite= invite;
    }
}

void RoomListItem::setUnreadCount(int unreadCount){
    if (unreadCount != _unreadCount) {
        _unreadCount= unreadCount;
    }
}

int RoomListItem::unreadCount() const{
    return _unreadCount;
}

QString RoomListItem::toString(){
    return "{\"ID\":\""     + _id   + "\"," +
            "\"Name\":\""   + _name + "\"," +
            "\"Avatar\":\"" + _avatar + "\"," +
            "\"Last Message\":\"" + _lastmessage + "\"," +
            "\"Unread counts\":\"" + QString::number(_unreadCount) + "\"," +
            "\"Status\":\"" + ((_invite) ? "Invite" : "Joined") + "\"}";
}