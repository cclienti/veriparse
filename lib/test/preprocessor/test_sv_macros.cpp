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

} // namespace
