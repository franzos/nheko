#include "MatrixQuickView.h"
#include <QQuickItem>
#include "ColorImageProvider.h"

namespace PX::GUI::MATRIX{

MatrixQuickView::MatrixQuickView(QWindow *parent): 
    QmlInterface(parent) ,QQuickView(mainLibQMLurl(), parent){
        engine()->addImageProvider(QStringLiteral("colorimage"), new ColorImageProvider());
}

void MatrixQuickView::setVideoCallItem() {
    auto videoItem = rootObject()->findChild<QQuickItem *>( QStringLiteral("videoCallItem"));
    if(videoItem){
        WebRTCSession::instance().setVideoItem(videoItem);
    }
}
}