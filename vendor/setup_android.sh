#!/usr/bin/env bash
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
STARTUP_DIR=$( pwd )
source "${SCRIPT_DIR}/utils.sh"
CORES=$((`nproc`+1))

###############################################################################
# Variables
###############################################################################
CHECK_VARIABLE_SET ANDROID_HOME
CHECK_VARIABLE_SET ANDROID_NDK
CMAKE_TOOLCHAIN="${ANDROID_NDK}/build/cmake/android.toolchain.cmake"
MIN_SDK_VERSION=31

###############################################################################
# Logic
###############################################################################
function BUILD_LIBRARY {
    # USAGE: BUILD_LIBRARY LIBNAME TARGET [OPTIONAL PARAMS]
    params=( "$@" )
    libname=${params[0]}
    target=${params[1]}
    [[ -z "$libname" ]] && PRINT_ERROR_EXIT "LIBNAME is not provided"
    [[ -z "$target" ]] && PRINT_ERROR_EXIT "TARGET is not provided"
    unset params[1]
    unset params[0]

    echo ""
    PRINT_INFO "======= Build: [$libname] for [$target] ======================="

    libpath="${BUILD_DIR}/${target}/${libname}"
    [ -d "$libpath" ] && rm -rf "$libpath"

    cmake -DCMAKE_TOOLCHAIN_FILE=$CMAKE_TOOLCHAIN \
        -DANDROID_ABI=$target -DANDROID_PLATFORM=android-$MIN_SDK_VERSION \
        -DCMAKE_INSTALL_PREFIX="${DIST_DIR}/${target}" \
        -DCMAKE_SHARED_LINKER_FLAGS="-L${DIST_DIR}/$target/lib" \
        -DCMAKE_BUILD_TYPE=Release \
        -S"${SCRIPT_DIR}/${libname}" -B"${libpath}" ${params[@]}
    
    cmake --build $libpath --config Release
    cmake --install $libpath --config Release
}

###############################################################################
# Main
###############################################################################
unset TARGET
unset LIBNAME
if [ $# -eq 0 ]; then
    echo "Usage: ./setup_android.sh LIBNAME [TARGET]"
    echo "      LIBNAME: 'ALL', name of directory"
    echo "      TARGET: 'ALL', 'armeabi-v7a', 'arm64-v8a'"
    echo ""
    exit 1
elif [ $# -eq 1 ]; then
    LIBNAME="${1^^}"
    TARGET="ALL"
elif [ $# -eq 2 ]; then
    LIBNAME="${1^^}"
    TARGET="$2"
fi

PRINT_INFO "BUILD = $([ -z $LIBNAME ] && echo 'ALL' || echo ${LIBNAME^^})"
PRINT_INFO "TARGET = $TARGET"

DIST_DIR="${SCRIPT_DIR}/dist/android"
BUILD_DIR="${SCRIPT_DIR}/_build/android"
MKPATH "$DIST_DIR"
MKPATH "$BUILD_DIR"

if [ "$TARGET" = "ALL" ]; then
    BUILD_TARGETS=( "armeabi-v7a"  "arm64-v8a" )
else
    BUILD_TARGETS=( $TARGET )
fi

if [ "$LIBNAME" = "ALL" ]; then
    for t in ${BUILD_TARGETS[@]}; do
        BUILD_ALL $t
    done
else
    for t in ${BUILD_TARGETS[@]}; do
        $(eval echo "BUILD_${LIBNAME} $t")
    done
fi
