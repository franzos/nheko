import QtQuick 2.9
import QtQuick.Layouts 1.2
import org.freedesktop.gstreamer.GLVideoItem 1.0

Item {
    anchors.fill: parent
    GstGLVideoItem {
        anchors.fill: parent
        objectName: "videoCallEmbedItem"
    }
}