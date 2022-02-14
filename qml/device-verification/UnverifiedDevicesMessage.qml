import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.5
import SelfVerificationStatus 1.0

Dialog {
    title: "Verify"
    signal verifyClicked()
    anchors.centerIn: parent
    Label {
        width: parent.width
        text: "To allow other users to see, which of your devices actually belong to you, you can verify them. This also allows key backup to work automatically. Verify an unverified device now? (Please make sure you have one of those devices available.)"
        wrapMode: Text.Wrap
    }

    footer: DialogButtonBox{
        Button {
            text: qsTr("Cancel")
            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
        }
        Button {
            text: qsTr("Start verification")
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
            onClicked: verifyClicked()
        }
    }
}