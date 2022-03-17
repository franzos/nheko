import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.5
import MatrixClient 1.0

Dialog {
    standardButtons: Dialog.Ok
    Label {
        id: lableMessage
        width: parent.width
        wrapMode: Text.Wrap
    }
    onAccepted: { }

    function loadMessage(dialogTitle,message){
        title = dialogTitle
        lableMessage.text = message
        open()
    }
}