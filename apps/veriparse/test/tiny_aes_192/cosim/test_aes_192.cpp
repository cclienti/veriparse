// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti

// Cosim of the tiny_aes_192 pipelined design (originally from Homer Hsing,
// Apache-2.0). Same shape as the AES-128 harness, scaled to a 192-bit key
// and 25-cycle pipeline depth.

#include "cosim_helpers.hpp"

#include "Vaes_192.h"

#include <gtest/gtest.h>

#include <array>
#include <cstdint>

namespace
{

using namespace Veriparse::Test::Cosim;

using Word128 = std::array<uint32_t, 4>;
using Word192 = std::array<uint32_t, 6>;

constexpr Word128 w128(uint32_t hi3, uint32_t hi2, uint32_t hi1, uint32_t hi0)
{
    return Word128{{hi0, hi1, hi2, hi3}};
}

constexpr Word192 w192(uint32_t hi5, uint32_t hi4, uint32_t hi3, uint32_t hi2, uint32_t hi1,
                       uint32_t hi0)
{
    return Word192{{hi0, hi1, hi2, hi3, hi4, hi5}};
}

// Inferred from test_aes_192.v: stim 1 at t=112ns, first check at t=362ns,
// clock period 10ns => 25 clocks between input and matching output.
constexpr int kPipelineDepth = 25;

TEST(TinyAes192Cosim, PipelineMatchesGoldenVectors)
{
    Vaes_192 dut;

    const Word128 stim_state[3] = {
        w128(0x3243f6a8, 0x885a308d, 0x313198a2, 0xe0370734),
        w128(0x00112233, 0x44556677, 0x8899aabb, 0xccddeeff),
        w128(0, 0, 0, 0),
    };
    const Word192 stim_key[3] = {
        w192(0x2b7e1516, 0x28aed2a6, 0xabf71588, 0x09cf4f3c, 0x762e7160, 0xf38b4da5),
        w192(0x00010203, 0x04050607, 0x08090a0b, 0x0c0d0e0f, 0x10111213, 0x14151617),
        w192(0, 0, 0, 0, 0, 0),
    };

    // Only outputs 0 and 1 are checked by the original testbench.
    const Word128 expected_out[2] = {
        w128(0xf9fb29ae, 0xfc384a25, 0x0340d833, 0xb87ebc00),
        w128(0xdda97ca4, 0x864cdfe0, 0x6eaf70a0, 0xec0d7191),
    };

    dut.clk = 0;
    wide_assign(dut.state, Word128{{0, 0, 0, 0}});
    wide_assign(dut.key, Word192{{0, 0, 0, 0, 0, 0}});
    dut.eval();

    for(int i = 0; i < 3; ++i) {
        wide_assign(dut.state, stim_state[i]);
        wide_assign(dut.key, stim_key[i]);
        tick(dut, [&](int v) { dut.clk = v; });
    }

    for(int i = 0; i < kPipelineDepth - 3; ++i) {
        tick(dut, [&](int v) { dut.clk = v; });
    }

    for(int i = 0; i < 2; ++i) {
        EXPECT_TRUE(wide_eq(dut.out, expected_out[i]))
            << "Stimulus #" << i << " expected " << to_hex(expected_out[i]) << " got "
            << to_hex(dut.out);
        if(i < 1) {
            tick(dut, [&](int v) { dut.clk = v; });
        }
    }

    dut.final();
}

} // namespace

int main(int argc, char **argv)
{
    Verilated::commandArgs(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
