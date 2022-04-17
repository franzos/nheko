import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import MatrixClient 1.0
import QmlInterface 1.0
import GlobalObject 1.0
import "regex"

Page {
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
            placeholderText: qsTr("ID")
            Keys.onReturnPressed: loginButton.gotoLogin()
            Keys.onEnterPressed: loginButton.gotoLogin()
        }

        TextField {
            id: matrixServerText
            // text: "https://matrix.pantherx.org"
            Layout.leftMargin: 50
            Layout.rightMargin: 50
            Layout.fillWidth: true
            placeholderText: "Matrix Server (e.g.: " + QmlInterface.defaultMatrixServer() + ")"
            text: QmlInterface.isSetServerAsDefault()?QmlInterface.defaultMatrixServer():""
            validator: MatrixServerRegex{}
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
                    matrixServerText.text = GlobalObject.checkMatrixServerUrl(matrixServerText.text)
                    MatrixClient.loginWithCiba(String(userIdText.text),
                                               String(matrixServerText.text))
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

}

