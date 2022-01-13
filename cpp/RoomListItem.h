#ifndef ROOM_LIST_ITEM_H
#define ROOM_LIST_ITEM_H

#include <QObject>
#include <matrix-client-library/Client.h>

class RoomListItem {
public: 
    RoomListItem(const QString &id, const QString &name, const QString &avatar, bool invite, int unreadCount = 0);
    RoomListItem();

    QString id() const;
    void setId(const QString &id);

    QString name() const;
    void setName(const QString &name);

    QString avatar() const;
    void setAvatar(const QString &avatar);

    QString lastMessage() const;
    void setLastMessage(const QString &message);
        
    void setInvite(bool invite);
    bool    invite() const;
    
    void setUnreadCount(int unreadCount);
    int    unreadCount() const;
    
    QString toString();

private:
    QString _id;
    QString _name;
    QString _avatar;
    bool    _invite;
    QString _lastmessage;
    int     _unreadCount;
};
#endif // ROOM_LIST_ITEM_H
