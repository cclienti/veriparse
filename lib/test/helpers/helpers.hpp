// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_LIB_TEST_HELPERS_HPP
#define VERIPARSE_LIB_TEST_HELPERS_HPP

#include <veriparse/generators/yaml_generator.hpp>
#include <veriparse/generators/dot_generator.hpp>
#include <veriparse/generators/verilog_generator.hpp>
#include <veriparse/AST/node.hpp>
#include <veriparse/logger/logger.hpp>
#include <veriparse/version.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <cstdlib>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#define ENABLE_LOGGER                                                                              \
    const ::testing::TestInfo *const test_info =                                                   \
        ::testing::UnitTest::GetInstance()->current_test_info();                                   \
    std::string test_name = test_info->name();                                                     \
    std::string test_string = test_info->test_case_name() + std::string(".") + test_name;          \
                                                                                                   \
    Logger::remove_all_sinks();                                                                    \
    Logger::add_text_sink(test_string + ".log");                                                   \
    Logger::add_stderr_sink();                                                                     \
                                                                                                   \
    LOG_INFO << "Veriparse version: " << Veriparse::Version::get_version() << " - "                \
             << Veriparse::Version::get_sha1()

namespace Veriparse
{

// The AST is a tree: every node must be reached as a child exactly once. A node
// reached a second time is shared between two parents (the dot output shows it
// as a node with several incoming edges). Such sharing breaks clone() / in-place
// mutation passes and is hidden by is_equal(). Collect every shared node.
inline void collect_shared_nodes(const AST::Node::Ptr &node, std::set<const void *> &seen,
                                 std::vector<AST::Node::Ptr> &shared)
{
    if(!node) {
        return;
    }
    if(!seen.insert(node.get()).second) {
        shared.push_back(node);
    }
    const AST::Node::ListPtr children = node->get_children();
    if(children) {
        for(const AST::Node::Ptr &c : *children) {
            collect_shared_nodes(c, seen, shared);
        }
    }
}

// Empty string if `node` is a proper tree; otherwise a description of the shared
// nodes (with their source lines).
inline std::string node_sharing_report(const AST::Node::Ptr &node)
{
    std::set<const void *> seen;
    std::vector<AST::Node::Ptr> shared;
    collect_shared_nodes(node, seen, shared);
    if(shared.empty()) {
        return "";
    }
    std::ostringstream os;
    os << shared.size() << " shared AST node(s) (tree invariant violated), at lines:";
    for(const AST::Node::Ptr &n : shared) {
        os << " " << n->get_line();
    }
    return os.str();
}

// Assert that `node` is a proper tree (no node shared between two parents).
// Use after a parse and after each transformation pass.
#define ASSERT_AST_IS_TREE(node)                                                                   \
    do {                                                                                           \
        const std::string _sharing = Veriparse::node_sharing_report(node);                         \
        ASSERT_TRUE(_sharing.empty()) << _sharing;                                                 \
    } while(0)

class TestHelpers
{
public:
    TestHelpers(const std::string &testcases_path) : m_testcases_path(testcases_path)
    {
        const char *veriparse_source_root(std::getenv("VERIPARSE_SOURCE_ROOT"));
        if(veriparse_source_root != nullptr) {
            m_testcases_path = boost::filesystem::path(veriparse_source_root) / testcases_path;
        }
        LOG_INFO << "testcases path: " << m_testcases_path;
    }

    std::string get_verilog_filename(const std::string &teststr)
    {
        auto filename = teststr + ".v";
        auto filename_path = m_testcases_path / boost::filesystem::path(filename);
        return filename_path.string();
    }

    std::string get_sv_filename(const std::string &teststr)
    {
        auto filename = teststr + ".sv";
        auto filename_path = m_testcases_path / boost::filesystem::path(filename);
        return filename_path.string();
    }

    std::string get_yaml_filename(const std::string &teststr)
    {
        auto filename = teststr + ".yaml";
        auto filename_path = m_testcases_path / boost::filesystem::path(filename);
        LOG_INFO << filename_path;
        return filename_path.string();
    }

    void render_node_to_yaml_file(AST::Node::Ptr node, const std::string &filename)
    {
        YAML::Node yaml = Generators::YAMLGenerator().render(node);
        YAML::Emitter emitter;
        emitter << YAML::Indent(2) << yaml;
        std::ofstream fout(filename);
        fout << emitter.c_str() << std::endl;
    }

    void render_node_to_dot_file(AST::Node::Ptr node, const std::string &filename)
    {
        std::string dot = Generators::DotGenerator().render(node);
        Generators::DotGenerator::save_dot(dot, filename);
    }

    template <typename TNodeListPtr>
    void render_node_list_to_dot_files(TNodeListPtr nodes, const std::string &basename)
    {
        int i = 0;
        for(AST::Node::Ptr n : *nodes) {
            std::stringstream ss;
            ss << basename << i++ << ".dot";
            render_node_to_dot_file(AST::to_node(n), ss.str());
        }
    }

    void render_node_to_verilog_file(AST::Node::Ptr node, const std::string &filename)
    {
        std::string str = Generators::VerilogGenerator().render(node);
        std::ofstream fout(filename);
        fout << str << std::endl;
    }

private:
    boost::filesystem::path m_testcases_path;
};

} // namespace Veriparse

#endif
