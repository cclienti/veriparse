// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti

// Cosim of the double_fpu-master design (David Lundgren's double-precision
// FPU). Replays the 50 test vectors from fpu_TB.v: each one drives
// opa/opb/fpu_op/rmode with enable high for 2 cycles, then keeps enable
// low for 80 cycles before reading 'out' and comparing.

#include "cosim_helpers.hpp"

#include "Vfpu.h"

#include <gtest/gtest.h>

#include <cstdint>

namespace
{

using namespace Veriparse::Test::Cosim;

struct Vector
{
    uint64_t opa;
    uint64_t opb;
    uint8_t fpu_op; // 3 bits
    uint8_t rmode;  // 2 bits
    uint64_t expected_out;
};

// Inferred from fpu_TB.v stim cadence: #20000ps with enable=1, #800000ps
// with enable=0, clock period 10000ps (`always #5000 clk = ~clk`).
constexpr int kCyclesEnableHigh = 2;
constexpr int kCyclesEnableLow = 80;

constexpr Vector kVectors[] = {
    {0x00000000cd1706a2ull, 0x01c56e1fc2f8f359ull, 3u, 0u, 0x3cf323ea98d06fb6ull},
    {0x03d2b6044942f855ull, 0x3bac558e0f15e8f7ull, 2u, 2u, 0x000010914a4c025aull},
    {0x407d100000000000ull, 0x4050400000000000ull, 1u, 0u, 0x4079000000000000ull},
    {0x3fcd0e5604189375ull, 0x4041000000000000ull, 0u, 2u, 0x40411d0e56041894ull},
    {0x406be00000000000ull, 0x404c000000000000ull, 3u, 0u, 0x400fdb6db6db6db7ull},
    {0xc057c00000000000ull, 0x4069000000000000ull, 2u, 0u, 0xc0d28e0000000000ull},
    {0xc046800000000000ull, 0xc040000000000000ull, 1u, 3u, 0xc02a000000000000ull},
    {0xc08c380000000000ull, 0x4035000000000000ull, 0u, 0u, 0xc08b900000000000ull},
    {0x407c700000000000ull, 0xc07cb00000000000ull, 3u, 0u, 0xbfefb89c2a6346d5ull},
    {0x406d78a3d70a3d71ull, 0x3f947ae147ae147bull, 2u, 2u, 0x4012dc91d14e3bceull},
    {0x40791f3333333333ull, 0xc040cccccccccccdull, 1u, 3u, 0x407b38ccccccccccull},
    {0x8000b8157268fdaeull, 0x0000ca7dfdd9e3d9ull, 0u, 2u, 0x000012688b70e62bull},
    {0x16887e92154ef7acull, 0x4069000000000000ull, 3u, 0u, 0x160f5a549627a36cull},
    {0x4435af1d78b58c40ull, 0x4069000000000000ull, 3u, 0u, 0x43bbc16d674ec800ull},
    {0x4014000000000000ull, 0x4004000000000000ull, 3u, 3u, 0x4000000000000000ull},
    {0x0000002f201d49fbull, 0x3ff0000000000000ull, 3u, 2u, 0x0000002f201d49fbull},
    {0x40e7ece000000000ull, 0x403799999999999aull, 1u, 2u, 0x40e7e9eccccccccdull},
    {0x16887e92154ef7acull, 0x5713f58d434a2baeull, 3u, 2u, 0x0000027456dbda6dull},
    {0x000080dbd0164b2dull, 0x2b5bff2ee48e0530ull, 3u, 3u, 0x14526914eebbd470ull},
    {0x00000293c19a0bbeull, 0x00002e055c9a3f6bull, 3u, 0u, 0x3facac083126e600ull},
    {0xc0444ccccccccccdull, 0xc041d9999999999aull, 0u, 0u, 0xc053133333333334ull},
    {0x4075900000000000ull, 0xc075800000000000ull, 0u, 2u, 0x3ff0000000000000ull},
    {0x4037c51eb851eb85ull, 0xc046800000000000ull, 1u, 3u, 0x40513147ae147ae1ull},
    {0xc07dd00000000000ull, 0x407e99999999999aull, 2u, 3u, 0xc10c82199999999aull},
    {0x000003ae8249c7a2ull, 0x0000000000000000ull, 0u, 0u, 0x000003ae8249c7a2ull},
    {0x0000000000000000ull, 0x4a833e709e2e312dull, 2u, 2u, 0x0000000000000000ull},
    {0x404b000000000000ull, 0x0000000000000000ull, 0u, 3u, 0x404b000000000000ull},
    {0xc050c00000000000ull, 0x0000000000000000ull, 3u, 2u, 0xffefffffffffffffull},
    {0xc046cccccccccccdull, 0xc051400000000000ull, 3u, 0u, 0x3fe525d7ee30f953ull},
    {0xc082b80000000000ull, 0x3f9ba5e353f7ced9ull, 3u, 0u, 0xc0d5aa4bda12f685ull},
    {0x000f19c2629ccf53ull, 0x000e61acf033d1a4ull, 0u, 2u, 0x001d7b6f52d0a0f7ull},
    {0x0021fa182c40c60dull, 0x0000005e403a93f6ull, 0u, 2u, 0x0021fa474c5e1008ull},
    {0x400fd70a3d70a3d7ull, 0x400e28f5c28f5c29ull, 0u, 2u, 0x401f000000000000ull},
    {0x4046000000000000ull, 0x3fb4395810624dd3ull, 0u, 0u, 0x40460a1cac083127ull},
    {0x0000093445b87315ull, 0x4200c388d0000000ull, 2u, 2u, 0x01934982fc467380ull},
    {0x80bc16c5c5253575ull, 0x3e55798ee2308c3aull, 2u, 3u, 0x80000025b34aa196ull},
    {0x00159283684dba77ull, 0x3d719799812dea11ull, 2u, 0u, 0x00000000000017b8ull},
    {0x4155be67c0000000ull, 0x4155be67bf5c28f6ull, 1u, 2u, 0x3f847ae140000000ull},
    {0xc010000000000000ull, 0x4022000000000000ull, 1u, 2u, 0xc02a000000000000ull},
    {0x4043d9999999999aull, 0x3f9a5119ce075f70ull, 1u, 2u, 0x4043d64f765fd8afull},
    {0x4002666666666666ull, 0x3fb1eb851eb851ecull, 1u, 0u, 0x4001d70a3d70a3d7ull},
    {0x000000000269aec2ull, 0x00000000003dc4adull, 1u, 0u, 0x00000000022bea15ull},
    {0x000000001820d39aull, 0x800000000269aec2ull, 1u, 2u, 0x000000001a8a825cull},
    {0x4010000000000000ull, 0x3abef2d0f5da7dd9ull, 1u, 2u, 0x4010000000000000ull},
    {0x00003739a252b281ull, 0x00f18e3b9b374169ull, 0u, 2u, 0x00f18e3c781dcab4ull},
    {0x000000107e70a6b1ull, 0x40f1170000000000ull, 3u, 0u, 0x00000000000f712bull},
    {0x808ca69686873ba5ull, 0x40a0000000000000ull, 3u, 3u, 0x8001ca69686873bbull},
    {0x80a5111453dbe818ull, 0x4090000000000000ull, 3u, 3u, 0x800a888a29edf40cull},
    {0xcbabbb8800bb9c05ull, 0xe960b8e0acac4eafull, 3u, 0u, 0x223a88ecc2ac8317ull},
    {0x3fc15b035bd512ecull, 0x40a20e3f7ced9168ull, 3u, 0u, 0x3f0ec257a882625full},
};

constexpr std::size_t kNumVectors = sizeof(kVectors) / sizeof(kVectors[0]);

TEST(DoubleFpuCosim, ReplaysFpuTBGoldenVectors)
{
    Vfpu dut;

    auto set_clk = [&](int v) { dut.clk = v; };

    dut.clk = 0;
    dut.rst = 1;
    dut.enable = 0;
    dut.opa = 0;
    dut.opb = 0;
    dut.fpu_op = 0;
    dut.rmode = 0;
    dut.eval();

    for(int i = 0; i < 2; ++i) {
        tick(dut, set_clk);
    }
    dut.rst = 0;

    for(std::size_t i = 0; i < kNumVectors; ++i) {
        const Vector &v = kVectors[i];

        dut.enable = 1;
        dut.opa = v.opa;
        dut.opb = v.opb;
        dut.fpu_op = v.fpu_op;
        dut.rmode = v.rmode;
        for(int c = 0; c < kCyclesEnableHigh; ++c) {
            tick(dut, set_clk);
        }

        dut.enable = 0;
        for(int c = 0; c < kCyclesEnableLow; ++c) {
            tick(dut, set_clk);
        }

        EXPECT_EQ(dut.out, v.expected_out)
            << "Vector #" << i << " op=" << static_cast<int>(v.fpu_op)
            << " rmode=" << static_cast<int>(v.rmode) << std::hex << " opa=0x" << v.opa << " opb=0x"
            << v.opb << " expected=0x" << v.expected_out << " got=0x" << dut.out;
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
