#include <QApplication>
#include <QDebug>
#include <QQuickWidget>
#include <QMainWindow>
#include <spdlog/spdlog.h>
#include "../cpp/MatrixQmlApplicationEngine.h"
#include "../cpp/MatrixQuickView.h"

#ifdef __ANDROID__
#include <spdlog/sinks/android_sink.h>
#endif

using namespace PX::GUI::MATRIX;

int main(int argc, char *argv[])
{
    #if 0
    // JUST An Example for using embeding the QML items into QWidgets
    QApplication app(argc, argv);
    auto qmlView = new PX::GUI::MATRIX::MatrixQuickView();
    qmlView->showMaximized();
    qmlView->videoCallPage()->showMaximized();
    // QWidget *container = QWidget::createWindowContainer(qmlView);
    // container->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    // container->showMaximized();
    #else
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    
    QGuiApplication app(argc, argv);
    app.setWindowIcon(QIcon(":images/app-icon_bright.svg"));
    
    MatrixQmlApplicationEngine matrixClientApp;   
    matrixClientApp.load();
    spdlog::info("info log from spdlog");
    #endif
#ifdef __ANDROID__
    std::string tag = "android-logs";
    auto android_logger = spdlog::android_logger_mt("android", tag);
    android_logger->critical("log from android.");
#endif

    return app.exec();
}
