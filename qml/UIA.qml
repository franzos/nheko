import QtQuick 2.9
import QtQuick.Window 2.0
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

import UIA 1.0

Item {
    visible: false
    width: qmlLibRoot.width
    height: qmlLibRoot.height

    Dialog {
        id: uiaPassPrompt
        title: UIA.title
        anchors.centerIn: parent
        closePolicy: Popup.NoAutoClose
        width: parent.width
        standardButtons: Dialog.Ok | Dialog.Cancel
        Column {
            width: parent.width
            spacing: 0
            Label {
                width: parent.width
                wrapMode: Text.Wrap
                text: qsTr("Please enter your login password to continue:")
            }

            TextField {
                id: passwordText
                echoMode: TextInput.Password
                width: parent.width
                placeholderText: qsTr("Password")
            }
        }
        onAccepted: UIA.continuePassword(passwordText.text)
    }

    Dialog {
        id: uiaEmailPrompt
        title: UIA.title
        anchors.centerIn: parent
        closePolicy: Popup.NoAutoClose
        width: parent.width
        standardButtons: Dialog.Ok | Dialog.Cancel
        Column {
            width: parent.width
            spacing: 0
            Label {
                width: parent.width
                wrapMode: Text.Wrap
                text: qsTr("Please enter a valid email address to continue:")
            }
            TextField {
                id: emailText
                width: parent.width
                placeholderText: qsTr("Email")
            }
        }
        onAccepted: UIA.continueEmail(emailText.text)
    }

    Dialog {
        id: uiaPhoneNumberPrompt
        anchors.centerIn: parent
        closePolicy: Popup.NoAutoClose
        width: parent.width
        title: UIA.title
        standardButtons: Dialog.Ok | Dialog.Cancel
        Column {
            width: parent.width
            spacing: 5
            Label {
                width: parent.width
                wrapMode: Text.Wrap
                text: qsTr("Please enter a valid phone number to continue:")
            }
            TextField {
                id: countryText
                width: parent.width
                placeholderText: qsTr("Country Code")
            }
            TextField {
                id: phoneText
                width: parent.width
                placeholderText: qsTr("Phone")
            }
        }
        onAccepted:  UIA.continuePhoneNumber(countryText.text, phoneText.text)
    }

    Dialog {
        id: uiaTokenPrompt
        anchors.centerIn: parent
        closePolicy: Popup.NoAutoClose
        width: parent.width
        title: UIA.title
        standardButtons: Dialog.Ok | Dialog.Cancel
        Column {
            width: parent.width
            spacing: 0
            Label {
                width: parent.width
                wrapMode: Text.Wrap
                text: qsTr("Please enter the token, which has been sent to you:")
            }
            TextField {
                id: tokenText
                width: parent.width
                echoMode: TextInput.Password
                placeholderText: qsTr("Token")
            }
        }
        onAccepted: UIA.submit3pidToken(tokenText.text)
    }

    Dialog {
        id: uiaErrorDialog
        standardButtons: Dialog.Ok
        title: "Error"
        anchors.centerIn: parent
        closePolicy: Popup.NoAutoClose
        width: parent.width
        Label {
            width: parent.width
            wrapMode: Text.Wrap
            id: errorLabel
        }
        function setError(m){
            errorLabel.text = m
        }
    }

    Dialog {
        id: uiaConfirmationLinkDialog
        anchors.centerIn: parent
        closePolicy: Popup.NoAutoClose
        width: parent.width
        standardButtons: Dialog.Ok
        Label {
            width: parent.width
            wrapMode: Text.Wrap
            text: qsTr("Wait for the confirmation link to arrive, then continue.")
        }
        onAccepted: UIA.continue3pidReceived()
    }

    function onPassword() {
        console.log("UIA: password needed");
        uiaPassPrompt.open();
    }

    function onEmail() {
        uiaEmailPrompt.open();
    }

    function onPhoneNumber() {
        uiaPhoneNumberPrompt.open();
    }

    function onPrompt3pidToken() {
        uiaTokenPrompt.open();
    }

    function onConfirm3pidToken() {
        uiaConfirmationLinkDialog.open();
    }

    function onError(msg) {
        uiaErrorDialog.text = msg;
        uiaErrorDialog.open();
    }
    
    Component.onCompleted: {
        UIA.password.connect(onPassword)
        UIA.email.connect(onEmail)
        UIA.phoneNumber.connect(onPhoneNumber)
        UIA.prompt3pidToken.connect(onPrompt3pidToken)
        UIA.confirm3pidToken.connect(onConfirm3pidToken)
        UIA.error.connect(onError)
        console.log("UIA: signal listener setup successfully.")
    }
}
