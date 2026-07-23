// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti

// Cosim of the type-parameterized accumulator pipeline: each lane's
// accumulator width comes from a `parameter type` binding (keyword
// override, typedef actual, default), so a cycle-accurate C++ model at
// 16/12/8 bits checks that the flattened widths truncate identically.

#include "cosim_helpers.hpp"

#include "Vtype_param_pipe.h"

#include <gtest/gtest.h>

#include <cstdint>

namespace
{

using namespace Veriparse::Test::Cosim;

TEST(TypeParamPipeCosim, AccumulatorWidths)
{
    Vtype_param_pipe dut;

    dut.clk = 0;
    dut.rst = 1;
    dut.a = 0;
    dut.eval();

    for(int i = 0; i < 2; ++i) {
        tick(dut, [&](int v) { dut.clk = v; });
    }
    dut.rst = 0;

    uint16_t wide = 0;
    uint16_t mid = 0;
    uint8_t narrow = 0;

    uint16_t stimulus = 0xace1u;
    for(int cycle = 0; cycle < 64; ++cycle) {
        dut.a = stimulus;
        dut.eval();

        tick(dut, [&](int v) { dut.clk = v; });

        wide = static_cast<uint16_t>(wide + stimulus);
        mid = static_cast<uint16_t>((mid + (stimulus & 0x0fffu)) & 0x0fffu);
        narrow = static_cast<uint8_t>(narrow + (stimulus & 0xffu));

        ASSERT_EQ(wide, dut.wide_acc) << "cycle " << cycle;
        ASSERT_EQ(mid, dut.mid_acc) << "cycle " << cycle;
        ASSERT_EQ(narrow, dut.narrow_acc) << "cycle " << cycle;

        // A xorshift step keeps the stimulus varied and reproducible.
        stimulus ^= static_cast<uint16_t>(stimulus << 7);
        stimulus ^= static_cast<uint16_t>(stimulus >> 9);
        stimulus ^= static_cast<uint16_t>(stimulus << 8);
    }
}

} // namespace
