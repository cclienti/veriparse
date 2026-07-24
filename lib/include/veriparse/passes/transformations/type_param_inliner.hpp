// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_TYPE_PARAM_INLINER
#define VERIPARSE_PASSES_TRANSFORMATIONS_TYPE_PARAM_INLINER

#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>

#include <functional>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

/**
 * @brief Reduce every type parameter to a typedef (ADR-0010).
 *
 * A `TypeParam` binds its instantiation actual — matched by name in the
 * same ParamArg list ParameterInliner consumes — or its declared default
 * (IEEE 1800-2017 §6.20.3: an unoverridden type parameter without a default
 * is an error). Header formals reduce to `Typedef` items spliced after the
 * leading typedef run at the body head, so a default referencing a spliced
 * package typedef still resolves in declaration order; body formals reduce
 * in place. TypedefInliner, running later, performs all substitution.
 *
 * Must run before ParameterInliner: after this pass no `TypeParam` remains,
 * and the type-actual ParamArgs are simply unmatched there.
 */
class TypeParamInliner : public TransformationBase
{
public:
    TypeParamInliner();

    /**
     * @brief Constructor with the instance parameter actuals.
     */
    TypeParamInliner(AST::ParamArg::ListPtr paramlist_inst);

private:
    /**
     * @return zero on success
     */
    int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

    /**
     * @brief Reduce the header formals of a module/interface and recurse
     * into the body.
     */
    int process_scope(const AST::Declaration::ListPtr &params,
                      const std::function<void(const AST::Declaration::ListPtr &)> &set_params,
                      const AST::Node::ListPtr &items, const AST::Node::Ptr &node);

    /**
     * @brief Bind the actual (or default) and build the reduced typedef;
     * null on error.
     */
    AST::Typedef::Ptr reduce(const AST::TypeParam::Ptr &tparam) const;

private:
    AST::ParamArg::ListPtr m_paramlist_inst;
};

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse

#endif
