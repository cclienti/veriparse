// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti

#include <gtest/gtest.h>
#include <veriparse/parser/preprocessor.hpp>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

namespace fs = std::filesystem;

namespace
{

std::string run(const std::string &input, bool sv_mode = true)
{
    Veriparse::Parser::Preprocessor pp;
    pp.set_sv_mode(sv_mode);
    std::istringstream in(input);
    std::ostringstream out;
    EXPECT_EQ(pp.preprocess(in, "<inline>", out), 0);
    return out.str();
}

// ---------- `__FILE__ / `__LINE__ (§22.13) ---------------------------

TEST(PreprocessorSvMacro, FileMacroExpandsToQuotedFilename)
{
    Veriparse::Parser::Preprocessor pp;
    pp.set_sv_mode(true);
    std::istringstream in("here = `__FILE__;\n");
    std::ostringstream out;
    EXPECT_EQ(pp.preprocess(in, "foo.sv", out), 0);
    EXPECT_NE(out.str().find("here = \"foo.sv\";"), std::string::npos);
}

TEST(PreprocessorSvMacro, LineMacroExpandsToDecimal)
{
    const std::string out = run("first;\nline = `__LINE__;\n");
    EXPECT_NE(out.find("line = 2;"), std::string::npos);
}

TEST(PreprocessorSvMacro, FileMacroRequiresSvMode)
{
    // Outside SV mode, `__FILE__ still expands but logs a warning.
    Veriparse::Parser::Preprocessor pp;
    pp.set_sv_mode(false);
    std::istringstream in("x = `__FILE__;\n");
    std::ostringstream out;
    EXPECT_EQ(pp.preprocess(in, "foo.v", out), 0);
    EXPECT_NE(out.str().find("\"foo.v\""), std::string::npos);
}

// ---------- `" / `\`" / `` body escapes (§22.5.1) --------------------

TEST(PreprocessorSvMacro, SyntheticStringStringifiesArgument)
{
    // `define STR(x) `"x`" — wraps the actual in quotes, with formal
    // substitution active inside the synthetic string.
    const std::string out = run("`define STR(x) `\"x`\"\n"
                                "msg = `STR(hello);\n");
    EXPECT_NE(out.find("msg = \"hello\";"), std::string::npos);
}

TEST(PreprocessorSvMacro, EscapedQuoteInsideSyntheticString)
{
    // Spec example: `define msg(x,y) `"x: `\`"y`\`"`"
    const std::string out = run("`define msg(x,y) `\"x: `\\`\"y`\\`\"`\"\n"
                                "$display(`msg(left side,right side));\n");
    EXPECT_NE(out.find("$display(\"left side: \\\"right side\\\"\");"), std::string::npos);
}

TEST(PreprocessorSvMacro, NestedMacroExpandsInsideSyntheticString)
{
    // §22.5.1: `IDENT inside `"..."` is a nested macro reference and
    // must be re-expanded after formal substitution.
    const std::string out = run("`define VERSION 42\n"
                                "`define LOG(msg) $display(`\"[v`VERSION] msg`\")\n"
                                "`LOG(hello)\n");
    EXPECT_NE(out.find("$display(\"[v42] hello\")"), std::string::npos);
}

TEST(PreprocessorSvMacro, NestedMacroChainsInsideSyntheticString)
{
    // Two-deep chain: `A → `B → "deep".
    const std::string out = run("`define A `B\n"
                                "`define B deep\n"
                                "`define S `\"x=`A`\"\n"
                                "v = `S;\n");
    EXPECT_NE(out.find("v = \"x=deep\";"), std::string::npos);
}

TEST(PreprocessorSvMacro, UnknownNestedRefInSyntheticPassesThroughLiterally)
{
    // Lookup miss → keep the textual `IDENT, do not crash.
    const std::string out = run("`define S `\"hello `UNDEFINED world`\"\n"
                                "v = `S;\n");
    EXPECT_NE(out.find("v = \"hello `UNDEFINED world\";"), std::string::npos);
}

TEST(PreprocessorSvMacro, FormalAndNestedRefShareSyntheticString)
{
    // `LOG(boot) inside `"v`VERSION: msg`" must do both jobs at once:
    // substitute formal msg → boot and re-expand `VERSION → 42.
    const std::string out = run("`define VERSION 42\n"
                                "`define LOG(msg) `\"v`VERSION: msg`\"\n"
                                "$display(`LOG(boot));\n");
    EXPECT_NE(out.find("$display(\"v42: boot\")"), std::string::npos);
}

TEST(PreprocessorSvMacro, RecursiveNestedRefInSyntheticIsDepthCapped)
{
    // A defined in terms of itself via a synthetic string. Without the
    // depth cap the textual recursion (in expand_body) and the Flex
    // re-scan (push_buffer) would each loop forever. We only assert
    // that the run terminates: preprocess returns 0 and the output is
    // bounded.
    Veriparse::Parser::Preprocessor pp;
    pp.set_sv_mode(true);
    std::istringstream in("`define A `\"`A`\"\n"
                          "v = `A;\n");
    std::ostringstream out;
    EXPECT_EQ(pp.preprocess(in, "<inline>", out), 0);
    EXPECT_LT(out.str().size(), size_t{64 * 1024});
}

TEST(PreprocessorSvMacro, EmptySyntheticStringProducesEmptyString)
{
    // `"`" — synthetic open immediately followed by synthetic close
    // — yields a zero-length string literal.
    const std::string out = run("`define E `\"`\"\n"
                                "v = `E;\n");
    EXPECT_NE(out.find("v = \"\";"), std::string::npos);
}

// §22.5.1: function-like macro reference inside a synthetic string
// must be expanded (matches iverilog -E on the same input).
TEST(PreprocessorSvMacro, FunctionLikeNestedRefInSyntheticIsExpanded)
{
    const std::string out = run("`define name(x) x suffix\n"
                                "`define S `\"prefix_`name(arg)_end`\"\n"
                                "v = `S;\n");
    EXPECT_NE(out.find("v = \"prefix_arg suffix_end\";"), std::string::npos);
}

// Bracket-aware textual arg collection: nested parens in the actual
// must not be split as separators.
TEST(PreprocessorSvMacro, FunctionLikeInSyntheticActualWithMatchedParens)
{
    const std::string out = run("`define wrap(x) [x]\n"
                                "`define S `\"`wrap(foo(a,b))`\"\n"
                                "v = `S;\n");
    EXPECT_NE(out.find("v = \"[foo(a,b)]\";"), std::string::npos);
}

// Bracket-aware textual arg collection: a string literal in the actual
// must hide the commas it contains. The asserted output has unescaped
// inner quotes inside a synthetic string — that's not valid Verilog
// downstream, but it matches iverilog -E byte-for-byte: §22.5.1 leaves
// quote-escaping inside synthetic strings to the user (garbage-in /
// garbage-out at the preprocessor level).
TEST(PreprocessorSvMacro, FunctionLikeInSyntheticActualWithStringContainingCommas)
{
    const std::string out = run("`define wrap(x) [x]\n"
                                "`define S `\"`wrap(\"a,b,c\")`\"\n"
                                "v = `S;\n");
    EXPECT_NE(out.find("v = \"[\"a,b,c\"]\";"), std::string::npos);
}

// Multiple actuals are routed through their formals as expected.
TEST(PreprocessorSvMacro, FunctionLikeInSyntheticMultipleActuals)
{
    const std::string out = run("`define pair(a, b) a / b\n"
                                "`define S `\"left=`pair(x, y)=right`\"\n"
                                "v = `S;\n");
    EXPECT_NE(out.find("v = \"left=x / y=right\";"), std::string::npos);
}

// SV default formal value used when the textual actual is empty.
TEST(PreprocessorSvMacro, FunctionLikeInSyntheticDefaultArg)
{
    const std::string out = run("`define name(x=fallback) x\n"
                                "`define S `\"hi-`name()`\"\n"
                                "v = `S;\n");
    EXPECT_NE(out.find("v = \"hi-fallback\";"), std::string::npos);
}

// Error recovery: too many actuals — we drop the argument list,
// emit the bare `IDENT, advance past the closing ')', and continue
// processing the rest of the synthetic body.
TEST(PreprocessorSvMacro, FunctionLikeInSyntheticTooManyActualsRecovers)
{
    const std::string out = run("`define one(x) x\n"
                                "`define S `\"head-`one(a, b)-tail`\"\n"
                                "v = `S;\n");
    // "tail" still reaches the output (no crash, no infinite loop).
    // The bare `one (without its args) is what the error path emits;
    // we don't assert on it directly to keep the test focused on the
    // recovery property.
    EXPECT_NE(out.find("-tail\";"), std::string::npos);
}

TEST(PreprocessorSvMacro, TokenPasteJoinsIdentifiers)
{
    // Spec example: `define append(f) f``_master
    const std::string out = run("`define append(f) f``_master\n"
                                "wire `append(clock);\n");
    EXPECT_NE(out.find("wire clock_master;"), std::string::npos);
}

TEST(PreprocessorSvMacro, RegularStringInBodyIsLiteral)
{
    // §22.5.1: regular "..." string in macro body is NOT subject to
    // formal substitution. Only `"..."` (synthetic) is.
    const std::string out = run("`define D(x) $display(\"x\")\n"
                                "`D(hello);\n");
    EXPECT_NE(out.find("$display(\"x\")"), std::string::npos);
    EXPECT_EQ(out.find("\"hello\""), std::string::npos);
}

// ---------- Default macro arguments (§22.5.1) ------------------------

TEST(PreprocessorSvMacro, DefaultArgUsedWhenActualOmitted)
{
    // `define MACRO1(a=5, b="B", c) $display(a, b, c);
    // `MACRO1(,2,3)  // a omitted -> default 5
    const std::string out = run("`define M(a=5,b=\"B\",c) $display(a,,b,,c);\n"
                                "`M(,2,3)\n");
    EXPECT_NE(out.find("$display(5,,2,,3);"), std::string::npos);
}

TEST(PreprocessorSvMacro, DefaultArgUsedWhenActualEmptyMiddle)
{
    // `MACRO1(1, ,3)  // b omitted -> default "B"
    const std::string out = run("`define M(a=5,b=\"B\",c) $display(a,,b,,c);\n"
                                "`M(1, ,3)\n");
    EXPECT_NE(out.find("$display(1,,\"B\",,3);"), std::string::npos);
}

TEST(PreprocessorSvMacro, MissingTrailingActualUsesDefault)
{
    // `define MACRO3(a=5, b=0, c="C") $display(a, b, c);
    // `MACRO3( 1 )  // b and c omitted -> defaults
    const std::string out = run("`define M(a=5,b=0,c=\"C\") $display(a,,b,,c);\n"
                                "`M( 1 )\n");
    EXPECT_NE(out.find("$display(1,,0,,\"C\");"), std::string::npos);
}

TEST(PreprocessorSvMacro, MissingActualWithoutDefaultIsError)
{
    // `define M(a, b) ...   `M(1) -> error, b has no default
    const std::string out = run("`define M(a,b) a+b\n"
                                "bad = `M(1);\n"
                                "good = ok;\n");
    EXPECT_NE(out.find("good = ok;"), std::string::npos);
}

TEST(PreprocessorSvMacro, AllDefaultsAllowEmptyParens)
{
    // `define M(a=5, b=0) a+b   `M( )  // both defaults
    const std::string out = run("`define M(a=5,b=0) a+b\n"
                                "v = `M( );\n");
    EXPECT_NE(out.find("v = 5+0;"), std::string::npos);
}

TEST(PreprocessorSvMacro, DefaultArgsRequireSvMode)
{
    // Outside SV mode, '=' in formal list is an error.
    Veriparse::Parser::Preprocessor pp;
    pp.set_sv_mode(false);
    std::istringstream in("`define M(a=5) a\n"
                          "v = `M(1);\n"
                          "ok = done;\n");
    std::ostringstream out;
    EXPECT_EQ(pp.preprocess(in, "<inline>", out), 0);
    // Should still recover and continue past the bad directive.
    EXPECT_NE(out.str().find("ok = done;"), std::string::npos);
}

// ---------- Macro-as-filename in `include (§22.5.1) ------------------

class IncludeMacroFixture : public ::testing::Test
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
        m_dir = fs::temp_directory_path() / ("veriparse_pp_" + suffix);
        fs::create_directories(m_dir);
    }
    void write_file(const std::string &name, const std::string &content)
    {
        std::ofstream f(m_dir / name);
        f << content;
    }
    fs::path m_dir;
};

