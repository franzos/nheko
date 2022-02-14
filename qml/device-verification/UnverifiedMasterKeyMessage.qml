import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.5

Dialog {
    title: "Verify Master Key"
    signal verifyClicked()
    signal passphraseClicked()
    anchors.centerIn: parent
    Label {
        width: parent.width
        text: "It seems like you have encryption already configured for this account. To be able to access your encrypted messages and make this device appear as trusted, you can either verify an existing device or (if you have one) enter your recovery passphrase. Please select one of the options below.\nIf you choose verify, you need to have the other device available. If you choose \"enter passphrase\", you will need your recovery key or passphrase. If you click cancel, you can choose to verify yourself at a later point."
        wrapMode: Text.Wrap
    }
    footer: DialogButtonBox{
        Button {
            text: qsTr("Cancel")
            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
        }
        Button {
            text: qsTr("Verify")
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
            onClicked: verifyClicked()
        }
        Button {
            text: qsTr("Passphrase")
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
            onClicked: passphraseClicked()
        }
    }
}