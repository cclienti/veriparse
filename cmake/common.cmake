cmake_minimum_required (VERSION 3.9)

if(NOT DEFINED VERIPARSE_COMMON_CMAKE)
  set(VERIPARSE_COMMON_CMAKE TRUE)

  message(STATUS "Using the veriparse common cmake listfile")
  message(STATUS "VERIPARSE_EXTERNAL_ROOT=${VERIPARSE_EXTERNAL_ROOT}")
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
    set(CMAKE_CXX_FLAGS "-Wa,-mbig-obj -std=c++14 -Wall")
  else()
    set(CMAKE_CXX_FLAGS "-std=c++14 -Wall")
  endif()

  set(CMAKE_CXX_FLAGS_DEBUG  "-g -DDEBUG")
  set(CMAKE_CXX_FLAGS_FULL_DEBUG  "-g3 -DDEBUG -DFULL_DEBUG")
  set(CMAKE_CXX_FLAGS_RELEASE "-Werror -g -O3")
  set(CMAKE_CXX_FLAGS_COVERAGE "-g3 -DDEBUG -fprofile-arcs -ftest-coverage")


  ########################################
  ### Coverage
  ########################################

  if (CMAKE_BUILD_TYPE STREQUAL "Coverage")
	include(CodeCoverage)
  endif()

  if (CMAKE_BUILD_TYPE STREQUAL "Coverage")
	set(LCOV_REMOVE_EXTRA "'${VERIPARSE_EXTERNAL_ROOT}/*'")
	message(STATUS "Coverage: excluding ${LCOV_REMOVE_EXTRA}")
	setup_target_for_coverage(coverage "make run_tests" coverage)
  endif()


  ########################################
  ### GTest
  ########################################

  find_path(GTEST_INCLUDE_DIR
	NAMES gtest/gtest.h
	HINTS ${VERIPARSE_EXTERNAL_ROOT}/googletest/install/include)

  find_library(GTEST_LIBRARY
	NAMES libgtest.a gtest
	HINTS ${VERIPARSE_EXTERNAL_ROOT}/googletest/install/lib)

  find_package_handle_standard_args(GTEST DEFAULT_MSG GTEST_INCLUDE_DIR GTEST_LIBRARY)
  mark_as_advanced(GTEST_INCLUDE_DIR GTEST_LIBRARY)


  ########################################
  ### YAML-CPP
  ########################################

  find_path(YAMLCPP_INCLUDE_DIR
  	NAMES yaml-cpp/yaml.h
  	HINTS ${VERIPARSE_EXTERNAL_ROOT}/yaml-cpp/install/include)

  find_library(YAMLCPP_LIBRARY
  	NAMES libyaml-cpp.a yaml-cpp
	HINTS ${VERIPARSE_EXTERNAL_ROOT}/yaml-cpp/install/lib)

  find_package_handle_standard_args(YAMLCPP DEFAULT_MSG YAMLCPP_INCLUDE_DIR YAMLCPP_LIBRARY)
  mark_as_advanced(YAMLCPP_INCLUDE_DIR YAMLCPP_LIBRARY)


  ########################################
  ### OpenSSL
  ########################################

  set(OPENSSL_ROOT_DIR ${VERIPARSE_EXTERNAL_ROOT}/openssl/install)
  set(OPENSSL_USE_STATIC_LIBS      ON)
  find_package(OpenSSL)


  ########################################
  ### GMP
  ########################################

  find_path(GMP_INCLUDE_DIR
	NAMES gmp.h
	HINTS ${VERIPARSE_EXTERNAL_ROOT}/gmp/install/include)

  find_library(GMP_LIBRARY
	NAMES libgmp.a gmp
	HINTS ${VERIPARSE_EXTERNAL_ROOT}/gmp/install/lib)

  find_package_handle_standard_args(GMP DEFAULT_MSG GMP_INCLUDE_DIR GMP_LIBRARY)
  mark_as_advanced(GMP_INCLUDE_DIR GMP_LIBRARY)

  find_path(GMPXX_INCLUDE_DIR
	NAMES gmpxx.h
	HINTS ${VERIPARSE_EXTERNAL_ROOT}/gmp/install/include)

  find_library(GMPXX_LIBRARY
	NAMES libgmpxx.a gmpxx
	HINTS ${VERIPARSE_EXTERNAL_ROOT}/gmp/install/lib)

  find_package_handle_standard_args(GMPXX DEFAULT_MSG GMPXX_INCLUDE_DIR GMPXX_LIBRARY)
  mark_as_advanced(GMPXX_INCLUDE_DIR GMPXX_LIBRARY)


  ########################################
  ### Boost configuration
  ########################################

  set(BOOST_ROOT ${VERIPARSE_EXTERNAL_ROOT}/boost/install)
  set(Boost_USE_STATIC_LIBS        ON)
  set(Boost_USE_MULTITHREADED      ON)
  set(Boost_USE_STATIC_RUNTIME     OFF)
  find_package(Boost 1.64.0 REQUIRED
	COMPONENTS system program_options thread filesystem date_time log log_setup)
  message(STATUS "Found Boost: ${Boost_INCLUDE_DIRS}")

endif()
