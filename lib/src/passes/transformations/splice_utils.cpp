// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/passes/transformations/splice_utils.hpp>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{
namespace SpliceUtils
{

void stamp_subroutine_lifetime(const AST::Node::Ptr &item, bool automatic_scope)
{
    if(item->is_node_type(AST::NodeType::Function)) {
        const auto &function = AST::cast_to<AST::Function>(item);
        if(function->get_lifetime() == AST::Function::LifetimeEnum::NONE) {
            function->set_lifetime(automatic_scope ? AST::Function::LifetimeEnum::AUTOMATIC
                                                   : AST::Function::LifetimeEnum::STATIC);
        }
        return;
    }
    if(item->is_node_type(AST::NodeType::Task)) {
        const auto &task = AST::cast_to<AST::Task>(item);
        if(task->get_lifetime() == AST::Task::LifetimeEnum::NONE) {
            task->set_lifetime(automatic_scope ? AST::Task::LifetimeEnum::AUTOMATIC
                                               : AST::Task::LifetimeEnum::STATIC);
        }
    }
}

} // namespace SpliceUtils
} // namespace Transformations
} // namespace Passes
} // namespace Veriparse
