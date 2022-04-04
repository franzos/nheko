import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import MatrixClient 1.0

Rectangle {
    width: parent.width
    height: 30

    signal titleClicked()
    Row {
        anchors.fill: parent
        spacing: 2
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
            onClicked: {titleClicked()}
        }

        Rectangle {
            id: verifyRect
            height: parent.height - 5
            width: height 
            radius: width/2
            color: "#ffaf49"
            anchors.right: logoutButton.left
            anchors.verticalCenter: parent.verticalCenter
            visible: false
            Label {
                anchors.centerIn: parent
                color: "white"
                font.pointSize: 10
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                text: "!"
            }
        }

        Button {
            id: logoutButton
            text: "Logout"
            anchors.leftMargin: 2
            height: parent.height
            onClicked: logoutDialog.open()
        }

        Dialog {
            id: logoutDialog
            x: (qmlLibRoot.width - width) / 2
            y: (qmlLibRoot.height - height) / 2
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

    function setTitle(title){
        titleLabel.text = title
        backButton.enabled= !stack.empty
    }

    function setVerified(flag){
        if(flag){
            verifyRect.visible = false
        } else {
            verifyRect.visible = true
        }
    }
}