TEST_F(IncludeMacroFixture, ObjectLikeMacroAsIncludeFilename)
{
    mkdir_unique();
    write_file("hdr.svh", "wire from_macro_include;\n");

    Veriparse::Parser::Preprocessor pp;
    pp.set_sv_mode(true);
    pp.add_include_dir(m_dir.string());
    std::istringstream in("`define HDR \"hdr.svh\"\n"
                          "`include `HDR\n");
    std::ostringstream out;
    EXPECT_EQ(pp.preprocess(in, "<inline>", out), 0);
    EXPECT_NE(out.str().find("wire from_macro_include;"), std::string::npos);
}

// §22.5.1 last paragraph — function-like macro returning the filename.
// The collected actual is substituted into the macro body, the
// resulting "..." is re-fed to INCLUDE_ARG, and the include resolves
// normally.
TEST_F(IncludeMacroFixture, FunctionLikeMacroAsIncludeFilename)
{
    mkdir_unique();
    write_file("rs232_tx.svh", "wire from_function_include;\n");

    Veriparse::Parser::Preprocessor pp;
    pp.set_sv_mode(true);
    pp.add_include_dir(m_dir.string());
    std::istringstream in("`define hdr_path(name) `\"name.svh`\"\n"
                          "`include `hdr_path(rs232_tx)\n");
    std::ostringstream out;
    EXPECT_EQ(pp.preprocess(in, "<inline>", out), 0);
    EXPECT_NE(out.str().find("wire from_function_include;"), std::string::npos);
}

