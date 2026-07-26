// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_PASSES_ANALYSIS_DECLARATION_HELPERS_PRIVATE
#define VERIPARSE_PASSES_ANALYSIS_DECLARATION_HELPERS_PRIVATE

#include <veriparse/AST/nodes.hpp>

namespace
{

// A declared signal is a Var or any Net — but NOT a bare port placeholder. A
// non-ANSI directional declaration with neither a net nor a data type keyword
// (`input clock`, `input [7:0] x`) parses as an ImplicitNet carrying an
// ImplicitType; its real signal is declared separately (`wire clock`) or
// defaults to a net, so it is not itself a variable declaration. An
// ImplicitNet with a concrete data type (`output reg valid` -> ImplicitNet +
// RegType) IS a real declaration and is kept. After DefaultResolution
// (ADR-0012) the placeholder shape survives only for a direction declaration
// backed by a standalone body declaration — for which this verdict stays
// exact.
inline bool is_declared_signal(const Veriparse::AST::Declaration::Ptr &d)
{
    using Veriparse::AST::NodeType;
    if(!(d->is_node_type(NodeType::Var) || d->is_node_category(NodeType::Net))) {
        return false;
    }
    if(d->is_node_type(NodeType::ImplicitNet)) {
        const Veriparse::AST::DataType::Ptr type = d->get_type();
        if(!type || type->is_node_type(NodeType::ImplicitType)) {
            return false;
        }
    }
    return true;
}

} // namespace

#endif
