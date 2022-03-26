QT += network qml quick quickcontrols2 widgets quickwidgets
CONFIG += c++17

SOURCES += \
    $$PWD/GlobalObject.cpp \
    $$PWD/QmlInterface.cpp \
    $$PWD/MatrixQmlApplicationEngine.cpp \
    $$PWD/MatrixQuickView.cpp \
    $$PWD/RoomListItem.cpp \
    $$PWD/RoomListItem.h  \
    $$PWD/RoomListModel.cpp \
    $$PWD/Theme.cpp \
    $$PWD/TimelineModel.cpp \
    $$PWD/TimelineItem.cpp

HEADERS += \
    $$PWD/GlobalObject.h \
    $$PWD/QmlInterface.h \
    $$PWD/MatrixQmlApplicationEngine.h \
    $$PWD/MatrixQuickView.h \
    $$PWD/RoomListItem.h  \
    $$PWD/Theme.h \
    $$PWD/TimelineModel.h \
    $$PWD/TimelineItem.h \
    $$PWD/RoomListModel.h
