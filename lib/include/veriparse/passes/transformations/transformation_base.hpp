#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_TRANSFORMATION_BASE
#define VERIPARSE_PASSES_TRANSFORMATIONS_TRANSFORMATION_BASE

#include <veriparse/AST/nodes.hpp>

#include <functional>


namespace Veriparse {
namespace Passes {
namespace Transformations {

class TransformationBase {
private:
	using ProcessFunction = std::function< int (AST::Node::Ptr, AST::Node::Ptr) >;

public:
	/**
	 * @return zero on success
	 */
	virtual int run(AST::Node::Ptr node) {
		return process(node, nullptr);
	}

protected:
	/**
	 * @brief Method that process the current node.
	 * @return zero on success.
	 */
	virtual int process(AST::Node::Ptr node, AST::Node::Ptr parent) = 0;

	/**
	 * @brief Call the process method for each child (and the
	 * current node become the parent).
	 * @return zero on success.
	 */
	virtual int recurse_in_childs(AST::Node::Ptr node);

	/**
	 * @brief Call the function method for each child (and the
	 * current node become the parent).
	 * @return zero on success.
	 */
	virtual int recurse_in_childs(AST::Node::Ptr node, ProcessFunction function);

	/**
	 * @brief Replace the given node in the parent by a list
	 * of nodes list.
	 *
	 * If necessary a block can be added or the existing list
	 * in the parent is appended with the stmts.
	 *
	 * This function is typically useful to replace a if node
	 * by either the true or the false statements.
	 *
	 * @return nothing.
	 */
	virtual void pickup_statements(AST::Node::Ptr parent, AST::Node::Ptr node,
	                               AST::Node::ListPtr stmts);

	/**
	 * @brief Replace the given node in the parent by the stmt node.
	 *
	 * If necessary the given block can be merged with an existing block
	 * in the parent to append the statements.
	 *
	 * This function is typically useful to replace a if node
	 * by either the true or the false statements.
	 *
	 * @return nothing.
	 */
	virtual void pickup_statements(AST::Node::Ptr parent, AST::Node::Ptr node,
	                               AST::Node::Ptr stmt);

};

}
}
}

#endif
