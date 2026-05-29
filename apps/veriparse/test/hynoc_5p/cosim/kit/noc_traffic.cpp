// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti

#include "noc_traffic.hpp"

#include <sstream>

namespace Veriparse
{
namespace Test
{
namespace Hynoc
{

uint64_t make_id_word(uint32_t writer_id, uint32_t packet_id)
{
    const uint64_t pid = static_cast<uint64_t>(packet_id) & ((uint64_t{1} << kPacketIdWidth) - 1);
    const uint64_t wid = static_cast<uint64_t>(writer_id) & ((uint64_t{1} << kWriterIdWidth) - 1);
    return (wid << kPacketIdWidth) | pid;
}

uint64_t make_addr_flit(uint32_t routing_proto, uint32_t dest_mask)
{
    // Address flit layout: {1'b0, proto[3:0], dest_mask[27:0]} packed into 33 bits.
    const uint64_t proto = static_cast<uint64_t>(routing_proto) & 0xfu;
    const uint64_t mask = static_cast<uint64_t>(dest_mask) & ((uint64_t{1} << 28) - 1);
    return (proto << 28) | mask;
}

uint64_t make_id_flit(uint32_t writer_id, uint32_t packet_id)
{
    return make_id_word(writer_id, packet_id);
}

uint64_t make_payload_flit(uint32_t payload)
{
    return static_cast<uint64_t>(payload) & kPayloadMask;
}

uint64_t make_close_flit(uint32_t writer_id, uint32_t packet_id)
{
    return kHeaderMask | make_id_word(writer_id, packet_id);
}

// ---------- StreamWriter ----------

StreamWriter::StreamWriter(const Config &cfg)
    : cfg_(cfg), flit_lcg_(cfg.flit_seed), nb_lcg_(cfg.nb_flits_seed)
{
}

StreamWriter::Tx StreamWriter::tx(bool fifo_full) const
{
    if(fifo_full || state_ == State::Idle || all_sent_) {
        return Tx{false, 0};
    }
    switch(state_) {
    case State::SendAddr:
        return Tx{true, cfg_.address_flit};
    case State::SendId:
        return Tx{true, make_id_flit(cfg_.writer_id, static_cast<uint32_t>(packets_sent_))};
    case State::SendPayload:
        return Tx{true, make_payload_flit(pending_payload_)};
    case State::SendClose:
        return Tx{true, make_close_flit(cfg_.writer_id, static_cast<uint32_t>(packets_sent_))};
    default:
        return Tx{false, 0};
    }
}

void StreamWriter::posedge(bool fifo_full)
{
    if(all_sent_) {
        return;
    }

    switch(state_) {
    case State::Idle:
        if(packets_sent_ >= cfg_.num_packets) {
            all_sent_ = true;
            return;
        }
        flits_left_ = static_cast<int>(nb_lcg_.next() % cfg_.max_flits_per_packet) + 1;
        state_ = State::SendAddr;
        return;

    case State::SendAddr:
        if(!fifo_full) {
            state_ = State::SendId;
        }
        return;

    case State::SendId:
        if(!fifo_full) {
            // Pre-load the payload word for the upcoming SendPayload tick.
            pending_payload_ = flit_lcg_.next();
            state_ = State::SendPayload;
        }
        return;

    case State::SendPayload:
        if(!fifo_full) {
            flits_left_ -= 1;
            if(flits_left_ == 0) {
                state_ = State::SendClose;
            } else {
                pending_payload_ = flit_lcg_.next();
            }
        }
        return;

    case State::SendClose:
        if(!fifo_full) {
            packets_sent_ += 1;
            state_ = State::Idle;
        }
        return;
    }
}

// ---------- StreamReader ----------

StreamReader::StreamReader(const Config &cfg)
    : cfg_(cfg), flit_lcg_(cfg.flit_seed), nb_lcg_(cfg.nb_flits_seed)
{
}

StreamReader::Rx StreamReader::rx(bool fifo_empty) const
{
    if(all_received()) {
        return Rx{false};
    }
    return Rx{!fifo_empty};
}

void StreamReader::posedge(bool data_valid, uint64_t data)
{
    if(!data_valid || all_received()) {
        return;
    }

    const uint32_t body = static_cast<uint32_t>(data & kPayloadMask);
    const bool header_bit = (data & kHeaderMask) != 0;

    switch(state_) {
    case State::ExpectId: {
        const uint32_t expected = static_cast<uint32_t>(
            make_id_word(cfg_.writer_id, static_cast<uint32_t>(packets_received_)));
        if(body != expected || header_bit) {
            std::ostringstream os;
            os << "reader " << cfg_.reader_id << " packet " << packets_received_
               << ": bad id flit, got 0x" << std::hex << data << " expected 0x" << expected;
            errors_.push_back(os.str());
        }
        expected_payload_count_ = static_cast<int>(nb_lcg_.next() % cfg_.max_flits_per_packet) + 1;
        payload_count_ = 0;
        state_ = State::ExpectPayloadOrClose;
        return;
    }

    case State::ExpectPayloadOrClose:
        if(!header_bit) {
            const uint32_t expected = flit_lcg_.next();
            if(body != expected) {
                std::ostringstream os;
                os << "reader " << cfg_.reader_id << " packet " << packets_received_ << " payload["
                   << payload_count_ << "]: got 0x" << std::hex << body << " expected 0x"
                   << expected;
                errors_.push_back(os.str());
            }
            payload_count_ += 1;
        } else {
            const uint32_t expected = static_cast<uint32_t>(
                make_id_word(cfg_.writer_id, static_cast<uint32_t>(packets_received_)));
            if(body != expected) {
                std::ostringstream os;
                os << "reader " << cfg_.reader_id << " packet " << packets_received_
                   << ": bad close flit body, got 0x" << std::hex << body << " expected 0x"
                   << expected;
                errors_.push_back(os.str());
            }
            if(payload_count_ != expected_payload_count_) {
                std::ostringstream os;
                os << "reader " << cfg_.reader_id << " packet " << packets_received_
                   << ": wrong payload count, got " << payload_count_ << " expected "
                   << expected_payload_count_;
                errors_.push_back(os.str());
            }
            packets_received_ += 1;
            state_ = State::ExpectId;
        }
        return;
    }
}

} // namespace Hynoc
} // namespace Test
} // namespace Veriparse
