#!/bin/bash

set -euxo pipefail

# export CFLAGS="$(echo $CFLAGS | sed 's/-march=[^ ]*/-march=x86-64/g') -mtune=generic"
# export CXXFLAGS="$(echo $CXXFLAGS | sed 's/-march=[^ ]*/-march=x86-64/g') -mtune=generic"

mkdir build-pkg
cd build-pkg

cmake -DCMAKE_CXX_FLAGS="-march=x86-64 -mtune=generic" -DCMAKE_C_FLAGS="-march=x86-64 -mtune=generic" -DCMAKE_INSTALL_PREFIX=${PREFIX} -DVERIPARSE_EXTERNAL_ROOT=${PREFIX} -DCMAKE_BUILD_TYPE=Release ${SRC_DIR}

make -j ${CPU_COUNT}

VERIPARSE_SOURCE_ROOT=${SRC_DIR} make ARGS=-j${CPU_COUNT} test

make install
