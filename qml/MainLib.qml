import QtQuick 2.9
import QtQuick.Window 2.0
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import CallManager 1.0
import MatrixClient 1.0
import CallType 1.0
import QmlInterface 1.0
import GlobalObject 1.0
import "voip/"

Item {
    id: qmlLibRoot
    anchors.fill:parent
    property bool embedVideoQML: false
    property bool singleVideoQML: false
    StackView {
        id: stack
        anchors.fill: parent
    }
    FontMetrics {
        id: fontMetrics
    }
    UIA{
    }
    
    VideoCall {
        id: videoItem
        visible: false
    }

    Component {
        id: mobileCallInviteDialog
        CallInvite {
        }
    }

    BusyIndicator {
        id: busyIndicator
        width: 64; height: width
        palette.dark: GlobalObject.colors.windowText
        x: (qmlLibRoot.width - width) / 2
        y: (qmlLibRoot.height - height) / 2
    }

    RoomList {
        id: roomList
        visible: false
    }

    Login {
        id: loginPage
        visible: false
    }

    ErrorDialog{
        id:errorPage
        x: (qmlLibRoot.width - width) / 2
        y: (qmlLibRoot.height - height) / 2
    }
    
    function destroyOnClose(obj) {
        if (obj.closing != undefined) obj.closing.connect(() => obj.destroy(1000));
        else if (obj.aboutToHide != undefined) obj.aboutToHide.connect(() => obj.destroy(1000));
    }

    function onNewInviteState() {
        if (CallManager.haveCallInvite) {
            console.log("New Call Invite!")
            var dialog = mobileCallInviteDialog.createObject(qmlLibRoot);
            dialog.open();
            destroyOnClose(dialog);

            if(singleVideoQML){
                dialog.acceptCall()
            }
        }
    }

    function onNewCallState(){
        if(CallManager.isOnCall && CallManager.callType != CallType.VOICE){
            if(embedVideoQML){
                videoItem.header.setBackButtonsVisible(false)
                stack.push(videoItem);
            }
            QmlInterface.setVideoCallItem();
        } else if (!CallManager.isOnCall) {
            if(stack.currentItem == videoItem && embedVideoQML)
                stack.pop()
        }
    }

    Connections {        
        target: MatrixClient
        function onDropToLogin(msg) {
            stack.replace(loginPage)
        }

        function onLoginOk(user) {
            MatrixClient.start()
        }

        function onLoginErrorOccurred(msg) {
            errorPage.loadMessage("Login Error",msg)
        }

        function onInitiateFinished(){
            if(singleVideoQML){
                console.log("Running GUI application in Single Video Screen/Auto Call accept mode");
                stack.replace(videoItem)
                videoItem.state="none"
                videoItem.header.setBackButtonsVisible(false)
            } else {
                stack.replace(roomList)
            }
        }

        function onLogoutErrorOccurred(){
            stack.pop()
        }

        function onLogoutOk(){
            stack.pop(null)
            loginPage.reload()
            stack.replace(loginPage)
        }
    }
    
    Component.onCompleted: {
        stack.push(busyIndicator)
        CallManager.onNewInviteState.connect(onNewInviteState)
        CallManager.onNewCallState.connect(onNewCallState)
        MatrixClient.start()
    }
}
