import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.5
import QtQml.Models 2.2

import MatrixClient 1.0
import TimelineModel 1.0
import CallManager 1.0
import Rooms 1.0
import CallType 1.0
import "ui"

Room {
    id: timeline
    anchors.fill: parent
    property TimelineModel timelineModel: Rooms.timelineModel(roomid) 
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.top: parent.top
    MessageView{
        anchors.fill: parent
    }
    // ListView {
    //     id: timelineView
    //     anchors.fill: parent
    //     spacing: 10
    //     anchors.leftMargin: 10
    //     anchors.rightMargin: 10
    //     anchors.topMargin: 10
    //     anchors.bottomMargin: 20

    //     flickableDirection: Flickable.VerticalFlick
    //     boundsBehavior: Flickable.StopAtBounds
    //     ScrollBar.vertical: ScrollBar {}
    //     model: timelineModel

    //     delegate:TimelineDelegate{
    //         id: eventItems
    //     }

    //     onCountChanged: {
    //         var newIndex = count - 1
    //         positionViewAtEnd()
    //         currentIndex = newIndex
    //     }
    // }

    footer: ColumnLayout {
        id: footer
        width: parent.width
        anchors.margins: 10
        anchors.left: parent.left
        anchors.leftMargin: 10
        TypingIndicator {
            id: typingIndicator
        }
        
        MessageInput {
            id: messageInput
            width: parent.width //- sendButton.width            
            // placeholderText: qsTr("Enter your message ...")
            // Keys.onReturnPressed: sendButton.sendMessage() // Enter key
            // Keys.onEnterPressed: sendButton.sendMessage() // Numpad enter key
            // onTextChanged: {
            //     sendButton.enabled = messageInput.text.length > 0 ? true : false
            // }
        }
        
        // Row {
        //     width: parent.width - 20
            
        //     MessageInput {
        //         id: messageInput
        //         width: parent.width //- sendButton.width            
        //         // placeholderText: qsTr("Enter your message ...")
        //         // Keys.onReturnPressed: sendButton.sendMessage() // Enter key
        //         // Keys.onEnterPressed: sendButton.sendMessage() // Numpad enter key
        //         // onTextChanged: {
        //         //     sendButton.enabled = messageInput.text.length > 0 ? true : false
        //         // }
        //     }
        //     ToolButton {
        //     //     id: sendButton
        //     //     icon.source: "qrc:/images/send.svg"
        //     //     enabled: true// messageInput.text.length > 0 ? true : false
        //     //     function sendMessage(){
        //     //         if(messageInput.text.length > 0) {
        //     //             timelineModel.send(messageInput.text);
        //     //             messageInput.text = ""
        //     //         }
        //     //         messageInput.forceActiveFocus()
        //     //     }
        //     //     onClicked: sendMessage()
        //     }
        // }
    }

    function startVoiceCall(){
        CallManager.sendInvite(roomid,CallType.VOICE)
    }
    
    function startVideoCall(){
        CallManager.sendInvite(roomid,CallType.VIDEO)
    }

    function onInputTextChanged(text){
        messageInput.text = text
    }

    Component.onCompleted: {
        mainHeader.optionClicked.connect(onOptionClicked)
        mainHeader.voiceCallClicked.connect(startVoiceCall)
        mainHeader.videoCallClicked.connect(startVideoCall)
        timelineModel.onTypingUsersChanged.connect(onTypingUsersChanged)
        timelineModel.onInputTextChanged.connect(onInputTextChanged)
    }

    Component.onDestruction: {
        mainHeader.optionClicked.disconnect(onOptionClicked)
        mainHeader.voiceCallClicked.disconnect(startVoiceCall)
        mainHeader.videoCallClicked.disconnect(startVideoCall)
        timelineModel.onTypingUsersChanged.disconnect(onTypingUsersChanged)
        timelineModel.onInputTextChanged.disconnect(onInputTextChanged)
        timelineModel.destroy()
    }

    function onTypingUsersChanged(text) {
        typingIndicator.setTypingDisplayText(text)
    }

    function onOptionClicked(){
        contextMenu.popup()     
    }

    LeaveMessage {
        id: leaveDialog
        roomId: roomid
        roomName: name
        x: (qmlLibRoot.width - width) / 2
        y: (qmlLibRoot.height - height) / 2
        onAccepted: goToPrevPage()
    }

    AddUserDialog {
        id: inviteuserDialog
        title: "Invite user"
        x: (qmlLibRoot.width - width) / 2
        y: (qmlLibRoot.height - height) / 2
        onUserAdded:{
            MatrixClient.inviteUser(roomid,userid,"Send invitation")
        }
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
            icon.source: "qrc:/images/leave-room-icon.svg"
            shortcut: StandardKey.Copy
            onTriggered: leaveDialog.open()
        }

        Action {
            id: membersAction
            text: qsTr("&Members")
            icon.source: "qrc:/images/people.svg"
            shortcut: StandardKey.Copy
            onTriggered: membersDialog.open()
        }

        Action {
            id: settingAction
            text: qsTr("&Setting")
            icon.source: "qrc:/images/settings.svg"
            shortcut: StandardKey.Copy
            onTriggered: roomSettingsDialog.open()
        }  
    }

    Dialog {
        id: membersDialog
        x: (qmlLibRoot.width - width) / 2
        y: (qmlLibRoot.height - height) / 2
        title: "Members"
        standardButtons: Dialog.Ok
        Label {            
            text: "Coming Soon"
        }
        onAccepted: { }
    }

    Dialog {
        id: roomSettingsDialog
        x: (qmlLibRoot.width - width) / 2
        y: (qmlLibRoot.height - height) / 2
        title: "Room Settings"
        standardButtons: Dialog.Ok
        Label {            
            text: "Coming Soon"
        }
        onAccepted: { }
    }
}
