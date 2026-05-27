// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti

#include <gtest/gtest.h>
#include <veriparse/parser/preprocessor.hpp>

#include <sstream>
#include <string>

namespace
{

std::string preprocess_inline(const std::string &input, const std::string &name = "<inline>")
{
    Veriparse::Parser::Preprocessor pp;
    std::istringstream in(input);
    std::ostringstream out;
    EXPECT_EQ(pp.preprocess(in, name, out), 0);
    return out.str();
}

TEST(PreprocessorSkeleton, EmitsInitialLineMarker)
{
    const std::string out = preprocess_inline("module foo; endmodule\n", "foo.v");
    EXPECT_EQ(out.find("`line 1 \"foo.v\" 0\n"), 0u);
}

TEST(PreprocessorSkeleton, PassesDirectiveFreeInputThrough)
{
    const std::string src = "module foo (input wire a, output wire b);\n"
                            "  assign b = ~a;\n"
                            "endmodule\n";
    const std::string out = preprocess_inline(src);
    EXPECT_NE(out.find(src), std::string::npos);
}

TEST(PreprocessorSkeleton, BacktickInsideStringIsNotADirective)
{
    const std::string src = "initial $display(\"`define foo bar\");\n";
    const std::string out = preprocess_inline(src);
    // String content is passed through verbatim and is not recognised as a
    // directive — §22.5.1: "Macro substitution and argument substitution
    // shall not occur within string literals."
    EXPECT_NE(out.find("\"`define foo bar\""), std::string::npos);
}

TEST(PreprocessorSkeleton, BacktickInsideBlockCommentIsNotADirective)
{
    const std::string src = "/* `define foo bar */\nwire w;\n";
    const std::string out = preprocess_inline(src);
    EXPECT_NE(out.find("/* `define foo bar */"), std::string::npos);
    EXPECT_NE(out.find("wire w;"), std::string::npos);
}

TEST(PreprocessorSkeleton, BacktickInsideLineCommentIsNotADirective)
{
    const std::string src = "// `define foo bar\nwire w;\n";
    const std::string out = preprocess_inline(src);
    EXPECT_NE(out.find("// `define foo bar"), std::string::npos);
    EXPECT_NE(out.find("wire w;"), std::string::npos);
}

} // namespace
