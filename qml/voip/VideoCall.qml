import QtQuick 2.9
import QtQuick.Layouts 1.2
import org.freedesktop.gstreamer.GLVideoItem 1.0
import ".."
import CallManager 1.0

CustomPage {
    Layout.fillWidth: true
    GstGLVideoItem {
        anchors.fill: parent
        objectName: "videoCallItem"
    }

    function endCallClicked(){
        CallManager.hangUp();
    }

    Component.onCompleted: {
        header.setTimelineButtonsVisible(false)
        header.setOptionButtonsVisible(false)
        header.setEndCallButtonsVisible(true)
        header.endCallClicked.connect(endCallClicked)
    }
}