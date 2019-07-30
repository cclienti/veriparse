########################################
### Global options
########################################

cmake_minimum_required (VERSION 3.9)
project (veriparse)
include (${CMAKE_CURRENT_LIST_DIR}/cmake/common.cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)
enable_testing()


########################################
### git revision
########################################

include(GetGitRevisionDescription)
git_describe(VERIPARSE_VERSION --tags)
get_git_head_revision(GIT_REFSPEC VERIPARSE_GIT_SHA1)

message(STATUS "Veriparse version: ${VERIPARSE_VERSION}")
message(STATUS "Veriparse git sha1: ${VERIPARSE_GIT_SHA1}")


########################################
### install path
########################################

set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/veriparse-packages/veripase-${VERIPARSE_VERSION}")
message(STATUS "Install path in ${CMAKE_INSTALL_PREFIX}")


########################################
### veriparse lib
########################################

add_subdirectory(lib)


########################################
### apps directory
########################################

add_subdirectory(apps)
