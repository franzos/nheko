import QtQuick 2.3
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.10

Pane {
    property string title: qsTr("Verification Code")
    Column {
        spacing: 16
        anchors.fill: parent

        Label {
            width: parent.width
            Layout.fillHeight: true
            Layout.fillWidth: true
            wrapMode: Text.Wrap
            text: qsTr("Please verify the following digits. You should see the same numbers on both sides. If they differ, please press 'They do not match!' to abort verification!")
            verticalAlignment: Text.AlignVCenter
        }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter

            Label {
                font.pixelSize: Qt.application.font.pixelSize * 2
                text: flow.sasList[0]
            }

            Label {
                font.pixelSize: Qt.application.font.pixelSize * 2
                text: flow.sasList[1]
            }

            Label {
                font.pixelSize: Qt.application.font.pixelSize * 2
                text: flow.sasList[2]
            }

        }

        RowLayout {
            Button {
                Layout.alignment: Qt.AlignLeft
                text: qsTr("They do not match!")
                onClicked: {
                    flow.cancel();
                    dialog.close();
                }
            }

            Item {
                Layout.fillWidth: true
            }

            Button {
                Layout.alignment: Qt.AlignRight
                text: qsTr("They match!")
                onClicked: flow.next()
            }

        }

    }

}
