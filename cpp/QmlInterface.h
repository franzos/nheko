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
#include "MxcImageProvider.h"
#include "Features.h"

class NotificationsManager;

namespace PX::GUI::MATRIX {
class QmlInterface : public QObject {
    Q_OBJECT
public: 
    // [[deprecated("Use the \"PX::AUTH::LOGIN_TYPE\" class instead of it.")]]
    typedef PX::AUTH::LOGIN_TYPE LOGIN_TYPE;
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
    void login(PX::AUTH::LOGIN_TYPE type, const QString &accessToken = "");
    void logout();

signals:
    void userIdChanged(const QString &userId);
    void serverAddressChanged(const QString &server);
    void loginTypeChanged(const PX::AUTH::LOGIN_TYPE type);
    void loginProgramatically(PX::AUTH::LOGIN_TYPE type, const QString &accessToken);
    void notificationClicked(const QString &roomid);
    
public slots:
    virtual void setVideoCallItem() = 0;
    QString userId();
    void setUserId(const QString userID);
    QString getServerAddress();
    void setServerAddress(const QString &server);
    PX::AUTH::LOGIN_TYPE getLoginType();
    void setLoginType(PX::AUTH::LOGIN_TYPE type);
#if CIBA_AUTH
    void setCMUserInformation(const PX::AUTH::UserProfileInfo &info);
    PX::AUTH::UserProfileInfo cmUserInformation();
#endif
    bool jdenticonProviderisAvailable();
    bool cibaSupport(){
        #if CIBA_AUTH
        return true;
        #else
        return false;
        #endif
    }

private slots:
    void initiateFinishedCB();
    void newSyncCb(const mtx::responses::Sync &sync);

private:
    void checkCacheDirectory();
    void addToRoomlist(QList<RoomListItem> &roomlist);

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
    PX::AUTH::LOGIN_TYPE _type;
#if defined(NOTIFICATION_DBUS_SYS)
    NotificationsManager _notificationsManager;
#endif
#if CIBA_AUTH
    PX::AUTH::UserProfileInfo _cmUserInformation;
#endif
protected:
    MxcImageProvider *_mxcImageProvider = nullptr;
};
}