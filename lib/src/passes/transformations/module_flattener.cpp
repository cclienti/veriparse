#include <veriparse/passes/transformations/module_flattener.hpp>
#include <veriparse/passes/transformations/resolve_module.hpp>
#include <veriparse/passes/analysis/instance.hpp>
#include <veriparse/misc/string_utils.hpp>
#include <veriparse/logger/logger.hpp>

#include <algorithm>


namespace Veriparse {
namespace Passes {
namespace Transformations {


ModuleFlattener::ModuleFlattener(const AST::ParamArg::ListPtr &paramlist_inst,
                             const Analysis::Module::ModulesMap &modules_map):
    m_paramlist_inst (paramlist_inst),
	m_modules_map (modules_map)
{
}

ModuleFlattener::~ModuleFlattener()
{
}

int ModuleFlattener::process(AST::Node::Ptr node, AST::Node::Ptr parent)
{
    ResolveModule resolver(m_paramlist_inst, m_modules_map);
    if (resolver.run(node)) {
        LOG_ERROR_N(node) << "failed to resolve the module";
        return 1;
    }

    if (flattener(node, parent)) {
        LOG_ERROR_N(node) << "failed to inline the module";
        return 1;
    }

	return 0;
}

int ModuleFlattener::flattener(const AST::Node::Ptr &node, const AST::Node::Ptr &parent)
{
    if (!node) {
        return 1;
    }

    switch (node->get_node_type()) {
    case AST::NodeType::Instancelist:
        {

        }
        break;

    default:
        {
            int ret = 0;
            AST::Node::ListPtr children = node->get_children();
            for (AST::Node::Ptr child: *children) {
                ret += flattener(child, node);
            }
            return ret;
        }

    }

    return 0;
}


}
}
}
