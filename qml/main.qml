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
    Material.theme: Material.Dark

    MainLib{
        embedVideoQML: true
    }

    onClosing: {
        MatrixClient.stop()
    }
}
