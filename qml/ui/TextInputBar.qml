import QtQuick 2.3
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import EmojiModel 1.0

RowLayout {
    id: passwordForm
    Layout.fillWidth: true
    property alias placeholderText: textInput.placeholderText
    property alias text: textInput.text
    
    EmojiModel {
        id: emojiModel
        iconsPath: 'qrc:/emoji/emojiSvgs/'
        iconsType: '.svg'
    }

    TextField {
        id: textInput
        Layout.fillWidth: true
        wrapMode: TextEdit.Wrap
        // textFormat: TextEdit.RichText
        anchors.fill: parent
    }

    
    Menu {
        id: contextMenu
        width: 400
        Rectangle {
            id: body
            width: parent.width
            height: 420
            radius: 10
            anchors.top: parent.top
            anchors.topMargin: 40
            anchors.horizontalCenter: parent.horizontalCenter
            EmojiPicker {
                id: emojiPicker
                model: emojiModel
                // editor: textInput
                anchors.fill: parent
            }
        }
    }

    ToolButton {
        id: button
        icon.source: "qrc:/images/smile.svg"
        onClicked: {contextMenu.popup()}
    }
}
