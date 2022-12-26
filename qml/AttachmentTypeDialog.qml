import QtQuick 2.2
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.12
import Settings 1.0
import GlobalObject 1.0
import TimelineModel 1.0

Dialog {
    id: atd
    width: 250
    height: 270
    title: "Attach"
    property var room: timelineModel

    ListModel {
        id: modelMenu
        ListElement {
            item: "Document"
            icon: ":/images/document.svg"            
        }
        ListElement {
            item: "Audio/Video"
            icon: ":/images/video-file.svg"            
        }
        ListElement {
            item: "Image"
            icon: ":/images/image.svg"            
        }
        ListElement {
            item: "Location"
            icon: ":/images/location.svg"            
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
        model: modelMenu
        delegate: componentDelegate
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
                        case "Document":
                        case "Audio/Video":
                        case "Image":
                            room.input.openFileSelection()
                            close()
                            break;  
                        case "Location":
                            console.log("TODO")
                            break;        
                    }
                }
            }
        }
    }
}