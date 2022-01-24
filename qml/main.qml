import QtQuick 2.9
import QtQuick.Window 2.0
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

import MatrixClient 1.0

ApplicationWindow {
    id: qmlApplication
    title: qsTr("Matrix Client")
    width: 400
    height: 600
    visible: true
    Material.theme: Material.Dark
    
    property string displayName;

    StackView {
        id: stack
        anchors.fill: parent
    }

    header: Rectangle {
        width: parent.width
        height: 30
        Row {
            anchors.fill: parent
            Button {
                id: backButton
                text: "<"
                width: 24
                height: parent.height
                enabled: !stack.empty
                onClicked: popStack()
            }

            Button {
                id: titleLabel
                width: parent.width - backButton.width - logoutButton.width - 2
                height: parent.height
                anchors.leftMargin: 2
                // anchors.centerIn: parent
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: backButton.right
            }

            Button {
                id: logoutButton
                text: "Logout"
                anchors.leftMargin: 2
                height: parent.height
                anchors.left: titleLabel.right
                onClicked: logoutDialog.open()
            }
            Dialog {
                id: logoutDialog
                x: (qmlApplication.width - width) / 2
                y: (qmlApplication.height - height) / 2
                title: "Logout"
                standardButtons: Dialog.Cancel | Dialog.Ok
                Label {
                    text: "Are you sure you want to logout ?"
                }
                onAccepted: {
                    pushToStack(loginIndicator,"...")
                    MatrixClient.logout()
                }
                onRejected: {}
            }
        }
    }

    function setTitle(title){
        titleLabel.text = title
        backButton.enabled= !stack.empty
    }

    BusyIndicator {
        id: loginIndicator
        width: 64; height: 64
        anchors.centerIn: parent
    }

    Component {
        id: timelineFactory
        Timeline {}
    }

    Component {
        id: invitationFactory
        InvitationRoom {}
    }

    function replaceInStack(newPage){
        stack.replace(stack.currentItem,newPage)
        if (stack.currentItem == roomList){
            setTitle(displayName)
            header.visible= true
        } else if (stack.currentItem == loginPage || stack.currentItem == loginIndicator){
            header.visible= false
        }
    }

    function pushToStack(item, title){
        console.log(item, title)
        stack.push(item)
        if(stack.currentItem == loginPage || stack.currentItem == loginIndicator)
            header.visible= false
        else
            setTitle(title)
    }

    function popStack(){
        stack.pop()
        if (stack.currentItem == roomList){
            setTitle(displayName)
            header.visible= true
        }
    }

    function onRoomInvitationAccepted(id,name,avatar){
        createTimeline(id,name,avatar,true)
    }

    function onRoomInvitationDeclined(){
        popStack()
    }

    function createTimeline(id,name,avatar,replace){
        var timeline = timelineFactory.createObject(stack, {"roomid": id,
                                                            "name": name,
                                                            "avatar": avatar});
        if(replace)
            popStack()
        pushToStack(timeline, timeline.name)
    }

    RoomList {
        id: roomList
        visible: false
        onRoomClicked:{
            if(invite){
                var invitationRoom = invitationFactory.createObject(stack, {"roomid": id,
                                                                            "name": name,
                                                                            "avatar": avatar})
                invitationRoom.roomInvitationAccepted.connect(onRoomInvitationAccepted)
                invitationRoom.roomInvitationDeclined.connect(onRoomInvitationDeclined)   
                pushToStack(invitationRoom, invitationRoom.name)
            } else {
                createTimeline(id,name,avatar, false)
            }
        }
    }

    Login {
        id: loginPage
        visible: false
    }

    Connections {
        target: MatrixClient

        function onDropToLogin(msg) {
            replaceInStack(loginPage)
        }

        function onLoginOk(user) {
            MatrixClient.start()
        }

        function onInitiateFinished(){
            replaceInStack(roomList)
        }

        function onUserDisplayNameReady(name){
            displayName = name
            setTitle(displayName)
        }

        function onLogoutErrorOccurred(){
            popStack()
        }

        function onLogoutOk(){
            stack.pop(null)
            loginPage.reload()
            replaceInStack(loginPage)
        }
    }
    
    Component.onCompleted: {
        pushToStack(loginIndicator,"")
        MatrixClient.start()
    }

    onClosing: {
        MatrixClient.stop()
    }
}
