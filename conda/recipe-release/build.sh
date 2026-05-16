#!/bin/bash

set -euxo pipefail

rm -rf build
mkdir -p build
cd build

cmake ${CMAKE_ARGS} \
  -DCMAKE_INSTALL_PREFIX=${PREFIX} \
  -DCMAKE_INSTALL_LIBDIR=lib \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_EXE_LINKER_FLAGS="-fuse-ld=mold" \
  -DCMAKE_SHARED_LINKER_FLAGS="-fuse-ld=mold" \
  -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON \
  ${SRC_DIR}

make -j ${CPU_COUNT}

VERIPARSE_SOURCE_ROOT=${SRC_DIR} make ARGS=-j${CPU_COUNT} test

make install
