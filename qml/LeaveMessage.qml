import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.5
import MatrixClient 1.0

Dialog {
    title: "Leave room"
    standardButtons: Dialog.Cancel | Dialog.Ok
    property var roomName : ""
    property var roomId : ""
    Column {
        width:parent.width
        spacing: 10
        Label {
            text: "Are you sure you want to leave " + roomName + " ?"
        }
        TextField {
            id: reasonTextField
            width:parent.width
            placeholderText: "Reason to leave ..."
        }
    }
    onAccepted: {
        MatrixClient.leaveRoom(roomId, reasonTextField.text)
    }
    onRejected: {}
}