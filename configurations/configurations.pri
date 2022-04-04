include(android/android.pri)
include(ios/ios.pri)

linux:!android {
    LIBS += -lmatrix-client-library
}

LIBS += -lspdlog -lcurl -lcoeurl \
        -lmatrix_client
