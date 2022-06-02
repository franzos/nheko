import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.5
import MatrixClient 1.0
import UserInformation 1.0
import CMUserInformation 1.0
import QmlInterface 1.0
import "ui"

Page {
    id: userInfo
    width: parent.width
    title: "Profile"
    property var info : MatrixClient.userInformation()
    Column{
        anchors.fill: parent
        anchors.margins: 10
        spacing: 25
        Avatar {
            id: avatarButton
            width: 86; height: 86
            anchors.horizontalCenter: parent.horizontalCenter
            url: info.avatarUrl.replace("mxc://", "image://MxcImage/")
            userid: info.userId 
            displayName: info.displayName 
        }
        ColumnLayout {
            width: parent.width
            anchors.margins: 10
            spacing: 5
            Label { text: "Matrix profile : " }
            GroupBox {
                Layout.fillWidth: true
                Label {
                    id: idLabel
                    text:   "Matrix ID :\n" + 
                            info.userId + "\n\n" +
                            "Server :\n" + 
                            info.homeServer + "\n\n" +
                            "Device ID :\n" + 
                            info.deviceId
                }
            }
        }
        ColumnLayout {
            width: parent.width
            anchors.margins: 10
            spacing: 5
            Label { text: "Global profile : " }
            GroupBox {
                Layout.fillWidth: true
                Label {
                    id: cmInfoText
                    text: (QmlInterface.cmUserInformation().username?(formatCmUserInfo(QmlInterface.cmUserInformation())):"Click on 'Refresh global profile' button to reload.")
                    anchors.fill: parent
                }
            }

            LoadingButton {
                id: refreshButton
                text: "Refresh global profile"
                Layout.alignment: Qt.AlignHCenter
                onClicked:{
                    MatrixClient.getCMuserInfo()
                    loadingState = true
                }
            }
        }
    }

    function formatCmUserInfo(info){
        return  "Username :\n" + 
                info.username + "\n" + 
                "\n" + 
                "Full name:\n" + 
                ((info.localizedFirstName || info.localizedLastName)?info.localizedTitle + " " + info.localizedFirstName + " " + info.localizedLastName:info.title + " " + info.firstname + " " + info.lastname) + "\n" + 
                "\n" + 
                "Phone number:\n" + 
                (info.phone?info.phone:"None") + "\n" + 
                "\n" +
                "Email address:\n" +
                (info.email?info.email:"None")
    }

    Connections {        
        target: MatrixClient
        function onCmUserInfoFailure(msg) {
            refreshButton.loadingState = false
        }

        function onCmUserInfoUpdated(info) {
            cmInfoText.text = formatCmUserInfo(info)
            console.log(info)
            refreshButton.loadingState = false
        }
    }
}
