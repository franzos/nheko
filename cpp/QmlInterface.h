#pragma once 

#include <QObject>
#include <QUrl>
#include <QQmlApplicationEngine>
#include <matrix-client-library/Client.h>
#include <matrix-client-library/voip/CallManager.h>
#include <QQuickView>
#include <QWindow>

#include "RoomListModel.h"
#include "RoomListItem.h"

namespace PX::GUI::MATRIX{
class QmlInterface : public QObject {
    Q_OBJECT
public: 
    QmlInterface(QObject *parent = nullptr);
    Client *client();
    QUrl mainLibQMLurl();
    QUrl mainAppQMLurl();

private slots:
    void initiateFinishedCB();
    void newSyncCb(const mtx::responses::Sync &sync);

private:
    RoomListModel *_roomListModel = nullptr;
    Client *_client = nullptr;
    CallManager *_callMgr = nullptr;
    VerificationManager *_verificationManager;
};
}