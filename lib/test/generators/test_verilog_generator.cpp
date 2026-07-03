// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include "../helpers/helpers.hpp"

#include <gtest/gtest.h>
#include <veriparse/parser/verilog.hpp>
#include <veriparse/logger/logger.hpp>
#include <veriparse/generators/yaml_generator.hpp>
#include <veriparse/generators/dot_generator.hpp>
#include <veriparse/generators/verilog_generator.hpp>
#include <veriparse/importers/yaml_importer.hpp>
#include <fstream>
#include <sstream>
#include <string>

using namespace Veriparse;

static TestHelpers test_helpers("lib/test/parser/testcases/");

#define TEST_CORE                                                                                  \
    const ::testing::TestInfo *const test_info =                                                   \
        ::testing::UnitTest::GetInstance()->current_test_info();                                   \
    std::string test_name = test_info->name();                                                     \
    std::string test_string = test_info->test_case_name() + std::string(".") + test_name;          \
                                                                                                   \
    Logger::remove_all_sinks();                                                                    \
    Logger::add_text_sink(test_string + ".log");                                                   \
    Logger::add_stderr_sink();                                                                     \
                                                                                                   \
    /* Render the ref YAML file */                                                                 \
    AST::Node::Ptr source_ref =                                                                    \
        Importers::YAMLImporter().import(test_helpers.get_yaml_filename(test_name).c_str());       \
    ASSERT_TRUE(source_ref != nullptr);                                                            \
    test_helpers.render_node_to_verilog_file(source_ref, test_string + ".v");                      \
                                                                                                   \
    /* Parse the rendered file */                                                                  \
    Parser::Verilog verilog;                                                                       \
    verilog.parse(test_string + ".v");                                                             \
    AST::Node::Ptr source = verilog.get_source();                                                  \
    ASSERT_TRUE(source != nullptr);                                                                \
    ASSERT_AST_IS_TREE(source); /* round-trip reparse must stay a proper tree */                   \
    test_helpers.render_node_to_yaml_file(source, test_string + "_parsed.yaml");                   \
    test_helpers.render_node_to_dot_file(source, test_string + "_parsed.dot");                     \
                                                                                                   \
    /* Check that source is equal to source_ref */                                                 \
    ASSERT_TRUE(source->is_equal(*source_ref))

#define TEST_CORE_SV                                                                               \
    const ::testing::TestInfo *const test_info =                                                   \
        ::testing::UnitTest::GetInstance()->current_test_info();                                   \
    std::string test_name = test_info->name();                                                     \
    std::string test_string = test_info->test_case_name() + std::string(".") + test_name;          \
                                                                                                   \
    Logger::remove_all_sinks();                                                                    \
    Logger::add_text_sink(test_string + ".log");                                                   \
    Logger::add_stderr_sink();                                                                     \
                                                                                                   \
    /* Render the ref YAML file */                                                                 \
    AST::Node::Ptr source_ref =                                                                    \
        Importers::YAMLImporter().import(test_helpers.get_yaml_filename(test_name).c_str());       \
    ASSERT_TRUE(source_ref != nullptr);                                                            \
    test_helpers.render_node_to_verilog_file(source_ref, test_string + ".sv");                     \
                                                                                                   \
    /* Parse the rendered file in SV mode */                                                       \
    Parser::Verilog verilog;                                                                       \
    verilog.set_sv_mode(true);                                                                     \
    verilog.parse(test_string + ".sv");                                                            \
    AST::Node::Ptr source = verilog.get_source();                                                  \
    ASSERT_TRUE(source != nullptr);                                                                \
    ASSERT_AST_IS_TREE(source); /* round-trip reparse must stay a proper tree */                   \
    test_helpers.render_node_to_yaml_file(source, test_string + "_parsed.yaml");                   \
    test_helpers.render_node_to_dot_file(source, test_string + "_parsed.dot");                     \
                                                                                                   \
    /* Check that source is equal to source_ref */                                                 \
    ASSERT_TRUE(source->is_equal(*source_ref))

