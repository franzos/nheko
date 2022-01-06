#include <QDebug>
#include "RoomListItem.h"

RoomListItem::RoomListItem(const QString &id, const QString &name, const QString &avatar, bool invite):
    _id(id), _name(name), _avatar(avatar), _invite(invite)
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

bool    RoomListItem::invite() const{
    return _invite;
}

void RoomListItem::setInvite(bool invite){
    if (invite != _invite) {
        _invite= invite;
    }
}
QString RoomListItem::toString(){
    return "{\"ID\":\""     + _id   + "\"," +
            "\"Name\":\""   + _name + "\"," +
            "\"Avatar\":\"" + _avatar + "\"," +
            "\"Status\":\"" + ((_invite) ? "Invite" : "Joined") + "\"}";
}