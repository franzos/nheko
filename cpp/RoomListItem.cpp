#include <QDebug>
#include "RoomListItem.h"

RoomListItem::RoomListItem(const QString &id, const RoomInfo &roomInfo, int unreadCount) : 
    _roomInformation(new RoomInformation(id, roomInfo, unreadCount)){}

QString RoomListItem::id() const{
    return _roomInformation->id();
}

QString RoomListItem::name() const {
    return _roomInformation->name();
}

QString RoomListItem::avatar() const {
    return _roomInformation->avatar();
}

QString RoomListItem::lastMessage() const {
    return _roomInformation->lastmessage();
}

void RoomListItem::setLastMessage(const QString &message){
    _roomInformation->setLastMessage(message);
}

bool    RoomListItem::invite() const{
    return _roomInformation->invite();
}

int RoomListItem::unreadCount() const{
    return _roomInformation->unreadCount();
}

void RoomListItem::setUnreadCount(int unreadCount){
    _roomInformation->setUnreadCount(unreadCount);
}

Timeline *RoomListItem::timeline(){
    return Client::instance()->timeline(_roomInformation->id());
}

int RoomListItem::memberCount() const{
    return _roomInformation->memberCount();
}

QString RoomListItem::topic() const{
    return _roomInformation->topic();
}

QString RoomListItem::version() const{
    return _roomInformation->version();
}

bool RoomListItem::guestAccess() const{
    return _roomInformation->guestAccess();
}

uint64_t RoomListItem::timestamp() const{
    return _roomInformation->timestamp();
}

void RoomListItem::setTimestamp(uint64_t ts) const{
    _roomInformation->setTimestamp(ts);
}

RoomInformation *RoomListItem::roomInformation() const{
    return _roomInformation;
}

QString RoomListItem::toString(){
    return "{\"ID\":\""     + _roomInformation->id()   + "\"," +
            "\"Name\":\""   + _roomInformation->name() + "\"," +
            "\"Avatar\":\"" + _roomInformation->avatar() + "\"," +
            "\"Last Message\":\"" + _roomInformation->lastmessage() + "\"," +
            "\"Unread counts\":\"" + QString::number(_roomInformation->unreadCount()) + "\"," +
            "\"Member counts\":\"" + QString::number(_roomInformation->memberCount()) + "\"," +
            "\"Topic\":\"" + _roomInformation->topic() + "\"," + 
            "\"Version\":\"" + _roomInformation->version() + "\"," +
            "\"Guest Access\":\"" + (_roomInformation->guestAccess() ? "True" : "False") + "\"," +
            "\"Status\":\"" + ((_roomInformation->invite()) ? "Invite" : "Joined") + "\"}";
}