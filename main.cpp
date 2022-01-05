#include <QGuiApplication>
#include <QQuickView>
#include <QtQml>
#include <QQuickStyle>
#include <matrix-client-library/Client.h>
#include <QString>
#include <QObject>

#include "RoomListModel.h"
QAbstractItemModel *roomListModel;
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
        QStringList roomIds;
        for(auto const &r: rooms.join)
            roomIds << QString::fromStdString(r.first);

        for(auto const &r: rooms.invite)
            roomIds << QString::fromStdString(r.first);

        roomListModel->insertRows(0, roomIds.size(), QModelIndex());
        int row = 0;
        for(auto const &r: roomIds){
            auto roomInfo = client->roomInfo(r);
             QModelIndex index = roomListModel->index(row , 0, QModelIndex());
             roomListModel->setData(index, roomInfo.name);
             row++;
        }
    };
    QObject::connect(client, &Client::initialSync,roomUpdateHandler);
    QObject::connect(client, &Client::newUpdated,roomUpdateHandler);
    // QObject::connect(client, &Client::initiateFinished,roomUpdateHandler);

    QQmlApplicationEngine engine;
    qmlRegisterSingletonInstance<Client>("MatrixClient", 1, 0, "MatrixClient", client);
    qmlRegisterSingletonType<RoomListModel>("Rooms", 1, 0, "Rooms", [](QQmlEngine *, QJSEngine *) -> QObject * {
          return roomListModel;
    });
    
    engine.load(url);
    return q_app.exec ();
}
