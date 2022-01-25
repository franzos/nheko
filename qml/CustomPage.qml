import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import MatrixClient 1.0

Page {
    id: customPage
    width: parent.width
    header: Rectangle {
        width: parent.width
        height: 30
        Row {
            anchors.fill: parent
            Button {
                id: backButton
                text: "<"
                width: 24
                height: parent.height
                enabled: !stack.empty
                onClicked: stack.pop()
            }

            Button {
                id: titleLabel
                width: parent.width - backButton.width - logoutButton.width - 2
                height: parent.height
                anchors.leftMargin: 2
                // anchors.centerIn: parent
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: backButton.right
                // onClicked: {}
            }

            Button {
                id: logoutButton
                text: "Logout"
                anchors.leftMargin: 2
                height: parent.height
                anchors.left: titleLabel.right
                onClicked: logoutDialog.open()
            }

            Dialog {
                id: logoutDialog
                x: (qmlApplication.width - width) / 2
                y: (qmlApplication.height - height) / 2
                title: "Logout"
                standardButtons: Dialog.Cancel | Dialog.Ok
                Label {
                    text: "Are you sure you want to logout ?"
                }
                onAccepted: {
                    MatrixClient.logout()
                }
                onRejected: {}
            }
        }
    }

    function setTitle(title){
        titleLabel.text = title
        backButton.enabled= !stack.empty
    }
}


