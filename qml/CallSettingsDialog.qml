import QtQuick 2.2
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import Settings 1.0
import CallManager 1.0

Dialog {
    id: callSettings
    title: "Audio/Video Settings"
    standardButtons: StandardButton.Ok | StandardButton.Cancel

    Column {
        ButtonGroup { id: audioGroup }
        ButtonGroup { id: videoGroup }
        spacing: 10
        Column {
            id: audioInputColumn
            Label {
                text: qsTr("Audio Input:")
            }
        }
        
        Column {
            id: videoInputColumn
            Label {
                text: qsTr("Video Input:")
            }
        }
    }

    Component {
        id: radioButtonFactory
        RadioButton {}
    }

    onButtonClicked: {
        if (clickedButton==StandardButton.Ok) {
            var audioButtons = audioGroup.buttons
            for(var i=0; i<audioButtons.length; i++){
                if(audioButtons[i].checked){
                    Settings.microphone = audioButtons[i].text
                    console.log("   - [default mic]: " + audioButtons[i].text)
                    break
                }
            }

            var videoButtons = videoGroup.buttons
            for(var i=0; i<videoButtons.length; i++){
                if(videoButtons[i].checked){
                    Settings.camera = videoButtons[i].text
                    console.log("   - [default cam]: " + videoButtons[i].text)
                    break
                }
            }
        }
    }

    function sortDevices(devices){
        let sortedList = []
        for(var i=0; i<devices.length; i++){
            sortedList.push(devices[i])
        }
        sortedList.sort();
        return sortedList  
    }

    Component.onCompleted: {
        var defaultMicrophone = Settings.microphone
        var defaultCamera = Settings.camera
        var mics = sortDevices(CallManager.mics)
        var cams = sortDevices(CallManager.cameras)
        if(mics.length){
            for (var m = 0; m < mics.length; m++) {
                var rButton = radioButtonFactory.createObject(audioInputColumn, {
                                                                                "text": mics[m], 
                                                                                "ButtonGroup.group": audioGroup,
                                                                                "checked": (mics[m] == defaultMicrophone)
                                                                                })
            }
        } else {
            var button = radioButtonFactory.createObject(audioInputColumn, {
                                                                                "text": "Not connected!", 
                                                                                "ButtonGroup.group": audioGroup,
                                                                                "checkable": false
                                                                                })
        }

        if(cams.length){
            for (var c = 0; c < cams.length; c++) {
                var rButton = radioButtonFactory.createObject(videoInputColumn, {
                                                                                "text": cams[c], 
                                                                                "ButtonGroup.group": videoGroup,
                                                                                "checked": (cams[c] == defaultCamera)
                                                                                })
            }
        } else {
            var button = radioButtonFactory.createObject(videoInputColumn, {
                                                                                "text": "Not connected!", 
                                                                                "ButtonGroup.group": videoGroup,
                                                                                "checkable": false
                                                                                })
        }
    }
}