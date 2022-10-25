import QtQuick 2.9
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import QtMultimedia 5.15
import GlobalObject 1.0
import MediaUpload 1.0
import InputVideoFilter 1.0
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
        
                    MediaPlayer { 
                        id: mediaPlayer

                        onError: console.log(modelData.absoluteFilePath)
                        volume: 0
                        muted: true
                        source: modelData.mediaType == MediaUpload.Video ? "file://" + modelData.absoluteFilePath : ""
                        autoPlay: true
                        onPlaying:{ 
                            if(videoOutput.filters[0]) {
                                videoOutput.filters[0].active = true;
                            }
                        }
                    }
                    VideoOutput {
                        id: videoOutput
                        visible: modelData.mediaType == MediaUpload.Video
                        clip: true
                        anchors.fill: parent
                        fillMode: VideoOutput.PreserveAspectFit
                        source: mediaPlayer
                        flushMode: VideoOutput.FirstFrame
                        filters: [ modelData.inputVideoFilter() ]
                        // videoSurface: modelData.mediaType == MediaUpload.Video ? modelData.videoSurface : null
                        // orientation: mediaPlayer.orientation
                    }

                    property string typeStr: switch(modelData.mediaType) {
                        case MediaUpload.Video: return "video-file";
                        case MediaUpload.Audio: return "music";
                        case MediaUpload.Image: return "image";
                        default: return "zip";
                    }
                    source: (modelData.mediaType != MediaUpload.Video) ? "image://colorimage/:/images/"+typeStr+".svg?" + GlobalObject.colors.buttonText : ""
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
