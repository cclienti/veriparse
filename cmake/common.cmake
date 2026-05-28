cmake_minimum_required (VERSION 3.15...3.27)

if(NOT DEFINED VERIPARSE_COMMON_CMAKE)
  set(VERIPARSE_COMMON_CMAKE TRUE)

  message(STATUS "Using the veriparse common cmake listfile")
  message(STATUS "CMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}")
  include(FindPackageHandleStandardArgs)

  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/../cmake/modules")

  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)

  if(MSVC)
    # /bigobj: our template-heavy expression_operators_*.cpp blow past
    # COFF's 65k-section ceiling without it.
    # NOMINMAX: drops windows.h's min/max macros that collide with Boost
    # template names; WIN32_LEAN_AND_MEAN trims the rest of windows.h.
    # _CRT_SECURE_NO_WARNINGS + _CRT_NONSTDC_NO_DEPRECATE silence MSVC's
    # deprecation warnings on portable C/POSIX names (getenv, getpid, ...);
    # they fire on every cross-platform test helper otherwise.
    add_compile_options(/EHsc /bigobj)
    add_compile_definitions(
      NOMINMAX
      WIN32_LEAN_AND_MEAN
      _CRT_SECURE_NO_WARNINGS
      _CRT_NONSTDC_NO_DEPRECATE)
    set(CMAKE_CXX_FLAGS_DEBUG          "/Zi /Od /DDEBUG")
    set(CMAKE_CXX_FLAGS_FULLDEBUG      "/Zi /Od /DDEBUG /DFULL_DEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/Zi /O2 /DDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE        "/O2")
  else()
    add_compile_options(-Wall)
    set(CMAKE_CXX_FLAGS_DEBUG          "-g3 -DDEBUG")
    set(CMAKE_CXX_FLAGS_FULLDEBUG      "-g3 -DDEBUG -DFULL_DEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-Werror -g3 -O3 -DDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE        "-Werror -s -O3")
    set(CMAKE_CXX_FLAGS_COVERAGE       "-g3 -DDEBUG -fprofile-arcs -ftest-coverage")
  endif()

  if (CMAKE_BUILD_TYPE STREQUAL "Coverage")
    include(CodeCoverage)
    message(STATUS "Coverage: excluding ${CMAKE_PREFIX_PATH}")
    setup_target_for_coverage(coverage "make run_tests" coverage)
  endif()

  find_package(GTest CONFIG REQUIRED)
  find_package(YAMLCPP REQUIRED)
  # Boost.System is header-only since 1.69 and conda-forge libboost-devel
  # 1.90 no longer ships a boost_systemConfig.cmake. Other components pull
  # whatever transitive system bits they need.
  find_package(Boost 1.90.0 CONFIG REQUIRED
    COMPONENTS filesystem log program_options)

  message(STATUS "Found Boost: ${Boost_INCLUDE_DIRS}")

  set(CMAKE_SKIP_BUILD_RPATH FALSE)
  set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

endif()
