#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_EXPRESSION_EVALUATION
#define VERIPARSE_PASSES_TRANSFORMATIONS_EXPRESSION_EVALUATION

#include <veriparse/AST/nodes.hpp>
#include <map>

namespace Veriparse {
	namespace Passes {
		namespace Transformations {

			class ExpressionEvaluation {
			public:
				typedef std::map<std::string, AST::Constant::Ptr> replace_map_t;


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
				ExpressionEvaluation(const replace_map_t &replace_identifier);

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
				const replace_map_t m_replace_identifier;

			};

		}
	}
}

#endif
