# Guard against a corrupted gtest discovery.
#
# gtest_discover_tests() enumerates each test binary at build time and writes
# the result to <target>_<hash>_tests.cmake, which ctest includes. Nothing
# downstream re-checks it, so a bad enumeration is invisible: ctest runs the
# recorded names against the recorded binary, gtest matches nothing, exits 0,
# and the cases count as passed while the binary's real ones never run.
#
# Compare, per binary, the cases registered against it with the cases it
# reports itself. Run as a test, so it fires after discovery:
#
#   cmake -DTEST_BIN_DIR=<dir> -DTEST_BINARIES=<a>|<b>|... -P <this>
#
# TEST_BINARIES comes from $<TARGET_FILE:...>: the binaries cannot be globbed,
# a multi-config generator puts them under a per-configuration subdirectory.

cmake_minimum_required(VERSION 3.15)

string(REPLACE "|" ";" binaries "${TEST_BINARIES}")
if(NOT binaries)
  message(FATAL_ERROR "TEST_BINARIES must be set")
endif()

# Registered cases, keyed by the binary each add_test() invokes. The name
# delimiter is ]=] and the filter's is ]==], so a greedy match stops at the name.
file(GLOB_RECURSE tests_files "${TEST_BIN_DIR}/*_tests.cmake")
foreach(tests_file IN LISTS tests_files)
  file(STRINGS "${tests_file}" add_lines REGEX "^add_test\\(")
  foreach(line IN LISTS add_lines)
    string(REGEX REPLACE "^add_test\\(\\[=\\[(.+)\\]=\\].*$" "\\1" case "${line}")
    string(REGEX REPLACE "^add_test\\(\\[=\\[.+\\]=\\][ \t]+([^ \t]+).*$" "\\1" exe "${line}")
    get_filename_component(exe "${exe}" NAME)
    list(APPEND registered_${exe} "${case}")
  endforeach()
endforeach()

foreach(binary IN LISTS binaries)
  get_filename_component(name "${binary}" NAME)
  execute_process(COMMAND "${binary}" --gtest_list_tests
                  OUTPUT_VARIABLE listing RESULT_VARIABLE status ERROR_QUIET)
  if(NOT status EQUAL 0)
    message(FATAL_ERROR "${name}: cannot enumerate its cases (${status})")
  endif()

  # Suite lines are unindented and end in '.'; case lines are indented. The
  # binaries also log to stdout, but those lines start with '[' and match neither.
  string(REPLACE ";" "\\;" listing "${listing}")
  string(REPLACE "\n" ";" listing "${listing}")
  set(defined "")
  foreach(line IN LISTS listing)
    if(line MATCHES "^([A-Za-z_][A-Za-z0-9_/]*)\\.[ \t]*$")
      set(suite "${CMAKE_MATCH_1}")
    elseif(line MATCHES "^[ \t]+([A-Za-z_][A-Za-z0-9_/]*)")
      list(APPEND defined "${suite}.${CMAKE_MATCH_1}")
    endif()
  endforeach()

  # One comparison covers every way they can drift: cases registered but not
  # defined, defined but never registered, and any registered twice.
  set(recorded "${registered_${name}}")
  list(SORT defined)
  list(SORT recorded)
  if(NOT defined STREQUAL recorded)
    list(LENGTH defined n_defined)
    list(LENGTH recorded n_recorded)
    message(FATAL_ERROR
      "${name}: ctest registers ${n_recorded} case(s), the binary defines "
      "${n_defined} — the discovery is stale or raced. Re-run the build.")
  endif()
endforeach()
