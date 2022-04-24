import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.2

import MatrixClient 1.0
import Rooms 1.0
Page {
    Layout.fillWidth: true
    required property string roomid
    required property string name
    required property string avatar
    
    signal roomInvitationAccepted(string roomid, string name, string avatar)

    Component {
        id: roomInfoFactory
        RoomInfo {}
    }

    function showRoomInfo(){
        var roominfo = roomInfoFactory.createObject(stack, {"roomid":roomid});
        stack.push(roominfo)
    }
    
    function onJoinedRoom(id){
        if(id == roomid) {
            stack.pop()
            roomInvitationAccepted(roomid, name, avatar)
        }
    }

    function goToPrevPage(){
        var prevPage = stack.pop()
        if (prevPage) {
            prevPage.destroy()
        }
    }

    Component.onCompleted: {
        title = name
        mainHeader.titleClicked.connect(showRoomInfo) 
        MatrixClient.onJoinedRoom.connect(onJoinedRoom)
    }

    Component.onDestruction: {
        mainHeader.titleClicked.disconnect(showRoomInfo)
        MatrixClient.onJoinedRoom.disconnect(onJoinedRoom)
    }
}
