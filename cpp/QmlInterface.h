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
    ~QmlInterface();
    void setStyle(const QString &style, const QString &fallback);
    Q_INVOKABLE void setMatrixServer(const QString &server, bool asDefault) { 
        _defaultMatrixServer = server;
        _setServerAsDefault = asDefault;
    };
    Q_INVOKABLE void setUserIdFormat(const QString &format) { _defaultUserIdFormat = format;};
    Q_INVOKABLE QString defaultMatrixServer() {return _defaultMatrixServer;};
    Q_INVOKABLE QString defaultUserIdFormat() {return _defaultUserIdFormat;};
    Q_INVOKABLE bool isSetServerAsDefault() {return _setServerAsDefault;};
    void setAutoAcceptCall(bool mode) { _callAutoAccept = mode; };
    bool autoAcceptCall() { return _callAutoAccept; };
    
public slots:
    virtual void setVideoCallItem() = 0;

private slots:
    void initiateFinishedCB();
    void newSyncCb(const mtx::responses::Sync &sync);

private:
    bool _callAutoAccept = false;
    RoomListModel *_roomListModel = nullptr;
    Client *_client = nullptr;
    CallManager *_callMgr = nullptr;
    VerificationManager *_verificationManager;
    QSharedPointer<UserSettings> _userSettings;
    bool    _setServerAsDefault = false;
    QString _defaultMatrixServer = "https://matrix.org";
    QString _defaultUserIdFormat = "@user:matrix.org";
};
}