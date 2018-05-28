#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_VARIABLE_FOLDING
#define VERIPARSE_PASSES_TRANSFORMATIONS_VARIABLE_FOLDING

#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>
#include <string>
#include <map>


namespace Veriparse {
	namespace Passes {
		namespace Transformations {

			class VariableFolding: public TransformationBase
			{
				typedef std::map<std::string, AST::Node::Ptr> StateMap;

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
				virtual int inline_blocking_assignation(AST::Node::Ptr node, AST::Node::Ptr parent);

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


			private:
				StateMap m_state_map;

			};

		}
	}
}

#endif
