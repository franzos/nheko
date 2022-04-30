import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.5
import MatrixClient 1.0
import UserInformation 1.0
import "ui"
Page {
    id: userInfo
    width: parent.width
    title: "Profile"
    property var info : MatrixClient.userInformation()
    Column {
        anchors.fill: parent
        anchors.margins: 10

        Avatar {
            id: avatarButton
            width: 86; height: 86
            anchors.horizontalCenter: parent.horizontalCenter
            url: info.avatarUrl.replace("mxc://", "image://MxcImage/")
            userid: info.userId 
            displayName: info.displayName 
        }
        
        Row {
            Label { text: "ID : " }
            Label {
                id: idLabel
                text: info.userId 
            }
        }
      
        Row {
            Label { text: "Device ID : " }
            Label {
                id: deviceIdLabel
                text: info.deviceId
            }
        }
        Row {
            Label { text: "Server : " }
            Label {
                id: serverLabel
                text : info.homeServer
            }
        }       
    }
}
