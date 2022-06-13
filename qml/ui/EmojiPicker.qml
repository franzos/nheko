// SPDX-FileCopyrightText: 2021 Nheko Contributors
// SPDX-FileCopyrightText: 2022 Nheko Contributors
//
// SPDX-License-Identifier: GPL-3.0-or-later

import "../"
import QtGraphicalEffects 1.0
import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Window 2.15
import GlobalObject 1.0
import EmojiModel 1.0
import Emoji 1.0

Menu {
    id: emojiPopup

    property var callback
    property var colors
    property alias model: gridView.model
    property var textArea
    property string emojiCategory: "people"
    property real highlightHue: GlobalObject.colors.highlight.hslHue
    property real highlightSat: GlobalObject.colors.highlight.hslSaturation
    property real highlightLight: GlobalObject.colors.highlight.hslLightness

    function show(showAt, callback) {
        console.debug("Showing emojiPicker");
        emojiPopup.callback = callback;
        popup(showAt ? showAt : null);
    }

    margins: 0
    bottomPadding: 1
    leftPadding: 1
    rightPadding: 1
    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    //height: columnView.implicitHeight + 4
    //width: columnView.implicitWidth
    width: 7 * 52 + 20

    Rectangle {
        color: GlobalObject.colors.window
        height: columnView.implicitHeight + 4
        width: 7 * 52 + 20

        ColumnLayout {
            id: columnView

            spacing: 0
            anchors.leftMargin: 3
            anchors.rightMargin: 3
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.topMargin: 2

            // Search field
            TextField {
                id: emojiSearch

                Layout.topMargin: 3
                Layout.preferredWidth: 7 * 52 + 20 - 6
                palette: GlobalObject.colors
                background: null
                placeholderTextColor: GlobalObject.colors.buttonText
                color: GlobalObject.colors.text
                placeholderText: qsTr("Search")
                selectByMouse: true
                rightPadding: clearSearch.width
                onTextChanged: searchTimer.restart()
                onVisibleChanged: {
                    if (visible)
                        forceActiveFocus();
                    else
                        clear();
                }

                Timer {
                    id: searchTimer

                    interval: 350 // tweak as needed?
                    onTriggered: {
                        emojiPopup.model.searchString = emojiSearch.text;
                        emojiPopup.model.category = Emoji.Category.Search;
                    }
                }

                ToolButton {
                    id: clearSearch

                    visible: emojiSearch.text !== ''
                    icon.source: "image://colorimage/:/images/round-remove-button.svg?" + (clearSearch.hovered ? GlobalObject.colors.highlight : GlobalObject.colors.buttonText)
                    focusPolicy: Qt.NoFocus
                    onClicked: emojiSearch.clear()
                    hoverEnabled: true
                    background: null

                    anchors {
                        verticalCenter: parent.verticalCenter
                        right: parent.right
                    }
                    // clear the default hover effects.

                    Image {
                        height: parent.height - 2 * 4
                        width: height
                        source: "image://colorimage/:/images/round-remove-button.svg?" + (clearSearch.hovered ? GlobalObject.colors.highlight : GlobalObject.colors.buttonText)

                        anchors {
                            verticalCenter: parent.verticalCenter
                            right: parent.right
                            margins: 4
                        }

                    }

                }

            }

            // emoji grid
            GridView {
                id: gridView

                Layout.preferredHeight: cellHeight * 5
                Layout.preferredWidth: 7 * 52 + 20
                Layout.leftMargin: 4
                cellWidth: 52
                cellHeight: 52
                boundsBehavior: Flickable.StopAtBounds
                clip: true
                currentIndex: -1 // prevent sorting from stealing focus
                cacheBuffer: 500

                ScrollHelper {
                    flickable: parent
                    anchors.fill: parent
                    enabled: true //!Settings.mobileMode
                }

                // Individual emoji
                delegate: AbstractButton {
                    width: 48
                    height: 48
                    hoverEnabled: true
                    ToolTip.text: model.shortName
                    ToolTip.visible: hovered
                    // TODO: maybe add favorites at some point?
                    onClicked: {
                        console.debug("Picked " + model.unicode);
                        emojiPopup.close();
                        callback(model.unicode);
                    }

                    // give the emoji a little oomf
                    // DropShadow {
                    //     width: parent.width
                    //     height: parent.height
                    //     horizontalOffset: 3
                    //     verticalOffset: 3
                    //     radius: 8
                    //     samples: 17
                    //     color: "#80000000"
                    //     source: parent.contentItem
                    // }

                    contentItem: Text {
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        // font.family: Settings.emojiFont
                        font.pixelSize: 36
                        text: model.unicode.replace('\ufe0f', '')
                        color: GlobalObject.colors.text
                    }

                    background: Rectangle {
                        anchors.fill: parent
                        color: hovered ? GlobalObject.colors.highlight : 'transparent'
                        radius: 5
                    }

                }

                ScrollBar.vertical: ScrollBar {
                    id: emojiScroll
                }

            }

            // Separator
            Rectangle {
                visible: emojiSearch.text === ''
                Layout.fillWidth: true
                Layout.preferredHeight: 1
                color: GlobalObject.theme.separator
            }

            // Category picker row
            RowLayout {
                visible: emojiSearch.text === ''
                Layout.bottomMargin: 0
                Layout.preferredHeight: 42
                implicitHeight: 42
                Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom

                // Display the normal categories
                Repeater {

                    model: ListModel {
                        // TODO: Would like to get 'simple' icons for the categories
                        ListElement {
                            image: ":/emoji/emoji-categories/people.svg"
                            category: Emoji.Category.People
                        }

                        ListElement {
                            image: ":/emoji/emoji-categories/nature.svg"
                            category: Emoji.Category.Nature
                        }

                        ListElement {
                            image: ":/emoji/emoji-categories/foods.svg"
                            category: Emoji.Category.Food
                        }

                        ListElement {
                            image: ":/emoji/emoji-categories/activity.svg"
                            category: Emoji.Category.Activity
                        }

                        ListElement {
                            image: ":/emoji/emoji-categories/travel.svg"
                            category: Emoji.Category.Travel
                        }

                        ListElement {
                            image: ":/emoji/emoji-categories/objects.svg"
                            category: Emoji.Category.Objects
                        }

                        ListElement {
                            image: ":/emoji/emoji-categories/symbols.svg"
                            category: Emoji.Category.Symbols
                        }

                        ListElement {
                            image: ":/emoji/emoji-categories/flags.svg"
                            category: Emoji.Category.Flags
                        }

                    }

                    delegate: AbstractButton {
                        Layout.preferredWidth: 36
                        Layout.preferredHeight: 36
                        hoverEnabled: true
                        ToolTip.text: {
                            switch (model.category) {
                            case Emoji.Category.People:
                                return qsTr('People');
                            case Emoji.Category.Nature:
                                return qsTr('Nature');
                            case Emoji.Category.Food:
                                return qsTr('Food');
                            case Emoji.Category.Activity:
                                return qsTr('Activity');
                            case Emoji.Category.Travel:
                                return qsTr('Travel');
                            case Emoji.Category.Objects:
                                return qsTr('Objects');
                            case Emoji.Category.Symbols:
                                return qsTr('Symbols');
                            case Emoji.Category.Flags:
                                return qsTr('Flags');
                            }
                        }
                        ToolTip.visible: hovered
                        onClicked: {
                            // emojiPopup.model.category = model.category;
                            gridView.positionViewAtIndex(emojiPopup.model.sourceModel.categoryToIndex(model.category), GridView.Beginning);
                        }

                        MouseArea {
                            id: mouseArea

                            anchors.fill: parent
                            onPressed: mouse.accepted = false
                            cursorShape: Qt.PointingHandCursor
                        }

                        contentItem: Image {
                            horizontalAlignment: Image.AlignHCenter
                            verticalAlignment: Image.AlignVCenter
                            fillMode: Image.Pad
                            height: 32
                            width: 32
                            smooth: true
                            mipmap: true
                            sourceSize.width: 32 * Screen.devicePixelRatio
                            sourceSize.height: 32 * Screen.devicePixelRatio
                            source: "image://colorimage/" + model.image + "?" + GlobalObject.colors.highlight// (hovered ? GlobalObject.colors.highlight : GlobalObject.colors.buttonText)
                        }

                        // background: Rectangle {
                        //     anchors.fill: parent
                        //     color: emojiPopup.model.category === model.category ? Qt.hsla(highlightHue, highlightSat, highlightLight, 0.2) : 'transparent'
                        //     radius: 5
                        //     border.color: emojiPopup.model.category === model.category ? GlobalObject.colors.highlight : 'transparent'
                        // }

                    }

                }

            }

        }

    }

}


