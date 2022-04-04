import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.5
import QtQuick.Extras 1.4
import SelfVerificationStatus 1.0

Dialog {
    id: bootstrapCrosssigning
    width: parent.width
    onAccepted: SelfVerificationStatus.setupCrosssigning(storeSecretsOnline.checked, usePassword.checked ? passwordField.text : "", useOnlineKeyBackup.checked)

    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel
    closePolicy: Popup.NoAutoClose

    ScrollView {
        id: scroll
        clip: true
        anchors.fill: parent 
        ScrollBar.horizontal.visible: false
        ScrollBar.vertical.visible: true
        width: parent.width
        Column {
            width: scroll.width
            spacing :10

            Label {
                Layout.alignment: Qt.AlignHCenter
                Layout.columnSpan: 2
                text: qsTr("Setup Encryption")
                wrapMode: Text.Wrap
            }

            Label {
                Layout.alignment: Qt.AlignLeft
                Layout.columnSpan: 2
                width: parent.width
                text: qsTr("Hello and welcome to Matrix!\nIt seems like you are new. Before you can securely encrypt your messages, we need to setup a few small things. You can either press accept immediately or adjust a few basic options. We also try to explain a few of the basics. You can skip those parts, but they might prove to be helpful!")
                wrapMode: Text.Wrap
            }

            Row {
                width: parent.width
                spacing: 10
                ToggleButton {
                    id: storeSecretsOnline
                    height: 48
                    width: 48
                    checked: true
                    onClicked: console.log("Store secrets toggled: " + checked)
                }
                Label {
                    Layout.alignment: Qt.AlignLeft
                    Layout.columnSpan: 1
                    width: parent.width - storeSecretsOnline.width - parent.spacing
                    text: "Store secrets online.\nYou have a few secrets to make all the encryption magic work. While you can keep them stored only locally, we recommend storing them encrypted on the server. Otherwise it will be painful to recover them. Only disable this if you are paranoid and like losing your data!"
                    wrapMode: Text.Wrap
                }
            }
            
            Row {
                width: parent.width
                spacing: 10
                visible: storeSecretsOnline.checked
                ToggleButton {
                    id: usePassword
                    height: 48
                    width: 48
                    checked: false
                }
                Label {
                    Layout.alignment: Qt.AlignLeft
                    Layout.columnSpan: 1
                    Layout.rowSpan: 2
                    width: parent.width - usePassword.width - parent.spacing
                    text: "Set an online backup password.\nWe recommend you DON'T set a password and instead only rely on the recovery key. You will get a recovery key in any case when storing the cross-signing secrets online, but passwords are usually not very random, so they are easier to attack than a completely random recovery key. If you choose to use a password, DON'T make it the same as your login password, otherwise your server can read all your encrypted messages. (You don't want that.)"
                    wrapMode: Text.Wrap
                }
            }

            TextField {
                id: passwordField

                Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                Layout.columnSpan: 1
                Layout.fillWidth: true
                width: parent.width
                placeholderText: "Online backup password"
                visible: storeSecretsOnline.checked && usePassword.checked
                echoMode: TextInput.Password
            }

            Row {
                width: parent.width
                spacing: 10
                ToggleButton {
                    id: useOnlineKeyBackup
                    height: 48
                    width: 48
                    checked: true
                    onClicked: console.log("Online key backup toggled: " + checked)
                }
                
                Label {
                    Layout.alignment: Qt.AlignLeft
                    Layout.columnSpan: 1
                    width: parent.width - useOnlineKeyBackup.width - parent.spacing
                    text: "Use online key backup.\nStore the keys for your messages securely encrypted online. In general you do want this, because it protects your messages from becoming unreadable, if you log out by accident. It does however carry a small security risk, if you ever share your recovery key by accident. Currently this also has some other weaknesses, that might allow the server to insert new keys into your backup. The server will however never be able to read your messages."
                    wrapMode: Text.Wrap
                }
            }
        }
    }
}