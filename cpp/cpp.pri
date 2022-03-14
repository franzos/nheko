QT += network qml quick quickcontrols2 widgets quickwidgets
CONFIG += c++17

SOURCES += \
    $$PWD/MatrixClient.cpp \
    $$PWD/RoomListItem.cpp \
    $$PWD/RoomListItem.h  \
    $$PWD/RoomListModel.cpp \
    $$PWD/TimelineModel.cpp \
    $$PWD/TimelineItem.cpp

HEADERS += \
    $$PWD/MatrixClient.h \
    $$PWD/RoomListItem.h  \
    $$PWD/TimelineModel.h \
    $$PWD/TimelineItem.h \
    $$PWD/RoomListModel.h
