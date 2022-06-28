    include(android/android.pri)
include(ios/ios.pri)

linux:!android {
    # Configurations only required for desktop linux (not android)
    LIBS += -lmatrix-client-library 
    QT += dbus

    target.path = /usr/bin/
    target.files = matrix-client

    desktop_file.path  = /usr/share/applications
    desktop_file.files = resources/matrix-client.desktop

    app_icon.path  = /usr/share/icons
    app_icon.files = resources/images/matrix-client-icon_bright.svg

    INSTALLS += target app_icon desktop_file
}

LIBS += -lspdlog -lcurl -lcoeurl \
        -lpx-auth-lib-cpp \
        -lmatrix_client
