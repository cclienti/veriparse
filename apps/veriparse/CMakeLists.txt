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

    # Nested rolled repeats (§7.2, §15): one countdown per nesting depth —
    # the 2D-scan shape, the captured inner count entering and skipping.
    add_verilower_sv_cosim(NAME nested_line VERILATOR_ARGS -Wno-CASEINCOMPLETE)

    # Mixed rolled nesting under jumps (§7.2, §8, §15): the countdown depth
    # threading through a counterless for layer, a captured count, and a
    # break abandoning inner laps with its commit coalesced.
    add_verilower_sv_cosim(NAME mixed_line VERILATOR_ARGS -Wno-CASEINCOMPLETE)

    # §6.1 blocking temporaries under live data: the declared-width carry
    # corner (§11.6) through the materialized wires, alternating forever.
    add_verilower_sv_cosim(NAME temp_line VERILATOR_ARGS -Wno-CASEINCOMPLETE)

    # §6.2 decoded outputs (ADR-0014 §12 phase 9): busy/done/sel through
    # the §7.3 wait-state, a fork-arrival tree, and the perpetual wrap —
    # the tracking-vs-holding window probed by the cycle-sampled bench.
    add_verilower_sv_cosim(NAME decode_line VERILATOR_ARGS -Wno-CASEINCOMPLETE)

    # §6.2 output encoding (§12 phase 10): the same cycle-sampled bench
    # over a machine whose state bits ARE the outputs.
    add_verilower_sv_cosim(NAME decode_enc VERILATOR_ARGS -Wno-CASEINCOMPLETE)

    # §7.4 task inlining (§12 phase 11): the Appendix-B byte write with
    # the LOW/HIGH halves as one task called per half-bit, two
    # back-to-back frames for §A.3's chaining case.
    add_verilower_sv_cosim(NAME i2c_line VERILATOR_ARGS -Wno-CASEINCOMPLETE)

    # §7.4 package tasks: the sub-sequence lives in a package, so the
    # machine's signals travel through its formals only (IEEE §26.2) —
    # const ref clock, inout accumulator, input count, zero-skip frame.
    add_verilower_sv_cosim(NAME pkg_line VERILATOR_ARGS -Wno-CASEINCOMPLETE)

    # §6.3 interface ports: the machine's whole boundary is an interface,
    # kept as it stands — a member read in a guard, one committed from a
    # task body, one carried as an input actual, one copied out under a
    # cut-point-free branch.
    add_verilower_sv_cosim(NAME iface_line VERILATOR_ARGS -Wno-CASEINCOMPLETE)

    # ADR-0015 §5.1 interface subroutine calls: the handshake helper lives
    # in the interface and the machine calls it through the port, twice —
    # spliced, inlined per site, the waits clocking on the bus's own clock.
    add_verilower_sv_cosim(NAME iface_call VERILATOR_ARGS -Wno-CASEINCOMPLETE)

    # §7.4 default arguments and early return: the task's default fills
    # the omitted actual and `if (abort) return;` jumps to the body's end
    # (IEEE §13.5.3, §13.3 — both measured).
    add_verilower_sv_cosim(NAME ret_line VERILATOR_ARGS -Wno-CASEINCOMPLETE)

    # The in-flattener slot (ADR-0014 §10.3): veriflat --fsm compiles one
    # marked module instantiated twice with different parameters — the
    # shape a per-module verilower run cannot compile (§15) — each
    # instance's machine at its own N.
    add_verilower_sv_cosim(NAME fsm_flat TOOL veriflat TOOL_ARGS --fsm
                           VERILATOR_ARGS -Wno-CASEINCOMPLETE)

    # A production RMII MAC receiver, three ways: the behavioural rewrite
    # against its verilower output AND against the pre-existing
    # three-process implementation it was rewritten from — an independent
    # reference, not a transcription (§1, §11).
    add_verilower_sv_cosim(NAME rmii_mac_rx VERILATOR_ARGS -Wno-CASEINCOMPLETE
                           EXTRA_SOURCES
                           ${CMAKE_CURRENT_SOURCE_DIR}/test/verilower/rmii_mac_rx/cosim/rmii_mac_rx_ref.sv)

    # The §10.2 state map and its graphviz view on the ADR's own example:
    # the JSON carries the labelled names and the reset contract, the dot
    # marks the entry with a double circle — and keeps the edges bare of
    # register updates unless asked.
    add_test(NAME test_fsm_state_map
             COMMAND sh -c "$<TARGET_FILE:verilower> --sv -t rs232_tx \
                     -o ${CMAKE_CURRENT_BINARY_DIR}/rs232_map.v \
                     --state-map ${CMAKE_CURRENT_BINARY_DIR}/rs232_map.json \
                     --fsm-dot ${CMAKE_CURRENT_BINARY_DIR}/rs232_map.dot \
                     ${CMAKE_CURRENT_SOURCE_DIR}/test/verilower/rs232_tx/src/rs232_tx.sv \
                     && grep -q '__fsm_WAIT_SEND' ${CMAKE_CURRENT_BINARY_DIR}/rs232_map.json \
                     && grep -q '\"registers\": \\[\"tx\", \"busy\", \"__fsm_state\"\\]' \
                        ${CMAKE_CURRENT_BINARY_DIR}/rs232_map.json \
                     && grep -q 'doublecircle' ${CMAKE_CURRENT_BINARY_DIR}/rs232_map.dot \
                     && ! grep -q 'busy <=' ${CMAKE_CURRENT_BINARY_DIR}/rs232_map.dot")
    set_tests_properties(test_fsm_state_map PROPERTIES LABELS unittest)

    # An explicitly requested --state-map is written even when no process
    # was compiled: a consumer opening the promised path must not fail.
    add_test(NAME test_fsm_state_map_empty
             COMMAND sh -c "$<TARGET_FILE:verilower> --sv -t plain \
                     -o ${CMAKE_CURRENT_BINARY_DIR}/plain_map.v \
                     --state-map ${CMAKE_CURRENT_BINARY_DIR}/plain_map.json \
                     ${CMAKE_CURRENT_SOURCE_DIR}/test/verilower/plain/src/plain.sv \
                     && test -f ${CMAKE_CURRENT_BINARY_DIR}/plain_map.json")
    set_tests_properties(test_fsm_state_map_empty PROPERTIES LABELS unittest)

    # Two marked processes: distinct ordinal prefixes, two entries in the
    # JSON, and one digraph holding two cluster subgraphs — a single
    # graphviz invocation renders the whole machine set.
    add_test(NAME test_fsm_state_map_dual
             COMMAND sh -c "$<TARGET_FILE:verilower> --sv -t dual \
                     -o ${CMAKE_CURRENT_BINARY_DIR}/dual_map.v \
                     --state-map ${CMAKE_CURRENT_BINARY_DIR}/dual_map.json \
                     --fsm-dot ${CMAKE_CURRENT_BINARY_DIR}/dual_map.dot \
                     ${CMAKE_CURRENT_SOURCE_DIR}/test/verilower/dual/src/dual.sv \
                     && grep -q '__fsm0_state' ${CMAKE_CURRENT_BINARY_DIR}/dual_map.json \
                     && grep -q '__fsm1_state' ${CMAKE_CURRENT_BINARY_DIR}/dual_map.json \
                     && test \"$(grep -c '^digraph' ${CMAKE_CURRENT_BINARY_DIR}/dual_map.dot)\" = 1 \
                     && test \"$(grep -c 'subgraph' ${CMAKE_CURRENT_BINARY_DIR}/dual_map.dot)\" = 2")
    set_tests_properties(test_fsm_state_map_dual PROPERTIES LABELS unittest)

    # --param-map parameterizes the top from the command line, veriflat-style:
    # an override re-sizes the machine (the repeat(N) countdown), a kept
    # parameter survives as a parameter of the output while only the datapath
    # reads it, and one the machine's structure depends on fails loudly
    # (§7.2) rather than compile a silently different machine.
    add_test(NAME test_fsm_param_map
             COMMAND sh -c "$<TARGET_FILE:verilower> --sv -t param_line -p '{N: 6, SEED:}' \
                     -o ${CMAKE_CURRENT_BINARY_DIR}/param_line.v \
                     ${CMAKE_CURRENT_SOURCE_DIR}/test/verilower/param_line/src/param_line.sv \
                     && grep -q \"__fsm_cnt <= 3'd5\" ${CMAKE_CURRENT_BINARY_DIR}/param_line.v \
                     && grep -q 'parameter logic \\[7:0\\] SEED' ${CMAKE_CURRENT_BINARY_DIR}/param_line.v \
                     && ! grep -q 'parameter int N' ${CMAKE_CURRENT_BINARY_DIR}/param_line.v \
                     && ! $<TARGET_FILE:verilower> --sv -t param_line -p '{N:}' \
                     -o ${CMAKE_CURRENT_BINARY_DIR}/param_line_keep.v \
                     ${CMAKE_CURRENT_SOURCE_DIR}/test/verilower/param_line/src/param_line.sv")
    set_tests_properties(test_fsm_param_map PROPERTIES LABELS unittest)

    # The same map through veriflat: an override folds into the flattened
    # module (N=6 re-sizes the [N-1:0] ports), a kept parameter survives
    # with its symbolic uses intact — the two tools share one contract.
    add_test(NAME test_flat_param_map
             COMMAND sh -c "$<TARGET_FILE:veriflat> --sv -t flat_param -p '{N: 6, SEED:}' \
                     -o ${CMAKE_CURRENT_BINARY_DIR}/flat_param.v \
                     ${CMAKE_CURRENT_SOURCE_DIR}/test/flat_param/src/flat_param.sv \
                     && grep -q 'logic \\[5:0\\] q' ${CMAKE_CURRENT_BINARY_DIR}/flat_param.v \
                     && grep -q 'parameter logic \\[7:0\\] SEED' ${CMAKE_CURRENT_BINARY_DIR}/flat_param.v \
                     && grep -q 'SEED\\[5:0\\]' ${CMAKE_CURRENT_BINARY_DIR}/flat_param.v \
                     && ! grep -q 'parameter int N' ${CMAKE_CURRENT_BINARY_DIR}/flat_param.v")
    set_tests_properties(test_flat_param_map PROPERTIES LABELS unittest)

    # veriflat --fsm resolves each instance with its own parameters before
    # the FSM slot: two instantiations of one marked module get separately
    # sized machines (N=3 -> a 2-bit countdown from 2'd2, N=6 -> a 3-bit
    # one from 3'd5) and no behavioural initial survives. Without --fsm the
    # marked processes flatten as-is and the log says so (ADR-0014 §2).
    add_test(NAME test_fsm_flat
             COMMAND sh -c "$<TARGET_FILE:veriflat> --sv --fsm -t fsm_flat \
                     -o ${CMAKE_CURRENT_BINARY_DIR}/fsm_flat_flat.v \
                     ${CMAKE_CURRENT_SOURCE_DIR}/test/verilower/fsm_flat/src/fsm_flat.sv \
                     && grep -q \"u_fast___fsm_cnt <= 2'd2\" ${CMAKE_CURRENT_BINARY_DIR}/fsm_flat_flat.v \
                     && grep -q \"u_slow___fsm_cnt <= 3'd5\" ${CMAKE_CURRENT_BINARY_DIR}/fsm_flat_flat.v \
                     && ! grep -q 'initial' ${CMAKE_CURRENT_BINARY_DIR}/fsm_flat_flat.v \
                     && $<TARGET_FILE:veriflat> --sv -t fsm_flat \
                     -o ${CMAKE_CURRENT_BINARY_DIR}/fsm_flat_nofsm.v \
                     --log ${CMAKE_CURRENT_BINARY_DIR}/fsm_flat_nofsm.log \
                     ${CMAKE_CURRENT_SOURCE_DIR}/test/verilower/fsm_flat/src/fsm_flat.sv \
                     && grep -q 'pass --fsm' ${CMAKE_CURRENT_BINARY_DIR}/fsm_flat_nofsm.log \
                     && grep -q 'initial' ${CMAKE_CURRENT_BINARY_DIR}/fsm_flat_nofsm.v \
                     && grep -q 'veriparse_fsm' ${CMAKE_CURRENT_BINARY_DIR}/fsm_flat_nofsm.v")
    set_tests_properties(test_fsm_flat PROPERTIES LABELS unittest)

    # The mark is never skipped in silence (ADR-0014 §2), including the
    # corners: a mark nested in a generate block still draws the no---fsm
    # note, and under --fsm a marked module the top never instantiates
    # draws a warning naming its own run.
    add_test(NAME test_fsm_mark_notes
             COMMAND sh -c "$<TARGET_FILE:veriflat> --sv -t mark_notes \
                     -o ${CMAKE_CURRENT_BINARY_DIR}/mark_notes_nofsm.v \
                     --log ${CMAKE_CURRENT_BINARY_DIR}/mark_notes_nofsm.log \
                     ${CMAKE_CURRENT_SOURCE_DIR}/test/verilower/mark_notes/src/mark_notes.sv \
                     && grep -q 'pass --fsm' ${CMAKE_CURRENT_BINARY_DIR}/mark_notes_nofsm.log \
                     && $<TARGET_FILE:veriflat> --sv --fsm -t mark_notes \
                     -o ${CMAKE_CURRENT_BINARY_DIR}/mark_notes_fsm.v \
                     --log ${CMAKE_CURRENT_BINARY_DIR}/mark_notes_fsm.log \
                     ${CMAKE_CURRENT_SOURCE_DIR}/test/verilower/mark_notes/src/mark_notes.sv \
                     && grep -q 'not instantiated' ${CMAKE_CURRENT_BINARY_DIR}/mark_notes_fsm.log")
    set_tests_properties(test_fsm_mark_notes PROPERTIES LABELS unittest)

    # The synthesizable-subset rules in force today are mode-independent
    # (virtual interfaces), so --fsm must not skip the input verdict:
    # deadcode-during-flatten can prune the offending declaration before
    # any output-side check would see it.
    add_test(NAME test_fsm_virtual_iface
             COMMAND sh -c "! $<TARGET_FILE:veriflat> --sv --fsm -d \
                     -t synthesizable_check_virtual0 \
                     -o ${CMAKE_CURRENT_BINARY_DIR}/fsm_virtual.v \
                     ${CMAKE_SOURCE_DIR}/lib/test/passes/analysis/testcases/synthesizable_check_virtual0.sv")
    set_tests_properties(test_fsm_virtual_iface PROPERTIES LABELS unittest)

    # A parameter map failure is never silent: malformed YAML, a sequence
    # instead of a map, a non-scalar value, a value the Verilog parser
    # refuses, and a name the top module does not declare each exit
    # non-zero — the last two would otherwise compile a silently
    # default-parameterized design.
    add_test(NAME test_param_map_errors
             COMMAND sh -c "! $<TARGET_FILE:veriflat> --sv -t flat_param -p '{N: ' \
                     -o ${CMAKE_CURRENT_BINARY_DIR}/param_err.v \
                     ${CMAKE_CURRENT_SOURCE_DIR}/test/flat_param/src/flat_param.sv \
                     && ! $<TARGET_FILE:veriflat> --sv -t flat_param -p '[1, 2]' \
                     -o ${CMAKE_CURRENT_BINARY_DIR}/param_err.v \
                     ${CMAKE_CURRENT_SOURCE_DIR}/test/flat_param/src/flat_param.sv \
                     && ! $<TARGET_FILE:verilower> --sv -t param_line -p '{N: [1, 2]}' \
                     -o ${CMAKE_CURRENT_BINARY_DIR}/param_err.v \
                     ${CMAKE_CURRENT_SOURCE_DIR}/test/verilower/param_line/src/param_line.sv \
                     && ! $<TARGET_FILE:veriflat> --sv -t flat_param -p '{N: 5px}' \
                     -o ${CMAKE_CURRENT_BINARY_DIR}/param_err.v \
                     ${CMAKE_CURRENT_SOURCE_DIR}/test/flat_param/src/flat_param.sv \
                     && ! $<TARGET_FILE:veriflat> --sv -t flat_param -p '{BAD: 1}' \
                     -o ${CMAKE_CURRENT_BINARY_DIR}/param_err.v \
                     ${CMAKE_CURRENT_SOURCE_DIR}/test/flat_param/src/flat_param.sv \
                     && ! $<TARGET_FILE:verilower> --sv -t param_line -p '{n: 6}' \
                     -o ${CMAKE_CURRENT_BINARY_DIR}/param_err.v \
                     ${CMAKE_CURRENT_SOURCE_DIR}/test/verilower/param_line/src/param_line.sv")
    set_tests_properties(test_param_map_errors PROPERTIES LABELS unittest)

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
