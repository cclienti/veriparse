########################################
### Global options
########################################

cmake_minimum_required (VERSION 3.9)
project (license_validator)


########################################
### Source Files
########################################

file (GLOB source_files
  src/license_validator.cpp
  src/main.cpp)


########################################
### Executable generation
########################################

add_executable (license_validator ${source_files})
target_link_libraries (license_validator license)
target_link_libraries (license_validator veriparse_static)
target_link_libraries (license_validator ${Boost_LIBRARIES})

# if(WIN32)
#   target_link_libraries (license_validator ws2_32)
# endif()

# install (TARGETS license_validator RUNTIME DESTINATION bin)
