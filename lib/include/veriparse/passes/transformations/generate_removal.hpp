#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_GENERATE_REMOVAL
#define VERIPARSE_PASSES_TRANSFORMATIONS_GENERATE_REMOVAL

#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>


namespace Veriparse {
namespace Passes {
namespace Transformations {

class GenerateRemoval: public TransformationBase
{
	/**
	 * @return zero on success.
	 */
	virtual int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

	/**
	 * @return return true if the generate block sub-statements can be safely removed.
	 */
	bool is_generate_removable(const AST::Node::Ptr &node);
};

}
}
}

#endif
