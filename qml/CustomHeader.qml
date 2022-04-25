import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import MatrixClient 1.0
import CallManager 1.0
import CallType 1.0
import QmlInterface 1.0
import "voip"
import "device-verification"

Column {
    width: toolBar.width
    height: toolBar.height + 20
    signal titleClicked()
    signal menuClicked()
    signal voiceCallClicked()
    signal videoCallClicked()
    signal optionClicked()
    property bool enableCallButtons: false
    property bool inCalling: false

    ToolBar {
        id: toolBar
        width: parent.width
        RowLayout {
            anchors.fill: parent
            spacing: 2
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
                        var prevPage = stack.pop()
                        if (prevPage) {
                            prevPage.destroy()
                        }
                    }
                }
            }

            ToolButton {
                id: verifyRect
                icon.source: "qrc:/images/shield-filled-exclamation-mark.svg"
                icon.color:"#C70039"
                width: parent.height
                height: parent.height
                onClicked: {
                    selfVerificationCheck.verify()
                }
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
                onClicked: {optionClicked()}
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
        voiceCallButton.visible = visible;
        videoCallButton.visible = visible;
    }

    function setEndCallButtonsVisible(visible){
        endCallButton.visible = visible;
    }

    function setOptionButtonsVisible(visible){        
        optionsButton.visible = visible;
    }

    function setTitle(title){
        if(title)
            titleLabel.text = title
    }

    function title(){
        return titleLabel.text
    }
    
    function setVerified(flag){
        if(flag){
            verifyRect.visible = false
        } else {
            verifyRect.visible = true
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

    states: [
        State {
            name: "none"
            StateChangeScript {
                script: {
                    setCallButtonsVisible(false)
                    setEndCallButtonsVisible(false)
                    callStatusbar.visible = false
                }
            }
        },
        State {
            name: "freecall"
            StateChangeScript {
                script: {
                    if(enableCallButtons && CallManager.callsSupported) {
                        setCallButtonsVisible(true)
                        setEndCallButtonsVisible(false)
                    }
                    callStatusbar.visible = false
                }
            }
        },
        State {
            name: "oncall"
            StateChangeScript {
                script: {
                    if(enableCallButtons && CallManager.callsSupported) {
                        setCallButtonsVisible(false)
                        setEndCallButtonsVisible(true)
                    }
                    callStatusbar.visible = true
                }
            }
        }
    ]

    function menuClickedCallback(){
        if(!navDrawer.opened)
            navDrawer.open()

        if(navDrawer.opened)
            navDrawer.close()
    }

    Component.onCompleted: {
        menuClicked.connect(menuClickedCallback)
        listenToCallManager()
    }

    MainMenu{
        id: navDrawer
        y: mainHeader.height
        width: (parent.width < parent.height)?parent.width/2: parent.width/5
        height: parent.height - mainHeader.height
    }
}


