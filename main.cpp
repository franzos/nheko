#include <QGuiApplication>
#include <QQuickView>
#include <QtQml>
#include <QQuickStyle>
#include <matrix-client-library/Client.h>
#include <QString>
#include <QObject>

int main (int argc, char* argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication q_app (argc, argv);
    qDebug() << QQuickStyle::availableStyles();
    // QQuickStyle::setStyle("Imagine");
    const QUrl url(QStringLiteral("qrc:///qmls/main.qml"));
    Client::instance()->enableLogger(true, true);
    QQmlApplicationEngine engine;
    qmlRegisterSingletonType<Client>("Client", 1, 0, "Client", [](QQmlEngine *, QJSEngine *) -> QObject * {
        return Client::instance();
    });

    engine.load(url);

    return q_app.exec ();
}

