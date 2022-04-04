import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

import MatrixClient 1.0

CustomPage {
    id: loginPage
    width: parent.width
    ColumnLayout{
        id: inputLayout
        anchors.centerIn: parent
        width: parent.width
        TextField {
            id: userIdText
            Layout.leftMargin: 50
            Layout.rightMargin: 50
            Layout.fillWidth: true
            // text: "hamzeh_test04"
            placeholderText: qsTr("User ID")
            Keys.onReturnPressed: loginButton.gotoLogin()
            Keys.onEnterPressed: loginButton.gotoLogin()
        }

        TextField {
            id: passwordText
            echoMode: TextInput.Password
            // text: "tingly-headdress-earthlike"
            Layout.leftMargin: 50
            Layout.rightMargin: 50
            Layout.fillWidth: true
            placeholderText: qsTr("Password")
            Keys.onReturnPressed: loginButton.gotoLogin()
            Keys.onEnterPressed: loginButton.gotoLogin()
        }

        Button {
            id: loginButton
            text: "Login"
            Layout.alignment: Qt.AlignHCenter
            function gotoLogin(){
                loginButton.enabled= false;
                MatrixClient.loginWithPassword(String("matrix_client_application"),
                                         String("@" + userIdText.text + ":pantherx.org"),
                                         String(passwordText.text),
                                         String("https://matrix.pantherx.org"))
            }

            onClicked: gotoLogin()
        }

        Component {
            id: cibaLoginFactory
            CibaLogin {}
        }

        
        Button {
            id: cibaLoginButton
            text: "BC-Login"
            Layout.alignment: Qt.AlignHCenter 
            function showCibaLogin(){
                var cibaLogin = cibaLoginFactory.createObject(stack, {});
                stack.push(cibaLogin)
            }          
            onClicked: showCibaLogin()
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
        passwordText.text = ""
        loginButton.enabled = true
    }

    Component.onCompleted: {
        header.visible = false
    }
}


