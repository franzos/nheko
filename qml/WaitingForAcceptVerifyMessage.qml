import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.5
import SelfVerificationStatus 1.0

Dialog {
    title: "Waiting"
    standardButtons: Dialog.Cancel
    width: parent.width
    Label {
        width: parent.width
        text: "Waiting for other side to accept the verification request."
        wrapMode: Text.Wrap
    }
}