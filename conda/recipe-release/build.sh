#!/bin/bash

set -euxo pipefail

rm -rf build
mkdir -p build
cd build

# mold linker is only supported on Linux
if [[ "${target_platform}" == linux-* ]]; then
  MOLD_FLAGS="-DCMAKE_EXE_LINKER_FLAGS=-fuse-ld=mold -DCMAKE_SHARED_LINKER_FLAGS=-fuse-ld=mold"
else
  MOLD_FLAGS=""
fi

# clang on macOS rejects unused flags and fields in generated code as errors
if [[ "${target_platform}" == osx-* ]]; then
  EXTRA_FLAGS="-DCMAKE_C_FLAGS=-Wno-unused-command-line-argument -DCMAKE_CXX_FLAGS='-Wno-unused-command-line-argument -Wno-unused-private-field'"
else
  EXTRA_FLAGS=""
fi

cmake ${CMAKE_ARGS} \
  -DCMAKE_PREFIX_PATH=${PREFIX} \
  -DCMAKE_INSTALL_PREFIX=${PREFIX} \
  -DCMAKE_INSTALL_LIBDIR=lib \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON \
  ${MOLD_FLAGS} \
  ${EXTRA_FLAGS} \
  ${SRC_DIR}

make -j ${CPU_COUNT}

VERIPARSE_SOURCE_ROOT=${SRC_DIR} make ARGS="-j${CPU_COUNT} -L unittest" test

make install
