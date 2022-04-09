import QtQuick 2.9
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import org.freedesktop.gstreamer.GLVideoItem 1.0
// import CallManager 1.0

Item {
    objectName: "videoCallEmbedItem"
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
        width: parent.width * 3 / 4
        height: parent.height * 3 / 4
        visible: false
        color: "gray"
        Image {
            anchors.centerIn: parent
            sourceSize.width: parent.width * 1 / 8
            sourceSize.height: sourceSize.width
            source: "qrc:/images/video-inactive.svg"
        }
    }

    Rectangle {
        id: transientItem
        anchors.fill: parent
        visible: false
        ColumnLayout{
            anchors.fill: parent
            BusyIndicator {
                id: busyIndicator
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

    function setTransientText(text){
        transientText.text = text
    }

    function changeState(s){
        state = s
    }

    state: "freecall"
    states: [
        State {
            name: "freecall"
            StateChangeScript {
                script: {
                    freecallItem.visible = true
                    transientItem.visible = false
                    gstglvideoitem.visible = false
                }
            }
        },
        State {
            name: "oncall"
            StateChangeScript {
                script: {
                    freecallItem.visible = false
                    transientItem.visible = false
                    gstglvideoitem.visible = true
                }
            }
        },
        State {
            name: "transient"
            StateChangeScript {
                script: {
                    freecallItem.visible = false
                    transientItem.visible = true
                    gstglvideoitem.visible = false
                    setTransientText("...")
                }
            }
        }
    ]
}