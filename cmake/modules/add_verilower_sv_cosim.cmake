# add_verilower_sv_cosim(NAME <design> [TOOL <target>] [TOOL_ARGS <arg> ...]
#                        [VERILATOR_ARGS <arg> ...] [EXTRA_SOURCES <src> ...])
#
# The verilower differential-bench convention: the behavioural source at
# test/verilower/<design>/src/<design>.sv is compiled by the tool (default
# verilower; TOOL veriflat with TOOL_ARGS --fsm benches the in-flattener
# slot) with --suffix _lowered, and the bench at test/verilower/<design>/
# cosim/tb_<design>.sv (top module tb_<design>) drives source and lowered
# machine side by side. Registers test_cosim_<design>_svbench and exports
# the lowered netlist path as <design>_LOWERED_V in the parent scope.
#
# Requires add_sv_cosim_test.cmake and the selected tool's target.

include_guard(GLOBAL)

function(add_verilower_sv_cosim)
    set(one_value_args NAME TOOL)
    set(multi_value_args TOOL_ARGS VERILATOR_ARGS EXTRA_SOURCES)
    cmake_parse_arguments(AVSC "" "${one_value_args}" "${multi_value_args}" ${ARGN})

    if(NOT AVSC_NAME)
        message(FATAL_ERROR "add_verilower_sv_cosim: NAME is required")
    endif()
    if(NOT AVSC_TOOL)
        set(AVSC_TOOL verilower)
    endif()

    set(_src ${CMAKE_CURRENT_SOURCE_DIR}/test/verilower/${AVSC_NAME}/src/${AVSC_NAME}.sv)
    set(_tb ${CMAKE_CURRENT_SOURCE_DIR}/test/verilower/${AVSC_NAME}/cosim/tb_${AVSC_NAME}.sv)
    set(_lowered ${CMAKE_CURRENT_BINARY_DIR}/${AVSC_NAME}_suffixed.v)

    add_custom_command(
        OUTPUT ${_lowered}
        COMMAND $<TARGET_FILE:${AVSC_TOOL}>
                --sv
                --top-module ${AVSC_NAME}
                --suffix _lowered
                --output ${_lowered}
                --log ${CMAKE_CURRENT_BINARY_DIR}/${AVSC_NAME}_suffixed.log
                ${AVSC_TOOL_ARGS}
                ${_src}
        DEPENDS ${AVSC_TOOL} ${_src}
        COMMENT "${AVSC_TOOL} ${AVSC_NAME} (renamed _lowered)"
        VERBATIM)

    add_sv_cosim_test(
        NAME test_cosim_${AVSC_NAME}_svbench
        TOP_MODULE tb_${AVSC_NAME}
        VERILOG_SOURCES ${_src} ${_lowered} ${AVSC_EXTRA_SOURCES} ${_tb}
        VERILATOR_ARGS ${AVSC_VERILATOR_ARGS})

    set(${AVSC_NAME}_LOWERED_V ${_lowered} PARENT_SCOPE)
endfunction()
