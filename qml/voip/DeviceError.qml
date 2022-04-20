import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.2
import GlobalObject 1.0

Popup {
    property string errorString
    property var image

    modal: true
    // only set the anchors on Qt 5.12 or higher
    // see https://doc.qt.io/qt-5/qml-qtquick-controls2-popup.html#anchors.centerIn-prop
    Component.onCompleted: {
        if (anchors)
            anchors.centerIn = parent;

    }

    RowLayout {
        Image {
            Layout.preferredWidth: 16
            Layout.preferredHeight: 16
            source: "image://colorimage/" + image + "?" + GlobalObject.colors.windowText
        }

        Label {
            text: errorString
            color: GlobalObject.colors.windowText
        }

    }

    background: Rectangle {
        color: GlobalObject.colors.window
        border.color: GlobalObject.colors.windowText
    }

}
