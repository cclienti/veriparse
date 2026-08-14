// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti

// Parse-rejection tests. The parser's fatal-error path (VerilogParser::error)
// reports and calls exit(1), so a rejected input kills the process — these
// tests pin the rejections with gtest death tests: the parse runs in a child
// process and both the exit code and the diagnostic text are asserted.
// Accepted-input companions run in-process and assert a clean parse.

#include <veriparse/parser/verilog.hpp>
#include <veriparse/logger/logger.hpp>

#include <gtest/gtest.h>

#include <fstream>
#include <string>

using namespace Veriparse;

namespace
{

// Parse from a real file: the parser's diagnostic formatter re-opens the
// source by name to quote the offending lines, so a stream parse would
// lose the message the death tests match on.
int parse_mode(bool sv_mode, const std::string &text)
{
    Logger::remove_all_sinks();
    Logger::add_stderr_sink();
    const ::testing::TestInfo *const info = ::testing::UnitTest::GetInstance()->current_test_info();
    const std::string filename = std::string(info->name()) + "_rejection.sv";
    std::ofstream(filename) << text;
    Parser::Verilog verilog;
    verilog.set_sv_mode(sv_mode);
    const int ret = verilog.parse(filename);
    if(ret == 0 && !verilog.get_source()) {
        return 1;
    }
    return ret;
}

int parse_sv(const std::string &text) { return parse_mode(true, text); }

int parse_verilog(const std::string &text) { return parse_mode(false, text); }

#define EXPECT_PARSE_FATAL(call, message_re)                                                       \
    do {                                                                                           \
        ::testing::FLAGS_gtest_death_test_style = "threadsafe";                                    \
        EXPECT_EXIT(call, ::testing::ExitedWithCode(1), message_re);                               \
    } while(0)

// §11.4.1 evaluates a left-hand index expression once; the ADR-0013
// desugaring clones the lvalue, so an index containing a subroutine or
// system call would be evaluated twice — rejected rather than mis-lowered.
TEST(VerilogParserRejection, sv_op_assign_lvalue_function_call)
{
    const std::string src = "module m;\n"
                            "  integer mem [0:3];\n"
                            "  function integer nid(input integer x);\n"
                            "    nid = x + 1;\n"
                            "  endfunction\n"
                            "  always @* mem[nid(0)] += 1;\n"
                            "endmodule\n";
    EXPECT_PARSE_FATAL(parse_sv(src), "evaluated twice");
}

TEST(VerilogParserRejection, sv_incdec_lvalue_function_call)
{
    const std::string src = "module m;\n"
                            "  integer mem [0:3];\n"
                            "  function integer nid(input integer x);\n"
                            "    nid = x + 1;\n"
                            "  endfunction\n"
                            "  always @* mem[nid(0)]++;\n"
                            "endmodule\n";
    EXPECT_PARSE_FATAL(parse_sv(src), "evaluated twice");
}

TEST(VerilogParserRejection, sv_op_assign_lvalue_system_call)
{
    const std::string src = "module m;\n"
                            "  integer mem [0:3];\n"
                            "  integer i;\n"
                            "  always @* mem[$signed(i)] += 1;\n"
                            "endmodule\n";
    EXPECT_PARSE_FATAL(parse_sv(src), "evaluated twice");
}

// A plain assignment with the same lvalue stays accepted — only the
// desugared forms clone the lvalue.
TEST(VerilogParserRejection, sv_plain_assign_lvalue_call_accepted)
{
    const std::string src = "module m;\n"
                            "  integer mem [0:3];\n"
                            "  function integer nid(input integer x);\n"
                            "    nid = x + 1;\n"
                            "  endfunction\n"
                            "  always @* mem[nid(0)] = mem[nid(0)] + 1;\n"
                            "endmodule\n";
    EXPECT_EQ(0, parse_sv(src));
}

// ADR-0013 §2 maximal munch: in SV mode `a--b` lexes as `a -- b` and is a
// syntax error (write `a - -b`), like every SV tool; Verilog mode keeps the
// 1364 tokenization.
TEST(VerilogParserRejection, sv_adjacent_decr_in_expression)
{
    const std::string src = "module m(input [3:0] a, b, output [3:0] y);\n"
                            "  assign y = a--b;\n"
                            "endmodule\n";
    EXPECT_PARSE_FATAL(parse_sv(src), "syntax error");
}

TEST(VerilogParserRejection, verilog_adjacent_incr_decr_accepted)
{
    const std::string src = "module m(a, b, y, z);\n"
                            "  input [3:0] a, b;\n"
                            "  output [3:0] y, z;\n"
                            "  assign y = a++b;\n"
                            "  assign z = a--b;\n"
                            "endmodule\n";
    EXPECT_EQ(0, parse_verilog(src));
}

// A tf declaration list shares one direction and one type across its names
// (IEEE 1800-2017 §13.3 / 1364-2005 §10.2): a subsequent name carrying its
// own data type is illegal and rejected.
TEST(VerilogParserRejection, tf_list_typed_directionless_name)
{
    const std::string src = "module m;\n"
                            "  task t;\n"
                            "    input reg [31:0] v, reg [15:0] a;\n"
                            "    begin\n"
                            "    end\n"
                            "  endtask\n"
                            "endmodule\n";
    EXPECT_PARSE_FATAL(parse_sv(src), "direction");
    EXPECT_PARSE_FATAL(parse_verilog(src), "direction");
}

// The var port kind (§23.2.2.3): a leading directionless 'var' port takes
// the default inout direction, and "an inout port shall not be [of a
// variable type]" (A.1.3 note 1) — the standard's own mh4 error case.
TEST(VerilogParserRejection, sv_var_port_leading_directionless)
{
    const std::string src = "module m(var x);\n"
                            "endmodule\n";
    EXPECT_PARSE_FATAL(parse_sv(src), "inout");
}

TEST(VerilogParserRejection, sv_var_port_inout)
{
    const std::string src = "module m(inout var logic x);\n"
                            "endmodule\n";
    EXPECT_PARSE_FATAL(parse_sv(src), "variable type");
}

// 'var' names a variable kind; a net type on the same port contradicts it.
TEST(VerilogParserRejection, sv_var_port_net_kind)
{
    const std::string src = "module m(input var wire x);\n"
                            "endmodule\n";
    EXPECT_PARSE_FATAL(parse_sv(src), "net type");
}

// The legal shared-list form stays accepted, the bare names inheriting the
// declaration's type (§13.3 — the width fix pinned by the parser goldens).
TEST(VerilogParserRejection, tf_list_shared_type_accepted)
{
    const std::string src = "module m;\n"
                            "  task t;\n"
                            "    input [31:0] v, a;\n"
                            "    begin\n"
                            "    end\n"
                            "  endtask\n"
                            "endmodule\n";
    EXPECT_EQ(0, parse_sv(src));
    EXPECT_EQ(0, parse_verilog(src));
}

} // namespace
