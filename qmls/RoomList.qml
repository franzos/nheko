import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.5

import MatrixClient 1.0
import Rooms 1.0

Page {
    id: roomPage
    width: parent.width
    ListView {
        id: roomListView
        anchors.fill: parent
        spacing: 10
        anchors.margins: 10
        flickableDirection: Flickable.VerticalFlick
        boundsBehavior: Flickable.StopAtBounds
        ScrollBar.vertical: ScrollBar {}
        model: Rooms
        header: Rectangle {
            width: parent.width
            height: 30
            Text {
                anchors.centerIn: parent
                text: "Room List"
            }
        }
        delegate:RoomDelegate{}
    }
}
