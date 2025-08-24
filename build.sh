#!/bin/bash

set -e

CMAKE_ARGS=""

while [[ $# -gt 0 ]]; do
    case "$1" in
        --debug)
            CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_BUILD_TYPE=Debug"
            shift
            ;;
        --release)
            CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release"
            shift
            ;;
        --i)
            INSTALL_MODE=1
            shift
            ;;
        --u)
            rm -rf /usr/local/include/conet
            rm -f /usr/local/lib/libconet.so
            rm -f /usr/local/lib/libconet.a
            rm -rf build
            rm -rf ./src/include
            echo "uninstall success!"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

HEADER_TARGET_DIR="./src/include/conet/"
mkdir -p "$HEADER_TARGET_DIR"
rsync -avm \
    --include='*/' \
    --include='*.h' \
    --exclude='*' \
    "./src/conet/" "$HEADER_TARGET_DIR"

# 构建
mkdir -p build
cd build
cmake .. $CMAKE_ARGS

if [ "$INSTALL_MODE" = "1" ]; then
    make -j4 install
    echo "install success!"
else
    make -j4
fi