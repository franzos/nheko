import QtQuick 2.3
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.10
import DeviceVerificationFlow 1.0

Pane {
    property string title: qsTr("Verification failed")
    Column {
        spacing: 16
        anchors.fill: parent
        Text {
            id: content
            width: parent.width
            Layout.fillHeight: true
            Layout.fillWidth: true
            wrapMode: Text.Wrap
            text: {
                switch (flow.error) {
                case DeviceVerificationFlow.UnknownMethod:
                    return qsTr("Other client does not support our verification protocol.");
                case DeviceVerificationFlow.MismatchedCommitment:
                case DeviceVerificationFlow.MismatchedSAS:
                case DeviceVerificationFlow.KeyMismatch:
                    return qsTr("Key mismatch detected!");
                case DeviceVerificationFlow.Timeout:
                    return qsTr("Device verification timed out.");
                case DeviceVerificationFlow.User:
                    return qsTr("Other party canceled the verification.");
                case DeviceVerificationFlow.OutOfOrder:
                    return qsTr("Verification messages received out of order!");
                default:
                    return qsTr("Unknown verification error.");
                }
            }
            verticalAlignment: Text.AlignVCenter
        }

        RowLayout {
            Item {
                Layout.fillWidth: true
            }

            Button {
                Layout.alignment: Qt.AlignRight
                text: qsTr("Close")
                onClicked: dialog.close()
            }

        }

    }

}
