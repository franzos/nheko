VERSION = 1.0.0
TARGET = MatrixClient

QMAKE_TARGET_BUNDLE_PREFIX = org.pantherx
QMAKE_BUNDLE = MatrixClient

include(configurations/configurations.pri)
include(cpp/cpp.pri)
include(qml/qml.pri)
