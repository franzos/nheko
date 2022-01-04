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
