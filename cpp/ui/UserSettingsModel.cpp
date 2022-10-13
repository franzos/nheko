// SPDX-FileCopyrightText: 2017 Konstantinos Sideris <siderisk@auth.gr>
// SPDX-FileCopyrightText: 2021 Nheko Contributors
// SPDX-FileCopyrightText: 2022 Nheko Contributors
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "UserSettingsModel.h"

#include <QStandardPaths>
#include <QString>

#include <QTextStream>
#include <QCoreApplication>
#include <QFileDialog>
#include <QFontDatabase>
#include <QInputDialog>
#include <QMessageBox>
#include <QFont>
#include <QDebug>
#include <mtx/secret_storage.hpp>
#include <matrix-client-library/Client.h>
#include <matrix-client-library/UserSettings.h>
#include <matrix-client-library/MatrixClient.h>
#include <matrix-client-library/Utils.h>
#include <matrix-client-library/encryption/Olm.h>
#include <matrix-client-library/voip/CallDevices.h>

#include "../Application.h"

QHash<int, QByteArray>
UserSettingsModel::roleNames() const
{
    static QHash<int, QByteArray> roles{
      {Name, "name"},
      {Description, "description"},
      {Value, "value"},
      {Type, "type"},
      {ValueLowerBound, "valueLowerBound"},
      {ValueUpperBound, "valueUpperBound"},
      {ValueStep, "valueStep"},
      {Values, "values"},
      {Good, "good"},
      {Enabled, "enabled"},
    };

    return roles;
}

