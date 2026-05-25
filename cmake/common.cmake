cmake_minimum_required (VERSION 3.15...3.27)

if(NOT DEFINED VERIPARSE_COMMON_CMAKE)
  set(VERIPARSE_COMMON_CMAKE TRUE)

  message(STATUS "Using the veriparse common cmake listfile")
  message(STATUS "CMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}")
  include(FindPackageHandleStandardArgs)


  ########################################
  ### Custom Modules
  ########################################

  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/../cmake/modules")


  ########################################
  ### CCache
  ########################################

  option(ENABLE_CCACHE "Use ccache" OFF)

  if(ENABLE_CCACHE)
    find_program(CCACHE_FOUND ccache)
    if(CCACHE_FOUND)
      message(STATUS "Using CCache")
      set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
      set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
    endif(CCACHE_FOUND)
  endif()


  ########################################
  ### Compiler flags
  ########################################

  if(MINGW)
    string(APPEND CMAKE_CXX_FLAGS " -Wa,-mbig-obj -std=c++17 -Wall")
  else()
    string(APPEND CMAKE_CXX_FLAGS " -std=c++17 -Wall")
  endif()

  set(CMAKE_CXX_FLAGS_DEBUG  "-g3 -DDEBUG")
  set(CMAKE_CXX_FLAGS_FULLDEBUG  "-g3 -DDEBUG -DFULL_DEBUG")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-Werror -g3 -O3 -DDEBUG")
  set(CMAKE_CXX_FLAGS_RELEASE "-Werror -s -O3")
  set(CMAKE_CXX_FLAGS_COVERAGE "-g3 -DDEBUG -fprofile-arcs -ftest-coverage")


  ########################################
  ### Coverage
  ########################################

  if (CMAKE_BUILD_TYPE STREQUAL "Coverage")
	include(CodeCoverage)
  endif()

  if (CMAKE_BUILD_TYPE STREQUAL "Coverage")
	message(STATUS "Coverage: excluding ${CMAKE_PREFIX_PATH}")
	setup_target_for_coverage(coverage "make run_tests" coverage)
  endif()


  ########################################
  ### Libraries
  ########################################

  # Prefer the upstream GTestConfig.cmake (CONFIG mode) over CMake's
  # built-in FindGTest module: the conda-forge gtest package on Windows
  # ships GTestConfig with the right IMPORTED_IMPLIB path to the DLL's
  # import lib, which the module path doesn't pick up cleanly.
  find_package(GTest CONFIG REQUIRED)
  find_package(YAMLCPP REQUIRED)
  # GMP is built from source (see cmake/external_gmp.cmake) so we have a
  # single code path across Linux/macOS/Windows. conda-forge's win-64 gmp
  # has no gmpxx, so find_package(GMP) cannot be made to work there.
  include(${CMAKE_CURRENT_LIST_DIR}/external_gmp.cmake)
  # On Windows, conda-forge's BoostConfig defaults to static-linking variants
  # of each component, but the compile flags it injects assume DYN_LINK (so
  # headers reference symbols as __imp_*). Force the shared variant to keep
  # both sides consistent. Linux/macOS already use shared by default.
  set(Boost_USE_STATIC_LIBS OFF)
  find_package(Boost 1.85.0 CONFIG REQUIRED
	COMPONENTS system filesystem log program_options)

  message(STATUS "Found Boost: ${Boost_INCLUDE_DIRS}")


  ########################################
  ### RPATH configuration
  ########################################

  set(CMAKE_SKIP_BUILD_RPATH FALSE)
  set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

endif()
