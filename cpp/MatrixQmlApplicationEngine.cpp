#include "MatrixQmlApplicationEngine.h"
#include <QQuickItem>
#include "MxcImageProvider.h"
#include "ColorImageProvider.h"
#include "JdenticonProvider.h"
#include "BlurhashProvider.h"

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
    addImageProvider(QStringLiteral("blurhash"), new BlurhashProvider());
    _mxcImageProvider = new MxcImageProvider();
    addImageProvider(QStringLiteral("MxcImage"), _mxcImageProvider);
    if (jdenticonProviderisAvailable())
        addImageProvider(QStringLiteral("jdenticon"), new JdenticonProvider());
}

void MatrixQmlApplicationEngine::load(bool callAutoAccept){
    QQmlApplicationEngine::setInitialProperties({
        { "embedVideoQML", !callAutoAccept },
        { "callAutoAccept", callAutoAccept}
    });
    setAutoAcceptCall(callAutoAccept);
    QQmlApplicationEngine::load(mainAppQMLurl());
}

void MatrixQmlApplicationEngine::setVideoCallItem() {
    auto videoItem = rootObjects().first()->findChild<QQuickItem *>("gstGlItem");
    if(videoItem){
        WebRTCSession::instance().setVideoItem(videoItem);
    }
}

}