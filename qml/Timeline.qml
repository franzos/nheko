import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.5
import QtQml.Models 2.2

import MatrixClient 1.0
import TimelineModel 1.0
import Rooms 1.0

Room {
    id: timeline
    anchors.fill: parent
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

        onCountChanged: {
            var newIndex = count - 1
            positionViewAtEnd()
            currentIndex = newIndex
        }
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
                placeholderText: qsTr("Enter your message ...")
                Keys.onReturnPressed: sendButton.sendMessage() // Enter key
                Keys.onEnterPressed: sendButton.sendMessage() // Numpad enter key
            }
            Button {
                id: sendButton
                text: "Send"
                enabled: messageInput.text ? true : false
                function sendMessage(){
                    timelineModel.send(messageInput.text);
                    messageInput.text = ""
                }
                onClicked: sendMessage()
            }
        }
    }

    Component.onCompleted: {
        timelineModel = Rooms.timelineModel(roomid)
    }

    Connections {
        target: timelineModel
        function onTypingUsersChanged(text) {
            typingIndicator.setTypingDisplayText(text)
        }
    }
}