// Multiple actuals get joined into the resolved filename via formal
// substitution; the include opens the joined path.
TEST_F(IncludeMacroFixture, FunctionLikeMacroWithMultipleActuals)
{
    mkdir_unique();
    fs::create_directories(m_dir / "v2");
    write_file("v2/rs232_tx.svh", "wire from_v2_include;\n");

    Veriparse::Parser::Preprocessor pp;
    pp.set_sv_mode(true);
    pp.add_include_dir(m_dir.string());
    std::istringstream in("`define hdr(ver, mod) `\"ver/mod.svh`\"\n"
                          "`include `hdr(v2, rs232_tx)\n");
    std::ostringstream out;
    EXPECT_EQ(pp.preprocess(in, "<inline>", out), 0);
    EXPECT_NE(out.str().find("wire from_v2_include;"), std::string::npos);
}

// Whitespace between the macro name and '(' is allowed by §22.5.1
// for ordinary function-like calls; it must also work here.
TEST_F(IncludeMacroFixture, FunctionLikeMacroWithWhitespaceBeforeParen)
{
    mkdir_unique();
    write_file("rs232.svh", "wire spaced_paren_ok;\n");

    Veriparse::Parser::Preprocessor pp;
    pp.set_sv_mode(true);
    pp.add_include_dir(m_dir.string());
    std::istringstream in("`define hdr_path(name) `\"name.svh`\"\n"
                          "`include `hdr_path  (rs232)\n");
    std::ostringstream out;
    EXPECT_EQ(pp.preprocess(in, "<inline>", out), 0);
    EXPECT_NE(out.str().find("wire spaced_paren_ok;"), std::string::npos);
}

