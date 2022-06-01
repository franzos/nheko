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
                if(!QmlInterface.getServerAddress())                     
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
            visible: !QmlInterface.getServerAddress()
            text: QmlInterface.getServerAddress()
            Layout.leftMargin: 50
            Layout.rightMargin: 50
            Layout.fillWidth: true
            validator: MatrixServerRegex{}
            placeholderText: "Matrix Server (e.g.: https://matri.pantherx.org)"
            Keys.onReturnPressed: loginButton.gotoLogin()
            Keys.onEnterPressed: loginButton.gotoLogin()
             onTextChanged: {                 
                var options = MatrixClient.loginOptions(matrixServerText.text)
                combo.restart()
                for (var prop in options) {
                    console.log("Object item:", prop, "=", options[prop])
                    combo.model.append ({ text: prop })
                }

            }
        }
        
        ComboBox {
            id: combo
            editable: false
            flat: true     
            displayText:"Select Login Option"  
            
            Layout.leftMargin: 50
            Layout.rightMargin: 50
            background:Rectangle {
                implicitWidth: 100
                implicitHeight: 40
                color: GlobalObject.colors.window
                border.color: GlobalObject.colors.windowText
            } 
            Layout.fillWidth: true
            model: ListModel {
                id: model               
            }
            //  Component.onCompleted:  displayText  = "Select Login Option"    
            onActivated: {
                displayText = combo.text
                if(currentText == "PASSWORD"){
                    passwordText.visible = true
                    loginButton.enabled= true
                }
                else{
                    passwordText.visible = false
                    loginButton.enabled= true
                }

            }
            function restart(){
                combo.model.clear()
                combo.displayText  = "Select Login Option"
                passwordText.visible = false 
                loginButton.enabled= false
            }
        }

        Button {
            id: loginButton
            text: "Login"
            Layout.alignment: Qt.AlignHCenter
            enabled: false
            function gotoLogin(){
                loginButton.enabled= false
                matrixServerText.text = GlobalObject.checkMatrixServerUrl(matrixServerText.text)
                MatrixClient.loginWithPassword(String("matrix_client_application"),
                                         String(userIdText.text),
                                         String(passwordText.text),
                                         String(matrixServerText.text))
            }
            function gotoCibaLogin(){
                    loginButton.enabled= false;
                    matrixServerText.text = GlobalObject.checkMatrixServerUrl(matrixServerText.text)
                    MatrixClient.loginWithCiba(String(userIdText.text),
                                               String(matrixServerText.text))
                }
            onClicked: {
                if(combo.currentText == "PASSWORD")
                    gotoLogin()
                else if (combo.currentText == "CIBA")
                    gotoCibaLogin()
            }
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


