#include "QmlInterface.h"
#include <QtGlobal>
#include <QCoreApplication>
#include <QApplication>
#include <QQuickStyle>
#include <QQuickItem>
#include <QStandardPaths>
#include <qglobalstatic.h>
#include <QFile>
#include <QDir>
#include <matrix-client-library/encryption/DeviceVerificationFlow.h>
#include <matrix-client-library/UIA.h>
#include <matrix-client-library/voip/AudioDeviceControl.h>
#include <matrix-client-library/UserSettings.h>
#include "TimelineModel.h"
#include "GlobalObject.h"
#include "mydevice.h"
#include "ui/NhekoCursorShape.h"
#include "ui/DelegateChooser.h"
#include "ui/MxcAnimatedImage.h"
#include "ui/MxcMediaProxy.h"
#include "ui/emoji/EmojiModel.h"
#include "ui/HiddenEvents.h"
#include "ui/RoomSettings.h"
#include "ui/UserSettingsModel.h"
#include "Configuration.h"
#include "Clipboard.h"
#include "AvatarProvider.h"
#include "JdenticonProvider.h"
#include "InviteesModel.h"

Q_DECLARE_METATYPE(std::vector<DeviceInfo>)
Q_DECLARE_METATYPE(QVector<UserInformation>)

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
            qSettings = new QSettings(cacheInfoFile, QSettings::IniFormat, this);
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
    _roomListModel(new RoomListModel(this)),
    _client(Client::instance()),
    _callMgr(_client->callManager()),
    _callDevices(&CallDevices::instance()),
    _verificationManager(_client->verificationManager()),
    _userSettings{UserSettings::instance()}
#if defined(NOTIFICATION_DBUS_SYS)    
    ,_notificationsManager(this)
