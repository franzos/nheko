import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import GlobalObject 1.0
import "ui"

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
    color: "transparent"

    RowLayout {
        Layout.fillWidth: true
        width: parent.width
        Layout.preferredWidth: parent.width 
        Layout.margins: 5

        RoundButton {
            id: avatar_button
            text: senderId[0]
            width: 20; height: 20
            anchors.margins: 10
            palette.button: GlobalObject.colors.alternateBase
            Layout.alignment: Qt.AlignVCenter
        }

        // Avatar {
        //     id: avatar_button
        //     width: 20; height: 20
        //     anchors.margins: 10
        //     url: info.avatar().replace("mxc://", "image://MxcImage/")
        //     userid: senderId
        //     displayName: senderId
        //     Layout.alignment: Qt.AlignVCenter
        // }

        ColumnLayout {
            Layout.fillWidth: true
            width: parent.width - avatar_button.width
            Layout.preferredWidth: parent.width - avatar_button.width
            
            Row{
                id: topRow
                Layout.fillWidth: true
                width: parent.width - avatar_button.width
                Label {
                    text: senderId
                    color: (isLocal ? "green" : "red")
                    width: topRow.width - timelabel.width
                }
                Label {
                    id: timelabel
                    text: descriptiveTime
                    color: "gray"
                }
            }
            Label {
                text: body
                x: 20
                y: 20
                Layout.maximumWidth: parent.width - avatar_button.width
                wrapMode: Label.WordWrap
            }
        }
    }
}
