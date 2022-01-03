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

function PREPARE_AUTOMAKE_ENVIRONMENT {
    # # Only choose one of these, depending on your device...
    # export TARGET=aarch64-linux-android
    # export TARGET=armv7a-linux-androideabi
    # export TARGET=i686-linux-android
    # export TARGET=x86_64-linux-android
    if [ "$1" = "armeabi-v7a" ]; then
        export TARGET_HOST="armv7a-linux-androideabi"
        export OS_COMPILER='android-arm'
    elif [ "$1" = "arm64-v8a" ]; then
        export TARGET_HOST="aarch64-linux-android"
        export OS_COMPILER='android-arm64'
    else
        PRINT_ERROR "unsupported target: $1"
        return 1
    fi

    export ANDROID_NDK_HOME=$ANDROID_NDK
    export TOOLCHAIN="$ANDROID_NDK_HOME/toolchains/llvm/prebuilt/linux-x86_64"
    export PATH=$TOOLCHAIN/bin:$PATH
    
    export ANDROID_ARCH=$1
    export AR=$TOOLCHAIN/bin/llvm-ar
    export CC=$TOOLCHAIN/bin/$TARGET_HOST$MIN_SDK_VERSION-clang
    export AS=$CC
    export CXX=$TOOLCHAIN/bin/$TARGET_HOST$MIN_SDK_VERSION-clang++
    export LD=$TOOLCHAIN/bin/ld
    export RANLIB=$TOOLCHAIN/bin/llvm-ranlib
    export STRIP=$TOOLCHAIN/bin/llvm-strip
    return 0
}

###############################################################################
# Library specific functions
###############################################################################
function BUILD_SPDLOG {
    target="$1"
    BUILD_LIBRARY 'spdlog' $target \
        '-DSPDLOG_BUILD_SHARED=ON' \
        '-DSPDLOG_BUILD_EXAMPLE=OFF' \
        '-DSPDLOG_BUILD_TESTS=OFF'
}

function BUILD_OPENSSL {
    libname='openssl'
    version="1.1.1l"
    target="$1"
    PRINT_INFO "======= Build: [$libname] for [$target] ======================="
    
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

    PREPARE_AUTOMAKE_ENVIRONMENT $target
    [ $? -ne 0 ] && PRINT_ERROR_EXIT "Failed to prepare build environment."

    cd $target_dir
    ./Configure $OS_COMPILER shared \
        -D__ANDROID_API__=$MIN_SDK_VERSION \
        --prefix="$DIST_DIR/$target"
    make -j$CORES
    make install_sw
    make clean
    cd "$STARTUP_DIR"
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
