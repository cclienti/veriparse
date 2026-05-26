// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/passes/transformations/expression_operators.hpp>
#include <veriparse/logger/logger.hpp>

#include <cmath>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

template <> double clog2<double>::operator()(const double x) const
{
    if(x == 0) {
        return 0;
    }
    return std::ceil(std::log2(x));
}

template <> double power<double>::operator()(const double x, const double y) const
{
    return std::pow(x, y);
}

template <> double mod<double>::operator()(const double x, const double y) const
{
    return std::fmod(x, y);
}

// MSVC symbol emission workaround; see expression_operators_intconst.cpp.
#define VP_INST(NAME) template struct NAME<double>
VP_INST(clog2);
VP_INST(power);
VP_INST(mod);
#undef VP_INST

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse
