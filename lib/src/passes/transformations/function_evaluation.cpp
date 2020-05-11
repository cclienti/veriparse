#include <veriparse/passes/transformations/function_evaluation.hpp>
#include <veriparse/passes/transformations/ast_replace.hpp>
#include <veriparse/passes/transformations/expression_evaluation.hpp>
#include <veriparse/passes/transformations/variable_folding.hpp>
#include <veriparse/passes/transformations/scope_elevator.hpp>
#include <veriparse/passes/analysis/function.hpp>
#include <veriparse/passes/analysis/functioncall.hpp>
#include <veriparse/generators/verilog_generator.hpp>
#include <veriparse/logger/logger.hpp>
#include <veriparse/AST/nodes.hpp>

#include <set>

namespace Veriparse {
namespace Passes {
namespace Transformations {


class FunctionCallCounter
{
public:
	FunctionCallCounter()
	{
		s_recurse_counter--;
	}

	~FunctionCallCounter()
	{
		s_recurse_counter++;
	}

	static void reset_max(std::int64_t max_value)
	{
		s_max_value = max_value;
		s_recurse_counter = max_value;
	}

	static int64_t get_max_configured()
	{
		return s_max_value;
	}

	static bool is_max_reached()
	{
		return s_recurse_counter < 0;
	}

	static int get_counter()
	{
		return s_recurse_counter.load();
	}

private:
	static inline std::atomic<int64_t> s_max_value{1<<16};
	static inline std::atomic<int64_t> s_recurse_counter{1<<16};
};


void FunctionEvaluation::set_force_automatic()
{
	s_force_auto = true;
}


void FunctionEvaluation::reset_force_automatic()
{
	s_force_auto = false;
}


void FunctionEvaluation::set_max_recurse(std::int64_t max_recurse)
{
	FunctionCallCounter::reset_max(max_recurse);
}


AST::Node::Ptr FunctionEvaluation::evaluate(const AST::FunctionCall::Ptr &function_call,
                                            const FunctionMap &function_map)
{
	FunctionCallCounter counter;

	if (counter.is_max_reached()) {
		LOG_ERROR_N(function_call) << "Max recursive call reached ("
		                           << counter.is_max_reached() << ")";
		return nullptr;
	}

	const auto &function_name = function_call->get_name();
	const auto it_function = function_map.find(function_name);
	if (it_function == function_map.end()) {
		LOG_ERROR_N(function_call) << "declaration of function " << function_name << " not found";
		return nullptr;
	}

	const auto &function = it_function->second;
	if (!Analysis::Function::is_like_automatic(function)) {
		LOG_WARNING_N(function) << "function is not automatic or it can not be considered as such, "
		                        << "function call line " << function_call->get_line() << " "
		                        << "cannot be evaluated";
		if (!s_force_auto) {
			return nullptr;
		}
	}

	const auto &function_copy = AST::cast_to<AST::Function>(it_function->second->clone());
	const auto &main_block = std::make_shared<AST::Block>(function_copy->get_filename(),
	                                                      function_copy->get_line());
	const auto &initial = std::make_shared<AST::Initial>(main_block,
	                                                     function_copy->get_filename(),
	                                                     function_copy->get_line());

	const auto &main_stmts = get_input_declarations(function_copy, function_call);

	if (!main_stmts) {
		LOG_ERROR_N(function_call) << "invalid arguments in function call " << function_call->get_name();
		return nullptr;
	}

	// Construct a set of all input declaration name.
	std::set<std::string> decl_set;
	for (const auto &stmt: *main_stmts) {
		if (stmt->is_node_category(AST::NodeType::Variable)) {
			const auto &var = AST::cast_to<AST::Variable>(stmt);
			decl_set.emplace(var->get_name());
		}
	}

	// Remove input var definition as we already grabbed in the
	// get_input_declarations method.
	const auto &fct_stmts = function_copy->get_statements();
	for (auto it = fct_stmts->begin(); it != fct_stmts->end(); ++it) {
		const auto &stmt = *it;
		if (stmt->is_node_category(AST::NodeType::Variable)) {
			const auto &var = AST::cast_to<AST::Variable>(stmt);
			if (decl_set.count(var->get_name())) {
				it = fct_stmts->erase(it);
			}
		}
		else if(stmt->is_node_type(AST::NodeType::Ioport)) {
			const auto &ioport = AST::cast_to<AST::Ioport>(stmt);
			const auto &iodir = AST::cast_to<AST::IODir>(ioport->get_first());
			if (decl_set.count(iodir->get_name())) {
				LOG_DEBUG << "removing " << iodir->get_name();
				it = fct_stmts->erase(it);
			}
		}
	}

	// Append function statements with grabbed declarations.
	main_stmts->splice(main_stmts->end(), *fct_stmts);
	main_block->set_statements(main_stmts);

	// Start the processing.
	ScopeElevator().run(initial);
	VariableFolding variable_folding(function_map);
	variable_folding.run(initial);

	// Check for result.
	bool res_found;
	const auto &result = variable_folding.get_state(function_name, res_found);
	if (res_found) {
		return result;
	}

	LOG_WARNING_N(function) << "No result found to replace function call line "
	                        << function_call->get_line();

	return nullptr;
};


AST::Node::ListPtr FunctionEvaluation::get_input_declarations(const AST::Function::Ptr &function_decl,
                                                              const AST::FunctionCall::Ptr &function_call)
{
	const auto &decls = std::make_shared<AST::Node::List>();
	const auto &ioports = Analysis::Function::get_ioport_nodes(function_decl);
	const auto &args = function_call->get_args();

	auto it_ioports = ioports->begin();
	auto it_args = args->begin();
	for (; it_ioports != ioports->end() && it_args != args->end(); ++it_ioports, it_args++) {
		const auto &ioport = AST::cast_to<AST::Ioport>((*it_ioports)->clone());

		AST::Variable::Ptr decl;
		const auto &input = ioport->get_first();

		if (ioport->get_second()) {
			decl = ioport->get_second();
			if (decl->is_node_category(AST::NodeType::Net)) {
				LOG_ERROR_N(ioport) << "incorrect parameter type ()" << decl->get_name()
				                    << " in function declaration " << function_decl->get_name();
				return nullptr;
			}
		}
		else {
			// Search in statements for the variable declaration
			const auto &stmts = function_decl->get_statements();

			for (const auto stmt: *stmts) {
				if (stmt->is_node_category(AST::NodeType::Variable)) {
					const auto &var = AST::cast_to<AST::Variable>(stmt);
					if (var->get_name() == input->get_name()) {
						decl = AST::cast_to<AST::Variable>(var->clone());
					}
				}
			}
		}

		// If no declaration found in the statement, we assume this is a
		// reg.
		if (!decl) {
			decl = std::make_shared<AST::Reg>(input->get_widths(), nullptr /*lengths*/,
			                                  nullptr /*right*/, false /*sign*/, input->get_name(),
			                                  ioport->get_filename(), ioport->get_line());
		}

		const auto &arg = (*it_args)->clone();
		const auto &rvalue = std::make_shared<AST::Rvalue>(arg, arg->get_filename(), arg->get_line());
		decl->set_right(rvalue);
		decls->push_back(decl);

	}

	if (it_ioports != ioports->end() || it_args != args->end()) {
		LOG_ERROR_N(function_call) << "too many arguments in call to " << function_call->get_name();
		return nullptr;
	}

	return decls;
}



}
}
}
