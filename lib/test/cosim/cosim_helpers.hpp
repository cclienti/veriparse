// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_TEST_COSIM_HELPERS_HPP
#define VERIPARSE_TEST_COSIM_HELPERS_HPP

// Generic, design-agnostic helpers for cosim tests driving a verilator-
// generated DUT (`V<top>` class) from GoogleTest. Each design's test
// stays free to pick stimulus/expected vectors and pipeline timing; this
// header only covers the boilerplate that repeats across designs:
//   - tick(dut, set_clk)             one full clock cycle
//   - reset(dut, set_rst, set_clk, polarity, cycles)
//   - wide_assign / wide_eq          for signals > 64 bits (VlWide<N>)
//   - to_hex                         diagnostic-friendly formatting
//
// Clock and reset are passed as callable setters so the helper works with
// verilator's top-level signals, which are exposed as references in the
// generated `V<top>` class (member pointers to references are not legal
// C++, ruling out a `&Dut::clk` API).

#include <verilated.h>

#include <array>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>

namespace Veriparse
{
namespace Test
{
namespace Cosim
{

template <typename Dut, typename ClkSetter> void tick(Dut &dut, ClkSetter set_clk)
{
    set_clk(0);
    dut.eval();
    set_clk(1);
    dut.eval();
}

template <typename Dut, typename RstSetter, typename ClkSetter>
void reset(Dut &dut, RstSetter set_rst, ClkSetter set_clk, bool active_high = true, int cycles = 2)
{
    set_rst(active_high ? 1 : 0);
    for(int i = 0; i < cycles; ++i) {
        tick(dut, set_clk);
    }
    set_rst(active_high ? 0 : 1);
}

template <std::size_t N> void wide_assign(VlWide<N> &dst, const std::array<uint32_t, N> &src)
{
    for(std::size_t i = 0; i < N; ++i) {
        dst[i] = src[i];
    }
}

template <std::size_t N> bool wide_eq(const VlWide<N> &got, const std::array<uint32_t, N> &expected)
{
    for(std::size_t i = 0; i < N; ++i) {
        if(got[i] != expected[i]) {
            return false;
        }
    }
    return true;
}

namespace detail
{
inline std::string format_hex_words(const uint32_t *words, std::size_t count)
{
    std::ostringstream os;
    os << std::hex << std::setfill('0');
    for(std::size_t i = count; i-- > 0;) {
        os << std::setw(8) << words[i];
        if(i > 0) {
            os << '_';
        }
    }
    return os.str();
}
} // namespace detail

template <std::size_t N> std::string to_hex(const VlWide<N> &v)
{
    uint32_t buf[N];
    for(std::size_t i = 0; i < N; ++i) {
        buf[i] = v[i];
    }
    return detail::format_hex_words(buf, N);
}

template <std::size_t N> std::string to_hex(const std::array<uint32_t, N> &v)
{
    return detail::format_hex_words(v.data(), N);
}

} // namespace Cosim
} // namespace Test
} // namespace Veriparse

#endif
