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
    else if (role == lastmessageRole)
        return room.lastMessage();
    else if (role == unreadcountRole)
        return room.unreadCount();
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
    roles[lastmessageRole] = "lastmessage";
    roles[unreadcountRole] = "unreadcount";
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
        if(_roomListItems.at(idx).invite() && !item.invite()){
            setData(index(idx), false, inviteRole);
        }
    } else if(!_roomIds.contains(item.id())) {
        // add new room [room events]
        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        auto timeline = Client::instance()->timeline(item.id());
        if(timeline){
            QString roomID = item.id();
            connect(timeline, &Timeline::lastMessageChanged,[&,roomID, timeline](const DescInfo &e){
                auto idx = this->roomidToIndex(roomID);
                if(idx != -1) {
                    qDebug() << "New event recieved from in " << roomID;
                    QString body = e.body;
                    if(e.isLocal)
                        body = "You: " + body;
                    else 
                        body = timeline->displayName(e.userid) + ": " + body;
                    setData(index(idx), body, lastmessageRole);
                }
            });
            connect(timeline, &Timeline::notificationsChanged,[&,roomID, timeline](){
                auto idx = this->roomidToIndex(roomID);
                if(idx != -1) {
                    qDebug() << "Notification counter changed in " << roomID;
                    setData(index(idx), timeline->notificationCount(), unreadcountRole);
                }
            });
        }
        _roomListItems << item;
        _roomIds << item.id();
        endInsertRows();
        if(timeline)
            timeline->updateLastMessage();
        qDebug() << "Added to RoomList (" << roomidToIndex(item.id()) << "): " << item.toString();
    }
}

bool RoomListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value && index.isValid()) {
        RoomListItem item = _roomListItems.at(index.row());

        switch (role) {
        case idRole:
            item.setId(value.toString());
            break;
        case nameRole:
            item.setName(value.toString());
            break;
        case avatarRole:
            item.setAvatar(value.toString());
            break;
        case lastmessageRole:
            item.setLastMessage(value.toString());
            break;
        case inviteRole:
            item.setInvite(value.toBool());
            break;
        case unreadcountRole:
            item.setUnreadCount(value.toInt());
            break;
        default:
            return false;
        }

        _roomListItems.replace(index.row(), item);

        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
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

TimelineModel *RoomListModel::timelineModel(const QString &roomId){
    return new TimelineModel(roomId);
}