import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

ApplicationWindow {
    id: app
    visible: true
    width: 400
    height: 600

    Material.theme: Material.Dark

    Label {
        text: qsTr("Hello Qt!")
        anchors.centerIn: parent
    }
}
