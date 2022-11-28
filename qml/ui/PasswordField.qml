import QtQuick 2.3
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0

RowLayout {
    id: passwordForm
    Layout.fillWidth: true
    property alias placeholderText: passwordTextField.placeholderText
    property alias text: passwordTextField.text
    
    // There is a bug in iOS when the Password mode is enabled, so as a quick/temporary workaround
    // this property won't be change for iOS
    // https://git.pantherx.org/development/mobile/matrix-client/-/issues/162
    TextField {
        id: passwordTextField
        Layout.fillWidth: true
        echoMode:   {
                        if(Qt.platform.os != "ios") 
                            return TextField.Password
                    }
    }

    ToolButton {
        id: button
        enabled: Qt.platform.os != "ios"
        icon.source: (passwordTextField.echoMode==TextField.Password?"qrc:/images/hint.svg":"qrc:/images/visibility.svg")
        onClicked: {
            if(passwordTextField.echoMode==TextField.Password){
                passwordTextField.echoMode = TextField.Normal
            } else {
                passwordTextField.echoMode=TextField.Password
            }
        }
    }
}