// Error recovery: missing actual (no default). The bad `include is
// dropped, the preprocessor continues with the rest of the file.
TEST_F(IncludeMacroFixture, FunctionLikeMacroMissingActualRecovers)
{
    mkdir_unique();

    Veriparse::Parser::Preprocessor pp;
    pp.set_sv_mode(true);
    pp.add_include_dir(m_dir.string());
    std::istringstream in("`define hdr(a, b) \"a-b.svh\"\n"
                          "`include `hdr(only_one)\n"
                          "after;\n");
    std::ostringstream out;
    EXPECT_EQ(pp.preprocess(in, "<inline>", out), 0);
    EXPECT_NE(out.str().find("after;"), std::string::npos);
}

// Edge case: actual contains nested parens — the bracket counter
// must not split it on the inner comma.
TEST_F(IncludeMacroFixture, FunctionLikeMacroActualWithMatchedParens)
{
    mkdir_unique();
    write_file("common.svh", "wire matched_parens_ok;\n");

    Veriparse::Parser::Preprocessor pp;
    pp.set_sv_mode(true);
    pp.add_include_dir(m_dir.string());
    std::istringstream in("`define hdr(unused) `\"common.svh`\"\n"
                          "`include `hdr(foo(a,b))\n");
    std::ostringstream out;
    EXPECT_EQ(pp.preprocess(in, "<inline>", out), 0);
    EXPECT_NE(out.str().find("wire matched_parens_ok;"), std::string::npos);
}

