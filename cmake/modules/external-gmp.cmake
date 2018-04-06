set(GMP_URL_PREFIX_DIR ${CMAKE_CURRENT_BINARY_DIR}/external/gmp)
set(GMP_URL_INSTALL_DIR ${GMP_URL_PREFIX_DIR}/install)

message(STATUS "External project GMP installation in ${GMP_URL_INSTALL_DIR}")

set(GMP_URL            https://gmplib.org/download/gmp/gmp-6.1.2.tar.bz2)
set(GMP_SHA256         5275bb04f4863a13516b2f39392ac5e272f5e1bb8057b18aec1c9b79d73d8fb2)
set(GMP_BOOTSTRAP_CMD  ./configure --prefix=${GMP_URL_INSTALL_DIR} --build=amd64 --disable-shared --enable-cxx)
set(GMP_BUILD_CMD      make -j8)
set(GMP_INSTALL_CMD    make -j8 check install)

set(GMP_LIBRARY gmp)
get_filename_component(GMP_LIBRARY_PATH
  ${GMP_URL_INSTALL_DIR}/lib/${CMAKE_FIND_LIBRARY_PREFIXES}${GMP_LIBRARY}${CMAKE_STATIC_LIBRARY_SUFFIX}
  REALPATH)

set(GMPXX_LIBRARY gmpxx)
get_filename_component(GMPXX_LIBRARY_PATH
  ${GMP_URL_INSTALL_DIR}/lib/${CMAKE_FIND_LIBRARY_PREFIXES}${GMPXX_LIBRARY}${CMAKE_STATIC_LIBRARY_SUFFIX}
  REALPATH)

ExternalProject_Add(gmp-url
    BUILD_IN_SOURCE   1
    PREFIX            ${CMAKE_CURRENT_BINARY_DIR}/external/gmp
    URL               ${GMP_URL}
    URL_HASH          SHA256=${GMP_SHA256}
    UPDATE_COMMAND    ""
    PATCH_COMMAND     ""
    CONFIGURE_COMMAND ${GMP_BOOTSTRAP_CMD}
    BUILD_COMMAND     ${GMP_BUILD_CMD}
    INSTALL_COMMAND   ${GMP_INSTALL_CMD}
    BUILD_BYPRODUCTS  ${GMP_LIBRARY_PATH} ${GMPXX_LIBRARY_PATH}

)

set_directory_properties(PROPERTY CLEAN_NO_CUSTOM ${GMP_URL_PREFIX_DIR})

# Include directory
set(GMP_INCLUDE_DIR ${GMP_URL_INSTALL_DIR}/include)

# Lib declaration
add_library(${GMP_LIBRARY} STATIC IMPORTED GLOBAL)
add_dependencies(${GMP_LIBRARY} gmp-url)
set_property(TARGET ${GMP_LIBRARY} PROPERTY IMPORTED_LOCATION ${GMP_LIBRARY_PATH})

add_library(${GMPXX_LIBRARY} STATIC IMPORTED GLOBAL)
add_dependencies(${GMPXX_LIBRARY} gmp-url)
set_property(TARGET ${GMPXX_LIBRARY} PROPERTY IMPORTED_LOCATION ${GMPXX_LIBRARY_PATH})

set(GMP_LIBRARIES ${GMPXX_LIBRARY} ${GMP_LIBRARY})
