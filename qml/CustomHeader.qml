import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import MatrixClient 1.0

ToolBar {
    width: parent.width

    signal titleClicked()
    signal menuClicked()
    signal verifyClicked()
    signal voiceCallClicked()
    signal videoCallClicked()
    signal endCallClicked()
    signal optionClicked()
    RowLayout {
        anchors.fill: parent
        spacing: 2
        ToolButton{
            id: menuButton
            icon.source: "qrc:/images/drawer.png"
            width: parent.height
            height: parent.height
            enabled: !stack.empty
            visible: false
            onClicked: {
                menuClicked()
            }
        }
        ToolButton {
            id: backButton
            icon.source: "qrc:/images/angle-arrow-left.svg"
            width: parent.height
            height: parent.height
            enabled: !stack.empty
            onClicked: stack.pop()
        }

        ToolButton {
            id: verifyRect
            icon.source: "qrc:/images/shield-filled-exclamation-mark.svg"
            width: parent.height
            height: parent.height
            enabled: !stack.empty
            onClicked: {verifyClicked()}
        }
        Item{
            Layout.fillWidth: true
            height: parent.height            
            Label {
                id: titleLabel
                width: parent.width 
                height: parent.height
                anchors.leftMargin: 2
                verticalAlignment:Text.AlignVCenter
            }

            MouseArea {
                id: ma
                anchors.fill: parent
                onClicked: {
                    titleClicked()                    
                }
            }
        }
       
       
        ToolButton {
            id: voiceCallButton
            icon.source: "qrc:/images/place-call.svg"
            width: parent.height
            height: parent.height
            visible: false
            onClicked: {voiceCallClicked()}
        }

        ToolButton {
            id: videoCallButton
            icon.source: "qrc:/images/video.svg"
            width: parent.height
            height: parent.height
            visible: false
            onClicked: {videoCallClicked()}
        } 

        ToolButton {
            id: endCallButton
            icon.source: "qrc:/images/end-call.svg"
            width: parent.height
            height: parent.height
            visible: false
            onClicked: {endCallClicked()}
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

    function setHomeButtonsVisible(visible){
        menuButton.visible = visible;
        backButton.visible = !visible;
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

    function setBackButtonsVisible(visible){        
        backButton.visible = visible
    }

    function setTitle(title){
        titleLabel.text = title
        backButton.enabled= !stack.empty
    }

    function setVerified(flag){
        if(flag){
            verifyRect.visible = false
        } else {
            verifyRect.visible = true
        }
    }
}


