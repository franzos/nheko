// SPDX-FileCopyrightText: 2021 GlobalObject Contributors
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QFontDatabase>
#include <QObject>
#include <QPalette>
#include <QBuffer>
#include <QUrl>
#include "Theme.h"
#include "Application.h"

class QWindow;

struct AndroidMaterialTheme {
Q_GADGET
    public:
    QString accent;
    QString primary;
    QString primaryForeground;
    QString foreground;
    QString background;

    Q_PROPERTY(QString accent MEMBER accent)
    Q_PROPERTY(QString primary MEMBER primary)
    Q_PROPERTY(QString primaryForeground MEMBER primaryForeground)
    Q_PROPERTY(QString foreground MEMBER foreground)
    Q_PROPERTY(QString background MEMBER background)
};

class GlobalObject : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QPalette colors READ colors NOTIFY colorsChanged)
    Q_PROPERTY(QPalette inactiveColors READ inactiveColors NOTIFY colorsChanged)
    Q_PROPERTY(Theme theme READ theme NOTIFY colorsChanged)
    Q_PROPERTY(int avatarSize READ avatarSize CONSTANT)
    Q_PROPERTY(int paddingSmall READ paddingSmall CONSTANT)
    Q_PROPERTY(int paddingMedium READ paddingMedium CONSTANT)
    Q_PROPERTY(int paddingLarge READ paddingLarge CONSTANT)

private:
    GlobalObject();
    static GlobalObject *_instance;

public:
    static GlobalObject *instance();

    QPalette colors() const;
    QPalette inactiveColors() const;
    Theme theme() const;

    int avatarSize() const { return 40; }

    int paddingSmall() const { return 4; }
    int paddingMedium() const { return 8; }
    int paddingLarge() const { return 20; }
    Q_INVOKABLE QFont monospaceFont() const {
        return QFontDatabase::systemFont(QFontDatabase::FixedFont);
    }
    Q_INVOKABLE void openLink(QString link);
    Q_INVOKABLE void setStatusMessage(QString msg) const;
    Q_INVOKABLE QString getApplicationVersion(){return QString::fromStdString(VERSION_APPLICATION);}
    Q_INVOKABLE QString checkMatrixServerUrl(QString url);
    Q_INVOKABLE AndroidMaterialTheme materialColors();
    Q_INVOKABLE QString mediaCachePath();
    Q_INVOKABLE bool mobileMode();
    
public slots:
    bool handleMatrixUri(const QByteArray &uri);
    bool handleMatrixUri(const QUrl &uri);
    static QString getSaveFileName(const QString &caption = QString(),
                                   const QString &dir = QString(),
                                   const QString &selectedFile = QString(),
                                   const QString &filter = QString());
    static void saveAs(const QString &source, const QString &dst);
    static void saveBufferToFile(const QString &filename, const QBuffer &buffer);

signals:
    void colorsChanged();
    void profileChanged();

    void openLogoutDialog();
    void openJoinRoomDialog();
    void joinRoom(QString roomId);
};
