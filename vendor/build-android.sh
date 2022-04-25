#!/usr/bin/env  bash

##### [Build Configurations] ##################################################
CMAKE_TOOLCHAIN="${ANDROID_NDK}/build/cmake/android.toolchain.cmake"
MIN_SDK_VERSION=21
TARGET=armeabi-v7a
OPENSSL_ROOT_DIR="${ANDROID_HOME}/android_openssl/"
# BUILD_VERBOSE=1

##### [Global Variables] ######################################################
STARTUP_DIR=`realpath .`
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
TEMP_DIR="$SCRIPT_DIR/_temp"
BUILD_DIR="$SCRIPT_DIR/_build"
DIST_DIR="$SCRIPT_DIR/_dist"
PATCH_DIR="$SCRIPT_DIR/patches"

##### [Build Utility Functions] ###############################################
OPENSSL_CMAKE_DEFINITIONS=( "-DOPENSSL_ROOT_DIR=$OPENSSL_ROOT_DIR" 
        "-DOPENSSL_INCLUDE_DIR=$OPENSSL_ROOT_DIR/static/include" 
        "-DOPENSSL_CRYPTO_LIBRARY=$OPENSSL_ROOT_DIR/latest/arm/libcrypto_1_1.so" 
        "-DOPENSSL_SSL_LIBRARY=${OPENSSL_ROOT_DIR}/latest/arm/libssl_1_1.so" )

function BUILD_LIB {
    params=( "$@" )
    src_path=${params[0]}
    build_path=${params[1]}
    target=${params[2]}
    unset params[2]
    unset params[1]
    unset params[0]

    echo "SRC: $src_path"
    echo "BLD: $build_path"
    echo "TRG: $target"
    echo "PARAMS: ${params[@]}" 

    # if  [ -d "$build_path" ]; then
    #     rm -rf "$build_path";
    # fi
    [ -f "$build_path/CMakeCache.txt" ] && rm "$build_path/CMakeCache.txt"
    mkdir -p "$build_path"

    cmake -DCMAKE_TOOLCHAIN_FILE=$CMAKE_TOOLCHAIN \
        -DANDROID_ABI=$target -DANDROID_PLATFORM=android-$MIN_SDK_VERSION \
        -DCMAKE_INSTALL_PREFIX="${DIST_DIR}/${target}" \
        -DCMAKE_SHARED_LINKER_FLAGS="-L${DIST_DIR}/$target/lib" \
        -DCMAKE_BUILD_TYPE=Release \
        -S"$src_path" -B"$build_path" ${params[@]}

    is_verbose=""
    if [ ! -z $BUILD_VERBOSE ]; then 
        is_verbose=-v 
    fi

    cmake --build $build_path $is_verbose --config Release \
        &&  cmake --install $build_path --config Release

}

function DOWNLOAD_EXTRACT {
    name="$1"
    version="$2"
    url="$3"
    name_version="$name-$version"
    archive_path="$TEMP_DIR/$name_version.tar.gz"
    src_path="$TEMP_DIR/$name_version"
    build_path="$BUILD_DIR/$name_version"

    [ ! -f "$archive_path" ] && wget -O "$archive_path" "$url" || echo ">> Use cached version: $archive_path"
    [ ! -d "$src_path" ] && tar xf $archive_path --directory "$TEMP_DIR"
}

function FETCH_REPOSITORY {
    name="$1"
    tag="$2"
    repo="$3"
    src_path="$TEMP_DIR/$name-$tag"
    build_path="$BUILD_DIR/$name-$tag"
    
    if [ ! -d "$src_path" ]; then
        git clone --branch "$tag" --depth 1 "$repo" "$src_path"
    else
        echo ">> Use cached version: $src_path"
    fi
}

function APPLY_PATCH {
    params=( "$@" )
    repo=${params[0]}
    unset params[0]

    git -C "$src_path" config commit.gpgsign off
    git -C "$src_path" am ${params[@]}
}


##### [Libraries ] ############################################################
function BUILD_FMT {
    target="$1"
    name="fmt"
    version="8.1.1"
    download_url="https://github.com/fmtlib/fmt/archive/refs/tags/8.1.1.tar.gz"
    DOWNLOAD_EXTRACT $name $version $download_url

    BUILD_LIB "$src_path" "$build_path" $target \
        -DBUILD_SHARED_LIBS=ON \
        -DFMT_TEST=OFF \
        -DFMT_DOC=OFF
}

