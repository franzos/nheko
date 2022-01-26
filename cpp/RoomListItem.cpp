#include <QDebug>
#include "RoomListItem.h"

RoomListItem::RoomListItem( const QString &id, 
                            const QString &name, 
                            const QString &avatar, 
                            bool invite, 
                            int unreadCount):
    _roomInformation(new RoomInformation(id, name, avatar, invite, unreadCount))
    {}

RoomListItem::RoomListItem(const QString &id, const RoomInfo &roomInfo, int unreadCount):
    _roomInformation(new RoomInformation(id, roomInfo, unreadCount))
    {}

QString RoomListItem::id() const{
    return _roomInformation->id();
}

void RoomListItem::setId(const QString &id){
    _roomInformation->setId(id);
}

QString RoomListItem::name() const {
    return _roomInformation->name();
}

void RoomListItem::setName(const QString &name){
    _roomInformation->setName(name);
}

QString RoomListItem::avatar() const {
    return _roomInformation->avatar();
}

void RoomListItem::setAvatar(const QString &avatar){
    _roomInformation->setAvatar(avatar);
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

void RoomListItem::setInvite(bool invite) {
    _roomInformation->setInvite(invite);
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

void RoomListItem::setMemberCount(int memberCount) {
    _roomInformation->setMemberCount(memberCount);
}

QString RoomListItem::topic() const{
    return _roomInformation->topic();
}

void RoomListItem::setTopic(const QString &topic) {
    _roomInformation->setTopic(topic);
}

QString RoomListItem::version() const{
    return _roomInformation->version();
}

void RoomListItem::setVersion(const QString &version){
    _roomInformation->setVersion(version);
}

bool RoomListItem::guestAccess() const{
    return _roomInformation->guestAccess();
}

void RoomListItem::setGuestAccess(bool access) {
    _roomInformation->setGuestAccess(access);
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