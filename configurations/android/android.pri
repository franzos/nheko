android {
    !exists($$(ANDROID_HOME)/android_openssl/openssl.pri):error("OpenSSL not found!")
    include($$(ANDROID_HOME)/android_openssl/openssl.pri)

    DIST_DIR=$$PWD/../../vendor/_dist
    for (abi, ANDROID_ABIS): !exists($$DIST_DIR/$${abi}):error("vendor packages has not been setup yet! (run vendor/build-android.sh)")

    ANDROID_TARGET_SDK_VERSION = 30
    ANDROID_MIN_SDK_VERSION = 23

    ANDROID_PACKAGE_SOURCE_DIR = $$PWD

    DEFINES += SPDLOG_FMT_EXTERNAL=ON

    # https://stackoverflow.com/a/69466519/4380582
    for (abi, ANDROID_ABIS): ANDROID_EXTRA_LIBS += \
            $$DIST_DIR/$${abi}/lib/libfmt.so \
            $$DIST_DIR/$${abi}/lib/libspdlog.so \
            $$DIST_DIR/$${abi}/lib/libpx-auth-lib-cpp_$${abi}.so \
            $$DIST_DIR/$${abi}/lib/libmatrix-client-library_$${abi}.so \
            $$DIST_DIR/$${abi}/lib/libmatrix_client_$${abi}.so \
            $$DIST_DIR/$${abi}/lib/libcmark.so \
            $$DIST_DIR/$${abi}/lib/libolm.so \
            $$DIST_DIR/$${abi}/lib/libblurhash.so


    QT += androidextras concurrent
    for (abi, ANDROID_ABIS): INCLUDEPATH += $$DIST_DIR/$${abi}/include
    for (abi, ANDROID_ABIS): LIBS += -L$$DIST_DIR/$${abi}/lib
    for (abi, ANDROID_ABIS): LIBS += -lfmt -lspdlog \
            -lmatrix_client_$${abi} \
            -lpx-auth-lib-cpp_$${abi} \
            -lmatrix-client-library_$${abi} \
            -lblurhash
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
