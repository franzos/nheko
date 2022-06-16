#ifndef ROOMLISTMODEL_H
#define ROOMLISTMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QStringList>
#include <matrix-client-library/Client.h>
#include <QDBusConnection>
#include "RoomListItem.h"
#include "TimelineModel.h"

class RoomListModel : public QAbstractListModel{
    Q_OBJECT

    Q_PROPERTY(int rowCount READ rowCount NOTIFY rowCountChanged)
public:
    enum RoomListItemRoles {
        idRole = Qt::UserRole + 1,
        nameRole,
        avatarRole,
        inviteRole,
        lastmessageRole,
        unreadcountRole,
        memberCountRole,
        topicRole,
        versionRole,
        guestAccessRole
    };

    RoomListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QVariant headerData(int section, Qt::Orientation orientation,int role = Qt::DisplayRole) const override;
    bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;
    int  roomidToIndex(const QString &roomid);

public slots:
    void add(RoomListItem &item);
    void add(QList<RoomListItem> &items);
    void remove(const QStringList &ids);
    TimelineModel *timelineModel(const QString &roomId);
    RoomInformation *roomInformation(const QString &roomId);

signals:
    void rowCountChanged();

protected:
    QHash<int, QByteArray> roleNames() const;
    QList<RoomListItem> _roomListItems;
    QStringList _roomIds;
};
#endif
