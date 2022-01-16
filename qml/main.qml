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
                onClicked: stack.pop()
            }

            Label {
                id: titleLabel
                anchors.centerIn: parent
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

    function onRoomInvitationAccepted(id,name,avatar){
        createTimeline(id,name,avatar,true)
    }

    function onRoomInvitationDeclined(){
        setTitle("Room List")
        stack.pop()
    }

    function createTimeline(id,name,avatar,replace){
        var timeline = timelineFactory.createObject(stack, {"roomid": id,
                                                            "name": name,
                                                            "avatar": avatar});
        setTitle(timeline.name)
        if(replace)
            stack.pop()
        stack.push(timeline)
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
                setTitle(invitationRoom.name)
                stack.push(invitationRoom)    
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
            stack.replace(loginIndicator,loginPage)
        }

        function onLoginOk(user) {
            MatrixClient.start()
        }

        function onInitiateFinished(){
            setTitle("Room List")
            stack.replace(loginIndicator, roomList)
        }
    }

    Component.onCompleted: {
        stack.push(loginIndicator)
        setTitle("Login")
        MatrixClient.start()
    }

    onClosing: {
        MatrixClient.stop()
    }
}
