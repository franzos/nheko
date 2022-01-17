import QtQuick 2.9
import QtQuick.Window 2.0
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

import MatrixClient 1.0

ApplicationWindow {
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

            Label {
                id: titleLabel
                width: parent.width - backButton.width - logoutButton.width - 10
                anchors.leftMargin: 10
                // anchors.centerIn: parent
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: backButton.right
            }

            Button {
                id: logoutButton
                text: "Logout"
                height: parent.height
                anchors.left: titleLabel.right
                onClicked:{
                    MatrixClient.logout()
                }
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

    function replaceInStack(oldPage,newPage){
        stack.replace(oldPage,newPage)
        if (stack.currentItem == roomList){
            setTitle(displayName)
            header.visible= true
        } else if (stack.currentItem == loginPage){
            header.visible= false
        }
    }

    function pushToStack(item, title){
        stack.push(item)
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
            replaceInStack(loginIndicator,loginPage)
        }

        function onLoginOk(user) {
            MatrixClient.start()
        }

        function onInitiateFinished(){
            replaceInStack(loginIndicator, roomList)
        }

        function onUserDisplayNameReady(name){
            displayName = name
            setTitle(displayName)
        }
    }

    Component.onCompleted: {
        pushToStack(loginIndicator,"Login")
        header.visible = false
        MatrixClient.start()
    }

    onClosing: {
        MatrixClient.stop()
    }
}
