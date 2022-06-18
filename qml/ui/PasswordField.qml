import QtQuick 2.3
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0

RowLayout {
    id: passwordForm
    Layout.fillWidth: true
    property alias placeholderText: passwordTextField.placeholderText
    property alias text: passwordTextField.text
    property alias echoMode: passwordTextField.echoMode
    
    TextField {
        id: passwordTextField
        Layout.fillWidth: true
        anchors.fill: parent
    }

    ToolButton {
        id: button
        icon.source: (passwordTextField.echoMode==TextInput.Password?"qrc:/images/hint.svg":"qrc:/images/visibility.svg")
        onClicked: {
            if(passwordTextField.echoMode==TextInput.Password){
                passwordTextField.echoMode = TextInput.Normal
            } else {
                passwordTextField.echoMode=TextInput.Password
            }
        }
    }
}
