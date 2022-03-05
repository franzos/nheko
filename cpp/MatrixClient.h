#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QUrl>
#include <QQmlApplicationEngine>
#include <matrix-client-library/Client.h>

#include "RoomListModel.h"
#include "RoomListItem.h"

class MatrixClient : public QObject {
public: 
    MatrixClient(const QUrl &url, QObject *parent = nullptr);
    void load();

private slots:
    void initiateFinishedCB();
    void newSyncCb(const mtx::responses::Sync &sync);

private:
    QUrl _mainUrl;
    RoomListModel *_roomListModel = nullptr;
    Client *_client = nullptr;
    QQmlApplicationEngine _engine;
    VerificationManager *_verificationManager;
};
#endif // CLIENT_H
