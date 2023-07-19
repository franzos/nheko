#ifndef NOTIFICATIONHANDLER_H
#define NOTIFICATIONHANDLER_H

#include <QObject>
#include <QString>

class FirebaseQtApp;
class FirebaseQtMessaging;

class NotificationHandler : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString token READ tokenStr NOTIFY tokenChanged)
protected:
    explicit NotificationHandler(QObject *parent = nullptr);

public:
    static NotificationHandler* Instance();

    void startMessaging();

    QByteArray token() const;

    void submitError(const QString &errorMessage);
    void submitLog(const QString &logMessage);

    Q_INVOKABLE QString tokenStr() const;

public slots:
    void setToken(const QByteArray &newToken);
    void submitMessage(const QMap<QString, QString> &message);

signals:
    void tokenChanged(const QString &tokenStr);
    void messageReceived(const QMap<QString, QString> &message);
    void errorOccurred(const QString &errorMessagge);
    void logSubmitted(const QString &logMessage);

private:
    static NotificationHandler* _instance;

    QByteArray m_token;
    FirebaseQtApp *m_app;
    FirebaseQtMessaging *m_messaging;
};

#endif // NOTIFICATIONHANDLER_H
