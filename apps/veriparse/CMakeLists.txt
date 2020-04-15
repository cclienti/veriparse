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
# Install examples
#----------------------------------------

install(DIRECTORY test/common/Makefiles
        DESTINATION share/veriparse/examples/common)
install(DIRECTORY test/dclkfifolut
        DESTINATION share/veriparse/examples)
install(DIRECTORY test/tiny_aes_128
        DESTINATION share/veriparse/examples)
install(DIRECTORY test/double_fpu-master
        DESTINATION share/veriparse/examples)
install(FILES test/readme.txt
        DESTINATION share/veriparse/examples)


#----------------------------------------
# Handle common test of veriflat and
# veriobf
#----------------------------------------

set(VERIPARSE_APPS_TESTS
  "tiny_aes_128" "tiny_aes_192" "tiny_aes_256"
  "hynoc_5p"
  "dclkfifolut"
  "double_fpu-master")


if(NOT MINGW)

  # Prepare license used by all test
  add_custom_target(test_veriparse_apps_license ALL DEPENDS license_test.dat)
  add_custom_command(OUTPUT license_test.dat
                     COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test/common/license.sh
                             ${CMAKE_CURRENT_BINARY_DIR}/../license/license_generator/license_generator
                             ${CMAKE_CURRENT_BINARY_DIR}/../license/license_validator/license_validator
                             ${CMAKE_CURRENT_SOURCE_DIR}/../license/license_generator/resources/private.pem
                     DEPENDS license_generator license_validator)

  # Iterate over all tests
  foreach(VERIPARSE_APPS_TEST ${VERIPARSE_APPS_TESTS})
    file(GLOB VERIPARSE_PROJECT_TEST_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/test/${VERIPARSE_APPS_TEST}/project*)

    # Iterate over all project within a test
    foreach(VERIPARSE_PROJECT_TEST_DIR ${VERIPARSE_PROJECT_TEST_DIRS})
      if(IS_DIRECTORY ${VERIPARSE_PROJECT_TEST_DIR})
        get_filename_component(VERIPARSE_PROJECT_NAME ${VERIPARSE_PROJECT_TEST_DIR} NAME)

        set(VERIPARSE_APPS_TEST_NAME test_veriparse_${VERIPARSE_APPS_TEST}_${VERIPARSE_PROJECT_NAME})
        set(VERIPARSE_WORKING_DIR ${CMAKE_CURRENT_BINARY_DIR}/test/${VERIPARSE_APPS_TEST}/${VERIPARSE_PROJECT_NAME})

        message(STATUS "Veriparse Apps: adding test ${VERIPARSE_APPS_TEST_NAME}")
	file(MAKE_DIRECTORY ${VERIPARSE_WORKING_DIR})

        add_custom_target(${VERIPARSE_APPS_TEST_NAME} DEPENDS test_veriparse_apps_license veriflat veriobf)

        add_test(NAME ${VERIPARSE_APPS_TEST_NAME}
                 COMMAND make -f ${CMAKE_CURRENT_SOURCE_DIR}/test/${VERIPARSE_APPS_TEST}/${VERIPARSE_PROJECT_NAME}/Makefile
                         VERIFLAT=${CMAKE_CURRENT_BINARY_DIR}/veriflat/veriflat
                         VERIOBF=${CMAKE_CURRENT_BINARY_DIR}/veriobf/veriobf
                         clean iverilog_check veriflat_check veriobf_check
                 WORKING_DIRECTORY ${VERIPARSE_WORKING_DIR})

        set_tests_properties(
          ${VERIPARSE_APPS_TEST_NAME}
          PROPERTIES ENVIRONMENT VERIPARSE_LICENSE_FILE=${CMAKE_CURRENT_BINARY_DIR}/license_test.dat)
      endif()
    endforeach()
  endforeach()

endif()
