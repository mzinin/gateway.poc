#!/bin/bash

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"

rm -rf "${BUILD_DIR}"/* && echo "Done"