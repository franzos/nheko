import ".."
import "../.."
import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import GlobalObject 1.0

CustomApplicationWindow {
    id: joinRoomRoot

    title: qsTr("Join room")
    modality: Qt.WindowModal
    flags: Qt.Dialog | Qt.WindowCloseButtonHint | Qt.WindowTitleHint
    palette: GlobalObject.colors
    width: 350
    height: 130

    Shortcut {
        sequence: StandardKey.Cancel
        onActivated: dbb.rejected()
    }

    ColumnLayout {
        spacing: 8
        anchors.margins: 8
        anchors.fill: parent

        Label {
            id: promptLabel

            text: qsTr("Room ID or alias")
            color: GlobalObject.colors.text
        }

        MatrixTextField {
            id: input

            focus: true
            Layout.fillWidth: true
            onAccepted: {
                if (input.text.match("#.+?:.{3,}"))
                    dbb.accepted();

            }
        }

    }

    footer: DialogButtonBox {
        id: dbb

        standardButtons: DialogButtonBox.Cancel
        onAccepted: {
            MatrixClient.joinRoom(input.text);
            joinRoomRoot.close();
        }
        onRejected: {
            joinRoomRoot.close();
        }

        Button {
            text: qsTr("Join")
            enabled: input.text.match("#.+?:.{3,}")
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
        }

    }

}
