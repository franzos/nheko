// SPDX-FileCopyrightText: 2021 Nheko Contributors
// SPDX-FileCopyrightText: 2022 Nheko Contributors
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QColor>
#include <QPalette>

namespace ui {
// Default font size.
const int FontSize = 16;

// Default avatar size. Width and height.
const int AvatarSize = 40;

enum class ButtonPreset
{
    FlatPreset,
    CheckablePreset
};

enum class RippleStyle
{
    CenteredRipple,
    PositionedRipple,
    NoRipple
};

enum class OverlayStyle
{
    NoOverlay,
    TintedOverlay,
    GrayOverlay
};

enum class Role
{
    Default,
    Primary,
    Secondary
};

enum class ButtonIconPlacement
{
    LeftIcon,
    RightIcon
};

enum class ProgressType
{
    DeterminateProgress,
    IndeterminateProgress
};

} // namespace ui

class Theme : public QPalette
{
    Q_GADGET
    Q_PROPERTY(QColor sidebarBackground READ sidebarBackground CONSTANT)
    Q_PROPERTY(QColor alternateButton READ alternateButton CONSTANT)
    Q_PROPERTY(QColor separator READ separator CONSTANT)
    Q_PROPERTY(QColor red READ red CONSTANT)
    Q_PROPERTY(QColor error READ error CONSTANT)
    Q_PROPERTY(QColor orange READ orange CONSTANT)
public:
    Theme() {}
    explicit Theme(QStringView theme);
    static QPalette paletteFromTheme(QStringView theme);

    QColor sidebarBackground() const { return sidebarBackground_; }
    QColor alternateButton() const { return alternateButton_; }
    QColor separator() const { return separator_; }
    QColor red() const { return red_; }
    QColor error() const { return QColor(0xdd, 0x3d, 0x3d); }
    QColor orange() const { return orange_; }

private:
    QColor sidebarBackground_, separator_, red_, orange_, alternateButton_;
};
