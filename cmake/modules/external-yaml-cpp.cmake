set(YAMLCPP_GIT_PREFIX_DIR ${CMAKE_CURRENT_BINARY_DIR}/external/yaml-cpp)
set(YAMLCPP_GIT_INSTALL_DIR ${YAMLCPP_GIT_PREFIX_DIR}/install)

message(STATUS "External project yaml-cpp installation in ${YAMLCPP_GIT_INSTALL_DIR}")

set(YAMLCPP_LIBRARY yaml-cpp)
get_filename_component(YAMLCPP_LIBRARY_PATH
  ${YAMLCPP_GIT_INSTALL_DIR}/lib/${CMAKE_FIND_LIBRARY_PREFIXES}${YAMLCPP_LIBRARY}${CMAKE_STATIC_LIBRARY_SUFFIX}
  REALPATH)

ExternalProject_Add(yaml-cpp-git
    BUILD_IN_SOURCE   1
    PREFIX            ${YAMLCPP_GIT_PREFIX_DIR}
    GIT_REPOSITORY    "https://github.com/jbeder/yaml-cpp.git"
    GIT_TAG           "562aefc"
    CMAKE_ARGS        "-DCMAKE_INSTALL_PREFIX=${YAMLCPP_GIT_INSTALL_DIR}"
    UPDATE_COMMAND    ""
    BUILD_BYPRODUCTS  ${YAMLCPP_LIBRARY_PATH}
)

set_directory_properties(PROPERTY CLEAN_NO_CUSTOM ${YAMLCPP_GIT_PREFIX_DIR})

# Include directory
set(YAMLCPP_INCLUDE_DIR ${YAMLCPP_GIT_INSTALL_DIR}/include)

# Lib declaration
add_library(${YAMLCPP_LIBRARY} STATIC IMPORTED)
add_dependencies(${YAMLCPP_LIBRARY} yaml-cpp-git)
set_property(TARGET ${YAMLCPP_LIBRARY} PROPERTY IMPORTED_LOCATION ${YAMLCPP_LIBRARY_PATH})
