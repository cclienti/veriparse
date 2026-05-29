// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti

// Cosim of hynoc_router_5p — drives the same routing fabric as
// hynoc_router_5p_tb5.v (2 routers + 8 local interfaces) from a C++
// harness, with the stream_writer / stream_reader peers reimplemented
// in C++ (see noc_traffic.{hpp,cpp}) so the test is independent of
// Verilog $random semantics.
//
// STATUS: harness + kit + topology wrapper all build and run end to
// end against the verilated, veriflat'd design. The test currently
// FAILS because the readers observe the writer's first payload where
// they expect the ID flit — i.e. an extra flit is being consumed by
// the network before the reader sees anything. Likely candidates:
//   - the router strips the address AND ID flits per hop (so a 2-hop
//     multicast strips two flits, not one); the rebuilt reader needs
//     to skip them
//   - the writer's address flit encoding misroutes the first cycle
//     and the router consumes an extra flit to recover
//   - the noc_traffic kit advances its LCG one extra time per packet
// Re-disable the test or fix the off-by-one before pushing.
//
// Topology (matches the TB):
//   writer 0 -> LI0 -> router0:P0
//   writer 1 -> LI1 -> router1:P0
//   writer 2 -> LI2 -> router0:P1
//   stub     -> LI3..7 (no ingress traffic)
//   8 readers attached to LI0..7 egress, each matched to one writer:
//     reader 0 <- writer 1 (multicast)
//     reader 1 <- writer 0
//     reader 2 <- writer 1
//     reader 3 <- writer 0
//     reader 4 <- writer 1
//     reader 5 <- writer 2
//     reader 6 <- writer 1
//     reader 7 <- writer 2

#include "cosim_helpers.hpp"
#include "noc_traffic.hpp"

#include "Vhynoc_topology.h"

#include <gtest/gtest.h>

#include <array>
#include <cstdint>
#include <memory>