// import QtQuick 2.15
// import QtQuick.Controls 2.5
// import QtQuick.Layouts 1.3
// import EmojiModel 1.0

// Item {
//     id: container
//     property var editor
//     property EmojiModel model
//     property var categories: ['Smileys & Emotion', 'People & Body', 'Animals & Nature',
//         'Food & Drink', 'Activities', 'Travel & Places', 'Objects', 'Symbols', 'Flags']
//     property var searchModel: ListModel {}
//     property bool searchMode: false
//     property int skinColor: -1
//     function changeSkinColor(index) {
//         if (index !== skinColors.current) {
//             skinColors.itemAt(skinColors.current + 1).scale = 0.6
//             skinColors.itemAt(index + 1).scale = 1
//             skinColors.current = index
//             container.skinColor = index
//         }
//     }
//     function refreshSearchModel() {
//         searchModel.clear()
//         var searchResult = model.search(searchField.text, skinColor)
//         for (var i = 0; i < searchResult.length; ++i) {
//             searchModel.append({path: searchResult[i]})
//         }
//     }
//     ColumnLayout {
//         anchors.fill: parent
//         RowLayout {
//             id: categoriesRow
//             Layout.preferredWidth: parent.width - 15
//             Layout.preferredHeight: 35
//             Layout.leftMargin: 5
//             Layout.alignment: Qt.AlignCenter
//             spacing: searchField.widthSize > 0 ? 7 : 17
//             clip: true
//             Image {
//                 id: searchIcon
//                 source: 'qrc:/emoji/icons/search.svg'
//                 sourceSize: Qt.size(21, 21)
//                 visible: !container.searchMode
//                 MouseArea {
//                     anchors.fill: parent
//                     cursorShape: Qt.PointingHandCursor
//                     onClicked: {
//                         container.searchMode = true
//                         searchField.widthSize = categoriesRow.width - 25
//                         list.model = 1
//                         searchField.focus = true
//                     }
//                 }
//             }
//             Image {
//                 id: closeIcon
//                 source: 'qrc:/emoji/icons/close.svg'
//                 sourceSize: Qt.size(21, 21)
//                 visible: container.searchMode
//                 MouseArea {
//                     anchors.fill: parent
//                     cursorShape: Qt.PointingHandCursor
//                     onClicked: {
//                         container.searchMode = false
//                         searchField.widthSize = 0
//                         list.model = container.categories
//                         searchField.clear()
//                     }
//                 }
//             }
//             TextField {
//                 id: searchField
//                 property int widthSize: 0
//                 Layout.preferredWidth: widthSize
//                 Layout.preferredHeight: 28
//                 visible: widthSize > 0 ? true : false
//                 placeholderText: 'Search Emoji'
//                 Behavior on widthSize {
//                     NumberAnimation {
//                         duration: 400
//                     }
//                 }
//                 background: Rectangle {
//                     radius: 10
//                     border.color: '#68c8ed'
//                 }
//                 onTextChanged: {
//                     text.length > 0 ? container.refreshSearchModel() : container.searchModel.clear()
//                 }
//             }
//             Repeater {
//                 id: cateIcons
//                 property var blackSvg: ['emoji-smiley.svg', 'emoji-people.svg', 'emoji-animal.svg', 'emoji-food.svg',
//                     'emoji-activity.svg', 'emoji-travel.svg', 'emoji-object.svg', 'emoji-symbol.svg', 'emoji-flag.svg']
//                 property var blueSvg: ['emoji-smiley-blue.svg', 'emoji-people-blue.svg', 'emoji-animal-blue.svg',
//                     'emoji-food-blue.svg', 'emoji-activity-blue.svg', 'emoji-travel-blue.svg', 'emoji-object-blue.svg',
//                     'emoji-symbol-blue.svg', 'emoji-flag-blue.svg']
//                 property int current: 0
//                 model: 9
//                 delegate: Image {
//                     id: icon
//                     source: 'qrc:/emoji/icons/' + cateIcons.blackSvg[index]
//                     sourceSize: Qt.size(20, 20)
//                     MouseArea {
//                         anchors.fill: parent
//                         cursorShape: Qt.PointingHandCursor
//                         onClicked: {
//                             if (cateIcons.current !== index) {
//                                 icon.source = 'qrc:/emoji/icons/' + cateIcons.blueSvg[index]
//                                 cateIcons.itemAt(cateIcons.current).source = 'qrc:/emoji/icons/' + cateIcons.blackSvg[cateIcons.current]
//                                 cateIcons.current = index
//                             }
//                             list.positionViewAtIndex(index, ListView.Beginning)
//                         }
//                     }
//                 }
//                 Component.onCompleted: {
//                     itemAt(0).source = 'qrc:/emoji/icons/' + cateIcons.blueSvg[0]
//                 }
//             }
//         }
//         ListView {
//             id: list
//             Layout.fillWidth: true
//             Layout.fillHeight: true
//             model: container.categories
//             spacing: 30
//             topMargin: 7
//             bottomMargin: 7
//             leftMargin: 12
//             clip: true
//             delegate: GridLayout {
//                 id: grid
//                 property string category: container.searchMode ? 'Search Result' : modelData
//                 property int columnCount: list.width / 50
//                 property int sc: grid.category === 'People & Body' ? container.skinColor : -1
//                 columns: columnCount
//                 columnSpacing: 8
//                 Text {
//                     Layout.fillWidth: true
//                     Layout.preferredHeight: 20
//                     text: grid.category
//                     color: Qt.rgba(0, 0, 0, 0.5)
//                     font.pixelSize: 15
//                     horizontalAlignment: Text.AlignLeft
//                     leftPadding: 6
//                     Layout.columnSpan: grid.columnCount != 0 ? grid.columnCount : 1
//                     Layout.bottomMargin: 8
//                 }
//                 Repeater {
//                     model: container.searchMode ? container.searchModel : container.model.count(grid.category)
//                     delegate: Rectangle  {
//                         property alias es: emojiSvg
//                         Layout.preferredWidth: 40
//                         Layout.preferredHeight: 40
//                         radius: 40
//                         color: mouseArea.containsMouse ? '#e6e6e6' : '#ffffff'
//                         Image {
//                             id: emojiSvg
//                             source: container.searchMode ? path : container.model.path(grid.category, index, grid.sc)
//                             sourceSize: Qt.size(30, 30)
//                             anchors.centerIn: parent
//                             asynchronous: true
//                         }
//                         MouseArea {
//                             id: mouseArea
//                             anchors.fill: parent
//                             hoverEnabled: true
//                             cursorShape: Qt.PointingHandCursor
//                             onClicked: {
//                                 var tag = "<img src = '%1' width = '20' height = '20' align = 'top'>"
//                                 container.editor.insert(container.editor.cursorPosition, tag.arg(emojiSvg.source))
//                             }
//                         }
//                     }
//                 }
//             }
//             onContentYChanged: {
//                 var index = list.indexAt(0, contentY + 15)
//                 if (index !== -1 && index !== cateIcons.current) {
//                     cateIcons.itemAt(index).source = 'qrc:/emoji/icons/' + cateIcons.blueSvg[index]
//                     cateIcons.itemAt(cateIcons.current).source = 'qrc:/emoji/icons/' + cateIcons.blackSvg[cateIcons.current]
//                     cateIcons.current = index
//                 }
//             }
//         }
//         RowLayout {
//             Layout.preferredHeight: 35
//             Layout.alignment: Qt.AlignCenter
//             spacing: 10
//             Repeater {
//                 id: skinColors
//                 property var colors: ['#ffb84d', '#ffdab3', '#d2a479', '#ac7139', '#734b26', '#26190d']
//                 property int current: -1
//                 model: 6
//                 delegate: Rectangle {
//                     id: colorRect
//                     Layout.preferredWidth: 30
//                     Layout.preferredHeight: 30
//                     Layout.bottomMargin: 3
//                     radius: 30
//                     scale: 0.65
//                     color: skinColors.colors[index]
//                     Behavior on scale {
//                         NumberAnimation {
//                             duration: 100
//                         }
//                     }
//                     MouseArea {
//                         anchors.fill: parent
//                         cursorShape: Qt.PointingHandCursor
//                         onClicked: {
//                             container.changeSkinColor(index - 1)
//                             if (container.searchMode) {
//                                 container.refreshSearchModel();
//                             }
//                         }
//                     }
//                 }
//                 Component.onCompleted: {
//                     itemAt(0).scale = 1
//                 }
//             }
//         }
//     }
// }
