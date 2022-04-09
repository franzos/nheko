#include "MatrixQuickView.h"
#include "ColorImageProvider.h"

namespace PX::GUI::MATRIX{

using webrtc::CallType;
using webrtc::State;

MatrixQuickView::MatrixQuickView(QWindow *parent): 
    QmlInterface(parent) ,
    QQuickView(mainLibQMLurl(), parent),
    _videoCallQuickView (new QQuickView(QUrl(QStringLiteral("qrc:/qml/voip/VideoCallEmbedPage.qml")))){
        engine()->addImageProvider(QStringLiteral("colorimage"), new ColorImageProvider());
        // TODO Code dupplication in QML and Cpp to handle call events (signals, states, and ...)
        QObject::connect(Client::instance()->callManager(), &CallManager::newCallState,[&](){
            auto state = Client::instance()->callManager()->callState();
            if(Client::instance()->callManager()->isOnCall()){
                auto callParty = Client::instance()->callManager()->callPartyDisplayName();
                if (state == webrtc::State::CONNECTED){
                    QMetaObject::invokeMethod(_videoCallQuickView->rootObject(), "changeState", Q_ARG(QVariant, QVariant("oncall")));
                } else if (state == webrtc::State::ANSWERSENT || state == webrtc::State::CONNECTING || 
                           state == webrtc::State::OFFERSENT  || state == webrtc::State::INITIATING ){
                    QMetaObject::invokeMethod(_videoCallQuickView->rootObject(), "changeState", Q_ARG(QVariant, QVariant("transient")));
                    QString text = "...";
                    if(state == webrtc::State::ANSWERSENT)
                        text = "Answering " + callParty + "...";
                    else if(state == webrtc::State::CONNECTING)
                        text = "Connecting " + callParty + "...";
                    else if(state == webrtc::State::OFFERSENT)
                        text = "Calling " + callParty + "...";
                    QMetaObject::invokeMethod(_videoCallQuickView->rootObject(), "setTransientText", Q_ARG(QVariant, QVariant(text)));
                } 
            } else {
                QMetaObject::invokeMethod(_videoCallQuickView->rootObject(), "changeState", Q_ARG(QVariant, QVariant("freecall")));
            }
        });
        QObject::connect(Client::instance()->callManager(), &CallManager::newCallState,[&](){
            if(Client::instance()->callManager()->isOnCall() && Client::instance()->callManager()->callType() != CallType::VOICE){
                setVideoCallItem();
            } else if (!Client::instance()->callManager()->isOnCall()) {
                qWarning() << "Call finished";
            }
        });
}

QQuickView *MatrixQuickView::videoCallPage(){
    return _videoCallQuickView;
}

void MatrixQuickView::setVideoCallItem() {
    auto videoItem = _videoCallQuickView->rootObject()->findChild<QQuickItem *>( QStringLiteral("gstGlItem"));
    if(videoItem){
        WebRTCSession::instance().setVideoItem(videoItem);
    } else {
        
    }
}
}