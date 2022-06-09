QT += network qml quick quickcontrols2 widgets quickwidgets svg
CONFIG += c++17

SOURCES += \
    $$PWD/../cpp/Clipboard.cpp \
    $$PWD/../cpp/ColorImageProvider.cpp \
    $$PWD/../cpp/GlobalObject.cpp \
    $$PWD/../cpp/QmlInterface.cpp \
    $$PWD/../cpp/MatrixQmlApplicationEngine.cpp \
    $$PWD/../cpp/MatrixQuickView.cpp \
    $$PWD/../cpp/mydevice.cpp \
    $$PWD/../cpp/RoomListItem.cpp \
    $$PWD/../cpp/RoomListItem.h  \
    $$PWD/../cpp/RoomListModel.cpp \
    $$PWD/../cpp/Theme.cpp \
    $$PWD/../cpp/TimelineModel.cpp \
    $$PWD/../cpp/TimelineItem.cpp\
    $$PWD/../cpp/ui/emoji/emojimodel.cpp \
    $$PWD/../cpp/ui/DelegateChooser.cpp \
    $$PWD/../cpp/ui/NhekoCursorShape.cpp \
    $$PWD/main.cpp

HEADERS += \
    $$PWD/../cpp/Application.h\
    $$PWD/../cpp/Clipboard.h \
    $$PWD/../cpp/ColorImageProvider.cpp \
    $$PWD/../cpp/Configuration.h \
    $$PWD/../cpp/GlobalObject.h \
    $$PWD/../cpp/QmlInterface.h \
    $$PWD/../cpp/MatrixQmlApplicationEngine.h \
    $$PWD/../cpp/MatrixQuickView.h \
    $$PWD/../cpp/mydevice.h \
    $$PWD/../cpp/RoomListItem.h  \
    $$PWD/../cpp/RoomListModel.h \
    $$PWD/../cpp/Theme.h \
    $$PWD/../cpp/TimelineModel.h \
    $$PWD/../cpp/TimelineItem.h  \
    $$PWD/../cpp/ui/emoji/emojimodel.h \
    $$PWD/../cpp/ui/DelegateChooser.h \
    $$PWD/../cpp/ui/NhekoCursorShape.h 
