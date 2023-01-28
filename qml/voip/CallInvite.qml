// SPDX-FileCopyrightText: 2021 Nheko Contributors
// SPDX-FileCopyrightText: 2022 Nheko Contributors
//
// SPDX-License-Identifier: GPL-3.0-or-later

import "../"
import "../ui"
import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.2
import GlobalObject 1.0
import CallManager 1.0
import CallType 1.0
import Settings 1.0

Popup {
    id: callInv

    closePolicy: Popup.NoAutoClose
    width: parent.width
    height: parent.height
    palette: GlobalObject.colors

    Component {
        id: deviceError

        DeviceError {
        }

    }

    Connections {
        function onNewInviteState() {
            if (!CallManager.haveCallInvite)
                close();

        }

        target: CallManager
    }

    ColumnLayout {
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        Label {
            Layout.alignment: Qt.AlignCenter
            Layout.topMargin: callInv.parent.height / 25
            Layout.fillWidth: true
            text: CallManager.callPartyDisplayName
            font.pointSize: fontMetrics.font.pointSize * 2
            color: GlobalObject.colors.windowText
            horizontalAlignment: Text.AlignHCenter
        }

        Avatar {
            Layout.alignment: Qt.AlignCenter
            Layout.preferredHeight: callInv.height / 5
            Layout.preferredWidth: callInv.height / 5
            url: CallManager.callPartyAvatarUrl.replace("mxc://", "image://MxcImage/")
            userid: CallManager.callParty
            displayName: CallManager.callPartyDisplayName
        }

        ColumnLayout {
            Layout.alignment: Qt.AlignCenter
            Layout.bottomMargin: callInv.height / 25

            Image {
                property string image: CallManager.callType == CallType.VIDEO ? ":/images/video.svg" : ":/images/place-call.svg"

                Layout.alignment: Qt.AlignCenter
                Layout.preferredWidth: callInv.height / 10
                Layout.preferredHeight: callInv.height / 10
                source: "image://colorimage/" + image + "?" + GlobalObject.colors.windowText
                fillMode: Image.PreserveAspectFit
            }

            Label {
                Layout.alignment: Qt.AlignCenter
                text: CallManager.callType == CallType.VIDEO ? qsTr("Video Call") : qsTr("Voice Call")
                font.pointSize: fontMetrics.font.pointSize * 2
                color: GlobalObject.colors.windowText
            }

        }

        ColumnLayout {
            id: deviceCombos

            property int imageSize: callInv.height / 20

            Layout.alignment: Qt.AlignCenter
            Layout.bottomMargin: callInv.height / 25

            RowLayout {
                Layout.alignment: Qt.AlignCenter

                Image {
                    Layout.preferredWidth: deviceCombos.imageSize
                    Layout.preferredHeight: deviceCombos.imageSize
                    source: "image://colorimage/:/images/microphone-unmute.svg?" + GlobalObject.colors.windowText
                    fillMode: Image.PreserveAspectFit
                }

                ComboBox {
                    id: micCombo

                    Layout.fillWidth: true
                    model: CallManager.mics
                }

            }

            RowLayout {
                visible: CallManager.callType == CallType.VIDEO && CallManager.cameras.length > 0
                Layout.alignment: Qt.AlignCenter

                Image {
                    Layout.preferredWidth: deviceCombos.imageSize
                    Layout.preferredHeight: deviceCombos.imageSize
                    source: "image://colorimage/:/images/video.svg?" + GlobalObject.colors.windowText
                    fillMode: Image.PreserveAspectFit
                }

                ComboBox {
                    id: cameraCombo

                    Layout.fillWidth: true
                    model: CallManager.cameras
                }

            }

        }

        RowLayout {
            id: buttonLayout

            property int buttonSize: callInv.height / 16

            function validateMic() {
                if (CallManager.mics.length == 0) {
                    var dialog = deviceError.createObject(callInv, {
                        "errorString": qsTr("No microphone found."),
                        "image": ":/images/place-call.svg"
                    });
                    dialog.open();
                    destroyOnClose(dialog);
                    return false;
                }
                return true;
            }

            Layout.alignment: Qt.AlignCenter
            spacing: callInv.height / 6

            RoundButton {
                implicitWidth: buttonLayout.buttonSize
                implicitHeight: buttonLayout.buttonSize
                onClicked: {
                    CallManager.hangUp();
                    close();
                }

                background: Rectangle {
                    anchors.fill: parent
                    color: "transparent"
                    border.color: "#ff0000"
                    border.width: 5
                    radius: buttonLayout.buttonSize / 2
                }

                Image {
                    width: buttonLayout.buttonSize - 30
                    height: width
                    anchors.centerIn: parent
                    source: "image://colorimage/:/images/end-call.svg?#ff0000"
                    fillMode: Image.PreserveAspectFit
                }


            }

            RoundButton {
                id: acceptButton

                property string image: CallManager.callType == CallType.VIDEO ? ":/images/video.svg" : ":/images/place-call.svg"

                implicitWidth: buttonLayout.buttonSize
                implicitHeight: buttonLayout.buttonSize
                onClicked: acceptCall()

                background: Rectangle {
                    anchors.fill: parent
                    color: "transparent"
                    border.color: "#00ff00"
                    border.width: 5
                    radius: buttonLayout.buttonSize / 2
                }

                Image {
                    width: buttonLayout.buttonSize - 30
                    height: width
                    anchors.centerIn: parent
                    source: "image://colorimage/" + acceptButton.image + "?#00ff00"
                    fillMode: Image.PreserveAspectFit
                }

            }

        }

    }

    background: Rectangle {
        color: GlobalObject.colors.window
        border.color: GlobalObject.colors.windowText
    }

    function acceptCall(){
        if (buttonLayout.validateMic()) {
            Settings.microphone = micCombo.currentText;
            if (cameraCombo.visible)
                Settings.camera = cameraCombo.currentText;

            CallManager.acceptInvite();
            console.log("Call invite Accepted!");
            close();
        }
    }

}
