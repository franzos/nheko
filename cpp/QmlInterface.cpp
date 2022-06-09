#include "QmlInterface.h"
#include <QCoreApplication>
#include <QQuickStyle>
#include <QQuickItem>
#include <QStandardPaths>
#include <qglobalstatic.h>
#include <QFile>
#include <QDir>
#include <matrix-client-library/encryption/DeviceVerificationFlow.h>
#include <matrix-client-library/UIA.h>
#include "TimelineModel.h"
#include "GlobalObject.h"
#include "mydevice.h"
#include "ui/NhekoCursorShape.h"
#include "ui/DelegateChooser.h"
#include "Configuration.h"
#include "ui/emoji/emojimodel.h"
#include "Clipboard.h"

namespace PX::GUI::MATRIX{

using webrtc::CallType;
using webrtc::State;

Client *QmlInterface::backendClient(){
    return _client;
}

void QmlInterface::checkCacheDirectory(){
    GlobalObject gobject;
    QSettings *qSettings;
    auto cacheDirs = QStandardPaths::standardLocations(QStandardPaths::CacheLocation);
    if(cacheDirs.size()){
        auto cacheInfoFile = cacheDirs[0] + "/info";
        qSettings = new QSettings(cacheInfoFile, QSettings::IniFormat);
        qInfo() << " > QML Cache dir found:" << cacheDirs[0];
        if(QFile(cacheInfoFile).exists()){
            qInfo() << " > QML Cache info detected:" << cacheInfoFile;
            if(qSettings->contains("version")) {
                auto version = qSettings->value("version").toString();
                if(version == gobject.getApplicationVersion()){
                    qInfo() << " > QML Cache version matched:" << version;
                    return;
                } else {
                    qInfo() << " > QML Cache should be updated from" << version << "to" << gobject.getApplicationVersion();
                }
            } else {
                qInfo() << " > QML Cache version not found!";
            }
        } else {
            qInfo() << " > QML Cache file info not found";
        }
        auto cacheDir = cacheDirs[0] + "/qmlcache";
        if(QDir(cacheDir).exists()){
            if(QDir(cacheDir).removeRecursively())
                qInfo() << " > QML Cache dir deleted:" << cacheDir; 
        }
        qSettings->setValue("version",gobject.getApplicationVersion());
        qSettings->sync();
        qInfo() << " > QML Cache info created: Version" << gobject.getApplicationVersion();
    }
}

QmlInterface::QmlInterface(QObject *parent): 
    QObject(parent),
    _roomListModel(new RoomListModel({})),
    _client(Client::instance()),
    _callMgr(_client->callManager()),
    _verificationManager(_client->verificationManager()),
    _userSettings{UserSettings::instance()}{    
    _client->enableLogger(true, true);    
    checkCacheDirectory();
    if(_callMgr->callsSupported()){
        qDebug() << "*** VOIP Supported";
    }
    #if ALLOW_SERVER_CHANGE
        setServerAddress("");
    #else
        setServerAddress(DEFAULT_SERVER);
    #endif
#ifdef Q_OS_ANDROID
    setStyle("Material", "Default");
#else
    #ifdef Q_OS_LINUX
        setStyle("Breeze", "Default");
    #else
        #ifdef Q_OS_WINDOWS
            setStyle("Universal", "Default");
        #endif
    #endif
#endif
    connect(_client, &Client::cmUserInfoUpdated,this, &QmlInterface::setCMUserInformation);
    connect(_client, &Client::newUpdated,this, &QmlInterface::newSyncCb);
    connect(_client, &Client::initiateFinished,this, &QmlInterface::initiateFinishedCB);
    connect(_client, &Client::logoutOk,[&](){
        _roomListModel->removeRows(0,_roomListModel->rowCount());
    });
    qmlRegisterType<MyDevice>("mydevice", 1, 0, "MyDevice");
    connect(_callMgr, &CallManager::devicesChanged, [=]() {
        auto defaultMic = UserSettings::instance()->microphone();
        auto defaultCam = UserSettings::instance()->camera();
        auto mics = CallDevices::instance().names(false, defaultMic.toStdString());
        auto cams = CallDevices::instance().names(true, defaultCam.toStdString());
        nhlog::ui()->info(">>> DEVICES CHANGED: mics: {} - cams: {}", mics.size(), cams.size());
        if (mics.size() > 0) {
            for (const auto &mic : mics) {
                auto q_mic = QString::fromStdString(mic);
                if (!q_mic.toLower().startsWith("monitor")) {
                    UserSettings::instance()->setMicrophone(q_mic);
                    nhlog::ui()->info("   - [mic]: {}", mic);
                    break;
                }
            }
        }
        if (cams.size() > 0) {
            UserSettings::instance()->setCamera(QString::fromStdString(cams[0]));
            nhlog::ui()->info("   - [cam]: {}", cams[0]);
        }
    });

    qmlRegisterSingletonType<GlobalObject>("GlobalObject", 1, 0, "GlobalObject", [](QQmlEngine *, QJSEngine *) -> QObject * {
          return new GlobalObject();
    });
    qmlRegisterType<EmojiModel>("EmojiModel", 1, 0, "EmojiModel");
    qmlRegisterType<NhekoCursorShape>("CursorShape", 1, 0, "CursorShape");
    qmlRegisterType<DelegateChoice>("DelegateChoice", 1, 0, "DelegateChoice");
    qmlRegisterType<DelegateChooser>("DelegateChooser", 1, 0, "DelegateChooser");
    qmlRegisterType<PresenceEmitter>("Presence", 1, 0, "Presence");
    qmlRegisterType<RoomInformation>("RoomInformation", 1, 0, "RoomInformation");
    qmlRegisterSingletonInstance<QmlInterface>("QmlInterface", 1, 0, "QmlInterface", this);
    qmlRegisterSingletonInstance<Client>("MatrixClient", 1, 0, "MatrixClient", _client);
    qmlRegisterSingletonInstance<CallManager>("CallManager", 1, 0, "CallManager", _callMgr);
    qmlRegisterSingletonInstance<UIA>("UIA", 1, 0, "UIA", UIA::instance());
    qmlRegisterSingletonInstance<VerificationManager>("VerificationManager", 1, 0, "VerificationManager", _verificationManager);
    qmlRegisterSingletonInstance<SelfVerificationStatus>("SelfVerificationStatus", 1, 0, "SelfVerificationStatus", _verificationManager->selfVerificationStatus());
    qmlRegisterSingletonInstance<RoomListModel>("Rooms", 1, 0, "Rooms", _roomListModel);
    qmlRegisterSingletonInstance("Settings", 1, 0, "Settings", _userSettings.data());
    qmlRegisterUncreatableType<DeviceVerificationFlow>("DeviceVerificationFlow", 1, 0, "DeviceVerificationFlow", "Can't create verification flow from QML!");
    qmlRegisterUncreatableType<TimelineModel>("TimelineModel", 1, 0, "TimelineModel", QStringLiteral("Room needs to be instantiated on the C++ side"));
    qmlRegisterSingletonType<Clipboard>("Clipboard", 1, 0, "Clipboard", [](QQmlEngine *, QJSEngine *) -> QObject * {
        return new Clipboard();
    });

    qmlRegisterUncreatableMetaObject(qml_mtx_events::staticMetaObject, "MtxEvent",   1,  0,  "MtxEvent", QStringLiteral("Can't instantiate enum!"));
    qRegisterMetaType<AndroidMaterialTheme>();
    qmlRegisterUncreatableMetaObject(AndroidMaterialTheme::staticMetaObject, "AndroidMaterialTheme", 1, 0, "AndroidMaterialTheme", QStringLiteral("Can't instantiate AndroidMaterialTheme"));   
    qRegisterMetaType<UserInformation>();
    qmlRegisterUncreatableMetaObject(UserInformation::staticMetaObject, "UserInformation", 1, 0, "UserInformation", QStringLiteral("Can't instantiate UserInformation"));    
    qRegisterMetaType<CMUserInformation>();
    qmlRegisterUncreatableMetaObject(CMUserInformation::staticMetaObject, "CMUserInformation", 1, 0, "CMUserInformation", QStringLiteral("Can't instantiate CMUserInformation"));    
    qRegisterMetaType<webrtc::CallType>();
    qmlRegisterUncreatableMetaObject(webrtc::staticMetaObject, "CallType", 1, 0, "CallType", QStringLiteral("Can't instantiate enum"));
    qRegisterMetaType<webrtc::State>();
    qmlRegisterUncreatableMetaObject(webrtc::staticMetaObject, "WebRTCState", 1, 0, "WebRTCState", QStringLiteral("Can't instantiate enum"));
}

QmlInterface::~QmlInterface(){
    _client->stop();
}
    
QUrl QmlInterface::mainLibQMLurl(){
    return QUrl(QStringLiteral("qrc:/qml/MainLib.qml"));
}

QUrl QmlInterface::mainAppQMLurl(){
    return QUrl(QStringLiteral("qrc:/qml/main.qml"));
}

void QmlInterface::newSyncCb(const mtx::responses::Sync &sync){
    auto rooms = sync.rooms;
    QList<RoomListItem> roomList;
    for(auto const &r: rooms.join){
        RoomListItem room(  QString::fromStdString(r.first),
                            r.second.unread_notifications.notification_count);
        roomList << room;
    }
    for(auto const &r: rooms.invite){
        RoomListItem room(  QString::fromStdString(r.first),
                            QString::fromStdString(r.second.name()),
                            QString::fromStdString(r.second.avatar()),
                            true);
        roomList << room;
        if(_callAutoAccept){
            _client->joinRoom(QString::fromStdString(r.first));
        }
    }
    _roomListModel->add(roomList);
    
    QStringList leaveRooms;
    for(auto const &r: rooms.leave){
        leaveRooms << QString::fromStdString(r.first);
    }
    _roomListModel->remove(leaveRooms);
}

void QmlInterface::initiateFinishedCB(){
    auto joinedRooms = _client->joinedRoomList();
    auto inviteRooms = _client->inviteRoomList();
    QList<RoomListItem> roomList;
    for(auto const &r: joinedRooms.toStdMap()){
        RoomListItem room(  r.first);
        roomList << room;
    }

    for(auto const &r: inviteRooms.toStdMap()){
        RoomListItem room(  r.first);
        roomList << room;
        if(_callAutoAccept){
            _client->joinRoom(r.first);
        }
    }
    _roomListModel->add(roomList);
}

void QmlInterface::setStyle(const QString &style, const QString &fallback){
    QQuickStyle::setStyle(style);
    QQuickStyle::setFallbackStyle(fallback);
    qDebug() << "Style:" << QQuickStyle::name() << QQuickStyle::availableStyles() << ", Fallback:" << fallback;
}

void QmlInterface::setCMUserInformation(const CMUserInformation &info){
    _cmUserInformation = info;
}

CMUserInformation QmlInterface::cmUserInformation(){
    return _cmUserInformation;
}

QString QmlInterface::userId(){
    return _userId;
}

void QmlInterface::setUserId(const QString userID){
    if(userID!=_userId){
        qInfo()<<"Default user ID set to " << userID;
        _userId = userID;
        emit userIdChanged(_userId);
    }
}

QString QmlInterface::getServerAddress(){
    return _serverAddress;
};

void QmlInterface::setServerAddress(const QString &server){
    if(server!=_serverAddress){
        qInfo()<<"Default server set to " << server;
        _serverAddress = server;
        emit serverAddressChanged(_serverAddress);
    }
};

void QmlInterface::login(QmlInterface::LOGIN_TYPE type){
    emit loginProgramatically(type);
}

}