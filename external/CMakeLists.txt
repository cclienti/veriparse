########################################
### Global options
########################################

cmake_minimum_required (VERSION 3.9)
project (veriparse-external)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)


########################################
### Count number of processors
########################################

include(ProcessorCount)
ProcessorCount(VERIPARSE_PROCESSOR_COUNT)

if(VERIPARSE_PROCESSOR_COUNT LESS 8)
  set(VERIPARSE_PROCESSOR_COUNT 1)
endif()

message(STATUS "Bulding using ${VERIPARSE_PROCESSOR_COUNT} core(s)")


########################################
### Include external projects
########################################

include(ExternalProject)

set(VERIPARSE_EXTERNALS boost gmp googletest yaml-cpp openssl)

foreach(VER_EXT ${VERIPARSE_EXTERNALS})
  set(VERIPARSE_EXTERNAL_DEPS ${VERIPARSE_EXTERNAL_DEPS} ${VER_EXT}-ext)
  include(external-${VER_EXT}.cmake)
endforeach()


########################################
### Veriparse external project
########################################

ExternalProject_Add(veriparse
  DEPENDS           ${VERIPARSE_EXTERNAL_DEPS}
  SOURCE_DIR        ${CMAKE_CURRENT_LIST_DIR}/..
  CMAKE_ARGS        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DVERIPARSE_EXTERNAL_ROOT=${CMAKE_CURRENT_BINARY_DIR}/external
  UPDATE_COMMAND    ""
  INSTALL_COMMAND   VERIPARSE_SOURCE_ROOT=${CMAKE_CURRENT_LIST_DIR}/.. make -j1 test)
