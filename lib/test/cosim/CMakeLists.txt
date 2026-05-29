# Header-only helper library for cosim tests. The verilator include path
# is added by `verilate()` on the consumer side; here we only expose our
# own header directory.
add_library(veriparse_cosim_helpers INTERFACE)
target_include_directories(veriparse_cosim_helpers INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
