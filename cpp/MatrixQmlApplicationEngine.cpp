#include "MatrixQmlApplicationEngine.h"
#include <QQuickItem>
#include "MxcImageProvider.h"
#include "ColorImageProvider.h"
#include "JdenticonProvider.h"
#include "BlurhashProvider.h"

namespace PX::GUI::MATRIX{

QMap<ApplicationOptions::AppFeatures, QString> ApplicationOptions::APP_FEATURES_DICT = {
    {ApplicationOptions::AppFeatures::keybackup, "keybackup"},
    {ApplicationOptions::AppFeatures::menu, "menu"},
    {ApplicationOptions::AppFeatures::interface, "interface"},
    {ApplicationOptions::AppFeatures::settings_interface, "settings_interface"},
    };

QMap<ApplicationOptions::AppMenuEntries, QString> ApplicationOptions::APP_MENU_ENTRIES_DICT = {
    { ApplicationOptions::AppMenuEntries::profile, "profile" },
    { ApplicationOptions::AppMenuEntries::settings, "settings" },
    { ApplicationOptions::AppMenuEntries::my_qr_code, "my_qr_code" },
    { ApplicationOptions::AppMenuEntries::logout, "logout" },
    { ApplicationOptions::AppMenuEntries::about, "about" },
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
    QStringList hiddenFeatures;
    for (const auto &feature : m_options.hiddenFeatures) {
        hiddenFeatures << ApplicationOptions::APP_FEATURES_DICT[feature];
    }

    QStringList hiddenMenuEntries;
    for (const auto& entry : m_options.hiddenMenuEntries) {
        hiddenMenuEntries << ApplicationOptions::APP_MENU_ENTRIES_DICT[entry];
    }

    QStringList visibleFeatures;
    for (const auto& feature : m_options.visibleFeatures) {
        visibleFeatures << ApplicationOptions::APP_FEATURES_DICT[feature];
    }

    QQmlApplicationEngine::setInitialProperties({
        { "embedVideoQML", !callAutoAccept },
        { "callAutoAccept", callAutoAccept},
        { "hiddenFeatures", hiddenFeatures },
        { "visibleFeatures", visibleFeatures },
        { "hiddenMenuEntries", hiddenMenuEntries }
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
