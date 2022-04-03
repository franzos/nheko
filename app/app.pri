QT += network qml quick quickcontrols2 widgets quickwidgets
CONFIG += c++17

SOURCES += \
    $$PWD/../cpp/QmlInterface.cpp \
    $$PWD/../cpp/MatrixQmlApplicationEngine.cpp \
    $$PWD/../cpp/MatrixQuickView.cpp \
    $$PWD/../cpp/mydevice.cpp \
    $$PWD/../cpp/RoomListItem.cpp \
    $$PWD/../cpp/RoomListItem.h  \
    $$PWD/../cpp/RoomListModel.cpp \
    $$PWD/../cpp/TimelineModel.cpp \
    $$PWD/../cpp/TimelineItem.cpp\
    $$PWD/main.cpp

HEADERS += \
    $$PWD/../cpp/QmlInterface.h \
    $$PWD/../cpp/MatrixQmlApplicationEngine.h \
    $$PWD/../cpp/MatrixQuickView.h \
    $$PWD/../cpp/mydevice.h \
    $$PWD/../cpp/RoomListItem.h  \
    $$PWD/../cpp/TimelineModel.h \
    $$PWD/../cpp/TimelineItem.h \
    $$PWD/../cpp/RoomListModel.h