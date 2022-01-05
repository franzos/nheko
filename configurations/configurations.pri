include(android/android.pri)
include(ios/ios.pri)

linux:!android {
    DIST_DIR=$$PWD/../vendor/dist/$$QT_ARCH
    !exists($$DIST_DIR):error("vendor packages has not been setup yet! (run vendor/setup_linux.sh)")
}

message(DIST PATH: $$DIST_DIR)
INCLUDEPATH += $$DIST_DIR/include
LIBS += -L$$DIST_DIR/lib
LIBS += -lspdlog
