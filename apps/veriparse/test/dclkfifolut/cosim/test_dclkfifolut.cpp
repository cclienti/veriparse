// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti

// Cosim of dclkfifolut — a dual-clock LUT-based FIFO. The original
// dclkfifolut_tb.v exercises the design with two free-running async
// clocks (rclk #2, wclk #3) and a counter-driven stimulus that's
// painful to replicate verbatim in a step-by-step C++ harness.
//
// Instead this cosim verifies the FIFO's *functional* contract — that
// values written on the write port come out in order on the read port,
// even with the cross-clock pointer synchronisation in between. The
// async behaviour is approximated by tightly serialising bursts of
// wclk and rclk ticks with enough sync cycles between bursts.

#include "cosim_helpers.hpp"

#include "Vdclkfifolut.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <vector>

namespace
{

using namespace Veriparse::Test::Cosim;

// FIFO_WIDTH = 8 and LOG2_FIFO_DEPTH = 3 → depth = 8 entries.
constexpr int kFifoDepth = 8;
// Enough rclk ticks for the write pointer Gray-code to cross the wclk→rclk
// synchroniser (and vice-versa). The dclkfifolut uses two flop stages.
constexpr int kSyncCycles = 4;

void tick_r(Vdclkfifolut &dut)
{
    tick(dut, [&](int v) { dut.rclk = v; });
}
void tick_w(Vdclkfifolut &dut)
{
    tick(dut, [&](int v) { dut.wclk = v; });
}

TEST(DclkFifoLutCosim, WriteThenReadPreservesOrderAcrossClockDomains)
{
    Vdclkfifolut dut;

    dut.rclk = 0;
    dut.wclk = 0;
    dut.rsrst = 1;
    dut.wsrst = 1;
    dut.ren = 0;
    dut.wen = 0;
    dut.wdata = 0;
    dut.eval();

    // Hold both resets high for a few cycles each, then release.
    for(int i = 0; i < 4; ++i) {
        tick_r(dut);
    }
    for(int i = 0; i < 4; ++i) {
        tick_w(dut);
    }
    dut.rsrst = 0;
    dut.wsrst = 0;

    std::vector<uint8_t> observed;

    // Two batches mirroring the TB intent: first 1..8, then 0x15..0x1c.
    const std::vector<std::vector<uint8_t>> batches = {
        {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08},
        {0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c},
    };

    for(const auto &batch : batches) {
        ASSERT_LE(batch.size(), static_cast<std::size_t>(kFifoDepth));

        // Push the burst on wclk.
        for(uint8_t v : batch) {
            dut.wen = 1;
            dut.wdata = v;
            tick_w(dut);
        }
        dut.wen = 0;
        tick_w(dut); // settle wen=0

        // Let the write pointer Gray-encode and synchronise into the
        // read domain.
        for(int i = 0; i < kSyncCycles; ++i) {
            tick_r(dut);
        }

        // Drain the burst on rclk; reads succeed only when !rempty.
        std::size_t popped = 0;
        const int max_attempts = static_cast<int>(batch.size()) * 4;
        for(int attempt = 0; attempt < max_attempts && popped < batch.size(); ++attempt) {
            if(!dut.rempty) {
                dut.ren = 1;
                tick_r(dut);
                // After the pop edge, rdata holds the value just popped.
                observed.push_back(static_cast<uint8_t>(dut.rdata));
                popped += 1;
            } else {
                dut.ren = 0;
                tick_r(dut);
            }
        }
        dut.ren = 0;
        tick_r(dut);

        // And let the read pointer sync back to the write side so the
        // next burst sees the FIFO as empty.
        for(int i = 0; i < kSyncCycles; ++i) {
            tick_w(dut);
        }
    }

    std::vector<uint8_t> expected;
    for(const auto &batch : batches) {
        for(uint8_t v : batch) {
            expected.push_back(v);
        }
    }

    ASSERT_EQ(observed.size(), expected.size())
        << "Captured " << observed.size() << " reads, expected " << expected.size();

    for(std::size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(observed[i], expected[i])
            << "Read #" << i << std::hex << " expected 0x" << static_cast<int>(expected[i])
            << " got 0x" << static_cast<int>(observed[i]);
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
