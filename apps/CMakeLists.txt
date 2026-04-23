cmake_minimum_required (VERSION 3.9)
project (apps)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)
enable_testing()


#----------------------------------------
# Visit subdirs
#----------------------------------------

add_subdirectory(veriparse)
