// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti

// Cosim of parmem3 — a 3-bank prime-interleaved parallel memory serving
// a dual load/store pair on side A (CRT addressing: bank = EA mod 3,
// index = EA mod 2^DEPTH) and a single linear-addressed port on side B.
//
// The reference model below replicates the RTL contract cycle by cycle
// with the default parameters (DEPTH=10, WIDTH=32, STRIDE_W=12,
// OUTREGA/B=0, PARRES=1):
//   - EA1 = addr + stride computed in AW+1 = 13 signed bits (wrapping,
//     like the RTL adder), oob1 from the wrapped sum's top bits;
//   - conflict = en & dual & (stride mod 3 == 0), not gated by oob;
//   - access 0 has priority on a bank; access 1 is dropped on conflict;
//   - READ_FIRST banks with a 1-cycle synchronous read, the bank-select
//     registered when the access is qualified (ce0/ce1/ceb).
// The two clock domains are exercised serially: side-A and side-B ticks
// alternate, which both the model and the 2-state verilated DUT see as
// the same global order.

#include "cosim_helpers.hpp"

#include "Vparmem3.h"

#include <gtest/gtest.h>

#include <array>
#include <cstdint>

namespace
{

using namespace Veriparse::Test::Cosim;

constexpr int kDepth = 10;
constexpr int kAw = kDepth + 2;
constexpr int kStrideW = 12;
constexpr uint32_t kWords = 3u << kDepth; // 3 * 2^DEPTH linear words
constexpr uint32_t kIdxMask = (1u << kDepth) - 1;
constexpr uint32_t kEa1Mask = (1u << (kAw + 1)) - 1; // AW+1-bit wrapping EA1 adder

int signed_stride(uint32_t raw)
{
    // STRIDE_W-bit two's complement.
    const uint32_t mask = (1u << kStrideW) - 1;
    uint32_t v = raw & mask;
    if(v & (1u << (kStrideW - 1))) {
        return static_cast<int>(v) - (1 << kStrideW);
    }
    return static_cast<int>(v);
}

int mod3(int v) { return ((v % 3) + 3) % 3; }

// Cycle-accurate reference of parmem3 with the default parameters.
struct Parmem3Model
{
    std::array<std::array<uint32_t, 1u << kDepth>, 3> ram{}; // [bank][index]
    std::array<uint32_t, 3> doa_reg{};                       // bank sync read regs, side A
    std::array<uint32_t, 3> dob_reg{};                       // bank sync read regs, side B
    int bank0_r = 0;
    int bank1_r = 0;
    int bankb_r = 0;

    struct SideAIn
    {
        bool en = false, wen = false, dual = false;
        uint32_t addr = 0, stride = 0, dia0 = 0, dia1 = 0;
    };

    struct SideAComb
    {
        bool conflict = false, oob0 = false, oob1 = false;
    };

    // The full side-A address decode — the model's single source of truth,
    // shared by the flag path (comb_a) and the data path (tick_a).
    struct SideADecode
    {
        SideAComb comb;
        bool ce0 = false, ce1 = false;
        int bank0 = 0, bank1 = 0;
        uint32_t idx0 = 0, idx1 = 0;
    };

    static SideADecode decode_a(const SideAIn &in)
    {
        SideADecode d;
        const int scorr = mod3(signed_stride(in.stride));
        d.comb.oob0 = in.en && ((in.addr >> (kAw - 2)) & 3u) == 3u;
        // AW+1-bit wrapping sum, oob from the wrapped pattern's top bits
        // (sign, or both top bits of the in-range field), like the RTL.
        const uint32_t sum = (in.addr + static_cast<uint32_t>(signed_stride(in.stride))) & kEa1Mask;
        const bool neg = (sum >> kAw) & 1u;
        const bool hi = ((sum >> (kAw - 1)) & 1u) && ((sum >> (kAw - 2)) & 1u);
        d.comb.oob1 = in.en && in.dual && (neg || hi);
        d.comb.conflict = in.en && in.dual && (scorr == 0);
        d.ce0 = in.en && !d.comb.oob0;
        d.ce1 = in.en && in.dual && !d.comb.oob1;
        d.bank0 = mod3(static_cast<int>(in.addr));
        d.bank1 = mod3(d.bank0 + scorr); // PARRES = 1
        d.idx0 = in.addr & kIdxMask;
        d.idx1 = sum & kIdxMask;
        return d;
    }

    static SideAComb comb_a(const SideAIn &in) { return decode_a(in).comb; }

    // One posedge clka with the given (already stable) inputs.
    void tick_a(const SideAIn &in)
    {
        const SideADecode d = decode_a(in);
        const bool ce0 = d.ce0;
        const bool ce1 = d.ce1;
        const int bank0 = d.bank0;
        const int bank1 = d.bank1;
        const uint32_t idx0 = d.idx0;
        const uint32_t idx1 = d.idx1;

        for(int b = 0; b < 3; ++b) {
            const bool sel0 = ce0 && (bank0 == b);
            const bool sel1 = ce1 && (bank1 == b) && !sel0;
            const bool ena = sel0 || sel1;
            if(!ena) {
                continue;
            }
            const uint32_t idx = sel0 ? idx0 : idx1;
            const uint32_t dia = sel0 ? in.dia0 : in.dia1;
            doa_reg[b] = ram[b][idx]; // READ_FIRST
            if(in.wen) {
                ram[b][idx] = dia;
            }
        }
        if(ce0) {
            bank0_r = bank0;
        }
        if(ce1) {
            bank1_r = bank1;
        }
    }

