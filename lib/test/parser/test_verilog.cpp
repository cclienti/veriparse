// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include "../helpers/helpers.hpp"
#include <veriparse/parser/verilog.hpp>
#include <veriparse/logger/logger.hpp>
#include <veriparse/generators/yaml_generator.hpp>
#include <veriparse/generators/dot_generator.hpp>
#include <veriparse/importers/yaml_importer.hpp>
#include <gtest/gtest.h>
#include <fstream>
#include <string>

using namespace Veriparse;

static TestHelpers test_helpers("lib/test/parser/testcases/");

#define TEST_CORE                                                                                  \
    ENABLE_LOGGER;                                                                                 \
                                                                                                   \
    /* Parse the module */                                                                         \
    Parser::Verilog verilog;                                                                       \
    verilog.parse(test_helpers.get_verilog_filename(test_name));                                   \
    AST::Node::Ptr source = verilog.get_source();                                                  \
    ASSERT_TRUE(source != nullptr);                                                                \
    ASSERT_AST_IS_TREE(source); /* no AST node shared between two parents */                       \
    test_helpers.render_node_to_yaml_file(source, test_string + "_parsed.yaml");                   \
    test_helpers.render_node_to_dot_file(source, test_string + "_parsed.dot");                     \
                                                                                                   \
    /* Full cloning and is_equal test */                                                           \
    AST::Node::Ptr source_cloned = source->clone();                                                \
    ASSERT_TRUE(source->is_equal(source_cloned));                                                  \
    test_helpers.render_node_to_dot_file(source_cloned, test_string + "_cloned.dot");              \
    test_helpers.render_node_to_yaml_file(source_cloned, test_string + "_cloned.yaml");            \
                                                                                                   \
    /* Import the YAML reference */                                                                \
    AST::Node::Ptr source_ref =                                                                    \
        Importers::YAMLImporter().import(test_helpers.get_yaml_filename(test_name).c_str());       \
    ASSERT_TRUE(source_ref != nullptr);                                                            \
                                                                                                   \
    /* Check parsed against reference */                                                           \
    ASSERT_TRUE(source->is_equal(*source_ref, false))

#define TEST_CORE_SV                                                                               \
    ENABLE_LOGGER;                                                                                 \
                                                                                                   \
    /* Parse the module in SV mode */                                                              \
    Parser::Verilog verilog;                                                                       \
    verilog.set_sv_mode(true);                                                                     \
    verilog.parse(test_helpers.get_sv_filename(test_name));                                        \
    AST::Node::Ptr source = verilog.get_source();                                                  \
    ASSERT_TRUE(source != nullptr);                                                                \
    ASSERT_AST_IS_TREE(source); /* no AST node shared between two parents */                       \
    test_helpers.render_node_to_yaml_file(source, test_string + "_parsed.yaml");                   \
    test_helpers.render_node_to_dot_file(source, test_string + "_parsed.dot");                     \
                                                                                                   \
    /* Full cloning and is_equal test */                                                           \
    AST::Node::Ptr source_cloned = source->clone();                                                \
    ASSERT_TRUE(source->is_equal(source_cloned));                                                  \
    test_helpers.render_node_to_dot_file(source_cloned, test_string + "_cloned.dot");              \
    test_helpers.render_node_to_yaml_file(source_cloned, test_string + "_cloned.yaml");            \
                                                                                                   \
    /* Import the YAML reference */                                                                \
    AST::Node::Ptr source_ref =                                                                    \
        Importers::YAMLImporter().import(test_helpers.get_yaml_filename(test_name).c_str());       \
    ASSERT_TRUE(source_ref != nullptr);                                                            \
                                                                                                   \
    /* Check parsed against reference */                                                           \
    ASSERT_TRUE(source->is_equal(*source_ref, false))

