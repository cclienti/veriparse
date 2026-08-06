// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERILOWER_STRAIGHT_LINE_EXPECTED_TRACE_HPP
#define VERILOWER_STRAIGHT_LINE_EXPECTED_TRACE_HPP

#include "cosim_helpers.hpp"

#include <gtest/gtest.h>
#include <cstdint>

// One canonical post-reset trace serves both models, compared at the
// constant offset ADR-0014 §11 defines: the reference starts at time zero
// regardless of reset, so it leads the generated FSM by exactly the reset
// hold. Sampling after each rising edge, the run enters every state and
// takes every transition of the straight-line machine — the §11.2 coverage
// baseline for this shape — and ends parked in the hold state.
struct Sample
{
    std::uint8_t acc;
    std::uint8_t done;
};

static constexpr Sample expected_trace[] = {
    {1, 0}, // acc <= 8'd1
    {3, 0}, // acc <= acc + 8'd2
    {3, 1}, // done <= 1'b1
    {3, 1}, // hold state: nothing moves
    {3, 1}, //
    {3, 1}, //
};

template <typename Dut> void check_from_reset(Dut &dut, int hold_cycles)
{
    using namespace Veriparse::Test::Cosim;

    dut.clk = 0;
    dut.rst_n = 0;
    dut.eval();

    for(int i = 0; i < hold_cycles; ++i) {
        tick(dut, [&](int v) { dut.clk = v; });
    }
    dut.rst_n = 1;
    dut.eval();

    int cycle = 0;
    for(const auto &sample : expected_trace) {
        tick(dut, [&](int v) { dut.clk = v; });
        dut.eval();
        ASSERT_EQ(sample.acc, dut.acc) << "cycle " << cycle;
        ASSERT_EQ(sample.done, dut.done) << "cycle " << cycle;
        ++cycle;
    }
}

#endif
