QT += network qml quick quickcontrols2 widgets quickwidgets svg dbus
CONFIG += c++17

SOURCES += \
    $$PWD/AvatarProvider.cpp \
    $$PWD/Clipboard.cpp \
    $$PWD/ColorImageProvider.cpp \
    $$PWD/CompletionProxyModel.cpp \
    $$PWD/GlobalObject.cpp \
    $$PWD/QmlInterface.cpp \
    $$PWD/MatrixQmlApplicationEngine.cpp \
    $$PWD/MatrixQuickView.cpp \
    $$PWD/MemberList.cpp \
    $$PWD/mydevice.cpp \
    $$PWD/Reaction.cpp \
    $$PWD/ReadReceiptsModel.cpp \
    $$PWD/RoomListItem.cpp \
    $$PWD/RoomListItem.h  \
    $$PWD/RoomListModel.cpp \
    $$PWD/RoomsModel.cpp \
    $$PWD/Theme.cpp \
    $$PWD/TimelineModel.cpp \
    $$PWD/TimelineItem.cpp \
    \
    $$PWD/notifications/Manager.cpp \
    $$PWD/notifications/ManagerLinux.cpp \
    \
    $$PWD/ui/emoji/EmojiModel.cpp \
    $$PWD/ui/emoji/Provider.cpp \
    $$PWD/ui/CombinedImagePackModel.cpp \
    $$PWD/ui/DelegateChooser.cpp \
    $$PWD/ui/InputBar.cpp \
    $$PWD/ui/NhekoCursorShape.cpp 
    

HEADERS += \
    $$PWD/Application.h\
    $$PWD/AvatarProvider.h \
    $$PWD/Clipboard.h\
    $$PWD/ColorImageProvider.h \
    $$PWD/CompletionModelRoles.h \
    $$PWD/CompletionProxyModel.h \
    $$PWD/GlobalObject.h \
    $$PWD/QmlInterface.h \
    $$PWD/MatrixQmlApplicationEngine.h \
    $$PWD/MatrixQuickView.h \
    $$PWD/MemberList.h \
    $$PWD/mydevice.h \
    $$PWD/Reaction.h \
    $$PWD/ReadReceiptsModel.h \
    $$PWD/RoomListItem.h  \
    $$PWD/RoomListModel.h \
    $$PWD/RoomsModel.h \
    $$PWD/Theme.h \
    $$PWD/TimelineModel.h \
    $$PWD/TimelineItem.h \
    \
    $$PWD/notifications/Manager.h \
    \
    $$PWD/ui/emoji/EmojiModel.h \
    $$PWD/ui/emoji/Provider.h \
    $$PWD/ui/CombinedImagePackModel.h \
    $$PWD/ui/DelegateChooser.h \
    $$PWD/ui/InputBar.h \
    $$PWD/ui/NhekoCursorShape.h