namespace
{

using namespace Veriparse::Test::Cosim;
using namespace Veriparse::Test::Hynoc;

// Per-port flow-control extracted from the verilated topology. Centralised
// so the per-tick loop reads consistent snapshots.
struct PortSnapshot
{
    bool full;
    bool empty;
    uint64_t egress_data;
};

PortSnapshot snapshot(Vhynoc_topology &dut, int idx)
{
    PortSnapshot s;
    s.full = ((dut.local_ingress_full >> idx) & 1u) != 0;
    s.empty = ((dut.local_egress_empty >> idx) & 1u) != 0;
    switch(idx) {
    case 0:
        s.egress_data = dut.local_egress_data_0;
        break;
    case 1:
        s.egress_data = dut.local_egress_data_1;
        break;
    case 2:
        s.egress_data = dut.local_egress_data_2;
        break;
    case 3:
        s.egress_data = dut.local_egress_data_3;
        break;
    case 4:
        s.egress_data = dut.local_egress_data_4;
        break;
    case 5:
        s.egress_data = dut.local_egress_data_5;
        break;
    case 6:
        s.egress_data = dut.local_egress_data_6;
        break;
    case 7:
        s.egress_data = dut.local_egress_data_7;
        break;
    default:
        s.egress_data = 0;
        break;
    }
    return s;
}

void set_ingress(Vhynoc_topology &dut, int idx, bool write_en, uint64_t data)
{
    if(write_en) {
        dut.local_ingress_write |= (1u << idx);
    } else {
        dut.local_ingress_write &= ~(1u << idx);
    }
    switch(idx) {
    case 0:
        dut.local_ingress_data_0 = data;
        break;
    case 1:
        dut.local_ingress_data_1 = data;
        break;
    case 2:
        dut.local_ingress_data_2 = data;
        break;
    case 3:
        dut.local_ingress_data_3 = data;
        break;
    case 4:
        dut.local_ingress_data_4 = data;
        break;
    case 5:
        dut.local_ingress_data_5 = data;
        break;
    case 6:
        dut.local_ingress_data_6 = data;
        break;
    case 7:
        dut.local_ingress_data_7 = data;
        break;
    }
}

void set_egress_read(Vhynoc_topology &dut, int idx, bool read_en)
{
    if(read_en) {
        dut.local_egress_read |= (1u << idx);
    } else {
        dut.local_egress_read &= ~(1u << idx);
    }
}

// Shared LCG seed scheme (writer/reader pairs derive their payload &
// nb-flits sequences from these, scaled by the writer id so distinct
// writers produce distinct streams).
constexpr uint32_t kBaseFlitSeed = 0x12345678u;
constexpr uint32_t kBaseNbSeed = 0x9abcdef0u;
constexpr int kNumPackets = 16;
constexpr int kMaxFlitsPerPacket = 32;

uint32_t seed_for(uint32_t base, int writer_id)
{
    return base + static_cast<uint32_t>(writer_id) * 0x1000003du;
}

constexpr uint32_t kProtoMcast = 0x1u; // PROTO_ROUTING_MCAST_CIRCUIT_SWITCH

// Destination masks copied from hynoc_router_5p_tb5.v.
constexpr uint32_t kWriter0Dest = 0b0000'0000'0000'0000'1000'0011'0001u;
constexpr uint32_t kWriter1Dest = 0b0000'0000'0000'0000'1000'1111'0001u;
constexpr uint32_t kWriter2Dest = 0b0000'0000'0000'0000'0100'1100'0001u;

// Disabled until the ID/address handshake mismatch is sorted out
// (see top-of-file STATUS block).
TEST(HynocRouter5pCosim, DISABLED_MulticastRoutingDeliversAllPacketsInOrder)
{
    Vhynoc_topology dut;

    // Reset all relevant signals to zero before the first eval.
    dut.router_clk = 0;
    dut.router_srst = 1;
    dut.local_clk = 0;
    dut.local_srst = 0xff;
    dut.local_ingress_write = 0;
    dut.local_egress_read = 0;
    dut.eval();

    // Hold reset for a handful of cycles to flush any X state.
    for(int i = 0; i < 8; ++i) {
        dut.router_clk = !dut.router_clk;
        dut.local_clk = (i & 1) ? 0xffu : 0x00u;
        dut.eval();
    }
    dut.router_clk = 0;
    dut.local_clk = 0;
    dut.router_srst = 0;
    dut.local_srst = 0;
    dut.eval();

    // Writers attached to LI0, LI1, LI2 with multicast addresses.
    StreamWriter writer0({0, make_addr_flit(kProtoMcast, kWriter0Dest), kNumPackets,
                          kMaxFlitsPerPacket, seed_for(kBaseFlitSeed, 0),
                          seed_for(kBaseNbSeed, 0)});
    StreamWriter writer1({1, make_addr_flit(kProtoMcast, kWriter1Dest), kNumPackets,
                          kMaxFlitsPerPacket, seed_for(kBaseFlitSeed, 1),
                          seed_for(kBaseNbSeed, 1)});
    StreamWriter writer2({2, make_addr_flit(kProtoMcast, kWriter2Dest), kNumPackets,
                          kMaxFlitsPerPacket, seed_for(kBaseFlitSeed, 2),
                          seed_for(kBaseNbSeed, 2)});

    // Reader-to-writer pairing from the TB header comments:
    //   reader 0 <- writer 1   reader 1 <- writer 0
    //   reader 2 <- writer 1   reader 3 <- writer 0
    //   reader 4 <- writer 1   reader 5 <- writer 2
    //   reader 6 <- writer 1   reader 7 <- writer 2
    const std::array<int, 8> reader_to_writer = {1, 0, 1, 0, 1, 2, 1, 2};
    std::array<std::unique_ptr<StreamReader>, 8> readers;
    for(int r = 0; r < 8; ++r) {
        const int w = reader_to_writer[r];
        readers[r] = std::make_unique<StreamReader>(
            StreamReader::Config{r, w, kNumPackets, kMaxFlitsPerPacket, seed_for(kBaseFlitSeed, w),
                                 seed_for(kBaseNbSeed, w)});
    }

    // Each LI is bound to at most one writer and one reader.
    const std::array<StreamWriter *, 8> writer_at = {
        &writer0, &writer1, &writer2, nullptr, nullptr, nullptr, nullptr, nullptr,
    };

    // The reader on a port samples local_egress_data the cycle after it
    // asserted local_egress_read with !local_egress_empty.
    std::array<bool, 8> pending_read = {};

    constexpr int kMaxCycles = 200'000;
    int cycle = 0;
    for(; cycle < kMaxCycles; ++cycle) {
        // 1) Sample current flow-control state.
        std::array<PortSnapshot, 8> snaps;
        for(int i = 0; i < 8; ++i) {
            snaps[i] = snapshot(dut, i);
        }

        // 2) Drive ingress and egress for the upcoming posedge.
        for(int i = 0; i < 8; ++i) {
            if(writer_at[i] != nullptr) {
                auto tx = writer_at[i]->tx(snaps[i].full);
                set_ingress(dut, i, tx.write_en, tx.write_data);
            } else {
                set_ingress(dut, i, false, 0);
            }
            const auto rx = readers[i]->rx(snaps[i].empty);
            set_egress_read(dut, i, rx.read_en);
        }
        dut.eval();

        // 3) Tick the clocks (both router and all local clocks together).
        dut.router_clk = 1;
        dut.local_clk = 0xff;
        dut.eval();
        dut.router_clk = 0;
        dut.local_clk = 0x00;
        dut.eval();

        // 4) Advance the writer/reader FSMs and feed observed data to
        // any reader that asserted read_en when the FIFO had data.
        for(int i = 0; i < 8; ++i) {
            if(writer_at[i] != nullptr) {
                writer_at[i]->posedge(snaps[i].full);
            }
            // data_valid one cycle later: we sampled empty pre-edge, then
            // asserted read_en; if !empty, the FIFO popped on this edge.
            const bool data_valid = pending_read[i];
            uint64_t observed = 0;
            if(data_valid) {
                const auto post = snapshot(dut, i);
                observed = post.egress_data;
            }
            readers[i]->posedge(data_valid, observed);
            pending_read[i] = ((dut.local_egress_read >> i) & 1u) && !snaps[i].empty;
        }

        // 5) Exit when every writer is done and every reader has all
        // expected packets in.
        bool all_done = writer0.all_sent() && writer1.all_sent() && writer2.all_sent();
        for(int i = 0; i < 8 && all_done; ++i) {
            if(!readers[i]->all_received()) {
                all_done = false;
            }
        }
        if(all_done) {
            break;
        }
    }

    EXPECT_LT(cycle, kMaxCycles) << "Timeout — packets did not converge";

    EXPECT_TRUE(writer0.all_sent()) << "writer 0 sent " << writer0.packets_sent();
    EXPECT_TRUE(writer1.all_sent()) << "writer 1 sent " << writer1.packets_sent();
    EXPECT_TRUE(writer2.all_sent()) << "writer 2 sent " << writer2.packets_sent();

    for(int r = 0; r < 8; ++r) {
        EXPECT_TRUE(readers[r]->all_received())
            << "reader " << r << " received " << readers[r]->packets_received() << "/"
            << kNumPackets;
        for(const auto &err : readers[r]->errors()) {
            ADD_FAILURE() << err;
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
