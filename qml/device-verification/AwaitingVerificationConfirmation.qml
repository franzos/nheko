import QtQuick 2.3
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.10
import GlobalObject 1.0

Pane {
    property string title: qsTr("Awaiting Confirmation")
    Column {
        spacing: 16
        anchors.fill: parent
        Label {
            id: content
            width: parent.width

            Layout.fillHeight: true
            Layout.fillWidth: true
            wrapMode: Text.Wrap
            text: qsTr("Waiting for other side to complete verification.")
            color: GlobalObject.colors.text
            verticalAlignment: Text.AlignVCenter
        }

        // Spinner {
        //     Layout.alignment: Qt.AlignHCenter
        //     foreground: GlobalObject.colors.mid
        // }

        RowLayout {
            Button {
                Layout.alignment: Qt.AlignLeft
                text: qsTr("Cancel")
                onClicked: {
                    flow.cancel();
                    dialog.close();
                }
            }

            Item {
                Layout.fillWidth: true
            }

        }

    }

}
