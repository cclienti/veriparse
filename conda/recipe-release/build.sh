#!/bin/bash

set -euxo pipefail

mkdir build
cd build

cmake -DCMAKE_INSTALL_PREFIX=${PREFIX} -DCMAKE_INSTALL_LIBDIR=lib -DVERIPARSE_EXTERNAL_ROOT=${PREFIX} -DCMAKE_BUILD_TYPE=Release ..

make -j ${CPU_COUNT}

VERIPARSE_SOURCE_ROOT=$(realpath ${SRC_DIR}) ctest -j ${CPU_COUNT}

make install
