#include <veriparse/passes/transformations/resolve_module.hpp>
#include <veriparse/passes/transformations/module_io_normalizer.hpp>
#include <veriparse/passes/transformations/parameter_inliner.hpp>
#include <veriparse/passes/transformations/localparam_inliner.hpp>
#include <veriparse/passes/transformations/constant_folding.hpp>
#include <veriparse/passes/transformations/loop_unrolling.hpp>
#include <veriparse/passes/transformations/branch_selection.hpp>
#include <veriparse/passes/transformations/generate_removal.hpp>
#include <veriparse/passes/transformations/variable_folding.hpp>
#include <veriparse/passes/transformations/deadcode_elimination.hpp>
#include <veriparse/passes/transformations/module_instance_normalizer.hpp>
#include <veriparse/misc/string_utils.hpp>
#include <veriparse/logger/logger.hpp>

#include <algorithm>


namespace Veriparse {
namespace Passes {
namespace Transformations {


ResolveModule::ResolveModule(bool deadcode_elimination):
	m_deadcode_elimination (deadcode_elimination)
{
}

ResolveModule::ResolveModule(const AST::ParamArg::ListPtr &paramlist_inst,
                             const Analysis::Module::ModulesMap &modules_map,
                             bool deadcode_elimination):
	m_paramlist_inst (paramlist_inst),
	m_modules_map (modules_map),
	m_deadcode_elimination (deadcode_elimination)
{
}

int ResolveModule::process(AST::Node::Ptr node, AST::Node::Ptr parent)
{
	if (ModuleIONormalizer().run(node)) {
		LOG_ERROR_N(node) << "Failed to normalized module I/Os";
		return 1;
	}

	if (ParameterInliner(m_paramlist_inst).run(node)) {
		LOG_ERROR_N(node) << "Failed to inline parameters";
		return 1;
	}

	if (LocalparamInliner().run(node)) {
		LOG_ERROR_N(node) << "Failed to inline local parameters";
		return 1;
	}

	if (ConstantFolding().run(node)) {
		LOG_ERROR_N(node) << "Failed to fold constants";
		return 1;
	}

	if (LoopUnrolling().run(node)) {
		LOG_ERROR_N(node) << "Failed to unroll loops";
		return 1;
	}

	if (BranchSelection().run(node)) {
		LOG_ERROR_N(node) << "Failed to select branches";
		return 1;
	}

	if (GenerateRemoval().run(node)) {
		LOG_ERROR_N(node) << "Failed to select branches";
		return 1;
	}

	if (ConstantFolding().run(node)) {
		LOG_ERROR_N(node) << "Failed to fold constants";
		return 1;
	}

	if (VariableFolding().run(node)) {
		LOG_ERROR_N(node) << "Failed to fold variables";
		return 1;
	}

	if (m_deadcode_elimination) {
		if (DeadcodeElimination().run(node)) {
			LOG_ERROR_N(node) << "Failure to remove dead code";
			return 1;
		}
	}

	if (ModuleInstanceNormalizer(m_modules_map).run(node)) {
		LOG_ERROR_N(node) << "Failed to normalize module's instances";
		return 1;
	}

	return 0;
}


}
}
}
