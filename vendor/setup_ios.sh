#!/bin/bash
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
STARTUP_DIR=$( pwd )
source "${SCRIPT_DIR}/utils.sh"

###############################################################################
# Variables
###############################################################################


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
    PRINT_INFO "======= Build: [$libname] for [iOS] ======================="

    libpath="${BUILD_DIR}/${libname}"
    [ -d "$libpath" ] && rm -rf "$libpath"

    cmake -G Xcode -DCMAKE_TOOLCHAIN_FILE="${SCRIPT_DIR}/ios.toolchain.cmake" -DPLATFORM=OS64COMBINED \
        -DCMAKE_INSTALL_PREFIX=${DIST_DIR} \
        -DCMAKE_BUILD_TYPE=Release \
        -S"${SCRIPT_DIR}/${libname}" -B"${libpath}" ${params[@]}
    
    cmake --build $libpath --config Release
    cmake --install $libpath --config Release
}

###############################################################################
# Library specific functions
###############################################################################
function BUILD_SPDLOG {
    BUILD_LIBRARY "spdlog" \
        "-DSPDLOG_BUILD_EXAMPLE=OFF"
}

function BUILD_OPENSSL {
    echo ""
    PRINT_INFO "======= Build: [OpenSSL] for [iOS] ======================="
    libpath="${BUILD_DIR}/openssl-xcframeworks"
    [ -d $libpath ] && rm -rf "$libpath"
    git clone https://github.com/adib/openssl-xcframeworks.git $libpath  --depth=1
    
    cd $libpath
    ./build-openssl.sh --version=1.1.1l --targets='ios-sim-cross-x86_64 ios64-cross'

    cp lib/libssl-iPhone.a "${DIST_DIR}/lib/libssl.a"
    cp lib/libcrypto-iPhone.a "${DIST_DIR}/lib/libcrypto.a"
    cp -R include/openssl "${DIST_DIR}/include/"

    lipo -info "${DIST_DIR}/lib/libssl.a"
    lipo -info "${DIST_DIR}/lib/libcrypto.a"
    
    cd $STARTUP_DIR
}

function BUILD_CURL {
    libname="curl"
    version="7.80.0"
    version_underlined=$(echo $version | sed s/[.]/_/g)

    echo ""
    PRINT_INFO "======= Build: [$libname] for [iOS] ======================="

    download_url="https://github.com/curl/curl/releases/download/curl-$version_underlined/curl-$version.tar.gz"
    libpath="$BUILD_DIR/$libname"
    zip_name="$libname-$version.tar.gz"
    zip_path="$libpath/$zip_name"

    MKPATH $libpath
    [ ! -f $zip_path ] && wget -O "$zip_path" $download_url || PRINT_INFO "use cached archive: $zip_path"

    target_dir="$libpath/$libname-$version"
    [ -d "$target_dir" ] && rm -rf "$target_dir"
    tar xf "$zip_path" --directory "$libpath"

    cmake -G Xcode -DCMAKE_TOOLCHAIN_FILE="${SCRIPT_DIR}/ios.toolchain.cmake" -DPLATFORM=OS64COMBINED \
        -DCMAKE_INSTALL_PREFIX=${DIST_DIR} \
        -DCMAKE_BUILD_TYPE=Release \
        -S"${target_dir}" -B"${libpath}" \
        -DCMAKE_USE_OPENSSL=OFF   \
        -DBUILD_SHARED_LIBS=OFF   \
        -DBUILD_CURL_EXE=OFF      \
        -DBUILD_TESTING=FALSE     \
        -DHAVE_LIBIDN2=FALSE      \
        -DCURL_CA_PATH=none       \
        -DCURL_DISABLE_FTP=ON     \
        -DCURL_DISABLE_LDAP=ON    \
        -DCURL_DISABLE_LDAPS=ON   \
        -DCURL_DISABLE_TELNET=ON  \
        -DCURL_DISABLE_DICT=ON    \
        -DCURL_DISABLE_FILE=ON    \
        -DCURL_DISABLE_TFTP=ON    \
        -DCURL_DISABLE_RTSP=ON    \
        -DCURL_DISABLE_POP3=ON    \
        -DCURL_DISABLE_IMAP=ON    \
        -DCURL_DISABLE_SMTP=ON    \
        -DCURL_DISABLE_GOPHER=ON  \
        -DUSE_MANUAL=OFF

    cmake --buil  d $libpath --config Release
    cmake --install $libpath --config Release
}

function BUILD_ALL {
    BUILD_SPDLOG
    BUILD_CURL
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
    LIBNAME=$(echo "$1" |  tr '[:lower:]' '[:upper:]')
fi

PRINT_INFO "BUILD = $([ -z $LIBNAME ] && echo 'ALL' || echo ${LIBNAME})"

DIST_DIR="${SCRIPT_DIR}/dist/ios"
BUILD_DIR="${SCRIPT_DIR}/_build/ios"
MKPATH "$DIST_DIR"
MKPATH "$BUILD_DIR"

if [ "$LIBNAME" = "ALL" ]; then
    BUILD_ALL
else
    $(eval echo "BUILD_${LIBNAME}")
fi
