import QtQuick 2.3
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.10

Pane {
    property string title: qsTr("Successful Verification")
    Column {
        spacing: 16
        anchors.fill: parent

        Label {
            id: content
            width: parent.width
            Layout.fillHeight: true
            Layout.fillWidth: true
            wrapMode: Text.Wrap
            text: qsTr("Verification successful! Both sides verified their devices!")
            verticalAlignment: Text.AlignVCenter
        }

        RowLayout {
            Item {
                Layout.fillWidth: true
            }

            Button {
                Layout.alignment: Qt.AlignRight
                text: qsTr("Close")
                onClicked: dialog.close()
            }

        }

    }

}
