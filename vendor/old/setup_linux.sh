#!/usr/bin/env bash
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
STARTUP_DIR=$( pwd )
source "${SCRIPT_DIR}/utils.sh"
CORES=$((`nproc`+1))

###############################################################################
# Logic
###############################################################################
function BUILD_LIBRARY {
    # USAGE: BUILD_LIBRARY LIBNAME [OPTIONAL PARAMS]
    params=( "$@" )
    libname=${params[0]}
    [[ -z "$libname" ]] && PRINT_ERROR_EXIT "LIBNAME is not provided"
    unset params[0]

    echo ""
    PRINT_INFO "======= Build: [$libname] for [x86_64] ======================="

    libpath="${BUILD_DIR}/${libname}"
    [ -d "$libpath" ] && rm -rf "$libpath"

    cmake -DCMAKE_INSTALL_PREFIX="${DIST_DIR}" \
        -S"${SCRIPT_DIR}/${libname}" -B"${libpath}" ${params[@]}
    
    cmake --build $libpath --config Release
    cmake --install $libpath --config Release
}

###############################################################################
# Library specific functions
###############################################################################
function BUILD_SPDLOG {
    BUILD_LIBRARY 'spdlog' \
        '-DSPDLOG_BUILD_SHARED=ON' \
        '-DSPDLOG_BUILD_EXAMPLE=OFF' \
        '-DSPDLOG_BUILD_TESTS=OFF'
}

function BUILD_OPENSSL {
    libname='openssl'
    version="1.1.1l"
    PRINT_INFO "======= Build: [$libname] for [x86_64] ======================="
    
    libpath="${BUILD_DIR}/${libname}"
    MKPATH $libpath

    download_url='https://openssl.org/source'
    lib_version_name="$libname-$version"
    zip_name="$lib_version_name.tar.gz"
    zip_path="$libpath/$zip_name"
    sig_name="$zip_name.sha256"
    sig_path="$libpath/$sig_name"

    [ ! -f "$sig_path" ] && wget -O "$sig_path" "$download_url/$sig_name" 
    [ ! -f "$zip_path" ] && wget -O "$zip_path" "$download_url/$zip_name" || PRINT_INFO "use cached archive: $zip_path"
    VERIFY_CHECKSUM $zip_path $sig_path
    [ $? -ne 0 ] && PRINT_ERROR_EXIT "archive signature mismatched. remove old files to download again"

    target_dir="$libpath/$lib_version_name"
    [ -d "$target_dir" ] && rm -rf "$target_dir"
    tar xf $zip_path --directory "$libpath"

    cd $target_dir
    ./Configure 'linux-x86_64' shared \
        --prefix="$DIST_DIR"
    make -j$CORES
    make install
    make clean
    cd "$STARTUP_DIR"
}

function BUILD_CURL {
    libname="curl"
    version="7.80.0"
    version_underlined=$(echo $version | sed s/[.]/_/g)
    PRINT_INFO "======= Build: [$libname] for [x86_64] ======================="

    download_url="https://github.com/curl/curl/releases/download/curl-$version_underlined/curl-$version.tar.gz"
    libpath="$BUILD_DIR/$libname"
    zip_name="$libname-$version.tar.gz"
    zip_path="$libpath/$zip_name"

    MKPATH $libpath
    [ ! -f $zip_path ] && wget -O "$zip_path" $download_url || PRINT_INFO "use cached archive: $zip_path"

    target_dir="$libpath/$libname-$version"
    [ -d "$target_dir" ] && rm -rf "$target_dir"
    tar xf "$zip_path" --directory "$libpath"
    
    SSL_DIR="$DIST_DIR"    
    cd $target_dir
    ./configure --prefix="$DIST_DIR" \
            --with-openssl=$SSL_DIR \
            --with-pic
    make -j$CORES
    make install
    make clean
    cd "$STARTUP_DIR"
}

