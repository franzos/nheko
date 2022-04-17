import QtQuick 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.15
import org.freedesktop.gstreamer.GLVideoItem 1.0
import WebRTCState 1.0
import CallManager 1.0
import ".."

Page {
    Layout.fillWidth: true
    state: "oncall"
    title: "Video Call"
    VideoCallEmbedPage {
        id: videocallembedpage
    }
}