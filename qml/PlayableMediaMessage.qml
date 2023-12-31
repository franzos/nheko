import "./media"
import QtMultimedia 5.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GlobalObject 1.0
import MxcMedia 1.0
import MtxEvent 1.0

Item {
    id: content

    required property double proportionalHeight
    required property int type
    required property int originalWidth
    required property int duration
    required property string thumbnailUrl
    required property string eventId
    required property string url
    required property string body
    required property string filesize
    property double divisor: isReply ? 4 : 2
    property int tempWidth: originalWidth < 1? 400: originalWidth
    implicitWidth: type == MtxEvent.VideoMessage ? Math.round(tempWidth*Math.min((timeline.height/divisor)/(tempWidth*proportionalHeight), 1)) : 500
    width: Math.min(parent.width, implicitWidth)
    height: ((type == MtxEvent.VideoMessage) ? width*proportionalHeight : 80) + fileInfoLabel.height
    implicitHeight: height

    property int metadataWidth
    property bool fitsMetadata: (parent.width - fileInfoLabel.width) > metadataWidth+4
    property bool playAfterDownload: false

    MxcMedia { 
        id: mxcmedia
        roomm: room
        eventId: content.eventId
        onMediaFilehanged: {
            mediaPlayer.source = mediaFile
            busyIndicator.visible = false
            if(playAfterDownload){
                playAfterDownload=false
                mediaPlayer.play()
            }
        }
    } 

    MediaPlayer { 
        id: mediaPlayer
 
        // TODO: Show error in overlay or so?
        onError: console.log(error)
        volume: mediaControls.desiredVolume
        muted: mediaControls.muted
    }

    Rectangle {
        id: videoContainer

        color: type == MtxEvent.VideoMessage ? GlobalObject.colors.window : "transparent"
        width: parent.width
        height: parent.height - fileInfoLabel.height

        // TapHandler {
        //     onTapped: room.openMedia(eventId) //Settings.openVideoExternal ? room.openMedia(eventId) : mediaControls.showControls()
        // }

        Image {
            anchors.fill: parent
            source: thumbnailUrl ? thumbnailUrl.replace("mxc://", "image://MxcImage/") + "?scale" : ""
            asynchronous: true
            fillMode: Image.PreserveAspectFit

            VideoOutput {
                id: videoOutput

                visible: type == MtxEvent.VideoMessage
                clip: true
                anchors.fill: parent
                fillMode: VideoOutput.PreserveAspectFit
                source: mediaPlayer
                flushMode: VideoOutput.FirstFrame
                // orientation: mediaPlayer.orientation
            }
            BusyIndicator {
                id: busyIndicator
                anchors.centerIn: parent    
                visible: false
                width: 64; height: width
                palette.dark: GlobalObject.colors.windowText
            }
        }

    }

    MediaControls {
        id: mediaControls

        anchors.left: content.left
        anchors.right: content.right
        anchors.bottom: fileInfoLabel.top
        playingVideo: type == MtxEvent.VideoMessage
        positionValue: mediaPlayer.position
        duration: mediaLoaded ? mediaPlayer.duration : content.duration
        mediaLoaded: mediaPlayer.source!=""
        mediaState: mediaPlayer.playbackState
        onPositionChanged: mediaPlayer.position = position
        onPlayPauseActivated: {
            if(!playAfterDownload){
                if(mediaPlayer.playbackState == MediaPlayer.PlayingState){
                    mediaPlayer.pause()
                } else {
                    if(mediaPlayer.source!=""){
                        mediaPlayer.play()
                    } else {
                        console.log("Media source isn't exist in the cache, go for download ...")
                        playAfterDownload = true
                        busyIndicator.visible = true
                        mxcmedia.startDownload(true)
                    }
                }
            }
        }
        onLoadActivated: {
            busyIndicator.visible = true
            mxcmedia.startDownload(false)
        }
    }

    // information about file name and file size
    Label {
        id: fileInfoLabel

        anchors.bottom: content.bottom
        text: filesize
        textFormat: Text.RichText
        elide: Text.ElideRight
        color: GlobalObject.colors.text

        background: Rectangle {
            color: GlobalObject.colors.base
        }

    }

}
