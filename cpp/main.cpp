#include <QApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <spdlog/spdlog.h>

#ifdef __ANDROID__
#include <spdlog/sinks/android_sink.h>
#endif

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl) {
            QCoreApplication::exit(-1);
        }
    }, Qt::QueuedConnection);
    engine.load(url);

   spdlog::info("info log from spdlog");

#ifdef __ANDROID__
    std::string tag = "android-logs";
    auto android_logger = spdlog::android_logger_mt("android", tag);
    android_logger->critical("log from android.");
#endif

    return app.exec();
}
