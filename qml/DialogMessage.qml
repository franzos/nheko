import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.5

Dialog {
    title: "Leave room"
    standardButtons: Dialog.Cancel | Dialog.Ok
    Label {
        text: "Are you sure you want to leave " + room.name + " ?"
    }
    onAccepted: {
        MatrixClient.leaveRoom(room.id)
    }
    onRejected: {}
}