TEST(VerilogParserTest, module0) { TEST_CORE; }
TEST(VerilogParserTest, module1) { TEST_CORE; }
TEST(VerilogParserTest, module2) { TEST_CORE; }
TEST(VerilogParserTest, module3) { TEST_CORE; }
TEST(VerilogParserTest, localparam0) { TEST_CORE; }
TEST(VerilogParserTest, parameter0) { TEST_CORE; }
TEST(VerilogParserTest, parameter1) { TEST_CORE; }
TEST(VerilogParserTest, defparam0) { TEST_CORE; }
TEST(VerilogParserTest, defparam1) { TEST_CORE; }
TEST(VerilogParserTest, defparam2) { TEST_CORE; }
TEST(VerilogParserTest, defparam3) { TEST_CORE; }
TEST(VerilogParserTest, translate_off0) { TEST_CORE; }
TEST(VerilogParserTest, translate_off1) { TEST_CORE; }
TEST(VerilogParserTest, pragma0) { TEST_CORE; }
TEST(VerilogParserTest, pragma1) { TEST_CORE; }
TEST(VerilogParserTest, pragma2) { TEST_CORE; }
TEST(VerilogParserTest, net0) { TEST_CORE; }
TEST(VerilogParserTest, net1) { TEST_CORE; }
TEST(VerilogParserTest, net2) { TEST_CORE; }
TEST(VerilogParserTest, net3) { TEST_CORE; }
TEST(VerilogParserTest, var0) { TEST_CORE; }
TEST(VerilogParserTest, var1) { TEST_CORE; }
TEST(VerilogParserTest, var2) { TEST_CORE; }
TEST(VerilogParserTest, var3) { TEST_CORE; }
TEST(VerilogParserTest, expression0) { TEST_CORE; }
TEST(VerilogParserTest, expression1) { TEST_CORE; }
TEST(VerilogParserTest, expression2) { TEST_CORE; }
TEST(VerilogParserTest, expression3) { TEST_CORE; }
TEST(VerilogParserTest, expression4) { TEST_CORE; }
TEST(VerilogParserTest, expression5) { TEST_CORE; }
TEST(VerilogParserTest, expression6) { TEST_CORE; }
TEST(VerilogParserTest, expression7) { TEST_CORE; }
TEST(VerilogParserTest, expression8) { TEST_CORE; }
TEST(VerilogParserTest, expression9) { TEST_CORE; }
TEST(VerilogParserTest, expression10) { TEST_CORE; }
TEST(VerilogParserTest, expression11) { TEST_CORE; }
TEST(VerilogParserTest, expression12) { TEST_CORE; }
TEST(VerilogParserTest, expression13) { TEST_CORE; }
TEST(VerilogParserTest, expression14) { TEST_CORE; }
TEST(VerilogParserTest, expression15) { TEST_CORE; }
TEST(VerilogParserTest, expression16) { TEST_CORE; }
TEST(VerilogParserTest, expression17) { TEST_CORE; }
TEST(VerilogParserTest, expression18) { TEST_CORE; }
TEST(VerilogParserTest, expression19) { TEST_CORE; }
TEST(VerilogParserTest, expression20) { TEST_CORE; }
TEST(VerilogParserTest, expression21) { TEST_CORE; }
TEST(VerilogParserTest, expression22) { TEST_CORE; }
TEST(VerilogParserTest, expression23) { TEST_CORE; }
TEST(VerilogParserTest, expression24) { TEST_CORE; }
TEST(VerilogParserTest, expression25) { TEST_CORE; }
TEST(VerilogParserTest, expression26) { TEST_CORE; }
TEST(VerilogParserTest, expression27) { TEST_CORE; }
TEST(VerilogParserTest, expression28) { TEST_CORE; }
TEST(VerilogParserTest, expression29) { TEST_CORE; }
TEST(VerilogParserTest, expression30) { TEST_CORE; }
TEST(VerilogParserTest, expression31) { TEST_CORE; }
TEST(VerilogParserTest, expression32) { TEST_CORE; }
TEST(VerilogParserTest, expression33) { TEST_CORE; }
TEST(VerilogParserTest, expression34) { TEST_CORE; }
TEST(VerilogParserTest, expression35) { TEST_CORE; }
TEST(VerilogParserTest, expression36) { TEST_CORE; }
TEST(VerilogParserTest, expression37) { TEST_CORE; }
TEST(VerilogParserTest, expression38) { TEST_CORE; }
TEST(VerilogParserTest, expression39) { TEST_CORE; }
TEST(VerilogParserTest, expression40) { TEST_CORE; }
TEST(VerilogParserTest, expression41) { TEST_CORE; }
TEST(VerilogParserTest, expression42) { TEST_CORE; }
TEST(VerilogParserTest, expression43) { TEST_CORE; }
TEST(VerilogParserTest, expression44) { TEST_CORE; }
TEST(VerilogParserTest, expression45) { TEST_CORE; }
TEST(VerilogParserTest, expression46) { TEST_CORE; }
TEST(VerilogParserTest, expression47) { TEST_CORE; }
TEST(VerilogParserTest, intconst0) { TEST_CORE; }
TEST(VerilogParserTest, intconst1) { TEST_CORE; }
TEST(VerilogParserTest, intconst2) { TEST_CORE; }
TEST(VerilogParserTest, intconst3) { TEST_CORE; }
TEST(VerilogParserTest, intconst4) { TEST_CORE; }
TEST(VerilogParserTest, intconst5) { TEST_CORE; }
TEST(VerilogParserTest, intconst6) { TEST_CORE; }
TEST(VerilogParserTest, intconst7) { TEST_CORE; }
TEST(VerilogParserTest, intconst8) { TEST_CORE; }
TEST(VerilogParserTest, intconst9) { TEST_CORE; }
TEST(VerilogParserTest, realconst0) { TEST_CORE; }
TEST(VerilogParserTest, realconst1) { TEST_CORE; }
TEST(VerilogParserTest, realconst2) { TEST_CORE; }
TEST(VerilogParserTest, realconst3) { TEST_CORE; }
TEST(VerilogParserTest, stringconst0) { TEST_CORE; }
TEST(VerilogParserTest, stringconst1) { TEST_CORE; }
TEST(VerilogParserTest, stringconst2) { TEST_CORE; }
TEST(VerilogParserTest, always0) { TEST_CORE; }
TEST(VerilogParserTest, always1) { TEST_CORE; }
TEST(VerilogParserTest, always2) { TEST_CORE; }
TEST(VerilogParserTest, always3) { TEST_CORE; }
TEST(VerilogParserTest, always4) { TEST_CORE; }
TEST(VerilogParserTest, always5) { TEST_CORE; }
TEST(VerilogParserTest, always6) { TEST_CORE; }
TEST(VerilogParserTest, cond0) { TEST_CORE; }
TEST(VerilogParserTest, if0) { TEST_CORE; }
TEST(VerilogParserTest, if1) { TEST_CORE; }
TEST(VerilogParserTest, if2) { TEST_CORE; }
TEST(VerilogParserTest, if3) { TEST_CORE; }
TEST(VerilogParserTest, if4) { TEST_CORE; }
TEST(VerilogParserTest, case0) { TEST_CORE; }
TEST(VerilogParserTest, case1) { TEST_CORE; }
TEST(VerilogParserTest, casex0) { TEST_CORE; }
TEST(VerilogParserTest, casez0) { TEST_CORE; }
TEST(VerilogParserTest, for0) { TEST_CORE; }
TEST(VerilogParserTest, for1) { TEST_CORE; }
TEST(VerilogParserTest, while0) { TEST_CORE; }
TEST(VerilogParserTest, while1) { TEST_CORE; }
TEST(VerilogParserTest, event0) { TEST_CORE; }
TEST(VerilogParserTest, event1) { TEST_CORE; }
TEST(VerilogParserTest, wait0) { TEST_CORE; }
TEST(VerilogParserTest, wait1) { TEST_CORE; }
TEST(VerilogParserTest, forever0) { TEST_CORE; }
TEST(VerilogParserTest, forever1) { TEST_CORE; }
TEST(VerilogParserTest, initial0) { TEST_CORE; }
TEST(VerilogParserTest, initial1) { TEST_CORE; }
TEST(VerilogParserTest, block0) { TEST_CORE; }
TEST(VerilogParserTest, block1) { TEST_CORE; }
TEST(VerilogParserTest, block2) { TEST_CORE; }
TEST(VerilogParserTest, namedblock0) { TEST_CORE; }
TEST(VerilogParserTest, fork0) { TEST_CORE; }
TEST(VerilogParserTest, assignment0) { TEST_CORE; }
TEST(VerilogParserTest, assignment1) { TEST_CORE; }
TEST(VerilogParserTest, disable0) { TEST_CORE; }
TEST(VerilogParserTest, function0) { TEST_CORE; }
TEST(VerilogParserTest, function1) { TEST_CORE; }
TEST(VerilogParserTest, function2) { TEST_CORE; }
TEST(VerilogParserTest, function3) { TEST_CORE; }
TEST(VerilogParserTest, functioncall0) { TEST_CORE; }
TEST(VerilogParserTest, task0) { TEST_CORE; }
TEST(VerilogParserTest, task1) { TEST_CORE; }
TEST(VerilogParserTest, task2) { TEST_CORE; }
TEST(VerilogParserTest, empty_subprogram0) { TEST_CORE; }
TEST(VerilogParserTest, taskcall0) { TEST_CORE; }
TEST(VerilogParserTest, taskcall1) { TEST_CORE; }
TEST(VerilogParserTest, systemcall0) { TEST_CORE; }
TEST(VerilogParserTest, instance0) { TEST_CORE; }
TEST(VerilogParserTest, instance1) { TEST_CORE; }
TEST(VerilogParserTest, instance2) { TEST_CORE; }
TEST(VerilogParserTest, instance3) { TEST_CORE; }
TEST(VerilogParserTest, instance4) { TEST_CORE; }
TEST(VerilogParserTest, instance5) { TEST_CORE; }
TEST(VerilogParserTest, instance6) { TEST_CORE; }
TEST(VerilogParserTest, instance7) { TEST_CORE; }
TEST(VerilogParserTest, instance8) { TEST_CORE; }
TEST(VerilogParserTest, instance9) { TEST_CORE; }
TEST(VerilogParserTest, generate0) { TEST_CORE; }
TEST(VerilogParserTest, generate1) { TEST_CORE; }
TEST(VerilogParserTest, generate2) { TEST_CORE; }
TEST(VerilogParserTest, generate3) { TEST_CORE; }
TEST(VerilogParserTest, generate4) { TEST_CORE; }
TEST(VerilogParserTest, width0) { TEST_CORE; }
TEST(VerilogParserTest, width1) { TEST_CORE; }
TEST(VerilogParserTest, bidir0) { TEST_CORE; }

