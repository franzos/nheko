#pragma once 

#include <QObject>
#include <QUrl>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QWindow>
#include <QQmlEngine>
#include <matrix-client-library/Client.h>
#include <matrix-client-library/UserProfile.h>
#include <matrix-client-library/voip/CallManager.h>
#include <matrix-client-library/UserSettings.h>
#include <matrix-client-library/voip/WebRTCSession.h>
#include "RoomListModel.h"
#include "RoomListItem.h"
#include "notifications/Manager.h"

class NotificationsManager;

namespace PX::GUI::MATRIX {

class QmlInterface : public QObject {
    Q_OBJECT

public: 
    enum class LOGIN_TYPE {
        PASSWORD,
        CIBA
    };
    Q_ENUMS(LOGIN_TYPE)

    QmlInterface(QObject *parent = nullptr);
    Client *backendClient();
    QUrl mainLibQMLurl();
    QUrl mainAppQMLurl();
    ~QmlInterface();
    void setStyle(const QString &style, const QString &fallback);    
    Q_INVOKABLE void setUserIdFormat(const QString &format) { _defaultUserIdFormat = format;};
    Q_INVOKABLE QString defaultUserIdFormat() {return _defaultUserIdFormat;};
    void setAutoAcceptCall(bool mode) { _callAutoAccept = mode; };
    bool autoAcceptCall() { return _callAutoAccept; };
    void login(LOGIN_TYPE type);
    bool dbusAvailable() const { return _dbusAvailable; }

signals:
    void userIdChanged(const QString &userId);
    void serverAddressChanged(const QString &server);
    void loginProgramatically(LOGIN_TYPE type);
    void notificationClicked(const QString &roomid);
    
public slots:
    virtual void setVideoCallItem() = 0;
    QString userId();
    void setUserId(const QString userID);
    QString getServerAddress();
    void setServerAddress(const QString &server);
    void setCMUserInformation(const PX::AUTH::UserProfileInfo &info);
    PX::AUTH::UserProfileInfo cmUserInformation();

private slots:
    void initiateFinishedCB();
    void newSyncCb(const mtx::responses::Sync &sync);

private:
    void checkCacheDirectory();

    bool _callAutoAccept = false;
    RoomListModel *_roomListModel = nullptr;
    Client      *_client = nullptr;
    CallManager *_callMgr = nullptr;
    CallDevices *_callDevices;
    VerificationManager *_verificationManager;
    QSharedPointer<UserSettings> _userSettings;
    QString _defaultUserIdFormat = "@user:matrix.org";
    QString _serverAddress = "";
    QString _userId = "";
#if defined(NOTIFICATION_DBUS_SYS)
    NotificationsManager _notificationsManager;
#endif
    bool _dbusAvailable;
    PX::AUTH::UserProfileInfo _cmUserInformation;
};
}