// SPDX-FileCopyrightText: 2021 Nheko Contributors
// SPDX-FileCopyrightText: 2022 Nheko Contributors
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Window 2.13
import VerificationManager 1.0
import GlobalObject 1.0

ApplicationWindow {
    id: dialog

    property var flow

    onClosing: VerificationManager.removeVerificationFlow(flow)
    title: stack.currentItem ? (stack.currentItem.title_ || "") : ""
    modality: Qt.NonModal
    palette: GlobalObject.colors
    color: GlobalObject.colors.window
    //height: stack.currentItem.implicitHeight
    minimumHeight: stack.currentItem.implicitHeight + 2 * GlobalObject.paddingLarge
    height: stack.currentItem.implicitHeight + 2 * GlobalObject.paddingMedium
    minimumWidth: 400
    flags: Qt.Dialog | Qt.WindowCloseButtonHint | Qt.WindowTitleHint

    background: Rectangle {
        color: GlobalObject.colors.window
    }


    StackView {
        id: stack

        anchors.centerIn: parent

        initialItem: newVerificationRequest
        implicitWidth: dialog.width - 2* GlobalObject.paddingMedium
        implicitHeight: dialog.height - 2* GlobalObject.paddingMedium
    }

    Component {
        id: newVerificationRequest

        NewVerificationRequest {
        }

    }

    Component {
        id: waiting

        Waiting {
        }

    }

    Component {
        id: success

        Success {
        }

    }

    Component {
        id: failed

        Failed {
        }

    }

    Component {
        id: digitVerification

        DigitVerification {
        }

    }

    Component {
        id: emojiVerification

        EmojiVerification {
        }

    }

    Item {
        state: flow.state
        states: [
            State {
                name: "PromptStartVerification"

                StateChangeScript {
                    script: stack.replace(null, newVerificationRequest)
                }

            },
            State {
                name: "CompareEmoji"

                StateChangeScript {
                    script: stack.replace(null, emojiVerification)
                }

            },
            State {
                name: "CompareNumber"

                StateChangeScript {
                    script: stack.replace(null, digitVerification)
                }

            },
            State {
                name: "WaitingForKeys"

                StateChangeScript {
                    script: stack.replace(null, waiting)
                }

            },
            State {
                name: "WaitingForOtherToAccept"

                StateChangeScript {
                    script: stack.replace(null, waiting)
                }

            },
            State {
                name: "WaitingForMac"

                StateChangeScript {
                    script: stack.replace(null, waiting)
                }

            },
            State {
                name: "Success"

                StateChangeScript {
                    script: stack.replace(null, success)
                }

            },
            State {
                name: "Failed"

                StateChangeScript {
                    script: stack.replace(null, failed)
                }

            }
        ]
    }

}
