// SPDX-FileCopyrightText: 2021 Nheko Contributors
// SPDX-FileCopyrightText: 2022 Nheko Contributors
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "MxcMediaProxy.h"

#include <QDir>
#include <QFile>
#include <QMediaMetaData>
#include <QMediaObject>
#include <QMediaPlayer>
#include <QMimeDatabase>
#include <QUrl>
#include <QFileDialog>
#include "../GlobalObject.h"
#if defined(Q_OS_MACOS)
// TODO (red_sky): Remove for Qt6.  See other ifdef below
#include <QTemporaryFile>
#elif defined(Q_OS_ANDROID)
#include <QtAndroid>
#endif

#include <matrix-client-library/EventAccessors.h>
#include <matrix-client-library/MatrixClient.h>

MxcMediaProxy::MxcMediaProxy(QObject *parent)
  : QObject(parent)
{
    connect(this, &MxcMediaProxy::eventIdChanged, &MxcMediaProxy::checkMediaFileExist);
    connect(this, &MxcMediaProxy::roomChanged, &MxcMediaProxy::checkMediaFileExist);
}

// TODO: Dupplication code in checkMediaFileExist and startDownload
void MxcMediaProxy::checkMediaFileExist(){
    if (!room_)
        return;

    if (eventId_.isEmpty())
        return;

    auto event = room_->eventById(eventId_);
    if (!event) {
        nhlog::ui()->error("Failed to load media for event {}, event not found.",
                           eventId_.toStdString());
        return;
    }

    QString mxcUrl   = QString::fromStdString(mtx::accessors::url(*event));
    QString mimeType = QString::fromStdString(mtx::accessors::mimetype(*event));

    auto encryptionInfo = mtx::accessors::file(*event);

    // If the message is a link to a non mxcUrl, don't download it
    if (!mxcUrl.startsWith(QLatin1String("mxc://"))) {
        return;
    }

    QString suffix = QMimeDatabase().mimeTypeForName(mimeType).preferredSuffix();

    const auto url  = mxcUrl.toStdString();
    const auto name = QString(mxcUrl).remove(QStringLiteral("mxc://"));

    QFileInfo filename(
      QStringLiteral("%1/media/%2.%3")
        .arg(GlobalObject::instance()->mediaCachePath(), name, suffix));
    if (QDir::cleanPath(name) != name) {
        nhlog::net()->warn("mxcUrl '{}' is not safe, not downloading file", url);
        return;
    }

    QDir().mkpath(filename.path());
    QPointer<MxcMediaProxy> self = this;
    
    if (filename.isReadable()) {
        QFile f(filename.filePath());
        if (f.open(QIODevice::ReadOnly)) {
            setMediaFile(filename);
            return;
        }
    }
}    

void
MxcMediaProxy::startDownload()
{
    if (!room_)
        return;

    if (eventId_.isEmpty())
        return;

    auto event = room_->eventById(eventId_);
    if (!event) {
        nhlog::ui()->error("Failed to load media for event {}, event not found.",
                           eventId_.toStdString());
        return;
    }

    QString mxcUrl   = QString::fromStdString(mtx::accessors::url(*event));
    QString mimeType = QString::fromStdString(mtx::accessors::mimetype(*event));

    auto encryptionInfo = mtx::accessors::file(*event);

    // If the message is a link to a non mxcUrl, don't download it
    if (!mxcUrl.startsWith(QLatin1String("mxc://"))) {
        return;
    }

    QString suffix = QMimeDatabase().mimeTypeForName(mimeType).preferredSuffix();

    const auto url  = mxcUrl.toStdString();
    const auto name = QString(mxcUrl).remove(QStringLiteral("mxc://"));

    QFileInfo filename(
      QStringLiteral("%1/media/%2.%3")
        .arg(GlobalObject::instance()->mediaCachePath(), name, suffix));
    if (QDir::cleanPath(name) != name) {
        nhlog::net()->warn("mxcUrl '{}' is not safe, not downloading file", url);
        return;
    }

    QDir().mkpath(filename.path());
    QPointer<MxcMediaProxy> self = this;
    
    const QString defaultFilePath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + mediaFile_.fileName();
    #ifdef Q_OS_ANDROID
    QtAndroid::PermissionResultMap res = QtAndroid::requestPermissionsSync({"android.permission.WRITE_EXTERNAL_STORAGE"});
    if (res["android.permission.WRITE_EXTERNAL_STORAGE"] != QtAndroid::PermissionResult::Granted)
        nhlog::ui()->warn("Don't have permission to write here \"" + defaultFilePath.toStdString() + "\"");
    #endif
    const auto saveAsFilename =
      QFileDialog::getSaveFileName(nullptr, tr("Save as ..."), defaultFilePath, suffix);

    if (filename.isReadable()) {
        QFile f(filename.filePath());
        if (f.open(QIODevice::ReadOnly)) {
            saveAs(saveAsFilename);
            setMediaFile(filename);
            return;
        }
    }

    http::client()->download(url,
                             [this, saveAsFilename, filename, url, encryptionInfo](const std::string &data,
                                                            const std::string &,
                                                            const std::string &,
                                                            mtx::http::RequestErr err) {
                                 if (err) {
                                     nhlog::net()->warn("failed to retrieve media {}: {} {}",
                                                        url,
                                                        err->matrix_error.error,
                                                        static_cast<int>(err->status_code));
                                     return;
                                 }

                                 try {
                                     QByteArray ba(data.data(), (int)data.size());
                                     QBuffer buffer;
                                     if (encryptionInfo) {
                                         std::string temp(ba.constData(), ba.size());
                                         temp = mtx::crypto::to_string(mtx::crypto::decrypt_file(temp, encryptionInfo.value()));
                                         buffer.setData(temp.data(), temp.size());
                                     } else {
                                         buffer.setData(ba);
                                     }
                                     
                                     QFile file(filename.filePath());
                                     if (!file.open(QIODevice::WriteOnly))
                                         return;
                                     
                                     file.write(buffer.data());
                                     file.close();
                                     saveAs(saveAsFilename);
                                     setMediaFile(filename);
                                 } catch (const std::exception &e) {
                                     nhlog::ui()->warn("Error while saving file to: {}", e.what());
                                 }
                             });
}

void MxcMediaProxy::setMediaFile(const QFileInfo &fileinfo){
    mediaFile_ = QUrl::fromLocalFile(fileinfo.absoluteFilePath());
    nhlog::ui()->info("Media file loaded (" + fileinfo.absoluteFilePath().toStdString() + 
                        ", size: " + std::to_string(fileinfo.size()) + " bytes)");
    emit mediaFilehanged();
}

void MxcMediaProxy::saveAs(const QString &filename){
    if (filename.isEmpty())
        return;
    if(QFile::copy(mediaFile_.toLocalFile(), filename)){
        nhlog::ui()->info("File saved in " + filename.toStdString());
    } else {
        nhlog::ui()->warn("Failure in file saving in " + filename.toStdString());
    }
}