TEST(VerilogGeneratorTest, module0) { TEST_CORE; }
TEST(VerilogGeneratorTest, module1) { TEST_CORE; }
TEST(VerilogGeneratorTest, module2) { TEST_CORE; }
TEST(VerilogGeneratorTest, module3) { TEST_CORE; }
TEST(VerilogGeneratorTest, parameter0) { TEST_CORE; }
TEST(VerilogGeneratorTest, parameter1) { TEST_CORE; }
TEST(VerilogGeneratorTest, localparam0) { TEST_CORE; }
TEST(VerilogGeneratorTest, translate_off0) { TEST_CORE; }
TEST(VerilogGeneratorTest, translate_off1) { TEST_CORE; }
TEST(VerilogGeneratorTest, pragma0) { TEST_CORE; }
TEST(VerilogGeneratorTest, pragma1) { TEST_CORE; }
TEST(VerilogGeneratorTest, pragma2) { TEST_CORE; }
TEST(VerilogGeneratorTest, net0) { TEST_CORE; }
TEST(VerilogGeneratorTest, net1) { TEST_CORE; }
TEST(VerilogGeneratorTest, net2) { TEST_CORE; }
TEST(VerilogGeneratorTest, net3) { TEST_CORE; }
TEST(VerilogGeneratorTest, var0) { TEST_CORE; }
TEST(VerilogGeneratorTest, var1) { TEST_CORE; }
TEST(VerilogGeneratorTest, var2) { TEST_CORE; }
TEST(VerilogGeneratorTest, defparam0) { TEST_CORE; }
TEST(VerilogGeneratorTest, defparam1) { TEST_CORE; }
TEST(VerilogGeneratorTest, defparam2) { TEST_CORE; }
TEST(VerilogGeneratorTest, defparam3) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression0) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression1) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression2) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression3) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression4) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression5) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression6) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression7) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression8) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression9) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression10) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression11) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression12) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression13) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression14) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression15) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression16) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression17) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression18) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression19) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression20) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression21) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression22) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression23) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression24) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression25) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression26) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression27) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression28) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression29) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression30) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression31) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression32) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression33) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression34) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression35) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression36) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression37) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression38) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression39) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression40) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression41) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression42) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression43) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression44) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression45) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression46) { TEST_CORE; }
TEST(VerilogGeneratorTest, expression47) { TEST_CORE; }
TEST(VerilogGeneratorTest, intconst0) { TEST_CORE; }
TEST(VerilogGeneratorTest, intconst1) { TEST_CORE; }
TEST(VerilogGeneratorTest, intconst2) { TEST_CORE; }
TEST(VerilogGeneratorTest, intconst3) { TEST_CORE; }
TEST(VerilogGeneratorTest, intconst4) { TEST_CORE; }
TEST(VerilogGeneratorTest, intconst5) { TEST_CORE; }
TEST(VerilogGeneratorTest, intconst6) { TEST_CORE; }
TEST(VerilogGeneratorTest, intconst7) { TEST_CORE; }
TEST(VerilogGeneratorTest, intconst8) { TEST_CORE; }
TEST(VerilogGeneratorTest, intconst9) { TEST_CORE; }
TEST(VerilogGeneratorTest, realconst0) { TEST_CORE; }
TEST(VerilogGeneratorTest, realconst1) { TEST_CORE; }
TEST(VerilogGeneratorTest, realconst2) { TEST_CORE; }
TEST(VerilogGeneratorTest, realconst3) { TEST_CORE; }
TEST(VerilogGeneratorTest, stringconst0) { TEST_CORE; }
TEST(VerilogGeneratorTest, stringconst1) { TEST_CORE; }
TEST(VerilogGeneratorTest, stringconst2) { TEST_CORE; }
TEST(VerilogGeneratorTest, always0) { TEST_CORE; }
TEST(VerilogGeneratorTest, always1) { TEST_CORE; }
TEST(VerilogGeneratorTest, always2) { TEST_CORE; }
TEST(VerilogGeneratorTest, always3) { TEST_CORE; }
TEST(VerilogGeneratorTest, always4) { TEST_CORE; }
TEST(VerilogGeneratorTest, always5) { TEST_CORE; }
TEST(VerilogGeneratorTest, always6) { TEST_CORE; }
TEST(VerilogGeneratorTest, cond0) { TEST_CORE; }
TEST(VerilogGeneratorTest, if0) { TEST_CORE; }
TEST(VerilogGeneratorTest, if1) { TEST_CORE; }
TEST(VerilogGeneratorTest, if2) { TEST_CORE; }
TEST(VerilogGeneratorTest, if3) { TEST_CORE; }
TEST(VerilogGeneratorTest, if4) { TEST_CORE; }
TEST(VerilogGeneratorTest, case0) { TEST_CORE; }
TEST(VerilogGeneratorTest, case1) { TEST_CORE; }
TEST(VerilogGeneratorTest, casex0) { TEST_CORE; }
TEST(VerilogGeneratorTest, casez0) { TEST_CORE; }
TEST(VerilogGeneratorTest, for0) { TEST_CORE; }
TEST(VerilogGeneratorTest, for1) { TEST_CORE; }
TEST(VerilogGeneratorTest, while0) { TEST_CORE; }
TEST(VerilogGeneratorTest, while1) { TEST_CORE; }
TEST(VerilogGeneratorTest, event0) { TEST_CORE; }
TEST(VerilogGeneratorTest, event1) { TEST_CORE; }
TEST(VerilogGeneratorTest, wait0) { TEST_CORE; }
TEST(VerilogGeneratorTest, wait1) { TEST_CORE; }
TEST(VerilogGeneratorTest, forever0) { TEST_CORE; }
TEST(VerilogGeneratorTest, forever1) { TEST_CORE; }
TEST(VerilogGeneratorTest, initial0) { TEST_CORE; }
TEST(VerilogGeneratorTest, initial1) { TEST_CORE; }
TEST(VerilogGeneratorTest, block0) { TEST_CORE; }
TEST(VerilogGeneratorTest, namedblock0) { TEST_CORE; }
TEST(VerilogGeneratorTest, fork0) { TEST_CORE; }
TEST(VerilogGeneratorTest, assignment0) { TEST_CORE; }
TEST(VerilogGeneratorTest, assignment1) { TEST_CORE; }
TEST(VerilogGeneratorTest, disable0) { TEST_CORE; }
TEST(VerilogGeneratorTest, function0) { TEST_CORE; }
TEST(VerilogGeneratorTest, function1) { TEST_CORE; }
TEST(VerilogGeneratorTest, function2) { TEST_CORE; }
TEST(VerilogGeneratorTest, function3) { TEST_CORE; }
TEST(VerilogGeneratorTest, functioncall0) { TEST_CORE; }
TEST(VerilogGeneratorTest, task0) { TEST_CORE; }
TEST(VerilogGeneratorTest, task1) { TEST_CORE; }
TEST(VerilogGeneratorTest, task2) { TEST_CORE; }
TEST(VerilogGeneratorTest, empty_subprogram0) { TEST_CORE; }
TEST(VerilogGeneratorTest, taskcall0) { TEST_CORE; }
TEST(VerilogGeneratorTest, taskcall1) { TEST_CORE; }
TEST(VerilogGeneratorTest, instance0) { TEST_CORE; }
TEST(VerilogGeneratorTest, instance1) { TEST_CORE; }
TEST(VerilogGeneratorTest, instance2) { TEST_CORE; }
TEST(VerilogGeneratorTest, instance3) { TEST_CORE; }
TEST(VerilogGeneratorTest, instance4) { TEST_CORE; }
TEST(VerilogGeneratorTest, generate0) { TEST_CORE; }
TEST(VerilogGeneratorTest, generate1) { TEST_CORE; }
TEST(VerilogGeneratorTest, generate2) { TEST_CORE; }
TEST(VerilogGeneratorTest, generate3) { TEST_CORE; }
TEST(VerilogGeneratorTest, width0) { TEST_CORE; }
TEST(VerilogGeneratorTest, width1) { TEST_CORE; }

