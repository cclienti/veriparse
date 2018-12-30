cmake_minimum_required (VERSION 3.7)
project (veriparse_lib_test)

enable_testing()

add_custom_target(build_tests)

include(../../cmake/modules/subdirs.cmake)
subdirlist(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

foreach(subdir ${SUBDIRS})
  file(GLOB_RECURSE test_source_files ${subdir}/*.cpp)
  if (test_source_files)
    set(testname test_veriparse_${subdir})

    add_executable(${testname} "${test_source_files}")
    add_dependencies(build_tests ${testname})
    target_include_directories(${testname} PRIVATE ${GTEST_INCLUDE_DIR})
    target_link_libraries(${testname} veriparse_static ${GTEST_LIBRARY})
    if(CMAKE_CROSSCOMPILING)
      add_test(${testname} wine ${testname}${CMAKE_EXECUTABLE_SUFFIX})
    else()
      add_test(${testname} ${testname})
    endif()
  endif()
endforeach()

add_custom_target(run_tests)
add_dependencies(run_tests build_tests)
add_custom_command(TARGET run_tests COMMAND ctest -C $<CONFIGURATION> --output-on-failure)
