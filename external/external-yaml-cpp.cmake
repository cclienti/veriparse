if(NOT VERIPARSE_PROCESSOR_COUNT)
  set(VERIPARSE_PROCESSOR_COUNT 1)
  message(STATUS "VERIPARSE_PROCESSOR_COUNT not set, forcing to 1")
endif()

set(YAMLCPP_GIT_PREFIX_DIR ${CMAKE_CURRENT_BINARY_DIR}/external/yaml-cpp)
set(YAMLCPP_GIT_INSTALL_DIR ${YAMLCPP_GIT_PREFIX_DIR}/install)

message(STATUS "External project yaml-cpp installation in ${YAMLCPP_GIT_INSTALL_DIR}")

ExternalProject_Add(yaml-cpp-ext
    BUILD_IN_SOURCE   1
    PREFIX            ${YAMLCPP_GIT_PREFIX_DIR}
    GIT_REPOSITORY    "https://github.com/jbeder/yaml-cpp.git"
    GIT_TAG           "562aefc"
    CMAKE_ARGS        -DCMAKE_INSTALL_PREFIX=${YAMLCPP_GIT_INSTALL_DIR}
                      -DCMAKE_BUILD_TYPE=Release
                      -DYAML_CPP_BUILD_TESTS=OFF
    UPDATE_COMMAND    ""
)

set_directory_properties(PROPERTY CLEAN_NO_CUSTOM ${YAMLCPP_GIT_PREFIX_DIR})
