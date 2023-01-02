#!/usr/bin/env bash
STARTUP_DIR=$( pwd )
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source "$SCRIPT_DIR/utils.sh"
CMAKE_TOOLCHAIN="${SCRIPT_DIR}/ios.toolchain.cmake"
QT5_ROOT=~/Qt/5.15.2/ios

ECHO_DEBUG "Debug mode is enabled"

##### [Build Functions] #######################################################

function BUILD_LIB {
    params=( "$@" )
    ECHO_DEBUG "PARAMS: ${params[@]}"
    libname=${params[0]}
    [[ -z $libname ]] && ECHO_ERR "libname is empty" && exit 1
    unset params[0]

    ECHO
    ECHO ">> Build: [$libname] for [iOS]"
    ECHO "-----------------------------------"

    cmake -G Xcode -DCMAKE_TOOLCHAIN_FILE="$CMAKE_TOOLCHAIN" -DPLATFORM=OS64COMBINED \
        -DCMAKE_INSTALL_PREFIX="$DIST_DIR" \
        -DCMAKE_BUILD_TYPE=Release \
        -S"$src_path" -B"$build_path" ${params[@]} && \
        cmake --build $build_path --config Release && \
        cmake --install $build_path --config Release
}

##### [Build Libraries] #######################################################

function BUILD_OPENSSL {
    name="openssl-apple"
    version="1.1.11700"
    download_url="https://github.com/passepartoutvpn/openssl-apple.git"
    FETCH_REPOSITORY "$name" "$version" "$download_url"

    if [ -d $build_path ]; then
        rm -rf "$build_path"
    fi 
    cp -r "$src_path" "$build_path"
    ECHO_DEBUG "Build path: $build_path"

    cd $build_path
    
    ./build-libssl.sh --cleanup --version=1.1.1l --targets='ios-sim-cross-x86_64 ios64-cross-arm64'
       
    iospath="${build_path}/bin/iPhoneOS16.1-arm64.sdk/lib"
    simpath="${build_path}/bin/iPhoneSimulator16.1-x86_64.sdk/lib"
    hdrpath="${build_path}/bin/iPhoneSimulator16.1-x86_64.sdk/include"

    mkdir -p "${DIST_DIR}/lib"
    mkdir -p "${DIST_DIR}/include"
    lipo -create "$simpath/libssl.a" "$iospath/libssl.a" -output "${DIST_DIR}/lib/libssl.a"
    lipo -create "$simpath/libcrypto.a" "$iospath/libcrypto.a" -output "${DIST_DIR}/lib/libcrypto.a"
    cp -R "$hdrpath/openssl" "${DIST_DIR}/include/"

    lipo -info "${DIST_DIR}/lib/libssl.a"
    lipo -info "${DIST_DIR}/lib/libcrypto.a"
    
    cd $STARTUP_DIR
}

function BUILD_FMT {
    name="fmt"
    version="8.1.1"
    download_url="https://github.com/fmtlib/fmt/archive/refs/tags/$version.tar.gz"
    DOWNLOAD_EXTRACT $name $version $download_url

    BUILD_LIB $name \
        "-DFMT_DOC=OFF" \
        "-DFMT_TEST=OFF" \
        "-DFMT_INSTALL=ON"
}

function BUILD_SPDLOG {
    name="spdlog"
    version="1.9.2"
    download_url="https://github.com/gabime/spdlog/archive/refs/tags/v$version.tar.gz"
    DOWNLOAD_EXTRACT $name $version $download_url

    BUILD_LIB $name \
        "-DSPDLOG_BUILD_EXAMPLE=OFF" \
        "-DSPDLOG_BUILD_TESTS=OFF" \
        "-DSPDLOG_BUILD_BENCH=OFF" \
        "-DSPDLOG_INSTALL=ON"
}

function BUILD_JSON {
    name="json"
    version="3.10.5"
    download_url="https://github.com/nlohmann/json/archive/refs/tags/v$version.tar.gz"
    DOWNLOAD_EXTRACT $name $version $download_url

    BUILD_LIB $name \
        "-DJSON_BuildTests=OFF"
}

function BUILD_OLM {
    name="olm"
    version="3.2.10"
    download_url="https://gitlab.matrix.org/matrix-org/olm/-/archive/$version/olm-$version.tar.gz"
    DOWNLOAD_EXTRACT $name $version $download_url

    BUILD_LIB $name \
        "-DOLM_TESTS=OFF" \
        "-DBUILD_SHARED_LIBS=OFF"
}

