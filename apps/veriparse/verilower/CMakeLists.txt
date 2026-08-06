########################################
### Global options
########################################

cmake_minimum_required (VERSION 3.15...3.27)
project (verilower)


########################################
### Source Files
########################################

file (GLOB source_files
  src/main.cpp)


########################################
### Executable generation
########################################

add_executable (verilower ${source_files})
target_link_libraries (verilower veriparse_static)
target_link_libraries (verilower Boost::program_options Boost::filesystem)

install (TARGETS verilower RUNTIME DESTINATION bin)
