import QtQuick 2.2
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.12
import Settings 1.0
import CallManager 1.0
import GlobalObject 1.0
import AudioDeviceInfo 1.0
import AudioDeviceControl 1.0

CustomApplicationWindow {
    id: callSettings
    title: "Audio/Video Settings"
    minimumWidth: 340
//    minimumHeight: 450
    width: 450
    height: 680
    flags: Qt.Dialog | Qt.WindowCloseButtonHint | Qt.WindowTitleHint
    visibility: window.Maximized

    ScrollView {
        id: scroll
        clip: true
        anchors.fill: parent 
        ScrollBar.horizontal.visible: false
        ScrollBar.vertical.visible: true
        width: parent.width
        anchors.topMargin: 20
        anchors.leftMargin: 20
        anchors.rightMargin: 20
        Column {
            width: parent.width
            spacing: 30

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
                        updateMicVolumeAndLevelMeter(audioCombo.currentText)
                    }
                }
                Slider {
                    id: micVolumeSlider
                    width: parent.width
                    visible: !GlobalObject.mobileMode()
                    onMoved: {
                        if (!GlobalObject.mobileMode())
                            AudioDeviceControl.setMicrophoneVolume(audioCombo.currentText, micVolumeSlider.value)
                    }
                }
                LinearGradient {
                    id: levelGradient
                    width: parent.width
                    visible: !GlobalObject.mobileMode()
                    height: 5
                    start: Qt.point(0, 0)
                    end: Qt.point(parent.width, 0)
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: "blue" }
                        GradientStop { position: 1.0; color: GlobalObject.colors.base } 
                    }
                }
            }
            Column {
                width: parent.width
                spacing: 5
                Label {
                    text: qsTr("Audio Output:")
                }
                ComboBox {
                    id: audioOutputCombo
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
                        AudioDeviceControl.setDefaultAudioOutput(audioOutputCombo.currentText)
                        updateSpkVolumeAndLevelMeter(audioOutputCombo.currentText)
                    }
                }
                Slider {
                    id: spkVolumeSlider
                    width: parent.width
                    visible: !GlobalObject.mobileMode()
                    onMoved: {
                        if (!GlobalObject.mobileMode())
                            AudioDeviceControl.setSpeakerVolume(audioOutputCombo.currentText,spkVolumeSlider.value)
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
                    onActivated:{}
                }
            }
        }
    }

    footer: DialogButtonBox {
        standardButtons: DialogButtonBox.Ok
        onAccepted: {
            Settings.microphone = audioCombo.currentText
            Settings.camera = videoCombo.currentText
            console.log("   - [default mic]: " + audioCombo.currentText)
            console.log("   - [default spk]: " + audioOutputCombo.currentText)
            console.log("   - [default cam]: " + videoCombo.currentText)
            close()
        }
        background: Rectangle {
            anchors.fill: parent
            color: GlobalObject.colors.window
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

    function updateMicVolumeAndLevelMeter(device){
        if (!GlobalObject.mobileMode()) {
            AudioDeviceControl.deviceChanged(device)
            micVolumeSlider.value = AudioDeviceControl.getMicrophoneVolume(device)
        }
    }

    function updateSpkVolumeAndLevelMeter(device){
        if (!GlobalObject.mobileMode()) {
            spkVolumeSlider.value = AudioDeviceControl.getSpeakerVolume(device)
        }
    }

    function onLevelChangedCallback(level) {
        levelGradient.end = Qt.point(parent.width * (level + 0.0001), 0)
    }

    
    function onNewInputDeviceStatusCallback(index){
        var info = AudioDeviceControl.deviceInfo(index)
        if(info.desc == audioCombo.currentText){
            micVolumeSlider.value = AudioDeviceControl.getMicrophoneVolume(audioCombo.currentText)
        }
    }

    function onDefaultOutputDeviceChangedCallback(index){
        // Nothing for now
    }

    function onNewOutputDeviceStatusCallback(index){
        var defaultAudioOut = AudioDeviceControl.defaultAudioOutput();
        var spks = sortDevices(AudioDeviceControl.audioOutputDevices())
        audioOutputCombo.model.clear()
        if(spks.length){
            for (var s = 0; s < spks.length; s++) {
                audioOutputCombo.model.append({text: spks[s]})
            }
            audioOutputCombo.currentIndex = audioOutputCombo.indexOfValue(defaultAudioOut)
        } else {
            audioOutputCombo.displayText = "Not connected!"
        }
        updateSpkVolumeAndLevelMeter(audioOutputCombo.currentText)
    }
    
    function disconnectSignals() {
        if (!GlobalObject.mobileMode()) {
            AudioDeviceControl.onLevelChanged.disconnect(onLevelChangedCallback)
            AudioDeviceControl.onNewInputDeviceStatus.disconnect(onNewInputDeviceStatusCallback)
            AudioDeviceControl.onNewOutputDeviceStatus.disconnect(onNewOutputDeviceStatusCallback)
            AudioDeviceControl.onDefaultOutputDeviceChanged.disconnect(onDefaultOutputDeviceChangedCallback)
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
            updateMicVolumeAndLevelMeter(defaultMicrophone)
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
        onNewOutputDeviceStatusCallback()
        if (!GlobalObject.mobileMode()) {
            AudioDeviceControl.onLevelChanged.connect(onLevelChangedCallback)
            AudioDeviceControl.onNewInputDeviceStatus.connect(onNewInputDeviceStatusCallback)
            AudioDeviceControl.onNewOutputDeviceStatus.connect(onNewOutputDeviceStatusCallback)
            AudioDeviceControl.onDefaultOutputDeviceChanged.connect(onDefaultOutputDeviceChangedCallback)
        }
    }
}