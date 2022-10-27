import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.5

import MatrixClient 1.0
import Rooms 1.0
import GlobalObject 1.0
import QmlInterface 1.0

import "device-verification"

Page {
    id: roomPage
    width: parent.width
    // property string displayName;
    title: "..."
    property var avatar: ""
    ListView {
        id: roomListView
        anchors.fill: parent
        spacing: 0
        anchors.margins: 10
        flickableDirection: Flickable.VerticalFlick
        boundsBehavior: Flickable.StopAtBounds
        ScrollBar.vertical: ScrollBar {}
        model: Rooms
        delegate:RoomDelegate{}
    }

    Component {
        id: directChatFactory
        AddUserDialog {
            title: "Direct Chat"
            x: (qmlLibRoot.width - width) / 2
            y: (qmlLibRoot.height - height) / 2
        }
    }

    RoundButton {
        id: newChatButton
        height: 50
        width: height 
        x: parent.width - width -10
        y: parent.height - height -10
        palette.button: GlobalObject.colors.alternateBase
        font.pointSize: 15            
        text: "+"
        onClicked: {    
            var directChatDialog = directChatFactory.createObject(roomPage);
            directChatDialog.open()
        }
    }   

    function onVerificationStatusChanged(){
        mainHeader.setVerified(selfVerificationCheck.isVerified())
    }

    SelfVerificationCheck{
        id: selfVerificationCheck
    }

    Component.onCompleted: {        
        selfVerificationCheck.statusChanged.connect(onVerificationStatusChanged)
        QmlInterface.onNotificationClicked.connect(openTimeline)
    }
    
    Component {
        id: timelineFactory
        Timeline {}
    }

    function createTimeline(id,name,avatar){
        var currentItem = stack.currentItem
        if((currentItem instanceof Timeline) && (currentItem.roomid == id)){
            return
        }
        var timeline = timelineFactory.createObject(stack, {"roomid": id,
                                                        "name": name,
                                                        "avatar": avatar});
        stack.push(timeline)
    }  


    function openTimeline(roomid){
        var roomInf = Rooms.roomInformation(roomid)
        createTimeline(roomInf.id(),roomInf.name(),roomInf.avatar())
    }
    
    Connections {
        target: MatrixClient

        function onUserDisplayNameReady(name){
            title = name
            mainHeader.setRoomInfo(title, "", avatar)
            onVerificationStatusChanged()
        }
        
        function onUserAvatarReady(avatarUrl){
            avatar = avatarUrl
            mainHeader.setRoomInfo(title, "", avatar)
        }

        function onRoomCreated(id){
            openTimeline(id)
        }
    }
}
