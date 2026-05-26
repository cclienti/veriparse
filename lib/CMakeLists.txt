########################################
### Global options
########################################

cmake_minimum_required (VERSION 3.15...3.27)
project (veriparse_lib)
include (${CMAKE_CURRENT_LIST_DIR}/../cmake/common.cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

########################################
### Manage Version in the Library
########################################

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/version.cpp.in
               ${CMAKE_CURRENT_BINARY_DIR}/version.cpp)

set(veriparse_version_file "${CMAKE_CURRENT_BINARY_DIR}/version.cpp")

########################################
### Veriparse Library
########################################

# conda-forge's winflexbison ships the binaries as win_bison.exe / win_flex.exe.
# Hint find_package to those names before falling back to the standard ones.
if(WIN32)
  find_program(BISON_EXECUTABLE NAMES win_bison bison)
  find_program(FLEX_EXECUTABLE  NAMES win_flex  flex)
endif()
find_package(BISON)
find_package(FLEX)

# winflexbison ships FlexLexer.h outside the default include path. The
# location varies by distribution: chocolatey installs it under
# `lib/winflexbison3/tools/` (the bin entry is a shim that doesn't sit
# next to the header), conda-forge puts it under
# `Library/share/winflexbison/`. Use find_path with explicit hints for
# both layouts.
add_library(veriparse_flex_lexer_h INTERFACE)
if(WIN32 AND FLEX_EXECUTABLE)
  get_filename_component(_flex_bin_dir ${FLEX_EXECUTABLE} DIRECTORY)
  get_filename_component(_flex_prefix  ${_flex_bin_dir}   DIRECTORY)
  find_path(FLEX_INCLUDE_DIR
    NAMES FlexLexer.h
    HINTS
      "$ENV{ProgramData}/chocolatey/lib/winflexbison3/tools"
      "$ENV{ChocolateyInstall}/lib/winflexbison3/tools"
      "C:/ProgramData/chocolatey/lib/winflexbison3/tools"
      ${_flex_bin_dir}
      ${_flex_prefix}/share/winflexbison
      ${_flex_prefix}/tools
  )
  if(FLEX_INCLUDE_DIR)
    target_include_directories(veriparse_flex_lexer_h INTERFACE ${FLEX_INCLUDE_DIR})
  else()
    message(FATAL_ERROR "FlexLexer.h not found; checked chocolatey and conda-forge layouts.")
  endif()
endif()

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


target_include_directories (veriparse_static PUBLIC include)
target_include_directories (veriparse_static SYSTEM PUBLIC ${BOOST_INCLUDE_DIRS})
target_include_directories (veriparse_static PRIVATE src)
target_include_directories (veriparse_static PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(veriparse_static YAMLCPP::YAMLCPP)
target_link_libraries(veriparse_static Boost::system Boost::filesystem Boost::log)
target_link_libraries(veriparse_static GMP::GMPXX GMP::GMP)
target_link_libraries(veriparse_static veriparse_flex_lexer_h)

# Specific compilation flags (GCC/Clang only; MSVC rejects -W... flags)
if(NOT MSVC)
  set(_parser_flags "-Wno-unused-but-set-variable")
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # Bison generates a parser class with an unused private `driver` field; clang
    # flags it under -Wunused-private-field, which -Wall (re-)enables after our
    # global CXX_FLAGS. Source-level COMPILE_FLAGS land after -Wall, so suppress
    # it here.
    string(APPEND _parser_flags " -Wno-unused-private-field")
  endif()

  set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/verilog/verilog_scanner.cpp PROPERTIES COMPILE_FLAGS -Wno-sign-compare)
  set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/verilog/verilog_parser.cpp PROPERTIES COMPILE_FLAGS "${_parser_flags}")
  set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/verilog_number/verilog_number_parser.cpp PROPERTIES COMPILE_FLAGS "${_parser_flags}")
endif()


########################################
### Test generation
########################################

add_subdirectory(test)
