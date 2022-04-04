#pragma once 

#include <QObject>
#include <QUrl>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QWindow>
#include <QQmlEngine>
#include <matrix-client-library/Client.h>
#include <matrix-client-library/voip/CallManager.h>
#include <matrix-client-library/UserSettings.h>
#include <matrix-client-library/voip/WebRTCSession.h>
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

public slots:
    void setVideoCallItem();

private slots:
    void initiateFinishedCB();
    void newSyncCb(const mtx::responses::Sync &sync);

protected:
    QQmlApplicationEngine *_engine = nullptr;

private:
    RoomListModel *_roomListModel = nullptr;
    Client *_client = nullptr;
    CallManager *_callMgr = nullptr;
    VerificationManager *_verificationManager;
    QSharedPointer<UserSettings> _userSettings;
};
}