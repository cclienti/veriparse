# add_cosim_test(NAME <test_name>
#                TOP_MODULE <verilog_top>
#                VERILOG_SOURCES <src1.v> [<src2.v> ...]
#                HARNESS <harness.cpp>
#                [SEED <int>]
#                [VERIFLAT_ARGS <arg> ...]
#                [VERILATOR_ARGS <arg> ...])
#
# Pipeline (all build-time, no iverilog dependency):
#   1. veriflat       raw Verilog -> flat .v
#   2. verilator      flat .v     -> V<top>__ALL.a + headers (own build)
#   3. add_executable harness.cpp linked against the verilated archive,
#      cosim_helpers, GoogleTest, and Verilator's runtime sources.
#   4. add_test       label "cosim"
#
# verilate() from verilator-config.cmake is intentionally NOT used: it
# runs verilator via execute_process() at configure time, but our flat
# .v file is generated at build time. We drive verilator ourselves via
# add_custom_command so dependency ordering works.
#
# Assumes find_package(verilator CONFIG) succeeded at parent scope
# (VERILATOR_BIN and VERILATOR_ROOT are exported by that).

include_guard(GLOBAL)

function(add_cosim_test)
    set(options)
    set(one_value_args NAME TOP_MODULE HARNESS SEED)
    set(multi_value_args VERILOG_SOURCES VERIFLAT_ARGS VERILATOR_ARGS)
    cmake_parse_arguments(ACT
        "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    foreach(required NAME TOP_MODULE HARNESS VERILOG_SOURCES)
        if(NOT ACT_${required})
            message(FATAL_ERROR "add_cosim_test: ${required} is required")
        endif()
    endforeach()

    if(NOT DEFINED ACT_SEED)
        set(ACT_SEED 0)
    endif()

    # The executable is created in CMAKE_CURRENT_BINARY_DIR/<NAME>, so the
    # work dir for verilated artefacts uses a distinct suffix to avoid a
    # path collision with that binary.
    set(_work_dir ${CMAKE_CURRENT_BINARY_DIR}/${ACT_NAME}.dir)
    set(_v_dir ${_work_dir}/verilated)
    set(_v_prefix V${ACT_TOP_MODULE})
    set(_flat_v ${_work_dir}/${ACT_TOP_MODULE}_flat.v)
    set(_v_archive ${_v_dir}/lib${_v_prefix}.a)
    set(_v_runtime ${_v_dir}/libverilated.a)
    set(_v_main_h ${_v_dir}/${_v_prefix}.h)

    file(MAKE_DIRECTORY ${_work_dir})
    file(MAKE_DIRECTORY ${_v_dir})

    add_custom_command(
        OUTPUT ${_flat_v}
        COMMAND $<TARGET_FILE:veriflat>
                --seed ${ACT_SEED}
                --top-module ${ACT_TOP_MODULE}
                --output ${_flat_v}
                ${ACT_VERIFLAT_ARGS}
                ${ACT_VERILOG_SOURCES}
        DEPENDS veriflat ${ACT_VERILOG_SOURCES}
        WORKING_DIRECTORY ${_work_dir}
        COMMENT "veriflat ${ACT_TOP_MODULE} -> ${_flat_v}"
        VERBATIM
    )

    # Verilator's `--build` invokes the conda toolchain ar/c++ via the
    # prefixed names (x86_64-conda-linux-gnu-ar etc.) that live in the
    # conda env's bin dir, but CMake configured against /usr toolchain
    # so PATH does not include the conda bin. Prepend it for this step.
    # VERILATOR_ROOT is <conda_env>/share/verilator, so go two levels up.
    get_filename_component(_conda_bin_dir
        "${VERILATOR_ROOT}/../../bin" ABSOLUTE)

    add_custom_command(
        OUTPUT ${_v_archive} ${_v_runtime} ${_v_main_h}
        COMMAND ${CMAKE_COMMAND} -E env "PATH=${_conda_bin_dir}:$ENV{PATH}"
                ${VERILATOR_BIN}
                --cc --build --build-jobs 1
                --prefix ${_v_prefix}
                --Mdir ${_v_dir}
                --top-module ${ACT_TOP_MODULE}
                -Wno-fatal
                ${ACT_VERILATOR_ARGS}
                ${_flat_v}
        DEPENDS ${_flat_v}
        WORKING_DIRECTORY ${_work_dir}
        COMMENT "verilating ${ACT_TOP_MODULE} -> ${_v_archive}"
        VERBATIM
    )

    add_custom_target(${ACT_NAME}_verilated DEPENDS ${_v_archive})

    add_executable(${ACT_NAME} ${ACT_HARNESS})
    add_dependencies(${ACT_NAME} ${ACT_NAME}_verilated)

    # SYSTEM so the project's strict warning flags (-Wall -Werror, etc.)
    # do not propagate into verilator-generated and verilator-runtime
    # headers (sign-compare and similar trip there).
    target_include_directories(${ACT_NAME}
        PRIVATE ${_v_dir}
    )
    target_include_directories(${ACT_NAME} SYSTEM
        PRIVATE
            ${VERILATOR_ROOT}/include
            ${VERILATOR_ROOT}/include/vltstd
    )
    target_link_libraries(${ACT_NAME}
        PRIVATE
            ${_v_archive}
            ${_v_runtime}
            veriparse_cosim_helpers
            GTest::gtest
            GTest::gtest_main
            Threads::Threads
    )

    add_test(NAME ${ACT_NAME}
             COMMAND ${ACT_NAME}
             WORKING_DIRECTORY ${_work_dir})
    set_tests_properties(${ACT_NAME} PROPERTIES LABELS "cosim")
endfunction()
