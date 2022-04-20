QT += network qml quick quickcontrols2 widgets quickwidgets
CONFIG += c++17

SOURCES += \
    $$PWD/ColorImageProvider.cpp \
    $$PWD/GlobalObject.cpp \
    $$PWD/QmlInterface.cpp \
    $$PWD/MatrixQmlApplicationEngine.cpp \
    $$PWD/MatrixQuickView.cpp \
    $$PWD/mydevice.cpp \
    $$PWD/RoomListItem.cpp \
    $$PWD/RoomListItem.h  \
    $$PWD/RoomListModel.cpp \
    $$PWD/Theme.cpp \
    $$PWD/TimelineModel.cpp \
    $$PWD/TimelineItem.cpp \
    $$PWD/ui/NhekoCursorShape.cpp 
    

HEADERS += \
    $$PWD/Application.h\
    $$PWD/ColorImageProvider.h \
    $$PWD/GlobalObject.h \
    $$PWD/QmlInterface.h \
    $$PWD/MatrixQmlApplicationEngine.h \
    $$PWD/MatrixQuickView.h \
    $$PWD/mydevice.h \
    $$PWD/RoomListItem.h  \
    $$PWD/RoomListModel.h \
    $$PWD/Theme.h \
    $$PWD/TimelineModel.h \
    $$PWD/TimelineItem.h \
    $$PWD/ui/NhekoCursorShape.h
