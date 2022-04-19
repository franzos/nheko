import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.5
import MatrixClient 1.0
import UserInformation 1.0

Page {
    id: userInfo
    width: parent.width
    title: "Profile"
   
    Column {
        anchors.fill: parent
        anchors.margins: 10
        RoundButton {
            id: avatarButton
            text: "."
            width: 86; height: 86
            anchors.horizontalCenter: parent.horizontalCenter
        }
        Row {
            Label { text: "ID : " }
            Label {
                id: idLabel
                text: "..."
            }
        }
      
        Row {
            Label { text: "Device ID : " }
            Label {
                id: deviceIdLabel
                text: "..."
            }
        }
        Row {
            Label { text: "Server : " }
            Label {
                id: serverLabel
                text: "..."
            }
        }       
    }
    Component.onCompleted: {
        var info = MatrixClient.userInformation()
        idLabel.text = info.userId  
        deviceIdLabel.text =  info.deviceId
        serverLabel.text =   info.homeServer
    }
}
