import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import QtQuick.Window 2.15
import MatrixClient 1.0
import GlobalObject 1.0

Drawer {
    id: menu

    signal aboutClicked()
    property alias currentItem: listViewMenu.currentIndex

    ListModel {
        id: modelMenu
        ListElement {
            item: "Settings"
            icon: ":/images/settings.svg"            
        }
        ListElement {
            item: "Logout"
            icon: ":/images/power-off.svg"            
        }
        ListElement {
            item: "About"
            icon: ":/images/about.svg"            
        }
    }

   
    ListView {
        id: listViewMenu
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        spacing: 2
        clip: true
        model: modelMenu
        delegate: componentDelegate
    }

    Component {
        id: componentDelegate
        
        Rectangle {
            id: wrapperItem
            height: 34
            width: parent.width
            color: "transparent"

            Image {
                id: imgItem
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 2
                height: parent.height
                width: height
                source: "image://colorimage/" + icon + "?" + GlobalObject.colors.windowText
                smooth: true
                antialiasing: true
            }

            Label {
                id: textItem
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: imgItem.right
                anchors.leftMargin: 2
                text: item
            }

            MouseArea {
                id: ma
                anchors.fill: parent
                enabled: true
                onClicked: {
                    listViewMenu.currentIndex = index
                    switch( item ) {
                        case "Logout":
                            logoutDialog.open()
                            break;
                        case "Settings":
                            settingsDialog.open()
                            break;
                        case "About":
                            aboutClicked()
                            aboutDialog.open()
                            break;        
                    }
                }
            }
        }
    }

    Dialog {
        id: logoutDialog
        x: (qmlLibRoot.width - width) / 2
        y: (qmlLibRoot.height - height) / 2
        title: "Logout"
        standardButtons: Dialog.Cancel | Dialog.Ok
        Label {            
            text: "Are you sure you want to logout ?"
        }
        onAccepted: {
            MatrixClient.logout()
            menu.close()
        }
        onRejected: {
            menu.close()
        }
    }

    Dialog {
        id: aboutDialog
        x: (qmlLibRoot.width - width) / 2
        y: (qmlLibRoot.height - height) / 2
        title: "About"
        standardButtons: Dialog.Ok
        Label {
            width: parent.width
            wrapMode: Text.Wrap
            text: "Library Version: "+MatrixClient.getLibraryVersion()+"\n"+"Application Version: "+GlobalObject.getApplicationVersion()         
        }
      
        onAccepted: {
            menu.close()
        }
    }

    Dialog {
        id: settingsDialog
        x: (qmlLibRoot.width - width) / 2
        y: (qmlLibRoot.height - height) / 2
        title: "Settings"
        standardButtons: Dialog.Ok
        Label {            
            text: "Coming Soon"
        }
        onAccepted: {
            menu.close()
        }
    }
   
}

