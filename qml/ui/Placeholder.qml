// SPDX-FileCopyrightText: 2021 Nheko Contributors
// SPDX-FileCopyrightText: 2022 Nheko Contributors
//
// SPDX-License-Identifier: GPL-3.0-or-later

import GlobalObject 1.0

MatrixText {
    required property string typeString

    text: qsTr("unimplemented event: ") + typeString
//    width: parent.width
    color: GlobalObject.inactiveColors.text
}
