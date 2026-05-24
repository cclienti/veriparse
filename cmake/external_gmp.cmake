# cmake/external_gmp.cmake
#
# Build GMP from source via ExternalProject for cross-platform reproducibility.
# conda-forge's `gmp` package on win-64 ships only the C library (no gmpxx),
# so we build our own with --enable-cxx everywhere to keep one code path.
#
# Static linking. LGPLv3 §4 requires that users be able to relink against a
# modified GMP; the conda package ships the build recipe (which pins the
# GMP source URL+SHA256 below) and veriparse itself is LGPLv3, so that
# condition is met. Static avoids RPATH/install_name gymnastics and Windows
# DLL search-path issues, and keeps the package self-contained.

include(ExternalProject)
include(GNUInstallDirs)

set(VERIPARSE_GMP_VERSION 6.3.0)
# Multiple URLs; CMake tries each in order until one succeeds. gmplib.org's
# CDN has been unreachable from some CI runners (notably GitHub Actions
# windows-2022), so list the GNU mirror first for reliability.
set(VERIPARSE_GMP_URLS
  https://ftp.gnu.org/gnu/gmp/gmp-${VERIPARSE_GMP_VERSION}.tar.bz2
  https://ftpmirror.gnu.org/gmp/gmp-${VERIPARSE_GMP_VERSION}.tar.bz2
  https://gmplib.org/download/gmp/gmp-${VERIPARSE_GMP_VERSION}.tar.bz2
)
set(VERIPARSE_GMP_SHA256 ac28211a7cfb609bae2e2c8d6058d66c8fe96434f740cf6fe2e47b000d1c20cb)

set(VERIPARSE_GMP_PREFIX  ${CMAKE_BINARY_DIR}/external/gmp)
set(VERIPARSE_GMP_INSTALL ${VERIPARSE_GMP_PREFIX}/install)

# Static lib filenames are uniform across platforms.
set(VERIPARSE_GMP_LIB   ${VERIPARSE_GMP_INSTALL}/lib/libgmp.a)
set(VERIPARSE_GMPXX_LIB ${VERIPARSE_GMP_INSTALL}/lib/libgmpxx.a)

# GMP 6.3.0's configure-time "long long reliability" test uses K&R-style
# empty parens, which GCC 15+ now interprets per C23 as "no parameters"
# instead of "unspecified". Pin to gnu17 to make configure pass; this only
# affects the GMP build, not veriparse's own compiler flags. Preserve any
# CFLAGS/CXXFLAGS injected by the build environment (conda-build sets
# -fdebug-prefix-map, -march, ...) by appending rather than replacing.
set(_gmp_cflags "$ENV{CFLAGS} -std=gnu17 -fPIC")
set(_gmp_cxxflags "$ENV{CXXFLAGS} -std=gnu++17 -fPIC")
set(VERIPARSE_GMP_CONFIG_ENV
  ${CMAKE_COMMAND} -E env
    "CFLAGS=${_gmp_cflags}"
    "CXXFLAGS=${_gmp_cxxflags}"
)

ExternalProject_Add(veriparse_gmp_external
  PREFIX            ${VERIPARSE_GMP_PREFIX}
  URL               ${VERIPARSE_GMP_URLS}
  URL_HASH          SHA256=${VERIPARSE_GMP_SHA256}
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  CONFIGURE_COMMAND ${VERIPARSE_GMP_CONFIG_ENV}
                    <SOURCE_DIR>/configure
                      --prefix=${VERIPARSE_GMP_INSTALL}
                      --enable-static
                      --enable-cxx
                      --disable-shared
                      --with-pic
  BUILD_COMMAND     make -j
  INSTALL_COMMAND   make install
  BUILD_BYPRODUCTS  ${VERIPARSE_GMP_LIB} ${VERIPARSE_GMPXX_LIB}
)

# CMake requires INTERFACE_INCLUDE_DIRECTORIES to exist at configure time even
# for IMPORTED targets; the directory will be populated at build time.
file(MAKE_DIRECTORY ${VERIPARSE_GMP_INSTALL}/include)

add_library(GMP::GMP STATIC IMPORTED GLOBAL)
set_target_properties(GMP::GMP PROPERTIES
  IMPORTED_LOCATION             ${VERIPARSE_GMP_LIB}
  INTERFACE_INCLUDE_DIRECTORIES ${VERIPARSE_GMP_INSTALL}/include
)
add_dependencies(GMP::GMP veriparse_gmp_external)

add_library(GMP::GMPXX STATIC IMPORTED GLOBAL)
set_target_properties(GMP::GMPXX PROPERTIES
  IMPORTED_LOCATION             ${VERIPARSE_GMPXX_LIB}
  INTERFACE_INCLUDE_DIRECTORIES ${VERIPARSE_GMP_INSTALL}/include
  INTERFACE_LINK_LIBRARIES      GMP::GMP
)
add_dependencies(GMP::GMPXX veriparse_gmp_external)

# Static libs get baked into the binaries; no install rule needed.
