#include "TimelineModel.h"
#include <QDebug>

TimelineModel::~TimelineModel(){
    if(_timeline) {
        disconnect(_timeline, &Timeline::newEventsStored, nullptr, nullptr);
        disconnect(_timeline, &Timeline::typingUsersChanged, nullptr, nullptr);
    }
}

TimelineModel::TimelineModel(const QString roomID , QObject *parent ): QAbstractListModel(parent) {
    _timeline = Client::instance()->timeline(roomID);
    if(_timeline){
        connect(_timeline, &Timeline::newEventsStored, [&](int from, int len){
            add(_timeline->getEvents(from, len));
        });

        connect(_timeline, &Timeline::typingUsersChanged,[&](const QStringList &users){
            QString text = "";
            if(users.size()){
                for(int i = 0; i<users.size(); i++){
                    text += _timeline->displayName(users[i]);
                    if( (i+1) < users.size())
                        text += " and ";
                }
                if(users.size() > 1)
                    text += " are ";
                else 
                    text += " is ";
                text +=  " typing ...";
            }
            emit typingUsersChanged(text);
        });

        add(_timeline->getEvents(0,_timeline->eventSize()));
    }
}

void TimelineModel::add(const QVector<DescInfo> &items){
    for(auto const &e: items){
        TimelineItem item(e.event_id, _timeline->displayName(e.userid), e.body, e.descriptiveTime, e.timestamp, e.isLocal);
        add(item);
    }
}

int TimelineModel::rowCount(const QModelIndex &parent) const
{
    (void)parent;
    return _timelineItems.count();
}

QVariant TimelineModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= _timelineItems.size())
        return QVariant();

    const TimelineItem room = _timelineItems.at(index.row()); // TODO
    if (role == idRole)
        return room.id();
    else if (role == senderIdRole)
        return room.senderId();
    else if (role == bodyRole)
        return room.body();
    else if (role == descriptiveTimeRole)
        return room.descriptiveTime();
    else if (role == timestampRole)
        return room.timestamp();
    else if (role == isLocalRole)
        return room.isLocal();
    return QVariant();
}

QVariant TimelineModel::headerData(int section, Qt::Orientation orientation,
                                     int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        return QString("Column %1").arg(section);
    else
        return QString("Row %1").arg(section);
}

Qt::ItemFlags TimelineModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

QHash<int, QByteArray> TimelineModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[idRole] = "id";
    roles[senderIdRole] = "senderId";
    roles[bodyRole] = "body";
    roles[descriptiveTimeRole] = "descriptiveTime";
    roles[timestampRole] = "timestamp";
    roles[isLocalRole] = "isLocal";
    return roles;
}

int TimelineModel::timelineIdToIndex(const QString &roomid){
    for (int i = 0; i < (int)_timelineItems.size(); i++) {
        if (_timelineItems[i].id() == roomid)
            return i;
    }
    return -1;
}

bool TimelineModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    (void)parent;
    beginRemoveRows(QModelIndex(), position, position+rows-1);
    for (int row = 0; row < rows; ++row) {
        _timelineItems.removeAt(position);
        _messageIds.removeAt(position);
    }
    endRemoveRows();
    return true;
}

void TimelineModel::add(TimelineItem &item){
    // if(_messageIds.contains(item.id())){
    //     auto idx = roomidToIndex(item.id());
    //     if(_timelineItems.at(idx).invite() && !item.invite()){
    //         setData(index(idx), false, inviteRole);
    //     }
    // } else 
    if(!_messageIds.contains(item.id())) {
        // add new room [room events]
        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        _timelineItems << item;
        _messageIds << item.id();
        endInsertRows();
        qDebug() << "Added to Timeline :" <<  item.toString();
    }
}

bool TimelineModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value && index.isValid()) {
        TimelineItem item = _timelineItems.at(index.row());

        switch (role) {
        case idRole:
            item.setId(value.toString());
            break;
        case senderIdRole:
            item.setSenderId(value.toString());
            break;
        case bodyRole:
            item.setBody(value.toString());
            break;
        case descriptiveTimeRole:
            item.setDescriptiveTime(value.toString());
            break;
        case timestampRole:
            item.setTimestamp(value.toUInt());
            break;
        case isLocalRole:
            item.setLocal(value.toBool());
            break;
        default:
            return false;
        }

        _timelineItems.replace(index.row(), item);

        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

void TimelineModel::add(QList<TimelineItem> &rooms){
    if(rooms.size()){
        for(auto &r: rooms){
            add(r);
        }
    }
}

void TimelineModel::remove(const QStringList &ids){
    for(auto const &id: ids){
        auto idx = timelineIdToIndex(id);
        if (idx != -1) {
            if(removeRows(idx,1)){
                qDebug() << "Removed from Timeline: " << id;
            }
        }
    }
}

void TimelineModel::send(const QString &message){
    if(_timeline){
        _timeline->sendMessage(message);
    }
}