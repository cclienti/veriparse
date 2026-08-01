########################################
### Global options
########################################

cmake_minimum_required (VERSION 3.15...3.27)
project (veriparse)
include (${CMAKE_CURRENT_LIST_DIR}/cmake/common.cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)
enable_testing()

# Serialises the link edges that carry gtest test discovery; see the comment on
# JOB_POOL_LINK in lib/test/CMakeLists.txt for what it protects against. Ninja
# only. Declared here so the pool exists before any target joins it.
set_property(GLOBAL APPEND PROPERTY JOB_POOLS gtest_discovery=1)


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

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
  set(VERIPARSE_BUILD_TYPE "debug")
else()
  set(VERIPARSE_BUILD_TYPE "release")
endif()

message(STATUS "Install path in ${CMAKE_INSTALL_PREFIX}")


########################################
### veriparse lib
########################################

add_subdirectory(lib)


########################################
### apps directory
########################################

add_subdirectory(apps)