QVariant
UserSettingsModel::data(const QModelIndex &index, int role) const
{
    if (index.row() >= COUNT)
        return {};

    auto i = UserSettings::instance();
    if (!i)
        return {};

    if (role == Name) {
        switch (index.row()) {
        // case ScaleFactor:
        //     return tr("Scale factor");
        // case MessageHoverHighlight:
        //     return tr("Highlight message on hover");
        // case EnlargeEmojiOnlyMessages:
        //     return tr("Large Emoji in timeline");
        // case Tray:
        //     return tr("Minimize to tray");
        // case StartInTray:
        //     return tr("Start in tray");
        // case GroupView:
        //     return tr("Group's sidebar");
        // case Markdown:
        //     return tr("Send messages as Markdown");
        // case Bubbles:
        //     return tr("Enable message bubbles");
        // case SmallAvatars:
        //     return tr("Enable small Avatars");
        // case AnimateImagesOnHover:
        //     return tr("Play animated images only on hover");
        // case TypingNotifications:
        //     return tr("Typing notifications");
        // case SortByImportance:
        //     return tr("Sort rooms by unreads");
        // case ButtonsInTimeline:
        //     return tr("Show buttons in timeline");
        // case TimelineMaxWidth:
        //     return tr("Limit width of timeline");
        // case ReadReceipts:
        //     return tr("Read receipts");
        // case DesktopNotifications:
        //     return tr("Desktop notifications");
        // case AlertOnNotification:
        //     return tr("Alert on notification");
        // case AvatarCircles:
        //     return tr("Circular Avatars");
        // case UseIdenticon:
        //     return tr("Use identicons");
        // case OpenImageExternal:
        //     return tr("Open images with external program");
        // case OpenVideoExternal:
        //     return tr("Open videos with external program");
        // case DecryptSidebar:
        //     return tr("Decrypt messages in sidebar");
        // case SpaceNotifications:
        //     return tr("Show message counts for spaces");
        // case PrivacyScreen:
        //     return tr("Privacy Screen");
        // case PrivacyScreenTimeout:
        //     return tr("Privacy screen timeout (in seconds [0 - 3600])");
        // case MobileMode:
        //     return tr("Touchscreen mode");
        // case FontSize:
        //     return tr("Font size");
        // case Font:
        //     return tr("Font Family");
        // case EmojiFont:
        //     return tr("Emoji Font Family");
        // case Ringtone:
        //     return tr("Ringtone");
        // case UseStunServer:
        //     return tr("Allow fallback call assist server");
        case OnlyShareKeysWithVerifiedUsers:
            return tr("Send encrypted messages to verified users only");
        case ShareKeysWithTrustedUsers:
            return tr("Share keys with verified users and devices");
        case UseOnlineKeyBackup:
            return tr("Online Key Backup");
        case Profile:
            return tr("Profile");
        case UserId:
            return tr("User ID");
        case AccessToken:
            return tr("Accesstoken");
        case DeviceId:
            return tr("Device ID");
        case DeviceFingerprint:
            return tr("Device Fingerprint");
        case Homeserver:
            return tr("Homeserver");
        case Version:
            return tr("Version");
        // case GeneralSection:
        //     return tr("GENERAL");
        // case TimelineSection:
        //     return tr("TIMELINE");
        // case SidebarSection:
        //     return tr("SIDEBAR");
        // case TraySection:
        //     return tr("TRAY");
        // case NotificationsSection:
        //     return tr("NOTIFICATIONS");
        // case VoipSection:
        //     return tr("CALLS");
        case EncryptionSection:
            return tr("ENCRYPTION");
        case LoginInfoSection:
            return tr("INFO");
        case SessionKeys:
            return tr("Session Keys");
        case CrossSigningSecrets:
            return tr("Cross Signing Secrets");
        case OnlineBackupKey:
            return tr("Online backup key");
        case SelfSigningKey:
            return tr("Self signing key");
        case UserSigningKey:
            return tr("User signing key");
        case MasterKey:
            return tr("Master signing key");
        }
    } else if (role == Value) {
        switch (index.row()) {
        // case ScaleFactor:
        //     return utils::scaleFactor();
        // case MessageHoverHighlight:
        //     return i->messageHoverHighlight();
        // case EnlargeEmojiOnlyMessages:
        //     return i->enlargeEmojiOnlyMessages();
        // case Tray:
        //     return i->tray();
        // case StartInTray:
        //     return i->startInTray();
        // case GroupView:
        //     return i->groupView();
        // case Markdown:
        //     return i->markdown();
        // case Bubbles:
        //     return i->bubbles();
        // case SmallAvatars:
        //     return i->smallAvatars();
        // case AnimateImagesOnHover:
        //     return i->animateImagesOnHover();
        // case TypingNotifications:
        //     return i->typingNotifications();
        // case SortByImportance:
        //     return i->sortByImportance();
        // case ButtonsInTimeline:
        //     return i->buttonsInTimeline();
        // case TimelineMaxWidth:
        //     return i->timelineMaxWidth();
        // case ReadReceipts:
        //     return i->readReceipts();
        // case DesktopNotifications:
        //     return i->hasDesktopNotifications();
        // case AlertOnNotification:
        //     return i->hasAlertOnNotification();
        // case AvatarCircles:
        //     return i->avatarCircles();
        // case UseIdenticon:
        //     return i->useIdenticon();
        // case OpenImageExternal:
        //     return i->openImageExternal();
        // case OpenVideoExternal:
        //     return i->openVideoExternal();
        // case DecryptSidebar:
        //     return i->decryptSidebar();
        // case SpaceNotifications:
        //     return i->spaceNotifications();
        // case PrivacyScreen:
        //     return i->privacyScreen();
        // case PrivacyScreenTimeout:
        //     return i->privacyScreenTimeout();
        // case MobileMode:
        //     return i->mobileMode();
        // case FontSize:
        //     return i->fontSize();
        // case Font:
        //     return data(index, Values).toStringList().indexOf(i->font());
        // case EmojiFont:
        //     return data(index, Values).toStringList().indexOf(i->emojiFont());
        // case Ringtone: {
        //     auto v = i->ringtone();
        //     if (v == QStringView(u"Mute"))
        //         return 0;
        //     else if (v == QStringView(u"Default"))
        //         return 1;
        //     else if (v == QStringView(u"Other"))
        //         return 2;
        //     else
        //         return 3;
        // }
        // case UseStunServer:
        //     return i->useStunServer();
        case OnlyShareKeysWithVerifiedUsers:
            return i->onlyShareKeysWithVerifiedUsers();
        case ShareKeysWithTrustedUsers:
            return i->shareKeysWithTrustedUsers();
        case UseOnlineKeyBackup:
            return i->useOnlineKeyBackup();
        case Profile:
            return i->profile().isEmpty() ? tr("Default") : i->profile();
        case UserId:
            return i->userId();
        case AccessToken:
            return i->accessToken();
        case DeviceId:
            return i->deviceId();
        case DeviceFingerprint:
            return utils::humanReadableFingerprint(olm::client()->identity_keys().ed25519);
        case Homeserver:
            return i->homeserver();
        case Version:
            return QString("GUI:") + VERSION_APPLICATION + " - LIB: " + Client::instance()->getLibraryVersion();
        case OnlineBackupKey:
            return cache::secret(mtx::secret_storage::secrets::megolm_backup_v1).has_value();
        case SelfSigningKey:
            return cache::secret(mtx::secret_storage::secrets::cross_signing_self_signing)
              .has_value();
        case UserSigningKey:
            return cache::secret(mtx::secret_storage::secrets::cross_signing_user_signing)
              .has_value();
        case MasterKey:
            return cache::secret(mtx::secret_storage::secrets::cross_signing_master).has_value();
        }
    } else if (role == Description) {
        switch (index.row()) {
        // case Font:
        // case EmojiFont:
        // case Ringtone:
        //     return {};
        // case TimelineMaxWidth:
        //     return tr("Set the max width of messages in the timeline (in pixels). This can help "
        //               "readability on wide screen, when Nheko is maximised");
        // case PrivacyScreenTimeout:
        //     return tr(
        //       "Set timeout (in seconds) for how long after window loses\nfocus before the screen"
        //       " will be blurred.\nSet to 0 to blur immediately after focus loss. Max value of 1 "
        //       "hour (3600 seconds)");
        // case FontSize:
        //     return {};
        // case MessageHoverHighlight:
        //     return tr("Change the background color of messages when you hover over them.");
        // case EnlargeEmojiOnlyMessages:
        //     return tr("Make font size larger if messages with only a few emojis are displayed.");
        // case Tray:
        //     return tr(
        //       "Keep the application running in the background after closing the client window.");
        // case StartInTray:
        //     return tr("Start the application in the background without showing the client window.");
        // case GroupView:
        //     return tr("Show a column containing groups and tags next to the room list.");
        // case Markdown:
        //     return tr(
        //       "Allow using markdown in messages.\nWhen disabled, all messages are sent as a plain "
        //       "text.");
        // case Bubbles:
        //     return tr(
        //       "Messages get a bubble background. This also triggers some layout changes (WIP).");
        // case SmallAvatars:
        //     return tr("Avatars are resized to fit above the message.");
        // case AnimateImagesOnHover:
        //     return tr("Plays media like GIFs or WEBPs only when explicitly hovering over them.");
        // case TypingNotifications:
        //     return tr(
        //       "Show who is typing in a room.\nThis will also enable or disable sending typing "
        //       "notifications to others.");
        // case SortByImportance:
        //     return tr(
        //       "Display rooms with new messages first.\nIf this is off, the list of rooms will only "
        //       "be sorted by the timestamp of the last message in a room.\nIf this is on, rooms "
        //       "which "
        //       "have active notifications (the small circle with a number in it) will be sorted on "
        //       "top. Rooms, that you have muted, will still be sorted by timestamp, since you don't "
        //       "seem to consider them as important as the other rooms.");
        // case ButtonsInTimeline:
        //     return tr(
        //       "Show buttons to quickly reply, react or access additional options next to each "
        //       "message.");
        // case ReadReceipts:
        //     return tr(
        //       "Show if your message was read.\nStatus is displayed next to timestamps.\nWarning: "
        //       "If your homeserver does not support this, your rooms will never be marked as read!");
        // case DesktopNotifications:
        //     return tr("Notify about received messages when the client is not currently focused.");
        // case AlertOnNotification:
        //     return tr(
        //       "Show an alert when a message is received.\nThis usually causes the application "
        //       "icon in the task bar to animate in some fashion.");
        // case AvatarCircles:
        //     return tr(
        //       "Change the appearance of user avatars in chats.\nOFF - square, ON - circle.");
        // case UseIdenticon:
        //     return tr("Display an identicon instead of a letter when no avatar is set.");
        // case OpenImageExternal:
        //     return tr("Opens images with an external program when tapping the image.\nNote that "
        //               "when this option is ON, opened files are left unencrypted on disk and must "
        //               "be manually deleted.");
        // case OpenVideoExternal:
        //     return tr("Opens videos with an external program when tapping the video.\nNote that "
        //               "when this option is ON, opened files are left unencrypted on disk and must "
        //               "be manually deleted.");
        // case DecryptSidebar:
        //     return tr("Decrypt the messages shown in the sidebar.\nOnly affects messages in "
        //               "encrypted chats.");
        // case SpaceNotifications:
        //     return tr(
        //       "Choose where to show the total number of notifications contained within a space.");
        // case PrivacyScreen:
        //     return tr("When the window loses focus, the timeline will\nbe blurred.");
        // case MobileMode:
        //     return tr(
        //       "Will prevent text selection in the timeline to make touch scrolling easier.");
        // case ScaleFactor:
        //     return tr("Change the scale factor of the whole user interface.");
        // case UseStunServer:
        //     return tr(
        //       "Will use turn.matrix.org as assist when your home server does not offer one.");
        case OnlyShareKeysWithVerifiedUsers:
            return tr("Requires a user to be verified to send encrypted messages to them. This "
                      "improves safety but makes E2EE more tedious.");
        case ShareKeysWithTrustedUsers:
            return tr(
              "Automatically replies to key requests from other users, if they are verified, "
              "even if that device shouldn't have access to those keys otherwise.");
        case UseOnlineKeyBackup:
            return tr(
              "Download message encryption keys from and upload to the encrypted online key "
              "backup.");
        case Profile:
        case UserId:
        case AccessToken:
        case DeviceId:
        case DeviceFingerprint:
        case Homeserver:
        case Version:
        // case GeneralSection:
        // case TimelineSection:
        // case SidebarSection:
        // case TraySection:
        // case NotificationsSection:
        // case VoipSection:
        case EncryptionSection:
        case LoginInfoSection:
        case SessionKeys:
        case CrossSigningSecrets:
            return {};
        case OnlineBackupKey:
            return tr(
              "The key to decrypt online key backups. If it is cached, you can enable online "
              "key backup to store encryption keys securely encrypted on the server.");
        case SelfSigningKey:
            return tr(
              "The key to verify your own devices. If it is cached, verifying one of your devices "
              "will mark it verified for all your other devices and for users that have verified "
              "you.");
        case UserSigningKey:
            return tr(
              "The key to verify other users. If it is cached, verifying a user will verify "
              "all their devices.");
        case MasterKey:
            return tr(
              "Your most important key. You don't need to have it cached, since not caching "
              "it makes it less likely it can be stolen and it is only needed to rotate your "
              "other signing keys.");
        }
    } else if (role == Type) {
        switch (index.row()) {
        // case Font:
        // case EmojiFont:
        // case Ringtone:
        //     return Options;
        // case TimelineMaxWidth:
        // case PrivacyScreenTimeout:
        //     return Integer;
        // case FontSize:
        // case ScaleFactor:
        //     return Double;
        // case MessageHoverHighlight:
        // case EnlargeEmojiOnlyMessages:
        // case Tray:
        // case StartInTray:
        // case GroupView:
        // case Markdown:
        // case Bubbles:
        // case SmallAvatars:
        // case AnimateImagesOnHover:
        // case TypingNotifications:
        // case SortByImportance:
        // case ButtonsInTimeline:
        // case ReadReceipts:
        // case DesktopNotifications:
        // case AlertOnNotification:
        // case AvatarCircles:
        // case UseIdenticon:
        // case OpenImageExternal:
        // case OpenVideoExternal:
        // case DecryptSidebar:
        // case PrivacyScreen:
        // case MobileMode:
        // case UseStunServer:
        case OnlyShareKeysWithVerifiedUsers:
        case ShareKeysWithTrustedUsers:
        case UseOnlineKeyBackup:
        // case SpaceNotifications:
            return Toggle;
        case Profile:
        case UserId:
        case AccessToken:
        case DeviceId:
        case DeviceFingerprint:
        case Homeserver:
        case Version:
            return ReadOnlyText;
        // case GeneralSection:
        // case TimelineSection:
        // case SidebarSection:
        // case TraySection:
        // case NotificationsSection:
        // case VoipSection:
        case EncryptionSection:
        case LoginInfoSection:
            return SectionTitle;
        case SessionKeys:
            return SessionKeyImportExport;
        case CrossSigningSecrets:
            return XSignKeysRequestDownload;
        case OnlineBackupKey:
        case SelfSigningKey:
        case UserSigningKey:
        case MasterKey:
            return KeyStatus;
        }
    } else if (role == ValueLowerBound) {
        switch (index.row()) {
        // case TimelineMaxWidth:
        //     return 0;
        // case PrivacyScreenTimeout:
        //     return 0;
        // case FontSize:
        //     return 8.0;
        // case ScaleFactor:
        //     return 1.0;
        }
    } else if (role == ValueUpperBound) {
        switch (index.row()) {
        // case TimelineMaxWidth:
        //     return 20000;
        // case PrivacyScreenTimeout:
        //     return 3600;
        // case FontSize:
        //     return 24.0;
        // case ScaleFactor:
        //     return 3.0;
        }
    } else if (role == ValueStep) {
        switch (index.row()) {
        // case TimelineMaxWidth:
        //     return 20;
        // case PrivacyScreenTimeout:
        //     return 10;
        // case FontSize:
        //     return 0.5;
        // case ScaleFactor:
        //     return .25;
        }
    } else if (role == Values) {
        // auto vecToList = [](const std::vector<std::string> &vec) {
        //     QStringList l;
        //     for (const auto &d : vec)
        //         l.push_back(QString::fromStdString(d));
        //     return l;
        // };
        // static QFontDatabase fontDb;

        switch (index.row()) {
        // case Font:
        //     return fontDb.families();
        // case EmojiFont:
        //     return fontDb.families(QFontDatabase::WritingSystem::Symbol);
        // case Ringtone: {
        //     QStringList l{
        //       QStringLiteral("Mute"),
        //       QStringLiteral("Default"),
        //       QStringLiteral("Other"),
        //     };
        //     if (!l.contains(i->ringtone()))
        //         l.push_back(i->ringtone());
        //     return l;
        // }
        }
    } else if (role == Good) {
        switch (index.row()) {
        case OnlineBackupKey:
            return cache::secret(mtx::secret_storage::secrets::megolm_backup_v1).has_value();
        case SelfSigningKey:
            return cache::secret(mtx::secret_storage::secrets::cross_signing_self_signing)
              .has_value();
        case UserSigningKey:
            return cache::secret(mtx::secret_storage::secrets::cross_signing_user_signing)
              .has_value();
        case MasterKey:
            return true;
        }
    } else if (role == Enabled) {
        switch (index.row()) {
        // case StartInTray:
        //     return i->tray();
        // case PrivacyScreenTimeout:
        //     return i->privacyScreen();
        // case UseIdenticon:
        //     return JdenticonProvider::isAvailable();
        default:
            return true;
        }
    }

    return {};
}

