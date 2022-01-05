#include <QDebug>
#include "RoomItem.h"

RoomItem::RoomItem(QObject *parent)
    : QObject(parent)
{
}

RoomItem::RoomItem(const QString &id, const QString &name, const QString &avatar, QObject *parent)
    : QObject(parent), _id(id), _name(name), _avatar(avatar)
{
}

QString RoomItem::id() const
{
    return _id;
}

void RoomItem::setId(const QString &id)
{
    if (id != _id) {
        _id = id;
        emit idChanged();
    }
}

QString RoomItem::name() const
{
    return _name;
}

void RoomItem::setName(const QString &name)
{
    if (name != _name) {
        _name = name;
        emit nameChanged();
    }
}

QString RoomItem::avatar() const
{
    return _avatar;
}

void RoomItem::setAvatar(const QString &avatar)
{
    if (avatar != _avatar) {
        _avatar= avatar;
        emit avatarChanged();
    }
}
