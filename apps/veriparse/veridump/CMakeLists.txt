########################################
### Global options
########################################

cmake_minimum_required (VERSION 3.9)
project (veridump)


########################################
### Source Files
########################################

file (GLOB source_files
  src/main.cpp)


########################################
### Executable generation
########################################

add_executable (veridump ${source_files})
target_link_libraries (veridump veriparse_static)
target_link_libraries (veridump ${Boost_LIBRARIES})

install (TARGETS veridump RUNTIME DESTINATION bin)
