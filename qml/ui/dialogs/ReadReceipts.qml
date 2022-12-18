// SPDX-FileCopyrightText: 2021 Nheko Contributors
// SPDX-FileCopyrightText: 2022 Nheko Contributors
//
// SPDX-License-Identifier: GPL-3.0-or-later

import ".."
import "../../"
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GlobalObject 1.0
import CursorShape 1.0
import ReadReceiptsProxy 1.0
import TimelineModel 1.0

CustomApplicationWindow {
    id: readReceiptsRoot

    property ReadReceiptsProxy readReceipts
    property TimelineModel timelineModel

    height: 380
    width: 340
    minimumHeight: 380
    minimumWidth: headerTitle.width + 2 * 8
    flags: Qt.Dialog | Qt.WindowCloseButtonHint | Qt.WindowTitleHint

    Shortcut {
        sequence: StandardKey.Cancel
        onActivated: readReceiptsRoot.close()
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 8

        Label {
            id: headerTitle

            color: GlobalObject.colors.text
            Layout.alignment: Qt.AlignCenter
            text: qsTr("Read receipts")
            font.pointSize: fontMetrics.font.pointSize * 1.5
        }

        ScrollView {
            palette: GlobalObject.colors
            padding: 8
            ScrollBar.horizontal.visible: false
            Layout.fillHeight: true
            Layout.minimumHeight: 200
            Layout.fillWidth: true

            ListView {
                id: readReceiptsList

                clip: true
                boundsBehavior: Flickable.StopAtBounds
                model: readReceipts

                delegate: ItemDelegate {
                    id: del

                    // onClicked: room.openUserProfile(model.mxid)
                    padding: 8
                    width: ListView.view.width
                    height: receiptLayout.implicitHeight + 4 * 2
                    hoverEnabled: true
                    ToolTip.visible: hovered
                    ToolTip.text: model.mxid
                    background: Rectangle {
                        color: del.hovered ? GlobalObject.colors.dark : readReceiptsRoot.color
                    }

                    RowLayout {
                        id: receiptLayout

                        spacing: 8
                        anchors.fill: parent
                        anchors.margins: 4

                        Avatar {
                            width: GlobalObject.avatarSize
                            height: GlobalObject.avatarSize
                            userid: model.mxid
                            url: model.avatarUrl.replace("mxc://", "image://MxcImage/")
                            displayName: model.displayName
                            enabled: false
                        }

                        ColumnLayout {
                            spacing: 4

                            Label {
                                text: model.displayName
                                color: timelineModel.userColor(model ? model.mxid : "", GlobalObject.colors.window)
                                font.pointSize: fontMetrics.font.pointSize
                            }

                            Label {
                                text: model.timestamp
                                color: GlobalObject.colors.buttonText
                                font.pointSize: fontMetrics.font.pointSize * 0.9
                            }

                        }

                        Item {
                            Layout.fillWidth: true
                        }

                    }

                    CursorShape {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                    }

                }

            }

        }

    }

    footer: DialogButtonBox {
        standardButtons: DialogButtonBox.Ok
        onAccepted: readReceiptsRoot.close()
        background: Rectangle {
            anchors.fill: parent
            color: GlobalObject.colors.window
        }
    }

}
