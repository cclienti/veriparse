#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_LOOP_UNROLLING
#define VERIPARSE_PASSES_TRANSFORMATIONS_LOOP_UNROLLING

#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>
#include <veriparse/passes/transformations/expression_evaluation.hpp>
#include <string>
#include <memory>
#include <vector>
#include <utility>


namespace Veriparse {
	namespace Passes {
		namespace Transformations {

			class LoopUnrolling: public TransformationBase {
				typedef std::pair<std::string, std::vector<AST::Node::Ptr>> range_t;
				typedef std::shared_ptr<range_t> range_ptr_t;

			protected:
				/**
				 * @return zero on success.
				 */
				int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

			private:

				/**
				 * Evaluate a ForStatement Node. It returns a range_t made
				 * of the loop variable string and the vector of all loop
				 * index values. It returns nullptr if the pre, post or
				 * condition part of the for expression cannot be resolved
				 * statically.
				 */
				static range_ptr_t get_for_range(const AST::ForStatement::Ptr for_node);

				/**
				 * Return the lvalue identifier name of a blocking
				 * substitution.
				 *
				 * @return non-empty string on success.
				 */
				static std::string get_cond_lvalue(const AST::BlockingSubstitution::Ptr subst);

				/**
				 * Return the rvalue node of a blocking substitution.
				 *
				 * @return non-nullptr on success.
				 */
				static AST::Node::Ptr get_cond_rvalue(const AST::BlockingSubstitution::Ptr subst);

				/**
				 * Return the rvalue node of a blocking substitution.
				 *
				 * @return non-nullptr on success.
				 */
				static AST::Node::Ptr get_cond_rvalue(const AST::BlockingSubstitution::Ptr subst,
				                                      const ExpressionEvaluation::replace_map_t &map);
			};

		}
	}
}

#endif
