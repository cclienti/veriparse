cmake_minimum_required (VERSION 3.9)
project (veriparse-apps)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)
enable_testing()

#----------------------------------------
# Visit subdirs
#----------------------------------------

add_subdirectory(license_generator)
add_subdirectory(license_validator)
add_subdirectory(veriflat)
add_subdirectory(veriobf)


#----------------------------------------
# Handle common test of license_generator
# and license_validator
#----------------------------------------

if(NOT MINGW)
  add_custom_target(test_check_license)
  add_dependencies(test_check_license license_generator license_validator)
  add_test(
    NAME
    test_check_license
    COMMAND
    ${CMAKE_CURRENT_SOURCE_DIR}/license_validator/check.sh
    ${CMAKE_CURRENT_BINARY_DIR}/license_generator/license_generator
    ${CMAKE_CURRENT_BINARY_DIR}/license_validator/license_validator
    ${CMAKE_CURRENT_SOURCE_DIR}/license_generator/resources/private.pem)
endif()
