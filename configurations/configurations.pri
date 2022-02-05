include(android/android.pri)
include(ios/ios.pri)

linux:!android {
    isEmpty($$(GUIX_ENVIRONMENT)) {
        message(Build for GUIX Environment: $$(GUIX_ENVIRONMENT))
        DIST_DIR=$$(GUIX_ENVIRONMENT)
        !exists($$DIST_DIR):error("GUIX_ENVIRONMENT is not set. (run `guix environment -m vendor/manifest.scm`)")
    } else {
        message(BUILD for Linux)
        DIST_DIR=$$PWD/../vendor/_dist/$$QT_ARCH
        !exists($$DIST_DIR):error("vendor packages has not been setup yet! (run vendor/setup_linux.sh)")
    }
    LIBS += -lmatrix-client-library
}

INCLUDEPATH += $$DIST_DIR/include

LIBS += -L$$DIST_DIR/lib
LIBS += -lspdlog -lcurl -lcoeurl \
        -lmatrix_client
