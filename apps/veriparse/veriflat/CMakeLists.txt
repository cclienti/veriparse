########################################
### Global options
########################################

cmake_minimum_required (VERSION 3.15...3.27)
project (veriflat)


########################################
### Source Files
########################################

file (GLOB source_files
  src/main.cpp
  ../common/parameters_overloading.cpp)


########################################
### Executable generation
########################################

add_executable (veriflat ${source_files})
target_include_directories (veriflat PRIVATE ../common)
target_link_libraries (veriflat veriparse_static)
target_link_libraries (veriflat Boost::program_options Boost::filesystem)

install (TARGETS veriflat RUNTIME DESTINATION bin)
