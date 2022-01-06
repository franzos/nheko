QT += qml quick quickcontrols2

CONFIG += c++17
QMAKE_CXXFLAGS += -std=c++17
CONFIG+=qml_debug
CONFIG+=declarative_debug

SOURCES += \
    RoomItem.cpp \
    RoomListModel.cpp \
    main.cpp 

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    RoomItem.h  \
    RoomListModel.h

LIBS += -L/home/panther/.guix-profile/lib/ -lmatrix-client-library
LIBS += -lmatrix_client

INCLUDEPATH += $$PWD/''
DEPENDPATH += $$PWD/''

#unix:!macx: LIBS += -lssl

