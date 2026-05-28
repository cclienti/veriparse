// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/misc/math.hpp>

#include <boost/multiprecision/cpp_bin_float.hpp>
#include <cmath>

namespace Veriparse
{
namespace Misc
{

mpz_class double_to_bignum(double d)
{
    if(!std::isfinite(d) || d == 0.0) {
        return mpz_class(0);
    }
    // Documented Boost.Multiprecision path for arbitrary-magnitude
    // double->integer truncation: load the double into a binary float
    // backend that preserves every mantissa bit, truncate toward zero,
    // then convert to cpp_int. This avoids the static_cast<long long>(d)
    // UB for |d| >= 2^63 that the naive cast would have introduced.
    boost::multiprecision::cpp_bin_float_50 tmp(d);
    return boost::multiprecision::trunc(tmp).convert_to<mpz_class>();
}

std::string bignum_to_str(const mpz_class &x, int base)
{
    if(base == 10) {
        return x.str();
    }
    if(base == 16) {
        return x.str(0, std::ios_base::hex);
    }
    if(base == 8) {
        return x.str(0, std::ios_base::oct);
    }
    if(base == 2) {
        if(x == 0) {
            return "0";
        }
        const bool neg = x < 0;
        mpz_class v = neg ? -x : x;
        std::string s;
        while(v != 0) {
            s.push_back(static_cast<char>('0' + static_cast<int>(v & 1)));
            v >>= 1;
        }
        if(neg) {
            s.push_back('-');
        }
        return std::string(s.rbegin(), s.rend());
    }
    // Fallback for any other base via successive division.
    if(x == 0) {
        return "0";
    }
    const bool neg = x < 0;
    mpz_class v = neg ? -x : x;
    const mpz_class b = base;
    std::string s;
    while(v != 0) {
        int digit = static_cast<int>(v % b);
        s.push_back(digit < 10 ? '0' + digit : 'a' + digit - 10);
        v /= b;
    }
    if(neg) {
        s.push_back('-');
    }
    return std::string(s.rbegin(), s.rend());
}

namespace Math
{

uint32_t log10(uint32_t x)
{
    uint32_t r = ((x >= 1000000000)  ? 9
                  : (x >= 100000000) ? 8
                  : (x >= 10000000)  ? 7
                  : (x >= 1000000)   ? 6
                  : (x >= 100000)    ? 5
                  : (x >= 10000)     ? 4
                  : (x >= 1000)      ? 3
                  : (x >= 100)       ? 2
                  : (x >= 10)        ? 1
                                     : 0);
    return r;
}

mpz_class u64_to_mpz(std::uint64_t num) { return mpz_class(num); }

mpz_class i64_to_mpz(std::int64_t num) { return mpz_class(num); }

} // namespace Math
} // namespace Misc
} // namespace Veriparse
