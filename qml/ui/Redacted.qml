import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GlobalObject 1.0

Rectangle{

    height: redactedLayout.implicitHeight + 4
    implicitWidth: redactedLayout.implicitWidth + 2 * 8
    width: Math.min(parent.width,implicitWidth+1)
    radius: fontMetrics.lineSpacing / 2 + 2 * 4
    color: GlobalObject.colors.alternateBase
    property int metadataWidth
    property bool fitsMetadata: parent.width - redactedLayout.width > metadataWidth + 4

    RowLayout {
        id: redactedLayout
        anchors.centerIn: parent
        width: parent.width - 2 * 8
        spacing: 4

        Image {
            id: trashImg
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            Layout.preferredWidth: fontMetrics.font.pixelSize
            Layout.preferredHeight: fontMetrics.font.pixelSize
            source: "image://colorimage/:/images/delete.svg?" + GlobalObject.colors.text
        }
        Label {
            id: redactedLabel
            Layout.margins: 0
            Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
            Layout.preferredWidth: implicitWidth
            Layout.fillWidth: true
            property var redactedPair: room.formatRedactedEvent(eventId)
            text: redactedPair["first"]
            wrapMode: Label.WordWrap
            color: GlobalObject.colors.text

            ToolTip.text: redactedPair["second"]
            ToolTip.visible: hh.hovered
            HoverHandler {
                id: hh
            }
        }
    }
}
