import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls.Material 2.12
import QtQuick.Controls 2.5

Page {
    id: roomPage
    width: parent.width
    ListView {
        id: myListView
        anchors.fill: parent
        spacing: 10
        anchors.margins: 10
        flickableDirection: Flickable.VerticalFlick
        boundsBehavior: Flickable.StopAtBounds
        ScrollBar.vertical: ScrollBar {}

        header: Rectangle {
            width: myListView.width
            height: 30
            Text {
                anchors.centerIn: parent
                text: "Room List"
            }
        }
        ListModel {
            id: listModel
        }
        model: listModel
        delegate: RoomDelegate{}

        Component.onCompleted: {
            listModel.append({roomName: "Daily", roomId: "H"})
            listModel.append({roomName: "Scrum", roomId: "1"})
            listModel.append({roomName: "Hamzeh", roomId: "4"})
        }
    }
}
