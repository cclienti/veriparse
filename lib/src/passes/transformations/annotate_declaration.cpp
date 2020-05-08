#include "./transformation_helpers.hpp"

#include <veriparse/passes/transformations/annotate_declaration.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/passes/analysis/function.hpp>
#include <veriparse/passes/analysis/task.hpp>
#include <veriparse/logger/logger.hpp>

#include <algorithm>
#include <iterator>


namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

AnnotateDeclaration::AnnotateDeclaration():
	m_ignore_io (true),
	m_search    ("^.*$"),
	m_replace   ("$&")
{}

AnnotateDeclaration::AnnotateDeclaration(const std::string &search, const std::string &replace, bool ignore_io):
	m_ignore_io (ignore_io),
	m_search    (search),
	m_replace   (replace)
{}

void AnnotateDeclaration::set_search_replace(const std::string &search,
                                             const std::string &replace)
{
	m_search = search;
	m_replace = replace;
}

int AnnotateDeclaration::process(AST::Node::Ptr node, AST::Node::Ptr parent)
{
	ReplaceDict replace_dict;
	std::set<std::string> excluded_names;

	if (m_ignore_io) {
		std::vector<std::string> iodir_names = Analysis::Module::get_iodir_names(node);
		excluded_names.insert(iodir_names.cbegin(), iodir_names.cend());
	}

	if(get_declaration_names(node, replace_dict, excluded_names))
		return 1;

	if(annotate_names(node, replace_dict))
		return 1;

	return 0;
}

int AnnotateDeclaration::get_declaration_names(const AST::Node::Ptr node, ReplaceDict &replace_dict,
                                               const std::set<std::string> &excluded_names)
{
	int rc = 0;

	if (!node) {
		LOG_ERROR << "Empty node found";
		return 1;
	}

	bool found_name = true;
	std::string name;

	if (node->is_node_category(AST::NodeType::VariableBase)) {
		const auto &varbase = AST::cast_to<AST::VariableBase>(node);
		name = varbase->get_name();
	}
	else if (node->is_node_type(AST::NodeType::Instance)) {
		const auto &instance = AST::cast_to<AST::Instance>(node);
		name = instance->get_name();
	}
	else if (node->is_node_type(AST::NodeType::Task)) {
		const auto &task = AST::cast_to<AST::Task>(node);
		name = task->get_name();
	}
	else if (node->is_node_type(AST::NodeType::Function)) {
		const auto &function = AST::cast_to<AST::Function>(node);
		name = function->get_name();
	}
	else if (node->is_node_type(AST::NodeType::Localparam)) {
		const auto &localparam = AST::cast_to<AST::Localparam>(node);
		name = localparam->get_name();
	}
	else {
		found_name = false;
		const auto &children = node->get_children();
		for (const auto &child: *children) {
			rc |= get_declaration_names(child, replace_dict, excluded_names);
		}
	}

	if (found_name) {
		if (excluded_names.count(name) == 0) {
			replace_dict[name] = std::regex_replace(name, m_search, m_replace);
		}
	}

	return rc;
}

int AnnotateDeclaration::annotate_names(AST::Node::Ptr node, ReplaceDict &replace_dict)
{
	int rc = 0;

	if (!node) {
		LOG_ERROR << "Empty node found";
		return 1;
	}

	if(node->is_node_category(AST::NodeType::VariableBase)) {
		const auto &variable = AST::cast_to<AST::VariableBase>(node);
		const std::string &name = variable->get_name();
		if(replace_dict.count(name)) {
			const std::string &new_name = replace_dict[name];
			variable->set_name(new_name);
		}
	}

	else if (node->is_node_type(AST::NodeType::Identifier)) {
		const auto &identifier = AST::cast_to<AST::Identifier>(node);
		const std::string &name = identifier->get_name();
		if(replace_dict.count(name)) {
			const std::string &new_name = replace_dict[name];
			identifier->set_name(new_name);
		}
	}

	else if (node->is_node_type(AST::NodeType::Localparam)) {
		const auto &localparam = AST::cast_to<AST::Localparam>(node);
		const std::string &name = localparam->get_name();
		if(replace_dict.count(name)) {
			const std::string &new_name = replace_dict[name];
			localparam->set_name(new_name);
		}
	}

	else if (node->is_node_type(AST::NodeType::Instance)) {
		const auto &instance = AST::cast_to<AST::Instance>(node);
		const std::string &name = instance->get_name();
		if(replace_dict.count(name)) {
			const std::string &new_name = replace_dict[name];
			instance->set_name(new_name);
		}
	}

	else if (node->is_node_type(AST::NodeType::Function)) {
		const auto &function = AST::cast_to<AST::Function>(node);
		const std::string &name = function->get_name();
		if(replace_dict.count(name)) {
			const std::string &new_name = replace_dict[name];
			function->set_name(new_name);
		}
		std::set<std::string> locals = merge_set(to_set(Analysis::Function::get_iodir_names(node)),
															  to_set(Analysis::Function::get_variable_names(node)));
		ReplaceDict new_replace_dict = remove_keys(replace_dict, locals);
		const auto &children = node->get_children();
		for (const auto &child: *children) {
			rc |= annotate_names(child, new_replace_dict);
		}
		return rc;
	}

	else if (node->is_node_type(AST::NodeType::FunctionCall)) {
		const auto &fcall = AST::cast_to<AST::FunctionCall>(node);
		const std::string &name = fcall->get_name();
		if(replace_dict.count(name)) {
			const std::string &new_name = replace_dict[name];
			fcall->set_name(new_name);
		}
	}

	else if (node->is_node_type(AST::NodeType::Task)) {
		const auto &task = AST::cast_to<AST::Task>(node);
		const std::string &name = task->get_name();
		if(replace_dict.count(name)) {
			const std::string &new_name = replace_dict[name];
			task->set_name(new_name);
		}
		std::set<std::string> locals = merge_set(to_set(Analysis::Task::get_iodir_names(node)),
															  to_set(Analysis::Task::get_variable_names(node)));
		ReplaceDict new_replace_dict = remove_keys(replace_dict, locals);
		const auto &children = node->get_children();
		for (const auto &child: *children) {
			rc |= annotate_names(child, new_replace_dict);
		}
		return rc;
	}

	else if (node->is_node_type(AST::NodeType::TaskCall)) {
		const auto &tcall = AST::cast_to<AST::TaskCall>(node);
		const std::string &name = tcall->get_name();
		if(replace_dict.count(name)) {
			const std::string &new_name = replace_dict[name];
			tcall->set_name(new_name);
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
