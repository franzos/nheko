import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import MatrixClient 1.0
import GlobalObject 1.0


Drawer {
    id: menu

    signal aboutClicked()
    property alias currentItem: listViewMenu.currentIndex

    ListModel {
        id: modelMenu
        ListElement {
            item: "Logout"
            icon: "qrc:/images/power-off.svg"            
        }
        ListElement {
            item: "Settings"
            icon: "qrc:/images/settings.svg"            
        }
        ListElement {
            item: "About"
            icon: "qrc:/images/star.svg"            
        }
    }

   
    ListView {
        id: listViewMenu
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        clip: true
        model: modelMenu
        delegate: componentDelegate
    }

    Component {
        id: componentDelegate
        
        Rectangle {
            id: wrapperItem
            height: 32
            width: parent.width
            Image {
                id: imgItem
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 2
                height: parent.height*0.80
                width: height
                source: icon
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
        }
        onRejected: {}
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
      
        onAccepted: {}
    }
   
}

