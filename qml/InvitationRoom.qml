import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.2
import TimelineModel 1.0
import MatrixClient 1.0

Page {
    Layout.fillWidth: true
    required property string roomid
    required property string name
    required property string avatar

    signal roomInvitationAccepted(string roomid, string name, string avatar)
    signal roomInvitationDeclined()
    width: parent.width
    ColumnLayout{
        id: inputLayout
        anchors.centerIn: parent
        width: parent.width
        
        Label {
            id: label
            Layout.alignment: Qt.AlignHCenter
            font.pointSize: 14
            text: "Do you want to join in?"
        }

        RowLayout {
            width: parent.width
            Layout.alignment: Qt.AlignCenter
            Button {
                id: joinButton
                text: "Join"
                onClicked: {
                    MatrixClient.joinRoom(roomid)
                    roomInvitationAccepted(roomid, name, avatar)
                }
            }

            Button {
                id: declineButton
                text: "Decline"
                onClicked: {
                    MatrixClient.leaveRoom(roomid)
                    roomInvitationDeclined()
                }
            }
        }
    }
}
