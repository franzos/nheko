// SPDX-FileCopyrightText: 2021 GlobalObject Contributors
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "GlobalObject.h"

#include <QDesktopServices>
#include <QUrl>
#include <QWindow>
#include <matrix-client-library/Client.h>
#include <QFileDialog>
#include <QString>
#include <QBuffer>
#if defined(Q_OS_ANDROID)
#include <QtAndroid>
#endif
#include "Configuration.h"

GlobalObject *GlobalObject::_instance  = nullptr;

#define SELECTED_THEME  Theme::ThemeMode::System

GlobalObject *GlobalObject::instance(){
    if(_instance == nullptr){
        _instance = new GlobalObject();
    }
    return _instance;
}

GlobalObject::GlobalObject(){
    Q_INIT_RESOURCE(mtx_gui_library_resources);
}

QPalette GlobalObject::colors() const {
    return Theme::paletteFromTheme(SELECTED_THEME);
}

QPalette GlobalObject::inactiveColors() const {
    auto p = colors();
    p.setCurrentColorGroup(QPalette::ColorGroup::Inactive);
    return p;
}

Theme GlobalObject::theme() const {
    return Theme(SELECTED_THEME);
}

void GlobalObject::openLink(QString link) {
    QUrl url(link);
    // Open externally if we couldn't handle it internally
    // auto bg = url.toString(QUrl::ComponentFormattingOption::FullyEncoded).toUtf8();
    if (!GlobalObject::handleMatrixUri(url)) {
        static const QStringList allowedUrlSchemes = {
          QStringLiteral("http"),
          QStringLiteral("https"),
          QStringLiteral("mailto"),
          QStringLiteral("geo"),
        };
        if (allowedUrlSchemes.contains(url.scheme()))
            QDesktopServices::openUrl(url);
        else
            nhlog::ui()->warn("Url '{}' not opened, because the scheme is not in the allow list",
                              url.toDisplayString().toStdString());
    }
}

void GlobalObject::setStatusMessage(QString msg) const {
    Client::instance()->setStatus(msg);
}


static QString
mxidFromSegments(QStringView sigil, QStringView mxid)
{
    if (mxid.isEmpty())
        return QString();

    auto mxid_ = QUrl::fromPercentEncoding(mxid.toUtf8());

    if (sigil == u"u") {
        return "@" + mxid_;
    } else if (sigil == u"roomid") {
        return "!" + mxid_;
    } else if (sigil == u"r") {
        return "#" + mxid_;
        //} else if (sigil == "group") {
        //        return "+" + mxid_;
    } else {
        return QString();
    }
}

