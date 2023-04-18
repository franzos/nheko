android {
    !exists($$(OPENSSL_ROOT_DIR)/../openssl.pri):error("OpenSSL not found!")
    include($$(OPENSSL_ROOT_DIR)/../openssl.pri)

    include(../../lib/SCodes/src/SCodes.pri)
    DIST_DIR=$$PWD/../../vendor/_dist
    
    !exists($${DIST_DIR}/$${ANDROID_TARGET_ARCH}):error("vendor packages has not been setup yet! (run vendor/build-android.sh) $${DIST_DIR}/$${ANDROID_TARGET_ARCH}")

    ANDROID_TARGET_SDK_VERSION = 32
    ANDROID_MIN_SDK_VERSION = 24

    ANDROID_PACKAGE_SOURCE_DIR = $$PWD

    DEFINES += SPDLOG_FMT_EXTERNAL=ON

    # https://stackoverflow.com/a/69466519/4380582
    ANDROID_EXTRA_LIBS += \
        $${DIST_DIR}/$${ANDROID_TARGET_ARCH}/lib/libfmt.so \
        $${DIST_DIR}/$${ANDROID_TARGET_ARCH}/lib/libspdlog.so \
        $${DIST_DIR}/$${ANDROID_TARGET_ARCH}/lib/libmatrix-client-library_$${ANDROID_TARGET_ARCH}.so \
        $${DIST_DIR}/$${ANDROID_TARGET_ARCH}/lib/libmatrix_client_$${ANDROID_TARGET_ARCH}.so \
        $${DIST_DIR}/$${ANDROID_TARGET_ARCH}/lib/libcmark.so \
        $${DIST_DIR}/$${ANDROID_TARGET_ARCH}/lib/libolm.so \
        $${DIST_DIR}/$${ANDROID_TARGET_ARCH}/lib/libblurhash.so


    QT += androidextras concurrent
    
    INCLUDEPATH += $${DIST_DIR}/$${ANDROID_TARGET_ARCH}/include
    LIBS += $${DIST_DIR}/$${ANDROID_TARGET_ARCH}/lib/libfmt.so \
            $${DIST_DIR}/$${ANDROID_TARGET_ARCH}/lib/libblurhash.so \
            $${DIST_DIR}/$${ANDROID_TARGET_ARCH}/lib/libmatrix_client_$${ANDROID_TARGET_ARCH}.so \
            $${DIST_DIR}/$${ANDROID_TARGET_ARCH}/lib/libmatrix-client-library_$${ANDROID_TARGET_ARCH}.so


    H = $$LITERAL_HASH
    lines = "/* Autogenerated: do not edit */"
    NO_CIBA_AUTH {
        lines += "$${H}define CIBA_AUTH 0"
    } else {
        ANDROID_EXTRA_LIBS += $${DIST_DIR}/$${ANDROID_TARGET_ARCH}/lib/libpx-auth-lib-cpp_$${ANDROID_TARGET_ARCH}.so
        LIBS += $${DIST_DIR}/$${ANDROID_TARGET_ARCH}/lib/libpx-auth-lib-cpp_$${ANDROID_TARGET_ARCH}.so
        lines += "$${H}define CIBA_AUTH 1"
        message(" + CIBA: enabled.")
    }
    write_file($$PWD/../../cpp/Features.h, lines)

}

DISTFILES += \
    $$PWD/AndroidManifest.xml \
    $$PWD/build.gradle \
    $$PWD/gradle.properties \
    $$PWD/gradle/wrapper/gradle-wrapper.jar \
    $$PWD/gradle/wrapper/gradle-wrapper.properties \
    $$PWD/gradlew \
    $$PWD/gradlew.bat \
    $$PWD/res/values/libs.xml
