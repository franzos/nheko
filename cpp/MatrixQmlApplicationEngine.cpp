#include "MatrixQmlApplicationEngine.h"
#include <QQuickItem>
#include "ColorImageProvider.h"
#include <matrix-client-library/MxcImageProvider.h>
#include <matrix-client-library/JdenticonProvider.h>

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
    auto imgProvider = new MxcImageProvider();
    addImageProvider(QStringLiteral("MxcImage"), imgProvider);
    if (JdenticonProvider::isAvailable())
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