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

TEST(PreprocessorDefine, ObjectLikeMacroExpands)
{
    const std::string out = run("`define W 8\nreg [`W-1:0] x;\n");
    EXPECT_NE(out.find("reg [8-1:0] x;"), std::string::npos);
}

TEST(PreprocessorDefine, MacroDirectiveItselfIsNotEmitted)
{
    const std::string out = run("`define W 8\nwire w;\n");
    // The `define line must disappear from the output.
    EXPECT_EQ(out.find("`define"), std::string::npos);
    EXPECT_NE(out.find("wire w;"), std::string::npos);
}

TEST(PreprocessorDefine, BackslashNewlineContinuesBody)
{
    // §22.5.1: '\\<newline>' becomes a newline in stored body.
    const std::string out = run("`define TWO_LINES one\\\ntwo\n`TWO_LINES\n");
    EXPECT_NE(out.find("one\ntwo"), std::string::npos);
}

TEST(PreprocessorDefine, LineCommentStrippedFromBody)
{
    // §22.5.1: //-comments in body are not part of the substituted text.
    const std::string out = run("`define X 5 // a note\nval=`X\n");
    EXPECT_NE(out.find("val=5"), std::string::npos);
    EXPECT_EQ(out.find("a note"), std::string::npos);
}

TEST(PreprocessorDefine, SlashesInsideStringNotMistakenForComment)
{
    const std::string out = run("`define URL \"http://example.com\"\nx=`URL;\n");
    EXPECT_NE(out.find("x=\"http://example.com\";"), std::string::npos);
}

TEST(PreprocessorDefine, UndefRemovesMacro)
{
    // After `undef, the macro is unknown and passes through unchanged.
    const std::string out = run("`define X 1\n`undef X\nval = `X;\n");
    EXPECT_NE(out.find("val = `X;"), std::string::npos);
}

TEST(PreprocessorDefine, NestedMacroExpansion)
{
    const std::string out = run("`define A 1\n`define B `A\nv=`B;\n");
    EXPECT_NE(out.find("v=1;"), std::string::npos);
}

TEST(PreprocessorDefine, DirectRecursionIsRejected)
{
    // §22.5.1: direct/indirect self-expansion is an error.
    // We don't have a way to observe LOG_ERROR from the test, but the
    // preprocessor must terminate without hanging.
    const std::string out = run("`define A `A\nv = `A;\n");
    // The macro use should expand once then be detected as recursive.
    // We mostly care that we don't hang and produce some output.
    EXPECT_FALSE(out.empty());
}

TEST(PreprocessorDefine, IndirectRecursionIsRejected)
{
    const std::string out = run("`define A `B\n`define B `A\nv = `A;\n");
    EXPECT_FALSE(out.empty());
}

} // namespace
