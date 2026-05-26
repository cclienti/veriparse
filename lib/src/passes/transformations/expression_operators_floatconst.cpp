// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/passes/transformations/expression_operators.hpp>
#include <veriparse/logger/logger.hpp>

#include <memory>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

template <>
AST::FloatConst::Ptr clog2<AST::FloatConst::Ptr>::operator()(const AST::FloatConst::Ptr x) const
{
    if(x) {
        double res = clog2<double>()(x->get_value());
        return std::make_shared<AST::FloatConst>(res);
    } else {
        return AST::FloatConst::Ptr();
    }
}

template <>
AST::FloatConst::Ptr uplus<AST::FloatConst::Ptr>::operator()(const AST::FloatConst::Ptr x) const
{
    if(x) {
        double res = uplus<double>()(x->get_value());
        return std::make_shared<AST::FloatConst>(res);
    } else {
        return AST::FloatConst::Ptr();
    }
}

template <>
AST::FloatConst::Ptr uminus<AST::FloatConst::Ptr>::operator()(const AST::FloatConst::Ptr x) const
{
    if(x) {
        double res = uminus<double>()(x->get_value());
        return std::make_shared<AST::FloatConst>(res);
    } else {
        return AST::FloatConst::Ptr();
    }
}

template <>
AST::FloatConst::Ptr power<AST::FloatConst::Ptr>::operator()(const AST::FloatConst::Ptr x,
                                                             const AST::FloatConst::Ptr y) const
{
    if(x && y) {
        double res = power<double>()(x->get_value(), y->get_value());
        return std::make_shared<AST::FloatConst>(res);
    } else {
        return AST::FloatConst::Ptr();
    }
}

template <>
AST::FloatConst::Ptr times<AST::FloatConst::Ptr>::operator()(const AST::FloatConst::Ptr x,
                                                             const AST::FloatConst::Ptr y) const
{
    if(x && y) {
        double res = times<double>()(x->get_value(), y->get_value());
        return std::make_shared<AST::FloatConst>(res);
    } else {
        return AST::FloatConst::Ptr();
    }
}

template <>
AST::FloatConst::Ptr divide<AST::FloatConst::Ptr>::operator()(const AST::FloatConst::Ptr x,
                                                              const AST::FloatConst::Ptr y) const
{
    if(x && y) {
        double res = divide<double>()(x->get_value(), y->get_value());
        return std::make_shared<AST::FloatConst>(res);
    } else {
        return AST::FloatConst::Ptr();
    }
}

template <>
AST::FloatConst::Ptr mod<AST::FloatConst::Ptr>::operator()(const AST::FloatConst::Ptr x,
                                                           const AST::FloatConst::Ptr y) const
{
    if(x && y) {
        double res = mod<double>()(x->get_value(), y->get_value());
        return std::make_shared<AST::FloatConst>(res);
    } else {
        return AST::FloatConst::Ptr();
    }
}

template <>
AST::FloatConst::Ptr plus<AST::FloatConst::Ptr>::operator()(const AST::FloatConst::Ptr x,
                                                            const AST::FloatConst::Ptr y) const
{
    if(x && y) {
        double res = plus<double>()(x->get_value(), y->get_value());
        return std::make_shared<AST::FloatConst>(res);
    } else {
        return AST::FloatConst::Ptr();
    }
}

template <>
AST::FloatConst::Ptr minus<AST::FloatConst::Ptr>::operator()(const AST::FloatConst::Ptr x,
                                                             const AST::FloatConst::Ptr y) const
{
    if(x && y) {
        double res = minus<double>()(x->get_value(), y->get_value());
        return std::make_shared<AST::FloatConst>(res);
    } else {
        return AST::FloatConst::Ptr();
    }
}

