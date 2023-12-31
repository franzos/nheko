// SPDX-FileCopyrightText: 2021 Nheko Contributors
// SPDX-FileCopyrightText: 2022 Nheko Contributors
//
// SPDX-License-Identifier: GPL-3.0-or-later

import ".."
import "../../"
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import GlobalObject 1.0
import InviteesModel 1.0
import CursorShape 1.0
import MatrixClient 1.0

CustomApplicationWindow {
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
                onTextChanged:{
                    memberList.model.clear()
                    if(text){
                        var list = MatrixClient.knownUsers(text)
                        for(var i = 0; i < list.length; i++){
                            memberList.model.append({
                                userId     : list[i].userId,
                                displayName: list[i].displayName,
                                avatarUrl  : list[i].avatarUrl
                            })
                        }
                    }
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
            id: memberList
            width: parent.width
            height: 100
            model: ListModel {}
            visible: (model.count?true:false)
            ScrollBar.vertical: ScrollBar {}
            
            delegate: ItemDelegate {
                id: del
                onClicked: {
                    inviteeEntry.text = model.userId
                    memberList.model.clear()
                }
                background: Rectangle {
                    color: del.hovered ? GlobalObject.colors.dark : inviteDialogRoot.color
                }
                padding: 8 //Nheko.paddingMedium
                width: ListView.view.width
                height: memberLayout.implicitHeight + 4 * 2 //Nheko.paddingSmall
                hoverEnabled: true

                RowLayout {
                    id: memberLayout

                    spacing: 8 //Nheko.paddingMedium
                    anchors.centerIn: parent
                    width: parent.width - 4 * 2 //Nheko.paddingSmall

                    Avatar {
                        id: avatar

                        width: GlobalObject.avatarSize
                        height: GlobalObject.avatarSize
                        userid: model.userId
                        url: model.avatarUrl.replace("mxc://", "image://MxcImage/")
                        displayName: model.displayName
                        enabled: false
                    }

                    ColumnLayout {
                        spacing: 4//Nheko.paddingSmall

                        ElidedLabel {
                            fullText: model.displayName
                            font.pixelSize: fontMetrics.font.pixelSize
                            elideWidth: del.width - 8 * 2 -avatar.width  //Nheko.paddingMedium
                        }

                        ElidedLabel {
                            fullText: model.userId
                            color: del.hovered ? GlobalObject.colors.brightText : GlobalObject.colors.buttonText
                            font.pixelSize: Math.ceil(fontMetrics.font.pixelSize * 0.9)
                            elideWidth: del.width - 8 * 2 - avatar.width //Nheko.paddingMedium
                        }

                    }
                }

                CursorShape {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                }

            }
        } 

        ListView {
            id: inviteesList

            Layout.fillWidth: true
            Layout.fillHeight: true
            model: invitees
            visible: (memberList.model.length?false:true)
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
        background: Rectangle {
            anchors.fill: parent
            color: GlobalObject.colors.window
        }
    }

}