bool GlobalObject::handleMatrixUri(const QByteArray &uri) {
    nhlog::ui()->info("Received uri! {}", uri.toStdString());
    QUrl uri_{uri};

    // Convert matrix.to URIs to proper format
    if (uri_.scheme() == QLatin1String("https") && uri_.host() == QLatin1String("matrix.to")) {
        QString p = uri_.fragment(QUrl::FullyEncoded);
        if (p.startsWith(QLatin1String("/")))
            p.remove(0, 1);

        auto temp = p.split(QStringLiteral("?"));
        QString query;
        if (temp.size() >= 2)
            query = QUrl::fromPercentEncoding(temp.takeAt(1).toUtf8());

        temp            = temp.first().split(QStringLiteral("/"));
        auto identifier = QUrl::fromPercentEncoding(temp.takeFirst().toUtf8());
        QString eventId = QUrl::fromPercentEncoding(temp.join('/').toUtf8());
        if (!identifier.isEmpty()) {
            if (identifier.startsWith(QLatin1String("@"))) {
                QByteArray newUri = "matrix:u/" + QUrl::toPercentEncoding(identifier.remove(0, 1));
                if (!query.isEmpty())
                    newUri.append("?" + query.toUtf8());
                return handleMatrixUri(QUrl::fromEncoded(newUri));
            } else if (identifier.startsWith(QLatin1String("#"))) {
                QByteArray newUri = "matrix:r/" + QUrl::toPercentEncoding(identifier.remove(0, 1));
                if (!eventId.isEmpty())
                    newUri.append("/e/" + QUrl::toPercentEncoding(eventId.remove(0, 1)));
                if (!query.isEmpty())
                    newUri.append("?" + query.toUtf8());
                return handleMatrixUri(QUrl::fromEncoded(newUri));
            } else if (identifier.startsWith(QLatin1String("!"))) {
                QByteArray newUri =
                  "matrix:roomid/" + QUrl::toPercentEncoding(identifier.remove(0, 1));
                if (!eventId.isEmpty())
                    newUri.append("/e/" + QUrl::toPercentEncoding(eventId.remove(0, 1)));
                if (!query.isEmpty())
                    newUri.append("?" + query.toUtf8());
                return handleMatrixUri(QUrl::fromEncoded(newUri));
            }
        }
    }

    // non-matrix URIs are not handled by us, return false
    if (uri_.scheme() != QLatin1String("matrix"))
        return false;

    auto tempPath = uri_.path(QUrl::ComponentFormattingOption::FullyEncoded);
    if (tempPath.startsWith('/'))
        tempPath.remove(0, 1);
    auto segments = QStringView(tempPath).split('/');

    if (segments.size() != 2 && segments.size() != 4)
        return false;

    auto sigil1 = segments[0];
    auto mxid1  = mxidFromSegments(sigil1, segments[1]);
    if (mxid1.isEmpty())
        return false;

    QString mxid2;
    if (segments.size() == 4 && segments[2] == QStringView(u"e")) {
        if (segments[3].isEmpty())
            return false;
        else
            mxid2 = "$" + QUrl::fromPercentEncoding(segments[3].toUtf8());
    }

    std::vector<std::string> vias;
    QString action;

    auto items =
      uri_.query(QUrl::ComponentFormattingOption::FullyEncoded).split('&', Qt::SkipEmptyParts);
    for (QString item : qAsConst(items)) {
        nhlog::ui()->info("item: {}", item.toStdString());

        if (item.startsWith(QLatin1String("action="))) {
            action = item.remove(QStringLiteral("action="));
        } else if (item.startsWith(QLatin1String("via="))) {
            vias.push_back(QUrl::fromPercentEncoding(item.remove(QStringLiteral("via=")).toUtf8())
                             .toStdString());
        }
    }

    if (sigil1 == u"u") {
        if (action.isEmpty()) {
            nhlog::ui()->warn("TODO: Review and update");
            // auto t = MainWindow::instance()->focusedRoom();
            // if (!t.isEmpty() && cache::isRoomMember(mxid1.toStdString(), t.toStdString())) {
            //     auto rm = view_manager_->rooms()->getRoomById(t);
            //     if (rm)
            //         rm->openUserProfile(mxid1);
            //     return true;
            // }
            // emit view_manager_->openGlobalUserProfile(mxid1);
        } else if (action == u"chat") {
            Client::instance()->startChat(mxid1);
        }
        return true;
    } else if (sigil1 == u"roomid") {
        auto joined_rooms = cache::joinedRooms();
        auto targetRoomId = mxid1;

        for (const auto &roomid : joined_rooms) {
            if (roomid == targetRoomId.toStdString()) {
                nhlog::ui()->warn("TODO: Review and update");
                // view_manager_->rooms()->setCurrentRoom(mxid1);
                // if (!mxid2.isEmpty())
                //     view_manager_->showEvent(mxid1, mxid2);
                return true;
            }
        }

        if (action == u"join" || action.isEmpty()) {
            Client::instance()->joinRoomVia(targetRoomId, vias);
            return true;
        } else if (action == u"knock" || action.isEmpty()) {
            nhlog::ui()->warn("TODO: Review and update");
            // knockRoom(mxid1, vias);
            return true;
        }
        return false;
    } else if (sigil1 == u"r") {
        auto joined_rooms    = cache::joinedRooms();
        auto targetRoomAlias = mxid1.toStdString();

        for (const auto &roomid : joined_rooms) {
            auto aliases = Client::instance()->timeline(QString::fromStdString(roomid))->getRoomAliases();
            if (aliases) {
                if (aliases->alias == targetRoomAlias) {
                    nhlog::ui()->warn("TODO: Review and update");
                    // view_manager_->rooms()->setCurrentRoom(QString::fromStdString(roomid));
                    // if (!mxid2.isEmpty())
                    //     view_manager_->showEvent(QString::fromStdString(roomid), mxid2);
                    return true;
                }
            }
        }

        if (action == u"join" || action.isEmpty()) {
            Client::instance()->joinRoomVia(mxid1, vias);
            return true;
        } else if (action == u"knock" || action.isEmpty()) {
            nhlog::ui()->warn("TODO: Review and update");
            // knockRoom(mxid1, vias);
            return true;
        }
        return false;
    }
    return false;
}


bool GlobalObject::handleMatrixUri(const QUrl &uri) {
    return handleMatrixUri(uri.toString(QUrl::ComponentFormattingOption::FullyEncoded).toUtf8());
}

QString GlobalObject::checkMatrixServerUrl(QString url){
    if (url[url.size() -1] == "/")
        url = url.remove(url.size() - 1, 1);
    return url;
}

Q_INVOKABLE AndroidMaterialTheme GlobalObject::materialColors(){
    AndroidMaterialTheme material;
    material.accent = ANDROID_MATERIAL_ACCENT; //colors().buttonText().color().name(QColor::HexArgb);
    material.primary = ANDROID_MATERIAL_PRIMARY;
    material.primaryForeground = ANDROID_MATERIAL_PRIMARY_FOREGROUND;
    material.foreground = ANDROID_MATERIAL_FOREGROUND;
    material.background = ANDROID_MATERIAL_BACKGROUND;
    return material;
};

