cmake_minimum_required (VERSION 3.15...3.27)
project (veriparse_apps)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)
enable_testing()


#----------------------------------------
# Visit subdirs
#----------------------------------------

add_subdirectory(veriflat)
add_subdirectory(verilower)
add_subdirectory(veriobf)
add_subdirectory(veridump)
add_subdirectory(veripp)

#----------------------------------------
# Installation
#----------------------------------------

install(DIRECTORY test/common/Makefiles
        DESTINATION share/veriparse/examples/common)
install(DIRECTORY test/dclkfifolut
        DESTINATION share/veriparse/examples)
install(DIRECTORY test/tiny_aes_128
        DESTINATION share/veriparse/examples)
install(DIRECTORY test/double_fpu-master
        DESTINATION share/veriparse/examples)
#----------------------------------------
# Cosim tests (label "cosim") — verilator-built C++ model of the design
# (optionally via veriflat), GoogleTest harness drives it. Each test
# opt-ins by shipping a test/<name>/cosim/test_<top>.cpp harness.
#----------------------------------------

if(NOT WIN32)
  find_package(verilator CONFIG QUIET)
  set(_verilator_version "")
  if(verilator_FOUND)
    # The floor is semantic (ADR-0014 §11): earlier versions execute an
    # initial block's nonblocking assignments as blocking, corrupting a
    # behavioural reference into a baffling divergence at run time.
    execute_process(COMMAND ${VERILATOR_BIN} --version
                    OUTPUT_VARIABLE _verilator_version_line
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(REGEX MATCH "[0-9]+\\.[0-9]+" _verilator_version "${_verilator_version_line}")
  endif()
  if(NOT verilator_FOUND)
    message(STATUS "Veriparse Apps: verilator not found, skipping cosim tests")
  elseif(_verilator_version VERSION_LESS 5.050)
    message(STATUS "Veriparse Apps: verilator ${_verilator_version} < 5.050 "
                   "(initial-block nonblocking assignments run as blocking), "
                   "skipping cosim tests")
  else()
    include(${CMAKE_SOURCE_DIR}/cmake/modules/add_cosim_test.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/modules/add_sv_cosim_test.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/modules/add_verilower_sv_cosim.cmake)

    foreach(_aes_width 128 192 256)
      set(_aes_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/test/tiny_aes_${_aes_width}/src/aes_${_aes_width}.v
        ${CMAKE_CURRENT_SOURCE_DIR}/test/tiny_aes_${_aes_width}/src/round.v
        ${CMAKE_CURRENT_SOURCE_DIR}/test/tiny_aes_${_aes_width}/src/table.v)
      set(_aes_harness
        ${CMAKE_CURRENT_SOURCE_DIR}/test/tiny_aes_${_aes_width}/cosim/test_aes_${_aes_width}.cpp)

      add_cosim_test(
        NAME test_cosim_tiny_aes_${_aes_width}_dut
        TOP_MODULE aes_${_aes_width}
        NO_VERIFLAT
        VERILOG_SOURCES ${_aes_sources}
        HARNESS ${_aes_harness}
      )
      add_cosim_test(
        NAME test_cosim_tiny_aes_${_aes_width}_flat
        TOP_MODULE aes_${_aes_width}
        VERILOG_SOURCES ${_aes_sources}
        HARNESS ${_aes_harness}
      )
    endforeach()

    set(_fpu_sources
      ${CMAKE_CURRENT_SOURCE_DIR}/test/double_fpu-master/src/fpu_double.v
      ${CMAKE_CURRENT_SOURCE_DIR}/test/double_fpu-master/src/fpu_add.v
      ${CMAKE_CURRENT_SOURCE_DIR}/test/double_fpu-master/src/fpu_div.v
      ${CMAKE_CURRENT_SOURCE_DIR}/test/double_fpu-master/src/fpu_exceptions.v
      ${CMAKE_CURRENT_SOURCE_DIR}/test/double_fpu-master/src/fpu_mul.v
      ${CMAKE_CURRENT_SOURCE_DIR}/test/double_fpu-master/src/fpu_round.v
      ${CMAKE_CURRENT_SOURCE_DIR}/test/double_fpu-master/src/fpu_sub.v)
    set(_fpu_harness
      ${CMAKE_CURRENT_SOURCE_DIR}/test/double_fpu-master/cosim/test_fpu.cpp)

    add_cosim_test(
      NAME test_cosim_double_fpu_dut
      TOP_MODULE fpu
      NO_VERIFLAT
      VERILOG_SOURCES ${_fpu_sources}
      HARNESS ${_fpu_harness}
    )
    add_cosim_test(
      NAME test_cosim_double_fpu_flat
      TOP_MODULE fpu
      VERILOG_SOURCES ${_fpu_sources}
      HARNESS ${_fpu_harness}
    )

    set(_dclkfifo_sources
      ${CMAKE_CURRENT_SOURCE_DIR}/test/dclkfifolut/src/dclkfifolut.v
      ${CMAKE_CURRENT_SOURCE_DIR}/test/dclkfifolut/src/bin2gray.v
      ${CMAKE_CURRENT_SOURCE_DIR}/test/dclkfifolut/src/gray2bin.v)
    set(_dclkfifo_harness
      ${CMAKE_CURRENT_SOURCE_DIR}/test/dclkfifolut/cosim/test_dclkfifolut.cpp)

    add_cosim_test(
      NAME test_cosim_dclkfifolut_dut
      TOP_MODULE dclkfifolut
      NO_VERIFLAT
      VERILOG_SOURCES ${_dclkfifo_sources}
      HARNESS ${_dclkfifo_harness}
    )
    add_cosim_test(
      NAME test_cosim_dclkfifolut_flat_inlined
      TOP_MODULE dclkfifolut
      VERILOG_SOURCES ${_dclkfifo_sources}
      HARNESS ${_dclkfifo_harness}
    )
    add_cosim_test(
      NAME test_cosim_dclkfifolut_flat_keep_fifo_width
      TOP_MODULE dclkfifolut
      VERIFLAT_ARGS -p "{FIFO_WIDTH:}"
      VERILOG_SOURCES ${_dclkfifo_sources}
      HARNESS ${_dclkfifo_harness}
    )

    # hynoc_5p — three cosim variants mirror the legacy iverilog surface
    # (modulo veriobf):
    #   * _dut          — original hierarchical DUT, no veriflat
    #   * _flat_inlined — veriflat with all parameters inlined (project0/1)
    #   * _flat_payload — veriflat keeping PAYLOAD_WIDTH/FLIT_WIDTH params
    #                     (project2/3)
    # All three share the same topology wrapper + noc_traffic kit harness.
    set(_hynoc_src ${CMAKE_CURRENT_SOURCE_DIR}/test/hynoc_5p/src)
    set(_hynoc_cosim ${CMAKE_CURRENT_SOURCE_DIR}/test/hynoc_5p/cosim)
    add_library(hynoc_noc_traffic STATIC ${_hynoc_cosim}/kit/noc_traffic.cpp)
    target_include_directories(hynoc_noc_traffic PUBLIC ${_hynoc_cosim}/kit)

    set(_hynoc_dut_sources
      ${_hynoc_src}/hynoc_router_5p.v
      ${_hynoc_src}/hynoc_router_base.v
      ${_hynoc_src}/hynoc_egress.v
      ${_hynoc_src}/hynoc_ingress.v
      ${_hynoc_src}/hynoc_ingress_routing_mcast.v
      ${_hynoc_src}/hynoc_ingress_routing_ucast.v
      ${_hynoc_src}/hynoc_prra.v
      ${_hynoc_src}/prra_lut.v)

    set(_hynoc_topology_sources
      ${_hynoc_cosim}/topology.v
      ${_hynoc_src}/hynoc_local_interface.v
      ${_hynoc_src}/dclkfifolut.v
      ${_hynoc_src}/sclkfifolut.v
      ${_hynoc_src}/bin2gray.v
      ${_hynoc_src}/gray2bin.v)

    add_cosim_test(
      NAME test_cosim_hynoc_router_5p_dut
      TOP_MODULE hynoc_router_5p
      VERILATOR_TOP hynoc_topology
      NO_VERIFLAT
      VERILATOR_ARGS -I${_hynoc_src}
      VERILOG_SOURCES ${_hynoc_dut_sources}
      VERILATOR_EXTRA_SOURCES ${_hynoc_topology_sources}
      HARNESS ${_hynoc_cosim}/test_hynoc_router_5p.cpp
      EXTRA_LINK_LIBS hynoc_noc_traffic
    )

    add_cosim_test(
      NAME test_cosim_hynoc_router_5p_flat_inlined
      TOP_MODULE hynoc_router_5p
      VERILATOR_TOP hynoc_topology
      VERIFLAT_ARGS -I ${_hynoc_src}
      VERILOG_SOURCES ${_hynoc_dut_sources}
      VERILATOR_EXTRA_SOURCES ${_hynoc_topology_sources}
      HARNESS ${_hynoc_cosim}/test_hynoc_router_5p.cpp
      EXTRA_LINK_LIBS hynoc_noc_traffic
    )

    add_cosim_test(
      NAME test_cosim_hynoc_router_5p_flat_payload
      TOP_MODULE hynoc_router_5p
      VERILATOR_TOP hynoc_topology
      VERIFLAT_ARGS -I ${_hynoc_src} -p "{PAYLOAD_WIDTH: null, FLIT_WIDTH: null}"
      VERILOG_SOURCES ${_hynoc_dut_sources}
      VERILATOR_EXTRA_SOURCES ${_hynoc_topology_sources}
      HARNESS ${_hynoc_cosim}/test_hynoc_router_5p.cpp
      EXTRA_LINK_LIBS hynoc_noc_traffic
    )

    # Interface-based valid/ready handshake (ADR-0008): interface elaboration
    # through veriflat vs verilator's native interface support.
    set(_iface_hs_sources
      ${CMAKE_CURRENT_SOURCE_DIR}/test/iface_handshake/src/iface_handshake.sv)
    set(_iface_hs_harness
      ${CMAKE_CURRENT_SOURCE_DIR}/test/iface_handshake/cosim/test_iface_handshake.cpp)

    add_cosim_test(
      NAME test_cosim_iface_handshake_dut
      TOP_MODULE iface_handshake
      NO_VERIFLAT
      VERILOG_SOURCES ${_iface_hs_sources}
      HARNESS ${_iface_hs_harness}
    )
    add_cosim_test(
      NAME test_cosim_iface_handshake_flat
      TOP_MODULE iface_handshake
      VERIFLAT_ARGS --sv
      VERILOG_SOURCES ${_iface_hs_sources}
      HARNESS ${_iface_hs_harness}
    )

    # parmem3 — 3-bank prime-interleaved parallel memory (CRT addressing,
    # dual load/store pair + linear side-B port); single-file testcase
    # with its dpmemrf bank folded in. Exercises parenthesized size casts,
    # compound assignments in constant functions, and same-named
    # conditional-generate arms.
    set(_parmem3_sources
      ${CMAKE_CURRENT_SOURCE_DIR}/test/parmem3/src/parmem3.sv)
    set(_parmem3_harness
      ${CMAKE_CURRENT_SOURCE_DIR}/test/parmem3/cosim/test_parmem3.cpp)

    add_cosim_test(
      NAME test_cosim_parmem3_dut
      TOP_MODULE parmem3
      NO_VERIFLAT
      VERILOG_SOURCES ${_parmem3_sources}
      HARNESS ${_parmem3_harness}
    )
    add_cosim_test(
      NAME test_cosim_parmem3_flat
      TOP_MODULE parmem3
      VERIFLAT_ARGS --sv
      VERILOG_SOURCES ${_parmem3_sources}
      HARNESS ${_parmem3_harness}
    )

    set(_type_param_pipe_sources
      ${CMAKE_CURRENT_SOURCE_DIR}/test/type_param_pipe/src/type_param_pipe.sv)
    set(_type_param_pipe_harness
      ${CMAKE_CURRENT_SOURCE_DIR}/test/type_param_pipe/cosim/test_type_param_pipe.cpp)

    add_cosim_test(
      NAME test_cosim_type_param_pipe_dut
      TOP_MODULE type_param_pipe
      NO_VERIFLAT
      VERILOG_SOURCES ${_type_param_pipe_sources}
      HARNESS ${_type_param_pipe_harness}
    )
    add_cosim_test(
      NAME test_cosim_type_param_pipe_flat
      TOP_MODULE type_param_pipe
      VERIFLAT_ARGS --sv
      VERILOG_SOURCES ${_type_param_pipe_sources}
      HARNESS ${_type_param_pipe_harness}
    )

    set(_struct_pipe_sources
      ${CMAKE_CURRENT_SOURCE_DIR}/test/struct_pipe/src/struct_pipe.sv)
    set(_struct_pipe_harness
      ${CMAKE_CURRENT_SOURCE_DIR}/test/struct_pipe/cosim/test_struct_pipe.cpp)

    add_cosim_test(
      NAME test_cosim_struct_pipe_dut
      TOP_MODULE struct_pipe
      NO_VERIFLAT
      VERILOG_SOURCES ${_struct_pipe_sources}
      HARNESS ${_struct_pipe_harness}
    )
    add_cosim_test(
      NAME test_cosim_struct_pipe_flat
      TOP_MODULE struct_pipe
      VERIFLAT_ARGS --sv
      VERILOG_SOURCES ${_struct_pipe_sources}
      HARNESS ${_struct_pipe_harness}
    )

    # verilower differential cosim (ADR-0014 §11): the behavioural source is
    # the golden model — verilated under --timing, it runs as written — and
    # the lowered machine must match it at the constant offset of the reset
    # hold. Both tests share one expected trace.
    set(_straight_line_src
      ${CMAKE_CURRENT_SOURCE_DIR}/test/verilower/straight_line/src/straight_line.sv)
    set(_straight_line_lowered
      ${CMAKE_CURRENT_BINARY_DIR}/straight_line_lowered.v)

    add_custom_command(
      OUTPUT ${_straight_line_lowered}
      COMMAND $<TARGET_FILE:verilower>
              --sv
              --top-module straight_line
              --output ${_straight_line_lowered}
              --log ${CMAKE_CURRENT_BINARY_DIR}/straight_line_lowered.log
              ${_straight_line_src}
      DEPENDS verilower ${_straight_line_src}
      COMMENT "verilower straight_line"
      VERBATIM)

    add_cosim_test(
      NAME test_cosim_straight_line_beh
      TOP_MODULE straight_line
      NO_VERIFLAT
      VERILOG_SOURCES ${_straight_line_src}
      VERILATOR_ARGS --timing
      HARNESS
        ${CMAKE_CURRENT_SOURCE_DIR}/test/verilower/straight_line/cosim/test_straight_line_beh.cpp
    )
    add_cosim_test(
      NAME test_cosim_straight_line_lower
      TOP_MODULE straight_line
      NO_VERIFLAT
      VERILOG_SOURCES ${_straight_line_lowered}
      VERILATOR_ARGS -Wno-CASEINCOMPLETE
      HARNESS
        ${CMAKE_CURRENT_SOURCE_DIR}/test/verilower/straight_line/cosim/test_straight_line_lower.cpp
    )

    # The same differential check as a pure-SV bench (verilator --binary
    # --timing): source and renamed verilower output side by side, one
    # testbench, the exit status carrying the verdict. The shape phases 5-6
    # register their protocol cosims in.
    add_verilower_sv_cosim(NAME straight_line VERILATOR_ARGS -Wno-CASEINCOMPLETE)

    # The enable half of the §5.3 emission, differentially checked: the
    # reference honours `iff`, so en toggles irregularly against both models.
    add_verilower_sv_cosim(NAME enable_line VERILATOR_ARGS -Wno-CASEINCOMPLETE)

    # The §4 path cover: an if/else fork with unequal arms, a cut-point-free
    # conditional inside a segment, and a no-default case with a fall-through
    # path — five input-pinned behavioural/lowered pairs so every edge runs.
    add_verilower_sv_cosim(NAME branch_line VERILATOR_ARGS -Wno-CASEINCOMPLETE)

    # The §7 loops: the §7.3 wait-state self-loop taken and not taken, the
    # rolled countdown's first and last lap, and the captured non-constant
    # count entering and skipping (§11.2's back-edge coverage).
    add_verilower_sv_cosim(NAME count_line VERILATOR_ARGS -Wno-CASEINCOMPLETE)

    # The ADR's worked example, end to end (§2, §12 phase 6): the perpetual
    # A.1 transmitter — prologue, forever, wait-state, unrolled DATA group,
    # rolled baud countdowns, the back-to-back frame restart.
    add_verilower_sv_cosim(NAME rs232_tx VERILATOR_ARGS -Wno-CASEINCOMPLETE)

    # The §8 receiver: continue as the protocol's error handling — the
    # mid-bit glitch and the framing-error drop — then recovery.
    add_verilower_sv_cosim(NAME rs232_rx VERILATOR_ARGS -Wno-CASEINCOMPLETE)

    # §5.3 is normative on synthesis: the sync reset stays outside the clock
    # enable, which the flop primitive already provides — through yosys the
    # flops are $sdffe, never the fabric-costing $sdffce. Skipped when yosys
    # is absent or predates opt_dff's $sdffe extraction (0.10).
    find_program(YOSYS_BIN yosys HINTS ${VERILATOR_ROOT}/../../bin)
    set(_yosys_version "")
    if(YOSYS_BIN)
      execute_process(COMMAND ${YOSYS_BIN} -V
                      OUTPUT_VARIABLE _yosys_version_line
                      OUTPUT_STRIP_TRAILING_WHITESPACE)
      string(REGEX MATCH "[0-9]+\\.[0-9]+" _yosys_version "${_yosys_version_line}")
    endif()
    if(NOT YOSYS_BIN)
      message(STATUS "Veriparse Apps: yosys not found, skipping the synthesis check")
    elseif(_yosys_version VERSION_LESS 0.10)
      message(STATUS "Veriparse Apps: yosys ${_yosys_version} < 0.10 "
                     "(opt_dff yields no $sdffe), skipping the synthesis check")
    else()
      add_test(NAME test_synth_enable_line
               COMMAND ${YOSYS_BIN} -q -p
                 "read_verilog -sv ${enable_line_LOWERED_V}; proc; opt_clean; opt_dff; select -assert-any t:$sdffe; select -assert-none t:$sdffce")
      set_tests_properties(test_synth_enable_line PROPERTIES LABELS "cosim")
    endif()
  endif()

endif()
