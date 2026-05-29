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

// Input shaped like a preprocessor's output (iverilog -E style): only
// `line markers and plain Verilog, no `define / `include / `ifdef.
// veripp must pass it through unchanged so a pipeline that already
// preprocessed its source upstream stays valid.
TEST(PreprocessorSkeleton, IverilogPreprocessedStylePassesThrough)
{
    const std::string src = "`line 1 \"foo.v\" 0\n"
                            "module foo;\n"
                            "endmodule\n"
                            "`line 1 \"bar.v\" 1\n"
                            "module bar;\n"
                            "endmodule\n"
                            "`line 5 \"foo.v\" 2\n";
    const std::string out = preprocess_inline(src, "wrapper.v");
    EXPECT_NE(out.find("`line 1 \"foo.v\" 0"), std::string::npos);
    EXPECT_NE(out.find("`line 1 \"bar.v\" 1"), std::string::npos);
    EXPECT_NE(out.find("`line 5 \"foo.v\" 2"), std::string::npos);
    EXPECT_NE(out.find("module foo;"), std::string::npos);
    EXPECT_NE(out.find("module bar;"), std::string::npos);
}

// Re-running the preprocessor on its own output must not consume or
// re-resolve anything: by the time we reach pass 2, every `define and
// macro reference is gone, and the substituted Verilog is intact.
TEST(PreprocessorSkeleton, OutputIsStableUnderASecondPass)
{
    const std::string src = "`define W 8\n"
                            "wire [`W-1:0] x;\n";
    const std::string pass1 = preprocess_inline(src, "f.v");
    const std::string pass2 = preprocess_inline(pass1, "f.v");

    EXPECT_EQ(pass1.find("`define"), std::string::npos);
    EXPECT_EQ(pass2.find("`define"), std::string::npos);
    EXPECT_EQ(pass1.find("`W"), std::string::npos);
    EXPECT_EQ(pass2.find("`W"), std::string::npos);
    EXPECT_NE(pass1.find("wire [8-1:0] x;"), std::string::npos);
    EXPECT_NE(pass2.find("wire [8-1:0] x;"), std::string::npos);
}

} // namespace
