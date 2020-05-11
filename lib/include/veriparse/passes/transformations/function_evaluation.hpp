#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_FUNCTION_EVALUATION
#define VERIPARSE_PASSES_TRANSFORMATIONS_FUNCTION_EVALUATION

#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/AST/nodes.hpp>
#include <map>
#include <cstddef>


namespace Veriparse {
namespace Passes {
namespace Transformations {


class FunctionEvaluation
{
public:
	using FunctionMap = Analysis::Module::FunctionMap;

	FunctionEvaluation() = default;

	/**
	 * @brief Force the evaluation of functions as if they were
	 * declared as automatic.
	 */
	static void set_force_automatic();

	/**
	 * @brief Functions are evaluated as they are declared (automatic
	 * or not).
	 */
	static void reset_force_automatic();

	/**
	 * @brief Set the maximum depth authorized. A negative value will
	 * prevent the function evaluation to work.
	 */
	static void set_max_recurse(std::int64_t max_recurse);

	/**
	 * @brief Evaluate the function call using the given map of
	 * functions.
	 */
	AST::Node::Ptr evaluate(const AST::FunctionCall::Ptr &function_call,
	                        const FunctionMap &function_map);

private:
	AST::Node::ListPtr get_input_declarations(const AST::Function::Ptr &function_decl,
	                                          const AST::FunctionCall::Ptr &function_call);

private:
	static inline bool s_force_auto {false};
};


}
}
}

#endif
