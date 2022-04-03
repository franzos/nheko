import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

import MatrixClient 1.0

CustomPage {
    id: loginPage
    width: parent.width
    spacing: 10
    ColumnLayout{
        id: inputLayout
        anchors.centerIn: parent
        width: parent.width
        TextField {
            id: userIdText
            Layout.leftMargin: 50
            Layout.rightMargin: 50
            Layout.fillWidth: true
            placeholderText: qsTr("Email")
            Keys.onReturnPressed: loginButton.gotoLogin()
            Keys.onEnterPressed: loginButton.gotoLogin()
        }

        Row{
            anchors{
                horizontalCenter: parent.horizontalCenter
                //verticalCenter: parent.verticalCenter
            }
            width: parent.width
            spacing: 10
            Button {
                id: loginButton
                text: "Login"
                anchors.rightMargin: 2
                anchors.verticalCenter: parent.verticalCenter
                function gotoLogin(){
                    loginButton.enabled= false;
                    MatrixClient.loginWithCiba(String(userIdText.text),
                                            String("https://matrix.pantherx.dev"))
                }

                onClicked: gotoLogin()
            }

            Button {
                id: cancelButton
                text: "Cancel"
                anchors.leftMargin: 2
                anchors.right: Login.left
                //anchors.verticalCenter: parent.verticalCenter  
                onClicked: stack.pop()
            }

        }        
    }
    Connections {
        target: MatrixClient
        function onLoginErrorOccurred(msg) {
            loginButton.enabled = true
        }
    }

    function reload(){
        userIdText.text = ""
        loginButton.enabled = true
    }

    Component.onCompleted: {
        header.visible = false
    }
}
