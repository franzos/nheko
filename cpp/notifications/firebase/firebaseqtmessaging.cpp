#include "firebaseqtmessaging.h"

#include "firebaseqtabstractmodule.h"
#include "firebaseqtapp.h"
#include "firebaseqtapp_p.h"

#include <firebase/messaging.h>

#include <QThread>
#include <QLoggingCategory>
#include <QDebug>

Q_LOGGING_CATEGORY(FIREBASE_MESSAGING, "FIREBASE.MESSAGING")

class FirebaseQtMessagingPrivate: public ::firebase::messaging::Listener
{
public:
    FirebaseQtMessagingPrivate(FirebaseQtMessaging *q): q_ptr(q) {}

    virtual void OnMessage(const ::firebase::messaging::Message &message);

    virtual void OnTokenReceived(const char *token);

    FirebaseQtMessaging *q_ptr;
};



FirebaseQtMessaging::FirebaseQtMessaging(FirebaseQtApp *parent)
    : FirebaseQtAbstractModule(parent), d_ptr(new FirebaseQtMessagingPrivate(this))
{
}

FirebaseQtMessaging::~FirebaseQtMessaging()
{
    delete d_ptr;
}

void FirebaseQtMessaging::initialize(FirebaseQtApp *app)
{
    ::firebase::messaging::Initialize(*app->d_ptr->app, d_ptr);
    qDebug(FIREBASE_MESSAGING) << "MESSAGING INITIALIZED";
}

void FirebaseQtMessagingPrivate::OnMessage(const ::firebase::messaging::Message &message)
{
    qDebug(FIREBASE_MESSAGING) << "NEW MESSAGE RECEIVED";
    QMap<QString, QString> data;
    auto it = message.data.begin();
    while (it != message.data.end()) {
        data.insert(QString::fromStdString(it->first), QString::fromStdString(it->second));
        ++it;
    }
    Q_EMIT q_ptr->messageReceived(data);
}

void FirebaseQtMessagingPrivate::OnTokenReceived(const char *token)
{
    qDebug(FIREBASE_MESSAGING) << "DEVICE TOKEN RECEIVED:" << token;
    Q_EMIT q_ptr->tokenReceived(token);
}