    struct SideBIn
    {
        bool enb = false, web = false;
        uint32_t addrb = 0, dib = 0;
    };

    static bool comb_oobb(const SideBIn &in)
    {
        return in.enb && ((in.addrb >> (kAw - 2)) & 3u) == 3u;
    }

    void tick_b(const SideBIn &in)
    {
        const bool ceb = in.enb && !comb_oobb(in);
        if(!ceb) {
            return;
        }
        const int bankb = mod3(static_cast<int>(in.addrb));
        const uint32_t idxb = in.addrb & kIdxMask;
        dob_reg[bankb] = ram[bankb][idxb]; // READ_FIRST
        if(in.web) {
            ram[bankb][idxb] = in.dib;
        }
        bankb_r = bankb;
    }

    uint32_t doa0() const { return doa_reg[bank0_r]; }
    uint32_t doa1() const { return doa_reg[bank1_r]; }
    uint32_t dob() const { return dob_reg[bankb_r]; }
};

void tick_a(Vparmem3 &dut)
{
    tick(dut, [&](int v) { dut.clka = v; });
}
void tick_b(Vparmem3 &dut)
{
    tick(dut, [&](int v) { dut.clkb = v; });
}

void drive_a(Vparmem3 &dut, const Parmem3Model::SideAIn &in)
{
    dut.en = in.en;
    dut.wen = in.wen;
    dut.dual = in.dual;
    dut.addr = in.addr;
    dut.stride = in.stride;
    dut.dia0 = in.dia0;
    dut.dia1 = in.dia1;
}

void drive_b(Vparmem3 &dut, const Parmem3Model::SideBIn &in)
{
    dut.enb = in.enb;
    dut.web = in.web;
    dut.addrb = in.addrb;
    dut.dib = in.dib;
}

// Drive one side-A cycle on model + DUT, checking the combinational
// flags before the edge and the registered read data after it.
void step_a(Vparmem3 &dut, Parmem3Model &model, const Parmem3Model::SideAIn &in)
{
    drive_a(dut, in);
    dut.eval();
    const Parmem3Model::SideAComb c = Parmem3Model::comb_a(in);
    ASSERT_EQ(dut.conflict, c.conflict) << "addr=" << in.addr << " stride=" << in.stride;
    ASSERT_EQ(dut.oob0, c.oob0) << "addr=" << in.addr;
    ASSERT_EQ(dut.oob1, c.oob1) << "addr=" << in.addr << " stride=" << in.stride;

    tick_a(dut);
    model.tick_a(in);
    ASSERT_EQ(dut.doa0, model.doa0()) << "addr=" << in.addr << " stride=" << in.stride;
    ASSERT_EQ(dut.doa1, model.doa1()) << "addr=" << in.addr << " stride=" << in.stride;
}

void step_b(Vparmem3 &dut, Parmem3Model &model, const Parmem3Model::SideBIn &in)
{
    drive_b(dut, in);
    dut.eval();
    ASSERT_EQ(dut.oobb, Parmem3Model::comb_oobb(in)) << "addrb=" << in.addrb;

    tick_b(dut);
    model.tick_b(in);
    ASSERT_EQ(dut.dob, model.dob()) << "addrb=" << in.addrb;
}

Parmem3Model::SideAIn idle_a() { return Parmem3Model::SideAIn{}; }

// Directed: strided pairs (write then read back), conflict and oob
// corners, READ_FIRST on a write+read of the same cell.
TEST(Parmem3Cosim, DirectedPairsConflictsAndRanges)
{
    Vparmem3 dut;
    Parmem3Model model;
    dut.clka = 0;
    dut.clkb = 0;
    drive_a(dut, idle_a());
    drive_b(dut, Parmem3Model::SideBIn{});
    dut.eval();

    // Non-conflicting pair store (stride=4, 4 mod 3 = 1): both banks hit.
    Parmem3Model::SideAIn st;
    st.en = true;
    st.wen = true;
    st.dual = true;
    st.addr = 100;
    st.stride = 4;
    st.dia0 = 0xa0a0a0a0u;
    st.dia1 = 0xb1b1b1b1u;
    {
        SCOPED_TRACE("pair store");
        step_a(dut, model, st);
    }

    // Pair load back at the same addresses.
    Parmem3Model::SideAIn ld = st;
    ld.wen = false;
    {
        SCOPED_TRACE("pair load");
        step_a(dut, model, ld);
    }
    EXPECT_EQ(dut.doa0, 0xa0a0a0a0u);
    EXPECT_EQ(dut.doa1, 0xb1b1b1b1u);

    // Conflicting pair (stride=6, multiple of 3): conflict asserted,
    // access 1 dropped — the load pair must return access 0 only.
    Parmem3Model::SideAIn cf = st;
    cf.stride = 6;
    cf.dia0 = 0xc2c2c2c2u;
    cf.dia1 = 0xd3d3d3d3u;
    {
        SCOPED_TRACE("conflict store");
        step_a(dut, model, cf);
    }
    cf.wen = false;
    {
        SCOPED_TRACE("conflict load");
        step_a(dut, model, cf);
    }
    EXPECT_EQ(dut.doa0, 0xc2c2c2c2u);

    // Negative stride pair: EA0 = 100 (written by the conflict store),
    // EA1 = 96 (never written, still zero).
    Parmem3Model::SideAIn ng;
    ng.en = true;
    ng.wen = false;
    ng.dual = true;
    ng.addr = 100;
    ng.stride = static_cast<uint32_t>(-4) & 0xfffu;
    {
        SCOPED_TRACE("negative stride load");
        step_a(dut, model, ng);
    }
    EXPECT_EQ(dut.doa0, 0xc2c2c2c2u);
    EXPECT_EQ(dut.doa1, 0u);

    // oob0: linear address >= 3*2^DEPTH (top two bits 11).
    Parmem3Model::SideAIn ob;
    ob.en = true;
    ob.dual = false;
    ob.addr = kWords + 5;
    {
        SCOPED_TRACE("oob0");
        step_a(dut, model, ob);
    }

    // oob1: pair overflowing the linear range; conflict NOT gated by it.
    Parmem3Model::SideAIn o1;
    o1.en = true;
    o1.dual = true;
    o1.addr = kWords - 1;
    o1.stride = 6;
    {
        SCOPED_TRACE("oob1 with conflict");
        step_a(dut, model, o1);
    }

    // READ_FIRST: write and read the same cell in one access — the read
    // returns the pre-write content.
    Parmem3Model::SideAIn rf;
    rf.en = true;
    rf.wen = true;
    rf.dual = false;
    rf.addr = 100;
    rf.dia0 = 0x11111111u;
    {
        SCOPED_TRACE("read-first write");
        step_a(dut, model, rf);
    }
    EXPECT_EQ(dut.doa0, 0xc2c2c2c2u); // pre-write content from the conflict store

    // Side B sees side-A data through its own CRT decode.
    Parmem3Model::SideBIn rb;
    rb.enb = true;
    rb.web = false;
    rb.addrb = 100;
    drive_a(dut, idle_a());
    dut.eval();
    {
        SCOPED_TRACE("side B read");
        step_b(dut, model, rb);
    }
    EXPECT_EQ(dut.dob, 0x11111111u);

    // Side B write, side A reads it back.
    Parmem3Model::SideBIn wb;
    wb.enb = true;
    wb.web = true;
    wb.addrb = 200;
    wb.dib = 0x22222222u;
    {
        SCOPED_TRACE("side B write");
        step_b(dut, model, wb);
    }
    drive_b(dut, Parmem3Model::SideBIn{});
    dut.eval();
    Parmem3Model::SideAIn ra;
    ra.en = true;
    ra.addr = 200;
    {
        SCOPED_TRACE("side A read of B write");
        step_a(dut, model, ra);
    }
    EXPECT_EQ(dut.doa0, 0x22222222u);
}

// Randomized soak: interleaved side-A pairs and side-B accesses against
// the reference model, full flag and data compare each cycle.
TEST(Parmem3Cosim, RandomizedSoakAgainstReferenceModel)
{
    Vparmem3 dut;
    Parmem3Model model;
    dut.clka = 0;
    dut.clkb = 0;
    drive_a(dut, idle_a());
    drive_b(dut, Parmem3Model::SideBIn{});
    dut.eval();

    Xorshift32 rng(0xc0ffee11u);

    for(int cycle = 0; cycle < 4000; ++cycle) {
        Parmem3Model::SideAIn a;
        const uint32_t r = rng.next();
        a.en = (r & 0x3u) != 0; // mostly enabled
        a.wen = (r >> 2) & 1u;
        a.dual = (r >> 3) & 1u;
        a.addr = (rng.next() >> 8) & 0xfffu;   // full 12-bit range incl. oob
        a.stride = (rng.next() >> 8) & 0xfffu; // full signed range
        a.dia0 = rng.next();
        a.dia1 = rng.next();
        {
            SCOPED_TRACE(testing::Message() << "cycle " << cycle);
            step_a(dut, model, a);
            if(::testing::Test::HasFatalFailure()) {
                return;
            }
        }

        if((rng.next() & 0x3u) == 0) { // occasional side-B access
            Parmem3Model::SideBIn b;
            b.enb = true;
            b.web = (rng.next() >> 4) & 1u;
            b.addrb = (rng.next() >> 8) & 0xfffu;
            b.dib = rng.next();
            drive_a(dut, idle_a());
            dut.eval();
            SCOPED_TRACE(testing::Message() << "cycle " << cycle << " side B");
            step_b(dut, model, b);
            if(::testing::Test::HasFatalFailure()) {
                return;
            }
            drive_b(dut, Parmem3Model::SideBIn{});
        }
    }
}

} // namespace
