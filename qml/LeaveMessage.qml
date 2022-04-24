import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.5
import MatrixClient 1.0

Dialog {
    title: "Leave room"
    standardButtons: Dialog.Cancel | Dialog.Ok
    property var roomName : ""
    property var roomId : ""
    Label {
        text: "Are you sure you want to leave " + roomName + " ?"
    }
    onAccepted: {
        MatrixClient.leaveRoom(roomId)
    }
    onRejected: {}
}