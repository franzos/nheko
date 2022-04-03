QT += network qml quick quickcontrols2 widgets quickwidgets
CONFIG += c++17

SOURCES += \
    $$PWD/QmlInterface.cpp \
    $$PWD/MatrixQmlApplicationEngine.cpp \
    $$PWD/MatrixQuickView.cpp \
    $$PWD/mydevice.cpp \
    $$PWD/RoomListItem.cpp \
    $$PWD/RoomListItem.h  \
    $$PWD/RoomListModel.cpp \
    $$PWD/TimelineModel.cpp \
    $$PWD/TimelineItem.cpp

HEADERS += \
    $$PWD/QmlInterface.h \
    $$PWD/MatrixQmlApplicationEngine.h \
    $$PWD/MatrixQuickView.h \
    $$PWD/mydevice.h \
    $$PWD/RoomListItem.h  \
    $$PWD/TimelineModel.h \
    $$PWD/TimelineItem.h \
    $$PWD/RoomListModel.h
