// SPDX-FileCopyrightText: 2021 Nheko Contributors
// SPDX-FileCopyrightText: 2022 Nheko Contributors
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GlobalObject 1.0
import Olm 1.0
import "ui"

Rectangle {
    id: r

    required property int encryptionError
    required property string eventId

    radius: fontMetrics.lineSpacing / 2 + 8
    width: parent.width? parent.width : 0
    implicitWidth: encryptedText.implicitWidth+24+8*3 // Column doesn't provide a useful implicitWidth, should be replaced by ColumnLayout
    height: contents.implicitHeight + 8 * 2
    color: GlobalObject.colors.alternateBase

    RowLayout {
        id: contents

        anchors.fill: parent
        anchors.margins: 8
        spacing: 8

        Image {
            source: "image://colorimage/:/images/shield-filled-cross.svg?" + GlobalObject.theme.error
            Layout.alignment: Qt.AlignVCenter
            width: 24
            height: width
        }

        Column {
            spacing: 4
            Layout.fillWidth: true

            MatrixText {
                id: encryptedText
                text: {
                    switch (encryptionError) {
                    case Olm.MissingSession:
                        return qsTr("There is no key to unlock this message. We requested the key automatically, but you can try requesting it again if you are impatient.");
                    case Olm.MissingSessionIndex:
                        return qsTr("This message couldn't be decrypted, because we only have a key for newer messages. You can try requesting access to this message.");
                    case Olm.DbError:
                        return qsTr("There was an internal error reading the decryption key from the database.");
                    case Olm.DecryptionFailed:
                        return qsTr("There was an error decrypting this message.");
                    case Olm.ParsingFailed:
                        return qsTr("The message couldn't be parsed.");
                    case Olm.ReplayAttack:
                        return qsTr("The encryption key was reused! Someone is possibly trying to insert false messages into this chat!");
                    default:
                        return qsTr("Unknown decryption error");
                    }
                }
                color: GlobalObject.colors.text
                width: parent.width
            }

            Button {
                palette: GlobalObject.colors
                visible: encryptionError == Olm.MissingSession || encryptionError == Olm.MissingSessionIndex
                text: qsTr("Request key")
                onClicked: room.requestKeyForEvent(eventId)
            }

        }

    }

}
