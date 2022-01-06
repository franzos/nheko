#include <QGuiApplication>
#include <QQuickView>
#include <QtQml>
#include <QQuickStyle>
#include <matrix-client-library/Client.h>
#include <QString>
#include <QObject>

#include "RoomListModel.h"
RoomListModel *roomListModel;
Client *client;

int main (int argc, char* argv[]) {
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication q_app (argc, argv);
    qDebug() << QQuickStyle::availableStyles();
    // QQuickStyle::setStyle("Imagine");

    roomListModel = new RoomListModel({});
    client = Client::instance();
    const QUrl url(QStringLiteral("qrc:///qmls/main.qml"));
    client->enableLogger(true, true);

    auto roomUpdateHandler = [&](const mtx::responses::Sync &sync){
        auto rooms = sync.rooms;
        QVector<RoomListItem> roomList;
        for(auto const &r: rooms.join){
            auto roomInfo = client->roomInfo(QString::fromStdString(r.first));
            RoomListItem room(  QString::fromStdString(r.first),
                                roomInfo.name,
                                roomInfo.avatar_url,
                                false);
            roomList.push_back(room);
        }

        for(auto const &r: rooms.invite){
            RoomListItem room(  QString::fromStdString(r.first),
                                QString::fromStdString(r.second.name()),
                                QString::fromStdString(r.second.avatar()),
                                true);
            roomList.push_back(room);
        }
        roomListModel->add(roomList);
        
        QStringList leaveRooms;
        for(auto const &r: rooms.leave){
            leaveRooms << QString::fromStdString(r.first);
        }
        roomListModel->remove(leaveRooms);
    };
    QObject::connect(client, &Client::initialSync,roomUpdateHandler);
    QObject::connect(client, &Client::newUpdated,roomUpdateHandler);
    QObject::connect(client, &Client::initiateFinished,[&](){
        auto joinedRooms = client->joinedRoomList();
        auto inviteRooms = client->inviteRoomList();
        QVector<RoomListItem> roomList;
        for(auto const &r: joinedRooms.toStdMap()){
            RoomListItem room(  r.first,
                                r.second.name,
                                r.second.avatar_url,
                                false);
            roomList.push_back(room);
        }

        for(auto const &r: inviteRooms.toStdMap()){
            RoomListItem room(  r.first,
                                r.second.name,
                                r.second.avatar_url,
                                true);
            roomList.push_back(room);
        }
        roomListModel->add(roomList);
    });

    QQmlApplicationEngine engine;
    qmlRegisterSingletonInstance<Client>("MatrixClient", 1, 0, "MatrixClient", client);
    qmlRegisterSingletonType<RoomListModel>("Rooms", 1, 0, "Rooms", [](QQmlEngine *, QJSEngine *) -> QObject * {
          return roomListModel;
    });
    
    engine.load(url);
    return q_app.exec();
}
