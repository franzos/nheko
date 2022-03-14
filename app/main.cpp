#include <QApplication>
#include <QDebug>
#include <QQuickWidget>
#include <QMainWindow>
#include <spdlog/spdlog.h>
#include "../cpp/MatrixClient.h"
#ifdef __ANDROID__
#include <spdlog/sinks/android_sink.h>
#endif

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MatrixClientQmlApplicationEngine matrixClientApp;
    matrixClientApp.load();
    spdlog::info("info log from spdlog");

#ifdef __ANDROID__
    std::string tag = "android-logs";
    auto android_logger = spdlog::android_logger_mt("android", tag);
    android_logger->critical("log from android.");
#endif

    return app.exec();
}
