#include "MatrixQmlApplicationEngine.h"
#include <QQuickItem>
#include "ColorImageProvider.h"

namespace PX::GUI::MATRIX{

MatrixQmlApplicationEngine::MatrixQmlApplicationEngine(QObject *parent): 
    QmlInterface(parent), QQmlApplicationEngine(parent){            
    // connect(this, &QQmlApplicationEngine::objectCreated,
    //             QCoreApplication::instance(), [&](QObject *obj, const QUrl &objUrl) {
    //     if (!obj && mainAppQMLurl() == objUrl) {
    //         QCoreApplication::instance()->exit(-1);
    //     }
    // }, Qt::QueuedConnection);
    addImageProvider(QStringLiteral("colorimage"), new ColorImageProvider());
}

void MatrixQmlApplicationEngine::load(){
    QQmlApplicationEngine::load(mainAppQMLurl());
}

void MatrixQmlApplicationEngine::setVideoCallItem() {
    auto videoItem = rootObjects().first()->findChild<QQuickItem *>("gstGlItem");
    if(videoItem){
        WebRTCSession::instance().setVideoItem(videoItem);
    }
}

}