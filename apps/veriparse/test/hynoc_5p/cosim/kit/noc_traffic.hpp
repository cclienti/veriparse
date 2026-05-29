// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti

// Reusable HyNoC traffic injectors and receivers for cosim tests of the
// hynoc_router_5p design. The C++ peers exchange flits over a verilator-
// generated topology that matches what hynoc_stream_writer.v /
// hynoc_stream_reader.v drive in the original Verilog testbench.
//
// Packet layout (FLIT_WIDTH = 33 = PAYLOAD_WIDTH + 1):
//   - 1 address flit  (header bit = 0, body = routing proto + dest mask)
//   - 1 ID flit       (header bit = 0, body = {writer_id, packet_id})
//   - N payload flits (header bit = 0, body = LCG-derived word)
//   - 1 close flit    (header bit = 1, body = {writer_id, packet_id})
//
// The router consumes the address flits while routing, so the reader sees
// the sequence:   ID, payload x N, close.
//
// This header lives under apps/veriparse/test/hynoc_5p/cosim/kit/ on
// purpose: the long-term home for this code is a dedicated NoC validation
// library outside this repo. It is intentionally NOT shipped via the conda
// package — only the hynoc cosim test consumes it.

#ifndef VERIPARSE_TEST_HYNOC_NOC_TRAFFIC_HPP
#define VERIPARSE_TEST_HYNOC_NOC_TRAFFIC_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace Veriparse
{
namespace Test
{
namespace Hynoc
{

constexpr int kPayloadWidth = 32;
constexpr int kFlitWidth = kPayloadWidth + 1;
constexpr int kHeaderBit = kPayloadWidth; // MSB of the flit
constexpr uint64_t kHeaderMask = uint64_t{1} << kHeaderBit;
constexpr uint64_t kPayloadMask = (uint64_t{1} << kPayloadWidth) - 1;

// The writer/reader split the payload word in two halves: lower half is the
// per-writer packet identifier, upper half is the writer identifier.
constexpr int kPacketIdWidth = kPayloadWidth / 2;
constexpr int kWriterIdWidth = kPayloadWidth - kPacketIdWidth;

uint64_t make_id_word(uint32_t writer_id, uint32_t packet_id);
uint64_t make_addr_flit(uint32_t routing_proto, uint32_t dest_mask);
uint64_t make_id_flit(uint32_t writer_id, uint32_t packet_id);
uint64_t make_payload_flit(uint32_t payload);
uint64_t make_close_flit(uint32_t writer_id, uint32_t packet_id);

// Glibc LCG (Park-Miller-style) — used by both writer and reader to
// produce a matching payload sequence per packet. Verilog's `$random`
// follows the same recurrence in the IEEE 1364 reference implementation.
struct Lcg32
{
    uint32_t seed;
    explicit Lcg32(uint32_t s = 1) : seed(s) {}
    uint32_t next()
    {
        seed = seed * 1103515245u + 12345u;
        return seed;
    }
};

class StreamWriter
{
public:
    struct Config
    {
        int writer_id;
        uint64_t address_flit; // pre-built (see make_addr_flit)
        int num_packets;
        int max_flits_per_packet; // upper bound for the random size
        uint32_t flit_seed;       // payload LCG seed
        uint32_t nb_flits_seed;   // per-packet size LCG seed
    };

    explicit StreamWriter(const Config &cfg);

    // Compute the {write_en, write_data} pair to apply before the next
    // posedge of local_clk, given the current fifo_full status.
    struct Tx
    {
        bool write_en;
        uint64_t write_data;
    };
    Tx tx(bool fifo_full) const;

    // Commit the FSM transition that goes with the most recent tx() call.
    void posedge(bool fifo_full);

    bool all_sent() const { return all_sent_; }
    int packets_sent() const { return packets_sent_; }
    int writer_id() const { return cfg_.writer_id; }

private:
    enum class State
    {
        Idle,
        SendAddr,
        SendId,
        SendPayload,
        SendClose
    };

    Config cfg_;
    State state_ = State::Idle;
    Lcg32 flit_lcg_;
    Lcg32 nb_lcg_;
    int packets_sent_ = 0;
    int flits_left_ = 0;
    bool all_sent_ = false;
    uint32_t pending_payload_ = 0;
};

class StreamReader
{
public:
    struct Config
    {
        int reader_id;
        int writer_id; // expected source
        int num_packets;
        int max_flits_per_packet; // mirrors the writer
        uint32_t flit_seed;
        uint32_t nb_flits_seed;
    };

    explicit StreamReader(const Config &cfg);

    // Compute the read_en signal to apply before the next posedge given
    // the current fifo_empty status. Pull-only — read_en just enables a
    // pop whenever data is available.
    struct Rx
    {
        bool read_en;
    };
    Rx rx(bool fifo_empty) const;

    // Notify a posedge. data_valid means the prior read_en was honoured
    // (the FIFO popped). data is the flit visible on local_egress_data.
    void posedge(bool data_valid, uint64_t data);

    bool all_received() const { return packets_received_ >= cfg_.num_packets; }
    int packets_received() const { return packets_received_; }
    int reader_id() const { return cfg_.reader_id; }
    const std::vector<std::string> &errors() const { return errors_; }

private:
    enum class State
    {
        ExpectId,
        ExpectPayloadOrClose
    };

    Config cfg_;
    State state_ = State::ExpectId;
    Lcg32 flit_lcg_;
    Lcg32 nb_lcg_;
    int packets_received_ = 0;
    int payload_count_ = 0;
    int expected_payload_count_ = 0;
    std::vector<std::string> errors_;
};

} // namespace Hynoc
} // namespace Test
} // namespace Veriparse

#endif
