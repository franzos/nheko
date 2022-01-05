ios {
    DIST_DIR=$$PWD/../../vendor/dist/ios
    !exists($$DIST_DIR):error("vendor packages has not been setup yet! (run vendor/setup_ios.sh)")
}
