# add_cosim_test(NAME <test_name>
#                TOP_MODULE <verilog_top>
#                VERILOG_SOURCES <src1.v> [<src2.v> ...]
#                HARNESS <harness.cpp> [<more.cpp> ...]
#                [NO_VERIFLAT]
#                [VERILATOR_TOP <verilog_top>]
#                [VERILATOR_EXTRA_SOURCES <src.v> ...]
#                [EXTRA_LINK_LIBS <target> ...]
#                [EXTRA_INCLUDE_DIRS <dir> ...]
#                [SEED <int>]
#                [VERIFLAT_ARGS <arg> ...]
#                [VERILATOR_ARGS <arg> ...])
#
# Pipeline (all build-time, no iverilog dependency):
#   1. veriflat       VERILOG_SOURCES + TOP_MODULE -> flat .v
#                     (skipped when NO_VERIFLAT is set; VERILOG_SOURCES
#                     are fed straight to verilator instead)
#   2. verilator      (flat .v or VERILOG_SOURCES) + VERILATOR_EXTRA_SOURCES
#                     -> static archive and runtime archive, top =
#                     VERILATOR_TOP (defaults to TOP_MODULE)
#   3. add_executable HARNESS sources linked against the verilated
#      archives, cosim_helpers, GoogleTest, EXTRA_LINK_LIBS, and the
#      verilator runtime
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
    set(options NO_VERIFLAT)
    set(one_value_args NAME TOP_MODULE VERILATOR_TOP SEED)
    set(multi_value_args
        VERILOG_SOURCES
        VERILATOR_EXTRA_SOURCES
        HARNESS
        EXTRA_LINK_LIBS
        EXTRA_INCLUDE_DIRS
        VERIFLAT_ARGS
        VERILATOR_ARGS)
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
    if(NOT ACT_VERILATOR_TOP)
        set(ACT_VERILATOR_TOP ${ACT_TOP_MODULE})
    endif()

    # The executable is created in CMAKE_CURRENT_BINARY_DIR/<NAME>, so the
    # work dir for verilated artefacts uses a distinct suffix to avoid a
    # path collision with that binary.
    set(_work_dir ${CMAKE_CURRENT_BINARY_DIR}/${ACT_NAME}.dir)
    set(_v_dir ${_work_dir}/verilated)
    set(_v_prefix V${ACT_VERILATOR_TOP})
    set(_v_archive ${_v_dir}/lib${_v_prefix}.a)
    set(_v_runtime ${_v_dir}/libverilated.a)
    set(_v_main_h ${_v_dir}/${_v_prefix}.h)

    file(MAKE_DIRECTORY ${_work_dir})
    file(MAKE_DIRECTORY ${_v_dir})

    if(ACT_NO_VERIFLAT)
        set(_verilator_dut_sources ${ACT_VERILOG_SOURCES})
        set(_verilator_dut_depends ${ACT_VERILOG_SOURCES})
    else()
        set(_flat_v ${_work_dir}/${ACT_TOP_MODULE}_flat.v)
        # Route veriflat's diagnostics to a per-test log file rather than
        # the build console: ${_work_dir} is unique per test (<NAME>.dir),
        # so ${ACT_NAME}.log never collides across the suite and a failing
        # run leaves its log right next to the generated netlist.
        set(_veriflat_log ${_work_dir}/${ACT_NAME}.log)
        add_custom_command(
            OUTPUT ${_flat_v}
            COMMAND $<TARGET_FILE:veriflat>
                    --seed ${ACT_SEED}
                    --top-module ${ACT_TOP_MODULE}
                    --output ${_flat_v}
                    --log ${_veriflat_log}
                    ${ACT_VERIFLAT_ARGS}
                    ${ACT_VERILOG_SOURCES}
            DEPENDS veriflat ${ACT_VERILOG_SOURCES}
            WORKING_DIRECTORY ${_work_dir}
            COMMENT "veriflat ${ACT_TOP_MODULE} -> ${_flat_v}"
            VERBATIM
        )
        set(_verilator_dut_sources ${_flat_v})
        set(_verilator_dut_depends ${_flat_v})
    endif()

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
                --top-module ${ACT_VERILATOR_TOP}
                -Wno-fatal
                ${ACT_VERILATOR_ARGS}
                ${_verilator_dut_sources}
                ${ACT_VERILATOR_EXTRA_SOURCES}
        DEPENDS ${_verilator_dut_depends} ${ACT_VERILATOR_EXTRA_SOURCES}
        WORKING_DIRECTORY ${_work_dir}
        COMMENT "verilating ${ACT_VERILATOR_TOP} -> ${_v_archive}"
        VERBATIM
    )

    add_custom_target(${ACT_NAME}_verilated DEPENDS ${_v_archive})

    add_executable(${ACT_NAME} ${ACT_HARNESS})
    add_dependencies(${ACT_NAME} ${ACT_NAME}_verilated)

    # SYSTEM so the project's strict warning flags (-Wall -Werror, etc.)
    # do not propagate into verilator-generated and verilator-runtime
    # headers (sign-compare and similar trip there).
    target_include_directories(${ACT_NAME}
        PRIVATE ${_v_dir} ${ACT_EXTRA_INCLUDE_DIRS}
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
            ${ACT_EXTRA_LINK_LIBS}
    )

    add_test(NAME ${ACT_NAME}
             COMMAND ${ACT_NAME}
             WORKING_DIRECTORY ${_work_dir})
    set_tests_properties(${ACT_NAME} PROPERTIES LABELS "cosim")
endfunction()
