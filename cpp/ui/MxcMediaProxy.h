// SPDX-FileCopyrightText: 2021 Nheko Contributors
// SPDX-FileCopyrightText: 2022 Nheko Contributors
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QBuffer>
#include <QObject>
#include <QPointer>
#include <QString>
#include <QFileInfo>
#include "../TimelineModel.h"

class TimelineModel;

class MxcMediaProxy : public QObject
{
    Q_OBJECT
    Q_PROPERTY(TimelineModel *roomm READ room WRITE setRoom NOTIFY roomChanged REQUIRED)
    Q_PROPERTY(QString eventId READ eventId WRITE setEventId NOTIFY eventIdChanged)
    Q_PROPERTY(QUrl mediaFile READ mediaFile NOTIFY mediaFilehanged)

public:
    MxcMediaProxy(QObject *parent = nullptr);

    QString eventId() const { return eventId_; }
    TimelineModel *room() const { return room_; }
    void setEventId(QString newEventId)
    {
        eventId_ = newEventId;
        emit eventIdChanged();
    }
    void setRoom(TimelineModel *room)
    {
        room_ = room;
        emit roomChanged();
    }
    QUrl mediaFile() const {return mediaFile_;}
    void setMediaFile(const QFileInfo &fileinfo);

signals:
    void roomChanged();
    void eventIdChanged();
    void loadedChanged();
    void mediaFilehanged();

public slots:
    void checkMediaFileExist();
    void startDownload(bool justCache);

private:
    TimelineModel *room_ = nullptr;
    QString eventId_;
    QUrl mediaFile_;
};
