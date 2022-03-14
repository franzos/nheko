message("Building MatrixClient GUI Library")
VERSION = 1.0.0
TEMPLATE = lib
TARGET = matrix-client-gui-library

include(configurations/configurations.pri)
include(cpp/cpp.pri)
include(qml/qml.pri)

headers.path = /usr/include/matrix-client-gui-library
headers.files = cpp/MatrixClient.h cpp/RoomListItem.h cpp/RoomListModel.h cpp/TimelineItem.h cpp/TimelineModel.h

target.path = /usr/lib/
INSTALLS += target headers