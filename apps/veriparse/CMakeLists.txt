cmake_minimum_required (VERSION 3.15...3.27)
project (veriparse_apps)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)
enable_testing()


#----------------------------------------
# Visit subdirs
#----------------------------------------

add_subdirectory(veriflat)
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
if(WIN32)
  install(FILES scripts/veriparse.ps1 scripts/veriparse.cmd DESTINATION bin)
else()
  install(FILES scripts/veriparse
          DESTINATION bin
          PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
                      GROUP_EXECUTE GROUP_READ
                      WORLD_EXECUTE WORLD_READ)
endif()
#----------------------------------------
# Cosim tests (label "cosim") — verilator-built C++ model of the design
# (optionally via veriflat), GoogleTest harness drives it. Each test
# opt-ins by shipping a test/<name>/cosim/test_<top>.cpp harness.
#----------------------------------------

if(NOT WIN32)
  find_package(verilator CONFIG QUIET)
  if(NOT verilator_FOUND)
    message(STATUS "Veriparse Apps: verilator not found, skipping cosim tests")
  else()
    include(${CMAKE_SOURCE_DIR}/cmake/modules/add_cosim_test.cmake)

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
  endif()

endif()
