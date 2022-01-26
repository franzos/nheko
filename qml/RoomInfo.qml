import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.5

import Rooms 1.0

CustomPage {
    id: roomInfo
    width: parent.width

    required property string roomid
    
    Component.onCompleted: {
        header.setTitle("Room Info: " + roomid)
        var info = Rooms.roomInformation(roomid)
        console.log(info.id())
        // TODO
    }
}
