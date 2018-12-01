cmake_minimum_required (VERSION 3.7)
message(STATUS "Using the veriparse common cmake listfile")
include(ExternalProject)

########################################
### Custom Modules
########################################

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/../cmake/modules")


########################################
### CCache
########################################

option(ENABLE_CCACHE "Use ccache" OFF)

if(ENABLE_CCACHE)
    find_program(CCACHE_FOUND ccache)
    if(CCACHE_FOUND)
	message(STATUS "Using CCache")
	set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
	set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
    endif(CCACHE_FOUND)
endif()


########################################
### Compiler flags
########################################

set(CMAKE_CXX_FLAGS "-std=c++14 -Wall")
set(CMAKE_CXX_FLAGS_DEBUG  "-g -DDEBUG")
set(CMAKE_CXX_FLAGS_FULL_DEBUG  "-g3 -DDEBUG -DFULL_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-Werror -g -O3")
set(CMAKE_CXX_FLAGS_COVERAGE "-g3 -DDEBUG -fprofile-arcs -ftest-coverage")


########################################
### GTest
########################################

include(external-googletest)


########################################
### YAML-CPP
########################################

include(external-yaml-cpp)


########################################
### Boost configuration
########################################

set(BOOST_MODULES
  system program_options log date_time
  log_setup filesystem thread regex chrono atomic)
include(external-boost)


########################################
### GMP
########################################

include(external-gmp)
