import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2

import MatrixClient 1.0
import Rooms 1.0
// import RoomInformation 1.0

Rectangle {
    id: room
    width: roomListView.width
    height: childrenRect.height
    required property string index
    required property string id
    required property string name
    required property string avatar
    required property bool invite
    required property string lastmessage
    required property int unreadcount
    // required property RoomInformation roomInformation
//    color: index % 2 == 0 ? "lightsteelblue" : "transparent"

    Component {
        id: timelineFactory
        Timeline {}
    }

    Component {
        id: invitationFactory
        InvitationRoom {}
    }

    function createTimeline(id,name,avatar){
        var timeline = timelineFactory.createObject(stack, {"roomid": id,
                                                            "name": name,
                                                            "avatar": avatar});
        stack.push(timeline)
    }  

    RowLayout {
        width: parent.width
        RoundButton {
            id: avatar_button
            text: name[0]
            width: 24; height: 24
            anchors.margins: 10
        }
        
        ColumnLayout{
            Layout.fillWidth: true
            width: parent.width - avatar_button.width
            Layout.preferredWidth: parent.width - avatar_button.width
            Label {
                text: name
                font.italic: invite ? true : false
                font.pointSize: 12
            }
            Label {
                text: invite ? "Pending invite." : lastmessage 
                font.pointSize: 9
                color: "gray"
            }
        }
        
        Rectangle {
            id: rect
            width: 20
            height: 20
            radius: width/2
            color: "red"
            visible: (!invite && unreadcount) ? true : false
            Layout.alignment: Qt.AlignRight
            Label {
                anchors.centerIn: parent
                text: unreadcount
                color: "white"
                font.pointSize: 9
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }

    MouseArea {    
        anchors.fill: parent
        onClicked: {
            if(room.invite){
                var invitationRoom = invitationFactory.createObject(stack, {"roomid": room.id,
                                                                            "name": room.name,
                                                                            "avatar": room.avatar})
                invitationRoom.roomInvitationAccepted.connect(createTimeline)
                stack.push(invitationRoom)
            } else {
                createTimeline(room.id, room.name, room.avatar)
            }   
        }       
    }
}
