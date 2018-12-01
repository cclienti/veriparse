#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_MODULE_FLATTENER
#define VERIPARSE_PASSES_TRANSFORMATIONS_MODULE_FLATTENER

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

class ModuleFlattener: public TransformationBase
{
public:
	ModuleFlattener() = delete;

	ModuleFlattener(const AST::ParamArg::ListPtr &paramlist_inst,
	              const Analysis::Module::ModulesMap &modules_map);

	virtual ~ModuleFlattener();

private:
	/**
	 * @return zero on success
	 */
	virtual int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

    int flattener(const AST::Node::Ptr &node, const AST::Node::Ptr &parent);

private:
    AST::ParamArg::ListPtr m_paramlist_inst;
	Analysis::Module::ModulesMap m_modules_map;
	Analysis::UniqueDeclaration::IdentifierSet m_declared;
};

}
}
}

#endif
