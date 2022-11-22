ios {
    DIST_DIR=$$PWD/../../vendor/_dist
    !exists($$DIST_DIR):error("vendor packages has not been setup yet! (run vendor/build-ios.sh)")

    QMAKE_INFO_PLIST = $$PWD/Info.plist

    INCLUDEPATH += $$DIST_DIR/include
    message($$INCLUDEPATH)

    LIBS += -L$$DIST_DIR/lib \
            -lssl -lcrypto \
            -lspdlog \
            -lolm \
            -llmdb \
            -lcmark \
            -lblurhash \
            -lmatrix_client \
            -lpx-auth-lib-cpp \
            -lmatrix-client-library
}
