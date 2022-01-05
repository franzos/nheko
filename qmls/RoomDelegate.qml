import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

import Rooms 1.0

Rectangle {
    width: roomListView.width
    height: childrenRect.height
//    color: index % 2 == 0 ? "lightsteelblue" : "transparent"
    RowLayout {
        RoundButton {
            text: Rooms.data(Rooms.index(index, 0), Qt.DisplayRole)[0]
            width: 24; height: 24
            anchors.margins: 10
        }
        Text {
            text: Rooms.data(Rooms.index(index, 0), Qt.DisplayRole)
            anchors.margins: 10
        }
    }
}
