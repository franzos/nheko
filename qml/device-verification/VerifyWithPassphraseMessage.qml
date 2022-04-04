import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.5
import SelfVerificationStatus 1.0

Dialog {
    title: "Verify with passphrase"
    standardButtons: Dialog.Cancel | Dialog.Ok
    signal passphraseEntered(string passphrase)
    anchors.centerIn: parent
    Column{
        width: parent.width
        Label {
            width: parent.width
            text: "Enter your recovery key"
            wrapMode: Text.Wrap
        }

        TextField {
            id: passphraseText
            Layout.leftMargin: 50
            Layout.rightMargin: 50
            Layout.fillWidth: true
            width: parent.width
            echoMode: TextInput.Password
            placeholderText: qsTr("Recovery Key")
            Keys.onReturnPressed: passphraseEntered(passphraseText.text)
            Keys.onEnterPressed: passphraseEntered(passphraseText.text)
        }
    }
    
    onAccepted: {
        passphraseEntered(passphraseText.text)
    }
}