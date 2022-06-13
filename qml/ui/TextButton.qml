import QtQuick 2.15
import QtQuick.Controls 2.15
import GlobalObject 1.0
import CursorShape 1.0

AbstractButton {
    id: button

    property alias cursor: mouseArea.cursorShape
    property color highlightColor: GlobalObject.colors.highlight
    property color buttonTextColor: GlobalObject.colors.buttonText

    focusPolicy: Qt.NoFocus
    width: buttonText.implicitWidth
    height: buttonText.implicitHeight
    implicitWidth: buttonText.implicitWidth
    implicitHeight: buttonText.implicitHeight

    Label {
        id: buttonText

        anchors.centerIn: parent
        padding: 0
        text: button.text
        color: button.hovered ? highlightColor : buttonTextColor
        font: button.font
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }

    CursorShape {
        id: mouseArea

        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
    }

    Ripple {
        color: Qt.rgba(buttonTextColor.r, buttonTextColor.g, buttonTextColor.b, 0.5)
    }

}
