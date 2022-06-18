include(android/android.pri)
include(ios/ios.pri)

linux:!android {
    # Configurations only required for desktop linux (not android)
    LIBS += -lmatrix-client-library

    target.path = /usr/bin/
    target.files = matrix-client
    INSTALLS += target
}

LIBS += -lspdlog -lcurl -lcoeurl \
        -lmatrix_client