template <>
AST::FloatConst::Ptr less_than<AST::FloatConst::Ptr>::operator()(const AST::FloatConst::Ptr x,
                                                                 const AST::FloatConst::Ptr y) const
{
    if(x && y) {
        double res = less_than<double>()(x->get_value(), y->get_value());
        return std::make_shared<AST::FloatConst>(res);
    } else {
        return AST::FloatConst::Ptr();
    }
}

template <>
AST::FloatConst::Ptr
greater_than<AST::FloatConst::Ptr>::operator()(const AST::FloatConst::Ptr x,
                                               const AST::FloatConst::Ptr y) const
{
    if(x && y) {
        double res = greater_than<double>()(x->get_value(), y->get_value());
        return std::make_shared<AST::FloatConst>(res);
    } else {
        return AST::FloatConst::Ptr();
    }
}

template <>
AST::FloatConst::Ptr
less_equal<AST::FloatConst::Ptr>::operator()(const AST::FloatConst::Ptr x,
                                             const AST::FloatConst::Ptr y) const
{
    if(x && y) {
        double res = less_equal<double>()(x->get_value(), y->get_value());
        return std::make_shared<AST::FloatConst>(res);
    } else {
        return AST::FloatConst::Ptr();
    }
}

template <>
AST::FloatConst::Ptr
greater_equal<AST::FloatConst::Ptr>::operator()(const AST::FloatConst::Ptr x,
                                                const AST::FloatConst::Ptr y) const
{
    if(x && y) {
        double res = greater_equal<double>()(x->get_value(), y->get_value());
        return std::make_shared<AST::FloatConst>(res);
    } else {
        return AST::FloatConst::Ptr();
    }
}

template <>
AST::FloatConst::Ptr equal<AST::FloatConst::Ptr>::operator()(const AST::FloatConst::Ptr x,
                                                             const AST::FloatConst::Ptr y) const
{
    if(x && y) {
        double res = equal<double>()(x->get_value(), y->get_value());
        return std::make_shared<AST::FloatConst>(res);
    } else {
        return AST::FloatConst::Ptr();
    }
}

template <>
AST::FloatConst::Ptr not_equal<AST::FloatConst::Ptr>::operator()(const AST::FloatConst::Ptr x,
                                                                 const AST::FloatConst::Ptr y) const
{
    if(x && y) {
        double res = not_equal<double>()(x->get_value(), y->get_value());
        return std::make_shared<AST::FloatConst>(res);
    } else {
        return AST::FloatConst::Ptr();
    }
}

template <>
AST::FloatConst::Ptr
logical_and<AST::FloatConst::Ptr>::operator()(const AST::FloatConst::Ptr x,
                                              const AST::FloatConst::Ptr y) const
{
    if(x && y) {
        double res = logical_and<double>()(x->get_value(), y->get_value());
        return std::make_shared<AST::FloatConst>(res);
    } else {
        return AST::FloatConst::Ptr();
    }
}

template <>
AST::FloatConst::Ptr
logical_or<AST::FloatConst::Ptr>::operator()(const AST::FloatConst::Ptr x,
                                             const AST::FloatConst::Ptr y) const
{
    if(x && y) {
        double res = logical_or<double>()(x->get_value(), y->get_value());
        return std::make_shared<AST::FloatConst>(res);
    } else {
        return AST::FloatConst::Ptr();
    }
}

// MSVC symbol emission workaround; see expression_operators_intconst.cpp.
#define VP_INST(NAME) template struct NAME<AST::FloatConst::Ptr>
VP_INST(clog2);
VP_INST(uplus);
VP_INST(uminus);
VP_INST(power);
VP_INST(times);
VP_INST(divide);
VP_INST(mod);
VP_INST(plus);
VP_INST(minus);
VP_INST(logical_and);
VP_INST(logical_or);
VP_INST(less_than);
VP_INST(less_equal);
VP_INST(greater_than);
VP_INST(greater_equal);
VP_INST(equal);
VP_INST(not_equal);
#undef VP_INST

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse
