QT += network qml quick quickcontrols2 widgets
CONFIG += c++17

SOURCES += \
    $$PWD/MatrixClient.cpp \
    $$PWD/RoomListItem.cpp \
    $$PWD/RoomListModel.cpp \
    $$PWD/main.cpp

HEADERS += \
    $$PWD/MatrixClient.h \
    $$PWD/RoomListItem.h  \
    $$PWD/RoomListModel.h

LIBS += -lmatrix-client-library
LIBS += -lmatrix_client
