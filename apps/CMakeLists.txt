cmake_minimum_required (VERSION 3.15...3.27)
project (apps)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)
enable_testing()


#----------------------------------------
# Visit subdirs
#----------------------------------------

add_subdirectory(veriparse)
