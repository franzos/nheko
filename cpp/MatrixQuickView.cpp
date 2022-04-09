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
            auto callParty = Client::instance()->callManager()->callPartyDisplayName();
            QMetaObject::invokeMethod(_videoCallQuickView->rootObject(), "setCallPartyName", Q_ARG(QVariant, QVariant(callParty)));
            QMetaObject::invokeMethod(_videoCallQuickView->rootObject(), "changeState", Q_ARG(QVariant, QVariant(int(state))));
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