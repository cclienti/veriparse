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

  if(MSVC)
    # MSVC doesn't speak GCC-style flags. Standard is set via CMAKE_CXX_STANDARD
    # below, /W3 is the default warning level. Skip /WX (warnings as errors)
    # because MSVC warns about a different set than GCC and a clean -Wall
    # Windows build is a separate effort.
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
    string(APPEND CMAKE_CXX_FLAGS " /EHsc /bigobj")
    set(CMAKE_CXX_FLAGS_DEBUG  "/Zi /Od /DDEBUG")
    set(CMAKE_CXX_FLAGS_FULLDEBUG  "/Zi /Od /DDEBUG /DFULL_DEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/Zi /O2 /DDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2")
  else()
    string(APPEND CMAKE_CXX_FLAGS " -std=c++17 -Wall")
    set(CMAKE_CXX_FLAGS_DEBUG  "-g3 -DDEBUG")
    set(CMAKE_CXX_FLAGS_FULLDEBUG  "-g3 -DDEBUG -DFULL_DEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-Werror -g3 -O3 -DDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "-Werror -s -O3")
    set(CMAKE_CXX_FLAGS_COVERAGE "-g3 -DDEBUG -fprofile-arcs -ftest-coverage")
  endif()


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
  # GMP comes from different sources per platform:
  #  - Linux/macOS: conda-forge `gmp` (host: requirement in meta.yaml).
  #  - Windows: vcpkg `gmp` port (declared in vcpkg.json, installed by
  #    bld.bat; the vcpkg toolchain file makes find_package(GMP) work).
  # conda-forge's win-64 gmp ships only the C library (no gmpxx).
  find_package(GMP REQUIRED)
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
