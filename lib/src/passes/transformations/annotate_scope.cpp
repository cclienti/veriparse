#include <veriparse/passes/transformations/annotate_scope.hpp>
#include <veriparse/logger/logger.hpp>

// #include <algorithm>
// #include <iterator>


namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

AnnotateScope::AnnotateScope():
	m_search    ("^.*$"),
	m_replace   ("$&")
{}

AnnotateScope::AnnotateScope(const std::string &search, const std::string &replace):
	m_search    (search),
	m_replace   (replace)
{}

void AnnotateScope::set_search_replace(const std::string &search,
                                             const std::string &replace)
{
	m_search = search;
	m_replace = replace;
}

int AnnotateScope::process(AST::Node::Ptr node, AST::Node::Ptr parent)
{
	ReplaceDict replace_dict;

	if(get_scope_names(node, replace_dict))
		return 1;

	if(annotate_names(node, replace_dict))
		return 1;

	return 0;
}

int AnnotateScope::get_scope_names(const AST::Node::Ptr node, ReplaceDict &replace_dict)
{
	int rc = 0;

	if (!node) {
		LOG_ERROR << "Empty node found";
		return 1;
	}

	bool found_scope = false;
	std::string scope;

	if (node->is_node_type(AST::NodeType::Block)) {
		const auto &block = AST::cast_to<AST::Block>(node);
		scope = block->get_scope();
		if (scope.size()) {
			found_scope = true;
		}
	}

	if (found_scope) {
		if (replace_dict.count(scope) != 0) {
			LOG_WARNING_N(node) << "redefinition of '" << scope << "'";
		}
		replace_dict[scope] = std::regex_replace(scope, m_search, m_replace);
	}

	const auto &children = node->get_children();
	for (const auto &child: *children) {
		rc |= get_scope_names(child, replace_dict);
	}

	return rc;
}

int AnnotateScope::annotate_names(AST::Node::Ptr node, ReplaceDict &replace_dict)
{
	int rc = 0;

	if (!node) {
		LOG_ERROR << "Empty node found";
		return 1;
	}

	if (node->is_node_type(AST::NodeType::Block)) {
		const auto &block = AST::cast_to<AST::Block>(node);
		std::string scope = block->get_scope();
		if (scope.size()) {
			if (replace_dict.count(scope)) {
				const std::string &new_scope = replace_dict[scope];
				block->set_scope(new_scope);
			}
		}
	}

	const auto &children = node->get_children();
	for (const auto &child: *children) {
		rc |= annotate_names(child, replace_dict);
	}

	return rc;
}

}
}
}
