#ifndef ROOM_LIST_ITEM_H
#define ROOM_LIST_ITEM_H

#include <QObject>

class RoomListItem {
public: 
    RoomListItem(const QString &id, const QString &name, const QString &avatar, bool invite);
    RoomListItem();

    QString id() const;
    void setId(const QString &id);

    QString name() const;
    void setName(const QString &name);

    QString avatar() const;
    void setAvatar(const QString &avatar);
        
    void setInvite(bool invite);
    bool    invite() const;
    QString toString();

private:
    QString _id;
    QString _name;
    QString _avatar;
    bool _invite;
};
#endif // ROOM_LIST_ITEM_H
