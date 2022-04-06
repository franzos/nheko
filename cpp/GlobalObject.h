// SPDX-FileCopyrightText: 2021 GlobalObject Contributors
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QFontDatabase>
#include <QObject>
#include <QPalette>
#include <QUrl>
#include "Theme.h"

class QWindow;

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

public:
    GlobalObject();

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

public slots:
    bool handleMatrixUri(const QByteArray &uri);
    bool handleMatrixUri(const QUrl &uri);
    
signals:
    void colorsChanged();
    void profileChanged();

    void openLogoutDialog();
    void openJoinRoomDialog();
    void joinRoom(QString roomId);
};
