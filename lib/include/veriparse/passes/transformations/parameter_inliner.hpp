#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_PARAMETER_INLINER
#define VERIPARSE_PASSES_TRANSFORMATIONS_PARAMETER_INLINER

#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>
#include <string>


namespace Veriparse {
namespace Passes {
namespace Transformations {

class ParameterInliner: public TransformationBase {
public:
	ParameterInliner();

	ParameterInliner(AST::ParamArg::ListPtr paramlist_inst);

private:
	/**
	 * @return zero on success
	 */
	virtual int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

	/**
	 * Inline parameters rvalue in all others parameters. The algorithm is
	 * in O(n^2). For each parameter, we replace the parameter rvalue
	 * in all other parameters.
	 *
	 * If m_paramlist_inst is null, it uses the parameters default values
	 *
	 * If there is circular dependencies, the method will fail.
	 *
	 * @return zero on success.
	 */
	int resolve_paramlist();

	/**
	 * Remove the parameter using its name.
	 * @return zero on success.
	 */
	int remove_parameter(AST::Node::Ptr node, std::string name, AST::Node::Ptr parent=nullptr);

private:
	AST::Parameter::ListPtr m_paramlist;
	AST::ParamArg::ListPtr m_paramlist_inst;

};

}
}
}

#endif
