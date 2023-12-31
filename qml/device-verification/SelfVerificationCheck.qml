import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.5

import SelfVerificationStatus 1.0
import VerificationManager 1.0
import GlobalObject 1.0
Item {
    id: selfVerificationCheck
    visible: false
    width: qmlLibRoot.width
    height: qmlLibRoot.height

    signal statusChanged()

    Dialog {
        id: showRecoverKeyDialog
        title: "Your Recovery Key"

        property string recoveryKey: ""

        parent: Overlay.overlay
        anchors.centerIn: parent
        width: parent.width
        standardButtons: Dialog.Ok
        closePolicy: Popup.NoAutoClose

        Column {
            id: content
            width: parent.width
            spacing: 0

            Label {
                Layout.fillWidth: true
                width: parent.width
                text: qsTr("This is your recovery key. You will need it to restore access to your encrypted messages and verification keys. Keep this safe. Don't share it with anyone and don't lose it! Do not pass go! Do not collect $200!")
                wrapMode: Text.Wrap
            }

            TextEdit {
                Layout.alignment: Qt.AlignHCenter
                horizontalAlignment: TextEdit.AlignHCenter
                verticalAlignment: TextEdit.AlignVCenter
                width: parent.width
                readOnly: true
                selectByMouse: true
                text: showRecoverKeyDialog.recoveryKey
                font.bold: true
                wrapMode: TextEdit.Wrap
            }
        }
    }

    Dialog {
        id: successDialog
        anchors.centerIn: parent
        title: "Done"
        standardButtons: Dialog.Ok
        Label {
            text: qsTr("Encryption setup successfully")
        }
    }


    Dialog {
        id: failureDialog
        anchors.centerIn: parent
        title: "Failed"
        standardButtons: Dialog.Ok
        width: parent.width

        Label {
            id: errorLabel
            width: parent.width
            wrapMode: Text.Wrap
        }        

        function setError(errorMessage){
            errorLabel.text = errorMessage
        }
    }

    UnverifiedMasterKeyMessage {
        id: unverifiedMasterKeyMessage
        width: parent.width
        onVerifyClicked:{
            SelfVerificationStatus.verifyMasterKey()
        }
        onPassphraseClicked:{
            verifyWithPassphraseMessage.open()
        }
    }

    VerifyWithPassphraseMessage {
        id: verifyWithPassphraseMessage
        width: parent.width
        onPassphraseEntered:{
            SelfVerificationStatus.verifyMasterKeyWithPassphrase(passphrase)
        }
    }

    UnverifiedDevicesMessage {
        id: unverifiedDevicesMessage
        width: parent.width
        onAccepted:{
            SelfVerificationStatus.verifyUnverifiedDevices()
        }
    }

    NoMasterKeyMessage {
        id: noMasterKeyMessage
    }

    function verify(){
        var status = SelfVerificationStatus.status
        console.log("Status: " + status)
        switch (status) {
            case SelfVerificationStatus.NoMasterKey:
                noMasterKeyMessage.open()
                return
            case SelfVerificationStatus.UnverifiedMasterKey:
                unverifiedMasterKeyMessage.open()
                return
            case SelfVerificationStatus.UnverifiedDevices:
                unverifiedDevicesMessage.open()
                return
            default:
                return
        }
    }

    function isVerified(){
        return ((SelfVerificationStatus.status == 0) ? true : false )
    }

    function onVerificationStatusChanged(status){
        var status = SelfVerificationStatus.status
        console.log("Verification Status Changed: " + status)
        statusChanged()
    }

    function onShowRecoveryKey(key) {
        showRecoverKeyDialog.recoveryKey = key;
        showRecoverKeyDialog.open();
    }

    function onSetupCompleted() {
        successDialog.open();
    }

    function onSetupFailed(m) {
        failureDialog.setError(qsTr("Failed to setup encryption: %1").arg(m));
        failureDialog.open();
    }
    
    function onVerifyFailed(m) {
        failureDialog.setError(m);
        failureDialog.open();
    }

    Component.onCompleted: {
        VerificationManager.newDeviceVerificationRequest.connect(onNewDeviceVerificationRequest)
        SelfVerificationStatus.statusChanged.connect(onVerificationStatusChanged)
        SelfVerificationStatus.showRecoveryKey.connect(onShowRecoveryKey)
        SelfVerificationStatus.setupCompleted.connect(onSetupCompleted)
        SelfVerificationStatus.setupFailed.connect(onSetupFailed)
        SelfVerificationStatus.verifyMasterKeyWithPassphraseFailed.connect(onVerifyFailed)
    }

    function onNewDeviceVerificationRequest(flow) {
        var dialog = deviceVerificationDialog.createObject(selfVerificationCheck, {
            "flow": flow
        });
        if(GlobalObject.mobileMode())
            dialog.showMaximized();
        else 
            dialog.show();
    }

    Component {
        id: deviceVerificationDialog

        DeviceVerification {
            width: qmlLibRoot.width
            height: qmlLibRoot.height
        }
    }
}
