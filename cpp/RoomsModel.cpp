// SPDX-FileCopyrightText: 2021 Nheko Contributors
// SPDX-FileCopyrightText: 2022 Nheko Contributors
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "RoomsModel.h"

#include <QUrl>
#include "CompletionModelRoles.h"

RoomsModel::RoomsModel(bool showOnlyRoomWithAliases, QObject *parent)
  : QAbstractListModel(parent)
  , showOnlyRoomWithAliases_(showOnlyRoomWithAliases)
{
    roomInfos = Client::instance()->joinedRoomList().toStdMap();
    
    if (!showOnlyRoomWithAliases_) {
        roomids.reserve(roomInfos.size());
        roomAliases.reserve(roomInfos.size());
    }

    for (const auto &r : roomInfos) {
        auto roomAliasesList = Client::instance()->getRoomAliases(r.first);

        if (showOnlyRoomWithAliases_) {
            if (roomAliasesList && !roomAliasesList->alias.empty()) {
                roomids.push_back(r.first);
                roomAliases.push_back(QString::fromStdString(roomAliasesList->alias));
            }
        } else {
            roomids.push_back(r.first);
            roomAliases.push_back(roomAliasesList ? QString::fromStdString(roomAliasesList->alias)
                                                  : QLatin1String(""));
        }
    }
}

QHash<int, QByteArray>
RoomsModel::roleNames() const
{
    return {{CompletionModel::CompletionRole, "completionRole"},
            {CompletionModel::SearchRole, "searchRole"},
            {CompletionModel::SearchRole2, "searchRole2"},
            {Roles::RoomAlias, "roomAlias"},
            {Roles::AvatarUrl, "avatarUrl"},
            {Roles::RoomID, "roomid"},
            {Roles::RoomName, "roomName"}};
}

QVariant
RoomsModel::data(const QModelIndex &index, int role) const
{
    if (hasIndex(index.row(), index.column(), index.parent())) {
        switch (role) {
        case CompletionModel::CompletionRole: {
            if (false) { //(UserSettings::instance()->markdown()) {
                QString percentEncoding = QUrl::toPercentEncoding(roomAliases[index.row()]);
                return QStringLiteral("[%1](https://matrix.to/#/%2)")
                  .arg(roomAliases[index.row()], percentEncoding);
            } else {
                return roomAliases[index.row()];
            }
        }
        case CompletionModel::SearchRole:
        case Qt::DisplayRole:
        case Roles::RoomAlias:
            return roomAliases[index.row()].toHtmlEscaped();
        case CompletionModel::SearchRole2:
        case Roles::RoomName:
            return roomInfos.at(roomids[index.row()]).name.toHtmlEscaped();
        case Roles::AvatarUrl:
            return roomInfos.at(roomids[index.row()]).avatar_url;
        case Roles::RoomID:
            return roomids[index.row()].toHtmlEscaped();
        }
    }
    return {};
}
