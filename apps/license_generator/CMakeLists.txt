########################################
### Global options
########################################

cmake_minimum_required (VERSION 3.9)
project (license_generator)


########################################
### Source Files
########################################

file (GLOB source_files
  src/license_generator.cpp
  src/main.cpp)


########################################
### Executable generation
########################################

add_executable (license_generator ${source_files})
target_link_libraries (license_generator license)
target_link_libraries (license_generator veriparse_static)
target_link_libraries (license_generator crypto)
target_link_libraries (license_generator ${Boost_LIBRARIES})

# if(WIN32)
#   target_link_libraries (license_generator ws2_32)
# endif()

# install (TARGETS license_generator RUNTIME DESTINATION bin)
