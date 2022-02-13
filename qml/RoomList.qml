import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.5

import MatrixClient 1.0
import SelfVerificationStatus 1.0
import Rooms 1.0
import VerificationManager 1.0

import "device-verification"

CustomPage {
    id: roomPage
    width: parent.width
    property string displayName;

    ListView {
        id: roomListView
        anchors.fill: parent
        spacing: 10
        anchors.margins: 10
        flickableDirection: Flickable.VerticalFlick
        boundsBehavior: Flickable.StopAtBounds
        ScrollBar.vertical: ScrollBar {}
        model: Rooms
        delegate:RoomDelegate{}
    }

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

    function verification(){
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

    function verificationStatusChanged(status){
        var status = SelfVerificationStatus.status
        console.log("Verification Status Changed: " + status)
        switch (status) {
            case SelfVerificationStatus.NoMasterKey:
            case SelfVerificationStatus.UnverifiedMasterKey:
            case SelfVerificationStatus.UnverifiedDevices:
                header.setVerified(false)
                return; 
            default:
                header.setVerified(true)
                return;
        }
    }

    Component.onCompleted: {
        header.titleClicked.connect(verification)
        VerificationManager.newDeviceVerificationRequest.connect(onNewDeviceVerificationRequest)
        SelfVerificationStatus.statusChanged.connect(verificationStatusChanged)
    }

    function onNewDeviceVerificationRequest(flow) {
        var dialog = deviceVerificationDialog.createObject(roomPage, {
            "flow": flow
        });
        dialog.show();
    }

    Component {
        id: deviceVerificationDialog

        DeviceVerification {
            width: parent.width
            x: (qmlApplication.width - width) / 2
            y: (qmlApplication.height - height) / 2
        }
    }

    Connections {
        target: MatrixClient

        function onUserDisplayNameReady(name){
            displayName = name
            header.setTitle(displayName)
            verificationStatusChanged()
        }
    }
}
