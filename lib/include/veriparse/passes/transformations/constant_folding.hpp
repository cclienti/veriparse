#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_CONSTANT_FOLDING
#define VERIPARSE_PASSES_TRANSFORMATIONS_CONSTANT_FOLDING

#include <veriparse/passes/transformations/transformation_base.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/AST/nodes.hpp>


namespace Veriparse {
namespace Passes {
namespace Transformations {


class ConstantFolding: public TransformationBase
{
public:
	using FunctionMap = Analysis::Module::FunctionMap;

	ConstantFolding() = default;

	ConstantFolding(const FunctionMap &function_map);

private:
	/**
	 * @return zero on success
	 */
	virtual int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

private:
	FunctionMap m_function_map;
};


}
}
}

#endif