// Preprocessor directives test
TEST(VerilogParserTest, vpp_line0) { TEST_CORE; }
TEST(VerilogParserTest, vpp_default_nettype0) { TEST_CORE; }
TEST(VerilogParserTest, vpp_default_nettype1) { TEST_CORE; }
TEST(VerilogParserTest, vpp_default_nettype2) { TEST_CORE; }
TEST(VerilogParserTest, vpp_default_nettype3) { TEST_CORE; }
TEST(VerilogParserTest, vpp_default_nettype4) { TEST_CORE; }
TEST(VerilogParserTest, vpp_default_nettype10) { TEST_CORE; }
TEST(VerilogParserTest, vpp_default_nettype8) { TEST_CORE; } // trireg
TEST(VerilogParserTest, vpp_default_nettype9) { TEST_CORE; } // wand

// reg/integer/real are NOT valid `default_nettype` values (IEEE 1364-2005 §19.2;
// the old INTEGER/REAL/REG enum was dropped, ADR-0001 §9.6). The scanner rejects
// them — the parse fails rather than silently treating them as `none`.
// (testcases 5/6/7 are `default_nettype reg|real|integer`.)
TEST(VerilogParserErrorTest, vpp_default_nettype_reg)
{
    ENABLE_LOGGER;
    Parser::Verilog verilog;
    ASSERT_NE(verilog.parse(test_helpers.get_verilog_filename("vpp_default_nettype5")), 0);
}
TEST(VerilogParserErrorTest, vpp_default_nettype_real)
{
    ENABLE_LOGGER;
    Parser::Verilog verilog;
    ASSERT_NE(verilog.parse(test_helpers.get_verilog_filename("vpp_default_nettype6")), 0);
}
TEST(VerilogParserErrorTest, vpp_default_nettype_integer)
{
    ENABLE_LOGGER;
    Parser::Verilog verilog;
    ASSERT_NE(verilog.parse(test_helpers.get_verilog_filename("vpp_default_nettype7")), 0);
}

