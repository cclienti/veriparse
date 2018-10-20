#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_DEADCODE_ELIMINATION
#define VERIPARSE_PASSES_TRANSFORMATIONS_DEADCODE_ELIMINATION

#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>
#include <string>
#include <set>
#include <iostream>

namespace Veriparse {
namespace Passes {
namespace Transformations {

class DeadcodeElimination: public TransformationBase
{
	using DSet = std::set<std::string>;

	virtual int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

	int analyze_identifiers(AST::Node::Ptr node, DSet &identifiers);

	DSet remove_deadcode_step(AST::Node::Ptr node, AST::Node::Ptr parent);

	int remove_deadstmt(const DSet &deadset,
	                    DeadcodeElimination::DSet &removedset,
	                    AST::Node::Ptr node, AST::Node::Ptr parent);

	int remove_deaddecl(const DSet &removedset,
	                    AST::Node::Ptr node, AST::Node::Ptr parent);

	int remove_emptyblock(AST::Node::Ptr node, AST::Node::Ptr parent);

	int remove_emptystmt(AST::Node::Ptr node, AST::Node::Ptr parent);

	int remove_unused_decl(const DSet &identifiers, const DSet &iodirs,
	                       const AST::Node::Ptr &node, AST::Node::Ptr parent);

	int collect_identifier(DSet &identifiers, const AST::Node::Ptr &node);
};

}
}
}

#endif
