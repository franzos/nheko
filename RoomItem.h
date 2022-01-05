#ifndef ROOM_ITEM_H
#define ROOM_ITEM_H

#include <QObject>

class RoomItem : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString id     READ id      WRITE setId     NOTIFY idChanged)
    Q_PROPERTY(QString name   READ name    WRITE setName   NOTIFY nameChanged)
    Q_PROPERTY(QString avatar READ avatar  WRITE setAvatar NOTIFY avatarChanged)

public:
    RoomItem(QObject *parent=0);
    RoomItem(const QString &id, const QString &name, const QString &avatar, QObject *parent=0);

    QString id() const;
    void setId(const QString &id);

    QString name() const;
    void setName(const QString &name);

    QString avatar() const;
    void setAvatar(const QString &avatar);

signals:
    void idChanged();
    void nameChanged();
    void avatarChanged();

private:
    QString _id;
    QString _name;
    QString _avatar;
};

#endif // ROOM_ITEM_
