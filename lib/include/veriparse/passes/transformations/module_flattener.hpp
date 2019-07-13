#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_MODULE_FLATTENER
#define VERIPARSE_PASSES_TRANSFORMATIONS_MODULE_FLATTENER

#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/passes/analysis/unique_declaration.hpp>
#include <veriparse/misc/tree.hpp>

#include <string>
#include <map>
#include <unordered_map>
#include <list>


namespace Veriparse {
namespace Passes {
namespace Transformations {


class ModuleFlattener: public TransformationBase
{
public:
	using TreeNode = Misc::TreeNode<std::pair<std::string, std::string>>;

public:
	ModuleFlattener() = delete;

	ModuleFlattener(const AST::ParamArg::ListPtr &paramlist_inst,
	                const Analysis::Module::ModulesMap &modules_map,
	                bool deadcode_elimination=true);

	virtual ~ModuleFlattener();

public:
	/**
	 * @brief Return the instance tree from the processed module.
	 *
	 * Each node tree consist in a pair of string. The first element is
	 * the module name and the second element is the instance name.
	 */
	TreeNode::Ptr get_instance_tree() const;

private:
	/**
	 * @return zero on success
	 */
	virtual int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

	/**
	 * @return zero on success
	 */
	int flattener(const AST::Node::Ptr &node, const AST::Node::Ptr &parent);

	/**
	 * @brief Bind a module
	 *
	 * Return necessary items (declaration and assignation) to
	 * flatten the instantiated module.
	 *
	 * @return nullptr on error
	 */
	AST::Node::ListPtr bind(const AST::Instance::Ptr &instance, const AST::Module::Ptr &module);

	/**
	 * @brief Check that instantiated port value is a wire or composed
	 * by wired.
	 *
	 * @return true if the port value is valid.
	 */
	bool check_output_rvalue_wire(const AST::Node::Ptr &node);

	/**
	 * @brief Check that instantiated port value is a wire or composed
	 * by wired.
	 *
	 * @return zero on success.
	 */
	int convert_concat_to_lconcat(const AST::Node::Ptr &node, const AST::Node::Ptr &parent);

	/**
	 * @brief Extract defparam in a multimap. The key is the outer
	 * scope and the value is the node itself.
	 */
	int extract_defparam(const AST::Node::Ptr &node, const AST::Node::Ptr &parent);

	/**
	 * @brief Restore defparam.
	 */
	int restore_defparam(const AST::Node::Ptr &node);

	/**
	 * @brief Replace scoped identifier that corresponds to flattened
	 * instance hierarchy.
	 */
	int replace_scoped_identifiers(const AST::Node::Ptr &node);

private:
	AST::ParamArg::ListPtr m_paramlist_inst;
	Analysis::Module::ModulesMap m_modules_map;
	const bool m_deadcode_elimination;
	std::map<std::string, AST::NodeType> m_var_type_map;
	Analysis::UniqueDeclaration::IdentifierSet m_declared;
	std::unordered_multimap<std::string, AST::Defparamlist::Ptr> m_defparams;
	TreeNode::Ptr m_instance_tree;
};

}
}
}

#endif
