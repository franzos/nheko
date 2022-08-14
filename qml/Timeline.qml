import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.5
import QtQml.Models 2.2

import MatrixClient 1.0
import TimelineModel 1.0
import CallManager 1.0
import Rooms 1.0
import CallType 1.0
import UserProfile 1.0
import "ui"

Room {
    id: timeline
    anchors.fill: parent
    property TimelineModel timelineModel: Rooms.timelineModel(roomid) 
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.top: parent.top
    header :RoomTopBar {
    }
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
        // anchors.margins: 10
        anchors.left: parent.left
        TypingIndicator {
            id: typingIndicator
        }
        
        ReplyPopup {
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

    EmojiPicker {
        id: emojiPopup

        colors: palette
        model: timelineModel.completerFor("allemoji", "")
    }

    function startVoiceCall(){
        CallManager.sendInvite(roomid,CallType.VOICE)
    }
    
    function startVideoCall(){
        CallManager.sendInvite(roomid,CallType.VIDEO)
    }

    function onOpenReadReceiptsDialog(rr) {
        var dialog = readReceiptsDialog.createObject(timeline, {
            "readReceipts": rr,
            "timelineModel": timelineModel
        });
        dialog.show();
        destroyOnClose(dialog);
    }

    function onShowRawMessageDialog(rawMessage) {
        var dialog = rawMessageDialog.createObject(timeline, {
            "rawMessage": rawMessage
        });

        dialog.x= (qmlLibRoot.width - dialog.width) / 2
        dialog.y= (qmlLibRoot.height - dialog.height) / 2
        dialog.show();
        destroyOnClose(dialog);
    }
    
    Component.onCompleted: {
        mainHeader.optionClicked.connect(onOptionClicked)
        mainHeader.voiceCallClicked.connect(startVoiceCall)
        mainHeader.videoCallClicked.connect(startVideoCall)
        timelineModel.onTypingUsersChanged.connect(onTypingUsersChanged)
        timelineModel.onOpenReadReceiptsDialog.connect(onOpenReadReceiptsDialog)
        timelineModel.onShowRawMessageDialog.connect(onShowRawMessageDialog)
        timelineModel.onOpenProfile.connect(onOpenProfile)
    }

    Component.onDestruction: {
        mainHeader.optionClicked.disconnect(onOptionClicked)
        mainHeader.voiceCallClicked.disconnect(startVoiceCall)
        mainHeader.videoCallClicked.disconnect(startVideoCall)
        timelineModel.onTypingUsersChanged.disconnect(onTypingUsersChanged)
        timelineModel.onOpenReadReceiptsDialog.disconnect(onOpenReadReceiptsDialog)
        timelineModel.onShowRawMessageDialog.disconnect(onShowRawMessageDialog)
         timelineModel.onOpenProfile.disconnect(onOpenProfile)
        timelineModel.destroy()
    }

    function onTypingUsersChanged(text) {
        typingIndicator.setTypingDisplayText(text)
    }

    function onOptionClicked(parent){
        contextMenu.popup(parent,0, 0)     
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
    
    function openUserInvitationDialog() {
        inviteuserDialog.open()
    }

    Menu {
        id: contextMenu
        margins: 10
        Action {
            id: inviteUserAction
            text: qsTr("Invite User")
            icon.source: "qrc:/images/add-square-button.svg"
            onTriggered: openUserInvitationDialog()
        }
        
        Action {
            id: leaveRoomAction
            text: qsTr("Leave Room")
            icon.source: "qrc:/images/leave-room-icon.svg"
            onTriggered: leaveDialog.open()
        }

        Action {
            id: membersAction
            text: qsTr("Members")
            icon.source: "qrc:/images/people.svg"
            onTriggered: timelineModel.openRoomMembers()
        }

        Action {
            id: settingAction
            text: qsTr("Setting")
            icon.source: "qrc:/images/settings.svg"
            onTriggered: roomSettingsDialog.open()
        }  
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

    Component {
        id: userProfileComponent

        UserProfileQm {
        }

    }

    function onOpenProfile(profile) {
        var userProfile = userProfileComponent.createObject(timeline, {
            "profile": profile,
            "room": timelineModel
        });
        userProfile.show();
        destroyOnClose(userProfile);
    }

    Connections{
        function onOpenRoomMembersDialog(members) {
            var membersDialog = roomMembersComponent.createObject(timeline, {
                "members": members,
                "room": timelineModel,
                "timeline" : timeline
            });
            membersDialog.show();
            destroyOnClose(membersDialog);
        }
         
        target: timelineModel
    }
}
