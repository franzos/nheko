import ".."
import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import GlobalObject 1.0

ApplicationWindow {
    id: inputDialog

    property alias prompt: promptLabel.text
    property alias echoMode: statusInput.echoMode
    property var onAccepted: undefined

    modality: Qt.NonModal
    flags: Qt.Dialog
    width: 350
    height: fontMetrics.lineSpacing * 7

    function forceActiveFocus() {
        statusInput.forceActiveFocus();
    }

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

            color: GlobalObject.colors.text
        }

        MatrixTextField {
            id: statusInput

            Layout.fillWidth: true
            onAccepted: dbb.accepted()
            focus: true
        }

    }

    footer: DialogButtonBox {
        id: dbb

        standardButtons: DialogButtonBox.Ok | DialogButtonBox.Cancel
        onAccepted: {
            if (inputDialog.onAccepted)
                inputDialog.onAccepted(statusInput.text);

            inputDialog.close();
        }
        onRejected: {
            inputDialog.close();
        }
    }

}
