// SPDX-FileCopyrightText: 2021 Nheko Contributors
// SPDX-FileCopyrightText: 2022 Nheko Contributors
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.12
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.2
import QtQuick.Window 2.13
import MtxEvent 1.0
import GlobalObject 1.0
import TimelineModel 1.0
import EmojiModel 1.0
import "ui"

Rectangle {
    id: inputBar
    property var room: timelineModel

    color: GlobalObject.colors.base
    Layout.fillWidth: true
    Layout.preferredHeight: row.implicitHeight
    Layout.minimumHeight: 40
    // property bool showAllButtons: width > 450 || (messageInput.length == 0 && !messageInput.inputMethodComposing)


    // Component {
    //     id: placeCallDialog

    //     PlaceCall {
    //     }

    // }

    // Component {
    //     id: screenShareDialog

    //     ScreenShare {
    //     }

    // }

    RowLayout {
        id: row

        visible: room ? room.permissions.canSend(MtxEvent.TextMessage) : false
        spacing: 0
        anchors.leftMargin: 5
        anchors.rightMargin: anchors.leftMargin
        width: parent.width - anchors.leftMargin - anchors.rightMargin

        ScrollView {
            id: textInput

            Layout.alignment: Qt.AlignVCenter
            Layout.maximumHeight: Window.height / 4
            Layout.minimumHeight: fontMetrics.lineSpacing
            Layout.preferredHeight: contentHeight
            Layout.fillWidth: true

            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

            contentWidth: availableWidth
            TextArea {
                id: messageInput

                property int completerTriggeredAt: 0

                function insertCompletion(completion) {
                    messageInput.remove(completerTriggeredAt, cursorPosition);
                    messageInput.insert(cursorPosition, completion);
                }

                function openCompleter(pos, type) {
                    if (popup.opened) return;
                    completerTriggeredAt = pos;
                    completer.completerName = type;
                    popup.open();
                    completer.completer.setSearchString(messageInput.getText(completerTriggeredAt, cursorPosition)+messageInput.preeditText);
                }

                function positionCursorAtEnd() {
                    cursorPosition = messageInput.length;
                }

                function positionCursorAtStart() {
                    cursorPosition = 0;
                }

                selectByMouse: true
                placeholderText: qsTr("Write a message...")
                placeholderTextColor: GlobalObject.colors.buttonText
                color: GlobalObject.colors.text
                width: textInput.width 
                verticalAlignment: TextEdit.AlignVCenter
                wrapMode: TextEdit.Wrap
                padding: 0
                topPadding: 8
                bottomPadding: 8
                leftPadding: 8//inputBar.showAllButtons? 0 : 8
                focus: true
                property string lastChar
                onTextChanged: {
                    if (room)
                        room.input.updateState(selectionStart, selectionEnd, cursorPosition, text);
                    forceActiveFocus();
                    if (cursorPosition > 0)
                        lastChar = text.charAt(cursorPosition-1)
                    else
                        lastChar = ''
                    if (lastChar == '@') {
                        messageInput.openCompleter(selectionStart-1, "user");
                    } else if (lastChar == ':') {
                        messageInput.openCompleter(selectionStart-1, "emoji");
                    } else if (lastChar == '#') {
                        messageInput.openCompleter(selectionStart-1, "roomAliases");
                    } else if (lastChar == "~") {
                        messageInput.openCompleter(selectionStart-1, "customEmoji");
                    }
                }
                onCursorPositionChanged: {
                    if (!room)
                        return ;

                    room.input.updateState(selectionStart, selectionEnd, cursorPosition, text);
                    if (popup.opened && cursorPosition <= completerTriggeredAt)
                        popup.close();

                    if (popup.opened)
                        completer.completer.setSearchString(messageInput.getText(completerTriggeredAt, cursorPosition)+messageInput.preeditText);

                }
                onPreeditTextChanged: {
                    if (popup.opened)
                        completer.completer.setSearchString(messageInput.getText(completerTriggeredAt, cursorPosition)+messageInput.preeditText);
                }
                onSelectionStartChanged: room.input.updateState(selectionStart, selectionEnd, cursorPosition, text)
                onSelectionEndChanged: room.input.updateState(selectionStart, selectionEnd, cursorPosition, text)
                // Ensure that we get escape key press events first.
                Keys.onShortcutOverride: event.accepted = (popup.opened && (event.key === Qt.Key_Escape || event.key === Qt.Key_Tab || event.key === Qt.Key_Enter || event.key === Qt.Key_Space))
                Keys.onPressed: {
                    if (event.matches(StandardKey.Paste)) {
                        event.accepted = room.input.tryPasteAttachment(false);
                    } else if (event.key == Qt.Key_Space) {
                        // close popup if user enters space after colon
                        if (cursorPosition == completerTriggeredAt + 1)
                            popup.close();

                        if (popup.opened && completer.count <= 0)
                            popup.close();

                    } else if (event.modifiers == Qt.ControlModifier && event.key == Qt.Key_U) {
                        messageInput.clear();
                    } else if (event.modifiers == Qt.ControlModifier && event.key == Qt.Key_P) {
                        messageInput.text = room.input.previousText();
                    } else if (event.modifiers == Qt.ControlModifier && event.key == Qt.Key_N) {
                        messageInput.text = room.input.nextText();
                    } else if (event.key == Qt.Key_Escape && popup.opened) {
                        completer.completerName = "";
                        popup.close();
                        event.accepted = true;
                    } else if (event.matches(StandardKey.SelectAll) && popup.opened) {
                        completer.completerName = "";
                        popup.close();
                    } else if (event.matches(StandardKey.InsertLineSeparator)) {
                        if (popup.opened) popup.close();
                    } else if (event.matches(StandardKey.InsertParagraphSeparator)) {
                        if (popup.opened) {
                            var currentCompletion = completer.currentCompletion();
                            completer.completerName = "";
                            popup.close();
                            if (currentCompletion) {
                                messageInput.insertCompletion(currentCompletion);
                                event.accepted = true;
                                return;
                            }
                        }
                        if (!Qt.inputMethod.visible) {
                            room.input.send();
                            event.accepted = true;
                        }
                    } else if (event.key == Qt.Key_Tab && (event.modifiers == Qt.NoModifier || event.modifiers == Qt.ShiftModifier)) {
                        event.accepted = true;
                        if (popup.opened) {
                            if (event.modifiers & Qt.ShiftModifier)
                                completer.down();
                            else
                                completer.up();
                        } else {
                            var pos = cursorPosition - 1;
                            while (pos > -1) {
                                var t = messageInput.getText(pos, pos + 1);
                                console.log('"' + t + '"');
                                if (t == '@') {
                                    messageInput.openCompleter(pos, "user");
                                    return ;
                                } else if (t == ' ' || t == '\t') {
                                    messageInput.openCompleter(pos + 1, "user");
                                    return ;
                                } else if (t == ':') {
                                    messageInput.openCompleter(pos, "emoji");
                                    return ;
                                } else if (t == '~') {
                                    messageInput.openCompleter(pos, "customEmoji");
                                    return ;
                                }
                                pos = pos - 1;
                            }
                            // At start of input
                            messageInput.openCompleter(0, "user");
                        }
                    } else if (event.key == Qt.Key_Up && popup.opened) {
                        event.accepted = true;
                        completer.up();
                    } else if ((event.key == Qt.Key_Down || event.key == Qt.Key_Backtab) && popup.opened) {
                        event.accepted = true;
                        completer.down();
                    } else if (event.key == Qt.Key_Up && event.modifiers == Qt.NoModifier) {
                        if (cursorPosition == 0) {
                            event.accepted = true;
                            var idx = room.edit ? room.idToIndex(room.edit) + 1 : 0;
                            while (true) {
                                var id = room.indexToId(idx);
                                if (!id || room.getDump(id, "").isEditable) {
                                    room.edit = id;
                                    cursorPosition = 0;
                                    Qt.callLater(positionCursorAtEnd);
                                    break;
                                }
                                idx++;
                            }
                        } else if (positionAt(0, cursorRectangle.y + cursorRectangle.height / 2) === 0) {
                            event.accepted = true;
                            positionCursorAtStart();
                        }
                    } else if (event.key == Qt.Key_Down && event.modifiers == Qt.NoModifier) {
                        if (cursorPosition == messageInput.length && room.edit) {
                            event.accepted = true;
                            var idx = room.idToIndex(room.edit) - 1;
                            while (true) {
                                var id = room.indexToId(idx);
                                if (!id || room.getDump(id, "").isEditable) {
                                    room.edit = id;
                                    Qt.callLater(positionCursorAtStart);
                                    break;
                                }
                                idx--;
                            }
                        } else if (positionAt(width, cursorRectangle.y + cursorRectangle.height / 2) === messageInput.length) {
                            event.accepted = true;
                            positionCursorAtEnd();
                        }
                    }
                }
                background: null

                // Connections {
                //     function onRoomChanged() {
                //         messageInput.clear();
                //         if (room)
                //             messageInput.append(room.input.text);

                //         completer.completerName = "";
                //         messageInput.forceActiveFocus();
                //     }

                //     target: timelineView
                // }

                Connections {
                    function onCompletionClicked(completion) {
                        messageInput.insertCompletion(completion);
                    }

                    target: completer
                }

                Popup {
                    id: popup

                    x: messageInput.positionToRectangle(messageInput.completerTriggeredAt).x
                    y: messageInput.positionToRectangle(messageInput.completerTriggeredAt).y - height

                    background: null
                    padding: 0

                    Completer {
                        anchors.fill: parent
                        id: completer
                        rowMargin: 2
                        rowSpacing: 0
                    }

                    enter: Transition {
                        NumberAnimation {
                            property: "opacity"
                            from: 0
                            to: 1
                            duration: 100
                        }

                    }

                    exit: Transition {
                        NumberAnimation {
                            property: "opacity"
                            from: 1
                            to: 0
                            duration: 100
                        }
                    }
                }

                Connections {
                    function onTextChanged(newText) {
                        messageInput.text = newText;
                        messageInput.cursorPosition = newText.length;
                    }

                    ignoreUnknownSignals: true
                    target: room ? room.input : null
                }

                Connections {
                    function onReplyChanged() {
                        messageInput.forceActiveFocus();
                    }

                    function onEditChanged() {
                        messageInput.forceActiveFocus();
                    }

                    ignoreUnknownSignals: true
                    target: room
                }

                Connections {
                    function onFocusInput() {
                        messageInput.forceActiveFocus();
                    }

                    target: room
                }

                MouseArea {
                    // workaround for wrong cursor shape on some platforms
                    anchors.fill: parent
                    acceptedButtons: Qt.MiddleButton
                    cursorShape: Qt.IBeamCursor
                    onPressed: (mouse) => mouse.accepted = room.input.tryPasteAttachment(true)
                }

            }

        }

        // ImageButton {
        //     id: stickerButton
        //     visible: showAllButtons

        //     Layout.alignment: Qt.AlignRight | Qt.AlignBottom
        //     Layout.margins: 8
        //     hoverEnabled: true
        //     width: 22
        //     height: 22
        //     image: ":/icons/icons/ui/sticky-note-solid.svg"
        //     ToolTip.visible: hovered
        //     ToolTip.text: qsTr("Stickers")
        //     onClicked: stickerPopup.visible ? stickerPopup.close() : stickerPopup.show(stickerButton, room.roomId, function(row) {
        //         room.input.sticker(stickerPopup.model.sourceModel, row);
        //         TimelineManager.focusMessageInput();
        //     })

        //     StickerPicker {
        //         id: stickerPopup

        //         colors: Nheko.colors
        //     }

        // }
        ImageButton {
            // visible: showAllButtons
            Layout.alignment: Qt.AlignBottom
            hoverEnabled: true
            width: 24; height: width
            image: ":/images/attach.svg"
            Layout.margins: 8
            onClicked: !uploadingicon.visible ? room.input.openFileSelection() : {}
            ToolTip.visible: hovered
            ToolTip.text: qsTr("Send a file")
            Rectangle {
                id: uploadingicon
                width: parent.width; height: width
                color: GlobalObject.colors.window
                visible: room && (room.input.uploading || room.input.uploads.length)
                BusyIndicator {
                    width: parent.width; height: width
                    palette.dark: GlobalObject.colors.windowText
                    running: true
                }
            }
        }

        ImageButton {
            id: emojiButton

            Layout.alignment: Qt.AlignRight | Qt.AlignBottom
            Layout.margins: 8
            hoverEnabled: true
            width: 22
            height: 22
            image: ":/images/smile.svg"
            ToolTip.visible: hovered
            ToolTip.text: qsTr("Emoji")
            onClicked: emojiPopup.visible ? emojiPopup.close() : emojiPopup.show(emojiButton, function(emoji) {
                messageInput.insert(messageInput.cursorPosition, emoji);
                room.focusMessageInput();
            })
        }

        ImageButton {
            Layout.alignment: Qt.AlignRight | Qt.AlignBottom
            Layout.margins: 8
            hoverEnabled: true
            width: 22
            height: 22
            image: ":/images/send.svg"
            // Layout.rightMargin: 8
            ToolTip.visible: hovered
            ToolTip.text: qsTr("Send")
            onClicked: {
                room.input.send();
            }
        }

    }

    Text {
        anchors.centerIn: parent
        visible: room ? (!room.permissions.canSend(MtxEvent.TextMessage)) : false
        text: qsTr("You don't have permission to send messages in this room")
        color: GlobalObject.colors.text
    }

}
