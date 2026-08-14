// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include "../../helpers/helpers.hpp"
#include <veriparse/parser/verilog.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/passes/analysis/storage_kind.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <gtest/gtest.h>

using namespace Veriparse;

static TestHelpers test_helpers("lib/test/passes/analysis/testcases/");

// The §23.2.2.3 kind rules and the §13.5.2 net classification over the
// port spectrum: an input port is a net unless declared `var`; an output
// with an explicit data type is a variable, an implicit one a net; body
// declarations classify by their node kind.
TEST(PassesAnalysis_StorageKind, storage_kind0)
{
    ENABLE_LOGGER;

    Parser::Verilog verilog;
    verilog.set_sv_mode(true);
    verilog.parse(test_helpers.get_sv_filename(test_name));
    AST::Node::Ptr source = verilog.get_source();
    ASSERT_TRUE(source != nullptr);

    Passes::Analysis::Module::ModulesMap modules_map;
    Passes::Analysis::Module::get_module_dictionary(source, modules_map);
    ASSERT_EQ(1u, modules_map.count(test_name));
    const auto &module = modules_map[test_name];

    using SK = Passes::Analysis::StorageKind;
    EXPECT_TRUE(SK::is_net(module, "in_net"));
    EXPECT_FALSE(SK::is_net(module, "in_var"));
    EXPECT_FALSE(SK::is_net(module, "out_var_t"));
    EXPECT_TRUE(SK::is_net(module, "out_net_t"));
    EXPECT_TRUE(SK::is_net(module, "out_wire"));
    EXPECT_FALSE(SK::is_net(module, "v"));
    EXPECT_TRUE(SK::is_net(module, "w"));
    EXPECT_FALSE(SK::is_net(module, "absent"));

    const auto &ports = module->get_ports();
    ASSERT_TRUE(ports != nullptr);
    std::map<std::string, bool> variable;
    for(const auto &port : *ports) {
        ASSERT_TRUE(port->is_node_type(AST::NodeType::Port));
        const auto &p = AST::cast_to<AST::Port>(port);
        variable[p->get_name()] = SK::port_is_variable(p);
    }
    EXPECT_FALSE(variable["in_net"]);
    EXPECT_TRUE(variable["in_var"]);
    EXPECT_TRUE(variable["out_var_t"]);
    EXPECT_FALSE(variable["out_net_t"]);
    EXPECT_FALSE(variable["out_wire"]);

    // A select classifies by its base (§13.5.2 "selects into nets").
    auto id = std::make_shared<AST::Identifier>("f", 1);
    id->set_name("w");
    auto sel = std::make_shared<AST::Pointer>("f", 1);
    sel->set_var(AST::to_node(id));
    const auto &base = SK::select_base(AST::to_node(sel));
    ASSERT_TRUE(base != nullptr);
    EXPECT_EQ("w", base->get_name());
}
