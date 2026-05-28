// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
//
// Thin wrapper that lets the rest of the codebase keep using `mpz_class`
// as the bignum type while the actual implementation is Boost.Multiprecision
// cpp_int (header-only, no external dep on libgmpxx). The alias is at
// global scope to match GMP's traditional placement of `mpz_class`.

#ifndef VERIPARSE_MISC_BIGNUM
#define VERIPARSE_MISC_BIGNUM

#include <climits>
#include <cstdint>

// MSVC's <stdint.h> defines SIZE_MAX with the `ui64` suffix
// (0xffffffffffffffffui64), which is an MSVC extension neither MSVC's
// nor clang-cl's preprocessor accepts inside `#if` constant expressions.
// boost/multiprecision/detail/digits.hpp opens with
//     #if ULONG_MAX != SIZE_MAX
// and that line fails on every flex-generated scanner TU (the working
// TUs in the project happen to include cpp_int through a path that
// dodges the `#if`). Re-define SIZE_MAX with the standard `ull` suffix
// so the preprocessor can evaluate the comparison.
#if defined(_MSC_VER) && defined(SIZE_MAX)
#undef SIZE_MAX
#define SIZE_MAX 0xffffffffffffffffull
#endif

#include <boost/multiprecision/cpp_int.hpp>
#include <string>

using mpz_class = boost::multiprecision::cpp_int;

namespace Veriparse
{
namespace Misc
{

// Convert a bignum to its string representation in an arbitrary base (2..36).
// cpp_int's built-in str() only supports dec/oct/hex; Verilog needs binary too.
std::string bignum_to_str(const mpz_class &x, int base = 10);

// Truncate a double toward zero into an arbitrary-precision integer. Safe
// for the full double range (|x| up to ~1.8e308); static_cast<long long>(d)
// would be undefined behaviour for |d| >= 2^63. Returns 0 for NaN/inf.
// Equivalent to gmpxx's implicit double->mpz_class conversion.
mpz_class double_to_bignum(double d);

} // namespace Misc
} // namespace Veriparse

#endif
