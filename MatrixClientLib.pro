message("Building MatrixClient GUI Library")
VERSION = 1.0.0
TEMPLATE = lib
TARGET = matrix-client-gui-library

LIBS += -lmatrix-client-library \
        -lpx-auth-lib-cpp \
        -lspdlog -lcurl -lcoeurl -llmdb \
        -lmatrix_client
        
include(cpp/cpp.pri)
include(qml/qml.pri)

headers.path  = /usr/include/matrix-client-gui-library
headers.files = cpp/Application.h \
                cpp/CompletionModelRoles.h \
                cpp/GlobalObject.h \
                cpp/InviteesModel.h \
                cpp/MemberList.h \
                cpp/Reaction.h \ 
                cpp/RoomListModel.h \
                cpp/Clipboard.h \ 
                cpp/CompletionProxyModel.h \
                cpp/MatrixQmlApplicationEngine.h \
                cpp/mydevice.h \
                cpp/ReadReceiptsModel.h \
                cpp/RoomsModel.h \
                cpp/TimelineModel.h \
                cpp/UsersModel.h \
                cpp/ColorImageProvider.h \
                cpp/Configuration.h \
                cpp/MatrixQuickView.h \
                cpp/QmlInterface.h \
                cpp/RoomListItem.h \
                cpp/Theme.h

ui_headers.path  = /usr/include/matrix-client-gui-library/ui
ui_headers.files =  cpp/ui/NhekoCursorShape.h \
                    cpp/ui/CombinedImagePackModel.h \
                    cpp/ui/DelegateChooser.h \
                    cpp/ui/InputBar.h \
                    cpp/ui/NhekoCursorShape.h

notification_headers.path  = /usr/include/matrix-client-gui-library/notifications
notification_headers.files = cpp/notifications/Manager.h \
                             cpp/notifications/ManagerLinux.h

target.path = /usr/lib/

INSTALLS += target headers ui_headers notification_headers