import QtQuick 2.2
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.12
import Settings 1.0
import CallManager 1.0
import GlobalObject 1.0
import InputDeviceInfo 1.0
import AudioInputControl 1.0

Dialog {
    id: callSettings
    title: "Audio/Video Settings"
    standardButtons: StandardButton.Ok | StandardButton.Cancel
    onVisibilityChanged: {
        if (!this.visible){
            disconnectSignals()
        }
    }

    Column {
        width: parent.width
        spacing: 20

        Column {
            width: parent.width
            spacing: 5
            Label {
                text: qsTr("Audio Input:")
            }
            ComboBox {
                id: audioCombo
                editable: false
                flat: true  
                width: parent.width
                
                Layout.leftMargin: 50
                Layout.rightMargin: 50
                background:Rectangle {
                    implicitWidth: 100
                    implicitHeight: 40
                    color: GlobalObject.colors.window
                    border.color: GlobalObject.colors.windowText
                } 
                model: ListModel {}
                onActivated: {
                    updateVolumeAndLevelMeter(audioCombo.currentText)
                }
            }
            Slider {
                id: volumeSlider
                width: parent.width
                visible: Qt.platform.os != "android"
                onMoved: {
                    if (Qt.platform.os != "android")
                        AudioInputControl.setVolume(audioCombo.currentText, volumeSlider.value)
                }
            }
            LinearGradient {
                id: levelGradient
                width: parent.width
                visible: Qt.platform.os != "android"
                height: 5
                start: Qt.point(0, 0)
                end: Qt.point(parent.width, 0)
                gradient: Gradient {
                    GradientStop { position: 0.0; color: "blue" }
                    GradientStop { position: 1.0; color: "black" }
                }
            }
        }

        Column {
            width: parent.width
            spacing: 5
            Label {
                text: qsTr("Video Input:")
            }
            ComboBox {
                id: videoCombo
                editable: false
                flat: true  
                width: parent.width
                Layout.leftMargin: 50
                Layout.rightMargin: 50
                background:Rectangle {
                    implicitWidth: 100
                    implicitHeight: 40
                    color: GlobalObject.colors.window
                    border.color: GlobalObject.colors.windowText
                } 
                model: ListModel {}   
            }
        }
    }

    onButtonClicked: {
        if (clickedButton==StandardButton.Ok) {
            Settings.microphone = audioCombo.currentText
            Settings.camera = videoCombo.currentText
            console.log("   - [default mic]: " + audioCombo.currentText)
            console.log("   - [default cam]: " + videoCombo.currentText)
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

    function updateVolumeAndLevelMeter(device){
        if (Qt.platform.os != "android") {
            AudioInputControl.deviceChanged(device)
            volumeSlider.value = AudioInputControl.getVolume(device)
        }
    }

    function onLevelChangedCallback(level) {
        levelGradient.end = Qt.point(parent.width * (level + 0.0001), 0)
    }

    function onNewDeviceStatusCallback(index){
        var info = AudioInputControl.deviceInfo(index)
        if(info.desc == audioCombo.currentText){
            volumeSlider.value = AudioInputControl.getVolume(audioCombo.currentText)
        }
    }
    
    function disconnectSignals() {
        if (Qt.platform.os != "android") {
            AudioInputControl.onLevelChanged.disconnect(onLevelChangedCallback)
            AudioInputControl.onNewDeviceStatus.disconnect(onNewDeviceStatusCallback)
        }
    }

    Component.onDestruction: {
        disconnectSignals()
    }

    Component.onCompleted: {
        var defaultMicrophone = Settings.microphone
        var defaultCamera = Settings.camera
        var mics = sortDevices(CallManager.mics)
        var cams = sortDevices(CallManager.cameras)
        audioCombo.model.clear()
        videoCombo.model.clear()
        if(mics.length){
            for (var m = 0; m < mics.length; m++) {
                audioCombo.model.append({text: mics[m]})
            }
            audioCombo.currentIndex = audioCombo.indexOfValue(defaultMicrophone)
            updateVolumeAndLevelMeter(defaultMicrophone)
        } else {
            audioCombo.displayText="Not connected!"
        }

        if(cams.length){
            for (var c = 0; c < cams.length; c++) {
                videoCombo.model.append({text: cams[c]})
            }
            videoCombo.currentIndex = videoCombo.indexOfValue(defaultCamera)
        } else {
            videoCombo.displayText = "Not connected!"
        }
        if (Qt.platform.os != "android") {
            AudioInputControl.onLevelChanged.connect(onLevelChangedCallback)
            AudioInputControl.onNewDeviceStatus.connect(onNewDeviceStatusCallback)
        }
    }
}