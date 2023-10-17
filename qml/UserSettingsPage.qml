// SPDX-FileCopyrightText: 2021 Nheko Contributors
// SPDX-FileCopyrightText: 2022 Nheko Contributors
//
// SPDX-License-Identifier: GPL-3.0-or-later

import Qt.labs.platform 1.1 as Platform
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.2
import QtQuick.Window 2.15
import UserSettingsModel 1.0
import Settings 1.0
import GlobalObject 1.0
import DelegateChooser 1.0
import DelegateChoice 1.0
import "./device-verification"
import "./ui"

Page {
    id: userSettingsDialog

    property int collapsePoint: 600
    property bool collapsed: width < collapsePoint

    VerifyWithPassphraseMessage {
        id: downloadCrossSigningDialog
        width: parent.width
        onPassphraseEntered:{
            UserSettingsModel.downloadCrossSigningSecrets(passphrase)
        }
    }


    ScrollView {
        id: scroll

        palette: GlobalObject.colors
        ScrollBar.horizontal.visible: false
        anchors.fill: parent
        anchors.topMargin: (collapsed? backButton.height : 0)+20
        leftPadding: collapsed? 8 : 20
        bottomPadding: 20
        contentWidth: availableWidth

        ColumnLayout {
            id: grid

            spacing: 8

            anchors.fill: parent
            anchors.leftMargin: userSettingsDialog.collapsed ? 0 : (userSettingsDialog.width-userSettingsDialog.collapsePoint) * 0.4 + 20
            anchors.rightMargin: anchors.leftMargin

            Repeater {
                model: UserSettingsModel
                Layout.fillWidth:true

                delegate: GridLayout {
                    columns: collapsed? 1 : 2
                    rows: collapsed? 2: 1
                    required property var model
                    id: r

                    Label {
                        Layout.alignment: Qt.AlignLeft
                        Layout.fillWidth: true
                        color: GlobalObject.colors.text
                        text: model.name
                        //Layout.column: 0
                        Layout.columnSpan: (model.type == UserSettingsModel.SectionTitle && !userSettingsDialog.collapsed) ? 2 : 1
                        //Layout.row: model.index
                        //Layout.minimumWidth: implicitWidth
                        Layout.leftMargin: model.type == UserSettingsModel.SectionTitle ? 0 : 8
                        Layout.topMargin: model.type == UserSettingsModel.SectionTitle ? 20 : 0
                        font.pointSize: 1.1 * fontMetrics.font.pointSize

                        HoverHandler {
                            id: hovered
                            enabled: model.description ?? false
                        }
                        ToolTip.visible: hovered.hovered && model.description
                        ToolTip.text: model.description ?? ""
                        // ToolTip.delay: Nheko.tooltipDelay
                        wrapMode: Text.Wrap
                    }

                    DelegateChooser {
                        id: chooser

                        roleValue: model.type
                        Layout.alignment: Qt.AlignRight

                        Layout.columnSpan: (model.type == UserSettingsModel.SectionTitle && !userSettingsDialog.collapsed) ? 2 : 1
                        Layout.preferredHeight: child.height
                        Layout.preferredWidth: Math.min(child.implicitWidth, child.width || 1000)
                        Layout.maximumWidth: model.type == UserSettingsModel.SectionTitle ? Number.POSITIVE_INFINITY : 400
                        Layout.fillWidth: model.type == UserSettingsModel.SectionTitle || model.type == UserSettingsModel.Options || model.type == UserSettingsModel.Number
                        Layout.rightMargin: model.type == UserSettingsModel.SectionTitle ? 0 : 8

                        DelegateChoice {
                            roleValue: UserSettingsModel.Toggle
                            ToggleButton {
                                checked: model.value
                                onCheckedChanged: model.value = checked
                                enabled: model.enabled
                            }
                        }
                        DelegateChoice {
                            roleValue: UserSettingsModel.Options
                            ComboBox {
                                anchors.right: parent.right
                                width: Math.min(parent.width, implicitWidth)
                                model: r.model.values
                                currentIndex: r.model.value
                                onCurrentIndexChanged: r.model.value = currentIndex

                                WheelHandler{} // suppress scrolling changing values
                            }
                        }
                        DelegateChoice {
                            roleValue: UserSettingsModel.Integer

                            SpinBox {
                                anchors.right: parent.right
                                width: Math.min(parent.width, implicitWidth)
                                from: model.valueLowerBound
                                to: model.valueUpperBound
                                stepSize: model.valueStep
                                value: model.value
                                onValueChanged: model.value = value
                                editable: true

                                WheelHandler{} // suppress scrolling changing values
                            }
                        }
                        DelegateChoice {
                            roleValue: UserSettingsModel.Double

                            SpinBox {
                                id: spinbox

                                readonly property double div: 100
                                readonly property int decimals: 2

                                anchors.right: parent.right
                                width: Math.min(parent.width, implicitWidth)
                                from: model.valueLowerBound * div
                                to: model.valueUpperBound * div
                                stepSize: model.valueStep * div
                                value: model.value * div
                                onValueChanged: model.value = value/div
                                editable: true

                                property real realValue: value / div

                                validator: DoubleValidator {
                                    bottom: Math.min(spinbox.from/spinbox.div, spinbox.to/spinbox.div)
                                    top:  Math.max(spinbox.from/spinbox.div, spinbox.to/spinbox.div)
                                }

                                textFromValue: function(value, locale) {
                                    return Number(value / spinbox.div).toLocaleString(locale, 'f', spinbox.decimals)
                                }

                                valueFromText: function(text, locale) {
                                    return Number.fromLocaleString(locale, text) * spinbox.div
                                }

                                WheelHandler{} // suppress scrolling changing values
                            }
                        }
                        DelegateChoice {
                            roleValue: UserSettingsModel.ReadOnlyText
                            TextEdit {
                                color: GlobalObject.colors.text
                                text: model.value
                                readOnly: true
                                selectByMouse: !Settings.mobileMode
                                textFormat: Text.PlainText
                            }
                        }
                        DelegateChoice {
                            roleValue: UserSettingsModel.SectionTitle
                            Item {
                                width: grid.width
                                height: fontMetrics.lineSpacing
                                Rectangle {
                                    anchors.topMargin: 4
                                    anchors.top: parent.top
                                    anchors.left: parent.left
                                    anchors.right: parent.right
                                    color: GlobalObject.colors.buttonText
                                    height: 1
                                }
                            }
                        }
                        DelegateChoice {
                            roleValue: UserSettingsModel.KeyStatus
                            Text {
                                color: model.good ? "green" : GlobalObject.theme.error
                                text: model.value ? qsTr("CACHED") : qsTr("NOT CACHED")
                            }
                        }
                        DelegateChoice {
                            roleValue: UserSettingsModel.SessionKeyImportExport
                            RowLayout {
                                Button {
                                    text: qsTr("IMPORT")
                                    onClicked: UserSettingsModel.importSessionKeys()
                                }
                                Button {
                                    text: qsTr("EXPORT")
                                    onClicked: UserSettingsModel.exportSessionKeys()
                                }
                            }
                        }
                        DelegateChoice {
                            roleValue: UserSettingsModel.XSignKeysRequestDownload
                            RowLayout {
                                Button {
                                    text: qsTr("DOWNLOAD")
                                    onClicked: downloadCrossSigningDialog.open()
                                }
                                Button {
                                    text: qsTr("REQUEST")
                                    onClicked: UserSettingsModel.requestCrossSigningSecrets()
                                }
                            }
                        }
                        DelegateChoice {
                            Text {
                                text: model.value
                            }
                        }
                    }
                }
            }

            Rectangle {
                anchors.topMargin: 4
                Layout.fillWidth: true
                color: GlobalObject.colors.buttonText
                height: 1
            }
            GridLayout {
                anchors.left: parent.left
                anchors.right: parent.right
                Layout.fillWidth: true

                columns: collapsed? 1 : 2
                rows: collapsed? 2: 1

                Text {
                    text: qsTr("Audio / Video Settings")
                    Layout.fillWidth: true
                }
                Component {
                    id: callSettingsDialogFactory
                    CallSettingsDialog {
                    }
                }
                Button {
                    text: qsTr("Open")
                    onClicked: {
                        if(!GlobalObject.mobileMode()) {
                            var callSettingsDialog = callSettingsDialogFactory.createObject(parent);
                            callSettingsDialog.show();
                            destroyOnClose(callSettingsDialog);
                        }
                    }
                }
            }
        }
    }
}

