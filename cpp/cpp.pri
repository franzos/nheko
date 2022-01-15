QT += network qml quick quickcontrols2 widgets
CONFIG += c++17

SOURCES += \
    $$PWD/MatrixClient.cpp \
    $$PWD/RoomListItem.cpp \
    $$PWD/RoomListModel.cpp \
    $$PWD/TimelineModel.cpp \
    $$PWD/TimelineItem.cpp \
    $$PWD/main.cpp

HEADERS += \
    $$PWD/MatrixClient.h \
    $$PWD/RoomListItem.h  \
    $$PWD/TimelineModel.h \
    $$PWD/TimelineItem.h \
    $$PWD/RoomListModel.h

LIBS += -lmatrix-client-library
LIBS += -lmatrix_client
