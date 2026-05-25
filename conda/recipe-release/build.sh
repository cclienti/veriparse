#!/bin/bash

set -euxo pipefail

rm -rf build
mkdir -p build
cd build

# Use a bash array so flags containing spaces survive expansion intact.
EXTRA_CMAKE_OPTS=()

# mold linker is only supported on Linux
if [[ "${target_platform}" == linux-* ]]; then
  EXTRA_CMAKE_OPTS+=(
    -DCMAKE_EXE_LINKER_FLAGS=-fuse-ld=mold
    -DCMAKE_SHARED_LINKER_FLAGS=-fuse-ld=mold
  )
fi

# clang complains about some of conda's injected toolchain flags
# (e.g. -fdebug-prefix-map) being unused for C++; with -Werror that's fatal.
if [[ "${target_platform}" == osx-* ]]; then
  EXTRA_CMAKE_OPTS+=(
    -DCMAKE_C_FLAGS=-Wno-unused-command-line-argument
    -DCMAKE_CXX_FLAGS=-Wno-unused-command-line-argument
  )
fi

# Windows runs inside MSYS2 UCRT64 via bld.bat. Force the Unix Makefiles
# generator (otherwise CMake may try Visual Studio) and prepend $PREFIX/bin
# so the conda env's tools (cmake, gcc, make, ...) win over MSYS2's.
if [[ "${target_platform}" == win-* ]]; then
  EXTRA_CMAKE_OPTS+=(-G "Unix Makefiles")
  EXTRA_CMAKE_OPTS+=(-DCMAKE_VERBOSE_MAKEFILE=ON)
  export PATH="${PREFIX}/Library/bin:${PREFIX}/bin:${PATH}"
  # Dump conda's injected toolchain envs for diagnostics.
  echo "--- LDFLAGS:   ${LDFLAGS:-}"
  echo "--- LDFLAGS_LD:${LDFLAGS_LD:-}"
  echo "--- CFLAGS:    ${CFLAGS:-}"
  echo "--- CXXFLAGS:  ${CXXFLAGS:-}"
fi

cmake ${CMAKE_ARGS} \
  -DCMAKE_PREFIX_PATH=${PREFIX} \
  -DCMAKE_INSTALL_PREFIX=${PREFIX} \
  -DCMAKE_INSTALL_LIBDIR=lib \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON \
  "${EXTRA_CMAKE_OPTS[@]}" \
  ${SRC_DIR}

make -j ${CPU_COUNT}

VERIPARSE_SOURCE_ROOT=${SRC_DIR} ctest -j${CPU_COUNT} -L unittest --output-on-failure

make install
