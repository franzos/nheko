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

bool RoomListModel::setData(const QModelIndex &index,
                              const QVariant &value, int role)
{
    RoomListItem room = _roomListItems[index.row()];

    if(role == idRole){
        room.setId(value.toString());
        emit dataChanged(index, index);
        return true;
    }
    else if(role == nameRole){
        room.setName(value.toString());
        emit dataChanged(index, index);
        return true;
    }
    else if(role == avatarRole){
        room.setAvatar(value.toString());
        emit dataChanged(index, index);
        return true;
    }
    else if(role == inviteRole){
        room.setInvite(value.toBool());
        emit dataChanged(index, index);
        return true;
    }
    return false;
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
    }

    endRemoveRows();
    return true;
}

void RoomListModel::add(QList<RoomListItem> &rooms){
    if(rooms.size()){
        beginInsertRows(QModelIndex(), rowCount(), rowCount() + rooms.size() - 1);
        _roomListItems << rooms;
        endInsertRows();
        for(auto &r: rooms){
            qDebug() << "Added to RoomList: " << r.toString();
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
