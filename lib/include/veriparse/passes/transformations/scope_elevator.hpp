#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_SCOPE_ELEVATOR
#define VERIPARSE_PASSES_TRANSFORMATIONS_SCOPE_ELEVATOR

#include <veriparse/passes/transformations/transformation_base.hpp>
#include <veriparse/AST/nodes.hpp>
#include <list>
#include <map>

namespace Veriparse {
namespace Passes {
namespace Transformations {

/**
 * @brief Remove useless block (named or not).
 *
 * When the block is named it pays attention to the scope declared and
 * used.
 */
class ScopeElevator: public TransformationBase
{
public:
	using ScopeStack = std::list<std::string>;
	using GlobalReplaceMap = std::map<std::string,
	                                  std::pair<ScopeStack, std::string>>;
	using LocalReplaceMap = std::map<std::string, std::string>;

protected:
	/**
	 * @return zero on success.
	 */
	virtual int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

	virtual int process_variables(const AST::Node::Ptr &node, AST::Node::Ptr &parent);

	virtual int process_scoped_identifiers(const AST::Node::Ptr &node, AST::Node::Ptr &parent);

	virtual int rename_nested_variables(const AST::Node::Ptr &node, const AST::Node::Ptr &parent,
	                                    const std::string &scope, LocalReplaceMap &replace_map);

	virtual int rename_nested_identifiers(const AST::Node::Ptr &node, const AST::Node::Ptr &parent,
	                                      const std::string &scope, const LocalReplaceMap &replace_map);

	/**
	 * @brief Remove the current stack state (in m_scope_stack) to the
	 * given scope_stack.
	 */
	virtual void remove_current_scope(ScopeElevator::ScopeStack &scope_stack);

private:
	ScopeStack m_scope_stack;
	GlobalReplaceMap m_global_replace_map;
};

}
}
}

#endif
