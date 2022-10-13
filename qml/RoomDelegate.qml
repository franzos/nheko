import QtQuick 2.5
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import GlobalObject 1.0
import MatrixClient 1.0
import Rooms 1.0
import "ui"

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
    required property string lastmessageTime
    required property int unreadcount
    // required property RoomInformation roomInformation
//    color: index % 2 == 0 ? "lightsteelblue" : "transparent"
    color: "transparent"

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
                                                            "avatar": avatar})
        stack.push(timeline)
    }  
    ColumnLayout{
        width: parent.width

        RowLayout {
            width: parent.width
            Layout.margins: 5
            
            Avatar {
                id: avatar_button
                width: 36; height: 36
                anchors.margins: 10
                url: avatar.replace("mxc://", "image://MxcImage/")
                userid: id 
                displayName: name
            }
            
            ColumnLayout{
                Layout.fillWidth: true
                width: parent.width - avatar_button.width
                Layout.preferredWidth: parent.width - avatar_button.width
                Layout.leftMargin: 5; Layout.rightMargin: 5
                Label {
                    text: name
                    font.italic: invite ? true : false
                    font.pointSize: 14
                    color: GlobalObject.colors.windowText
                }
                Label {
                    text: invite ? "Pending invite." : lastmessage 
                    font.pointSize: 10
                    color: "gray"
                }
            }
            
            ColumnLayout{
                Layout.fillWidth: true
                Layout.leftMargin: 5; Layout.rightMargin: 5
                Rectangle {
                    id: rect
                    width: 20
                    height: 20
                    radius: width/2
                    color: "#03A9F4"
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

                Label {
                    text: lastmessageTime
                    Layout.alignment: Qt.AlignRight
                    color: "gray"
                }
            }
        }
        Rectangle{
            width: parent.width
            height: 1
            color: GlobalObject.colors.alternateBase
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
