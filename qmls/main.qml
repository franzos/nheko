import QtQuick 2.9
import QtQuick.Window 2.0
import QtQuick.Controls 2.15
import QtQuick.Controls.Styles 1.1
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.15
//import QtQuick.Controls.Material 2.12

import MatrixClient 1.0
ApplicationWindow {
    title: qsTr("Matrix Client")
    width: 400
    height: 600
    visible: true
//    Material.theme: Material.Light
//    Material.accent: Material.Purple

    StackView {
        id: stack
        anchors.fill: parent
    }

    BusyIndicator {
        id: loginIndicator
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
            stack.replace(loginIndicator,loginPage)
        }

        function onLoginOk(user) {
            MatrixClient.start()
        }

        function onInitiateFinished(){
            stack.replace(loginIndicator, roomList)
        }
    }

    Component.onCompleted: {
        stack.push(loginIndicator)
        MatrixClient.start()
    }

    onClosing: {
        MatrixClient.stop()
    }
}

