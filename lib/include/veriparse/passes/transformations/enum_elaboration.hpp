// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_ENUM_ELABORATION
#define VERIPARSE_PASSES_TRANSFORMATIONS_ENUM_ELABORATION

#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

/**
 * @brief Resolve enum item ordinal values.
 *
 * Must run after ConstantFolding so that explicit enum item values
 * (e.g. `A = WIDTH-1`) are already folded to IntConstN.
 *
 * For each EnumDef in the AST, walks its items in declaration order:
 * - If an item already has an explicit IntConstN value, that value
 *   becomes the new counter base.
 * - If an item has no value, an IntConstN is injected with the
 *   current counter value.
 * In both cases the counter is incremented after each item.
 */
class EnumElaboration : public TransformationBase
{
private:
    /**
     * @return zero on success
     */
    virtual int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

    /**
     * @brief Elaborate a single EnumDef node in-place.
     * @return zero on success
     */
    int elaborate_enumdef(const AST::EnumType::Ptr &enumdef);
};

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse

#endif
