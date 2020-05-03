#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_EXPRESSION_EVALUATION
#define VERIPARSE_PASSES_TRANSFORMATIONS_EXPRESSION_EVALUATION

#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/AST/nodes.hpp>
#include <map>


namespace Veriparse {
namespace Passes {
namespace Transformations {

class ExpressionEvaluation {
public:
	/**
	 * Map to replace an identifier by a constant during expression
	 * evaluations.
	 */
	using ReplaceMap = std::map<std::string, AST::Constant::Ptr>;

	/**
	 * Map to get the function that corresponds to the function call
	 * found during expression evaluations.
	 */
	using FunctionMap = Analysis::Module::FunctionMap;


public:
	/**
	 * Construct the ExpressionEvaluation.
	 */
	ExpressionEvaluation();

	/**
	 * Construct the ExpressionEvaluation and pass a map that
	 * will be used to replace identifier node (that match the
	 * string name) with the corresponding constant node.
	 */
	ExpressionEvaluation(const ReplaceMap &replace_map);

	/**
	 * Construct the ExpressionEvaluation and pass a map that
	 * will be used to process function call in expressions.
	 */
	ExpressionEvaluation(const FunctionMap &function_map);

	/**
	 * Construct the ExpressionEvaluation and pass a replace map that
	 * will be used to replace identifier node (that match the string
	 * name) with the corresponding constant node. A function map is
	 * also required to process function call in expressions.
	 */
	ExpressionEvaluation(const ReplaceMap &replace_map,
	                     const FunctionMap &function_map);

	/**
	 * Evaluate node recursively and return a constant node if
	 * all parts of the expression are constants, else it
	 * returns nullptr.
	 */
	AST::Node::Ptr evaluate_node(const AST::Node::Ptr node);

	/**
	 * Evaluate node recursively and return the value by
	 * reference. If the value is valid, the method returns
	 * true.
	 */
	bool evaluate_node(const AST::Node::Ptr &node, mpz_class &value);

	/**
	 * Evaluate node recursively and return the value by
	 * reference. If the value is valid, the method returns
	 * true.
	 */
	bool evaluate_node(const AST::Node::Ptr &node, double &value);

private:
	/**
	 * Evaluate unary system call
	 */
	AST::Node::Ptr evaluate_system_call(const std::string &call,
	                                    const AST::Node::ListPtr args,
	                                    const std::string &filename, uint32_t line);

	/**
	 * Evaluate unary node
	 */
	AST::Node::Ptr evaluate_constant_node(const AST::NodeType operation,
	                                      const AST::Node::Ptr node0);

	/**
	 * Evaluate binary node
	 */
	AST::Node::Ptr evaluate_constant_node(const AST::NodeType operation,
	                                      const AST::Node::Ptr node0,
	                                      const AST::Node::Ptr node1);

	/**
	 * Evaluate ternary node
	 */
	AST::Node::Ptr evaluate_constant_node(const AST::NodeType operation,
	                                      const AST::Node::Ptr node0,
	                                      const AST::Node::Ptr node1,
	                                      const AST::Node::Ptr node2);

private:
	const ReplaceMap m_replace_map;
	const FunctionMap m_function_map;

};

}
}
}

#endif
