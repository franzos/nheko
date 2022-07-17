android {
    !exists($$(ANDROID_HOME)/android_openssl/openssl.pri):error("OpenSSL not found!")
    include($$(ANDROID_HOME)/android_openssl/openssl.pri)

    DIST_DIR=$$PWD/../../vendor/_dist/armeabi-v7a
    !exists($$DIST_DIR):error("vendor packages has not been setup yet! (run vendor/build-android.sh)")

    ANDROID_TARGET_SDK_VERSION = 30

    ANDROID_PACKAGE_SOURCE_DIR = $$PWD

    DEFINES += SPDLOG_FMT_EXTERNAL=ON
    ANDROID_EXTRA_LIBS += \
        $$DIST_DIR/lib/libfmt.so \
        $$DIST_DIR/lib/libspdlog.so \
        $$DIST_DIR/lib/libcurl.so \
        $$DIST_DIR/lib/libevent-2.1.so \
        $$DIST_DIR/lib/libevent_core-2.1.so \
        $$DIST_DIR/lib/libevent_extra-2.1.so \
        $$DIST_DIR/lib/libevent_openssl-2.1.so \
        $$DIST_DIR/lib/libevent_pthreads-2.1.so \
        $$DIST_DIR/lib/libpx-auth-lib-cpp_armeabi-v7a.so \
        $$DIST_DIR/lib/libmatrix-client-library_armeabi-v7a.so \
        $$DIST_DIR/lib/libmatrix_client.so \
        $$DIST_DIR/lib/libcmark.so \
        $$DIST_DIR/lib/libolm.so

    QT += androidextras concurrent
    INCLUDEPATH += $$DIST_DIR/include
    LIBS += -L$$DIST_DIR/lib
    LIBS += -lfmt -levent -levent_core -levent_extra -levent_pthreads \
            -lpx-auth-lib-cpp_armeabi-v7a \
            -lmatrix-client-library_armeabi-v7a
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
