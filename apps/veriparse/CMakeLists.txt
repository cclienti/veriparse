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
# Handle common test of veriflat and
# veriobf
#----------------------------------------

set(VERIPARSE_APPS_TESTS
  "tiny_aes_128" "tiny_aes_192" "tiny_aes_256"
  "hynoc_5p"
  "dclkfifolut"
  "double_fpu-master")


if(NOT WIN32)

  # cosim tests (label "cosim"): verilator generates a C++ model of the
  # flattened design, a GoogleTest harness drives it. Coexists with the
  # iverilog/verilator-lint tests below during the migration.
  find_package(verilator CONFIG QUIET)
  if(verilator_FOUND)
    include(${CMAKE_SOURCE_DIR}/cmake/modules/add_cosim_test.cmake)
  else()
    message(STATUS "Veriparse Apps: verilator not found, skipping cosim tests")
  endif()

  # Iterate over all tests
  foreach(VERIPARSE_APPS_TEST ${VERIPARSE_APPS_TESTS})
    file(GLOB VERIPARSE_PROJECT_TEST_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/test/${VERIPARSE_APPS_TEST}/project*)

    # Iterate over all project within a test
    foreach(VERIPARSE_PROJECT_TEST_DIR ${VERIPARSE_PROJECT_TEST_DIRS})
      if(IS_DIRECTORY ${VERIPARSE_PROJECT_TEST_DIR})
        get_filename_component(VERIPARSE_PROJECT_NAME ${VERIPARSE_PROJECT_TEST_DIR} NAME)

        set(VERIPARSE_APPS_TEST_NAME test_veriparse_${VERIPARSE_APPS_TEST}_${VERIPARSE_PROJECT_NAME})
        set(VERIPARSE_WORKING_DIR ${CMAKE_CURRENT_BINARY_DIR}/test/${VERIPARSE_APPS_TEST}/${VERIPARSE_PROJECT_NAME})

        message(STATUS "Veriparse Apps: adding integration test ${VERIPARSE_APPS_TEST_NAME}")
        file(MAKE_DIRECTORY ${VERIPARSE_WORKING_DIR})

        add_custom_target(${VERIPARSE_APPS_TEST_NAME} DEPENDS veriflat veriobf)

        # Integration test (iverilog simulation)
        add_test(NAME ${VERIPARSE_APPS_TEST_NAME}
                 COMMAND make -f ${CMAKE_CURRENT_SOURCE_DIR}/test/${VERIPARSE_APPS_TEST}/${VERIPARSE_PROJECT_NAME}/Makefile
                         VERIFLAT=${CMAKE_CURRENT_BINARY_DIR}/veriflat/veriflat
                         VERIOBF=${CMAKE_CURRENT_BINARY_DIR}/veriobf/veriobf
                         clean iverilog_check veriflat_check veriobf_check
                 WORKING_DIRECTORY ${VERIPARSE_WORKING_DIR})
        set_tests_properties(${VERIPARSE_APPS_TEST_NAME} PROPERTIES LABELS "integration" RESOURCE_LOCK "${VERIPARSE_WORKING_DIR}")

        # Verilator lint test
        set(VERIPARSE_VERILATOR_TEST_NAME test_verilator_${VERIPARSE_APPS_TEST}_${VERIPARSE_PROJECT_NAME})
        message(STATUS "Veriparse Apps: adding verilator test ${VERIPARSE_VERILATOR_TEST_NAME}")

        add_test(NAME ${VERIPARSE_VERILATOR_TEST_NAME}
                 COMMAND make -f ${CMAKE_CURRENT_SOURCE_DIR}/test/${VERIPARSE_APPS_TEST}/${VERIPARSE_PROJECT_NAME}/Makefile
                         VERIFLAT=${CMAKE_CURRENT_BINARY_DIR}/veriflat/veriflat
                         VERIOBF=${CMAKE_CURRENT_BINARY_DIR}/veriobf/veriobf
                         verilator_check
                 WORKING_DIRECTORY ${VERIPARSE_WORKING_DIR})
        set_tests_properties(${VERIPARSE_VERILATOR_TEST_NAME} PROPERTIES LABELS "verilator" RESOURCE_LOCK "${VERIPARSE_WORKING_DIR}")

      endif()
    endforeach()
  endforeach()

  #----------------------------------------
  # Cosim tests — registered separately as designs get migrated. Each test
  # opt-ins by shipping a test/<name>/cosim/test_<top>.cpp harness.
  #----------------------------------------
  if(verilator_FOUND)
    foreach(_aes_width 128 192 256)
      add_cosim_test(
        NAME test_cosim_tiny_aes_${_aes_width}
        TOP_MODULE aes_${_aes_width}
        VERILOG_SOURCES
          ${CMAKE_CURRENT_SOURCE_DIR}/test/tiny_aes_${_aes_width}/src/aes_${_aes_width}.v
          ${CMAKE_CURRENT_SOURCE_DIR}/test/tiny_aes_${_aes_width}/src/round.v
          ${CMAKE_CURRENT_SOURCE_DIR}/test/tiny_aes_${_aes_width}/src/table.v
        HARNESS
          ${CMAKE_CURRENT_SOURCE_DIR}/test/tiny_aes_${_aes_width}/cosim/test_aes_${_aes_width}.cpp
      )
    endforeach()

    add_cosim_test(
      NAME test_cosim_double_fpu
      TOP_MODULE fpu
      VERILOG_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test/double_fpu-master/src/fpu_double.v
        ${CMAKE_CURRENT_SOURCE_DIR}/test/double_fpu-master/src/fpu_add.v
        ${CMAKE_CURRENT_SOURCE_DIR}/test/double_fpu-master/src/fpu_div.v
        ${CMAKE_CURRENT_SOURCE_DIR}/test/double_fpu-master/src/fpu_exceptions.v
        ${CMAKE_CURRENT_SOURCE_DIR}/test/double_fpu-master/src/fpu_mul.v
        ${CMAKE_CURRENT_SOURCE_DIR}/test/double_fpu-master/src/fpu_round.v
        ${CMAKE_CURRENT_SOURCE_DIR}/test/double_fpu-master/src/fpu_sub.v
      HARNESS
        ${CMAKE_CURRENT_SOURCE_DIR}/test/double_fpu-master/cosim/test_fpu.cpp
    )

    add_cosim_test(
      NAME test_cosim_dclkfifolut
      TOP_MODULE dclkfifolut
      VERILOG_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test/dclkfifolut/src/dclkfifolut.v
        ${CMAKE_CURRENT_SOURCE_DIR}/test/dclkfifolut/src/bin2gray.v
        ${CMAKE_CURRENT_SOURCE_DIR}/test/dclkfifolut/src/gray2bin.v
      HARNESS
        ${CMAKE_CURRENT_SOURCE_DIR}/test/dclkfifolut/cosim/test_dclkfifolut.cpp
    )

    # hynoc_5p — veriflat targets hynoc_router_5p; verilator's top is a
    # thin Verilog topology wrapper (2 routers + 8 LIs, no writers/readers)
    # so the C++ noc_traffic kit can drive every local interface.
    set(_hynoc_src ${CMAKE_CURRENT_SOURCE_DIR}/test/hynoc_5p/src)
    set(_hynoc_cosim ${CMAKE_CURRENT_SOURCE_DIR}/test/hynoc_5p/cosim)
    add_library(hynoc_noc_traffic STATIC ${_hynoc_cosim}/kit/noc_traffic.cpp)
    target_include_directories(hynoc_noc_traffic PUBLIC ${_hynoc_cosim}/kit)

    add_cosim_test(
      NAME test_cosim_hynoc_router_5p
      TOP_MODULE hynoc_router_5p
      VERILATOR_TOP hynoc_topology
      VERIFLAT_ARGS -I ${_hynoc_src}
      VERILOG_SOURCES
        ${_hynoc_src}/hynoc_router_5p.v
        ${_hynoc_src}/hynoc_router_base.v
        ${_hynoc_src}/hynoc_egress.v
        ${_hynoc_src}/hynoc_ingress.v
        ${_hynoc_src}/hynoc_ingress_routing_mcast.v
        ${_hynoc_src}/hynoc_ingress_routing_ucast.v
        ${_hynoc_src}/hynoc_prra.v
        ${_hynoc_src}/prra_lut.v
      VERILATOR_EXTRA_SOURCES
        ${_hynoc_cosim}/topology.v
        ${_hynoc_src}/hynoc_local_interface.v
        ${_hynoc_src}/dclkfifolut.v
        ${_hynoc_src}/sclkfifolut.v
        ${_hynoc_src}/bin2gray.v
        ${_hynoc_src}/gray2bin.v
      HARNESS
        ${_hynoc_cosim}/test_hynoc_router_5p.cpp
      EXTRA_LINK_LIBS hynoc_noc_traffic
    )
  endif()

endif()
