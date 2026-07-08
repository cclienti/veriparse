// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include "../../helpers/helpers.hpp"
#include <veriparse/parser/verilog.hpp>
#include <veriparse/passes/analysis/synthesizable_check.hpp>
#include <veriparse/logger/logger.hpp>
#include <gtest/gtest.h>

using namespace Veriparse;

static TestHelpers test_helpers("lib/test/passes/analysis/testcases/");

#define PARSE_SV_SOURCE                                                                            \
    ENABLE_LOGGER;                                                                                 \
                                                                                                   \
    Parser::Verilog verilog;                                                                       \
    verilog.set_sv_mode(true);                                                                     \
    verilog.parse(test_helpers.get_sv_filename(test_name));                                        \
    AST::Node::Ptr source = verilog.get_source();                                                  \
    ASSERT_TRUE(source != nullptr)

// A virtual interface (§25.9) is not synthesizable: the check must reject it.
TEST(PassesAnalysis_SynthesizableCheck, synthesizable_check_virtual0)
{
    PARSE_SV_SOURCE;
    ASSERT_NE(0, Passes::Analysis::SynthesizableCheck::check(source));
}

// A non-virtual interface (modport-qualified port) stays inside the
// synthesizable subset: the check must accept it.
TEST(PassesAnalysis_SynthesizableCheck, synthesizable_check_ok0)
{
    PARSE_SV_SOURCE;
    ASSERT_EQ(0, Passes::Analysis::SynthesizableCheck::check(source));
}
