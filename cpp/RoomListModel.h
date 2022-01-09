#ifndef ROOMLISTMODEL_H
#define ROOMLISTMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QStringList>

#include "RoomListItem.h"

class RoomListModel : public QAbstractListModel{
    Q_OBJECT

    Q_PROPERTY(int rowCount READ rowCount NOTIFY rowCountChanged)
public:
    enum RoomListItemRoles {
        idRole = Qt::UserRole + 1,
        nameRole,
        avatarRole,
        inviteRole
    };

    RoomListModel(QObject *parent = 0)
        : QAbstractListModel(parent) {}

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation,int role = Qt::DisplayRole) const override;
    bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;
    int  roomidToIndex(const QString &roomid);

protected:
    QHash<int, QByteArray> roleNames() const;

public slots:
    void add(RoomListItem &item);
    void add(QList<RoomListItem> &items);
    void remove(const QStringList &ids);

signals:
    void rowCountChanged();

private:
    QList<RoomListItem> _roomListItems;
    QStringList _roomIds;
};


#endif
