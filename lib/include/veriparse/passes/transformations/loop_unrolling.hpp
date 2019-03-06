#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_LOOP_UNROLLING
#define VERIPARSE_PASSES_TRANSFORMATIONS_LOOP_UNROLLING

#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>
#include <veriparse/passes/transformations/expression_evaluation.hpp>

#include <string>
#include <memory>
#include <vector>
#include <list>
#include <map>


namespace Veriparse {
namespace Passes {
namespace Transformations {

class LoopUnrolling: public TransformationBase {
private:
	using Range = std::pair<std::string, std::vector<AST::Node::Ptr>>;
	using RangePtr = std::shared_ptr<Range>;
	using ScopeMap = std::map<std::string, std::string>;

private:
	/**
	 * @brief Apply the LoopUnrolling transform.
	 *
	 * @return zero on success.
	 */
	virtual int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

	/**
	 * @brief Unroll loops
	 *
	 * Unroll loops and keep a track of scope and how identifiers are
	 * renamed.
	 *
	 * @return zero on success.
	 */
	int unroll(AST::Node::Ptr node, AST::Node::Ptr parent, const std::string scope_state);

	/**
	 * @brief Rename scoped identifier with mapping gathered during
	 * unrolling.
	 *
	 * @return zero on success.
	 */
	int rename_scoped_identifiers(AST::Node::Ptr node, AST::Node::Ptr parent);

	/**
	 * @brief Push scope into map of scopes.
	 *
	 * The scope of map keep tracks of how identifiers are renamed
	 * depending on scope hierarchy.
	 *
	 * @return zero on success.
	 */
	int map_scope(const std::string &verilog_scope, const std::string &scope_state, const std::string &rename_suffix);

	/**
	 * @brief Evaluate a ForStatement Node.
	 *
	 * It returns a range_t made of the loop variable string and the
	 * vector of all loop index values. It returns nullptr if the pre,
	 * post or condition part of the for expression cannot be resolved
	 * statically.
	 *
	 * @return The range pointer.
	 */
	static RangePtr get_for_range(const AST::ForStatement::Ptr &for_node);

	/**
	 * @brief Return the lvalue identifier name of a blocking
	 * substitution.
	 *
	 * @return non-empty string on success.
	 */
	static std::string get_cond_lvalue(const AST::BlockingSubstitution::Ptr &subst);

	/**
	 * @brief Return the rvalue node of a blocking substitution.
	 *
	 * @return non-nullptr on success.
	 */
	static AST::Node::Ptr get_cond_rvalue(const AST::BlockingSubstitution::Ptr &subst);

	/**
	 * @brief Return the rvalue node of a blocking substitution.
	 *
	 * @return non-nullptr on success.
	 */
	static AST::Node::Ptr get_cond_rvalue(const AST::BlockingSubstitution::Ptr &subst,
	                                      const ExpressionEvaluation::replace_map_t &map);

private:
	ScopeMap m_scope_map;
};

}
}
}

#endif
