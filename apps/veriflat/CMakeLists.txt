########################################
### Global options
########################################

cmake_minimum_required (VERSION 3.9)
project (veriflat)


########################################
### Source Files
########################################

file (GLOB source_files
  src/main.cpp)


########################################
### Executable generation
########################################

add_executable (veriflat ${source_files})
target_link_libraries (veriflat license)
target_link_libraries (veriflat veriparse_static)
target_link_libraries (veriflat ${Boost_LIBRARIES})

install (TARGETS veriflat RUNTIME DESTINATION bin)
