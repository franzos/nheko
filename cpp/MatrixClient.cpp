#include "MatrixClient.h"
#include <QCoreApplication>
#include <QQuickStyle>
#include "TimelineModel.h"

MatrixClient::MatrixClient(const QUrl &url, QObject *parent): 
    QObject(parent),
    _mainUrl(url),
    _roomListModel(new RoomListModel({})),
    _client(Client::instance()){
    
    _client->enableLogger(true, true);
    connect(_client, &Client::newUpdated,this, &MatrixClient::newSyncCb);
    connect(_client, &Client::initiateFinished,this, &MatrixClient::initiateFinishedCB);
    qmlRegisterType<TimelineModel>("TimelineModel", 1, 0, "TimelineModel");
    qmlRegisterType<RoomInformation>("RoomInformation", 1, 0, "RoomInformation");
    qmlRegisterSingletonInstance<Client>("MatrixClient", 1, 0, "MatrixClient", _client);
    qmlRegisterSingletonType<RoomListModel>("Rooms", 1, 0, "Rooms", [&](QQmlEngine *, QJSEngine *) -> QObject * {
        return _roomListModel;
    });

    connect(&_engine, &QQmlApplicationEngine::objectCreated,
                     QCoreApplication::instance(), [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl) {
            QCoreApplication::instance()->exit(-1);
        }
    }, Qt::QueuedConnection);
}

void MatrixClient::load(){
    _engine.load(_mainUrl);
}

void MatrixClient::newSyncCb(const mtx::responses::Sync &sync){
    auto rooms = sync.rooms;
    QList<RoomListItem> roomList;
    for(auto const &r: rooms.join){
        auto roomInfo = _client->roomInfo(QString::fromStdString(r.first));
        RoomListItem room(  QString::fromStdString(r.first),
                            roomInfo,
                            r.second.unread_notifications.notification_count);
        roomList << room;
    }

    for(auto const &r: rooms.invite){
        RoomListItem room(  QString::fromStdString(r.first),
                            QString::fromStdString(r.second.name()),
                            QString::fromStdString(r.second.avatar()),
                            true);
        roomList << room;
    }
    _roomListModel->add(roomList);
    
    QStringList leaveRooms;
    for(auto const &r: rooms.leave){
        leaveRooms << QString::fromStdString(r.first);
    }
    _roomListModel->remove(leaveRooms);
}

void MatrixClient::initiateFinishedCB(){auto joinedRooms = _client->joinedRoomList();
    auto inviteRooms = _client->inviteRoomList();
    QList<RoomListItem> roomList;
    for(auto const &r: joinedRooms.toStdMap()){
        RoomListItem room(  r.first,
                            r.second);
        roomList << room;
    }

    for(auto const &r: inviteRooms.toStdMap()){
        RoomListItem room(  r.first,
                            r.second);
        roomList << room;
    }
    _roomListModel->add(roomList);
}