bool
UserSettingsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    static QFontDatabase fontDb;

    auto i = UserSettings::instance();
    if (role == Value) {
        switch (index.row()) {
        // case MessageHoverHighlight: {
        //     if (value.userType() == QMetaType::Bool) {
        //         i->setMessageHoverHighlight(value.toBool());
        //         return true;
        //     } else
        //         return false;
        // }
        // case ScaleFactor: {
        //     if (value.canConvert(QMetaType::Double)) {
        //         utils::setScaleFactor(static_cast<float>(value.toDouble()));
        //         return true;
        //     } else
        //         return false;
        // }
        // case EnlargeEmojiOnlyMessages: {
        //     if (value.userType() == QMetaType::Bool) {
        //         i->setEnlargeEmojiOnlyMessages(value.toBool());
        //         return true;
        //     } else
        //         return false;
        // }
        // case Tray: {
        //     if (value.userType() == QMetaType::Bool) {
        //         i->setTray(value.toBool());
        //         return true;
        //     } else
        //         return false;
        // }
        // case StartInTray: {
        //     if (value.userType() == QMetaType::Bool) {
        //         i->setStartInTray(value.toBool());
        //         return true;
        //     } else
        //         return false;
        // }
        // case GroupView: {
        //     if (value.userType() == QMetaType::Bool) {
        //         i->setGroupView(value.toBool());
        //         return true;
        //     } else
        //         return false;
        // }
        // case Markdown: {
        //     if (value.userType() == QMetaType::Bool) {
        //         i->setMarkdown(value.toBool());
        //         return true;
        //     } else
        //         return false;
        // }
        // case Bubbles: {
        //     if (value.userType() == QMetaType::Bool) {
        //         i->setBubbles(value.toBool());
        //         return true;
        //     } else
        //         return false;
        // }
        // case SmallAvatars: {
        //     if (value.userType() == QMetaType::Bool) {
        //         i->setSmallAvatars(value.toBool());
        //         return true;
        //     } else
        //         return false;
        // }
        // case AnimateImagesOnHover: {
        //     if (value.userType() == QMetaType::Bool) {
        //         i->setAnimateImagesOnHover(value.toBool());
        //         return true;
        //     } else
        //         return false;
        // }
        // case TypingNotifications: {
        //     if (value.userType() == QMetaType::Bool) {
        //         i->setTypingNotifications(value.toBool());
        //         return true;
        //     } else
        //         return false;
        // }
        // case SortByImportance: {
        //     if (value.userType() == QMetaType::Bool) {
        //         i->setSortByImportance(value.toBool());
        //         return true;
        //     } else
        //         return false;
        // }
        // case ButtonsInTimeline: {
        //     if (value.userType() == QMetaType::Bool) {
        //         i->setButtonsInTimeline(value.toBool());
        //         return true;
        //     } else
        //         return false;
        // }
        // case TimelineMaxWidth: {
        //     if (value.canConvert(QMetaType::Int)) {
        //         i->setTimelineMaxWidth(value.toInt());
        //         return true;
        //     } else
        //         return false;
        // }
        // case ReadReceipts: {
        //     if (value.userType() == QMetaType::Bool) {
        //         i->setReadReceipts(value.toBool());
        //         return true;
        //     } else
        //         return false;
        // }
        // case DesktopNotifications: {
        //     if (value.userType() == QMetaType::Bool) {
        //         i->setDesktopNotifications(value.toBool());
        //         return true;
        //     } else
        //         return false;
        // }
        // case AlertOnNotification: {
        //     if (value.userType() == QMetaType::Bool) {
        //         i->setAlertOnNotification(value.toBool());
        //         return true;
        //     } else
        //         return false;
        // }
        // case AvatarCircles: {
        //     if (value.userType() == QMetaType::Bool) {
        //         i->setAvatarCircles(value.toBool());
        //         return true;
        //     } else
        //         return false;
        // }
        // case UseIdenticon: {
        //     if (value.userType() == QMetaType::Bool) {
        //         i->setUseIdenticon(value.toBool());
        //         return true;
        //     } else
        //         return false;
        // }
        // case OpenImageExternal: {
        //     if (value.userType() == QMetaType::Bool) {
        //         i->setOpenImageExternal(value.toBool());
        //         return true;
        //     } else
        //         return false;
        // }
        // case OpenVideoExternal: {
        //     if (value.userType() == QMetaType::Bool) {
        //         i->setOpenVideoExternal(value.toBool());
        //         return true;
        //     } else
        //         return false;
        // }
        // case DecryptSidebar: {
        //     if (value.userType() == QMetaType::Bool) {
        //         i->setDecryptSidebar(value.toBool());
        //         return true;
        //     } else
        //         return false;
        // }
        //     return i->decryptSidebar();
        // case SpaceNotifications: {
        //     if (value.userType() == QMetaType::Bool) {
        //         i->setSpaceNotifications(value.toBool());
        //         return true;
        //     } else
        //         return false;
        // }
        // case PrivacyScreen: {
        //     if (value.userType() == QMetaType::Bool) {
        //         i->setPrivacyScreen(value.toBool());
        //         return true;
        //     } else
        //         return false;
        // }
        // case PrivacyScreenTimeout: {
        //     if (value.canConvert(QMetaType::Int)) {
        //         i->setPrivacyScreenTimeout(value.toInt());
        //         return true;
        //     } else
        //         return false;
        // }
        // case MobileMode: {
        //     if (value.userType() == QMetaType::Bool) {
        //         i->setMobileMode(value.toBool());
        //         return true;
        //     } else
        //         return false;
        // }
        // case FontSize: {
        //     if (value.canConvert(QMetaType::Double)) {
        //         i->setFontSize(value.toDouble());
        //         return true;
        //     } else
        //         return false;
        // }
        // case Font: {
        //     if (value.userType() == QMetaType::Int) {
        //         i->setFontFamily(fontDb.families().at(value.toInt()));
        //         return true;
        //     } else
        //         return false;
        // }
        // case EmojiFont: {
        //     if (value.userType() == QMetaType::Int) {
        //         i->setEmojiFontFamily(
        //           fontDb.families(QFontDatabase::WritingSystem::Symbol).at(value.toInt()));
        //         return true;
        //     } else
        //         return false;
        // }
        // case Ringtone: {
        //     if (value.userType() == QMetaType::Int) {
        //         int ringtone = value.toInt();

        //         // setRingtone is called twice, because updating the list breaks the set value,
        //         // because it does not exist yet!
        //         if (ringtone == 2) {
        //             QString homeFolder =
        //               QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        //             auto filepath = QFileDialog::getOpenFileName(
        //               nullptr, tr("Select a file"), homeFolder, tr("All Files (*)"));
        //             if (!filepath.isEmpty()) {
        //                 i->setRingtone(filepath);
        //                 i->setRingtone(filepath);
        //             }
        //         } else if (ringtone == 0) {
        //             i->setRingtone(QStringLiteral("Mute"));
        //             i->setRingtone(QStringLiteral("Mute"));
        //         } else if (ringtone == 1) {
        //             i->setRingtone(QStringLiteral("Default"));
        //             i->setRingtone(QStringLiteral("Default"));
        //         }
        //         return true;
        //     }
        //     return false;
        // }
        // case UseStunServer: {
        //     if (value.userType() == QMetaType::Bool) {
        //         i->setUseStunServer(value.toBool());
        //         return true;
        //     } else
        //         return false;
        // }
        case OnlyShareKeysWithVerifiedUsers: {
            if (value.userType() == QMetaType::Bool) {
                i->setOnlyShareKeysWithVerifiedUsers(value.toBool());
                return true;
            } else
                return false;
        }
        case ShareKeysWithTrustedUsers: {
            if (value.userType() == QMetaType::Bool) {
                i->setShareKeysWithTrustedUsers(value.toBool());
                return true;
            } else
                return false;
        }
        case UseOnlineKeyBackup: {
            if (value.userType() == QMetaType::Bool) {
                i->setUseOnlineKeyBackup(value.toBool());
                return true;
            } else
                return false;
        }
        }
    }
    return false;
}

