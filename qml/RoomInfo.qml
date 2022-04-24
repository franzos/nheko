import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.5

import Rooms 1.0

Page {
    id: roomInfo
    width: parent.width
    title: "Room Info"
    required property string roomid
    property var info : Rooms.roomInformation(roomid)
    Column {
        anchors.fill: parent
        anchors.margins: 10
        RoundButton {
            id: avatarButton
            text: (info.avatar() ? "" : name[0])
            width: 86; height: 86
            anchors.horizontalCenter: parent.horizontalCenter
        }
        Row {
            Label { text: "Name : " }
            Label {
                id: nameLabel
                text: info.name()
            }
        }
        Row {
            Label { text: "ID : " }
            Label {
                id: idLabel
                text: info.id()
            }
        }
        Row {
            Label { text: "Members : " }
            Label {
                id: memberLabel
                text: info.memberCount()
            }
        }
        Row {
            Label { text: "Topic : " }
            Label {
                id: topicLabel
                text: info.topic()
            }
        }
        Row {
            Label { text: "Version : " }
            Label {
                id: versionLabel
                text: info.version()
            }
        }
        Row {
            Label { text: "Guest Access : " }
            Label {
                id: geustAccessLabel
                text: (info.version() ? "Enable" : "Disable")
            }
        }
    }
}
