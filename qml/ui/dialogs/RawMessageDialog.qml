// SPDX-FileCopyrightText: 2021 Nheko Contributors
// SPDX-FileCopyrightText: 2022 Nheko Contributors
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import GlobalObject 1.0
import "../../"

CustomApplicationWindow {
    id: rawMessageRoot

    property alias rawMessage: rawMessageView.text

    height: 420
    width: 420
    flags: Qt.Tool | Qt.WindowStaysOnTopHint | Qt.WindowCloseButtonHint | Qt.WindowTitleHint

    Shortcut {
        sequence: StandardKey.Cancel
        onActivated: rawMessageRoot.close()
    }

    ScrollView {
        anchors.margins: 8
        anchors.fill: parent
        palette: GlobalObject.colors
        padding: 8

        TextArea {
            id: rawMessageView

            font: GlobalObject.monospaceFont()
            color: GlobalObject.colors.text
            readOnly: true
            textFormat: Text.PlainText

            anchors.fill: parent

            background: Rectangle {
                color: GlobalObject.colors.base
            }

        }

    }

    footer: DialogButtonBox {
        standardButtons: DialogButtonBox.Ok
        onAccepted: rawMessageRoot.close()
    }

}
