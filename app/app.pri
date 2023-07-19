QT += network qml quick quickcontrols2 widgets quickwidgets svg multimedia location
CONFIG += c++17

SOURCES += \
    $$PWD/../cpp/AvatarProvider.cpp \
    $$PWD/../cpp/BlurhashProvider.cpp \
    $$PWD/../cpp/Clipboard.cpp \
    $$PWD/../cpp/ColorImageProvider.cpp \
    $$PWD/../cpp/CompletionProxyModel.cpp \
    $$PWD/../cpp/GlobalObject.cpp \
    $$PWD/../cpp/InviteesModel.cpp \
    $$PWD/../cpp/JdenticonProvider.cpp \
    $$PWD/../cpp/QmlInterface.cpp \
    $$PWD/../cpp/MatrixQmlApplicationEngine.cpp \
    $$PWD/../cpp/MatrixQuickView.cpp \
    $$PWD/../cpp/MemberList.cpp \
    $$PWD/../cpp/MxcImageProvider.cpp \
    $$PWD/../cpp/mydevice.cpp \
    $$PWD/../cpp/Reaction.cpp \
    $$PWD/../cpp/ReadReceiptsModel.cpp \
    $$PWD/../cpp/RoomListItem.cpp \
    $$PWD/../cpp/RoomListModel.cpp \
    $$PWD/../cpp/RoomsModel.cpp \
    $$PWD/../cpp/Theme.cpp \
    $$PWD/../cpp/TimelineModel.cpp \
    $$PWD/../cpp/UsersModel.cpp \
    \
    $$PWD/../cpp/notifications/Manager.cpp \
    $$PWD/../cpp/notifications/ManagerLinux.cpp \
    $$PWD/../cpp/notifications/notificationhandler.cpp \
    $$PWD/../cpp/notifications/firebase/firebaseqtabstractmodule.cpp \
    $$PWD/../cpp/notifications/firebase/firebaseqtapp.cpp \
    $$PWD/../cpp/notifications/firebase/firebaseqtmessaging.cpp \
    \
    $$PWD/../cpp/ui/emoji/EmojiModel.cpp \
    $$PWD/../cpp/ui/emoji/Provider.cpp \
    $$PWD/../cpp/ui/CombinedImagePackModel.cpp \
    $$PWD/../cpp/ui/DelegateChooser.cpp \
    $$PWD/../cpp/ui/HiddenEvents.cpp \
    $$PWD/../cpp/ui/MxcAnimatedImage.cpp \
    $$PWD/../cpp/ui/MxcMediaProxy.cpp \
    $$PWD/../cpp/ui/InputBar.cpp \
    $$PWD/../cpp/ui/NhekoCursorShape.cpp \
    $$PWD/../cpp/ui/RoomSettings.cpp \
    $$PWD/../cpp/ui/UserSettingsModel.cpp \
    $$PWD/main.cpp

HEADERS += \
    $$PWD/../cpp/Application.h\
    $$PWD/../cpp/AvatarProvider.h \
    $$PWD/../cpp/BlurhashProvider.h \
    $$PWD/../cpp/Clipboard.h \
    $$PWD/../cpp/ColorImageProvider.cpp \
    $$PWD/../cpp/Configuration.h \
    $$PWD/../cpp/CompletionModelRoles.h \
    $$PWD/../cpp/CompletionProxyModel.h \
    $$PWD/../cpp/GlobalObject.h \
    $$PWD/../cpp/InviteesModel.h \
    $$PWD/../cpp/jdenticoninterface.h \
    $$PWD/../cpp/JdenticonProvider.h \
    $$PWD/../cpp/QmlInterface.h \
    $$PWD/../cpp/MatrixQmlApplicationEngine.h \
    $$PWD/../cpp/MatrixQuickView.h \
    $$PWD/../cpp/MemberList.h \
    $$PWD/../cpp/MxcImageProvider.h \
    $$PWD/../cpp/mydevice.h \
    $$PWD/../cpp/Reaction.h \
    $$PWD/../cpp/ReadReceiptsModel.h \
    $$PWD/../cpp/RoomListItem.h  \
    $$PWD/../cpp/RoomListModel.h \
    $$PWD/../cpp/RoomsModel.h \
    $$PWD/../cpp/Theme.h \
    $$PWD/../cpp/TimelineModel.h \
    $$PWD/../cpp/UsersModel.cpp \
    \
    $$PWD/../cpp/notifications/Manager.h \
    $$PWD/../cpp/notifications/notificationhandler.h \
    $$PWD/../cpp/notifications/firebase/firebaseqtabstractmodule.h \
    $$PWD/../cpp/notifications/firebase/firebaseqtapp.h \
    $$PWD/../cpp/notifications/firebase/firebaseqtapp_p.h \
    $$PWD/../cpp/notifications/firebase/firebaseqtmessaging.h \
    \
    $$PWD/../cpp/ui/emoji/EmojiModel.h \
    $$PWD/../cpp/ui/emoji/Provider.h \
    $$PWD/../cpp/ui/CombinedImagePackModel.h \
    $$PWD/../cpp/ui/DelegateChooser.h \
    $$PWD/../cpp/ui/HiddenEvents.h \
    $$PWD/../cpp/ui/MxcAnimatedImage.h \
    $$PWD/../cpp/ui/InputBar.h \
    $$PWD/../cpp/ui/MxcMediaProxy.h \
    $$PWD/../cpp/ui/NhekoCursorShape.h \
    $$PWD/../cpp/ui/RoomSettings.h \
    $$PWD/../cpp/ui/UserSettingsModel.h
