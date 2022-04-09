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
        var callstate = CallManager.callState;
        if(state == "oncall"){
            var callParty = CallManager.callPartyDisplayName
            if (callstate == WebRTCState.CONNECTED){
                videocallembedpage.changeState("oncall")
            } else if (callstate == WebRTCState.ANSWERSENT || callstate == WebRTCState.CONNECTING || 
                       callstate == WebRTCState.OFFERSENT  || callstate == WebRTCState.INITIATING ){
                videocallembedpage.changeState("transient")
                var text = "...";
                if(callstate == WebRTCState.ANSWERSENT)
                    text = "Answering " + callParty + "...";
                else if(callstate == WebRTCState.CONNECTING)
                    text = "Connecting " + callParty + "...";
                else if(callstate == WebRTCState.OFFERSENT)
                    text = "Calling " + callParty + "...";
                videocallembedpage.setTransientText(text)
            } 
        } else {
            videocallembedpage.changeState("freecall")
        }
    }

    Component.onCompleted: {
        header.setTitle("Video Call")
        header.setOptionButtonsVisible(false)
        header.setBackButtonsVisible(false)
        CallManager.onNewCallState.connect(onCallStateChanged)
        listenToCallManager()
    }
}