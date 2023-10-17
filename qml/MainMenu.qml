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
    property var hiddenEntries: []

    ListModel {
        id: modelMenu
        ListElement {
            item: "Profile"
            icon: ":/images/px-user.svg"
            name: "profile"
        }
        ListElement {
            item: "Settings"
            icon: ":/images/settings.svg"
            name: "settings"
        }
        ListElement {
            item: "My QR code"
            icon: ":/images/qrcode.svg"            
            name: "my_qr_code"
        }
        ListElement {
            item: "Logout"
            icon: ":/images/power-off.svg"            
            name: "logout"
        }
        ListElement {
            item: "About"
            icon: ":/images/about.svg"            
            name: "about"
        }       
    }

    ListModel {
        id: modelMenuFiltered
    }

    Component.onCompleted: {
        modelMenuFiltered.clear()
        for (var i = 0; i < modelMenu.count; i++) {
            var currentName = modelMenu.get(i).name
            if (menu.hiddenEntries.indexOf(currentName) < 0) {
                modelMenuFiltered.append(modelMenu.get(i))
            }
        }
    }

    ListView {
        id: listViewMenu
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        // spacing: 10
        clip: true
        model: modelMenuFiltered
        delegate: componentDelegate
    }
    Component {
        id: qrCodeComponent

        QRCode {
        }
    }
    Component {
        id: componentDelegate

        Rectangle {
            id: wrapperItem
            height: 50
            width: parent.width
            color: "transparent"
            Image {
                id: imgItem
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 8
                height: parent.height - 25
                width: height
                source: "image://colorimage/" + icon + "?" + GlobalObject.colors.windowText
                smooth: true
                antialiasing: true
            }

            Label {
                id: textItem
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: imgItem.right
                anchors.leftMargin: 8
                text: item
            }
            CustomBorder
            {
                anchors.top: parent.bottom
                commonBorder: false
                lBorderwidth: 0
                rBorderwidth: 0
                tBorderwidth: 0
                bBorderwidth: 1
                borderColor: GlobalObject.colors.alternateBase
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
                        case "My QR code":
                            var qrCode = qrCodeComponent.createObject(menu);
                            if(GlobalObject.mobileMode())
                                qrCode.showMaximized();
                            else 
                                qrCode.show();
                            destroyOnClose(qrCode);
                            menu.close()
                            break;
                        case "Settings":
                            var settings = userSettingsPage.createObject(stack);
                            stack.push(settings)
                            menu.close()
                            break;
                        case "About":
                            aboutClicked()
                            aboutDialog.open()
                            break;  
                        case "Profile":
                            showUserInfo()
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
            text: "Matrix Library: "+MatrixClient.getLibraryVersion()+"\n"+"Matrix GUI Library: "+GlobalObject.getApplicationVersion()         
        }
      
        onAccepted: {
            menu.close()
        }
    }

    Component {
        id: userSettingsPage

        UserSettingsPage {
        }
    }

    Component {
        id: userInfoFactory
        UserInfo {}
    }

    function showUserInfo(){
        var userinf = userInfoFactory.createObject(stack);
        stack.push(userinf)
        menu.close()
    }   
}

