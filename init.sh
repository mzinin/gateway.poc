#!/bin/bash

set -e

CURRENT_DIR="$(pwd)"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"

cd "${SCRIPT_DIR}" && \
git submodule init && \
git submodule update

cd "${CURRENT_DIR}"
