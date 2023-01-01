#ifndef ROOMLISTMODEL_H
#define ROOMLISTMODEL_H

#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include <QObject>
#include <QStringList>
#include <matrix-client-library/Client.h>
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
        lastmessageTimeRole,
        timestampRole,
        unreadcountRole,
        memberCountRole,
        topicRole,
        versionRole,
        guestAccessRole,
        updateallRole
    };

    RoomListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QVariant headerData(int section, Qt::Orientation orientation,int role = Qt::DisplayRole) const override;
    bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;
    int  roomidToIndex(const QString &roomid);
    void cleanup();

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
    QMap<QString, TimelineModel *> _timelines;
};

class FilteredRoomlistModel : public QSortFilterProxyModel
{
    Q_OBJECT
    // Q_PROPERTY(
    //   TimelineModel *currentRoom READ currentRoom NOTIFY currentRoomChanged RESET resetCurrentRoom)
    // Q_PROPERTY(RoomPreview currentRoomPreview READ currentRoomPreview NOTIFY currentRoomChanged
    //              RESET resetCurrentRoom)
public:
    FilteredRoomlistModel(RoomListModel *model, QObject *parent = nullptr);
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
    // bool filterAcceptsRow(int sourceRow, const QModelIndex &) const override;

public slots:
    TimelineModel *timelineModel(const QString &roomId);
    RoomInformation *roomInformation(const QString &roomId);
//     int roomidToIndex(QString roomid)
//     {
//         return mapFromSource(roomlistmodel->index(roomlistmodel->roomidToIndex(roomid))).row();
//     }
//     void joinPreview(QString roomid) { roomlistmodel->joinPreview(roomid); }
//     void acceptInvite(QString roomid) { roomlistmodel->acceptInvite(roomid); }
//     void declineInvite(QString roomid) { roomlistmodel->declineInvite(roomid); }
//     void leave(QString roomid, QString reason = "") { roomlistmodel->leave(roomid, reason); }
//     void toggleTag(QString roomid, QString tag, bool on);
//     void copyLink(QString roomid);

//     TimelineModel *currentRoom() const { return roomlistmodel->currentRoom(); }
//     RoomPreview currentRoomPreview() const { return roomlistmodel->currentRoomPreview(); }
//     void setCurrentRoom(QString roomid) { roomlistmodel->setCurrentRoom(std::move(roomid)); }
//     void resetCurrentRoom() { roomlistmodel->resetCurrentRoom(); }
//     TimelineModel *getRoomById(const QString &id) const
//     {
//         auto r = roomlistmodel->getRoomById(id).data();
//         QQmlEngine::setObjectOwnership(r, QQmlEngine::CppOwnership);
//         return r;
//     }
//     RoomPreview getRoomPreviewById(QString roomid) const
//     {
//         return roomlistmodel->getRoomPreviewById(roomid);
//     }

//     void nextRoomWithActivity();
//     void nextRoom();
//     void previousRoom();

//     void updateFilterTag(QString tagId)
//     {
//         if (tagId.startsWith(QLatin1String("tag:"))) {
//             filterType = FilterBy::Tag;
//             filterStr  = tagId.mid(4);
//         } else if (tagId.startsWith(QLatin1String("space:"))) {
//             filterType = FilterBy::Space;
//             filterStr  = tagId.mid(6);
//             roomlistmodel->fetchPreviews(filterStr);
//         } else if (tagId.startsWith(QLatin1String("dm"))) {
//             filterType = FilterBy::DirectChats;
//             filterStr.clear();
//         } else {
//             filterType = FilterBy::Nothing;
//             filterStr.clear();
//         }

//         invalidateFilter();
//     }

//     void updateHiddenTagsAndSpaces();

// signals:
//     void currentRoomChanged();

private:
    short int calculateImportance(const QModelIndex &idx) const;
    RoomListModel *roomlistmodel;
    bool sortByImportance = true;

//     enum class FilterBy
//     {
//         Tag,
//         Space,
//         DirectChats,
//         Nothing,
//     };
//     QString filterStr   = QLatin1String("");
//     FilterBy filterType = FilterBy::Nothing;
//     QStringList hiddenTags, hiddenSpaces;
//     bool hideDMs = false;
};

#endif
