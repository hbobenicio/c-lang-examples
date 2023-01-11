#!/bin/bash
set -eu -o pipefail

BUILD_DIR="$PWD/build-wasm"
EMSDK_ROOT="$HOME/repos/github/emscripten-core/emsdk-3.1.20"

echo "=== sourcing emsdk_env.sh ==="
. "$EMSDK_ROOT/emsdk_env.sh"

echo

echo "=== emcc ==="
emcc --version
export CC=emcc

echo "=== em++ ==="
em++ --version
export CXX=em++

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

cmake .. -DCMAKE_BUILD_TYPE=Debug -DEMSCRIPTEN:bool=ON
make -j8 VERBOSE=1
