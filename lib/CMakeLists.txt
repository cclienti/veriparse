########################################
### Global options
########################################

cmake_minimum_required (VERSION 3.9)
project (veriparse_lib)
include (${CMAKE_CURRENT_LIST_DIR}/../cmake/common.cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

########################################
### License Library
########################################

add_library(license INTERFACE)
target_include_directories(license INTERFACE ${OPENSSL_INCLUDE_DIR})
if(MINGW)
  target_link_libraries(license INTERFACE ${OPENSSL_LIBRARIES} z iphlpapi ws2_32)
else()
  target_link_libraries(license INTERFACE ${OPENSSL_LIBRARIES} dl)
endif()

########################################
### Manage Version in the Library
########################################

include(GetGitRevisionDescription)
git_describe(VERIPARSE_VERSION --tags --dirty=-d)
get_git_head_revision(GIT_REFSPEC VERIPARSE_GIT_SHA1)

message(STATUS "Veriparse version: ${VERIPARSE_VERSION}")
message(STATUS "Veriparse git sha1: ${VERIPARSE_GIT_SHA1}")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/version.cpp.in
               ${CMAKE_CURRENT_BINARY_DIR}/version.cpp)

set(veriparse_version_file "${CMAKE_CURRENT_BINARY_DIR}/version.cpp")

########################################
### Veriparse Library
########################################

find_package(BISON)
find_package(FLEX)

# Verilog parser
set(FLEX_BISON_VERILOG_DIR ${CMAKE_CURRENT_BINARY_DIR}/verilog)
file(MAKE_DIRECTORY ${FLEX_BISON_VERILOG_DIR})
flex_target (Veriparse_VerilogScanner src/parser/verilog/verilog_scanner.ll ${FLEX_BISON_VERILOG_DIR}/verilog_scanner.cpp
             COMPILE_FLAGS "-P Verilog")
bison_target (Veriparse_VerilogParser src/parser/verilog/verilog_parser.yy  ${FLEX_BISON_VERILOG_DIR}/verilog_parser.cpp
              COMPILE_FLAGS "-v")
add_flex_bison_dependency (Veriparse_VerilogScanner Veriparse_VerilogParser)

# Verilog Number parser
set(FLEX_BISON_VERILOG_NUMBER_DIR ${CMAKE_CURRENT_BINARY_DIR}/verilog_number)
file(MAKE_DIRECTORY ${FLEX_BISON_VERILOG_NUMBER_DIR})
flex_target (Veriparse_VerilogNumberScanner
             src/parser/verilog_number/verilog_number_scanner.ll
             ${FLEX_BISON_VERILOG_NUMBER_DIR}/verilog_number_scanner.cpp
			 COMPILE_FLAGS "-P VerilogNumber")
bison_target (Veriparse_VerilogNumberParser
			  src/parser/verilog_number/verilog_number_parser.yy
              ${FLEX_BISON_VERILOG_NUMBER_DIR}/verilog_number_parser.cpp
              COMPILE_FLAGS "-v")
add_flex_bison_dependency (Veriparse_VerilogNumberScanner Veriparse_VerilogNumberParser)


# Build library
file(GLOB_RECURSE source_files src/*.cpp)

add_library (veriparse_static STATIC
  ${source_files} ${veriparse_version_file}
  ${BISON_Veriparse_VerilogParser_OUTPUTS}
  ${FLEX_Veriparse_VerilogScanner_OUTPUTS}
  ${BISON_Veriparse_VerilogNumberParser_OUTPUTS}
  ${FLEX_Veriparse_VerilogNumberScanner_OUTPUTS})

if (NOT MINGW)
  if (CMAKE_BUILD_TYPE STREQUAL "Release")
	set_property(TARGET veriparse_static PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
  endif()
endif()

target_include_directories (veriparse_static PUBLIC include)
target_include_directories (veriparse_static PUBLIC ${YAMLCPP_INCLUDE_DIR})
target_include_directories (veriparse_static PUBLIC ${BOOST_INCLUDE_DIRS})
target_include_directories (veriparse_static PUBLIC ${GMP_INCLUDE_DIR})
target_include_directories (veriparse_static PUBLIC ${GMPXX_INCLUDE_DIR})
target_include_directories (veriparse_static PRIVATE src)
target_include_directories (veriparse_static PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(veriparse_static ${YAMLCPP_LIBRARY})
target_link_libraries(veriparse_static Boost::system Boost::filesystem Boost::log)
target_link_libraries(veriparse_static ${GMPXX_LIBRARY} ${GMP_LIBRARY})

# Specific compilation flags
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/verilog/verilog_scanner.cpp PROPERTIES COMPILE_FLAGS -Wno-sign-compare)


########################################
### Test generation
########################################

add_subdirectory(test)
