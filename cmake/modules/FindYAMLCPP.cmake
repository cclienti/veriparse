# FindYAMLCPP.cmake - Find the yaml-cpp library
#
# This module defines:
#   YAMLCPP::YAMLCPP - imported target for libyaml-cpp
#   YAMLCPP_FOUND

find_path(YAMLCPP_INCLUDE_DIR NAMES yaml-cpp/yaml.h)
find_library(YAMLCPP_LIBRARY NAMES yaml-cpp)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(YAMLCPP DEFAULT_MSG YAMLCPP_LIBRARY YAMLCPP_INCLUDE_DIR)
mark_as_advanced(YAMLCPP_INCLUDE_DIR YAMLCPP_LIBRARY)

if(YAMLCPP_FOUND AND NOT TARGET YAMLCPP::YAMLCPP)
  add_library(YAMLCPP::YAMLCPP UNKNOWN IMPORTED)
  set_target_properties(YAMLCPP::YAMLCPP PROPERTIES
    IMPORTED_LOCATION "${YAMLCPP_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${YAMLCPP_INCLUDE_DIR}")
endif()
