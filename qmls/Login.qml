import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import QtQml 2.15
import QtQml.Models 2.2
import QtQuick.Window 2.0
import QtQuick.Controls.Styles 1.1
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.0
import Client 1.0

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
            Layout.leftMargin: 50
            Layout.rightMargin: 50
            Layout.fillWidth: true
            placeholderText: qsTr("Password")
        }

        Button {
            text: "Login"
            Layout.alignment: Qt.AlignHCenter
            onClicked: Client.loginWithPassword(String("MATRIX_TEST_APP"),userIdText.text, passwordText.text, "https://matrix.pantherx.org")
        }
    }
}


