#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_VARIABLE_FOLDING
#define VERIPARSE_PASSES_TRANSFORMATIONS_VARIABLE_FOLDING

#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>
#include <veriparse/AST/nodes.hpp>
#include <string>
#include <map>


namespace Veriparse {
namespace Passes {
namespace Transformations {

class VariableFolding: public TransformationBase
{
public:
	using StateMap = std::map<std::string, AST::Node::Ptr>;
	using FunctionMap = Analysis::Module::FunctionMap;

	VariableFolding() = default;

	VariableFolding(const FunctionMap &function_map);

	AST::Node::Ptr get_state(const std::string &var_name, bool &matched);

private:
	/**
	 * @return zero on success
	 */
	virtual int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

	/**
	 * @brief Walk through all elements of a block. For each
	 * node, it maintains a state map of variables
	 * values. If possible, all rvalues will be replaced by
	 * their results.
	 *
	 * @return zero on success
	 */
	virtual int execute(AST::Node::Ptr node, AST::Node::Ptr parent);

	/**
	 * @brief Walk through a block.
	 *
	 * @return zero on success
	 */
	virtual int execute_in_childs(AST::Node::Ptr node);

	/**
	 * @brief Walk through a variable declaration.
	 *
	 * @return zero on success
	 */
	virtual int execute_variable_decl(AST::Variable::Ptr var, AST::Node::Ptr parent);

	/**
	 * @brief Walk through a blocking substitution.
	 *
	 * @return zero on success
	 */
	virtual int execute_blocking_substitution(AST::BlockingSubstitution::Ptr subst, AST::Node::Ptr parent);

	/**
	 * @brief Walk through a if statement.
	 *
	 * @return zero on success
	 */
	virtual int execute_if(AST::IfStatement::Ptr ifstmt, AST::Node::Ptr parent);

	/**
	 * @brief Walk through a for statement.
	 *
	 * @return zero on success
	 */
	virtual int execute_for(AST::ForStatement::Ptr forstmt, AST::Node::Ptr parent);

	/**
	 * @brief Walk through a while statement.
	 *
	 * @return zero on success
	 */
	virtual int execute_while(AST::WhileStatement::Ptr whilestmt, AST::Node::Ptr parent);

	/**
	 * @brief Walk through a repeat statement.
	 *
	 * @return zero on success
	 */
	virtual int execute_repeat(AST::RepeatStatement::Ptr repeatstmt, AST::Node::Ptr parent);

	/**
	 * @brief Walk through a repeat statement.
	 *
	 * @return zero on success
	 */
	virtual int execute_call(AST::Node::Ptr call, AST::Node::Ptr parent);

	/**
	 * @brief Resolve the rvalue using the variables
	 * state. Return the constant value. The given rvalue is
	 * updated with the node created using the rvalue result.
	 *
	 * @return constant value on success, else nullptr
	 */
	virtual AST::Node::Ptr analyze_rvalue(AST::Rvalue::Ptr rvalue);

	/**
	 * @brief Return the lvalue string, if there is a verilog pointer, try to resolve it
	 * using the current variables states.
	 *
	 * @return lvalue string
	 */
	virtual std::string analyze_lvalue(AST::Lvalue::Ptr lvalue);

	/**
	 * @brief Resolve the expression using the variables
	 * state. Return the constant value.
	 *
	 * @return constant value on success, else nullptr
	 */
	virtual AST::Node::Ptr analyze_expression(AST::Node::Ptr expr);

private:
	StateMap m_state_map;
	FunctionMap m_function_map;
};

}
}
}

#endif