// Edge case: actual is a string literal containing commas — the
// MACRO_CALL_STRING sub-state hides them from the separator logic.
TEST_F(IncludeMacroFixture, FunctionLikeMacroActualWithStringContainingCommas)
{
    mkdir_unique();
    write_file("common.svh", "wire string_in_arg_ok;\n");

    Veriparse::Parser::Preprocessor pp;
    pp.set_sv_mode(true);
    pp.add_include_dir(m_dir.string());
    std::istringstream in("`define hdr(unused) `\"common.svh`\"\n"
                          "`include `hdr(\"a,b,c\")\n");
    std::ostringstream out;
    EXPECT_EQ(pp.preprocess(in, "<inline>", out), 0);
    EXPECT_NE(out.str().find("wire string_in_arg_ok;"), std::string::npos);
}

// Edge case: the macro arg list spans multiple lines.
TEST_F(IncludeMacroFixture, FunctionLikeMacroActualSpansMultipleLines)
{
    mkdir_unique();
    write_file("rs232.svh", "wire multiline_ok;\n");

    Veriparse::Parser::Preprocessor pp;
    pp.set_sv_mode(true);
    pp.add_include_dir(m_dir.string());
    std::istringstream in("`define hdr(name) `\"name.svh`\"\n"
                          "`include `hdr(\n"
                          "    rs232\n"
                          ")\n");
    std::ostringstream out;
    EXPECT_EQ(pp.preprocess(in, "<inline>", out), 0);
    EXPECT_NE(out.str().find("wire multiline_ok;"), std::string::npos);
}

// Edge case: zero formals + empty parens is allowed by §22.5.1.
TEST_F(IncludeMacroFixture, FunctionLikeMacroZeroFormalsEmptyParens)
{
    mkdir_unique();
    write_file("common.svh", "wire zero_formals_ok;\n");

    Veriparse::Parser::Preprocessor pp;
    pp.set_sv_mode(true);
    pp.add_include_dir(m_dir.string());
    std::istringstream in("`define hdr() `\"common.svh`\"\n"
                          "`include `hdr()\n");
    std::ostringstream out;
    EXPECT_EQ(pp.preprocess(in, "<inline>", out), 0);
    EXPECT_NE(out.str().find("wire zero_formals_ok;"), std::string::npos);
}

// Error recovery: too many actuals — the bad `include is dropped,
// preprocessor continues.
TEST_F(IncludeMacroFixture, FunctionLikeMacroTooManyActualsRecovers)
{
    mkdir_unique();

    Veriparse::Parser::Preprocessor pp;
    pp.set_sv_mode(true);
    pp.add_include_dir(m_dir.string());
    std::istringstream in("`define hdr(a) `\"a.svh`\"\n"
                          "`include `hdr(one, two)\n"
                          "after;\n");
    std::ostringstream out;
    EXPECT_EQ(pp.preprocess(in, "<inline>", out), 0);
    EXPECT_NE(out.str().find("after;"), std::string::npos);
}

