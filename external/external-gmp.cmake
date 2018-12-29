include(ProcessorCount)
ProcessorCount(GMP_PROC)

set(GMP_URL_PREFIX_DIR ${CMAKE_CURRENT_BINARY_DIR}/external/gmp)
set(GMP_URL_INSTALL_DIR ${GMP_URL_PREFIX_DIR}/install)

message(STATUS "External project GMP installation in ${GMP_URL_INSTALL_DIR}")

set(GMP_URL            https://gmplib.org/download/gmp/gmp-6.1.2.tar.bz2)
set(GMP_SHA256         5275bb04f4863a13516b2f39392ac5e272f5e1bb8057b18aec1c9b79d73d8fb2)
set(GMP_BOOTSTRAP_CMD  ./configure --prefix=${GMP_URL_INSTALL_DIR} --build=amd64 --disable-shared --enable-cxx)
set(GMP_BUILD_CMD      make -j${GMP_PROC})

ExternalProject_Add(gmp-url
    BUILD_IN_SOURCE   1
    PREFIX            ${CMAKE_CURRENT_BINARY_DIR}/external/gmp
    URL               ${GMP_URL}
    URL_HASH          SHA256=${GMP_SHA256}
    UPDATE_COMMAND    ""
    PATCH_COMMAND     ""
    CONFIGURE_COMMAND ${GMP_BOOTSTRAP_CMD}
    BUILD_COMMAND     ${GMP_BUILD_CMD}
)

set_directory_properties(PROPERTY CLEAN_NO_CUSTOM ${GMP_URL_PREFIX_DIR})
