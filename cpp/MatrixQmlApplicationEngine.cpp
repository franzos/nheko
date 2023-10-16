#include "MatrixQmlApplicationEngine.h"
#include <QQuickItem>
#include "MxcImageProvider.h"
#include "ColorImageProvider.h"
#include "JdenticonProvider.h"
#include "BlurhashProvider.h"

namespace PX::GUI::MATRIX{

QMap<QString, ApplicationFeatures> ApplicationOptions::FeatureStrings = {
    {"keybackup", ApplicationFeatures::KeyBackup}
};

MatrixQmlApplicationEngine::MatrixQmlApplicationEngine(QObject *parent, ApplicationOptions options):
    QmlInterface(parent), QQmlApplicationEngine(parent),
    m_options(options)
{
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
    if (m_options.hiddenFeatures.contains(ApplicationFeatures::KeyBackup)) {
        qDebug() << "!!! KEY BACKUP FEATURE IS DISABLED !!!";
    }
    QQmlApplicationEngine::setInitialProperties({
        { "embedVideoQML", !callAutoAccept },
        { "callAutoAccept", callAutoAccept},
        {"hideKeyBackup", m_options.hiddenFeatures.contains(ApplicationFeatures::KeyBackup)},
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