import QtQml.Models 2.2
import QtQuick 2.9
import QtQuick.Window 2.0
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.15
//import QtQuick.Controls.Material 2.12
import Client 1.0

ApplicationWindow {
    title: qsTr("Matrix Client")
    width: 400
    height: 600
    visible: true
//    Material.theme: Material.Light
//    Material.accent: Material.Purple

    BusyIndicator {
        id: loginIndicator
        width: 64; height: 64
        Layout.alignment: Qt.AlignCenter
    }

    StackView {
        id: stack
        anchors.fill: parent
    }

////    RoomList {
////        id: roomList
////        visible: false
////    }

    Login {
        id: loginPage
        visible: false
    }

    Connections {
        target: Client
        function onDropToLogin(msg) {
            console.log("***",msg)
            stack.push(loginPage)
//            stack.replace(loginIndicator,loginPage)
        }

        function onLoginOk(user) {
            console.log("LOGIN DONE")
            stack.replace(loginPage, roomList)
        }
    }

    Component.onCompleted: {
        stack.push(loginIndicator)
        Client.start()
    }
}

