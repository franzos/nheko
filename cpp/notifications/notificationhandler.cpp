#include "notificationhandler.h"

#include <QDebug>

#ifdef Q_OS_ANDROID
#include "firebase/firebaseqtapp.h"
#include "firebase/firebaseqtmessaging.h"
#endif

NotificationHandler* NotificationHandler::_instance = nullptr;

NotificationHandler::NotificationHandler(QObject *parent) : QObject(parent)
{
#ifdef Q_OS_ANDROID
    m_app = new FirebaseQtApp(this);
    m_messaging = new FirebaseQtMessaging(m_app);
    connect(m_messaging, &FirebaseQtMessaging::tokenReceived, this, &NotificationHandler::setToken, Qt::QueuedConnection);
    connect(m_messaging, &FirebaseQtMessaging::messageReceived, this, &NotificationHandler::submitMessage, Qt::QueuedConnection);
#endif
}

void NotificationHandler::startMessaging()
{
#ifdef Q_OS_ANDROID
    m_app->initialize();
#endif
}

NotificationHandler *NotificationHandler::Instance()
{
    if (_instance == nullptr) {
        _instance = new NotificationHandler(nullptr);
    }
    return _instance;
}

QByteArray NotificationHandler::token() const
{
    return m_token;
}

void NotificationHandler::setToken(const QByteArray &newToken)
{
    if (m_token == newToken) {
        return;
    }
    m_token = newToken;
    emit tokenChanged(this->tokenStr());
}

void NotificationHandler::submitError(const QString &errorMessage)
{
    emit errorOccurred(errorMessage);
}

void NotificationHandler::submitMessage(const QMap<QString, QString> &message)
{
    emit messageReceived(message);
}

void NotificationHandler::submitLog(const QString &logMessage)
{
    emit logSubmitted(logMessage);
}

QString NotificationHandler::tokenStr() const
{
#ifdef Q_OS_IOS
    // APNS push token needs to be converted to base64:
    //      https://github.com/matrix-org/matrix-ios-sdk/blob/822095fc29093ad0aa5e2dd48ff5b3c71183aef4/README.rst#L431
    return m_token.toBase64();
#endif
    return m_token;
}
