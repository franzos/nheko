import QtQuick 2.3
import QtQuick.Controls 2.3
import GlobalObject 1.0
import QtQuick.Layouts 1.15

ToolButton{
    property bool loadingState : false

    // BusyIndicator {
    //     id: busyIndicator
    //     running: loadingState
    //     width: 36; height: width
    //     palette.dark: GlobalObject.colors.windowText
    // }

    id: menuButton
    enabled: !loadingState
    // icon.source: "qrc:/images/empty.svg"
    onPressed: anim.start()
    background: Rectangle {
        implicitWidth: 100
        implicitHeight: 40
        color: "transparent"
        border.color: "#26282a"
        border.width: 1
        radius: 4
    }
    
    SequentialAnimation {
        id: anim
        // Expand the button
        PropertyAnimation {
            target: menuButton
            property: "scale"
            to: 1.2
            duration: 200
            easing.type: Easing.InOutQuad
        }

        // Shrink back to normal
        PropertyAnimation {
            target: menuButton
            property: "scale"
            to: 1.0
            duration: 200
            easing.type: Easing.InOutQuad
        }
    }
}