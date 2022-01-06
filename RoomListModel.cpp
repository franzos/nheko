#include "RoomListModel.h"
#include <QDebug>
int RoomListModel::rowCount(const QModelIndex &parent) const
{
    return _roomList.count();
}

QVariant RoomListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= _roomList.size())
        return QVariant();

    if (role == Qt::DisplayRole)
        return _roomList.at(index.row());
    else
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
    if (index.isValid() && role == Qt::EditRole) {

        _roomList.replace(index.row(), value.toString());
        emit dataChanged(index, index);
        return true;
    }
 
    return false;
}

bool RoomListModel::insertRows(int position, int rows, const QModelIndex &parent) {
    beginInsertRows(QModelIndex(), position, position+rows-1);

    for (int row = 0; row < rows; ++row) {
        _roomList.insert(position, "");
    }

    endInsertRows();
    return true;
}

bool RoomListModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    beginRemoveRows(QModelIndex(), position, position+rows-1);

    for (int row = 0; row < rows; ++row) {
        _roomList.removeAt(position);
        _roomIds.removeAt(position);
    }

    endRemoveRows();
    return true;
}

int RoomListModel::roomidToIndex(const QString &roomid){
    for (int i = 0; i < (int)_roomIds.size(); i++) {
        if (_roomIds[i] == roomid)
            return i;
    }
    return -1;
}

void RoomListModel::add(RoomListItem &room){
    if(!_roomIds.contains(room.id()) && !room.id().isEmpty()){
        _roomIds.push_back(room.id());
        int row = rowCount();
        insertRows( rowCount(), 
                    1, 
                    QModelIndex());
        QModelIndex index = this->index(row , 0, QModelIndex());
        setData(index, room.name());
        qDebug() << "Added to RoomList: " << room.toString();
    }
}

void RoomListModel::add(QVector<RoomListItem> &rooms){
    for(auto &r: rooms){
        add(r);
    }
}

void RoomListModel::remove(const QStringList &ids){
    for(auto const &id: ids){
        if(_roomIds.contains(id)){
            auto idx = roomidToIndex(id);
            if (idx != -1) {
                if(removeRows(idx,1)){
                    qDebug() << "Removed from RoomList: " << "Room ID: " << id;
                }
            }
        }
    }
}

RoomListItem::RoomListItem(const QString &id, const QString &name, const QString &avatar, bool invite):
    _id(id), _name(name), _avatar(avatar), _invite(invite)
    {}

QString RoomListItem::id(){
    return _id;
}

QString RoomListItem::name(){
    return _name;
}

QString RoomListItem::avatar(){
    return _avatar;
}

bool    RoomListItem::invite(){
    return _invite;
}

QString RoomListItem::toString(){
    return "{\"ID\":\""     + _id   + "\"," +
            "\"Name\":\""   + _name + "\"," +
            "\"Avatar\":\"" + _avatar + "\"," +
            "\"Status\":\"" + ((_invite) ? "Invite" : "Joined") + "\"}";
}