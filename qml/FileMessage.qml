import QtQuick 2.12
import QtQuick.Layouts 1.2
import GlobalObject 1.0
import CursorShape 1.0

Item {
    required property string eventId
    required property string filename
    required property string filesize

    height: row.height + 16 //(Settings.bubbles? 16: 24)
    width: parent.width
    implicitWidth: row.implicitWidth+metadataWidth
    property int metadataWidth
    property bool fitsMetadata: true

    RowLayout {
        id: row

        anchors.centerIn: parent
        width: parent.width - 16 //(Settings.bubbles? 16 : 24)
        spacing: 15

        Rectangle {
            id: button

            color: GlobalObject.colors.light
            radius: 22
            height: 44
            width: 44

            Image {
                id: img

                height: 40
                width: 40
                sourceSize.height: 40
                sourceSize.width: 40

                anchors.centerIn: parent
                source: "qrc:/images/download.svg"
                fillMode: Image.Pad
            }

            TapHandler {
                onSingleTapped: room.saveMedia(eventId)
                gesturePolicy: TapHandler.ReleaseWithinBounds
            }

            CursorShape {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
            }

        }

        ColumnLayout {
            id: col

            Text {
                id: filename_

                Layout.fillWidth: true
                text: filename
                textFormat: Text.PlainText
                elide: Text.ElideRight
                color: GlobalObject.colors.text
            }

            Text {
                id: filesize_

                Layout.fillWidth: true
                text: filesize
                textFormat: Text.PlainText
                elide: Text.ElideRight
                color: GlobalObject.colors.text
            }

        }

    }

    Rectangle {
        color: GlobalObject.colors.alternateBase
        z: -1
        radius: 10
        anchors.fill: parent
        visible: false //!Settings.bubbles // the bubble in a bubble looks odd
    }

}
