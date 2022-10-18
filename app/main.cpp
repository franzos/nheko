#include <QApplication>
#include <QStandardPaths>
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
    // JUST as an Example for using embeding the QML items into QWidgets
    QApplication app(argc, argv);
    auto qmlView = new PX::GUI::MATRIX::MatrixQuickView();
    qmlView->showMaximized();
    qmlView->videoCallPage()->showMaximized();
    qmlView->setUserId("USERID@pantherx.org");
    qmlView->setServerAddress("https://matrix.pantherx.dev");
    // QWidget *container = QWidget::createWindowContainer(qmlView);
    // container->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    // container->showMaximized();
    #else
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":images/app-icon_bright.svg"));
    
    MatrixQmlApplicationEngine matrixClientApp;   
    matrixClientApp.load();
    spdlog::info("info log from spdlog");
    #endif
#ifdef __ANDROID__
    auto android_logger = spdlog::android_logger_mt("android");
    android_logger->info("log from android.");
    android_logger->info("- DATA LOCATION: {}", QStandardPaths::writableLocation(QStandardPaths::DataLocation).toStdString());
    android_logger->info("- CACHE LOCATION: {}", GlobalObject::instance()->toStdString());
#endif

    return app.exec();
}
