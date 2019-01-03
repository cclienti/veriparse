########################################
### Global options
########################################

cmake_minimum_required (VERSION 3.9)
project (veriparse-external)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

include(ExternalProject)

include (external-boost.cmake)
include (external-gmp.cmake)
include (external-googletest.cmake)
include (external-yaml-cpp.cmake)
include (external-openssl.cmake)


########################################
### Veriparse external project
########################################

ExternalProject_Add(veriparse
  DEPENDS           boost-url gmp-url yaml-cpp-git googletest-git
  SOURCE_DIR        ${CMAKE_CURRENT_LIST_DIR}/..
  CMAKE_ARGS        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DVERIPARSE_EXTERNAL_ROOT_PATH=${CMAKE_CURRENT_BINARY_DIR}/external
  UPDATE_COMMAND    ""
  INSTALL_COMMAND   VERIPARSE_SOURCE_ROOT=${CMAKE_CURRENT_LIST_DIR}/.. make -j1 test)
