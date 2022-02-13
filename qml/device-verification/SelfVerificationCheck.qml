import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.5

import SelfVerificationStatus 1.0
import VerificationManager 1.0

Item {
    id: selfVerificationCheck
    visible: false
    width: qmlApplication.width
    height: qmlApplication.height

    signal statusChanged()

    UnverifiedMasterKeyMessage {
        id: unverifiedMasterKeyMessage
        width: parent.width
        x: (qmlApplication.width - width) / 2
        y: (qmlApplication.height - height) / 2
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
        x: (qmlApplication.width - width) / 2
        y: (qmlApplication.height - height) / 2
        onPassphraseEntered:{
            SelfVerificationStatus.verifyMasterKeyWithPassphrase(passphrase)
        }
    }

    UnverifiedDevicesMessage {
        id: unverifiedDevicesMessage
        width: parent.width
        x: (qmlApplication.width - width) / 2
        y: (qmlApplication.height - height) / 2
        onAccepted:{
            SelfVerificationStatus.verifyUnverifiedDevices()
        }
    }

    function verify(){
        var status = SelfVerificationStatus.status
        console.log("Status: " + status)
        switch (status) {
            case SelfVerificationStatus.NoMasterKey:
                console.log("TODO: NoMasterKey")
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

    function verificationStatusChanged(status){
        var status = SelfVerificationStatus.status
        console.log("Verification Status Changed: " + status)
        statusChanged()
    }

    Component.onCompleted: {
        VerificationManager.newDeviceVerificationRequest.connect(onNewDeviceVerificationRequest)
        SelfVerificationStatus.statusChanged.connect(verificationStatusChanged)
    }

    function onNewDeviceVerificationRequest(flow) {
        var dialog = deviceVerificationDialog.createObject(selfVerificationCheck, {
            "flow": flow
        });
        dialog.show();
    }

    Component {
        id: deviceVerificationDialog

        DeviceVerification {
            width: qmlApplication.width
            height: qmlApplication.height
            // x: (qmlApplication.width - width) / 2
            // y: (qmlApplication.height - height) / 2
        }
    }
}
