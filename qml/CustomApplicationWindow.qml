import QtQuick 2.9
import QtQuick.Window 2.0
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import GlobalObject 1.0

ApplicationWindow {
    Material.theme: Material.System
    Material.primary: GlobalObject.materialColors().primary
    Material.accent: GlobalObject.materialColors().accent
    palette: GlobalObject.colors
    // color: GlobalObject.colors.window
}