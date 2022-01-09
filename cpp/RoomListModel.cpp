#include "RoomListModel.h"
#include <QDebug>
int RoomListModel::rowCount(const QModelIndex &parent) const
{
    return _roomListItems.count();
}

QVariant RoomListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= _roomListItems.size())
        return QVariant();

    const RoomListItem room = _roomListItems.at(index.row()); // TODO
    if (role == idRole)
        return room.id();
    else if (role == nameRole)
        return room.name();
    else if (role == avatarRole)
        return room.avatar();
    else if (role == inviteRole)
        return room.invite();
    return QVariant();
}

QVariant RoomListModel::headerData(int section, Qt::Orientation orientation,
                                     int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        return QString("Column %1").arg(section);
    else
        return QString("Row %1").arg(section);
}

Qt::ItemFlags RoomListModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

QHash<int, QByteArray> RoomListModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[idRole] = "id";
    roles[nameRole] = "name";
    roles[avatarRole] = "avatar";
    roles[inviteRole] = "invite";
    return roles;
}

int RoomListModel::roomidToIndex(const QString &roomid){
    for (int i = 0; i < (int)_roomListItems.size(); i++) {
        if (_roomListItems[i].id() == roomid)
            return i;
    }
    return -1;
}

bool RoomListModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    beginRemoveRows(QModelIndex(), position, position+rows-1);
    for (int row = 0; row < rows; ++row) {
        _roomListItems.removeAt(position);
        _roomIds.removeAt(position);
    }
    endRemoveRows();
    return true;
}

void RoomListModel::add(RoomListItem &item){
    if(_roomIds.contains(item.id())){
        auto idx = roomidToIndex(item.id());
        if (!item.invite() &&  _roomListItems.at(idx).invite()){
            // invited --> join [room events]
            // TODO edit data and emit
            remove({item.id()});
            add({item});
        }
    } else if(!_roomIds.contains(item.id())){
        // add new room [room events]
        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        _roomListItems << item;
        _roomIds << item.id();
        endInsertRows();
        qDebug() << "Added to RoomList: " << item.toString();
    }
}

void RoomListModel::add(QList<RoomListItem> &rooms){
    if(rooms.size()){
        for(auto &r: rooms){
            add(r);
        }
    }
}

void RoomListModel::remove(const QStringList &ids){
    for(auto const &id: ids){
        auto idx = roomidToIndex(id);
        if (idx != -1) {
            if(removeRows(idx,1)){
                qDebug() << "Removed from RoomList: " << "Room ID: " << id;
            }
        }
    }
}