void
UserSettingsModel::importSessionKeys()
{
    const QString homeFolder = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    const QString fileName   = QFileDialog::getOpenFileName(
      nullptr, tr("Open Sessions File"), homeFolder, QLatin1String(""));

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(nullptr, tr("Error"), file.errorString());
        return;
    }

    auto bin     = file.peek(file.size());
    auto payload = std::string(bin.data(), bin.size());

    bool ok;
    auto password = QInputDialog::getText(nullptr,
                                          tr("File Password"),
                                          tr("Enter the passphrase to decrypt the file:"),
                                          QLineEdit::Password,
                                          QLatin1String(""),
                                          &ok);
    if (!ok)
        return;

    if (password.isEmpty()) {
        QMessageBox::warning(nullptr, tr("Error"), tr("The password cannot be empty"));
        return;
    }

    try {
        auto sessions = mtx::crypto::decrypt_exported_sessions(payload, password.toStdString());
        cache::importSessionKeys(std::move(sessions));
    } catch (const std::exception &e) {
        QMessageBox::warning(nullptr, tr("Error"), e.what());
    }
    }

void
UserSettingsModel::exportSessionKeys()
{
    // Open password dialog.
    bool ok;
    auto password = QInputDialog::getText(nullptr,
                                          tr("File Password"),
                                          tr("Enter passphrase to encrypt your session keys:"),
                                          QLineEdit::Password,
                                          QLatin1String(""),
                                          &ok);
    if (!ok)
        return;

    if (password.isEmpty()) {
        QMessageBox::warning(nullptr, tr("Error"), tr("The password cannot be empty"));
        return;
}
    
    // Open file dialog to save the file.
    const QString homeFolder = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    const QString fileName   = QFileDialog::getSaveFileName(
      nullptr, tr("File to save the exported session keys"), homeFolder);

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, tr("Error"), file.errorString());
        return;
    }

    // Export sessions & save to file.
    try {
        auto encrypted_blob = mtx::crypto::encrypt_exported_sessions(cache::exportSessionKeys(),
                                                                     password.toStdString());

        QString b64 = QString::fromStdString(mtx::crypto::bin2base64(encrypted_blob));

        QString prefix(QStringLiteral("-----BEGIN MEGOLM SESSION DATA-----"));
        QString suffix(QStringLiteral("-----END MEGOLM SESSION DATA-----"));
        QString newline(QStringLiteral("\n"));
        QTextStream out(&file);
        out << prefix << newline << b64 << newline << suffix << newline;
        file.close();
    } catch (const std::exception &e) {
        QMessageBox::warning(nullptr, tr("Error"), e.what());
    }
}
void
UserSettingsModel::requestCrossSigningSecrets()
{
    olm::request_cross_signing_keys();
}

