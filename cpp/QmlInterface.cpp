#include "QmlInterface.h"
#include <QCoreApplication>
#include <QQuickStyle>
#include <QQuickItem>
#include <matrix-client-library/encryption/DeviceVerificationFlow.h>
#include <matrix-client-library/UIA.h>
#include "TimelineModel.h"
#include "GlobalObject.h"
#include "mydevice.h"

namespace PX::GUI::MATRIX{

using webrtc::CallType;
using webrtc::State;

QmlInterface::QmlInterface(QObject *parent): 
    QObject(parent),
    _roomListModel(new RoomListModel({})),
    _client(Client::instance()),
    _callMgr(_client->callManager()),
    _verificationManager(_client->verificationManager()),
    _userSettings{UserSettings::instance()}{
    _client->enableLogger(true, true);
    connect(_client, &Client::newUpdated,this, &QmlInterface::newSyncCb);
    connect(_client, &Client::initiateFinished,this, &QmlInterface::initiateFinishedCB);
    connect(_client, &Client::logoutOk,[&](){
        _roomListModel->removeRows(0,_roomListModel->rowCount());
    });
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


    connect(_callMgr,
                     qOverload<const QString &, const mtx::events::msg::CallInvite &>(&CallManager::newMessage),
                     [=](const QString &roomid, const mtx::events::msg::CallInvite &invite) {
                         nhlog::ui()->info("CALL INVITE: callid: {} - room: {}", invite.call_id, roomid.toStdString());
                         if (auto timeline = _client->timeline(roomid)) {
                             timeline->sendMessageEvent(invite, mtx::events::EventType::CallInvite);
                         }
                     });

    connect(_callMgr,
                     qOverload<const QString &, const mtx::events::msg::CallCandidates &>(&CallManager::newMessage),
                     [=](const QString &roomid, const mtx::events::msg::CallCandidates &candidate) {
                         nhlog::ui()->info("CALL CANDIDATE: callid: {} - room: {}", candidate.call_id, roomid.toStdString());
                         if (auto timeline = _client->timeline(roomid)) {
                             timeline->sendMessageEvent(candidate, mtx::events::EventType::CallCandidates);
                         }
                     });

    connect(_callMgr,
                     qOverload<const QString &, const mtx::events::msg::CallAnswer &>(&CallManager::newMessage),
                     [=](const QString &roomid, const mtx::events::msg::CallAnswer &answer) {
                         nhlog::ui()->info("CALL ANSWER: callid: {} - room: {}", answer.call_id, roomid.toStdString());
                         if (auto timeline = _client->timeline(roomid)) {
                             timeline->sendMessageEvent(answer, mtx::events::EventType::CallAnswer);
                         }
                     });

    connect(_callMgr,
                     qOverload<const QString &, const mtx::events::msg::CallHangUp &>(&CallManager::newMessage),
                     [=](const QString &roomid, const mtx::events::msg::CallHangUp &hangup) {
                         nhlog::ui()->info("CALL HANGUP: callid: {} - room: {}", hangup.call_id, roomid.toStdString());
                         if (auto timeline = _client->timeline(roomid)) {
                             timeline->sendMessageEvent(hangup, mtx::events::EventType::CallHangUp);
                         }
                     });

    qmlRegisterSingletonType<GlobalObject>("GlobalObject", 1, 0, "GlobalObject", [](QQmlEngine *, QJSEngine *) -> QObject * {
          return new GlobalObject();
    });
    qmlRegisterType<TimelineModel>("TimelineModel", 1, 0, "TimelineModel");
    qmlRegisterType<RoomInformation>("RoomInformation", 1, 0, "RoomInformation");
    qmlRegisterSingletonInstance<QmlInterface>("QmlInterface", 1, 0, "QmlInterface", this);
    qmlRegisterSingletonInstance<Client>("MatrixClient", 1, 0, "MatrixClient", _client);
    qmlRegisterSingletonInstance<CallManager>("CallManager", 1, 0, "CallManager", _callMgr);
    qmlRegisterSingletonInstance<UIA>("UIA", 1, 0, "UIA", UIA::instance());
    qmlRegisterUncreatableType<DeviceVerificationFlow>("DeviceVerificationFlow", 1, 0, "DeviceVerificationFlow", "Can't create verification flow from QML!");
    qmlRegisterSingletonInstance<VerificationManager>("VerificationManager", 1, 0, "VerificationManager", _verificationManager);
    qmlRegisterSingletonInstance<SelfVerificationStatus>("SelfVerificationStatus", 1, 0, "SelfVerificationStatus", _verificationManager->selfVerificationStatus());
    qmlRegisterSingletonType<RoomListModel>("Rooms", 1, 0, "Rooms", [&](QQmlEngine *, QJSEngine *) -> QObject * {
        return _roomListModel;
    });
    qRegisterMetaType<webrtc::CallType>();
    qmlRegisterUncreatableMetaObject(webrtc::staticMetaObject, "CallType", 1, 0, "CallType", QStringLiteral("Can't instantiate enum"));
    qRegisterMetaType<webrtc::State>();
    qmlRegisterUncreatableMetaObject(webrtc::staticMetaObject, "WebRTCState", 1, 0, "WebRTCState", QStringLiteral("Can't instantiate enum"));
    qmlRegisterSingletonInstance("Settings", 1, 0, "Settings", _userSettings.data());
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
        auto roomInfo = _client->roomInfo(QString::fromStdString(r.first));
        RoomListItem room(  QString::fromStdString(r.first),
                            roomInfo,
                            r.second.unread_notifications.notification_count);
        roomList << room;
    }

    for(auto const &r: rooms.invite){
        RoomListItem room(  QString::fromStdString(r.first),
                            QString::fromStdString(r.second.name()),
                            QString::fromStdString(r.second.avatar()),
                            true);
        roomList << room;
    }
    _roomListModel->add(roomList);
    
    QStringList leaveRooms;
    for(auto const &r: rooms.leave){
        leaveRooms << QString::fromStdString(r.first);
    }
    _roomListModel->remove(leaveRooms);
}

void QmlInterface::initiateFinishedCB(){auto joinedRooms = _client->joinedRoomList();
    auto inviteRooms = _client->inviteRoomList();
    QList<RoomListItem> roomList;
    for(auto const &r: joinedRooms.toStdMap()){
        RoomListItem room(  r.first,
                            r.second);
        roomList << room;
    }

    for(auto const &r: inviteRooms.toStdMap()){
        RoomListItem room(  r.first,
                            r.second);
        roomList << room;
    }
    _roomListModel->add(roomList);
}
}