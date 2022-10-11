// SPDX-FileCopyrightText: 2017 Konstantinos Sideris <siderisk@auth.gr>
// SPDX-FileCopyrightText: 2021 Nheko Contributors
// SPDX-FileCopyrightText: 2022 Nheko Contributors
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAbstractListModel>
#include <QProcessEnvironment>
#include <QSettings>
#include <QSharedPointer>

#include <optional>
class UserSettingsModel : public QAbstractListModel
{
    Q_OBJECT

    enum Indices
    {
        // GeneralSection,
        // MobileMode,
// #ifndef Q_OS_MAC
//         ScaleFactor,
// #endif
        // Font,
        // FontSize,
        // EmojiFont,
        // AvatarCircles,
        // UseIdenticon,
        // PrivacyScreen,
        // PrivacyScreenTimeout,

        // TimelineSection,
        // TimelineMaxWidth,
        // MessageHoverHighlight,
        // EnlargeEmojiOnlyMessages,
        // AnimateImagesOnHover,
        // OpenImageExternal,
        // OpenVideoExternal,
        // ButtonsInTimeline,
        // TypingNotifications,
        // ReadReceipts,
        // Markdown,
        // Bubbles,
        // SmallAvatars,
        SidebarSection,
        // GroupView,
        SortByImportance,
        // DecryptSidebar,
        // SpaceNotifications,

        // TraySection,
        // Tray,
        // StartInTray,

        // NotificationsSection,
        // DesktopNotifications,
        // AlertOnNotification,

        // VoipSection,
        // UseStunServer,
        // Ringtone,

        EncryptionSection,
        OnlyShareKeysWithVerifiedUsers,
        ShareKeysWithTrustedUsers,
        SessionKeys,
        UseOnlineKeyBackup,
        OnlineBackupKey,
        SelfSigningKey,
        UserSigningKey,
        MasterKey,
        CrossSigningSecrets,
        DeviceId,
        DeviceFingerprint,

        LoginInfoSection,
        UserId,
        Homeserver,
        Profile,
        Version,
        COUNT,
        // hidden for now
        AccessToken,
// #ifdef Q_OS_MAC
//         ScaleFactor,
// #endif
    };

public:
    enum Types
    {
        Toggle,
        ReadOnlyText,
        Options,
        Integer,
        Double,
        SectionTitle,
        SectionBar,
        KeyStatus,
        SessionKeyImportExport,
        XSignKeysRequestDownload,
    };
    Q_ENUM(Types);

    enum Roles
    {
        Name,
        Description,
        Value,
        Type,
        ValueLowerBound,
        ValueUpperBound,
        ValueStep,
        Values,
        Good,
        Enabled,
    };

    UserSettingsModel(QObject *parent = nullptr);
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override
    {
        (void)parent;
        return (int)COUNT;
    }
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    Q_INVOKABLE void importSessionKeys();
    Q_INVOKABLE void exportSessionKeys();
    Q_INVOKABLE void requestCrossSigningSecrets();
    Q_INVOKABLE void downloadCrossSigningSecrets(const QString &recoveryKey);
};
