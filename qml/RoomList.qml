import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.5

import MatrixClient 1.0
import Rooms 1.0

Page {
    id: roomPage
    width: parent.width
    signal roomClicked(string id, string name, string avatar, bool invite)
    
    ListView {
        id: roomListView
        anchors.fill: parent
        spacing: 10
        anchors.margins: 10
        flickableDirection: Flickable.VerticalFlick
        boundsBehavior: Flickable.StopAtBounds
        ScrollBar.vertical: ScrollBar {}
        model: Rooms
        delegate:RoomDelegate{
            id: roomItems
            onClicked: {
                roomClicked(id, name, avatar, invite)                
            }
        }
    }
}
