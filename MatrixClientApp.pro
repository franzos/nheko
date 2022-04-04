message("Building MatrixClient GUI Application")
VERSION = 1.0.0
TEMPLATE = app
TARGET = MatrixClient

include(configurations/configurations.pri)
include(qml/qml.pri)
include(app/app.pri)

QMAKE_TARGET_BUNDLE_PREFIX = org.pantherx
QMAKE_BUNDLE = MatrixClient

MatrixClient.path = /usr/bin/
MatrixClient.files = MatrixClient
INSTALLS += MatrixClient 