// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti

// Cosim of the interface-based valid/ready handshake design: a cycle-accurate
// C++ model of the producer/consumer pair checks acc/count each cycle, and
// the 4-lane interface-array mixer is checked combinationally.

#include "cosim_helpers.hpp"

#include "Viface_handshake.h"

#include <gtest/gtest.h>

#include <cstdint>

namespace
{

using namespace Veriparse::Test::Cosim;

TEST(IfaceHandshakeCosim, ProducerConsumerAndLanes)
{
    Viface_handshake dut;

    dut.clk = 0;
    dut.rst = 1;
    dut.seeds = 0xd00dfeedu;
    dut.eval();

    for(int i = 0; i < 2; ++i) {
        tick(dut, [&](int v) { dut.clk = v; });
    }
    dut.rst = 0;

    // The lane mixer is combinational: mix = seeds[7:0] ^ seeds[15:8] ^
    // seeds[23:16] ^ seeds[31:24].
    const uint32_t seeds = 0xd00dfeedu;
    const uint8_t expected_mix = static_cast<uint8_t>(
        (seeds & 0xff) ^ ((seeds >> 8) & 0xff) ^ ((seeds >> 16) & 0xff) ^ ((seeds >> 24) & 0xff));
    dut.eval();
    EXPECT_EQ(expected_mix, dut.mix);

    // Cycle-accurate model of the handshake: valid is constant 1, ready
    // toggles, fire = valid & ready, and counter/acc/count update together
    // on fire (nonblocking semantics).
    uint16_t counter = 0;
    uint16_t acc = 0;
    uint8_t count = 0;
    bool toggle = false;

    for(int cycle = 0; cycle < 40; ++cycle) {
        const bool fire = toggle;
        tick(dut, [&](int v) { dut.clk = v; });
        if(fire) {
            acc = static_cast<uint16_t>(acc ^ counter);
            count = static_cast<uint8_t>(count + 1);
            counter = static_cast<uint16_t>(counter + 1);
        }
        toggle = !toggle;

        ASSERT_EQ(acc, dut.acc) << "cycle " << cycle;
        ASSERT_EQ(count, dut.count) << "cycle " << cycle;
    }

    EXPECT_EQ(expected_mix, dut.mix);

    dut.final();
}

} // namespace

int main(int argc, char **argv)
{
    Verilated::commandArgs(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