// SystemVerilog tests
TEST(VerilogParserTest, sv_logic0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_always_ff0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_always_comb0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_always_latch0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_case0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_case1) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_param0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_for0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_pragma0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_generate0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_deadcode0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_io_normalizer0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_variable_folding0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_package0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_package_lifetime0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_package_empty0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_interface0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_interface1) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_interface_lifetime0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_interface_empty0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_import0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_import1) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_import_header0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_scoped_ref0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_typedef_alias0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_typedef_chain0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_typedef_struct0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_typedef_struct1) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_typedef_enum0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_typedef_enum1) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_typedef_enum2) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_data_type_scalar0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_data_type_aggregate0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_data_type_named0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_data_type_member0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_data_type_funcbody0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_data_type_typedef0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_struct_multiname0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_signing0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_port_types0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_assign_pattern0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_cast0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_data_type_net0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_data_type_funcret0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_data_type_port0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_data_type_funcport0) { TEST_CORE_SV; }

// [const] [var] data declaration qualifiers, with explicit and implicit
// (ImplicitType) data types, wrapped in DataModifier (IEEE 1800-2017 §6.8).
TEST(VerilogParserTest, sv_var0) { TEST_CORE_SV; }

// [lifetime] qualifier (static/automatic) on a data declaration, stacking with
// const/var, carried by DataModifier.lifetime (IEEE 1800-2017 §6.21).
TEST(VerilogParserTest, sv_lifetime0) { TEST_CORE_SV; }

