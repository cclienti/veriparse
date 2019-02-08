cmake_minimum_required (VERSION 3.9)
project (veriparse_apps)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)
enable_testing()


#----------------------------------------
# Visit subdirs
#----------------------------------------

add_subdirectory(veriflat)
add_subdirectory(veriobf)


#----------------------------------------
# Handle common test of veriflat and
# veriobf
#----------------------------------------

set(VERIPARSE_APPS_TESTS
  "tiny_aes_128" "tiny_aes_192" "tiny_aes_256")


if(NOT MINGW)

  add_custom_target(test_veriparse_apps_license ALL DEPENDS license_test.dat)
  add_custom_command(
    OUTPUT
    license_test.dat
    COMMAND
    ${CMAKE_CURRENT_SOURCE_DIR}/test/common/license.sh
    ${CMAKE_CURRENT_BINARY_DIR}/../license/license_generator/license_generator
    ${CMAKE_CURRENT_BINARY_DIR}/../license/license_validator/license_validator
    DEPENDS
    license_generator license_validator)

  foreach(VERIPARSE_APPS_TEST ${VERIPARSE_APPS_TESTS})
    add_custom_target(test_veriparse_${VERIPARSE_APPS_TEST} DEPENDS test_veriparse_apps_license veriflat veriobf)

    add_test(
      NAME
      test_veriparse_${VERIPARSE_APPS_TEST}
      COMMAND
      make -f ${CMAKE_CURRENT_SOURCE_DIR}/test/${VERIPARSE_APPS_TEST}/project/Makefile
      VERIFLAT=${CMAKE_CURRENT_BINARY_DIR}/veriflat/veriflat
      VERIOBF=${CMAKE_CURRENT_BINARY_DIR}/veriobf/veriobf
      clean iverilog_check veriflat_check veriobf_check)

    set_tests_properties(
      test_veriparse_${VERIPARSE_APPS_TEST}
      PROPERTIES ENVIRONMENT VERIPARSE_LICENSE_FILE=${CMAKE_CURRENT_BINARY_DIR}/license_test.dat)
  endforeach()

endif()
