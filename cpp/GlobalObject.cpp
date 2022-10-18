// SPDX-FileCopyrightText: 2021 GlobalObject Contributors
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "GlobalObject.h"

#include <QDesktopServices>
#include <QUrl>
#include <QWindow>
#include <matrix-client-library/Client.h>
#include "Configuration.h"

GlobalObject *GlobalObject::_instance  = nullptr;

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
    return Theme::paletteFromTheme(QString("system"));
}

QPalette GlobalObject::inactiveColors() const {
    auto p = colors();
    p.setCurrentColorGroup(QPalette::ColorGroup::Inactive);
    return p;
}

Theme GlobalObject::theme() const {
    return Theme(QString("system"));
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
    material.accent = ANDROID_MATERIAL_ACCENT;
    material.primary = ANDROID_MATERIAL_PRIMARY;
    material.primaryForeground = ANDROID_MATERIAL_PRIMARY_FOREGROUND;
    material.foreground = ANDROID_MATERIAL_FOREGROUND;
    material.background = ANDROID_MATERIAL_BACKGROUND;
    return material;
};

Q_INVOKABLE QString GlobalObject::mediaCachePath(){
    return QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/media_cache";
}
    