#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_FUNCTION_EVALUATION
#define VERIPARSE_PASSES_TRANSFORMATIONS_FUNCTION_EVALUATION

#include <veriparse/AST/nodes.hpp>
#include <map>
#include <cstddef>


namespace Veriparse {
namespace Passes {
namespace Transformations {


class FunctionEvaluation
{
public:
	using FunctionMap = std::map<std::string, AST::Function::Ptr>;

	FunctionEvaluation() = default;

	FunctionEvaluation(std::size_t max_recurse);

	AST::Node::Ptr evaluate(const AST::FunctionCall::Ptr &function_call,
	                        const FunctionMap &function_map);

private:
	AST::Node::ListPtr get_input_declarations(const AST::Function::Ptr &function_decl,
	                                          const AST::FunctionCall::Ptr &function_call);

private:
	std::size_t m_max_recurse {1ull<<16};
};


}
}
}

#endif
