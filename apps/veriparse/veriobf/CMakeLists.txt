########################################
### Global options
########################################

cmake_minimum_required (VERSION 3.9)
project (veriobf)


########################################
### Source Files
########################################

file (GLOB source_files
  src/main.cpp)


########################################
### Executable generation
########################################

add_executable (veriobf ${source_files})
target_link_libraries (veriobf license)
target_link_libraries (veriobf veriparse_static)
target_link_libraries (veriobf ${Boost_LIBRARIES})

install (TARGETS veriobf RUNTIME DESTINATION bin)
