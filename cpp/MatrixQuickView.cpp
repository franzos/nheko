#include "MatrixQuickView.h"
#include "ColorImageProvider.h"
#include "MxcImageProvider.h"
#include "JdenticonProvider.h"
#include "BlurhashProvider.h"

namespace PX::GUI::MATRIX{

using webrtc::CallType;
using webrtc::State;

MatrixQuickView::MatrixQuickView(QWindow *parent): 
    QmlInterface(parent) ,
    QQuickView(mainLibQMLurl(), parent),
    _videoCallQuickView (new QQuickView(engine(),parent)){
        _videoCallQuickView->setSource(QUrl(QStringLiteral("qrc:/qml/voip/VideoCallEmbedPage.qml")));
        engine()->addImageProvider(QStringLiteral("colorimage"), new ColorImageProvider());
        engine()->addImageProvider(QStringLiteral("blurhash"), new BlurhashProvider());
        _mxcImageProvider = new MxcImageProvider();
        engine()->addImageProvider(QStringLiteral("MxcImage"), _mxcImageProvider);
        if (jdenticonProviderisAvailable())
            engine()->addImageProvider(QStringLiteral("jdenticon"), new JdenticonProvider());

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