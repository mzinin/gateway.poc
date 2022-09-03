#!/bin/bash

set -e

BUILD_TYPE=RELEASE

while [[ $# -gt 0 ]]; do
    key="$1"

    case $key in
        -m|--mode)
            BUILD_TYPE=$(echo "$2" | awk '{print toupper($0)}')
            shift
            shift
            ;;

        *)
            shift
            ;;
    esac
done

CURRENT_DIR="$(pwd)"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"
BUILD_SUB_DIR=$(echo "$BUILD_TYPE" | awk '{print tolower($0)}')
CMAKE_BUILD_DIR="${SCRIPT_DIR}/build/${BUILD_SUB_DIR}"

# run CMake, make and install
mkdir -p "${CMAKE_BUILD_DIR}" && \
cd "${CMAKE_BUILD_DIR}" && \
cmake ../../ -DCMAKE_BUILD_TYPE=${BUILD_TYPE} && \
make -j$(nproc)
make install

cd "${CURRENT_DIR}"
