import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import MatrixClient 1.0
import "device-verification"

Page {
    id: page
    width: parent.width
    readonly property bool inPortrait: width < height
    header: CustomHeader {} 
    function menuClickedCallback(){
        if(!navDrawer.opened)
            navDrawer.open()

        if(navDrawer.opened)
            navDrawer.close()
    }

    SelfVerificationCheck{
        id: selfVerificationCheck
    }

    Component.onCompleted: {
        header.menuClicked.connect(menuClickedCallback)
        header.verifyClicked.connect(selfVerificationCheck.verify)
    }
    MainMenu{
        id: navDrawer
        y: header.height
        width: parent.width / 2
        height: parent.height - header.height
    }
}
