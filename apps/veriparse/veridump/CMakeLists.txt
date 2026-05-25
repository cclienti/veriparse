########################################
### Global options
########################################

cmake_minimum_required (VERSION 3.15...3.27)
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
target_link_libraries (veridump Boost::program_options Boost::filesystem)

# See comment in veriflat/CMakeLists.txt about -mconsole on MinGW.
if(MINGW)
  set_target_properties(veridump PROPERTIES LINK_FLAGS "-mconsole")
endif()

install (TARGETS veridump RUNTIME DESTINATION bin)
