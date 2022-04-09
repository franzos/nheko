import QtQuick 2.9
import QtQuick.Layouts 1.2
import org.freedesktop.gstreamer.GLVideoItem 1.0
import WebRTCState 1.0
import CallManager 1.0
import ".."

CustomPage {
    Layout.fillWidth: true
    state: "oncall"
    VideoCallEmbedPage {
        id: videocallembedpage
    }

    function onCallStateChanged(s){
        videocallembedpage.setCallPartyName(CallManager.callPartyDisplayName)
        videocallembedpage.changeState(CallManager.callState)
    }

    Component.onCompleted: {
        header.setTitle("Video Call")
        header.setOptionButtonsVisible(false)
        header.setBackButtonsVisible(false)
        CallManager.onNewCallState.connect(onCallStateChanged)
        listenToCallManager()
    }
}