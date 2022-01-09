android {
    DIST_DIR=$$PWD/../../vendor/dist/android/$$QT_ARCH
    !exists($$DIST_DIR):error("vendor packages has not been setup yet! (run vendor/setup_android.sh)")

    ANDROID_PACKAGE_SOURCE_DIR = $$PWD

    ANDROID_EXTRA_LIBS += \
        $$DIST_DIR/lib/libspdlog.so
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
