#include "MatrixClient.h"
#include <QCoreApplication>
#include <QQuickStyle>
#include <matrix-client-library/encryption/DeviceVerificationFlow.h>
#include <matrix-client-library/UIA.h>
#include "TimelineModel.h"

MatrixClient::MatrixClient(QObject *parent): 
    QObject(parent),
    _roomListModel(new RoomListModel({})),
    _client(Client::instance()),
    _verificationManager(_client->verificationManager()){
    
    _client->enableLogger(true, true);
    connect(_client, &Client::newUpdated,this, &MatrixClient::newSyncCb);
    connect(_client, &Client::initiateFinished,this, &MatrixClient::initiateFinishedCB);
    connect(_client, &Client::logoutOk,[&](){
        _roomListModel->removeRows(0,_roomListModel->rowCount());
    });
    qmlRegisterType<TimelineModel>("TimelineModel", 1, 0, "TimelineModel");
    qmlRegisterType<RoomInformation>("RoomInformation", 1, 0, "RoomInformation");
    qmlRegisterSingletonInstance<Client>("MatrixClient", 1, 0, "MatrixClient", _client);
    qmlRegisterSingletonInstance<UIA>("UIA", 1, 0, "UIA", UIA::instance());
    qmlRegisterUncreatableType<DeviceVerificationFlow>("DeviceVerificationFlow", 1, 0, "DeviceVerificationFlow", "Can't create verification flow from QML!");
    qmlRegisterSingletonInstance<VerificationManager>("VerificationManager", 1, 0, "VerificationManager", _verificationManager);
    qmlRegisterSingletonInstance<SelfVerificationStatus>("SelfVerificationStatus", 1, 0, "SelfVerificationStatus", _verificationManager->selfVerificationStatus());
    qmlRegisterSingletonType<RoomListModel>("Rooms", 1, 0, "Rooms", [&](QQmlEngine *, QJSEngine *) -> QObject * {
        return _roomListModel;
    });
}

QUrl MatrixClient::mainLibQMLurl(){
    return QUrl(QStringLiteral("qrc:/qml/MainLib.qml"));
}

QUrl MatrixClient::mainAppQMLurl(){
    return QUrl(QStringLiteral("qrc:/qml/main.qml"));
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

MatrixClientQmlApplicationEngine::MatrixClientQmlApplicationEngine(QObject *parent): 
    MatrixClient(parent), QQmlApplicationEngine(parent){            
    // connect(this, &QQmlApplicationEngine::objectCreated,
    //             QCoreApplication::instance(), [&](QObject *obj, const QUrl &objUrl) {
    //     if (!obj && mainAppQMLurl() == objUrl) {
    //         QCoreApplication::instance()->exit(-1);
    //     }
    // }, Qt::QueuedConnection);
}

void MatrixClientQmlApplicationEngine::load(){
    QQmlApplicationEngine::load(mainAppQMLurl());
}

MatrixClientQuickView::MatrixClientQuickView(QWindow *parent): 
    MatrixClient(parent) ,QQuickView(mainLibQMLurl(), parent){
}
