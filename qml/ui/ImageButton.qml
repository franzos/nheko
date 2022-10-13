// SPDX-FileCopyrightText: 2021 Nheko Contributors
// SPDX-FileCopyrightText: 2022 Nheko Contributors
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.3
import QtQuick.Controls 2.3
import GlobalObject 1.0
import CursorShape 1.0

AbstractButton {
    id: button

    property alias cursor: mouseArea.cursorShape
    property string image: undefined
    property color highlightColor: GlobalObject.colors.highlight
    property color buttonTextColor: GlobalObject.colors.buttonText
    property bool changeColorOnHover: true
    property bool ripple: true

    focusPolicy: Qt.NoFocus
    width: 16
    height: 16

    Image {
        id: buttonImg

        // Workaround, can't get icon.source working for now...
        anchors.fill: parent
        source: image != "" ? ("image://colorimage/" + image + "?" + ((button.hovered && changeColorOnHover) ? highlightColor : buttonTextColor)) : ""
        sourceSize.height: button.height
        sourceSize.width: button.width
        fillMode: Image.PreserveAspectFit
    }

    CursorShape {
        id: mouseArea

        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
    }

    Ripple {
        enabled: button.ripple
        color: Qt.rgba(buttonTextColor.r, buttonTextColor.g, buttonTextColor.b, 0.5)
    }

}
