import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.5

import MatrixClient 1.0
import Rooms 1.0

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

    function onVerificationStatusChanged(){
        header.setVerified(selfVerificationCheck.isVerified())
    }

    SelfVerificationCheck{
        id: selfVerificationCheck
    }

    Component.onCompleted: {
        header.titleClicked.connect(selfVerificationCheck.verify)
        selfVerificationCheck.statusChanged.connect(onVerificationStatusChanged)
    }

    Connections {
        target: MatrixClient

        function onUserDisplayNameReady(name){
            displayName = name
            header.setTitle(displayName)
            onVerificationStatusChanged()
        }
    }
}
