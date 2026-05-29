// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti

// Provenance tracking: filenames in `__FILE__ and in `line markers,
// line numbers in `__LINE__ and `line markers, across plain input
// and `include boundaries. These invariants are what makes downstream
// error messages point at the right place.

#include <gtest/gtest.h>
#include <veriparse/parser/preprocessor.hpp>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

namespace fs = std::filesystem;

namespace
{

class LineTrackingFixture : public ::testing::Test
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
        m_dir = fs::temp_directory_path() / ("veriparse_pp_lt_" + suffix);
        fs::create_directories(m_dir);
    }

    void write_file(const std::string &name, const std::string &content)
    {
        std::ofstream f(m_dir / name);
        f << content;
    }

    std::string preprocess_file(const std::string &name, bool sv_mode = true)
    {
        Veriparse::Parser::Preprocessor pp;
        pp.set_sv_mode(sv_mode);
        pp.add_include_dir(m_dir.string());
        std::ostringstream out;
        EXPECT_EQ(pp.preprocess((m_dir / name).string(), out), 0);
        return out.str();
    }

    fs::path m_dir;
};

// ---------- `__LINE__ accuracy in a single file -------------------

TEST(PreprocessorLineTracking, LineMacroAtKnownPosition)
{
    Veriparse::Parser::Preprocessor pp;
    pp.set_sv_mode(true);
    std::istringstream in("\n"                 // line 1
                          "\n"                 // line 2
                          "\n"                 // line 3
                          "// pad\n"           // line 4
                          "x = `__LINE__;\n"); // line 5
    std::ostringstream out;
    EXPECT_EQ(pp.preprocess(in, "<inline>", out), 0);
    EXPECT_NE(out.str().find("x = 5;"), std::string::npos);
}

TEST(PreprocessorLineTracking, LineMacroAfterMacroExpansion)
{
    // A multi-line macro body must not shift the source line counter:
    // __LINE__ on the line *after* the macro invocation reports the
    // original line in the source, not somewhere inside the body.
    Veriparse::Parser::Preprocessor pp;
    pp.set_sv_mode(true);
    std::istringstream in("`define PAD a + \\\n"
                          "             b + \\\n"
                          "             c\n"
                          "v = `PAD;\n"
                          "w = `__LINE__;\n");
    std::ostringstream out;
    EXPECT_EQ(pp.preprocess(in, "<inline>", out), 0);
    EXPECT_NE(out.str().find("w = 5;"), std::string::npos);
}

// ---------- `__FILE__ / `__LINE__ inside an included file ---------

TEST_F(LineTrackingFixture, LineMacroInsideIncludeRefersToIncludedFile)
{
    mkdir_unique();
    // hdr.vh has `__LINE__ on its own line 3. Once expanded it must
    // report 3 (the position in hdr.vh), not the include-site line.
    write_file("hdr.vh", "// padding 1\n"
                         "// padding 2\n"
                         "hi = `__LINE__;\n");
    write_file("top.v", "// top line 1\n"
                        "// top line 2\n"
                        "// top line 3\n"
                        "`include \"hdr.vh\"\n");
    const std::string out = preprocess_file("top.v");
    EXPECT_NE(out.find("hi = 3;"), std::string::npos);
}

TEST_F(LineTrackingFixture, FileMacroInsideIncludeRefersToIncludedFile)
{
    mkdir_unique();
    write_file("hdr.vh", "src = `__FILE__;\n");
    write_file("top.v", "`include \"hdr.vh\"\n");
    const std::string out = preprocess_file("top.v");
    // The substituted filename must be the header, not the top file.
    EXPECT_NE(out.find("hdr.vh"), std::string::npos);
    EXPECT_EQ(out.find("src = \"" + (m_dir / "top.v").string() + "\""), std::string::npos);
}

// ---------- `line marker positions on include enter / exit -------

TEST_F(LineTrackingFixture, IncludeEntryMarkerStartsAtLineOneOfHeader)
{
    mkdir_unique();
    write_file("hdr.vh", "wire from_hdr;\n");
    write_file("top.v", "// 1\n"
                        "// 2\n"
                        "`include \"hdr.vh\"\n");
    const std::string out = preprocess_file("top.v");
    // §22.12: entering an include emits "`line 1 "<incl>" 1".
    EXPECT_NE(out.find("`line 1 \"" + (m_dir / "hdr.vh").string() + "\" 1"), std::string::npos);
}

TEST_F(LineTrackingFixture, IncludeExitMarkerReturnsToLineAfterDirective)
{
    mkdir_unique();
    write_file("hdr.vh", "wire from_hdr;\n");
    // `include is on line 3 of top.v, so the exit marker must point
    // at line 4 of top.v (the next line after the directive).
    write_file("top.v", "// 1\n"
                        "// 2\n"
                        "`include \"hdr.vh\"\n"
                        "wire after_include;\n");
    const std::string out = preprocess_file("top.v");
    EXPECT_NE(out.find("`line 4 \"" + (m_dir / "top.v").string() + "\" 2"), std::string::npos);
}

} // namespace
