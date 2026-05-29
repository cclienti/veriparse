# SPDX-License-Identifier: LGPL-3.0-or-later
# Copyright (C) 2013-2026 Christophe Clienti
#
# Driver script for veripp golden-file tests. Invoked via `cmake -P`
# from add_test() in apps/veriparse/veripp/CMakeLists.txt.
#
# Required variables:
#   VERIPP        absolute path to the veripp executable
#   SRCDIR        source dir with fixture files + reference output
#   TESTDIR       build dir to run in (kept clean each invocation)
#   EXPECTED      reference filename (relative to SRCDIR)
#   VERIPP_ARGS   semicolon-separated arg list passed to veripp

file(REMOVE_RECURSE ${TESTDIR})
file(MAKE_DIRECTORY ${TESTDIR})

# Copy fixture files (.v / .vh) into the test workspace so veripp runs
# against relative paths, keeping `line markers portable.
file(GLOB _fixtures "${SRCDIR}/*.v" "${SRCDIR}/*.vh")
foreach(_f IN LISTS _fixtures)
    file(COPY ${_f} DESTINATION ${TESTDIR})
endforeach()

execute_process(
    COMMAND ${VERIPP} ${VERIPP_ARGS}
    WORKING_DIRECTORY ${TESTDIR}
    OUTPUT_FILE ${TESTDIR}/actual.v
    RESULT_VARIABLE _rc)

if(NOT _rc EQUAL 0)
    message(FATAL_ERROR "veripp exited ${_rc}")
endif()

execute_process(
    COMMAND ${CMAKE_COMMAND} -E compare_files ${SRCDIR}/${EXPECTED} ${TESTDIR}/actual.v
    RESULT_VARIABLE _rc)

if(NOT _rc EQUAL 0)
    execute_process(COMMAND ${CMAKE_COMMAND} -E echo "--- expected (${EXPECTED}) ---")
    execute_process(COMMAND ${CMAKE_COMMAND} -E cat ${SRCDIR}/${EXPECTED})
    execute_process(COMMAND ${CMAKE_COMMAND} -E echo "--- actual ---")
    execute_process(COMMAND ${CMAKE_COMMAND} -E cat ${TESTDIR}/actual.v)
    message(FATAL_ERROR "veripp output differs from ${EXPECTED}")
endif()