// SystemVerilog generator round-trip tests
TEST(VerilogGeneratorTest, sv_logic0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_always_ff0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_always_comb0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_always_latch0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_case0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_case1) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_param0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_for0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_pragma0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_generate0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_deadcode0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_io_normalizer0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_variable_folding0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_typedef_enum0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_typedef_enum1) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_typedef_struct0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_typedef_enum2) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_typedef_struct1) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_data_type_scalar0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_data_type_aggregate0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_data_type_named0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_data_type_member0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_data_type_funcbody0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_data_type_typedef0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_struct_multiname0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_signing0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_port_types0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_assign_pattern0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_cast0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_data_type_net0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_data_type_funcret0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_data_type_port0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_data_type_funcport0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_data_type_multiname0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_package0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_package_lifetime0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_package_empty0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_interface0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_interface1) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_interface_lifetime0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_interface_empty0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_modport0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_modport1) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_interface_port0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_interface_port1) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_import0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_import1) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_import_header0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_scoped_ref0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_typedef_alias0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_typedef_chain0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_scoped_type0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_unit_scope0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_scoped_call0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_scoped_taskcall0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_typed_port0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_user_types0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_func_void0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_enum_range0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_size_cast0) { TEST_CORE_SV; }
TEST(VerilogGeneratorTest, sv_type_op0) { TEST_CORE_SV; }
