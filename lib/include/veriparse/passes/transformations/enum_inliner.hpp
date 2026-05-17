// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_ENUM_INLINER
#define VERIPARSE_PASSES_TRANSFORMATIONS_ENUM_INLINER

#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>
#include <map>
#include <string>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

/**
 * @brief Replace enum item name identifiers with their resolved IntConstN values.
 *
 * Must run after EnumElaboration so that every EnumItem has an
 * explicit IntConstN value child.
 *
 * The pass:
 * 1. Walks all EnumDef nodes to build an internal map (name -> IntConstN).
 * 2. Walks the full AST and replaces every Identifier whose name
 *    matches an enum item with a clone of the corresponding IntConstN.
 *
 * EnumDef/Typedef nodes are left untouched.
 */
class EnumInliner : public TransformationBase
{
public:
    using ReplaceMap = std::map<std::string, AST::IntConstN::Ptr>;

private:
    /**
     * @return zero on success
     */
    virtual int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

    /**
     * @brief Collect all enum item name->value pairs from EnumDef nodes.
     * @return zero on success
     */
    int collect_enum_items(const AST::Node::Ptr &node);

    /**
     * @brief Replace Identifier nodes that match a collected enum item name.
     * @return zero on success
     */
    int replace_identifiers(AST::Node::Ptr node, AST::Node::Ptr parent);

private:
    ReplaceMap m_replace_map;
};

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse

#endif
