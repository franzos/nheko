import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.5
import MatrixClient 1.0
import QmlInterface 1.0

Dialog {
    title: "Invite User"
    standardButtons: Dialog.Cancel | Dialog.Ok
    TextField {
        id: inviteUserField
        width:parent.width
        placeholderText: qsTr("User ID: " + QmlInterface.defaultUserIdFormat())
    }    
    onAccepted: {
        MatrixClient.inviteUser(room.id,inviteUserField.text,"")
        inviteUserField.text = ""
    }
    onRejected: {
        inviteUserField.text = ""
    }
}