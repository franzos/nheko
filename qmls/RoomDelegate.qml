import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

import Rooms 1.0

Rectangle {
    width: roomListView.width
    height: childrenRect.height
    required property string id
    required property string name
    required property string avatar
    required property bool invite
//    color: index % 2 == 0 ? "lightsteelblue" : "transparent"
    RowLayout {
        RoundButton {
            text: name[0]
            width: 24; height: 24
            anchors.margins: 10
        }
        Label {
            text: name
            anchors.margins: 10
            font.italic: invite ? true : false
        }
    }
}
