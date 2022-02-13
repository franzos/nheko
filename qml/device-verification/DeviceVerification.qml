import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Window 2.13
import VerificationManager 1.0
 
ApplicationWindow {
    id: dialog

    property var flow

    onClosing: VerificationManager.removeVerificationFlow(flow)
    title: stack.currentItem.title
    modality: Qt.NonModal
    // width: stack.implicitWidth
    flags: Qt.Dialog | Qt.WindowCloseButtonHint | Qt.WindowTitleHint

    StackView {
        id: stack

        initialItem: newVerificationRequest
        anchors.fill: parent
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
                    script: stack.replace(newVerificationRequest)
                }

            },
            State {
                name: "CompareEmoji"

                StateChangeScript {
                    script: stack.replace(emojiVerification)
                }

            },
            State {
                name: "CompareNumber"

                StateChangeScript {
                    script: stack.replace(digitVerification)
                }

            },
            State {
                name: "WaitingForKeys"

                StateChangeScript {
                    script: stack.replace(waiting)
                }

            },
            State {
                name: "WaitingForOtherToAccept"

                StateChangeScript {
                    script: stack.replace(waiting)
                }

            },
            State {
                name: "WaitingForMac"

                StateChangeScript {
                    script: stack.replace(waiting)
                }

            },
            State {
                name: "Success"

                StateChangeScript {
                    script: stack.replace(success)
                }

            },
            State {
                name: "Failed"

                StateChangeScript {
                    script: stack.replace(failed)
                }

            }
        ]
    }

}
