// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_WIRE_SPLIT
#define VERIPARSE_PASSES_TRANSFORMATIONS_WIRE_SPLIT

#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>

namespace Veriparse {
namespace Passes {
namespace Transformations {

/**
 * Split inline wire declarations `wire [N:0] x = expr;` into
 * separate declaration and continuous assignment:
 *   wire [N:0] x;
 *   assign x = expr;
 *
 * This avoids forward-reference issues in flattened Verilog.
 */
class WireSplit: public TransformationBase
{
	virtual int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;
};

}
}
}

#endif
