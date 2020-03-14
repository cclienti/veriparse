#!/bin/bash

set -euxo pipefail

mkdir build
cd build

cmake -DCMAKE_INSTALL_PREFIX=${PREFIX} -DCMAKE_INSTALL_LIBDIR=lib -DVERIPARSE_EXTERNAL_ROOT=${PREFIX} -DCMAKE_BUILD_TYPE=Release ${SRC_DIR}

make -j ${CPU_COUNT}

VERIPARSE_SOURCE_ROOT=${SRC_DIR} make ARGS=-j${CPU_COUNT} test

make install