#endif
    {
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
    connect(_client, &Client::newUpdate,this, &QmlInterface::newSyncCb);
    connect(_client, &Client::initiateFinished,this, &QmlInterface::initiateFinishedCB);
    connect(_client, &Client::logoutOk,[&](){
        _roomListModel->removeRows(0,_roomListModel->rowCount());
    });
#if defined(NOTIFICATION_DBUS_SYS)
    connect(_client, &Client::newNotifications,[&](const mtx::responses::Notifications &notifications){
        for (auto const &item : notifications.notifications) {
            auto info = _client->roomInfo(QString::fromStdString(item.room_id));
            AvatarProvider::resolve(info.avatar_url,
                                            96,
                                            this,
                                            [this, item](QPixmap image) {
                                                _notificationsManager.postNotification(
                                                  item, image.toImage());
                                            });
        }
    });
    connect(&_notificationsManager,
            &NotificationsManager::notificationClicked,
            this,
            [this](const QString &roomid, const QString &eventid) {
                Q_UNUSED(eventid)
                if(qApp->allWindows().size())
                    qApp->allWindows().at(0)->requestActivate();
                emit notificationClicked(roomid);
            });
    connect(&_notificationsManager,
            &NotificationsManager::sendNotificationReply,
            this,
            [this](const QString &roomid, const QString &eventid, const QString &body) {
                // view_manager_->queueReply(roomid, eventid, body);
                Q_UNUSED(eventid)
                Q_UNUSED(body)
                if(qApp->allWindows().size())
                    qApp->allWindows().at(0)->requestActivate();
                emit notificationClicked(roomid);
            });

    // connect(cache::client(),
    //         &Cache::removeNotification,
    //         &_notificationsManager,
    //         &NotificationsManager::removeNotification);
#endif
        qmlRegisterType<MyDevice>("mydevice", 1, 0, "MyDevice");
        connect(_callMgr, &CallManager::devicesChanged, [=]() {
            auto defaultMic = UserSettings::instance()->microphone();
            auto defaultCam = UserSettings::instance()->camera();
            auto mics = CallDevices::instance().names(false, defaultMic.toStdString());
            auto cams = CallDevices::instance().names(true, defaultCam.toStdString());
            nhlog::ui()->info(">>> DEVICES CHANGED: mics: {} - cams: {}", mics.size(), cams.size());

            for (const auto &mic : mics) {
                nhlog::ui()->info("   - [mic]: {}", mic);
            }
            for (const auto &cam : cams) {
                nhlog::ui()->info("   - [cam]: {}", cam);
            }
            nhlog::ui()->info("   - [default mic]: {}", defaultMic.toStdString());
            nhlog::ui()->info("   - [default cam]: {}", defaultCam.toStdString());
        });

        qmlRegisterSingletonType<GlobalObject>("GlobalObject", 1, 0, "GlobalObject", [](QQmlEngine *, QJSEngine *) -> QObject * {
            return new GlobalObject();
        });
        qmlRegisterSingletonType<UserSettingsModel>("UserSettingsModel", 1, 0, "UserSettingsModel", [](QQmlEngine *, QJSEngine *) -> QObject * {
            return new UserSettingsModel();
        });
        qmlRegisterSingletonInstance("Settings", 1, 0, "Settings", _client->userSettings().data());
        auto audioContol = new AudioDeviceControl();
        qmlRegisterSingletonType<AudioDeviceControl>("AudioDeviceControl", 1, 0, "AudioDeviceControl", [audioContol](QQmlEngine *, QJSEngine *) -> QObject * {
            return audioContol;
        });
        qRegisterMetaType<std::vector<DeviceInfo>>();
        qRegisterMetaType<QVector<UserInformation>>();
        qmlRegisterType<HiddenEvents>("HiddenEvents", 1, 0, "HiddenEvents");
        qmlRegisterType<emoji::EmojiModel>("EmojiModel", 1, 0, "EmojiModel");
        qmlRegisterUncreatableType<emoji::Emoji>("Emoji", 1, 0, "Emoji", QStringLiteral("Used by emoji models"));
        qmlRegisterUncreatableType<MediaUpload>("MediaUpload", 1, 0, "MediaUpload", QStringLiteral("MediaUploads can not be created in Qml"));
        qmlRegisterType<NhekoCursorShape>("CursorShape", 1, 0, "CursorShape");
        qmlRegisterType<DelegateChoice>("DelegateChoice", 1, 0, "DelegateChoice");
        qmlRegisterType<DelegateChooser>("DelegateChooser", 1, 0, "DelegateChooser");
        qmlRegisterType<MxcAnimatedImage>("MxcAnimatedImage", 1, 0, "MxcAnimatedImage");
        qmlRegisterType<MxcMediaProxy>("MxcMedia", 1, 0, "MxcMedia");
        qmlRegisterType<PresenceEmitter>("Presence", 1, 0, "Presence");
        qmlRegisterType<RoomInformation>("RoomInformation", 1, 0, "RoomInformation");
        qmlRegisterSingletonInstance<QmlInterface>("QmlInterface", 1, 0, "QmlInterface", this);
        qmlRegisterSingletonInstance<Client>("MatrixClient", 1, 0, "MatrixClient", _client);
        qmlRegisterSingletonInstance<CallManager>("CallManager", 1, 0, "CallManager", _callMgr);
        qmlRegisterSingletonInstance<CallDevices>("CallDevices", 1, 0, "CallDevices", _callDevices);
        qmlRegisterSingletonInstance<UIA>("UIA", 1, 0, "UIA", UIA::instance());
        qmlRegisterSingletonInstance<VerificationManager>("VerificationManager", 1, 0, "VerificationManager", _verificationManager);
        qmlRegisterSingletonInstance<SelfVerificationStatus>("SelfVerificationStatus", 1, 0, "SelfVerificationStatus", _verificationManager->selfVerificationStatus());
        qmlRegisterSingletonType<FilteredRoomlistModel>("Rooms", 1, 0, "Rooms", [&](QQmlEngine *, QJSEngine *) -> QObject * {
            auto ptr = new FilteredRoomlistModel(_roomListModel);

            // connect(self->communities_,
            //         &CommunitiesModel::currentTagIdChanged,
            //         ptr,
            //         &FilteredRoomlistModel::updateFilterTag);
            // connect(self->communities_,
            //         &CommunitiesModel::hiddenTagsChanged,
            //         ptr,
            //         &FilteredRoomlistModel::updateHiddenTagsAndSpaces);
            return ptr;
        });
        qmlRegisterSingletonInstance("Settings", 1, 0, "Settings", _userSettings.data());
        qmlRegisterUncreatableType<DeviceVerificationFlow>("DeviceVerificationFlow", 1, 0, "DeviceVerificationFlow", "Can't create verification flow from QML!");
        qmlRegisterUncreatableType<TimelineModel>("TimelineModel", 1, 0, "TimelineModel", QStringLiteral("Room needs to be instantiated on the C++ side"));
        qmlRegisterSingletonType<Clipboard>("Clipboard", 1, 0, "Clipboard", [](QQmlEngine *, QJSEngine *) -> QObject * {
            return new Clipboard();
        });
        qmlRegisterUncreatableType<InviteesModel>("InviteesModel", 1, 0, "InviteesModel", QStringLiteral("InviteesModel needs to be instantiated on the C++ side"));
        qmlRegisterUncreatableType<MemberList>("MemberList", 1, 0, "MemberList", QStringLiteral("MemberList needs to be instantiated on the C++ side"));
        qmlRegisterUncreatableType<ReadReceiptsProxy>("ReadReceiptsProxy", 1, 0,"ReadReceiptsProxy", QStringLiteral("ReadReceiptsProxy needs to be instantiated on the C++ side"));
        qmlRegisterUncreatableType<RoomSettings>("RoomSettingsModel", 1, 0, "RoomSettingsModel", QStringLiteral("Room Settings needs to be instantiated on the C++ side"));
        qmlRegisterUncreatableMetaObject(olm::staticMetaObject, "Olm", 1, 0, "Olm", QStringLiteral("Can't instantiate enum!"));
        qmlRegisterUncreatableMetaObject(crypto::staticMetaObject, "Crypto", 1, 0, "Crypto", QStringLiteral("Can't instantiate enum!"));
        qmlRegisterUncreatableMetaObject(qml_mtx_events::staticMetaObject, "MtxEvent",   1,  0,  "MtxEvent", QStringLiteral("Can't instantiate enum!"));
        qRegisterMetaType<AndroidMaterialTheme>();
        qmlRegisterUncreatableMetaObject(AndroidMaterialTheme::staticMetaObject, "AndroidMaterialTheme", 1, 0, "AndroidMaterialTheme", QStringLiteral("Can't instantiate AndroidMaterialTheme"));   
        qRegisterMetaType<AudioDeviceInfo>();
        qmlRegisterUncreatableMetaObject(AudioDeviceInfo::staticMetaObject, "AudioDeviceInfo", 1, 0, "AudioDeviceInfo", QStringLiteral("Can't instantiate AudioDeviceInfo"));    
        qRegisterMetaType<UserInformation>();
        qmlRegisterUncreatableMetaObject(UserInformation::staticMetaObject, "UserInformation", 1, 0, "UserInformation", QStringLiteral("Can't instantiate UserInformation"));    
        qRegisterMetaType<PX::AUTH::UserProfileInfo>();
        qmlRegisterUncreatableMetaObject(PX::AUTH::UserProfileInfo::staticMetaObject, "UserProfileInfo", 1, 0, "UserProfileInfo", QStringLiteral("Can't instantiate UserProfileInfo"));    
        qRegisterMetaType<webrtc::CallType>();
        qmlRegisterUncreatableMetaObject(webrtc::staticMetaObject, "CallType", 1, 0, "CallType", QStringLiteral("Can't instantiate enum"));
        qRegisterMetaType<webrtc::State>();
        qmlRegisterUncreatableMetaObject(webrtc::staticMetaObject, "WebRTCState", 1, 0, "WebRTCState", QStringLiteral("Can't instantiate enum"));
        qmlRegisterUncreatableType<UserProfile>("UserProfile",1,0,"UserProfile","UserProfile needs to be instantiated on the C++ side");
        qmlRegisterUncreatableType<Permissions>("Permissions",1,0,"Permissions","Permissions needs to be instantiated on the C++ side");
        qmlRegisterUncreatableMetaObject(verification::staticMetaObject,"VerificationStatus",1,0,"VerificationStatus",QStringLiteral("Can't instantiate enum!"));
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
                                Client::instance()->roomInfo(QString::fromStdString(r.first)),
                                r.second.unread_notifications.notification_count);
            roomList << room;
        }
        for(auto const &r: rooms.invite){
            auto invite = cache::client()->invite(r.first);
            if (!invite)
                continue;
            RoomListItem room(  QString::fromStdString(r.first),
                                *invite);
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
            RoomListItem room(r.first, r.second);
            roomList << room;
        }

        for(auto const &r: inviteRooms.toStdMap()){
            RoomListItem room(r.first, r.second);
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

    void QmlInterface::setCMUserInformation(const PX::AUTH::UserProfileInfo &info){
        _cmUserInformation = info;
    }

    PX::AUTH::UserProfileInfo QmlInterface::cmUserInformation(){
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

    void QmlInterface::login(LOGIN_TYPE type, const QString &accessToken){
        emit loginProgramatically(type, accessToken);
    }
    
    bool QmlInterface::jdenticonProviderisAvailable(){
        return JdenticonProvider::isAvailable();
    }
    
    void QmlInterface::logout(){
        _roomListModel->cleanup();
        _client->logout();
    }

    void QmlInterface::addToRoomlist(QList<RoomListItem> &roomlist){
        for(auto const &r: roomlist){
            auto timeline = Client::instance()->timeline(r.id());
            if(timeline && timeline->events() && _mxcImageProvider){
                connect(timeline->events(),
                        &EventStore::newEncryptedImage,
                        _mxcImageProvider,
                        &MxcImageProvider::addEncryptionInfo);
            }
        }
        _roomListModel->add(roomlist);
    }
}