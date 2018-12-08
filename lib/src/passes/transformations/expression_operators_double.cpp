#include <veriparse/passes/transformations/expression_operators.hpp>
#include <veriparse/logger/logger.hpp>

#include <cmath>


namespace Veriparse {
namespace Passes {
namespace Transformations {

template<>
double clog2<double>::operator()(const double x) const {
	if (x == 0) return 0;
	return std::ceil(std::log2(x));
}

template<>
double power<double>::operator()(const double x, const double y) const {
	return std::pow(x, y);
}

template<>
double mod<double>::operator()(const double x, const double y) const {
	return std::fmod(x, y);
}


}
}
}
