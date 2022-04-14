import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.5

import MatrixClient 1.0
import Rooms 1.0
import GlobalObject 1.0

import "device-verification"

CustomPage {
    id: roomPage
    width: parent.width
    property string displayName;

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

    DirectChatDialog{
        id:directChat
        x: (qmlLibRoot.width - width) / 2
        y: (qmlLibRoot.height - height) / 2
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
        onClicked: directChat.open()
    }   

    function onVerificationStatusChanged(){
        header.setVerified(selfVerificationCheck.isVerified())
    }

    SelfVerificationCheck{
        id: selfVerificationCheck
    }

    Component.onCompleted: {        
        selfVerificationCheck.statusChanged.connect(onVerificationStatusChanged)
    }
    
    Component {
        id: timelineFactory
        Timeline {}
    }

    function createTimeline(id,name,avatar){
        var timeline = timelineFactory.createObject(stack, {"roomid": id,
                                                            "name": name,
                                                            "avatar": avatar});
        stack.push(timeline)
    }  

    Connections {
        target: MatrixClient

        function onUserDisplayNameReady(name){
            displayName = name
            header.setTitle(displayName)
            onVerificationStatusChanged()
        }
        function onRoomCreated(id){
            var roomInf = Rooms.roomInformation(id)
            createTimeline(roomInf.id(),roomInf.name(),roomInf.avatar())
        }       
       
    }
}
