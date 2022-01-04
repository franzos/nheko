import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

Rectangle {
    width: myListView.width
    height: childrenRect.height
//    color: index % 2 == 0 ? "lightsteelblue" : "transparent"
    RowLayout {
        RoundButton {
            text: roomId
            width: 24; height: 24
            anchors.margins: 10
        }
        Text {
            text: roomName
            anchors.margins: 10
        }
    }
}
