import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.15
import MatrixClient 1.0
import CallManager 1.0
import CallDevices 1.0
import CallType 1.0
import QmlInterface 1.0
import GlobalObject 1.0
import "voip"
import "device-verification"
import "ui"

Column {
    width: toolBar.width
    height: toolBar.height
    signal titleClicked()
    signal menuClicked()
    signal voiceCallClicked()
    signal videoCallClicked()
    signal optionClicked(Item item)
    property bool enableCallButtons: false
    property bool inCalling: false

    Component {
        id: callSettingsDialogFactory
        CallSettingsDialog {
            x: (qmlLibRoot.width - width) / 2
            y: (qmlLibRoot.height - height) / 2
        }
    }

    ToolBar {
        id: toolBar
        width: parent.width
        RowLayout {
            anchors.fill: parent
            spacing: 5
            ToolButton{
                id: menuButton
                icon.source: "qrc:/images/slide-icon.svg"
                width: parent.height
                height: parent.height
                visible: stack.depth == 1
                onClicked: {
                    menuClicked()
                }
            }
            
            ToolButton {
                id: backButton
                icon.source: "qrc:/images/angle-arrow-left.svg"
                width: parent.height
                height: parent.height
                visible: stack.depth > 1
                onClicked: {
                    if(!inCalling || (CallManager.callType == CallType.VOICE)){
//                        var prevPage = stack.pop()
//                        if (prevPage) {
//                            prevPage.destroy()
//                        }
                        qmlLibRoot.backPressed()
                    }
                }
            }

            ToolButton {
                id: verifyRect
                icon.source: "qrc:/images/shield-filled-exclamation-mark.svg"
                width: parent.height
                height: parent.height
                onClicked: {
                    selfVerificationCheck.verify()
                }
            }

            ToolButton {
                id: callSettingsItem
                icon.source: "qrc:/images/call-settings.svg"
                icon.color:"gray"
                width: parent.height
                height: parent.height
                visible: CallManager.callsSupported
                onClicked: {
                    var callSettingsDialog = callSettingsDialogFactory.createObject(parent);
                    callSettingsDialog.open()
                }
            }

            Avatar {
                id: avatar
                Layout.alignment: Qt.AlignVCenter
                width: GlobalObject.avatarSize - 5
                height: width
                visible: false
            }

            Item{
                Layout.fillWidth: true
                Layout.fillHeight: true
                Label {
                    id: titleLabel
                    width: parent.width  
                    height: parent.height
                    anchors.leftMargin: 2
                    verticalAlignment:Text.AlignVCenter
                }

                MouseArea {
                    id: ma
                    height: titleLabel.height            
                    width: titleLabel.width            
                    onClicked: {
                        if(stack.currentItem instanceof Timeline)
                            titleClicked()
                    }
                }
            }

            ToolButton {
                id: voiceCallButton
                icon.source: "qrc:/images/place-call.svg"
                width: parent.height
                height: parent.height
                visible: enableCallButtons
                onClicked: {voiceCallClicked()}
            }

            ToolButton {
                id: videoCallButton
                icon.source: "qrc:/images/video.svg"
                width: parent.height
                height: parent.height
                visible: enableCallButtons
                onClicked: {videoCallClicked()}
            } 

            ToolButton {
                id: endCallButton
                icon.source: "qrc:/images/end-call.svg"
                width: parent.height
                height: parent.height
                visible: enableCallButtons
                onClicked: { endCallClicked()}
            }
            
            ToolButton {
                id: optionsButton
                icon.source: "qrc:/images/options.svg"
                width: parent.height
                height: parent.height
                visible: false
                onClicked: {
                    optionClicked(optionsButton)
                }
            }
        }
    }
   
    ActiveCallBar {
        id: callStatusbar
        width: parent.width
        color: "#09af00"
        visible: false
    }

    function setCallButtonsVisible(visible){
        if(visible){
            if(CallDevices.haveCamera())
                videoCallButton.visible = visible;
            else 
                videoCallButton.visible = false;
            if(CallDevices.haveMic())
                voiceCallButton.visible = visible;
            else 
                voiceCallButton.visible = false;
        } else {
            voiceCallButton.visible = visible;
            videoCallButton.visible = visible;
        }
    }

    function setEndCallButtonsVisible(visible){
        endCallButton.visible = visible;
    }

    function setOptionButtonsVisible(visible){        
        optionsButton.visible = visible;
    }

    function setRoomInfo(title, roomid, avatarUrl){
        titleLabel.text = title
        avatar.url= avatarUrl.replace("mxc://", "image://MxcImage/")
        avatar.roomid= roomid
        avatar.userid= roomid
        avatar.displayName= title
        avatar.visible=true
    }

    function setVerified(flag){
        if(flag){
            verifyRect.icon.color = "green"
        } else {
            verifyRect.icon.color = "#C70039"
        }
    }

    SelfVerificationCheck{
        id: selfVerificationCheck
    }

    function endCallClicked(){
        CallManager.hangUp();
    }

    // Timer {
    //     id: updateCallManagerTimer
    //     interval: 100; running: false; repeat: false
    //     onTriggered: onNewCallState()
    // }

    function listenToCallManager(){
        if(CallManager.callsSupported){
            CallManager.onNewCallState.connect(onNewCallState)
            onNewCallState()
        }
    }

    function onNewCallState(){
        if(CallManager.isOnCall){
            state = "oncall"
            inCalling = true
            if(CallManager.callType != CallType.VOICE){
                if(qmlLibRoot.embedVideoQML){
                    stack.push(videoItem);
                }
                QmlInterface.setVideoCallItem();
            }
        } else {
            state = "freecall"
            inCalling = false
            if(stack.currentItem == videoItem && qmlLibRoot.embedVideoQML)
                stack.pop()
        }
    }    

    function noneStateHandler(){
        setCallButtonsVisible(false)
        setEndCallButtonsVisible(false)
        callStatusbar.visible = false
    }

    function freeCallStateHandler(){
        if(enableCallButtons && CallManager.callsSupported) {
            setCallButtonsVisible(true)
            setEndCallButtonsVisible(false)
        }
        callStatusbar.visible = false
    }

    function onCallStateHandler(){
        if(enableCallButtons && CallManager.callsSupported) {
            setCallButtonsVisible(false)
            setEndCallButtonsVisible(true)
        }
        callStatusbar.visible = true
    }

    states: [
        State {
            name: "none"
            StateChangeScript {
                script: noneStateHandler()
            }
        },
        State {
            name: "freecall"
            StateChangeScript {
                script: freeCallStateHandler()
            }
        },
        State {
            name: "oncall"
            StateChangeScript {
                script: onCallStateHandler()
            }
        }
    ]

    function menuClickedCallback(){
        if(!navDrawer.opened)
            navDrawer.open()

        if(navDrawer.opened)
            navDrawer.close()
    }

    function onDevicesChanged(){
        if(state == "none")
            noneStateHandler()
        else if(state == "oncall")
            onCallStateHandler()
        else if(state == "freecall")
            freeCallStateHandler()
        if(CallManager.callsSupported){
            var mics = CallManager.mics
            var cams = CallManager.cameras
            if(mics.length && cams.length){
                callSettingsItem.icon.color="green"
            } else if (mics.length || cams.length){
                callSettingsItem.icon.color="red"
            } else {
                callSettingsItem.icon.color="gray"
            }
        }
    }

    Component.onCompleted: {
        menuClicked.connect(menuClickedCallback)
        listenToCallManager()
        CallManager.onDevicesChanged.connect(onDevicesChanged)
    }

    MainMenu{
        id: navDrawer
        y: mainHeader.height
        width: (parent.width < parent.height)?parent.width/2: parent.width/5
        height: parent.height - mainHeader.height
    }
}


