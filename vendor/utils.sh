#!/usr/bin/env  bash

##### [Global Variables] ######################################################

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
TEMP_DIR="$SCRIPT_DIR/_temp"
BUILD_DIR="$SCRIPT_DIR/_build"
DIST_DIR="$SCRIPT_DIR/_dist"
PATCH_DIR="$SCRIPT_DIR/patches"

##### [Utility Functions] #####################################################

function ECHO {
    echo -e "[INF] $@"
}

function ECHO_DEBUG {
    if [ ! -z $DEBUG ]; then
        echo -e "[DBG] $@"
    fi
}

function ECHO_ERR {
    echo -e "[ERR] $@" >&2
}

function DOWNLOAD_EXTRACT {
    name="$1"
    version="$2"
    url="$3"
    name_version="$name-$version"
    archive_path="$TEMP_DIR/$name_version.tar.gz"
    src_path="$TEMP_DIR/$name_version"
    build_path="$BUILD_DIR/$target/$name_version"

    [ ! -f "$archive_path" ] && wget -O "$archive_path" "$url" || ECHO "Use cached version: $archive_path"
    [ ! -d "$src_path" ] && tar xf $archive_path --directory "$TEMP_DIR"
}

function FETCH_REPOSITORY {
    name="$1"
    tag="$2"
    repo="$3"
    src_path="$TEMP_DIR/$name-$tag"
    build_path="$BUILD_DIR/$target/$name-$tag"
    
    if [ ! -d "$src_path" ]; then
        git clone --branch "$tag" --depth 1 "$repo" "$src_path"
        return 0
    else
        ECHO "Use cached version: $src_path"
        return 1
    fi
}

function APPLY_PATCH {
    params=( "$@" )
    repo=${params[0]}
    unset params[0]

    git -C "$src_path" config commit.gpgsign off
    git -C "$src_path" am ${params[@]}
}
