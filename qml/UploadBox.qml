import QtQuick 2.9
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import GlobalObject 1.0
import MediaUpload 1.0
import "./ui"

Page {
    id: uploadPopup
    visible: room && room.input.uploads.length > 0
    Layout.preferredHeight: 200
    clip: true
    property var room: timelineModel

    Layout.fillWidth: true

    padding: 8

    contentItem: ListView {
        id: uploadsList
        anchors.horizontalCenter: parent.horizontalCenter
        boundsBehavior: Flickable.StopAtBounds

        ScrollBar.horizontal: ScrollBar {
            id: scr
        }

        orientation: ListView.Horizontal
        width: Math.min(contentWidth, parent.availableWidth)
        model: room ? room.input.uploads : undefined
        spacing: 8

        delegate: Pane {
            padding: 4
            height: uploadPopup.availableHeight - buttons.height - (scr.visible? scr.height : 0)
            width: uploadPopup.availableHeight - buttons.height

            background: Rectangle {
                color: GlobalObject.colors.window
                radius: 8
            }
            contentItem: ColumnLayout {
                Image {
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    sourceSize.height: height
                    sourceSize.width: width
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                    mipmap: true

                    property string typeStr: switch(modelData.mediaType) {
                        case MediaUpload.Video: return "video-file";
                        case MediaUpload.Audio: return "music";
                        case MediaUpload.Image: return "image";
                        default: return "zip";
                    }
                    source: (modelData.thumbnail != "") ? modelData.thumbnail : ("image://colorimage/:/images/"+typeStr+".svg?" + GlobalObject.colors.buttonText)
                }
                MatrixTextField {
                    Layout.fillWidth: true
                    text: modelData.filename
                    onTextEdited: modelData.filename = text
                }
            }
        }
    }

    footer: DialogButtonBox {
        id: buttons

        standardButtons: DialogButtonBox.Cancel
        Button {
            text: qsTr("Upload %n file(s)", "", (room ? room.input.uploads.length : 0))
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
        }
        onAccepted: room.input.acceptUploads()
        onRejected: room.input.declineUploads()
    }

    background: Rectangle {
        color: GlobalObject.colors.base
    }
}
