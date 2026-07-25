// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti

// Cosim of the packed-struct pipeline: a C++ model computes the header
// fields from their §7.2.1 offsets (kind [15:12], flags [11:10], len [9:0])
// and the checker's union-based score, so the flattened part-select
// rewrites are checked bit-exactly cycle by cycle.

#include "cosim_helpers.hpp"

#include "Vstruct_pipe.h"

#include <gtest/gtest.h>

#include <cstdint>

namespace
{

using namespace Veriparse::Test::Cosim;

TEST(StructPipeCosim, HeaderFieldsAndUnionView)
{
    Vstruct_pipe dut;

    dut.clk = 0;
    dut.rst = 1;
    dut.raw = 0;
    dut.eval();

    for(int i = 0; i < 2; ++i) {
        tick(dut, [&](int v) { dut.clk = v; });
    }
    dut.rst = 0;

    uint16_t hdr = 0;
    uint16_t stimulus = 0xbeefu;
    for(int cycle = 0; cycle < 64; ++cycle) {
        dut.raw = stimulus;
        dut.eval();

        tick(dut, [&](int v) { dut.clk = v; });
        hdr = stimulus; // pack is a field-wise identity re-pack

        const uint16_t kind = (hdr >> 12) & 0xf;
        const uint16_t flags = (hdr >> 10) & 0x3;
        const uint16_t len = hdr & 0x3ff;
        const uint16_t score = (len ^ kind ^ flags) & 0x3ff;

        dut.eval();
        ASSERT_EQ(hdr, dut.hdr_bits) << "cycle " << cycle;
        ASSERT_EQ(score, dut.score) << "cycle " << cycle;

        stimulus ^= static_cast<uint16_t>(stimulus << 7);
        stimulus ^= static_cast<uint16_t>(stimulus >> 9);
        stimulus ^= static_cast<uint16_t>(stimulus << 8);
    }
}

} // namespace