void
UserSettingsModel::downloadCrossSigningSecrets(const QString &recoveryKey)
{
    olm::download_cross_signing_keys(recoveryKey.toStdString());
}

UserSettingsModel::UserSettingsModel(QObject *p)
  : QAbstractListModel(p)
{
    auto s = UserSettings::instance();
    // connect(s.get(), &UserSettings::mobileModeChanged, this, [this]() {
    //     emit dataChanged(index(MobileMode), index(MobileMode), {Value});
    // });

    // connect(s.get(), &UserSettings::fontChanged, this, [this]() {
    //     emit dataChanged(index(Font), index(Font), {Value});
    // });
    // connect(s.get(), &UserSettings::fontSizeChanged, this, [this]() {
    //     emit dataChanged(index(FontSize), index(FontSize), {Value});
    // });
    // connect(s.get(), &UserSettings::emojiFontChanged, this, [this]() {
    //     emit dataChanged(index(EmojiFont), index(EmojiFont), {Value});
    // });
    // connect(s.get(), &UserSettings::avatarCirclesChanged, this, [this]() {
    //     emit dataChanged(index(AvatarCircles), index(AvatarCircles), {Value});
    // });
    // // connect(s.get(), &UserSettings::useIdenticonChanged, this, [this]() {
    // //     emit dataChanged(index(UseIdenticon), index(UseIdenticon), {Value});
    // // });
    // connect(s.get(), &UserSettings::openImageExternalChanged, this, [this]() {
    //     emit dataChanged(index(OpenImageExternal), index(OpenImageExternal), {Value});
    // });
    // connect(s.get(), &UserSettings::openVideoExternalChanged, this, [this]() {
    //     emit dataChanged(index(OpenVideoExternal), index(OpenVideoExternal), {Value});
    // });
    // connect(s.get(), &UserSettings::privacyScreenChanged, this, [this]() {
    //     emit dataChanged(index(PrivacyScreen), index(PrivacyScreen), {Value});
    //     emit dataChanged(index(PrivacyScreenTimeout), index(PrivacyScreenTimeout), {Enabled});
    // });
    // connect(s.get(), &UserSettings::privacyScreenTimeoutChanged, this, [this]() {
    //     emit dataChanged(index(PrivacyScreenTimeout), index(PrivacyScreenTimeout), {Value});
    // });

    // connect(s.get(), &UserSettings::timelineMaxWidthChanged, this, [this]() {
    //     emit dataChanged(index(TimelineMaxWidth), index(TimelineMaxWidth), {Value});
    // });
    // connect(s.get(), &UserSettings::messageHoverHighlightChanged, this, [this]() {
    //     emit dataChanged(index(MessageHoverHighlight), index(MessageHoverHighlight), {Value});
    // });
    // connect(s.get(), &UserSettings::enlargeEmojiOnlyMessagesChanged, this, [this]() {
    //     emit dataChanged(index(EnlargeEmojiOnlyMessages), index(EnlargeEmojiOnlyMessages), {Value});
    // });
    // connect(s.get(), &UserSettings::animateImagesOnHoverChanged, this, [this]() {
    //     emit dataChanged(index(AnimateImagesOnHover), index(AnimateImagesOnHover), {Value});
    // });
    // connect(s.get(), &UserSettings::typingNotificationsChanged, this, [this]() {
    //     emit dataChanged(index(TypingNotifications), index(TypingNotifications), {Value});
    // });
    // connect(s.get(), &UserSettings::readReceiptsChanged, this, [this]() {
    //     emit dataChanged(index(ReadReceipts), index(ReadReceipts), {Value});
    // });
    // connect(s.get(), &UserSettings::buttonInTimelineChanged, this, [this]() {
    //     emit dataChanged(index(ButtonsInTimeline), index(ButtonsInTimeline), {Value});
    // });
    // connect(s.get(), &UserSettings::markdownChanged, this, [this]() {
    //     emit dataChanged(index(Markdown), index(Markdown), {Value});
    // });
    // connect(s.get(), &UserSettings::bubblesChanged, this, [this]() {
    //     emit dataChanged(index(Bubbles), index(Bubbles), {Value});
    // });
    // connect(s.get(), &UserSettings::smallAvatarsChanged, this, [this]() {
    //     emit dataChanged(index(SmallAvatars), index(SmallAvatars), {Value});
    // });
    // connect(s.get(), &UserSettings::groupViewStateChanged, this, [this]() {
    //     emit dataChanged(index(GroupView), index(GroupView), {Value});
    // });
    // connect(s.get(), &UserSettings::roomSortingChanged, this, [this]() {
    //     emit dataChanged(index(SortByImportance), index(SortByImportance), {Value});
    // });
    // connect(s.get(), &UserSettings::decryptSidebarChanged, this, [this]() {
    //     emit dataChanged(index(DecryptSidebar), index(DecryptSidebar), {Value});
    // });
    // connect(s.get(), &UserSettings::spaceNotificationsChanged, this, [this] {
    //     emit dataChanged(index(SpaceNotifications), index(SpaceNotifications), {Value});
    // });
    // connect(s.get(), &UserSettings::trayChanged, this, [this]() {
    //     emit dataChanged(index(Tray), index(Tray), {Value});
    //     emit dataChanged(index(StartInTray), index(StartInTray), {Enabled});
    // });
    // connect(s.get(), &UserSettings::startInTrayChanged, this, [this]() {
    //     emit dataChanged(index(StartInTray), index(StartInTray), {Value});
    // });

    // connect(s.get(), &UserSettings::desktopNotificationsChanged, this, [this]() {
    //     emit dataChanged(index(DesktopNotifications), index(DesktopNotifications), {Value});
    // });
    // connect(s.get(), &UserSettings::alertOnNotificationChanged, this, [this]() {
    //     emit dataChanged(index(AlertOnNotification), index(AlertOnNotification), {Value});
    // });

    // connect(s.get(), &UserSettings::useStunServerChanged, this, [this]() {
    //     emit dataChanged(index(UseStunServer), index(UseStunServer), {Value});
    // });
    // connect(s.get(), &UserSettings::ringtoneChanged, this, [this]() {
    //     emit dataChanged(index(Ringtone), index(Ringtone), {Values, Value});
    // });

    connect(s.get(), &UserSettings::onlyShareKeysWithVerifiedUsersChanged, this, [this]() {
        emit dataChanged(
          index(OnlyShareKeysWithVerifiedUsers), index(OnlyShareKeysWithVerifiedUsers), {Value});
    });
    connect(s.get(), &UserSettings::shareKeysWithTrustedUsersChanged, this, [this]() {
        emit dataChanged(
          index(ShareKeysWithTrustedUsers), index(ShareKeysWithTrustedUsers), {Value});
    });
    connect(s.get(), &UserSettings::useOnlineKeyBackupChanged, this, [this]() {
        emit dataChanged(index(UseOnlineKeyBackup), index(UseOnlineKeyBackup), {Value});
    });
    // connect(MainWindow::instance(), &MainWindow::secretsChanged, this, [this]() {
    //     emit dataChanged(index(OnlineBackupKey), index(MasterKey), {Value, Good});
    // });
}
