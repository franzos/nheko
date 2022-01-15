import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

import Rooms 1.0

Rectangle {
    id: room
    width: roomListView.width
    height: childrenRect.height
    required property string index
    required property string id
    required property string name
    required property string avatar
    required property bool invite
    required property string lastmessage
    required property int unreadcount
//    color: index % 2 == 0 ? "lightsteelblue" : "transparent"
    signal timelineClicked(Timeline timeline)
    RowLayout {
        width: parent.width
        RoundButton {
            id: avatar_button
            text: name[0]
            width: 24; height: 24
            anchors.margins: 10
        }
        Rectangle{
            anchors.left: avatar_button.right
            anchors.margins: 10
            ColumnLayout{
                Label {
                    text: name
                    font.italic: invite ? true : false
                    font.pointSize: 12
                }
                Label {
                    text: lastmessage
                    visible: invite ? false : true
                    font.pointSize: 9
                    color: "gray"
                }
            }
        }
        Rectangle {
            id: rect
            width: 20
            height: 20
            radius: width/2
            color: "red"
            visible: (!invite && unreadcount) ? true : false
            Layout.alignment: Qt.AlignRight
            Label {
                anchors.centerIn: parent
                text: unreadcount
                color: "white"
                font.pointSize: 9
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }
    MouseArea {
        anchors.fill: parent
        Timeline {
            id: timeline
            visible: false
        }
        onClicked: {
            timeline.load(room.id, room.name, room.avatar)
            timelineClicked(timeline)
        }
    }
}
