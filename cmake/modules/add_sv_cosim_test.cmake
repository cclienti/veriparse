# add_sv_cosim_test(NAME <test_name>
#                   TOP_MODULE <bench_top>
#                   VERILOG_SOURCES <src1> [<src2> ...]
#                   [VERILATOR_ARGS <arg> ...])
#
# A self-contained SystemVerilog cosimulation: the bench compares its own
# models and reports through its exit status — $fatal on divergence, $finish
# on success — so `verilator --binary --timing` is the whole harness, no C++.
# Requires verilator >= 5.050 (ADR-0014 §11: earlier versions execute an
# initial block's nonblocking assignments as blocking, corrupting a
# behavioural reference).
#
# Assumes find_package(verilator CONFIG) succeeded at parent scope.

include_guard(GLOBAL)

function(add_sv_cosim_test)
    set(one_value_args NAME TOP_MODULE)
    set(multi_value_args VERILOG_SOURCES VERILATOR_ARGS)
    cmake_parse_arguments(ASCT "" "${one_value_args}" "${multi_value_args}" ${ARGN})

    foreach(required NAME TOP_MODULE VERILOG_SOURCES)
        if(NOT ASCT_${required})
            message(FATAL_ERROR "add_sv_cosim_test: ${required} is required")
        endif()
    endforeach()

    set(_work_dir ${CMAKE_CURRENT_BINARY_DIR}/${ASCT_NAME}.dir)
    set(_v_dir ${_work_dir}/verilated)
    set(_bin ${_v_dir}/${ASCT_NAME})
    file(MAKE_DIRECTORY ${_work_dir})

    # Verilator's --build runs the conda toolchain via prefixed names living
    # in the conda env's bin dir; CMake configured against the /usr
    # toolchain, so prepend that dir for this step (same fix as
    # add_cosim_test).
    get_filename_component(_conda_bin_dir "${VERILATOR_ROOT}/../../bin" ABSOLUTE)

    add_custom_command(
        OUTPUT ${_bin}
        COMMAND ${CMAKE_COMMAND} -E env "PATH=${_conda_bin_dir}:$ENV{PATH}"
                ${VERILATOR_BIN}
                --binary --timing --build-jobs 1
                --Mdir ${_v_dir}
                --top-module ${ASCT_TOP_MODULE}
                -o ${ASCT_NAME}
                -Wno-fatal
                ${ASCT_VERILATOR_ARGS}
                ${ASCT_VERILOG_SOURCES}
        DEPENDS ${ASCT_VERILOG_SOURCES}
        WORKING_DIRECTORY ${_work_dir}
        COMMENT "verilating (binary) ${ASCT_TOP_MODULE} -> ${_bin}"
        VERBATIM
    )

    add_custom_target(${ASCT_NAME}_build ALL DEPENDS ${_bin})

    add_test(NAME ${ASCT_NAME}
             COMMAND ${_bin}
             WORKING_DIRECTORY ${_work_dir})
    set_tests_properties(${ASCT_NAME} PROPERTIES LABELS "cosim")
endfunction()
