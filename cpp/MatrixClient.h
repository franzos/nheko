#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QUrl>
#include <QQmlApplicationEngine>
#include <matrix-client-library/Client.h>
#include <QQuickView>
#include <QWindow>

#include "RoomListModel.h"
#include "RoomListItem.h"

class MatrixClient : public QObject {
    Q_OBJECT
public: 
    MatrixClient(QObject *parent = nullptr);
    Client *client();
    QUrl mainLibQMLurl();
    QUrl mainAppQMLurl();

private slots:
    void initiateFinishedCB();
    void newSyncCb(const mtx::responses::Sync &sync);

private:
    RoomListModel *_roomListModel = nullptr;
    Client *_client = nullptr;
    VerificationManager *_verificationManager;
};

class MatrixClientQmlApplicationEngine : public MatrixClient ,public QQmlApplicationEngine{
public:
    MatrixClientQmlApplicationEngine(QObject *parent = nullptr);
    void load();
};

class MatrixClientQuickView : public MatrixClient, public QQuickView{
public:
    MatrixClientQuickView(QWindow *parent = nullptr);
};
#endif // CLIENT_H
