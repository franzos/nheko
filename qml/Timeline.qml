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
            ToolButton {
                id: sendButton
                icon.source: "qrc:/images/send.svg"
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
        header.setCallButtonsVisible(true)
        header.setOptionButtonsVisible(true)
        header.optionClicked.connect(onOptionClicked)
    }

    Connections {
        target: timelineModel
        function onTypingUsersChanged(text) {
            typingIndicator.setTypingDisplayText(text)
        }
    }

    function onOptionClicked(){
        contextMenu.popup()     
    }

    LeaveMessage {
        id: leaveDialog
        x: (qmlLibRoot.width - width) / 2
        y: (qmlLibRoot.height - height) / 2
    }

    InviteUserDialog {
        id: inviteuserDialog
        x: (qmlLibRoot.width - width) / 2
        y: (qmlLibRoot.height - height) / 2
    }

    Menu {
        id: contextMenu
        margins: 10
        Action {
            id: inviteUserAction
            text: qsTr("&Invite User")
            icon.source: "qrc:/images/add-square-button.svg"
            shortcut: StandardKey.Copy
            onTriggered: inviteuserDialog.open()
        }
        
        Action {
            id: leaveRoomAction
            text: qsTr("&Leave Room")
            icon.source: "qrc:/images/ban.svg"
            shortcut: StandardKey.Copy
            onTriggered: leaveDialog.open()
        }

        Action {
            id: membersAction
            text: qsTr("&Members")
            icon.source: "qrc:/images/people.svg"
            shortcut: StandardKey.Copy
            // onTriggered: 
        }

        Action {
            id: settingAction
            text: qsTr("&Setting")
            icon.source: "qrc:/images/settings.svg"
            shortcut: StandardKey.Copy
            // onTriggered:
        }
              
    }
}