function BUILD_MTXCLIENT {
    name="qmtxclient"
    version="v0.8.2-4"
    download_url="git@git.pantherx.org:development/libraries/qmtxclient.git"
    FETCH_REPOSITORY $name $version $download_url
    APPLY_PATCH $name $PATCH_DIR/mtxclient/0001-fix-ios-build.patch

    BUILD_LIB $name \
        -DCMAKE_FIND_ROOT_PATH=$QT5_ROOT \
        -DOPENSSL_ROOT_DIR=$DIST_DIR \
        -DCMAKE_PREFIX_PATH=$DIST_DIR/lib/cmake \
        -DBUILD_LIB_TESTS=OFF \
        -DBUILD_LIB_EXAMPLES=OFF \
        -DBUILD_SHARED_LIBS=OFF
}

function BUILD_LMDB {
    name="lmdb"
    version="LMDB_0.9.29"
    download_url="https://github.com/LMDB/lmdb.git"
    FETCH_REPOSITORY $name $version $download_url
    APPLY_PATCH $name \
        $PATCH_DIR/lmdb/0001-add-cmake-support.patch

    BUILD_LIB $name \
        -DMDB_USE_POSIX_SEM=ON
}

function BUILD_LMDBXX {
    name="lmdbxx"
    version="master"
    download_url="https://github.com/Nheko-Reborn/lmdbxx.git"
    FETCH_REPOSITORY $name $version $download_url

    cp "$src_path/lmdb++.h" "$DIST_DIR/include/"
}

function BUILD_CMARK {
    name="cmark"
    version="0.30.2"
    download_url="https://github.com/commonmark/cmark.git"
    FETCH_REPOSITORY $name $version $download_url
    APPLY_PATCH $name \
        $PATCH_DIR/cmark/0001-remove-exe-target-for-ios-build.patch

    BUILD_LIB $name \
        "-DCMARK_TESTS=OFF" \
        "-DCMARK_STATIC=ON" \
        "-DCMARK_SHARED=OFF"
}

function BUILD_PX_AUTH_LIB_CPP {
    name="px-auth-lib-cpp"
    version="0.0.28"
    download_url="git@git.pantherx.org:development/libraries/px-auth-library-cpp.git"
    FETCH_REPOSITORY $name $version $download_url

    BUILD_LIB $name \
        -DCMAKE_FIND_ROOT_PATH=$QT5_ROOT \
        -DSTATIC_LIB=ON \
        -DBUILD_TESTS=OFF
}

function BUILD_MATRIX_CLIENT_LIBRARY {
    name="matrix-client-library"
    version="0.1.32"
    download_url="git@git.pantherx.org:development/libraries/matrix-client-library.git"
    FETCH_REPOSITORY $name $version $download_url
    APPLY_PATCH $name \
        $PATCH_DIR/matrix-client-library/0001-fix-ios-build-issues.patch

    BUILD_LIB $name \
        -DCMAKE_FIND_ROOT_PATH=$QT5_ROOT \
        -DCIBA_AUTHENTICATION=ON \
        -DVOIP=OFF \
        -DSTATIC_LIB=ON
}

function BUILD_BLURHASH {
    name="blurhash"
    version="v0.0.1"
    download_url="https://github.com/Nheko-Reborn/blurhash.git"
    FETCH_REPOSITORY $name $version $download_url
    APPLY_PATCH $name \
        $PATCH_DIR/blurhash/0001-add-cmake-build-system.patch
    BUILD_LIB $name \
        "-DBUILD_SHARED_LIBS=OFF"
}


function BUILD_ALL {
    BUILD_FMT && \
        BUILD_SPDLOG && \
        BUILD_JSON && \
        BUILD_OLM && \
        BUILD_LMDB && \
        BUILD_LMDBXX && \
        BUILD_CMARK && \
        BUILD_BLURHASH && \
        BUILD_MTXCLIENT && \
        BUILD_PX_AUTH_LIB_CPP && \
        BUILD_MATRIX_CLIENT_LIBRARY
}

##### [Main] ##################################################################

mkdir -p $TEMP_DIR
mkdir -p $BUILD_DIR
mkdir -p $DIST_DIR

LIBNAME="ALL"
if [[ -n $1 ]]; then
    LIBNAME=$(echo $1 | tr '[:lower:]' '[:upper:]')
fi
BUILD_$LIBNAME