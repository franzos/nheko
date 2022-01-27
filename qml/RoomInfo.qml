import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.5

import Rooms 1.0

CustomPage {
    id: roomInfo
    width: parent.width

    required property string roomid
    Column {
        anchors.fill: parent
        anchors.margins: 10
        RoundButton {
            id: avatarButton
            text: "."
            width: 86; height: 86
            anchors.horizontalCenter: parent.horizontalCenter
        }
        Row {
            Label { text: "Name : " }
            Label {
                id: nameLabel
                text: "..."
            }
        }
        Row {
            Label { text: "ID : " }
            Label {
                id: idLabel
                text: "..."
            }
        }
        Row {
            Label { text: "Members : " }
            Label {
                id: memberLabel
                text: "..."
            }
        }
        Row {
            Label { text: "Topic : " }
            Label {
                id: topicLabel
                text: "..."
            }
        }
        Row {
            Label { text: "Version : " }
            Label {
                id: versionLabel
                text: "..."
            }
        }
        Row {
            Label { text: "Guest Access : " }
            Label {
                id: geustAccessLabel
                text: "..."
            }
        }
    }
    Component.onCompleted: {
        var info = Rooms.roomInformation(roomid)
        header.setTitle("Room Info")
        nameLabel.text = info.name()
        idLabel.text = info.id()
        memberLabel.text = info.memberCount()
        topicLabel.text = info.topic()
        versionLabel.text = info.version()
        geustAccessLabel.text = (info.version() ? "Enable" : "Disable")
        avatarButton.text = (info.avatar() ? "" : name[0])
    }
}
