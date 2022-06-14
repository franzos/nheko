// SPDX-FileCopyrightText: 2021 Nheko Contributors
// SPDX-FileCopyrightText: 2022 Nheko Contributors
//
// SPDX-License-Identifier: GPL-3.0-or-later

import Qt.labs.platform 1.1 as Platform
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.2
import QtQuick.Window 2.15
import GlobalObject 1.0
import CursorShape 1.0
import MtxEvent 1.0
import TimelineModel 1.0
import "ui"

Pane {
    id: topBar
    property var room : timelineModel

    Layout.fillWidth: true
    implicitHeight: topLayout.height + 8 * 2
    z: 3

    padding: 0
    background: Rectangle {
        color: GlobalObject.colors.window
    }

    contentItem: Item {
        GridLayout {
            id: topLayout

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 8
            anchors.verticalCenter: parent.verticalCenter
            columnSpacing: 4
            rowSpacing: 4

            ScrollView {
                id: pinnedMessages

                Layout.row: 2
                Layout.column: 2
                Layout.columnSpan: 3

                Layout.fillWidth: true
                Layout.preferredHeight: Math.min(contentHeight, GlobalObject.avatarSize * 4)

                visible: !!room && room.pinnedMessages.length > 0 
                clip: true

                palette: GlobalObject.colors
                ScrollBar.horizontal.visible: false

                ListView {

                    spacing: 4
                    model: room ? room.pinnedMessages : undefined
                    delegate: RowLayout {
                        required property string modelData

                        width: ListView.view.width
                        height: implicitHeight

                        Reply {
                            property var e: room ? room.getDump(modelData, "") : {}
                            Layout.fillWidth: true
                            Layout.preferredHeight: height

                            userColor: room.userColor(e.userId, GlobalObject.colors.window)
                            blurhash: e.blurhash ?? ""
                            body: e.body ?? ""
                            formattedBody: e.formattedBody ?? ""
                            eventId: e.eventId ?? ""
                            filename: e.filename ?? ""
                            filesize: e.filesize ?? ""
                            proportionalHeight: e.proportionalHeight ?? 1
                            type: e.type ?? MtxEvent.UnknownMessage
                            typeString: e.typeString ?? ""
                            url: e.url ?? ""
                            originalWidth: e.originalWidth ?? 0
                            isOnlyEmoji: e.isOnlyEmoji ?? false
                            userId: e.userId ?? ""
                            userName: e.userName ?? ""
                            encryptionError: e.encryptionError ?? ""
                        }

                        ImageButton {
                            id: deletePinButton

                            Layout.preferredHeight: 16
                            Layout.preferredWidth: 16
                            Layout.alignment: Qt.AlignTop | Qt.AlignLeft
                            visible: room.permissions.canChange(MtxEvent.PinnedEvents)

                            hoverEnabled: true
                            image: ":/images/dismiss.svg"
                            ToolTip.visible: hovered
                            ToolTip.text: qsTr("Unpin")

                            onClicked: room.unpin(modelData)
                        }
                    }

                    ScrollHelper {
                        flickable: parent
                        anchors.fill: parent
                        enabled: true
                    }
                }
            }
        }
    }
    Connections {
        function onPinnedMessagesChanged() {
            // TODO
        }

        target: room
    }
}
