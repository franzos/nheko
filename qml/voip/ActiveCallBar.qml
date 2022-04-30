import "../"
import "../ui"
import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.2
import GlobalObject 1.0
import CallManager 1.0
import CallType 1.0
import WebRTCState 1.0

Rectangle {
    visible: CallManager.isOnCall
    color: callInviteBar.color
    implicitHeight: visible ? rowLayout.height + 8 : 0

    MouseArea {
        anchors.fill: parent
        onClicked: {
            if (CallManager.callType != CallType.VOICE)
                stackLayout.currentIndex = stackLayout.currentIndex ? 0 : 1;
        }
    }

    RowLayout {
        id: rowLayout

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: 8

        Avatar {
            width: GlobalObject.avatarSize
            height: GlobalObject.avatarSize
            url: CallManager.callPartyAvatarUrl.replace("mxc://", "image://MxcImage/")
            userid: CallManager.callParty
            displayName: CallManager.callPartyDisplayName
        }

        Label {
            Layout.leftMargin: 8
            font.pointSize: fontMetrics.font.pointSize * 1.1
            text: CallManager.callPartyDisplayName
            color: "#000000"
        }

        Image {
            id: callTypeIcon

            Layout.leftMargin: 4
            Layout.preferredWidth: 24
            Layout.preferredHeight: 24
        }

        Item {
            states: [
                State {
                    name: "VOICE"
                    when: CallManager.callType == CallType.VOICE

                    PropertyChanges {
                        target: callTypeIcon
                        source: "qrc:/images/place-call.svg"
                    }

                },
                State {
                    name: "VIDEO"
                    when: CallManager.callType == CallType.VIDEO

                    PropertyChanges {
                        target: callTypeIcon
                        source: "qrc:/images/video.svg"
                    }

                },
                State {
                    name: "SCREEN"
                    when: CallManager.callType == CallType.SCREEN

                    PropertyChanges {
                        target: callTypeIcon
                        source: "qrc:/images/screen-share.svg"
                    }

                }
            ]
        }

        Label {
            id: callStateLabel

            font.pointSize: fontMetrics.font.pointSize * 1.1
            color: "#000000"
        }

        Item {
            states: [
                State {
                    name: "OFFERSENT"
                    when: CallManager.callState == WebRTCState.OFFERSENT

                    PropertyChanges {
                        target: callStateLabel
                        text: qsTr("Calling...")
                    }

                },
                State {
                    name: "CONNECTING"
                    when: CallManager.callState == WebRTCState.CONNECTING

                    PropertyChanges {
                        target: callStateLabel
                        text: qsTr("Connecting...")
                    }

                },
                State {
                    name: "ANSWERSENT"
                    when: CallManager.callState == WebRTCState.ANSWERSENT

                    PropertyChanges {
                        target: callStateLabel
                        text: qsTr("Connecting...")
                    }

                },
                State {
                    name: "CONNECTED"
                    when: CallManager.callState == WebRTCState.CONNECTED

                    PropertyChanges {
                        target: callStateLabel
                        text: "00:00"
                    }

                    PropertyChanges {
                        target: callTimer
                        startTime: Math.floor((new Date()).getTime() / 1000)
                    }

                    PropertyChanges {
                        target: stackLayout
                        currentIndex: CallManager.callType != CallType.VOICE ? 1 : 0
                    }

                },
                State {
                    name: "DISCONNECTED"
                    when: CallManager.callState == WebRTCState.DISCONNECTED

                    PropertyChanges {
                        target: callStateLabel
                        text: ""
                    }

                    // PropertyChanges {
                    //     target: stackLayout
                    //     currentIndex: 0
                    // }

                }
            ]
        }

        Timer {
            id: callTimer

            property int startTime

            function pad(n) {
                return (n < 10) ? ("0" + n) : n;
            }

            interval: 1000
            running: CallManager.callState == WebRTCState.CONNECTED
            repeat: true
            onTriggered: {
                var d = new Date();
                let seconds = Math.floor(d.getTime() / 1000 - startTime);
                let s = Math.floor(seconds % 60);
                let m = Math.floor(seconds / 60) % 60;
                let h = Math.floor(seconds / 3600);
                callStateLabel.text = (h ? (pad(h) + ":") : "") + pad(m) + ":" + pad(s);
            }
        }

        Label {
            Layout.leftMargin: 16
            visible: CallManager.callType == CallType.SCREEN && CallManager.callState == WebRTCState.CONNECTED
            text: qsTr("You are screen sharing")
            font.pointSize: fontMetrics.font.pointSize * 1.1
            color: "#000000"
        }

        Item {
            Layout.fillWidth: true
        }

        ImageButton {
            visible: CallManager.haveLocalPiP
            width: 24
            height: 24
            buttonTextColor: "#000000"
            image: ":/images/picture-in-picture.svg"
            hoverEnabled: true
            ToolTip.visible: hovered
            ToolTip.text: qsTr("Hide/Show Picture-in-Picture")
            onClicked: CallManager.toggleLocalPiP()
        }

        ImageButton {
            Layout.leftMargin: 8
            Layout.rightMargin: 16
            width: 24
            height: 24
            buttonTextColor: "#000000"
            image: CallManager.isMicMuted ? ":/images/microphone-unmute.svg" : ":/images/microphone-mute.svg"
            hoverEnabled: true
            ToolTip.visible: hovered
            ToolTip.text: CallManager.isMicMuted ? qsTr("Unmute Mic") : qsTr("Mute Mic")
            onClicked: CallManager.toggleMicMute()
        }
    }

}
