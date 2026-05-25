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
  src/parameters_overloading.cpp)


########################################
### Executable generation
########################################

add_executable (veriflat ${source_files})
target_link_libraries (veriflat veriparse_static)
target_link_libraries (veriflat Boost::program_options Boost::filesystem)

# conda-forge's gcc_win-64 activation puts -mwindows in LDFLAGS, and some
# boost imported targets propagate it via INTERFACE_LINK_OPTIONS. Either
# way GCC ends up selecting crtexewin.o (which expects WinMain()). Force
# -mconsole as the target's own LINK_FLAGS so it lands AFTER everything
# else and wins the "last -m flag" rule for startup-object selection.
if(MINGW)
  set_target_properties(veriflat PROPERTIES LINK_FLAGS "-mconsole")
endif()

install (TARGETS veriflat RUNTIME DESTINATION bin)
