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
import "ui/"
import "ui/dialogs/"


Page {
    id: qmlLibRoot
    anchors.fill:parent
    property bool embedVideoQML: false
    property bool callAutoAccept: false
    property var videoItem
    
    StackView {
        id: stack
        anchors.fill: parent
        onCurrentItemChanged:{
            // TODO these params should be retireved from a general Page class and load to the Header
            mainHeader.state = "none"
            if(currentItem instanceof Timeline || currentItem == videoItem) {
                mainHeader.onNewCallState() 
            }
            
            if(currentItem instanceof RoomList){
                mainHeader.setRoomInfo(currentItem.title, "", currentItem.avatar)
            } else if(currentItem instanceof Timeline){
                mainHeader.setRoomInfo(currentItem.title, currentItem.roomid, currentItem.avatar)
            }

            if(currentItem instanceof Timeline){
                mainHeader.setOptionButtonsVisible(true)
            } else {
                mainHeader.setOptionButtonsVisible(false)
            }
            
            if(currentItem instanceof Login){
                mainHeader.visible= false
            } else {
                mainHeader.visible= true
            }
        }
    }

    function stackDepth() {
        return stack.depth
    }

    function backPressed() {
        if (stack.depth > 1) {
            var prevPage = stack.pop()
            if (prevPage) {
                prevPage.destroy()
            }
        }
    }

    Snackbar {
        id: snackbar 
    }

    FontMetrics {
        id: fontMetrics
    }

    UIA{
    }

    header: CustomHeader {
        id: mainHeader
        enableCallButtons: !callAutoAccept
        state: "none"
    } 

    Component {
        id: mobileCallInviteDialog
        CallInvite {
        }
    }

    Component {
        id: readReceiptsDialog

        ReadReceipts {
        }
    }

    Component {
        id: attachmentTypeDialog
        AttachmentTypeDialog {
            x: (qmlLibRoot.width - width) / 2
            y: (qmlLibRoot.height - height) / 2
        }
    }

    Component {
        id: locationPickerDialog
        LocationPicker {
        }
    }

    Component {
        id: rawMessageDialog

        RawMessageDialog {
        }
    }

    Component {
        id: roomMembersComponent

        RoomMembers {
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

    Timer {
        id: callAcceptTimer
        interval: 1000
        onTriggered: showCallInviteDialog()
    }

    function showCallInviteDialog() {
        if (CallManager.haveCallInvite) {
            console.log("New Call Invitation received.")
            var dialog = mobileCallInviteDialog.createObject(qmlLibRoot);
            dialog.open();
            destroyOnClose(dialog);
            if(callAutoAccept){
                console.log("Call-Auto Accept => answer")
                dialog.acceptCall()
            }
        }
    }

    function onNewInviteState() {
        if(callAutoAccept){
            callAcceptTimer.restart()
        } else {
            showCallInviteDialog()
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
            stack.pop(null)
            if(callAutoAccept){
                console.log("Running GUI application in Single Video Screen/Auto Call accept mode");
                stack.replace(videoItem)
            } else {
                stack.replace(roomList)
            }
        }

        function onLogoutErrorOccurred(){
            stack.pop()
        }

        function onLogoutOk(){
            for(var i=0; i<stack.depth-1; i++){
                var p  = stack.pop()
                if (p) {
                    p.destroy()
                }
            }
            loginPage.reload()
            stack.replace(loginPage)
        }

        function onShowNotification(msg) {
            snackbar.showNotification(msg)
        }

        function onUserInfoLoadingFailed(msg){
            snackbar.showNotification(msg)
        }
    }
    
    Component.onCompleted: {
        stack.push(busyIndicator)
        if(CallManager.callsSupported){            
            videoItem = Qt.createQmlObject('import QtQuick 2.15; import QtQuick.Layouts 1.3; import QtQuick.Controls 2.15; import "voip/"; Page {Layout.fillWidth: true; title: "Video Call"; VideoCallEmbedPage{}}',
                                   qmlLibRoot,
                                   "dynamicSnippet");
            CallManager.onNewInviteState.connect(onNewInviteState)
        }
    }
}