// Error recovery: trailing content on the `include line (here a
// comment) must be dropped, not leaked into the output. Without this
// the comment from the failing `include line would appear in the
// preprocessor stream as if it were standalone source.
TEST_F(IncludeMacroFixture, FunctionLikeMacroErrorDropsTrailingComment)
{
    mkdir_unique();

    Veriparse::Parser::Preprocessor pp;
    pp.set_sv_mode(true);
    pp.add_include_dir(m_dir.string());
    std::istringstream in("`define hdr(a) `\"a.svh`\"\n"
                          "`include `hdr(too, many) // leaking_comment\n"
                          "after;\n");
    std::ostringstream out;
    EXPECT_EQ(pp.preprocess(in, "<inline>", out), 0);
    EXPECT_NE(out.str().find("after;"), std::string::npos);
    EXPECT_EQ(out.str().find("leaking_comment"), std::string::npos);
}

// Same recovery story when the function-like form is missing its
// opening '(' — MACRO_CALL_PRE error path.
TEST_F(IncludeMacroFixture, FunctionLikeMacroNoParenDropsTrailingComment)
{
    mkdir_unique();

    Veriparse::Parser::Preprocessor pp;
    pp.set_sv_mode(true);
    pp.add_include_dir(m_dir.string());
    std::istringstream in("`define hdr(a) `\"a.svh`\"\n"
                          "`include `hdr ! // leaking_comment\n"
                          "after;\n");
    std::ostringstream out;
    EXPECT_EQ(pp.preprocess(in, "<inline>", out), 0);
    EXPECT_NE(out.str().find("after;"), std::string::npos);
    EXPECT_EQ(out.str().find("leaking_comment"), std::string::npos);
}

// SV default formal value: actual omitted → default text is used.
TEST_F(IncludeMacroFixture, FunctionLikeMacroDefaultFormalUsedForEmptyActual)
{
    mkdir_unique();
    write_file("fallback.svh", "wire default_arg_ok;\n");

    Veriparse::Parser::Preprocessor pp;
    pp.set_sv_mode(true);
    pp.add_include_dir(m_dir.string());
    std::istringstream in("`define hdr(name=fallback) `\"name.svh`\"\n"
                          "`include `hdr()\n");
    std::ostringstream out;
    EXPECT_EQ(pp.preprocess(in, "<inline>", out), 0);
    EXPECT_NE(out.str().find("wire default_arg_ok;"), std::string::npos);
}

// Function-like `include `hdr followed by a newline (instead of '(')
// must surface as an error on the `include line — the missing-'('
// recovery must NOT swallow the first token of the next line.
TEST_F(IncludeMacroFixture, FunctionLikeIncludeNoParenStraddlingNewline)
{
    mkdir_unique();

    Veriparse::Parser::Preprocessor pp;
    pp.set_sv_mode(true);
    pp.add_include_dir(m_dir.string());
    std::istringstream in("`define hdr(a) `\"a.svh`\"\n"
                          "`include `hdr\n"
                          "module foo; endmodule\n");
    std::ostringstream out;
    EXPECT_EQ(pp.preprocess(in, "<inline>", out), 0);
    EXPECT_NE(out.str().find("module foo; endmodule"), std::string::npos);
}

// `include `UNDEFINED — the bad ref logs an error and the rest of
// the directive line is dropped, so no cascading INCLUDE_ARG error
// fires on the trailing comment.
TEST_F(IncludeMacroFixture, IncludeUndefinedMacroDropsRestOfLine)
{
    mkdir_unique();

    Veriparse::Parser::Preprocessor pp;
    pp.set_sv_mode(true);
    pp.add_include_dir(m_dir.string());
    std::istringstream in("`include `UNDEFINED // leaking_comment\n"
                          "after;\n");
    std::ostringstream out;
    EXPECT_EQ(pp.preprocess(in, "<inline>", out), 0);
    EXPECT_NE(out.str().find("after;"), std::string::npos);
    EXPECT_EQ(out.str().find("leaking_comment"), std::string::npos);
}

} // namespace