function BUILD_SPDLOG {
    target="$1"
    name="spdlog"
    version="1.9.2"
    download_url="https://github.com/gabime/spdlog/archive/refs/tags/v$version.tar.gz"
    DOWNLOAD_EXTRACT $name $version $download_url

    BUILD_LIB "$src_path" "$build_path" $target \
        -DSPDLOG_FMT_EXTERNAL=ON \
        -Dfmt_DIR=${DIST_DIR}/$target/lib/cmake/fmt \
        -DSPDLOG_BUILD_SHARED=ON \
        -DSPDLOG_BUILD_EXAMPLE=OFF \
        -DSPDLOG_BUILD_TESTS=OFF
}

function BUILD_JSON {
    target="$1"
    name="json"
    version="3.10.5"
    download_url="https://github.com/nlohmann/json/archive/refs/tags/v3.10.5.tar.gz"
    DOWNLOAD_EXTRACT $name $version $download_url

    BUILD_LIB $src_path $build_path $target \
        "-DJSON_BuildTests=OFF"
}

function BUILD_OLM {
    target="$1"
    name="olm"
    version="3.2.10"
    download_url="https://gitlab.matrix.org/matrix-org/olm/-/archive/3.2.10/olm-3.2.10.tar.gz"
    DOWNLOAD_EXTRACT $name $version $download_url

    BUILD_LIB $src_path $build_path $target \
        -DOLM_TESTS=OFF
}

function BUILD_CURL {
    target="$1"
    name="curl"
    version="curl-7_81_0"
    download_url="https://github.com/curl/curl/archive/refs/tags/curl-7_81_0.tar.gz"
    DOWNLOAD_EXTRACT $name $version $download_url

    export CURL_CA_BUNDLE=/etc/ssl/certs/ca-certificates.crt

    BUILD_LIB $src_path $build_path $target \
        ${OPENSSL_CMAKE_DEFINITIONS[@]} \
        -DCURL_CA_BUNDLE=${CURL_CA_BUNDLE}
        
        # -DCURL_CA_PATH=/system/etc/security/cacerts        
        # -DCURL_CA_BUNDLE="/home/reza/QtProjects/cURLTest/android/assets/cacert.pem"        
        # -DBUILD_SHARED_LIBS=OFF
}

function BUILD_LIBEVENT {
    target="$1"
    name="libevent"
    version="release-2.1.12-stable"
    download_url="https://github.com/libevent/libevent/archive/refs/tags/release-2.1.12-stable.tar.gz"
    DOWNLOAD_EXTRACT $name $version $download_url

    BUILD_LIB $src_path $build_path $target \
        ${OPENSSL_CMAKE_DEFINITIONS[@]} \
        -DEVENT__DISABLE_BENCHMARK=ON \
        -DEVENT__DISABLE_TESTS=ON \
        -DEVENT__DISABLE_REGRESS=ON \
        -DEVENT__DISABLE_SAMPLES=ON

        # -DEVENT__LIBRARY_TYPE=STATIC
}


function BUILD_COEURL {
    target="$1"
    name="coeurl"
    version="v0.1.1"
    download_url="https://nheko.im/nheko-reborn/coeurl/-/archive/v0.1.1/coeurl-v0.1.1.tar.gz"
    DOWNLOAD_EXTRACT $name $version $download_url

    BUILD_LIB $src_path $build_path $target \
        -Dfmt_DIR=${DIST_DIR}/$target/lib/cmake/fmt \
        -Dspdlog_DIR=${DIST_DIR}/$target/lib/cmake/spdlog
}

