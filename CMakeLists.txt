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
### Bundle vcpkg runtime DLLs on Windows
########################################

# On Windows we link against vcpkg-supplied GMP (dynamic). Ship the DLLs
# alongside our binaries so the conda package is self-contained. vcpkg's
# CMake integration enables VCPKG_APPLOCAL_DEPS by default for x64-windows
# (copies DLLs into the build tree next to executables), but install(TARGETS)
# only installs the .exe, not adjacent DLLs. Explicitly install everything
# in vcpkg's bin dir for our triplet.
if(WIN32 AND DEFINED VCPKG_TARGET_TRIPLET)
  set(_veriparse_vcpkg_bin
      ${CMAKE_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/bin)
  if(EXISTS ${_veriparse_vcpkg_bin})
    install(DIRECTORY ${_veriparse_vcpkg_bin}/
            DESTINATION bin
            FILES_MATCHING PATTERN "*.dll")
  endif()
endif()
