ios {
    DIST_DIR=$$PWD/../../vendor/_dist
    !exists($$DIST_DIR):error("vendor packages has not been setup yet! (run vendor/build-ios.sh)")
    include(../../lib/SCodes/src/SCodes.pri)

    # GStreamer setup
    GST_ROOT = $$(HOME)/Library/Developer/GStreamer/iPhone.sdk
    !exists($$GST_ROOT):error("GStreamer not found")

    INCLUDEPATH += $$GST_ROOT/GStreamer.framework/Headers/
    LIBS += -F$$GST_ROOT -framework GStreamer

    LIBS += -framework Foundation
    LIBS += -framework CoreGraphics
    LIBS += -framework UIKit
    LIBS += -framework AVFoundation
    LIBS += -framework CoreMedia
    LIBS += -framework CoreVideo
    LIBS += -framework IOSurface
    LIBS += -framework VideoToolbox
    LIBS += -liconv
    LIBS += -lresolv

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
