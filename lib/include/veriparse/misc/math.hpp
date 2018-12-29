#ifndef VERIPARSE_MISC_MATH
#define VERIPARSE_MISC_MATH

#include <gmpxx.h>
#include <cstdint>

namespace Veriparse {
namespace Misc {
namespace Math {

/**
 * Return the decimal log of x
 */
uint32_t log10(uint32_t x);

/**
 * Convert a u64 (unsigned) integer to mpz
 */
mpz_class u64_to_mpz(std::uint64_t num);

/**
 * Convert a i64 (signed) integer to mpz
 */
mpz_class i64_to_mpz(std::int64_t num);

}
}
}

#endif