Q_INVOKABLE QString GlobalObject::mediaCachePath(){
    return QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/media_cache";
}

QString GlobalObject::getSaveFileName(const QString &caption,
                                   const QString &dir,
                                   const QString &selectedFile,
                                   const QString &filter){
#ifdef Q_OS_ANDROID
    Q_UNUSED(caption);
    QtAndroid::PermissionResultMap res = QtAndroid::requestPermissionsSync({"android.permission.WRITE_EXTERNAL_STORAGE"});
    if (res["android.permission.WRITE_EXTERNAL_STORAGE"] != QtAndroid::PermissionResult::Granted){
        nhlog::ui()->warn("Don't have permission to write here \"" + dir.toStdString() + "\"");
        return "";
    }
    return QFileDialog::getSaveFileName(nullptr, selectedFile, dir, filter);
#else
    return QFileDialog::getSaveFileName(nullptr, caption, dir + "/" + selectedFile, filter);
#endif
}

void GlobalObject::saveBufferToFile(const QString &filename, const QBuffer &buffer) {
    if(filename.isEmpty())
        return;
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
        return;
    file.write(buffer.data());
    file.close();
    nhlog::ui()->info(QString("File stored in \"" + filename +"\" (size: " + QString::number(buffer.size()) + ")").toStdString());
}

void GlobalObject::saveAs(const QString &source, const QString &dst){
    if (source.isEmpty() || dst.isEmpty())
        return;

    QFile dstFile(source);
    if (dstFile.open(QIODevice::ReadOnly)) {
        auto data = dstFile.readAll();
        saveBufferToFile(dst, QBuffer(&data));
        dstFile.close();
    }
}

Q_INVOKABLE bool GlobalObject::mobileMode(){
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
    return true;
#else
    return false;
#endif
}

Q_INVOKABLE QString GlobalObject::themeName(){
    // TODO improvment: access enum object in qml instead of string
    if (SELECTED_THEME == Theme::ThemeMode::Light)
        return "light";
    else if (SELECTED_THEME == Theme::ThemeMode::Dark)
        return "dark";
    return "system";
}

Q_INVOKABLE void
GlobalObject::createRoom(bool space,
                  QString name,
                  QString topic,
                  QString aliasLocalpart,
                  bool isEncrypted,
                  int preset)
{
    mtx::requests::CreateRoom req;

    if (space) {
        req.creation_content       = mtx::events::state::Create{};
        req.creation_content->type = mtx::events::state::room_type::space;
        req.creation_content->creator.clear();
        req.creation_content->room_version.clear();
    }

    switch (preset) {
    case 1:
        req.preset = mtx::requests::Preset::PublicChat;
        break;
    case 2:
        req.preset = mtx::requests::Preset::TrustedPrivateChat;
        break;
    case 0:
    default:
        req.preset = mtx::requests::Preset::PrivateChat;
    }

    req.name            = name.toStdString();
    req.topic           = topic.toStdString();
    req.room_alias_name = aliasLocalpart.toStdString();

    if (isEncrypted) {
        mtx::events::StrippedEvent<mtx::events::state::Encryption> enc;
        enc.type              = mtx::events::EventType::RoomEncryption;
        enc.content.algorithm = mtx::crypto::MEGOLM_ALGO;
        req.initial_state.emplace_back(std::move(enc));
    }

    emit Client::instance()->createRoom(req);
}

Q_INVOKABLE bool GlobalObject::isLocationPermissionGranted(){
#ifdef Q_OS_ANDROID
    auto result = QtAndroid::checkPermission("android.permission.ACCESS_FINE_LOCATION");
    if(result == QtAndroid::PermissionResult::Denied) {
        return false;
    }
    return true;
#else
    return false;
#endif
}

Q_INVOKABLE bool GlobalObject::requestCameraPermission() {
#ifdef Q_OS_ANDROID
    QtAndroid::PermissionResultMap result = QtAndroid::requestPermissionsSync(QStringList({"android.permission.CAMERA"}));
    if (result["android.permission.CAMERA"] == QtAndroid::PermissionResult::Denied) {
        nhlog::ui()->warn("Don't have permission to use camera");
        return false;
    }
#endif
    return true;

}

Q_INVOKABLE bool GlobalObject::requestMicrophonePermission() {
#ifdef Q_OS_ANDROID
    QtAndroid::PermissionResultMap result = QtAndroid::requestPermissionsSync(QStringList({"android.permission.RECORD_AUDIO"}));
    if (result["android.permission.RECORD_AUDIO"] == QtAndroid::PermissionResult::Denied) {
        nhlog::ui()->warn("Don't have permission to use microphone");
        return false;
    }
#endif
    return true;
}
