import QtQuick 2.9
import QtQuick.Layouts 1.2
import org.freedesktop.gstreamer.GLVideoItem 1.0
import ".."

CustomPage {
    Layout.fillWidth: true
    GstGLVideoItem {
        anchors.fill: parent
        objectName: "videoCallItem"
    }
}