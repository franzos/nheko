import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

Rectangle {
    id: event
    width: timelineView.width
    height: childrenRect.height
    required property string index
    required property string id
    required property string senderId
    required property string body
    required property string descriptiveTime
    required property int timestamp
    required property bool isLocal

    RowLayout {
        width: parent.width
        RoundButton {
            id: avatar_button
            text: senderId[0]
            width: 24; height: 24
            anchors.margins: 10
        }
        Rectangle{
            anchors.left: avatar_button.right
            anchors.margins: 10
            ColumnLayout{
                Label {
                    text: senderId
                    color: (isLocal ? "green" : "red")
                }
                Label {
                    text: body
                    x: 20
                    y: 20
                    wrapMode: Label.WordWrap
                }
            }
        }
    }
}
