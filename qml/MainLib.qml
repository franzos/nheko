import QtQuick 2.9
import QtQuick.Window 2.0
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

import MatrixClient 1.0

Item {
    id: qmlLibRoot
    anchors.fill:parent

    StackView {
        id: stack
        anchors.fill: parent
    }

    UIA{
    }

    BusyIndicator {
        id: busyIndicator
        width: 64; height: 64
        anchors.centerIn: parent
    }

    RoomList {
        id: roomList
        visible: false
    }

    Login {
        id: loginPage
        visible: false
    }

    Connections {        
        target: MatrixClient
        function onDropToLogin(msg) {
            stack.replace(loginPage)
        }

        function onLoginOk(user) {
            MatrixClient.start()
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
        MatrixClient.start()
    }
}
