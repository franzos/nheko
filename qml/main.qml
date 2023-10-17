import QtQuick 2.9
import QtQuick.Window 2.0
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

import MatrixClient 1.0
import GlobalObject 1.0

CustomApplicationWindow {
    id: qmlApplication
    title: qsTr("Matrix Client")
    visibility: "Maximized"
    visible: true
    property bool embedVideoQML
    property bool callAutoAccept
    property var hiddenFeatures: []
    property var visibleFeatures: []
    property var hiddenMenuEntries: []

    MainLib{
        id: mainLibqml
        embedVideoQML: qmlApplication.embedVideoQML
        callAutoAccept: qmlApplication.callAutoAccept
        hiddenFeatures: qmlApplication.hiddenFeatures
        visibleFeatures: qmlApplication.visibleFeatures
        hiddenMenuEntries: qmlApplication.hiddenMenuEntries
    }

    Component.onCompleted: {
        console.log("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
        console.log('hidden features: ', hiddenFeatures)
        console.log('visible features: ', visibleFeatures)
        console.log('hidden entries: ', hiddenMenuEntries)
        console.log("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
        MatrixClient.start()
    }

    onClosing: {
        if (GlobalObject.mobileMode()) {
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
