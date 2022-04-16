import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import MatrixClient 1.0
import QmlInterface 1.0

Page {
    id: loginPage

    Validator{
        id: validator
    }

    ColumnLayout{
        id: inputLayout
        anchors.centerIn: parent
        width: parent.width
        TextField {
            id: userIdText
            Layout.leftMargin: 50
            Layout.rightMargin: 50
            Layout.fillWidth: true
            validator: validator.userIdRegex()
            // text: "@hamzeh_test05:pantherx.org"
            placeholderText: "User ID" + (QmlInterface.defaultUserIdFormat()?" (e.g.: " + QmlInterface.defaultUserIdFormat() + ")" : "")
            Keys.onReturnPressed: loginButton.gotoLogin()
            Keys.onEnterPressed: loginButton.gotoLogin()
        }

        TextField {
            id: passwordText
            echoMode: TextInput.Password
            // text: "riverbed-judiciary-sworn"
            Layout.leftMargin: 50
            Layout.rightMargin: 50
            Layout.fillWidth: true
            placeholderText: qsTr("Password")
            Keys.onReturnPressed: loginButton.gotoLogin()
            Keys.onEnterPressed: loginButton.gotoLogin()
        }

        TextField {
            id: matrixServerText
            // text: "https://matrix.pantherx.org"
            Layout.leftMargin: 50
            Layout.rightMargin: 50
            Layout.fillWidth: true
            validator: validator.matrixServerRegex()
            placeholderText: "Matrix Server (e.g.: " + QmlInterface.defaultMatrixServer() + ")"
            text: QmlInterface.isSetServerAsDefault()?QmlInterface.defaultMatrixServer():""
            Keys.onReturnPressed: loginButton.gotoLogin()
            Keys.onEnterPressed: loginButton.gotoLogin()
        }

        Button {
            id: loginButton
            text: "Login"
            Layout.alignment: Qt.AlignHCenter
            function gotoLogin(){
                loginButton.enabled= false;
                matrixServerText.text = validator.checkMatrixServerUrl(matrixServerText.text)
                MatrixClient.loginWithPassword(String("matrix_client_application"),
                                         String(userIdText.text),
                                         String(passwordText.text),
                                         String(matrixServerText.text))
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
}


