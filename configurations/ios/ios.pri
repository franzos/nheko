ios {
    DIST_DIR=$$PWD/../../vendor/_dist
    !exists($$DIST_DIR):error("vendor packages has not been setup yet! (run vendor/build-ios.sh)")
    include(../../lib/SCodes/src/SCodes.pri)

    QMAKE_INFO_PLIST = $$PWD/Info.plist
    CONFIG -= bitcode
    # QMAKE_ASSET_CATALOGS += $$PWD/Assets.xcassets

    INCLUDEPATH += $$DIST_DIR/include

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
