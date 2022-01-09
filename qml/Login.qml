import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

import MatrixClient 1.0

Page {
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
            placeholderText: qsTr("User ID")
        }

        TextField {
            id: passwordText
            echoMode: TextInput.Password
            Layout.leftMargin: 50
            Layout.rightMargin: 50
            Layout.fillWidth: true
            placeholderText: qsTr("Password")
        }

        Button {
            id: loginButton
            text: "Login"
            Layout.alignment: Qt.AlignHCenter
            onClicked: {
                loginButton.enabled= false;
                MatrixClient.loginWithPassword(String("matrix_client_application"),
                                         String("@" + userIdText.text + ":pantherx.org"),
                                         String(passwordText.text),
                                         String("https://matrix.pantherx.org"))
            }
        }
    }
    Connections {
        target: MatrixClient
        function onLoginErrorOccurred(msg) {
            loginButton.enabled= true
        }
    }
}


