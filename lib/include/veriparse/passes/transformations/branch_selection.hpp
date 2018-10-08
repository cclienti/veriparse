#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_BRANCH_SELECTION
#define VERIPARSE_PASSES_TRANSFORMATIONS_BRANCH_SELECTION

#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>


namespace Veriparse {
namespace Passes {
namespace Transformations {

class BranchSelection: public TransformationBase
{
	/**
	 * @return zero on success.
	 */
	virtual int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;
};

}
}
}

#endif
