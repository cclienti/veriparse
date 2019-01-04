if(NOT VERIPARSE_PROCESSOR_COUNT)
  set(VERIPARSE_PROCESSOR_COUNT 1)
  message(STATUS "VERIPARSE_PROCESSOR_COUNT not set, forcing to 1")
endif()

set(GMP_URL_PREFIX_DIR ${CMAKE_CURRENT_BINARY_DIR}/external/gmp)
set(GMP_URL_INSTALL_DIR ${GMP_URL_PREFIX_DIR}/install)

message(STATUS "External project gmp installation in ${GMP_URL_INSTALL_DIR}")

ExternalProject_Add(gmp-ext
    BUILD_IN_SOURCE   1
    PREFIX            ${CMAKE_CURRENT_BINARY_DIR}/external/gmp
    URL               https://gmplib.org/download/gmp/gmp-6.1.2.tar.bz2
    URL_HASH          SHA256=5275bb04f4863a13516b2f39392ac5e272f5e1bb8057b18aec1c9b79d73d8fb2
    UPDATE_COMMAND    ""
    PATCH_COMMAND     ""
    CONFIGURE_COMMAND ./configure --prefix=${GMP_URL_INSTALL_DIR} --build=amd64
                                  --disable-shared --enable-cxx
    BUILD_COMMAND     make -j${VERIPARSE_PROCESSOR_COUNT}
)

set_directory_properties(PROPERTY CLEAN_NO_CUSTOM ${GMP_URL_PREFIX_DIR})
