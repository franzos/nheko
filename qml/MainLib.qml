import QtQuick 2.9
import QtQuick.Window 2.0
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import CallManager 1.0
import MatrixClient 1.0
import "voip/"

Item {
    id: qmlLibRoot
    anchors.fill:parent

    StackView {
        id: stack
        anchors.fill: parent
    }

    UIA{
    }

    Component {
        id: mobileCallInviteDialog

        CallInvite {
        }

    }

    BusyIndicator {
        id: busyIndicator
        width: 64; height: width
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
        x: (qmlApplication.width - width) / 2
        y: (qmlApplication.height - height) / 2
    }

    function onNewInviteState() {
        if (CallManager.haveCallInvite) {
            console.log("New Call Invite!")
            var dialog = mobileCallInviteDialog.createObject(qmlLibRoot);
            dialog.open();
            destroyOnClose(dialog);
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
            stack.replace(roomList)
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
        MatrixClient.start()
    }
}
