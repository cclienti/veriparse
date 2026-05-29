// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti

// End-to-end test: feed preprocessor output through Parser::Verilog and
// check the resulting AST. Sanity-checks the contract between the new
// preprocessor and the existing main parser on a realistic multi-file
// fixture with macros + ifdef + function-like expansion.

#include <gtest/gtest.h>

#include <veriparse/AST/description.hpp>
#include <veriparse/AST/module.hpp>
#include <veriparse/AST/source.hpp>
#include <veriparse/parser/preprocessor.hpp>
#include <veriparse/parser/verilog.hpp>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

namespace fs = std::filesystem;

namespace
{

class PreprocessorParserFixture : public ::testing::Test
{
protected:
    void TearDown() override
    {
        std::error_code ec;
        fs::remove_all(m_dir, ec);
    }

    void mkdir_unique()
    {
        const ::testing::TestInfo *info = ::testing::UnitTest::GetInstance()->current_test_info();
        const std::string suffix =
            std::string(info ? info->name() : "anon") + "_" + std::to_string(::getpid());
        m_dir = fs::temp_directory_path() / ("veriparse_ppparse_" + suffix);
        fs::create_directories(m_dir);
    }

    void write_file(const std::string &name, const std::string &content)
    {
        std::ofstream f(m_dir / name);
        f << content;
    }

    // Find the first Module in the parsed Source, or nullptr.
    static Veriparse::AST::Module::Ptr first_module(const Veriparse::AST::Node::Ptr &root)
    {
        auto source = std::dynamic_pointer_cast<Veriparse::AST::Source>(root);
        if(!source) {
            return nullptr;
        }
        auto desc = source->get_description();
        if(!desc) {
            return nullptr;
        }
        auto defs = desc->get_definitions();
        if(!defs) {
            return nullptr;
        }
        for(const auto &n : *defs) {
            if(auto m = std::dynamic_pointer_cast<Veriparse::AST::Module>(n)) {
                return m;
            }
        }
        return nullptr;
    }

