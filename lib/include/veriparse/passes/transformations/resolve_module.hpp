#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_RESOLVE_MODULE
#define VERIPARSE_PASSES_TRANSFORMATIONS_RESOLVE_MODULE

#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <string>


namespace Veriparse {
namespace Passes {
namespace Transformations {


class ResolveModule: public TransformationBase
{
public:
	ResolveModule();

	ResolveModule(const AST::ParamArg::ListPtr &paramlist_inst,
	              const Analysis::Module::ModulesMap &modules_map);

private:
	/**
	 * @return zero on success
	 */
	virtual int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

private:
	AST::ParamArg::ListPtr m_paramlist_inst;
	Analysis::Module::ModulesMap m_modules_map;
};


}
}
}

#endif
