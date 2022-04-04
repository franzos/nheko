#!/usr/bin/env bash

function PRINT_INFO {
    echo -e "[INF]: $@"
}

function PRINT_WARNING {
    echo -e "[WRN]: $@"
}

function PRINT_ERROR {
    echo -e "[ERR]: $@"
}

function PRINT_ERROR_EXIT {
    PRINT_ERROR $@
    exit -1
}

function CHECK_VARIABLE_SET {
     if [ ! $(eval echo "\${${1}+x}") ]; then
        PRINT_ERROR_EXIT "$1 is not set"
    fi
    PRINT_INFO "Check \$$1 = $(eval echo \"\${${1}}\")"
}

function MKPATH {
    [ ! -d "$1" ] && PRINT_INFO "Create $1"
    [ ! -d "$1" ] && mkdir -p "$1"
}

function VERIFY_CHECKSUM {
    if [  ! "$#" -eq 2 ]; then 
        PRINT_ERROR_EXIT "VERIFY_CHECKSUM - missing params: FILE_PATH SIGNATURE_PATH"
    fi
    [ -f "$1" ] || PRINT_ERROR_EXIT "file not found: $1"
    [ -f "$2" ] || PRINT_ERROR_EXIT "signature not found: $1"

    file_hash=$(cat $1 | sha256sum | head -c 64)
    signature=$(cat $2)

    if [ "$file_hash" != "$signature" ]; then 
        return 1
    fi
    return 0;
    # return [ "$file_hash" = "$signature" ] || PRINT_ERROR_EXIT "Archive signature mismatch"
    
}

