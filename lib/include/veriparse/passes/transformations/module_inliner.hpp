#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_MODULE_INLINER
#define VERIPARSE_PASSES_TRANSFORMATIONS_MODULE_INLINER

#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/passes/analysis/unique_declaration.hpp>
#include <string>
#include <map>
#include <list>


namespace Veriparse {
namespace Passes {
namespace Transformations {

class ModuleInliner: public TransformationBase
{
public:
	ModuleInliner() = delete;

	ModuleInliner(const AST::ParamArg::ListPtr &paramlist_inst,
	              const Analysis::Module::ModulesMap &modules_map);

	virtual ~ModuleInliner();

private:
	/**
	 * @return zero on success
	 */
	virtual int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

private:
	Analysis::Module::ModulesMap m_modules_map;
	Analysis::UniqueDeclaration::IdentifierSet m_declared;
};

}
}
}

#endif
