#include <QApplication>
#include <QDebug>
#include <QQuickWidget>
#include <QMainWindow>
#include <spdlog/spdlog.h>
#include "../cpp/MatrixQmlApplicationEngine.h"
#ifdef __ANDROID__
#include <spdlog/sinks/android_sink.h>
#endif

using namespace PX::GUI::MATRIX;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    // MatrixClient matrixClient(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    // matrixClient.load();
    // QApplication app(argc, argv);
    MatrixQmlApplicationEngine matrixClientApp;
    matrixClientApp.load();
    spdlog::info("info log from spdlog");

#ifdef __ANDROID__
    std::string tag = "android-logs";
    auto android_logger = spdlog::android_logger_mt("android", tag);
    android_logger->critical("log from android.");
#endif

    return app.exec();
}
