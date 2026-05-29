// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti

// Cosim of the tiny_aes_128 pipelined design (originally from Homer Hsing,
// Apache-2.0). Drives the five {state, key} stimuli from the original
// test_aes_128.v testbench through the verilated, flattened model, then
// walks the pipeline and compares the outputs against the same golden
// vectors.

#include "cosim_helpers.hpp"

#include "Vaes_128.h"

#include <gtest/gtest.h>

#include <array>
#include <cstdint>

namespace
{

using namespace Veriparse::Test::Cosim;

using Word128 = std::array<uint32_t, 4>;

// Helper: build a 128-bit value from four 32-bit chunks expressed in MSB
// order, so test data lines up with the original Verilog literals
// (128'h3243f6a8_885a308d_313198a2_e0370734 -> {0x3243f6a8, ..., 0xe0370734}).
constexpr Word128 w128(uint32_t hi3, uint32_t hi2, uint32_t hi1, uint32_t hi0)
{
    return Word128{{hi0, hi1, hi2, hi3}};
}

// AES pipeline depth: 21 clocks between { state, key } presented at the
// inputs and the matching value on `out` (one register before the rounds,
// 10 rounds, plus the final round register).
constexpr int kPipelineDepth = 21;

TEST(TinyAes128Cosim, PipelineMatchesGoldenVectors)
{
    Vaes_128 dut;

    const Word128 stim_state[5] = {
        w128(0x3243f6a8, 0x885a308d, 0x313198a2, 0xe0370734),
        w128(0x00112233, 0x44556677, 0x8899aabb, 0xccddeeff),
        w128(0, 0, 0, 0),
        w128(0, 0, 0, 0),
        w128(0, 0, 0, 1),
    };
    const Word128 stim_key[5] = {
        w128(0x2b7e1516, 0x28aed2a6, 0xabf71588, 0x09cf4f3c),
        w128(0x00010203, 0x04050607, 0x08090a0b, 0x0c0d0e0f),
        w128(0, 0, 0, 0),
        w128(0, 0, 0, 1),
        w128(0, 0, 0, 0),
    };
    const Word128 expected_out[5] = {
        w128(0x3925841d, 0x02dc09fb, 0xdc118597, 0x196a0b32),
        w128(0x69c4e0d8, 0x6a7b0430, 0xd8cdb780, 0x70b4c55a),
        w128(0x66e94bd4, 0xef8a2c3b, 0x884cfa59, 0xca342b2e),
        w128(0x0545aad5, 0x6da2a97c, 0x3663d143, 0x2a3d1c84),
        w128(0x58e2fcce, 0xfa7e3061, 0x367f1d57, 0xa4e7455a),
    };

    dut.clk = 0;
    wide_assign(dut.state, Word128{{0, 0, 0, 0}});
    wide_assign(dut.key, Word128{{0, 0, 0, 0}});
    dut.eval();

    for(int i = 0; i < 5; ++i) {
        wide_assign(dut.state, stim_state[i]);
        wide_assign(dut.key, stim_key[i]);
        tick(dut, [&](int v) { dut.clk = v; });
    }

    // Drain the pipeline up to the first valid output.
    for(int i = 0; i < kPipelineDepth - 5; ++i) {
        tick(dut, [&](int v) { dut.clk = v; });
    }

    for(int i = 0; i < 5; ++i) {
        EXPECT_TRUE(wide_eq(dut.out, expected_out[i]))
            << "Stimulus #" << i << " expected " << to_hex(expected_out[i]) << " got "
            << to_hex(dut.out);
        if(i < 4) {
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
