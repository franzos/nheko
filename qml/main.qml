import QtQuick 2.9
import QtQuick.Window 2.0
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

import MatrixClient 1.0

ApplicationWindow {
    id: qmlApplication
    title: qsTr("Matrix Client")
    visibility: "Maximized"
    visible: true
    property bool embedVideoQML
    property bool callAutoAccept

    MainLib{
        id: mainLibqml
        embedVideoQML: qmlApplication.embedVideoQML
        callAutoAccept: qmlApplication.callAutoAccept
    }

    Component.onCompleted: {
        MatrixClient.start()
    }

    onClosing: {
        if (Qt.platform.os === 'android') {
            if (mainLibqml.stackDepth() > 1) {
                close.accepted = false
                mainLibqml.backPressed()
                return
            }
        }
        MatrixClient.stop()
        close.accepted = true
    }
}
