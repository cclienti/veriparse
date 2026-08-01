cmake_minimum_required (VERSION 3.15...3.27)
project (veriparse_lib_test)
include(GoogleTest)
enable_testing()

add_custom_target(build_tests)

include(../../cmake/modules/subdirs.cmake)
subdirlist(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

# gtest_discover_tests(POST_BUILD) writes its enumeration to
# <CMAKE_CURRENT_BINARY_DIR>/cmake_test_discovery_<hash>.json. The hash is meant
# to give one file per target, but GoogleTest.cmake never passes TEST_TARGET to
# GoogleTestAddTests.cmake, so it hashes an empty string: every target in this
# directory shares one file. The discovery runs from the target's link edge, so
# Ninja executes several of them concurrently and they overwrite each other —
# a target then registers another one's cases, running them against a binary
# that does not define them (gtest matches nothing and exits 0) while its own
# never run at all.
#
# Serialising the link edges of these targets removes the concurrency the race
# needs, without moving the targets into separate binary directories. Only the
# unit-test executables join the pool; everything else still links in parallel.
# The pool itself is declared at the top level, before any target joins it.

foreach(subdir ${SUBDIRS})
  # cosim/ is a header-only helper library, not a bundled unit-test
  # executable. It is registered explicitly below via add_subdirectory().
  if(subdir STREQUAL "cosim")
    continue()
  endif()

  file(GLOB_RECURSE test_source_files ${subdir}/*.cpp)
  if (test_source_files)
    set(testname test_veriparse_${subdir})

    add_executable(${testname} "${test_source_files}")
    add_dependencies(build_tests ${testname})
    target_link_libraries(${testname} veriparse_static GTest::gtest GTest::gtest_main)
    set_target_properties(${testname} PROPERTIES JOB_POOL_LINK gtest_discovery)
    gtest_discover_tests(${testname} PROPERTIES LABELS "unittest")
  endif()
endforeach()

add_subdirectory(cosim)

add_custom_target(run_tests)
add_dependencies(run_tests build_tests)
add_custom_command(TARGET run_tests POST_BUILD COMMAND ctest -C $<CONFIGURATION> --output-on-failure)
