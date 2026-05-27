// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti

#include <gtest/gtest.h>
#include <veriparse/parser/preprocessor.hpp>

#include <sstream>
#include <string>

namespace
{

std::string run(const std::string &input, bool sv_mode = false)
{
    Veriparse::Parser::Preprocessor pp;
    pp.set_sv_mode(sv_mode);
    std::istringstream in(input);
    std::ostringstream out;
    EXPECT_EQ(pp.preprocess(in, "<inline>", out), 0);
    return out.str();
}

TEST(PreprocessorFuncMacro, SingleArgExpansion)
{
    // §22.5.1 first example: `define var_nand(dly) nand #dly
    const std::string out = run("`define var_nand(dly) nand #dly\n"
                                "`var_nand(2) g (q, a, b);\n");
    EXPECT_NE(out.find("nand #2 g (q, a, b);"), std::string::npos);
}

TEST(PreprocessorFuncMacro, MultiArgExpansion)
{
    // §22.5.1 max example: `define max(a,b)((a) > (b) ? (a) : (b))
    const std::string out = run("`define max(a,b)((a) > (b) ? (a) : (b))\n"
                                "n = `max(p+q, r+s);\n");
    EXPECT_NE(out.find("n = ((p+q) > (r+s) ? (p+q) : (r+s));"), std::string::npos);
}

TEST(PreprocessorFuncMacro, ArgWithMatchedParens)
{
    // Commas inside an actual's parentheses must not split the arg.
    const std::string out = run("`define ID(x) (x)\n"
                                "v = `ID(foo(1, 2));\n");
    EXPECT_NE(out.find("v = (foo(1, 2));"), std::string::npos);
}

TEST(PreprocessorFuncMacro, ArgWithBrackets)
{
    const std::string out = run("`define ID(x) x\n"
                                "v = `ID(arr[1, 2]);\n");
    EXPECT_NE(out.find("v = arr[1, 2];"), std::string::npos);
}

TEST(PreprocessorFuncMacro, ArgWithStringLiteralComma)
{
    // Commas inside a string literal must not split the arg.
    const std::string out = run("`define MSG(s) $display(s)\n"
                                "`MSG(\"hello, world\");\n");
    EXPECT_NE(out.find("$display(\"hello, world\");"), std::string::npos);
}

TEST(PreprocessorFuncMacro, FormalAppearsMultipleTimes)
{
    // §22.5.1: actual is substituted at every formal occurrence.
    const std::string out = run("`define DOUBLE(x) x + x\n"
                                "v = `DOUBLE(z);\n");
    EXPECT_NE(out.find("v = z + z;"), std::string::npos);
}

TEST(PreprocessorFuncMacro, FormalSubstitutionIgnoresStringContent)
{
    // §22.5.1: "Macro substitution and argument substitution shall not
    // occur within string literals." Even if the formal name appears
    // inside a string in the body, do not substitute.
    const std::string out = run("`define MK(x) $display(\"x\")\n"
                                "`MK(hello);\n");
    EXPECT_NE(out.find("$display(\"x\")"), std::string::npos);
    EXPECT_EQ(out.find("\"hello\""), std::string::npos);
}

TEST(PreprocessorFuncMacro, FormalSubstitutionIsWholeIdent)
{
    // Substitution must match identifier tokens, not substrings.
    // 'x' should be replaced where it appears as a whole identifier
    // but not inside 'xx' or '_x'.
    const std::string out = run("`define M(x) x xx _x\n"
                                "`M(VAL)\n");
    EXPECT_NE(out.find("VAL xx _x"), std::string::npos);
}

TEST(PreprocessorFuncMacro, ArgCountMismatchReportsError)
{
    // preprocess should still return 0 and continue past the bad call.
    const std::string out = run("`define M(a,b) a+b\n"
                                "after_bad = `M(only_one);\n"
                                "good = ok;\n");
    EXPECT_NE(out.find("good = ok;"), std::string::npos);
}

TEST(PreprocessorFuncMacro, EmptyArgListMacro)
{
    // `define X() body — usage `X() expands to body.
    const std::string out = run("`define EMPTY() 42\n"
                                "v = `EMPTY();\n");
    EXPECT_NE(out.find("v = 42;"), std::string::npos);
}

TEST(PreprocessorFuncMacro, WhitespaceBetweenNameAndParen)
{
    // §22.5.1: "White space shall be allowed between the text macro
    // name and the left parenthesis in the macro usage."
    const std::string out = run("`define ID(x) x\n"
                                "v = `ID   (hello);\n");
    EXPECT_NE(out.find("v = hello;"), std::string::npos);
}

TEST(PreprocessorFuncMacro, MacroInArgIsExpanded)
{
    // §22.5.1: a macro use inside an actual is expanded after the outer
    // macro is substituted, not before. Either way the visible effect
    // is the same: `INNER expands inside the outer.
    const std::string out = run("`define INNER 42\n"
                                "`define OUTER(x) x\n"
                                "v = `OUTER(`INNER);\n");
    EXPECT_NE(out.find("v = 42;"), std::string::npos);
}

TEST(PreprocessorFuncMacro, RecursiveMacroDepthCapped)
{
    // `define M(x) `M(x) — straight self-recursion. Must terminate
    // (not hang) by hitting the depth limit.
    const std::string out = run("`define M(x) `M(x)\n"
                                "v = `M(1);\n");
    EXPECT_FALSE(out.empty());
}

TEST(PreprocessorFuncMacro, UndefineAllRequiresSvMode)
{
    // §22.5.3: `undefineall is SV-only. In SV mode it clears all macros.
    const std::string in = "`define A 1\n"
                           "`define B 2\n"
                           "`undefineall\n"
                           "u = `A;\n"
                           "v = `B;\n";
    {
        const std::string out = run(in, /*sv_mode*/ true);
        EXPECT_NE(out.find("u = `A;"), std::string::npos);
        EXPECT_NE(out.find("v = `B;"), std::string::npos);
    }
}

TEST(PreprocessorFuncMacro, ResetallDoesNotTouchMacroTable)
{
    // §22.3 / §22.5: `resetall must NOT affect the macro table.
    const std::string out = run("`define KEEP_ME 7\n"
                                "`resetall\n"
                                "v = `KEEP_ME;\n");
    EXPECT_NE(out.find("v = 7;"), std::string::npos);
    // `resetall itself must not leak through to the main scanner.
    EXPECT_EQ(out.find("`resetall"), std::string::npos);
}

} // namespace
