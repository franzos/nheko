import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import MatrixClient 1.0

ToolBar {
    width: parent.width

    signal titleClicked()
    signal menuClicked()
    Row {
        anchors.fill: parent
        spacing: 2
        ToolButton{
            id: menuButton
            icon.source: "qrc:/images/drawer.png"
            width: parent.height
            height: parent.height
            enabled: !stack.empty
            onClicked: {
                menuClicked()
            }
        }
        ToolButton {
            id: backButton
            icon.source: "qrc:/images/back.png"
            width: parent.height
            height: parent.height
            enabled: !stack.empty
            onClicked: stack.pop()
        }

        Button {
            id: titleLabel
            width: parent.width - backButton.width - 2
            height: parent.height
            anchors.leftMargin: 2
            onClicked: {titleClicked()}
        }

        Rectangle {
            id: verifyRect
            height: parent.height - 5
            width: height 
            radius: width/2
            color: "#ffaf49"
            anchors.right: titleLabel.right
            anchors.verticalCenter: parent.verticalCenter
            visible: false
            Label {
                anchors.centerIn: parent
                color: "white"
                font.pointSize: 10
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                text: "!"
            }
        }
    }

    function setHomeButtonsVisible(visible){
        menuButton.visible = visible;
        backButton.visible = !visible;
    }

    function setTitle(title){
        titleLabel.text = title
        backButton.enabled= !stack.empty
    }

    function setVerified(flag){
        if(flag){
            verifyRect.visible = false
        } else {
            verifyRect.visible = true
        }
    }

    Component.onCompleted: {
        setHomeButtonsVisible(false)
    }
}


