########################################
### Global options
########################################

cmake_minimum_required (VERSION 3.15...3.27)
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


########################################
### Bundle runtime DLLs on Windows
########################################

# Make the .zip self-contained on a clean Windows install:
#   - MSVC runtime (vcruntime140, msvcp140, ...) via the CMake helper;
#     these usually ship with Windows Update but not on a vanilla install.
#   - vcpkg-supplied DLLs (boost, gmp, yaml-cpp, ...) from the build's
#     vcpkg_installed bin dir. install(TARGETS) only handles the .exe.
# Filter out gtest/gmock DLLs: they live in the same vcpkg bin dir but
# the shipping executables don't link them.
if(WIN32)
  include(InstallRequiredSystemLibraries)

  if(DEFINED VCPKG_TARGET_TRIPLET)
    set(_veriparse_vcpkg_bin
        ${CMAKE_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/bin)
    if(EXISTS ${_veriparse_vcpkg_bin})
      install(DIRECTORY ${_veriparse_vcpkg_bin}/
              DESTINATION bin
              FILES_MATCHING
              PATTERN "*.dll"
              PATTERN "gtest*.dll" EXCLUDE
              PATTERN "gmock*.dll" EXCLUDE)
    endif()
  endif()
endif()
