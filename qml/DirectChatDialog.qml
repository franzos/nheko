import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.5
import MatrixClient 1.0
import QmlInterface 1.0
import "regex"

Dialog {
    title: "Direct Chat"
    standardButtons: Dialog.Cancel | Dialog.Ok


    TextField {
        id: directTextField
        width:parent.width
        validator: UserIDRegex{}
        placeholderText: qsTr("User ID: " + QmlInterface.defaultUserIdFormat())
    }    
    onAccepted: {
        MatrixClient.startChat(directTextField.text)
        directTextField.text = ""
    }
    onRejected: {
        directTextField.text = ""
    }
}