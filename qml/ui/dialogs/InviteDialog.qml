// SPDX-FileCopyrightText: 2021 Nheko Contributors
// SPDX-FileCopyrightText: 2022 Nheko Contributors
//
// SPDX-License-Identifier: GPL-3.0-or-later

import ".."
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import GlobalObject 1.0
import InviteesModel 1.0
import CursorShape 1.0

ApplicationWindow {
    id: inviteDialogRoot

    property string roomId
    property string plainRoomName
    property InviteesModel invitees

    function addInvite() {
        if (inviteeEntry.isValidMxid) {
            invitees.addUser(inviteeEntry.text);
            inviteeEntry.clear();
        }
    }

    function cleanUpAndClose() {
        if (inviteeEntry.isValidMxid)
            addInvite();

        invitees.accept();
        close();
    }

    title: qsTr("Invite users to %1").arg(plainRoomName)
    height: 380
    width: 340
    palette: GlobalObject.colors
    color: GlobalObject.colors.window
    flags: Qt.Dialog | Qt.WindowCloseButtonHint | Qt.WindowTitleHint

    Shortcut {
        sequence: "Ctrl+Enter"
        onActivated: cleanUpAndClose()
    }

    Shortcut {
        sequence: StandardKey.Cancel
        onActivated: inviteDialogRoot.close()
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 8

        Label {
            text: qsTr("User ID to invite")
            Layout.fillWidth: true
            color: GlobalObject.colors.text
        }

        RowLayout {
            spacing: 8

            MatrixTextField {
                id: inviteeEntry

                property bool isValidMxid: text.match("@.+?:.{3,}")

                backgroundColor: GlobalObject.colors.window
                placeholderText: qsTr("@joe:matrix.org", "Example user id. The name 'joe' can be localized however you want.")
                Layout.fillWidth: true
                onAccepted: {
                    if (isValidMxid)
                        addInvite();

                }
                Component.onCompleted: forceActiveFocus()
                Keys.onShortcutOverride: event.accepted = ((event.key === Qt.Key_Return || event.key === Qt.Key_Enter) && (event.modifiers & Qt.ControlModifier))
                Keys.onPressed: {
                    if ((event.key === Qt.Key_Return || event.key === Qt.Key_Enter) && (event.modifiers === Qt.ControlModifier))
                        cleanUpAndClose();

                }
            }

            Button {
                text: qsTr("Add")
                enabled: inviteeEntry.isValidMxid
                onClicked: addInvite()
            }

        }

        ListView {
            id: inviteesList

            Layout.fillWidth: true
            Layout.fillHeight: true
            model: invitees

            delegate: ItemDelegate {
                id: del

                hoverEnabled: true
                width: ListView.view.width
                height: layout.implicitHeight + 4 * 2
                // onClicked: TimelineManager.openGlobalUserProfile(model.mxid)
                background: Rectangle {
                    color: del.hovered ? GlobalObject.colors.dark : inviteDialogRoot.color
                }

                RowLayout {
                    id: layout

                    spacing: 8
                    anchors.centerIn: parent
                    width: del.width - 4 * 2

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
                            color: timelineModel.userColor(model ? model.mxid : "", del.background.color)
                            font.pointSize: fontMetrics.font.pointSize
                        }

                        Label {
                            text: model.mxid
                            color: del.hovered ? GlobalObject.colors.brightText : GlobalObject.colors.buttonText
                            font.pointSize: fontMetrics.font.pointSize * 0.9
                        }

                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    ImageButton {
                        image: ":/images/dismiss.svg"
                        onClicked: invitees.removeUser(model.mxid)
                    }

                }

                CursorShape {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                }

            }

        }

    }

    footer: DialogButtonBox {
        id: buttons

        Button {
            text: qsTr("Invite")
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
            enabled: invitees.count > 0
            onClicked: cleanUpAndClose()
        }

        Button {
            text: qsTr("Cancel")
            DialogButtonBox.buttonRole: DialogButtonBox.DestructiveRole
            onClicked: inviteDialogRoot.close()
        }

    }

}
