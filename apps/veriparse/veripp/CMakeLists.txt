########################################
### Global options
########################################

cmake_minimum_required (VERSION 3.15...3.27)
project (veripp)


########################################
### Source Files
########################################

file (GLOB source_files
  src/main.cpp)


########################################
### Executable generation
########################################

add_executable (veripp ${source_files})
target_link_libraries (veripp veriparse_static)
target_link_libraries (veripp Boost::program_options Boost::filesystem)

install (TARGETS veripp RUNTIME DESTINATION bin)
