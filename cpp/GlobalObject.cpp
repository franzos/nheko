// SPDX-FileCopyrightText: 2021 GlobalObject Contributors
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "GlobalObject.h"

#include <QDesktopServices>
#include <QUrl>
#include <QWindow>
#include <matrix-client-library/Client.h>

GlobalObject::GlobalObject(){
    Q_INIT_RESOURCE(mtx_gui_library_resources);
}

QPalette GlobalObject::colors() const {
    return Theme::paletteFromTheme("system");
}

QPalette GlobalObject::inactiveColors() const {
    auto p = colors();
    p.setCurrentColorGroup(QPalette::ColorGroup::Inactive);
    return p;
}

Theme GlobalObject::theme() const {
    return Theme("system");
}

void GlobalObject::openLink(QString link) {
    QUrl url(link);
    // Open externally if we couldn't handle it internally
    // auto bg = url.toString(QUrl::ComponentFormattingOption::FullyEncoded).toUtf8();
    if (!GlobalObject::handleMatrixUri(url)) {
        const QStringList allowedUrlSchemes = {
          "http",
          "https",
          "mailto",
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

bool GlobalObject::handleMatrixUri(const QByteArray &uri) {
    // nhlog::ui()->info("Received uri! {}", uri.toStdString());
    // QUrl uri_{QString::fromUtf8(uri)};

    // // Convert matrix.to URIs to proper format
    // if (uri_.scheme() == "https" && uri_.host() == "matrix.to") {
    //     QString p = uri_.fragment(QUrl::FullyEncoded);
    //     if (p.startsWith("/"))
    //         p.remove(0, 1);

    //     auto temp = p.split("?");
    //     QString query;
    //     if (temp.size() >= 2)
    //         query = QUrl::fromPercentEncoding(temp.takeAt(1).toUtf8());

    //     temp            = temp.first().split("/");
    //     auto identifier = QUrl::fromPercentEncoding(temp.takeFirst().toUtf8());
    //     QString eventId = QUrl::fromPercentEncoding(temp.join('/').toUtf8());
    //     if (!identifier.isEmpty()) {
    //         if (identifier.startsWith("@")) {
    //             QByteArray newUri = "matrix:u/" + QUrl::toPercentEncoding(identifier.remove(0, 1));
    //             if (!query.isEmpty())
    //                 newUri.append("?" + query.toUtf8());
    //             return handleMatrixUri(QUrl::fromEncoded(newUri));
    //         } else if (identifier.startsWith("#")) {
    //             QByteArray newUri = "matrix:r/" + QUrl::toPercentEncoding(identifier.remove(0, 1));
    //             if (!eventId.isEmpty())
    //                 newUri.append("/e/" + QUrl::toPercentEncoding(eventId.remove(0, 1)));
    //             if (!query.isEmpty())
    //                 newUri.append("?" + query.toUtf8());
    //             return handleMatrixUri(QUrl::fromEncoded(newUri));
    //         } else if (identifier.startsWith("!")) {
    //             QByteArray newUri =
    //               "matrix:roomid/" + QUrl::toPercentEncoding(identifier.remove(0, 1));
    //             if (!eventId.isEmpty())
    //                 newUri.append("/e/" + QUrl::toPercentEncoding(eventId.remove(0, 1)));
    //             if (!query.isEmpty())
    //                 newUri.append("?" + query.toUtf8());
    //             return handleMatrixUri(QUrl::fromEncoded(newUri));
    //         }
    //     }
    // }

    // // non-matrix URIs are not handled by us, return false
    // if (uri_.scheme() != "matrix")
    //     return false;

    // auto tempPath = uri_.path(QUrl::ComponentFormattingOption::FullyEncoded);
    // if (tempPath.startsWith('/'))
    //     tempPath.remove(0, 1);
    // auto segments = tempPath.splitRef('/');

    // if (segments.size() != 2 && segments.size() != 4)
    //     return false;

    // auto sigil1 = segments[0];
    // auto mxid1  = mxidFromSegments(sigil1, segments[1]);
    // if (mxid1.isEmpty())
    //     return false;

    // QString mxid2;
    // if (segments.size() == 4 && segments[2] == "e") {
    //     if (segments[3].isEmpty())
    //         return false;
    //     else
    //         mxid2 = "$" + QUrl::fromPercentEncoding(segments[3].toUtf8());
    // }

    // std::vector<std::string> vias;
    // QString action;

    // for (QString item : uri_.query(QUrl::ComponentFormattingOption::FullyEncoded).split('&')) {
    //     nhlog::ui()->info("item: {}", item.toStdString());

    //     if (item.startsWith("action=")) {
    //         action = item.remove("action=");
    //     } else if (item.startsWith("via=")) {
    //         vias.push_back(QUrl::fromPercentEncoding(item.remove("via=").toUtf8()).toStdString());
    //     }
    // }

    // if (sigil1 == "u") {
    //     if (action.isEmpty()) {
    //         auto t = view_manager_->rooms()->currentRoom();
    //         if (t && cache::isRoomMember(mxid1.toStdString(), t->roomId().toStdString())) {
    //             t->openUserProfile(mxid1);
    //             return true;
    //         }
    //         emit view_manager_->openGlobalUserProfile(mxid1);
    //     } else if (action == "chat") {
    //         this->startChat(mxid1);
    //     }
    //     return true;
    // } else if (sigil1 == "roomid") {
    //     auto joined_rooms = cache::joinedRooms();
    //     auto targetRoomId = mxid1.toStdString();

    //     for (auto roomid : joined_rooms) {
    //         if (roomid == targetRoomId) {
    //             view_manager_->rooms()->setCurrentRoom(mxid1);
    //             if (!mxid2.isEmpty())
    //                 view_manager_->showEvent(mxid1, mxid2);
    //             return true;
    //         }
    //     }

    //     if (action == "join" || action.isEmpty()) {
    //         joinRoomVia(targetRoomId, vias);
    //         return true;
    //     }
    //     return false;
    // } else if (sigil1 == "r") {
    //     auto joined_rooms    = cache::joinedRooms();
    //     auto targetRoomAlias = mxid1.toStdString();

    //     for (auto roomid : joined_rooms) {
    //         auto aliases = cache::client()->getRoomAliases(roomid);
    //         if (aliases) {
    //             if (aliases->alias == targetRoomAlias) {
    //                 view_manager_->rooms()->setCurrentRoom(QString::fromStdString(roomid));
    //                 if (!mxid2.isEmpty())
    //                     view_manager_->showEvent(QString::fromStdString(roomid), mxid2);
    //                 return true;
    //             }
    //         }
    //     }

    //     if (action == "join" || action.isEmpty()) {
    //         joinRoomVia(mxid1.toStdString(), vias);
    //         return true;
    //     }
    //     return false;
    // }
    return false;
}


bool GlobalObject::handleMatrixUri(const QUrl &uri) {
    return handleMatrixUri(uri.toString(QUrl::ComponentFormattingOption::FullyEncoded).toUtf8());
}