# Guard against a corrupted gtest discovery.
#
# gtest_discover_tests() enumerates each test binary at build time and writes
# the result to <target>_<hash>_tests.cmake, which ctest includes. Nothing
# downstream re-checks that what got written matches what the binary actually
# defines, so a bad enumeration is invisible: ctest runs the recorded names
# against the recorded binary, gtest matches nothing, exits 0, and the case
# counts as passed while the binary's real cases never run at all.
#
# This script compares, per binary, the set of cases registered against it with
# the set it reports itself, and fails on any difference. Run it as a test so it
# executes after discovery, with the cmake that produced the build tree:
#
#   cmake -DTEST_BIN_DIR=<dir> -DTEST_BINARIES=<a>|<b>|... \
#         -P scripts/check_test_discovery.cmake
#
# TEST_BINARIES comes from $<TARGET_FILE:...>, pipe-separated: the binaries
# cannot be found by globbing, because a multi-config generator puts them in a
# per-configuration subdirectory while leaving same-named project files in the
# directory itself.

cmake_minimum_required(VERSION 3.15)

if(NOT DEFINED TEST_BIN_DIR)
  message(FATAL_ERROR "TEST_BIN_DIR must be set")
endif()
if(NOT DEFINED TEST_BINARIES)
  message(FATAL_ERROR "TEST_BINARIES must be set")
endif()

# --- what each *_tests.cmake registers, keyed by the binary it invokes --------

# Recursive: a multi-config generator writes them under the configuration.
file(GLOB_RECURSE tests_files "${TEST_BIN_DIR}/*_tests.cmake")
set(seen_exes "")

foreach(tests_file IN LISTS tests_files)
  file(STRINGS "${tests_file}" add_lines REGEX "^add_test\\(")
  foreach(line IN LISTS add_lines)
    # add_test([=[Suite.Case]=]  /path/to/binary [==[--gtest_filter=...]==] ...
    # The name delimiter is ]=] and the filter's is ]==], so a greedy match up
    # to ]=] stops at the end of the name.
    string(REGEX REPLACE "^add_test\\(\\[=\\[(.+)\\]=\\].*$" "\\1" case_name "${line}")
    string(REGEX REPLACE "^add_test\\(\\[=\\[.+\\]=\\][ \t]+([^ \t]+).*$" "\\1" exe "${line}")
    if(case_name STREQUAL line OR exe STREQUAL line)
      message(FATAL_ERROR "${tests_file}: cannot parse an add_test entry:\n  ${line}")
    endif()
    get_filename_component(exe_name "${exe}" NAME)
    list(APPEND registered_${exe_name} "${case_name}")
    list(APPEND seen_exes "${exe_name}")
  endforeach()
endforeach()

list(REMOVE_DUPLICATES seen_exes)

# --- every test binary, so one registered nowhere is caught too --------------

string(REPLACE "|" ";" binaries "${TEST_BINARIES}")
if(binaries STREQUAL "")
  message(FATAL_ERROR "TEST_BINARIES is empty")
endif()

foreach(binary IN LISTS binaries)
  if(NOT EXISTS "${binary}")
    message(FATAL_ERROR "test binary not built: ${binary}")
  endif()
endforeach()

# --- compare, per binary -----------------------------------------------------

set(failures "")

foreach(binary IN LISTS binaries)
  get_filename_component(exe_name "${binary}" NAME)

  execute_process(
    COMMAND "${binary}" --gtest_list_tests
    OUTPUT_VARIABLE listing
    ERROR_VARIABLE  ignored
    RESULT_VARIABLE status
  )
  if(NOT status EQUAL 0)
    list(APPEND failures "${exe_name}: cannot enumerate its cases (exit ${status})")
    continue()
  endif()

  string(REPLACE ";" "\\;" listing "${listing}")
  string(REPLACE "\n" ";" listing_lines "${listing}")

  set(actual "")
  set(suite "")
  foreach(line IN LISTS listing_lines)
    # The binaries log to stdout while listing; those lines start with '['.
    if(line MATCHES "^\\[")
      continue()
    endif()
    if(line MATCHES "^([A-Za-z_][A-Za-z0-9_/]*)\\.[ \t]*$")
      set(suite "${CMAKE_MATCH_1}")
    elseif(suite AND line MATCHES "^[ \t]+([A-Za-z_][A-Za-z0-9_/]*)")
      list(APPEND actual "${suite}.${CMAKE_MATCH_1}")
    endif()
  endforeach()

  set(recorded "${registered_${exe_name}}")

  # Duplicates mean the same case was registered twice — the symptom of two
  # concurrent discoveries writing to one file.
  set(deduped "${recorded}")
  list(REMOVE_DUPLICATES deduped)
  list(LENGTH recorded n_recorded)
  list(LENGTH deduped n_deduped)
  if(NOT n_recorded EQUAL n_deduped)
    math(EXPR n_dup "${n_recorded} - ${n_deduped}")
    list(APPEND failures "${exe_name}: ${n_dup} case(s) registered more than once")
  endif()

  # Registered against this binary but not defined by it: they would run the
  # binary with a filter matching nothing and pass without executing anything.
  set(phantom "${deduped}")
  if(actual)
    list(REMOVE_ITEM phantom ${actual})
  endif()
  list(LENGTH phantom n_phantom)
  if(n_phantom GREATER 0)
    list(GET phantom 0 sample)
    list(APPEND failures
         "${exe_name}: ${n_phantom} case(s) registered but not defined by it, e.g. ${sample}")
  endif()

  # Defined but registered nowhere against this binary: they never run.
  set(missing "${actual}")
  if(deduped)
    list(REMOVE_ITEM missing ${deduped})
  endif()
  list(LENGTH missing n_missing)
  if(n_missing GREATER 0)
    list(GET missing 0 sample)
    list(APPEND failures
         "${exe_name}: ${n_missing} case(s) defined but never registered, e.g. ${sample}")
  endif()
endforeach()

if(failures)
  string(REPLACE ";" "\n  " report "${failures}")
  message(FATAL_ERROR
    "gtest discovery is inconsistent with the test binaries:\n  ${report}\n"
    "Re-run the build; if it persists, the discovery step raced or wrote a stale list.")
endif()

list(LENGTH binaries n_binaries)
message(STATUS "gtest discovery consistent across ${n_binaries} test binaries")
