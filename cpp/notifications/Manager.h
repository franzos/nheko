// SPDX-FileCopyrightText: 2021 Nheko Contributors
// SPDX-FileCopyrightText: 2022 Nheko Contributors
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QImage>
#include <QObject>
#include <QString>

#include <mtx/responses/notifications.hpp>

// convenience definition
#if defined(Q_OS_LINUX) || defined(Q_OS_FREEBSD) || defined(Q_OS_HAIKU)
#define NOTIFICATION_DBUS_SYS
#endif

#if defined(NOTIFICATION_DBUS_SYS)
#include <QtDBus/QDBusArgument>
#include <QtDBus/QDBusInterface>

struct roomEventId
{
    QString roomId;
    QString eventId;
};

inline bool
operator==(const roomEventId &a, const roomEventId &b)
{
    return a.roomId == b.roomId && a.eventId == b.eventId;
}

class NotificationsManager : public QObject
{
    Q_OBJECT
public:
    NotificationsManager(QObject *parent = nullptr);

    void postNotification(const mtx::responses::Notification &notification, const QImage &icon);

signals:
    void notificationClicked(const QString roomId, const QString eventId);
    void sendNotificationReply(const QString roomId, const QString eventId, const QString body);
    void systemPostNotificationCb(const QString &room_id,
                                  const QString &event_id,
                                  const QString &roomName,
                                  const QString &text,
                                  const QImage &icon);

public slots:
    void removeNotification(const QString &roomId, const QString &eventId);

public:
    void closeNotifications(QString roomId);

private:
    QDBusInterface dbus;

    void systemPostNotification(const QString &room_id,
                                const QString &event_id,
                                const QString &roomName,
                                const QString &text,
                                const QImage &icon);
    void closeNotification(uint id);

    // notification ID to (room ID, event ID)
    QMap<uint, roomEventId> notificationIds;

    const bool hasMarkup_;
    const bool hasImages_;

    // these slots are platform specific (D-Bus only)
    // but Qt slot declarations can not be inside an ifdef!
private slots:
    void actionInvoked(uint id, QString action);
    void notificationClosed(uint id, uint reason);
    void notificationReplied(uint id, QString reply);

private:
    QString getMessageTemplate(const mtx::responses::Notification &notification);
};

QDBusArgument &
operator<<(QDBusArgument &arg, const QImage &image);
const QDBusArgument &
operator>>(const QDBusArgument &arg, QImage &);
#endif
