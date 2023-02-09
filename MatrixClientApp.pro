message("Building MatrixClient GUI Application")
VERSION = 0.1.46
TEMPLATE = app
TARGET = matrix-client

include(configurations/configurations.pri)
include(app/app.pri)
include(qml/qml.pri)
include(vendor/SCodes/SCodes.pri)

QMAKE_TARGET_BUNDLE_PREFIX = org.pantherx
QMAKE_BUNDLE = matrix-client