    fs::path m_dir;
};

// 1. `include of a header with `define, macro used in module port decl.
//    Validates: include resolution + object-like substitution + parser
//    accepts the preprocessed result.
TEST_F(PreprocessorParserFixture, IncludeWithObjectMacroIsParseable)
{
    mkdir_unique();
    write_file("widths.vh", "`define DATA_W 8\n"
                            "`define ADDR_W 4\n");
    write_file("top.v", "`include \"widths.vh\"\n"
                        "module top(\n"
                        "  input  wire clk,\n"
                        "  input  wire [`DATA_W-1:0] d,\n"
                        "  output reg  [`DATA_W-1:0] q\n"
                        ");\n"
                        "  always @(posedge clk) q <= d;\n"
                        "endmodule\n");

    Veriparse::Parser::Preprocessor pp;
    pp.add_include_dir(m_dir.string());
    std::ostringstream pp_out;
    ASSERT_EQ(pp.preprocess((m_dir / "top.v").string(), pp_out), 0);

    // The DATA_W use sites must have been substituted in the stream we
    // hand to the parser — otherwise the parser would see a backtick.
    EXPECT_EQ(pp_out.str().find("`DATA_W"), std::string::npos);
    EXPECT_NE(pp_out.str().find("[8-1:0]"), std::string::npos);

    std::istringstream parse_in(pp_out.str());
    Veriparse::Parser::Verilog parser;
    ASSERT_EQ(parser.parse(parse_in), 0);
    auto root = parser.get_source();
    ASSERT_NE(root, nullptr);
    auto mod = first_module(root);
    ASSERT_NE(mod, nullptr);
    EXPECT_EQ(mod->get_name(), "top");
}

// 2. `ifdef branch selection driven by a CLI predefine. The two branches
//    declare modules with different names; predefining MODE_B must pick
//    the second one.
TEST_F(PreprocessorParserFixture, CliPredefineFlipsIfdefBranch)
{
    mkdir_unique();
    write_file("dut.v", "`ifdef MODE_B\n"
                        "module variant_b(input wire i, output wire o); assign o = ~i; endmodule\n"
                        "`else\n"
                        "module variant_a(input wire i, output wire o); assign o =  i; endmodule\n"
                        "`endif\n");

    // Without predefine -> variant_a
    {
        Veriparse::Parser::Preprocessor pp;
        std::ostringstream pp_out;
        ASSERT_EQ(pp.preprocess((m_dir / "dut.v").string(), pp_out), 0);
        std::istringstream parse_in(pp_out.str());
        Veriparse::Parser::Verilog parser;
        ASSERT_EQ(parser.parse(parse_in), 0);
        auto mod = first_module(parser.get_source());
        ASSERT_NE(mod, nullptr);
        EXPECT_EQ(mod->get_name(), "variant_a");
    }

    // With -DMODE_B -> variant_b
    {
        Veriparse::Parser::Preprocessor pp;
        pp.define("MODE_B");
        std::ostringstream pp_out;
        ASSERT_EQ(pp.preprocess((m_dir / "dut.v").string(), pp_out), 0);
        std::istringstream parse_in(pp_out.str());
        Veriparse::Parser::Verilog parser;
        ASSERT_EQ(parser.parse(parse_in), 0);
        auto mod = first_module(parser.get_source());
        ASSERT_NE(mod, nullptr);
        EXPECT_EQ(mod->get_name(), "variant_b");
    }
}

// 3. Function-like macro expansion inside a continuous assign. Validates
//    bracket-aware argument parsing + substitution + parser acceptance.
TEST_F(PreprocessorParserFixture, FunctionLikeMacroExpansionParses)
{
    mkdir_unique();
    write_file("math.vh", "`define MAX(a, b) ((a) > (b) ? (a) : (b))\n");
    write_file("top.v", "`include \"math.vh\"\n"
                        "module top(input wire [7:0] x, input wire [7:0] y, output wire [7:0] z);\n"
                        "  assign z = `MAX(x, y);\n"
                        "endmodule\n");

    Veriparse::Parser::Preprocessor pp;
    pp.add_include_dir(m_dir.string());
    std::ostringstream pp_out;
    ASSERT_EQ(pp.preprocess((m_dir / "top.v").string(), pp_out), 0);

    // The expanded body should appear; the bare macro reference should not.
    EXPECT_EQ(pp_out.str().find("`MAX"), std::string::npos);
    EXPECT_NE(pp_out.str().find("((x) > (y) ? (x) : (y))"), std::string::npos);

    std::istringstream parse_in(pp_out.str());
    Veriparse::Parser::Verilog parser;
    ASSERT_EQ(parser.parse(parse_in), 0);
    auto mod = first_module(parser.get_source());
    ASSERT_NE(mod, nullptr);
    EXPECT_EQ(mod->get_name(), "top");
}

// 4. Nested include: outer.vh `include "inner.vh", and the symbol the
//    inner header defines is referenced from the top file.
TEST_F(PreprocessorParserFixture, NestedIncludeChain)
{
    mkdir_unique();
    write_file("inner.vh", "`define W 16\n");
    write_file("outer.vh", "`include \"inner.vh\"\n");
    write_file("top.v", "`include \"outer.vh\"\n"
                        "module top(input wire [`W-1:0] d, output wire [`W-1:0] q);\n"
                        "  assign q = d;\n"
                        "endmodule\n");

    Veriparse::Parser::Preprocessor pp;
    pp.add_include_dir(m_dir.string());
    std::ostringstream pp_out;
    ASSERT_EQ(pp.preprocess((m_dir / "top.v").string(), pp_out), 0);

    EXPECT_NE(pp_out.str().find("[16-1:0]"), std::string::npos);

    std::istringstream parse_in(pp_out.str());
    Veriparse::Parser::Verilog parser;
    ASSERT_EQ(parser.parse(parse_in), 0);
    auto mod = first_module(parser.get_source());
    ASSERT_NE(mod, nullptr);
    EXPECT_EQ(mod->get_name(), "top");
}

} // namespace
