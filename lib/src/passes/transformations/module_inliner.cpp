#include <veriparse/passes/transformations/module_inliner.hpp>
#include <veriparse/passes/transformations/expression_evaluation.hpp>
#include <veriparse/passes/analysis/instance.hpp>
#include <veriparse/misc/string_utils.hpp>
#include <veriparse/logger/logger.hpp>

#include <algorithm>


namespace Veriparse {
namespace Passes {
namespace Transformations {


ModuleInliner::ModuleInliner(const AST::ParamArg::ListPtr &paramlist_inst,
                             const Analysis::Module::ModulesMap &modules_map):
	m_modules_map (modules_map)
{
}

ModuleInliner::~ModuleInliner()
{
}

int ModuleInliner::process(AST::Node::Ptr node, AST::Node::Ptr parent)
{
	return 0;
}


}
}
}
