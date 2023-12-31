// SPDX-FileCopyrightText: 2021 Nheko Contributors
// SPDX-FileCopyrightText: 2022 Nheko Contributors
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.2
import QtQuick.Window 2.13
import GlobalObject 1.0
import MtxEvent 1.0
import TimelineModel 1.0
import "ui"

AbstractButton {
    id: r

    required property double proportionalHeight
    required property int type
    required property string typeString
    required property int originalWidth
    required property string blurhash
    required property string body
    required property string formattedBody
    required property string eventId
    required property string filename
    required property string geoUri
    required property string filesize
    required property string url
    required property string thumbnailUrl
    required property bool isOnlyEmoji
    required property bool isSender
    required property bool isEncrypted
    required property bool isEditable
    required property bool isEdited
    required property bool isStateEvent
    required property string replyTo
    required property string userId
    required property string userName
    required property string roomTopic
    required property string roomName
    required property string callType
    required property var reactions
    required property int trustlevel
    required property int encryptionError
    required property int duration
    required property var timestamp
    required property int status
    required property int relatedEventCacheBuster

    hoverEnabled: true

    width: parent.width
    height: row.height+(reactionRow.height > 0 ? reactionRow.height-2 : 0 )
    // Updated to improve working in mobile mode
    MouseArea {
        anchors.fill: parent
        onPressAndHold: {
            messageContextMenu.show(eventId, type, isSender, isEncrypted, isEditable, contentItem.child.hoveredLink, contentItem.child.copyText, r, mouse.x, mouse.y)
        }

        onClicked: {
            if(GlobalObject.mobileMode()){
                contentItem.selectByMouse = false
                row.bgColor=GlobalObject.colors.base
            }
        }

        onDoubleClicked: {
            if(GlobalObject.mobileMode()){
                contentItem.selectByMouse = true
                row.bgColor="gray"
            }
        }
    }

    onDoubleClicked: chat.model.reply = eventId

    DragHandler {
        id: draghandler
        yAxis.enabled: false
        xAxis.maximum: 100
        xAxis.minimum: -100
        onActiveChanged: {
            if(!active && (x < -70 || x > 70))
                chat.model.reply = eventId
        }
    }
    states: State {
        name: "dragging"
        when: draghandler.active
    }
    transitions: Transition {
        from: "dragging"
        to: ""
        PropertyAnimation {
            target: r
            properties: "x"
            easing.type: Easing.InOutQuad
            to: 0
            duration: 100
        }
    }

    onClicked: {
        let link = contentItem.child.linkAt != undefined && contentItem.child.linkAt(pressX-row.x-msg.x, pressY-row.y-msg.y-contentItem.y);
        if (link) {
            GlobalObject.openLink(link)
        }
    }

    Rectangle {
        id: row
        property bool bubbleOnRight : isSender
        anchors.leftMargin: isStateEvent ? 0 : GlobalObject.avatarSize+8 // align bubble with section header
        anchors.left: isStateEvent? undefined : (bubbleOnRight? undefined : parent.left)
        anchors.right: isStateEvent? undefined: (bubbleOnRight? parent.right : undefined)
        anchors.horizontalCenter: isStateEvent? parent.horizontalCenter : undefined
        property int maxWidth: (parent.width-(isStateEvent? 0 : GlobalObject.avatarSize+8))*(!isStateEvent? 0.9 : 1)
        width: Math.min(maxWidth,Math.max(reply.implicitWidth+8,contentItem.implicitWidth+metadata.width+20))// Settings.bubbles? Math.min(maxWidth,Math.max(reply.implicitWidth+8,contentItem.implicitWidth+metadata.width+20)) : maxWidth
        height: msg.height+msg.anchors.margins*2

        property color userColor: room.userColor(userId, GlobalObject.colors.base)
        property color bgColor: GlobalObject.colors.base
        color: !isStateEvent ? Qt.tint(bgColor, Qt.hsla(userColor.hslHue, 0.5, userColor.hslLightness, 0.2)) : "#00000000"
        radius: 4

        GridLayout {
            anchors {
                left: parent.left
                top: parent.top
                right: parent.right
                margins: !isStateEvent? 4 : 2//(Settings.bubbles && ! isStateEvent)? 4 : 2
                leftMargin: 4
            }
            id: msg
            rowSpacing: 0
            columnSpacing: 2
            columns: 1//Settings.bubbles? 1 : 2
            rows: 3//Settings.bubbles? 3 : 2
            // fancy reply, if this is a reply
            Reply {
                Layout.row: 0
                Layout.column: 0
                Layout.fillWidth: true
                Layout.maximumWidth: Number.MAX_VALUE//Settings.bubbles? Number.MAX_VALUE : implicitWidth
                Layout.bottomMargin: visible? 2 : 0
                Layout.preferredHeight: height
                id: reply

                function fromModel(role) {
                    return replyTo != "" ? room.dataById(replyTo, role, r.eventId) : null;
                }
                visible: replyTo
                userColor: r.relatedEventCacheBuster, room.userColor(userId, GlobalObject.colors.base)
                blurhash: r.relatedEventCacheBuster, fromModel(TimelineModel.Blurhash) ?? ""
                body: r.relatedEventCacheBuster, fromModel(TimelineModel.Body) ?? ""
                formattedBody: r.relatedEventCacheBuster, fromModel(TimelineModel.FormattedBody) ?? ""
                eventId: fromModel(TimelineModel.EventId) ?? ""
                filename: r.relatedEventCacheBuster, fromModel(TimelineModel.Filename) ?? ""
                geoUri: r.relatedEventCacheBuster, fromModel(TimelineModel.GeoUri) ?? ""
                filesize: r.relatedEventCacheBuster, fromModel(TimelineModel.Filesize) ?? ""
                proportionalHeight: r.relatedEventCacheBuster, fromModel(TimelineModel.ProportionalHeight) ?? 1
                type: r.relatedEventCacheBuster, fromModel(TimelineModel.Type) ?? MtxEvent.UnknownMessage
                typeString: r.relatedEventCacheBuster, fromModel(TimelineModel.TypeString) ?? ""
                url: r.relatedEventCacheBuster, fromModel(TimelineModel.Url) ?? ""
                originalWidth: r.relatedEventCacheBuster, fromModel(TimelineModel.OriginalWidth) ?? 0
                isOnlyEmoji: r.relatedEventCacheBuster, fromModel(TimelineModel.IsOnlyEmoji) ?? false
                isStateEvent: r.relatedEventCacheBuster, fromModel(TimelineModel.IsStateEvent) ?? false
                userId: r.relatedEventCacheBuster, fromModel(TimelineModel.UserId) ?? ""
                userName: r.relatedEventCacheBuster, fromModel(TimelineModel.UserName) ?? ""
                thumbnailUrl: r.relatedEventCacheBuster, fromModel(TimelineModel.ThumbnailUrl) ?? ""
                duration: r.relatedEventCacheBuster, fromModel(TimelineModel.Duration) ?? ""
                roomTopic: r.relatedEventCacheBuster, fromModel(TimelineModel.RoomTopic) ?? ""
                roomName: r.relatedEventCacheBuster, fromModel(TimelineModel.RoomName) ?? ""
                callType: r.relatedEventCacheBuster, fromModel(TimelineModel.CallType) ?? ""
                encryptionError: r.relatedEventCacheBuster, fromModel(TimelineModel.EncryptionError) ?? ""
                relatedEventCacheBuster: r.relatedEventCacheBuster, fromModel(TimelineModel.RelatedEventCacheBuster) ?? 0
            }

            // actual message content
            MessageDelegate {
                Layout.row: 1
                Layout.column: 0
                Layout.fillWidth: true
                Layout.preferredHeight: height
                id: contentItem

                blurhash: r.blurhash
                body: r.body
                formattedBody: r.formattedBody
                eventId: r.eventId
                filename: r.filename
                geoUri: r.geoUri
                filesize: r.filesize
                proportionalHeight: r.proportionalHeight
                type: r.type
                typeString: r.typeString ?? ""
                url: r.url
                thumbnailUrl: r.thumbnailUrl
                duration: r.duration
                originalWidth: r.originalWidth
                isOnlyEmoji: r.isOnlyEmoji
                isStateEvent: r.isStateEvent
                userId: r.userId
                userName: r.userName
                roomTopic: r.roomTopic
                roomName: r.roomName
                callType: r.callType
                encryptionError: r.encryptionError
                relatedEventCacheBuster: r.relatedEventCacheBuster
                isReply: false
                metadataWidth: metadata.width
            }

            Row {
                id: metadata
                Layout.column: 0//Settings.bubbles? 0 : 1
                Layout.row: 2//Settings.bubbles? 2 : 0
                Layout.rowSpan:1// Settings.bubbles? 1 : 2
                Layout.bottomMargin: -2
                // Commented this file because of odd behaviour for last long message height issue #158
                // Layout.topMargin: contentItem.fitsMetadata? -height-Layout.bottomMargin : 0
                Layout.alignment: Qt.AlignTop | Qt.AlignRight
                Layout.preferredWidth: implicitWidth
                visible: !isStateEvent
                spacing: 2

                property double scaling: 0.75//Settings.bubbles? 0.75 : 1

                property int iconSize: Math.floor(fontMetrics.ascent*scaling)

                StatusIndicator {
                    Layout.alignment: Qt.AlignRight | Qt.AlignTop
                    height: parent.iconSize
                    width: parent.iconSize
                    status: r.status
                    eventId: r.eventId
                    anchors.verticalCenter: ts.verticalCenter
                }

                Image {
                    visible: isEdited || eventId == chat.model.edit
                    Layout.alignment: Qt.AlignRight | Qt.AlignTop
                    height: parent.iconSize
                    width: parent.iconSize
                    sourceSize.width: parent.iconSize * Screen.devicePixelRatio
                    sourceSize.height: parent.iconSize * Screen.devicePixelRatio
                    source: "image://colorimage/:/images/edit.svg?" + ((eventId == chat.model.edit) ? GlobalObject.colors.highlight : GlobalObject.colors.buttonText)
                    ToolTip.visible: editHovered.hovered
                    // ToolTip.delay: Nheko.tooltipDelay
                    ToolTip.text: qsTr("Edited")
                    anchors.verticalCenter: ts.verticalCenter

                    HoverHandler {
                        id: editHovered
                    }

                }

                EncryptionIndicator {
                    visible: room.isEncrypted
                    encrypted: isEncrypted
                    trust: trustlevel
                    Layout.alignment: Qt.AlignRight | Qt.AlignTop
                    height: parent.iconSize
                    width: parent.iconSize
                    sourceSize.width: parent.iconSize * Screen.devicePixelRatio
                    sourceSize.height: parent.iconSize * Screen.devicePixelRatio
                    anchors.verticalCenter: ts.verticalCenter
                }

                Label {
                    id: ts
                    Layout.alignment: Qt.AlignRight | Qt.AlignTop
                    Layout.preferredWidth: implicitWidth
                    text: timestamp.toLocaleTimeString(Locale.ShortFormat)
                    color: GlobalObject.inactiveColors.text
                    ToolTip.visible: ma.hovered
                    // ToolTip.delay: Nheko.tooltipDelay
                    ToolTip.text: Qt.formatDateTime(timestamp, Qt.DefaultLocaleLongDate)
                    font.pointSize: fontMetrics.font.pointSize*parent.scaling
                    HoverHandler {
                        id: ma
                    }

                }
            }
        }
    }
    Reactions {
        anchors {
            top: row.bottom
            topMargin: -2
            left: row.bubbleOnRight? undefined : row.left
            right: row.bubbleOnRight? row.right : undefined
        }
        width: row.maxWidth
        layoutDirection: row.bubbleOnRight? Qt.RightToLeft : Qt.LeftToRight

        id: reactionRow

        reactions: r.reactions
        eventId: r.eventId
    }
}
