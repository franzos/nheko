import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import MatrixClient 1.0
import CallManager 1.0
import QtQml 2.15
import "device-verification"

Page {
    id: page
    anchors.fill:parent
    readonly property bool inPortrait: width < height
    property string savedTitle

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

    function endCallClicked(){
        CallManager.hangUp();
    }

    // Timer {
    //     id: updateCallManagerTimer
    //     interval: 100; running: false; repeat: false
    //     onTriggered: onNewCallState()
    // }

    function listenToCallManager(){
        CallManager.onNewCallState.connect(onNewCallState)
        onNewCallState()
    }

    function onNewCallState(){
        if(CallManager.isOnCall){
            page.state = "OnCall"
        } else {
            page.state = "FreeCall"
        }
    }

    state: "none"
    states: [
        State {
            name: "none"
            StateChangeScript {
                script: {
                    header.setCallButtonsVisible(false)
                    header.setEndCallButtonsVisible(false)
                }
            }
        },
        State {
            name: "FreeCall"
            StateChangeScript {
                script: {
                    header.setCallButtonsVisible(true)
                    header.setEndCallButtonsVisible(false)
                    if(savedTitle)
                        header.setTitle(savedTitle)
                }
            }
        },
        State {
            name: "OnCall"
            StateChangeScript {
                script: {
                    header.setCallButtonsVisible(false)
                    header.setEndCallButtonsVisible(true)
                    savedTitle = header.title()
                    header.setTitle(CallManager.callPartyDisplayName + " calling ...")
                }
            }
        }
    ]

    Component.onCompleted: {
        header.menuClicked.connect(menuClickedCallback)
        header.verifyClicked.connect(selfVerificationCheck.verify)
        header.endCallClicked.connect(endCallClicked)
    }

    MainMenu{
        id: navDrawer
        y: header.height
        width: parent.width / 2
        height: parent.height - header.height
    }
}
