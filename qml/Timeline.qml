import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.5
import QtQml.Models 2.2

import MatrixClient 1.0
import TimelineModel 1.0
import Rooms 1.0

Page {
    id: timeline
    anchors.fill: parent
    property string roomid
    property string name
    property string avatar
    property TimelineModel timelineModel

    ListView {
        id: timelineView
        anchors.fill: parent
        spacing: 10
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        anchors.topMargin: 10
        anchors.bottomMargin: 20

        flickableDirection: Flickable.VerticalFlick
        boundsBehavior: Flickable.StopAtBounds
        ScrollBar.vertical: ScrollBar {}
        model: timelineModel

        delegate:TimelineDelegate{
            id: eventItems
        }

//        delegate: Rectangle {
//            height: 25
//            width: 100
//            Text { text: body }
//        }

        onCountChanged: {
            var newIndex = count - 1
            positionViewAtEnd()
            currentIndex = newIndex
        }

        Component.onCompleted: timeline.load(roomid, name, avatar)
    }

    footer: Column {
        id: footer
        width: parent.width
        anchors.margins: 10
        TypingIndicator {
            id: typingIndicator
        }
        Row {
            width: parent.width
            TextField {
                id: messageInput
                width: parent.width - sendButton.width
                placeholderText: qsTr("Enter your message" + footer.height)
            }
            Button {
                id: sendButton
                text: "Send"
                enabled: messageInput.text ? true : false
                onClicked: {
                    timelineModel.send(messageInput.text);
                    messageInput.text = ""
                }
            }
        }
    }

    function load(rid, rname, ravatar){
        roomid = rid
        name = rname
        avatar = ravatar
        if(roomid)
            timelineModel = Rooms.timelineModel(roomid)
    }

    Connections {
        target: timelineModel
        function onTypingUsersChanged(text) {
            typingIndicator.setTypingDisplayText(text)
        }
    }
}
