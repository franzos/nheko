include(android/android.pri)
include(ios/ios.pri)

linux:!android {
    DIST_DIR=$$PWD/../vendor/_dist/$$QT_ARCH
    !exists($$DIST_DIR):error("vendor packages has not been setup yet! (run vendor/setup_linux.sh)")
}

INCLUDEPATH += $$DIST_DIR/include

LIBS += -L$$DIST_DIR/lib
LIBS += -lfmt -lspdlog -lcurl \
        -levent -levent_core -levent_extra -levent_openssl -levent_pthreads \
        -lmatrix_client \
        -lmatrix-client-library_armeabi-v7a
