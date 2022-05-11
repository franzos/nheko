import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import MatrixClient 1.0
import QmlInterface 1.0
import GlobalObject 1.0
import "regex"

Page {
    id: loginPage

    ColumnLayout{
        id: inputLayout
        anchors.centerIn: parent
        width: parent.width
        TextField {
            id: userIdText
            Layout.leftMargin: 50
            Layout.rightMargin: 50
            Layout.fillWidth: true
            // validator: UserIDRegex{}
            placeholderText: "User ID or CM account" + (QmlInterface.defaultUserIdFormat()?" (e.g.: " + QmlInterface.defaultUserIdFormat() + ")" : "")
            Keys.onReturnPressed: loginButton.gotoLogin()
            Keys.onEnterPressed: loginButton.gotoLogin()
             onTextChanged: {                 
                    MatrixClient.serverDiscovery(MatrixClient.extractHostName(userIdText.text))
                }
        }

        TextField {
            id: passwordText
            echoMode: TextInput.Password
            visible: false
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
            validator: MatrixServerRegex{}
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
                matrixServerText.text = GlobalObject.checkMatrixServerUrl(matrixServerText.text)
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

    Connections {
        target: MatrixClient
        function onServerChanged(server) {
           matrixServerText.text = server
        }
    }

    Connections {
        target: MatrixClient
        function onDiscoverryErrorOccurred(err) {
          
        }
    }

    function reload(){
        userIdText.text = ""
        passwordText.text = ""
        loginButton.enabled = true
    }
}


