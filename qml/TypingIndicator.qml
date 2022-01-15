import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.2
import TimelineModel 1.0

Item {
    implicitHeight: Math.max(fontMetrics.height * 1.2, typingDisplay.height)
    Layout.fillWidth: true

    Rectangle {
        id: typingRect
        anchors.fill: parent
        anchors.margins: 5
        z: 3

        Label {
            id: typingDisplay
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.right: parent.right
            anchors.rightMargin: 10
            anchors.bottom: parent.bottom
            color: "gray"
            textFormat: Text.RichText
        }
    }

    function setTypingDisplayText(text){
        typingDisplay.text = text
    }
}
