#pragma once

#include <QtGlobal>

// -------------------------------------------------------------------------- ANDROID Material Colors definitions
// Material Theme = Light (to change it go to the `MainLib.qml` and update the `Material.theme: Light` to `Dark` or `System`)
// https://doc.qt.io/qt-5/qtquickcontrols2-material.html
#define ANDROID_MATERIAL_ACCENT                     "#0198b7"
#define ANDROID_MATERIAL_PRIMARY                    "#0198b7"
#define ANDROID_MATERIAL_PRIMARY_FOREGROUND         "#fff"
#define ANDROID_MATERIAL_FOREGROUND                 ""          // not used yet anywhere
#define ANDROID_MATERIAL_BACKGROUND                 ""          // not used yet anywhere
// --------------------------------------------------------------------------------------------------------------
#define DEFAULT_SERVER      "https://matrix.pantherx.org"
#define ALLOW_SERVER_CHANGE true                              //                       

// --- Push notification configuration --------------------------------------------------------------------------
#define PUSH_URL ""

#if defined(Q_OS_ANDROID)
#define APPLICATION_ID ""
#elif defined(Q_OS_IOS)
#define APPLICATION_ID ""
#endif
#define APPLICATION_NAME "MatrixClient"