function BUILD_MTXCLIENT {
    target="$1"
    name="mtxclient"
    # version="0.6.1"
    # download_url="https://github.com/Nheko-Reborn/mtxclient/archive/refs/tags/v$version.tar.gz"
    # DOWNLOAD_EXTRACT $name $version $download_url
    tag="v0.7.0"
    download_url="https://github.com/Nheko-Reborn/mtxclient.git"
    FETCH_REPOSITORY $name $tag $download_url
    APPLY_PATCH $src_path \
        "$PATCH_DIR/mtxclient/0001-disable-Tls-verification-on-coeurl.patch"
    

    BUILD_LIB "$src_path" "$build_path" $target \
        ${OPENSSL_CMAKE_DEFINITIONS[@]} \
        -Dfmt_DIR=${DIST_DIR}/$target/lib/cmake/fmt \
        -Dspdlog_DIR=${DIST_DIR}/$target/lib/cmake/spdlog \
        -DSPDLOG_FMT_EXTERNAL=ON \
        -DOlm_DIR=${DIST_DIR}/$target/lib/cmake/Olm \
        -Dnlohmann_json_DIR=${DIST_DIR}/$target/lib/cmake/nlohmann_json \
        -DBUILD_LIB_TESTS=OFF \
        -DBUILD_LIB_EXAMPLES=OFF \
        -DBUILD_SHARED_LIBS=ON
}

function BUILD_LMDB {
    target="$1"
    name="lmdb"
    tag="mdb.master"
    repo="https://github.com/ramajd/lmdb.git"
    FETCH_REPOSITORY $name $tag $repo

    BUILD_LIB "$src_path" "$build_path" "$target"
}

function BUILD_LMDBXX {
    target="$1"
    name="lmdbxx"
    tag="master"
    repo="https://github.com/Nheko-Reborn/lmdbxx.git"
    FETCH_REPOSITORY $name $tag $repo

    cp "$src_path/lmdb++.h" "${DIST_DIR}/${target}/include/"
}

function BUILD_CMARK {
    target="$1"
    name="cmark"
    version="0.30.2"
    download_url="https://github.com/commonmark/cmark/archive/refs/tags/0.30.2.tar.gz"
    DOWNLOAD_EXTRACT $name $version $download_url

    BUILD_LIB "$src_path" "$build_path" "$target"

        # -DCMARK_SHARED=OFF
}

function BUILD_MATRIX_CLIENT_LIBRARY {
    target="$1"
    name="matrix-client-library"
    tag="0.0.37"
    repo="git@git.pantherx.org:development/libraries/matrix-client-library.git"
    FETCH_REPOSITORY $name $tag $repo

    BUILD_LIB "$src_path" "$build_path" "$target" \
        ${OPENSSL_CMAKE_DEFINITIONS[@]} \
        -DVOIP=OFF \
        -Dfmt_DIR=${DIST_DIR}/$target/lib/cmake/fmt \
        -Dspdlog_DIR=${DIST_DIR}/$target/lib/cmake/spdlog \
        -DLMDB_INCLUDE_DIR=${DIST_DIR}/${target}/include \
        -DLMDB_LIBRARY=${DIST_DIR}/${target}/lib/liblmdb.a \
        -DOlm_DIR=${DIST_DIR}/$target/lib/cmake/Olm \
        -DMatrixClient_DIR=${DIST_DIR}/${target}/lib/cmake/MatrixClient \
        -DCMARK_INCLUDE_DIR=${DIST_DIR}/${target}/include \
        -DCMARK_LIBRARY=${DIST_DIR}/${target}/lib/libcmark.a \
        -Dnlohmann_json_DIR=${DIST_DIR}/${target}/lib/cmake/nlohmann_json \
        -DCMAKE_FIND_ROOT_PATH=~/Qt/5.15.2/android
}


function BUILD_ALL {
    target="$1"
    BUILD_FMT "$target" && \
        BUILD_SPDLOG "$target" && \
        BUILD_JSON "$target" && \
        BUILD_OLM "$target" && \
        BUILD_CURL "$target" && \
        BUILD_LIBEVENT "$target" && \
        BUILD_COEURL "$target" && \
        BUILD_MTXCLIENT "$target" && \
        BUILD_LMDB "$target" && \
        BUILD_LMDBXX "$target" && \
        BUILD_CMARK "$target" && \
        BUILD_MATRIX_CLIENT_LIBRARY "$target" && \
        echo "DONE"
}

##### [Main] ##################################################################

mkdir -p $TEMP_DIR
mkdir -p $BUILD_DIR
mkdir -p $DIST_DIR

$(eval echo "BUILD_${1^^} $TARGET")