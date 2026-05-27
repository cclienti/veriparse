// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti

#include <gtest/gtest.h>
#include <veriparse/parser/preprocessor.hpp>

#include <sstream>
#include <string>

namespace
{

std::string run(const std::string &input)
{
    Veriparse::Parser::Preprocessor pp;
    std::istringstream in(input);
    std::ostringstream out;
    EXPECT_EQ(pp.preprocess(in, "<inline>", out), 0);
    return out.str();
}

TEST(PreprocessorConditional, IfdefTakenBranchEmits)
{
    const std::string out =
        run("`define FOO\n`ifdef FOO\nIF_BRANCH;\n`else\nELSE_BRANCH;\n`endif\n");
    EXPECT_NE(out.find("IF_BRANCH;"), std::string::npos);
    EXPECT_EQ(out.find("ELSE_BRANCH;"), std::string::npos);
}

TEST(PreprocessorConditional, IfdefElseBranchEmits)
{
    const std::string out = run("`ifdef FOO\nIF_BRANCH;\n`else\nELSE_BRANCH;\n`endif\n");
    EXPECT_EQ(out.find("IF_BRANCH;"), std::string::npos);
    EXPECT_NE(out.find("ELSE_BRANCH;"), std::string::npos);
}

TEST(PreprocessorConditional, IfndefInverts)
{
    const std::string out = run("`ifndef FOO\nempty_def;\n`endif\n");
    EXPECT_NE(out.find("empty_def;"), std::string::npos);
}

TEST(PreprocessorConditional, ElsifSelectsCorrectBranch)
{
    const std::string in = "`define B\n"
                           "`ifdef A\n"
                           "  branch_a;\n"
                           "`elsif B\n"
                           "  branch_b;\n"
                           "`elsif C\n"
                           "  branch_c;\n"
                           "`else\n"
                           "  branch_else;\n"
                           "`endif\n";
    const std::string out = run(in);
    EXPECT_EQ(out.find("branch_a;"), std::string::npos);
    EXPECT_NE(out.find("branch_b;"), std::string::npos);
    EXPECT_EQ(out.find("branch_c;"), std::string::npos);
    EXPECT_EQ(out.find("branch_else;"), std::string::npos);
}

TEST(PreprocessorConditional, NestedIfdefInactiveOuter)
{
    // §22.6: a nested ifdef inside a skipped outer is itself fully
    // skipped regardless of its macro check.
    const std::string in = "`define INNER\n"
                           "`ifdef OUTER\n"
                           "  `ifdef INNER\n"
                           "    inner_in_outer;\n"
                           "  `endif\n"
                           "`endif\n"
                           "after_block;\n";
    const std::string out = run(in);
    EXPECT_EQ(out.find("inner_in_outer;"), std::string::npos);
    EXPECT_NE(out.find("after_block;"), std::string::npos);
}

TEST(PreprocessorConditional, DefineInsideSkippedBlockHasNoEffect)
{
    // §22.6: directives inside an inactive branch must not take effect.
    const std::string out = run("`ifdef NEVER\n"
                                "`define X 42\n"
                                "`endif\n"
                                "value=`X;\n");
    // X must remain undefined → passes through verbatim.
    EXPECT_NE(out.find("value=`X;"), std::string::npos);
}

TEST(PreprocessorConditional, BacktickInsideStringNotADirectiveInSkip)
{
    // In a skipped region, strings are lex-validated so a `endif inside
    // a string is not taken as a directive.
    const std::string in = "`ifdef NEVER\n"
                           "  $display(\"`endif\");\n"
                           "`endif\n"
                           "after;\n";
    const std::string out = run(in);
    EXPECT_NE(out.find("after;"), std::string::npos);
}

} // namespace
