QT += network qml quick quickcontrols2 widgets quickwidgets
CONFIG += c++17

SOURCES += \
    $$PWD/../cpp/MatrixClient.cpp \
    $$PWD/../cpp/RoomListItem.cpp \
    $$PWD/../cpp/RoomListItem.h  \
    $$PWD/../cpp/RoomListModel.cpp \
    $$PWD/../cpp/TimelineModel.cpp \
    $$PWD/../cpp/TimelineItem.cpp\
    $$PWD/main.cpp

HEADERS += \
    $$PWD/../cpp/MatrixClient.h \
    $$PWD/../cpp/RoomListItem.h  \
    $$PWD/../cpp/TimelineModel.h \
    $$PWD/../cpp/TimelineItem.h \
    $$PWD/../cpp/RoomListModel.h