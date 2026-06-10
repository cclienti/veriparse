cmake_minimum_required (VERSION 3.15...3.27)
project (veriparse_lib_test)
include(GoogleTest)
enable_testing()

add_custom_target(build_tests)

include(../../cmake/modules/subdirs.cmake)
subdirlist(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

foreach(subdir ${SUBDIRS})
  # cosim/ is a header-only helper library, not a bundled unit-test
  # executable. It is registered explicitly below via add_subdirectory().
  if(subdir STREQUAL "cosim")
    continue()
  endif()

  # TEMPORARY (ADR-0001 migration, "passes last"): the passes are excluded from
  # the library until their phase, so their tests cannot link yet. Skip them
  # (and the cosim end-to-end tests, which exercise passes). Re-enable once the
  # passes are ported.
  if(subdir STREQUAL "passes")
    continue()
  endif()

  file(GLOB_RECURSE test_source_files ${subdir}/*.cpp)
  if (test_source_files)
    set(testname test_veriparse_${subdir})

    add_executable(${testname} "${test_source_files}")
    add_dependencies(build_tests ${testname})
    target_link_libraries(${testname} veriparse_static GTest::gtest GTest::gtest_main)
    if(CMAKE_CROSSCOMPILING)
      add_test(${testname} wine ${testname}${CMAKE_EXECUTABLE_SUFFIX})
      set_tests_properties(${testname} PROPERTIES LABELS "unittest")
    else()
      gtest_discover_tests(${testname} PROPERTIES LABELS "unittest")
    endif()
  endif()
endforeach()

add_subdirectory(cosim)

add_custom_target(run_tests)
add_dependencies(run_tests build_tests)
add_custom_command(TARGET run_tests POST_BUILD COMMAND ctest -C $<CONFIGURATION> --output-on-failure)
