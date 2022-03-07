import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.5
import MatrixClient 1.0

Dialog {
    title: "Direct Chat"
    standardButtons: Dialog.Cancel | Dialog.Ok
    TextField {
        id: directTextField
        width:parent.width
        placeholderText: qsTr("Userid")
    }    
    onAccepted: {
        MatrixClient.startChat("@"+directTextField.text+":pantherx.org")
        directTextField.text = ""
    }
    onRejected: {
        directTextField.text = ""
    }
}