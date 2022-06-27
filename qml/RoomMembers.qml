// SPDX-FileCopyrightText: 2021 Nheko Contributors
// SPDX-FileCopyrightText: 2022 Nheko Contributors
//
// SPDX-License-Identifier: GPL-3.0-or-later

import "ui"
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.13
import MemberList 1.0


import MatrixClient 1.0
import GlobalObject 1.0
import TimelineModel 1.0
import CursorShape 1.0

ApplicationWindow {
    id: roomMembersRoot

    property MemberList members
    property TimelineModel room

    title: qsTr("Members of %1").arg(members.roomName)
    height: 650
    width: 420
    minimumHeight: 420
    palette: GlobalObject.colors
    color: GlobalObject.colors.window
    flags: Qt.Dialog | Qt.WindowCloseButtonHint | Qt.WindowTitleHint

    Shortcut {
        sequence: StandardKey.Cancel
        onActivated: roomMembersRoot.close()
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8 //Nheko.paddingMedium
        spacing: 8 //Nheko.paddingMedium

        Avatar {
            id: roomAvatar

            width: 130
            height: width
            roomid: members.roomId
            displayName: members.roomName
            Layout.alignment: Qt.AlignHCenter
            url: members.avatarUrl.replace("mxc://", "image://MxcImage/")
            onClicked: TimelineManager.openRoomSettings(members.roomId)
        }

        ElidedLabel {
            font.pixelSize: fontMetrics.font.pixelSize * 2
            fullText: qsTr("%n people in %1", "Summary above list of members", members.memberCount).arg(members.roomName)
            Layout.alignment: Qt.AlignHCenter
            elideWidth: parent.width - 8 //Nheko.paddingMedium
        }

        ImageButton {
            Layout.alignment: Qt.AlignHCenter
            image: ":/images/add-square-button.svg"
            hoverEnabled: true
            ToolTip.visible: hovered
            ToolTip.text: qsTr("Invite more people")
            onClicked: TimelineManager.openInviteUsers(members.roomId)
        }

        ScrollView {
            palette: GlobalObject.colors
            padding: 8 // Nheko.paddingMedium
            ScrollBar.horizontal.visible: false
            Layout.fillHeight: true
            Layout.minimumHeight: 200
            Layout.fillWidth: true

            ListView {
                id: memberList

                clip: true
                boundsBehavior: Flickable.StopAtBounds
                model: members

                ScrollHelper {
                    flickable: parent
                    anchors.fill: parent
                    enabled: true //!Settings.mobileMode
                }

                delegate: ItemDelegate {
                    id: del

                    onClicked: room.openUserProfile(model.mxid)
                    padding: 8 //Nheko.paddingMedium
                    width: ListView.view.width
                    height: memberLayout.implicitHeight + 4 * 2 //Nheko.paddingSmall
                    hoverEnabled: true
                    background: Rectangle {
                        color: del.hovered ? GlobalObject.colors.dark : roomMembersRoot.color
                    }

                    RowLayout {
                        id: memberLayout

                        spacing: 8 //Nheko.paddingMedium
                        anchors.centerIn: parent
                        width: parent.width - 4 * 2 //Nheko.paddingSmall

                        Avatar {
                            id: avatar

                            width: GlobalObject.avatarSize
                            height: GlobalObject.avatarSize
                            userid: model.mxid
                            url: model.avatarUrl.replace("mxc://", "image://MxcImage/")
                            displayName: model.displayName
                            enabled: false
                        }

                        ColumnLayout {
                            spacing: 4//Nheko.paddingSmall

                            ElidedLabel {
                                fullText: model.displayName
                                color: room.userColor(model ? model.mxid : "", del.background.color)
                                font.pixelSize: fontMetrics.font.pixelSize
                                elideWidth: del.width - 8 * 2 -avatar.width - encryptInd.width //Nheko.paddingMedium
                            }

                            ElidedLabel {
                                fullText: model.mxid
                                color: del.hovered ? GlobalObject.colors.brightText : GlobalObject.colors.buttonText
                                font.pixelSize: Math.ceil(fontMetrics.font.pixelSize * 0.9)
                                elideWidth: del.width - 8 * 2 - avatar.width - encryptInd.width //Nheko.paddingMedium
                            }

                        }

                        Item {
                            Layout.fillWidth: true
                        }

                        EncryptionIndicator {
                            id: encryptInd

                            Layout.alignment: Qt.AlignRight
                            visible: room.isEncrypted
                            encrypted: room.isEncrypted
                            trust: encrypted ? model.trustlevel : Crypto.Unverified
                            ToolTip.text: {
                                if (!encrypted)
                                    return qsTr("This room is not encrypted!");

                                switch (trust) {
                                case Crypto.Verified:
                                    return qsTr("This user is verified.");
                                case Crypto.TOFU:
                                    return qsTr("This user isn't verified, but is still using the same master key from the first time you met.");
                                default:
                                    return qsTr("This user has unverified devices!");
                                }
                            }
                        }

                    }

                    CursorShape {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                    }

                }

                footer: Item {
                    width: parent.width
                    visible: (members.numUsersLoaded < members.memberCount) && members.loadingMoreMembers
                    // use the default height if it's visible, otherwise no height at all
                    height: membersLoadingSpinner.height
                    anchors.margins: 8 //Nheko.paddingMedium

                    Spinner {
                        id: membersLoadingSpinner

                        anchors.centerIn: parent
                        height: visible ? 35 : 0
                    }

                }

            }

        }

    }

    footer: DialogButtonBox {
        standardButtons: DialogButtonBox.Ok
        onAccepted: roomMembersRoot.close()
    }

}