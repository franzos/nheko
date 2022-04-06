import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.5
import MatrixClient 1.0

Dialog {
    title: "Invite User"
    standardButtons: Dialog.Cancel | Dialog.Ok
    TextField {
        id: inviteUserField
        width:parent.width
        placeholderText: qsTr("Userid")
    }    
    onAccepted: {
        MatrixClient.inviteUser(room.id,"@"+inviteUserField.text+":pantherx.org","")
        inviteUserField.text = ""
    }
    onRejected: {
        inviteUserField.text = ""
    }
}