// [const][var] on named/scoped types (CustomTypeVar in a DataModifier) plus the
// factored implicit branch (`var g, h`, `var i [2:0]`).
TEST(VerilogParserTest, sv_var_named0) { TEST_CORE_SV; }

// Parser-local semantic checks on data declaration qualifiers (IEEE 1800-2017
// 6.8): an omitted data type requires `var`, and `const` requires an
// initializer. These are locally decidable, so the parser rejects them
// (error -> exit(1)) rather than deferring to an analysis pass.
TEST(VerilogParserErrorTest, sv_implicit_requires_var)
{
    ENABLE_LOGGER;
    Parser::Verilog verilog;
    verilog.set_sv_mode(true);
    ASSERT_EXIT(verilog.parse(test_helpers.get_sv_filename("sv_err_implicit_no_var0")),
                ::testing::ExitedWithCode(1), "requires the 'var' keyword");
}

TEST(VerilogParserErrorTest, sv_const_requires_initializer)
{
    ENABLE_LOGGER;
    Parser::Verilog verilog;
    verilog.set_sv_mode(true);
    ASSERT_EXIT(verilog.parse(test_helpers.get_sv_filename("sv_err_const_no_init0")),
                ::testing::ExitedWithCode(1), "'const' variable shall have an initializer");
}

TEST(VerilogParserErrorTest, sv_const_named_requires_initializer)
{
    ENABLE_LOGGER;
    Parser::Verilog verilog;
    verilog.set_sv_mode(true);
    ASSERT_EXIT(verilog.parse(test_helpers.get_sv_filename("sv_err_const_named_no_init0")),
                ::testing::ExitedWithCode(1), "'const' variable shall have an initializer");
}

// A packed dimension admits only a range `[msb:lsb]` (or unsized `[]`), never the
// single-size `[N]` form (IEEE 1800-2017 7.4.1, ADR-0001 §3.8). The `[N]` after a
// named type (`var my_t [4] x`) is captured via `lengths`, which also accepts
// `[N]`; the parser rejects it rather than building an illegal packed dim.
TEST(VerilogParserErrorTest, sv_packed_dim_rejects_size)
{
    ENABLE_LOGGER;
    Parser::Verilog verilog;
    verilog.set_sv_mode(true);
    ASSERT_EXIT(verilog.parse(test_helpers.get_sv_filename("sv_err_packed_size_dim0")),
                ::testing::ExitedWithCode(1), "packed dimension shall be a range");
}

// Focused multi-name node-sharing case (bit a,b + named e,f + inherited ports
// p,q). The no-node-sharing tree invariant is now checked by TEST_CORE/
// TEST_CORE_SV for *every* testcase; this just keeps a dedicated case for it.
TEST(VerilogParserTest, sv_data_type_multiname0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_scoped_type0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_unit_scope0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_scoped_call0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_scoped_taskcall0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_typed_port0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_port_signing0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_func_void0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_enum_range0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_size_cast0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_type_op0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_user_types0) { TEST_CORE_SV; }
TEST(VerilogParserTest, sv_jump0) { TEST_CORE_SV; }
