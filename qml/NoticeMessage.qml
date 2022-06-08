import QtQuick 2.5
import GlobalObject 1.0
import "ui"

TextMessage {
    property bool isStateEvent
    font.italic: true
    color: GlobalObject.colors.buttonText
    // font.pointSize: isStateEvent? 0.8*Settings.fontSize : Settings.fontSize
    horizontalAlignment: isStateEvent? Text.AlignHCenter : undefined
}