function BUILD_LIBEVENT {
    libname="libevent"
    version="2.1.12-stable"
    echo ""
    PRINT_INFO "======= Build: [$libname] for [x86_64] ======================="

    libpath="$BUILD_DIR/$libname"
    zip_name="$libname-$version.tar.gz"
    zip_path="$libpath/$zip_name"
    download_url="https://github.com/libevent/libevent/releases/download/release-$version/$zip_name"

    MKPATH $libpath
    [ ! -f $zip_path ] && wget -O "$zip_path" $download_url || PRINT_INFO "use cached archive: $zip_path"

    target_dir="$libpath/$libname-$version"
    [ -d "$target_dir" ] && rm -rf "$target_dir"
    tar xf "$zip_path" --directory "$libpath"
    
    export PKG_CONFIG_PATH="$DIST_DIR/lib/pkgconfig"
    cd $target_dir
    ./configure --prefix="$DIST_DIR" \
            --with-pic \
            --disable-samples

    make -j$CORES
    make install
    make clean
    cd "$STARTUP_DIR"
}

function BUILD_COEURL {
    export PKG_CONFIG_PATH="${DIST_DIR}/lib/pkgconfig"
    BUILD_LIBRARY "coeurl" \
        -Dspdlog_DIR="${DIST_DIR}/lib/cmake/spdlog" \
        -DBUILD_SHARED_LIBS=ON
}

function BUILD_JSON {
    BUILD_LIBRARY "json" "-DJSON_BuildTests=OFF"
}

function BUILD_OLM {
    BUILD_LIBRARY "olm" 
}

function BUILD_LMDB {
    BUILD_LIBRARY lmdb 
    cp "${SCRIPT_DIR}/lmdbxx/lmdb++.h" "${DIST_DIR}/include/"
}

function BUILD_MTXCLIENT {
    target="$1"
    export OPENSSL_ROOT_DIR="${DIST_DIR}"
    export PKG_CONFIG_PATH="${DIST_DIR}/lib/pkgconfig"

    PRINT_INFO $OPENSSL_ROOT_DIR
    PRINT_INFO $PKG_CONFIG_PATH
    BUILD_LIBRARY "mtxclient" \
                  "-DBUILD_LIB_TESTS=OFF" \
                  "-DBUILD_LIB_EXAMPLES=OFF" \
                  "-DOlm_DIR=${DIST_DIR}/lib/cmake/Olm" \
                  "-Dnlohmann_json_DIR=${DIST_DIR}/lib/cmake/nlohmann_json" \
                  "-Dspdlog_DIR=${DIST_DIR}/lib/cmake/spdlog" \
                  "-DCMAKE_PREFIX_PATH=${DIST_DIR}/lib/pkgconfig" \
                  "-DOPENSSL_ROOT_DIR=${OPENSSL_ROOT_DIR}" \
                  "-DOPENSSL_INCLUDE_DIR=${OPENSSL_ROOT_DIR}/include" \
                  "-DOPENSSL_LIBRARIES=${OPENSSL_ROOT_DIR}/lib" \
                  "-DOPENSSL_CRYPTO_LIBRARY=${OPENSSL_ROOT_DIR}/lib/libcrypto.so" \
                  "-DOPENSSL_SSL_LIBRARY=${OPENSSL_ROOT_DIR}/lib/libssl.so" \
                  "-DBUILD_SHARED_LIBS=ON"
}

function BUILD_CMARK {
    BUILD_LIBRARY "cmark"
}

function BUILD_MATRIX-CLIENT-LIBRARY {
    BUILD_LIBRARY "matrix-client-library"
}

function BUILD_ALL {
    BUILD_SPDLOG
    BUILD_COEURL
    BUILD_JSON
    BUILD_OLM
    BUILD_LMDB
    BUILD_MTXCLIENT
    BUILD_CMARK
    BUILD_MATRIX-CLIENT-LIBRARY
}

###############################################################################
# Main
###############################################################################
unset LIBNAME
if [ $# -eq 0 ]; then
    echo "Usage: ./setup_android.sh LIBNAME"
    echo "      LIBNAME: 'ALL', name of directory"
    echo ""
    exit 1
else
    LIBNAME="${1^^}"
fi

PRINT_INFO "BUILD = $([ -z $LIBNAME ] && echo 'ALL' || echo ${LIBNAME^^})"

DIST_DIR="${SCRIPT_DIR}/dist/x86_64"
BUILD_DIR="${SCRIPT_DIR}/_build/x86_64"
MKPATH "$DIST_DIR"
MKPATH "$BUILD_DIR"

if [ "$TARGET" = "ALL" ]; then
    BUILD_TARGETS=( "armeabi-v7a"  "arm64-v8a" )
else
    BUILD_TARGETS=( $TARGET )
fi

$(eval echo "BUILD_${LIBNAME} $t")
