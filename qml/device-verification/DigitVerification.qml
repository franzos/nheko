// SPDX-FileCopyrightText: 2021 Nheko Contributors
// SPDX-FileCopyrightText: 2022 Nheko Contributors
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.3
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.10
import GlobalObject 1.0

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
            color: GlobalObject.colors.text
            verticalAlignment: Text.AlignVCenter
        }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter

            Label {
                font.pixelSize: Qt.application.font.pixelSize * 2
                text: flow.sasList[0]
                color: GlobalObject.colors.text
            }

            Label {
                font.pixelSize: Qt.application.font.pixelSize * 2
                text: flow.sasList[1]
                color: GlobalObject.colors.text
            }

            Label {
                font.pixelSize: Qt.application.font.pixelSize * 2
                text: flow.sasList[2]
                color: GlobalObject.colors.text
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
