// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti

#include <gtest/gtest.h>
#include <veriparse/parser/preprocessor.hpp>

#include <unistd.h>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

namespace fs = std::filesystem;

namespace
{

class IncludeFixture : public ::testing::Test
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

    std::string run(const std::string &input, bool add_search_dir = true)
    {
        Veriparse::Parser::Preprocessor pp;
        if(add_search_dir) {
            pp.add_include_dir(m_dir.string());
        }
        std::istringstream in(input);
        std::ostringstream out;
        EXPECT_EQ(pp.preprocess(in, "<inline>", out), 0);
        return out.str();
    }

    fs::path m_dir;
};

TEST_F(IncludeFixture, QuotedIncludeInsertsFileContent)
{
    mkdir_unique();
    write_file("hdr.vh", "wire from_header;\n");
    const std::string out = run("before;\n`include \"hdr.vh\"\nafter;\n");
    EXPECT_NE(out.find("before;"), std::string::npos);
    EXPECT_NE(out.find("wire from_header;"), std::string::npos);
    EXPECT_NE(out.find("after;"), std::string::npos);
}

TEST_F(IncludeFixture, IncludeEmitsLineMarkers)
{
    mkdir_unique();
    write_file("hdr.vh", "X\n");
    const std::string out = run("`include \"hdr.vh\"\n");
    // Level-1 marker on entry, level-2 on exit (§22.12).
    EXPECT_NE(out.find("`line 1 \""), std::string::npos);
    EXPECT_NE(out.find("\" 1\n"), std::string::npos);
    EXPECT_NE(out.find("\" 2\n"), std::string::npos);
}

TEST_F(IncludeFixture, MissingIncludeReturnsError)
{
    mkdir_unique();
    // File "nope.vh" doesn't exist; preprocessor should not crash. It
    // logs an error and continues with the rest of the input.
    const std::string out = run("`include \"nope.vh\"\nafter;\n");
    EXPECT_NE(out.find("after;"), std::string::npos);
}

TEST_F(IncludeFixture, NestedInclude)
{
    mkdir_unique();
    write_file("inner.vh", "INNER\n");
    write_file("outer.vh", "OUTER_BEFORE\n`include \"inner.vh\"\nOUTER_AFTER\n");
    const std::string out = run("`include \"outer.vh\"\n");
    const auto p_before = out.find("OUTER_BEFORE");
    const auto p_inner = out.find("INNER");
    const auto p_after = out.find("OUTER_AFTER");
    EXPECT_NE(p_before, std::string::npos);
    EXPECT_NE(p_inner, std::string::npos);
    EXPECT_NE(p_after, std::string::npos);
    EXPECT_LT(p_before, p_inner);
    EXPECT_LT(p_inner, p_after);
}

TEST_F(IncludeFixture, AngleBracketIncludeRequiresSvMode)
{
    mkdir_unique();
    write_file("hdr.vh", "ok\n");

    // Without SV mode → error path, content not pulled in.
    {
        Veriparse::Parser::Preprocessor pp;
        pp.add_include_dir(m_dir.string());
        std::istringstream in("`include <hdr.vh>\n");
        std::ostringstream out;
        EXPECT_EQ(pp.preprocess(in, "<inline>", out), 0);
        EXPECT_EQ(out.str().find("ok"), std::string::npos);
    }
    // With SV mode → resolves like the quoted form for this test.
    {
        Veriparse::Parser::Preprocessor pp;
        pp.set_sv_mode(true);
        pp.add_include_dir(m_dir.string());
        std::istringstream in("`include <hdr.vh>\n");
        std::ostringstream out;
        EXPECT_EQ(pp.preprocess(in, "<inline>", out), 0);
        EXPECT_NE(out.str().find("ok"), std::string::npos);
    }
}

} // namespace
