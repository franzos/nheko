message("Building MatrixClient GUI Application")
VERSION = 1.0.0
TEMPLATE = app
TARGET = matrix-client

include(configurations/configurations.pri)
include(app/app.pri)
include(qml/qml.pri)

QMAKE_TARGET_BUNDLE_PREFIX = org.pantherx
QMAKE_BUNDLE = matrix-client

target.path = /usr/bin/
target.files = matrix-client
INSTALLS += target