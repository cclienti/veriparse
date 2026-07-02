# Copyright (c) 2012 - 2015, Lars Bilke
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its contributors
#    may be used to endorse or promote products derived from this software without
#    specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#
#
# 2012-01-31, Lars Bilke
# - Enable Code Coverage
#
# 2013-09-17, Joakim Söderberg
# - Added support for Clang.
# - Some additional usage instructions.
#
# Veriparse fork: only the gcov/gcovr path is kept — the upstream lcov/genhtml
# and Cobertura targets were removed as unused. The Coverage compile/link flags
# are owned by cmake/common.cmake, and cmake/common.cmake instantiates the
# `coverage` target with SETUP_TARGET_FOR_COVERAGE_GCOVR (build with
# CMAKE_BUILD_TYPE=Coverage; see `make dev-coverage`).

# Check prereqs
FIND_PROGRAM( GCOV_PATH gcov )
FIND_PROGRAM( GCOVR_PATH gcovr )

IF(NOT GCOV_PATH)
	MESSAGE(FATAL_ERROR "gcov not found! Aborting...")
ENDIF() # NOT GCOV_PATH

IF("${CMAKE_CXX_COMPILER_ID}" MATCHES "(Apple)?[Cc]lang")
	IF("${CMAKE_CXX_COMPILER_VERSION}" VERSION_LESS 3)
		MESSAGE(FATAL_ERROR "Clang version must be 3.0.0 or greater! Aborting...")
	ENDIF()
ELSEIF(NOT CMAKE_COMPILER_IS_GNUCXX)
	MESSAGE(FATAL_ERROR "Compiler is not GNU gcc! Aborting...")
ENDIF() # CHECK VALID COMPILER

# The Coverage compile/link flags (CMAKE_<LANG>_FLAGS_COVERAGE,
# CMAKE_{EXE,SHARED}_LINKER_FLAGS_COVERAGE) are owned by cmake/common.cmake —
# a single owner avoids the cache-vs-normal-variable precedence trap (CMP0126).

IF ( NOT (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "Coverage"))
  MESSAGE( WARNING "Code coverage results with an optimized (non-Debug) build may be misleading" )
ENDIF() # NOT CMAKE_BUILD_TYPE STREQUAL "Debug"


# Param _targetname     The name of the new custom target
# Param _outputdir      HTML report is generated in _outputdir/index.html;
#                       a text summary is printed to stdout
# Param _ctest_labels    ctest -L filter (e.g. "unittest")
FUNCTION(SETUP_TARGET_FOR_COVERAGE_GCOVR _targetname _outputdir _ctest_labels)

	IF(NOT GCOVR_PATH)
		MESSAGE(FATAL_ERROR "gcovr not found! Aborting...")
	ENDIF() # NOT GCOVR_PATH

	INCLUDE(ProcessorCount)
	PROCESSORCOUNT(_cores)
	IF(_cores EQUAL 0)
		SET(_cores 1)
	ENDIF()

	ADD_CUSTOM_TARGET(${_targetname}
		COMMAND ${CMAKE_COMMAND} -E make_directory ${_outputdir}

		# Zero the counters: .gcda files accumulate across runs and stale ones
		# from a previous build mismatch the rebuilt .gcno stamps.
		COMMAND find ${CMAKE_BINARY_DIR} -name '*.gcda' -delete

		# Run tests (VERIPARSE_SOURCE_ROOT is required by the test suite);
		# concurrent .gcda writes are safe (libgcov locks and merges).
		COMMAND ${CMAKE_COMMAND} -E env VERIPARSE_SOURCE_ROOT=${CMAKE_SOURCE_DIR}
			ctest --test-dir ${CMAKE_BINARY_DIR} -L ${_ctest_labels}
			--parallel ${_cores} --output-on-failure

		# Generate HTML report + print a summary to stdout. gcovr must use the
		# gcov matching the compiler, not whatever is first in PATH.
		COMMAND ${GCOVR_PATH} --root ${CMAKE_SOURCE_DIR} -j ${_cores}
			--gcov-executable ${GCOV_PATH}
			--exclude '.*/lib/test/.*'
			--exclude '.*/apps/veriparse/test/.*'
			--exclude '.*/build.*'
			--html-details ${_outputdir}/index.html
			--print-summary
			${CMAKE_BINARY_DIR}

		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		COMMENT "Running '${_ctest_labels}' tests and generating gcovr coverage report in ${_outputdir}/"
	)

	ADD_CUSTOM_COMMAND(TARGET ${_targetname} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E echo
			"Open ${CMAKE_BINARY_DIR}/${_outputdir}/index.html in your browser to view the coverage report."
	)

ENDFUNCTION() # SETUP_TARGET_FOR_COVERAGE_GCOVR
