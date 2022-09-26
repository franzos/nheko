// SPDX-FileCopyrightText: 2021 Nheko Contributors
// SPDX-FileCopyrightText: 2022 Nheko Contributors
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import GlobalObject 1.0
import CursorShape 1.0
import MatrixClient 1.0
import Presence 1.0
import QmlInterface 1.0

AbstractButton {
    id: avatar

    property string url
    property string userid
    property string roomid
    property string displayName
    property alias textColor: label.color
    property bool crop: true
    property alias color: bg.color

    width: 48
    height: 48
    background: Rectangle {
        id: bg
        radius: height / 2
        color: GlobalObject.colors.alternateBase
    }

    Label {
        id: label

        enabled: false

        anchors.fill: parent
        text: displayName ? String.fromCodePoint(displayName.codePointAt(0)) : "."
        textFormat: Text.RichText
        font.pixelSize: avatar.height / 2
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        visible: img.status != Image.Ready
        color: GlobalObject.colors.text
    }

    Image {
        id: identicon

        anchors.fill: parent
        visible: QmlInterface.jdenticonProviderisAvailable() && img.status != Image.Ready
        source: QmlInterface.jdenticonProviderisAvailable()?"image://jdenticon/" + (userid !== "" ? userid : roomid) + "?radius=" + 100:""
    }

    Image {
        id: img

        anchors.fill: parent
        asynchronous: true
        fillMode: avatar.crop ? Image.PreserveAspectCrop : Image.PreserveAspectFit
        mipmap: true
        smooth: true
        sourceSize.width: avatar.width * Screen.devicePixelRatio
        sourceSize.height: avatar.height * Screen.devicePixelRatio
        source: avatar.url ? (avatar.url + "?radius=" + 100 + ((avatar.crop) ? "" : "&scale")) : ""

    }

    Rectangle {
        id: onlineIndicator

        anchors.bottom: avatar.bottom
        anchors.right: avatar.right
        visible: !!userid
        height: avatar.height / 6
        width: height
        radius: height / 2
        color: updatePresence()

        function updatePresence() {
            if(MatrixClient.presenceEmitter()){
                switch (MatrixClient.presenceEmitter().userPresence(userid)) {
                case "online":
                    return "#00cc66";
                case "unavailable":
                    return "#ff9933";
                case "offline":
                default:
                    // return "#a82353" don't show anything if offline, since it is confusing, if presence is disabled
                    return "transparent";
                }
            } else {
                return "transparent";
            }
        }

        Connections {
            target: MatrixClient.presenceEmitter()

            function onPresenceChanged(id) {
                if (id == userid) onlineIndicator.color = onlineIndicator.updatePresence();
            }
        }
    }

    CursorShape {
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
    }

    Ripple {
        color: Qt.rgba(GlobalObject.colors.alternateBase.r, GlobalObject.colors.alternateBase.g, GlobalObject.colors.alternateBase.b, 0.5)
    }

}
