#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_BRANCH_SELECTION
#define VERIPARSE_PASSES_TRANSFORMATIONS_BRANCH_SELECTION

#include <veriparse/passes/transformations/transformation_base.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/AST/nodes.hpp>

namespace Veriparse {
namespace Passes {
namespace Transformations {

class BranchSelection: public TransformationBase
{
public:
	using FunctionMap = Analysis::Module::FunctionMap;

	BranchSelection() = default;

	BranchSelection(const FunctionMap &function_map);

private:
	/**
	 * @return zero on success.
	 */
	virtual int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

private:
	FunctionMap m_function_map;
};

}
}
}

#endif
