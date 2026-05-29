########################################
### Global options
########################################

cmake_minimum_required (VERSION 3.15...3.27)
project (veripp)


########################################
### Source Files
########################################

file (GLOB source_files
  src/main.cpp)


########################################
### Executable generation
########################################

add_executable (veripp ${source_files})
target_link_libraries (veripp veriparse_static)
target_link_libraries (veripp Boost::program_options Boost::filesystem)

install (TARGETS veripp RUNTIME DESTINATION bin)


########################################
### Golden-file tests
########################################

if(NOT WIN32)

  function(add_veripp_golden_test name workdir expected)
    add_test(NAME test_veripp_${name}
             COMMAND ${CMAKE_COMMAND}
                     -DVERIPP=$<TARGET_FILE:veripp>
                     -DSRCDIR=${CMAKE_CURRENT_SOURCE_DIR}/test/${workdir}
                     -DTESTDIR=${CMAKE_CURRENT_BINARY_DIR}/test_${name}
                     -DEXPECTED=${expected}
                     "-DVERIPP_ARGS=${ARGN}"
                     -P ${CMAKE_CURRENT_SOURCE_DIR}/test/run_veripp_test.cmake)
    set_tests_properties(test_veripp_${name} PROPERTIES LABELS "unittest")
  endfunction()

  add_veripp_golden_test(basic     basic     expected.v        -I;.;top.v)
  add_veripp_golden_test(ifdef     ifdef     expected_no_def.v dut.v)
  add_veripp_golden_test(ifdef_def ifdef     expected_mode_b.v -DMODE_B;dut.v)
  add_veripp_golden_test(funcmacro funcmacro expected.v        -I;.;top.v)

endif()
