import QtQuick 2.9
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import org.freedesktop.gstreamer.GLVideoItem 1.0
import WebRTCState 1.0
import CallManager 1.0
import GlobalObject 1.0

Page {
    property string callpartyName: qsTr("")
    anchors.fill: parent
    GstGLVideoItem {
        id: gstglvideoitem
        anchors.fill: parent
        objectName: "gstGlItem"
        visible: false
    }

    Rectangle {
        id: freecallItem
        anchors.centerIn: parent
        anchors.fill: parent
        visible: false
        color: GlobalObject.colors.base
        Image {
            anchors.centerIn: parent
            source: "image://colorimage/:/images/video-inactive.svg?" + GlobalObject.colors.windowText
            width: parent.width * 1 / 8
            height: parent.width * 1 / 8
            sourceSize.width: width
            sourceSize.height: width
        }
    }

    Rectangle {
        id: transientItem
        anchors.fill: parent
        visible: false
        color: GlobalObject.colors.alternateBase
        ColumnLayout{
            anchors.fill: parent
            BusyIndicator {
                id: busyIndicator
                palette.dark: GlobalObject.colors.windowText
                Layout.alignment: Qt.AlignCenter
            }
            Label {
                id: transientText
                width: parent.width
                text: "transient ..."
                font.pointSize: 12
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            }
        }
    }

    function enableTransiationState(state){
        freecallItem.visible = false
        transientItem.visible = true
        gstglvideoitem.visible = false
        transientText.text = state + " " + callpartyName + "..."
    }

    state: WebRTCState.DISCONNECTED
    states: [
        State {
            name: WebRTCState.DISCONNECTED
            StateChangeScript {
                script: {
                    freecallItem.visible = true
                    transientItem.visible = false
                    gstglvideoitem.visible = false
                }
            }
        },
        State {
            name: WebRTCState.CONNECTED
            StateChangeScript {
                script: {
                    freecallItem.visible = false
                    transientItem.visible = false
                    gstglvideoitem.visible = true
                }
            }
        },
        State {
            name: WebRTCState.ANSWERSENT
            StateChangeScript {
                script: {
                    enableTransiationState("Connecting")
                }
            }
        },
        State {
            name: WebRTCState.CONNECTING
            StateChangeScript {
                script: {
                    freecallItem.visible = false
                    transientItem.visible = true
                    gstglvideoitem.visible = false
                    enableTransiationState("Connecting")
                }
            }
        },
        State {
            name: WebRTCState.OFFERSENT
            StateChangeScript {
                script: {
                    freecallItem.visible = false
                    transientItem.visible = true
                    gstglvideoitem.visible = false
                    enableTransiationState("Calling")
                }
            }
        },
        State {
            name: WebRTCState.INITIATING
            StateChangeScript {
                script: {
                    freecallItem.visible = false
                    transientItem.visible = true
                    gstglvideoitem.visible = false
                    enableTransiationState("Connecting")
                }
            }
        }
    ]

    function onCallStateChanged(){
        callpartyName = CallManager.callPartyDisplayName
        state = CallManager.callState
    }

    Component.onCompleted: {
        CallManager.onNewCallState.connect(onCallStateChanged)
    }
}