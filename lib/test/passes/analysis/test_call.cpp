// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include "../../helpers/helpers.hpp"
#include <veriparse/parser/verilog.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/passes/analysis/call.hpp>
#include <veriparse/logger/logger.hpp>
#include <gtest/gtest.h>

using namespace Veriparse;

static TestHelpers test_helpers("lib/test/parser/testcases/");

TEST(PassesAnalysis_Call, width1)
{
    ENABLE_LOGGER;

    Parser::Verilog verilog;
    verilog.parse(test_helpers.get_verilog_filename(test_name));
    AST::Node::Ptr source = verilog.get_source();
    ASSERT_TRUE(source != nullptr);

    // `word(address, MEM_WORD);` is a function called as a statement: the
    // parser emits the neutral Call (task-vs-function is not syntactic).
    AST::Call::ListPtr call_list = Passes::Analysis::Module::get_call_nodes(source);
    ASSERT_EQ(1u, call_list->size());

    AST::Call::Ptr call = call_list->front();
    ASSERT_TRUE(call->is_node_type(AST::NodeType::Call));
    ASSERT_EQ("word", call->get_name());

    AST::Identifier::ListPtr identifiers;
    identifiers = Passes::Analysis::Call::get_argument_identifier_nodes(call);
    ASSERT_EQ(2u, identifiers->size());

    std::vector<std::string> names;
    std::vector<std::string> ref_names{"address", "MEM_WORD"};
    names = Passes::Analysis::Call::get_argument_identifier_names(call);
    ASSERT_EQ(2u, names.size());
    ASSERT_EQ(ref_names, names);
}

TEST(PassesAnalysis_Call, taskcall0)
{
    ENABLE_LOGGER;

    Parser::Verilog verilog;
    verilog.parse(test_helpers.get_verilog_filename(test_name));
    AST::Node::Ptr source = verilog.get_source();
    ASSERT_TRUE(source != nullptr);

    // The statement call `mytask(...)` is a neutral Call; its nested
    // `otherfunc(in3)` operand is expression position, hence a FunctionCall —
    // both belong to the Call category.
    AST::Call::ListPtr call_list = Passes::Analysis::Module::get_call_nodes(source);
    ASSERT_EQ(2u, call_list->size());

    AST::Call::Ptr call = call_list->front();
    ASSERT_TRUE(call->is_node_type(AST::NodeType::Call));
    ASSERT_EQ("mytask", call->get_name());
    ASSERT_TRUE(call_list->back()->is_node_type(AST::NodeType::FunctionCall));
    ASSERT_EQ("otherfunc", call_list->back()->get_name());

    AST::Identifier::ListPtr fct1_identifiers;
    fct1_identifiers = Passes::Analysis::Call::get_argument_identifier_nodes(call);
    ASSERT_EQ(4u, fct1_identifiers->size());

    std::vector<std::string> fct1_names;
    std::vector<std::string> fct1_ref_names{"in1", "in2", "in0", "in3"};
    fct1_names = Passes::Analysis::Call::get_argument_identifier_names(call);
    ASSERT_EQ(4u, fct1_names.size());
    ASSERT_EQ(fct1_ref_names, fct1_names);
}
