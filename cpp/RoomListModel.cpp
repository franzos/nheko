#include "RoomListModel.h"
#include <QDebug>
#include <QQmlEngine>

int RoomListModel::rowCount(const QModelIndex &parent) const
{
    (void)parent;
    return _roomListItems.count();
}
RoomListModel::RoomListModel(QObject *parent)
    : QAbstractListModel(parent) {
    
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
    else if (role == timestampRole)
        return QVariant{static_cast<quint64>(room.timestamp())};
    else if (role == unreadcountRole)
        return room.unreadCount();
    else if (role == memberCountRole)
        return room.memberCount();
    else if (role == topicRole)
        return room.topic();
    else if (role == versionRole)
        return room.version();
    else if (role == guestAccessRole)
        return room.guestAccess();
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
    roles[memberCountRole] = "memberCount";
    roles[topicRole] = "topic";
    roles[versionRole] = "version";
    roles[guestAccessRole] = "guestaccess";
    roles[updateallRole] = "updateall";
    return roles;
}

int RoomListModel::roomidToIndex(const QString &roomid){
    for (int i = 0; i < (int)_roomListItems.size(); i++) {
        if (_roomListItems[i].id() == roomid)
            return i;
    }
    return -1;
}

void RoomListModel::cleanup(){
    for(auto const &t: _timelines){
        t->deleteLater();
    }
    _timelines.clear();
}

bool RoomListModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    (void)parent;
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
        if(idx == -1){
            return;
        }

        setData(index(idx), "", updateallRole);
    } else if(!_roomIds.contains(item.id())) {
        // add new room [room events]
        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        auto timeline = Client::instance()->timeline(item.id());
        if(timeline){
            QString roomID = item.id();
            connect(timeline, &Timeline::lastMessageChanged,[this,roomID, timeline](const DescInfo &e){
                auto idx = this->roomidToIndex(roomID);
                if(idx != -1) {
                    qDebug() << "New event recieved from in " << roomID;
                    QString body = e.body;
                    body.remove(QRegExp("[\\n\\t\\r]"));
                    
                    this->setData(this->index(idx), body, lastmessageRole);
                    this->setData(this->index(idx), static_cast<quint64>(e.timestamp), timestampRole);
                }
            });
            connect(timeline, &Timeline::notificationsChanged,[this,roomID, timeline](){
                auto idx = this->roomidToIndex(roomID);
                if(idx != -1) {
                    qDebug() << "Notification counter changed in " << roomID;
                    this->setData(this->index(idx), timeline->notificationCount(), unreadcountRole);
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
        QVector<int> roles;
        switch (role) {
        case lastmessageRole:
            item.setLastMessage(value.toString());
            roles << role;
            break;
        case unreadcountRole:
            item.setUnreadCount(value.toInt());
            roles << role;
            break;
        case updateallRole:
            item.roomInformation()->update();
            roles << nameRole << avatarRole << inviteRole << lastmessageRole << unreadcountRole \
             << memberCountRole << topicRole << versionRole << guestAccessRole;
            break;
        case timestampRole:
            item.setTimestamp(value.toInt());
            roles << role;
            break;
        default:
            return false;
        }

        _roomListItems.replace(index.row(), item);

        emit dataChanged(index, index, roles);
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
    auto model = new TimelineModel(roomId, this);
    _timelines << model;
    QQmlEngine::setObjectOwnership(model, QQmlEngine::CppOwnership);
    return model;
}

RoomInformation *RoomListModel::roomInformation(const QString &roomId){ 
    for(auto const &r: _roomListItems){
        if(r.id() == roomId) {
            QQmlEngine::setObjectOwnership(r.roomInformation(), QQmlEngine::CppOwnership);
            return r.roomInformation();
        }
    }
    return nullptr;
}

bool FilteredRoomlistModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    QModelIndex const left_idx = sourceModel()->index(left.row(), 0, QModelIndex());
    QModelIndex const right_idx = sourceModel()->index(right.row(), 0, QModelIndex());

    // Sort by "importance" (i.e. invites before mentions before
    // notifs before new events before old events), then secondly
    // by recency.

    // Checking importance first
    // const auto a_importance = calculateImportance(left_idx);
    // const auto b_importance = calculateImportance(right_idx);
    // if (a_importance != b_importance) {
    //     return a_importance > b_importance;
    // }

    // Now sort by recency
    // Zero if empty, otherwise the time that the event occured
    uint64_t a_recency = sourceModel()->data(left_idx, RoomListModel::timestampRole).toULongLong();
    uint64_t b_recency = sourceModel()->data(right_idx, RoomListModel::timestampRole).toULongLong();
    if (a_recency != b_recency)
        return a_recency > b_recency;
    else
        return left.row() < right.row();
}

FilteredRoomlistModel::FilteredRoomlistModel(RoomListModel *model, QObject *parent)
    : QSortFilterProxyModel(parent), roomlistmodel(model)
{
    // this->sortByImportance = UserSettings::instance()->sortByImportance();
    setSourceModel(model);
    setDynamicSortFilter(true);

    // QObject::connect(UserSettings::instance().get(),
    //                  &UserSettings::roomSortingChanged,
    //                  this,
    //                  [this](bool sortByImportance_) {
    //                      this->sortByImportance = sortByImportance_;
    //                      invalidate();
    //                  });

    // connect(roomlistmodel,
    //         &RoomListModel::currentRoomChanged,
    //         this,
    //         &FilteredRoomlistModel::currentRoomChanged);

    sort(0);
}

TimelineModel *FilteredRoomlistModel::timelineModel(const QString &roomId) {
    return roomlistmodel->timelineModel(roomId);
}

RoomInformation *FilteredRoomlistModel::roomInformation(const QString &roomId) {
    return roomlistmodel->roomInformation(roomId);
}