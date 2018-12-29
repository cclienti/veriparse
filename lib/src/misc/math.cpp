#include <veriparse/misc/math.hpp>
#include <sstream>


namespace Veriparse {
namespace Misc {
namespace Math {

uint32_t log10(uint32_t x)
{
	uint32_t r = ((x >= 1000000000) ? 9 :
	              (x >= 100000000)  ? 8 :
	              (x >= 10000000)   ? 7 :
	              (x >= 1000000)    ? 6 :
	              (x >= 100000)     ? 5 :
	              (x >= 10000)      ? 4 :
	              (x >= 1000)       ? 3 :
	              (x >= 100)        ? 2 :
	              (x >= 10)         ? 1 : 0);
	return r;
}

mpz_class u64_to_mpz(std::uint64_t num)
{
	std::stringstream ss;
	ss << num;
	mpz_class mpz;
	ss >> mpz;
	return mpz;
}

mpz_class i64_to_mpz(std::int64_t num)
{
	std::stringstream ss;
	ss << num;
	mpz_class mpz;
	ss >> mpz;
	return mpz;
}

}
}
}
