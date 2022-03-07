import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.5
import MatrixClient 1.0

Page {
    height:parent.height
    width:parent.width
    anchors.fill:parent
    Column{
        anchors.fill:parent
        spacing: 10
        TextField {
            id: nameTextField
            width:parent.width
            placeholderText: qsTr("Name")
        }
        TextField {
            id: topicTextField
            width:parent.width
            placeholderText: qsTr("Topic")
        }
        TextField {
            id: aliasTextField
            width:parent.width
            placeholderText: qsTr("Alias")
        }
    }   
    footer:Row{
        spacing: 10
        Button{
            id: startChatButton
            text: "Start Chat"
            width: parent.width/2 - 5
            anchors.leftMargin: 2
            onClicked: stack.push(createRoom)
        }
        Button{
            id: cancelChatButton
            text: "Cancel"
            width: parent.width/2 - 5
            anchors.leftMargin: 2
            onClicked: stack.pop()
        }        
    }
}