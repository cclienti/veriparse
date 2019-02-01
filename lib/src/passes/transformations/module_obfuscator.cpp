#include "transformation_helpers.hpp"

#include <veriparse/passes/transformations/module_obfuscator.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/passes/analysis/function.hpp>
#include <veriparse/passes/analysis/task.hpp>
#include <veriparse/misc/string_utils.hpp>
#include <veriparse/logger/logger.hpp>

#include <functional>
#include <string>
#include <random>
#include <set>


namespace Veriparse {
namespace Passes {
namespace Transformations {


int ModuleObfuscator::process(AST::Node::Ptr node, AST::Node::Ptr parent)
{
	if (!node) {
		return 0;
	}

	auto decls = AST::to_node_list(Analysis::Module::get_variable_nodes_within_module(node));

	auto localparams = AST::to_node_list(Analysis::Module::get_localparam_nodes(node));
	decls->insert(decls->end(), localparams->begin(), localparams->end());

	if (!node->is_node_type(AST::NodeType::Function)) {
		auto functions = AST::to_node_list(Analysis::Module::get_function_nodes(node));
		decls->insert(decls->end(), functions->begin(), functions->end());
	}

	if (!node->is_node_type(AST::NodeType::Task)) {
		auto tasks = AST::to_node_list(Analysis::Module::get_task_nodes(node));
		decls->insert(decls->end(), tasks->begin(), tasks->end());
	}

	for (const auto &decl: *decls) {
		std::string decl_name;
		if (decl->is_node_category(AST::NodeType::Variable)) {
			decl_name = AST::cast_to<AST::Variable>(decl)->get_name();
		}
		else if (decl->is_node_type(AST::NodeType::Localparam)) {
			decl_name = AST::cast_to<AST::Localparam>(decl)->get_name();
		}
		else if (decl->is_node_type(AST::NodeType::Function)) {
			decl_name = AST::cast_to<AST::Function>(decl)->get_name();
		}
		else if (decl->is_node_type(AST::NodeType::Task)) {
			decl_name = AST::cast_to<AST::Task>(decl)->get_name();
		}
		else {
			LOG_FATAL_N(node) << "Unexpected node type, expecting Variable or Localparam";
			return 1;
		}

		push_decl(decl, decl_name);
	}

	if (ASTReplace::replace_identifier(node, m_replace_map)) {
		LOG_ERROR_N(node) << "Cannot obfuscate the module";
		return 1;
	}

	if (rename_locals(node)) {
		LOG_ERROR_N(node) << "Cannot obfuscate the module";
		return 1;
	}

	return 0;
}


std::string ModuleObfuscator::push_decl(AST::Node::Ptr decl, const std::string &decl_name,
                                        bool override_collision)
{
	std::hash<std::string> hash;
	std::string obf_name;

	auto id = std::make_shared<AST::Identifier>(decl->get_filename(), decl->get_line());

	if (m_use_hash) {
		obf_name = std::to_string(hash(decl_name));
		obf_name.insert(0, "h_");
		if (m_unique_id.count(obf_name)) {
			LOG_DEBUG_N(decl) << "Collision for name: " << decl_name << " hash:" << obf_name;
			obf_name = Analysis::UniqueDeclaration::get_unique_identifier("h", m_unique_id, m_identifier_length);
			LOG_DEBUG_N(decl) << "Collision resolved with: " << obf_name;
		}
		else {
			m_unique_id.insert(obf_name);
		}
	}
	else {
		obf_name = Analysis::UniqueDeclaration::get_unique_identifier("h", m_unique_id, m_identifier_length);
		m_unique_id.insert(obf_name);
	}

	id->set_name(obf_name);

	LOG_DEBUG_N(decl) << "Obfuscating '" << decl_name << "' with '" << id->get_name() << "'";

	auto result = m_replace_map.emplace(decl_name, id);
	if (result.second == false) {
		if (override_collision) {
			result.first->second = id;
		}
		else {
			LOG_ERROR_N(decl) << "Multiple declaration of " << decl_name;
		}
	}

	return obf_name;
}


int ModuleObfuscator::rename_locals(const AST::Node::Ptr &node)
{
	int ret = 0;

	if (!node) {
		return 0;
	}

	if (node->is_node_category(AST::NodeType::Variable)) {
		const auto &variable = AST::cast_to<AST::Variable>(node);
		auto it = m_replace_map.find(variable->get_name());
		if (it != m_replace_map.end()) {
			auto value = AST::cast_to<AST::Identifier>(it->second);
			LOG_DEBUG_N(node) << "AST::NodeType::Variable: Renaming "
			                  << variable->get_name() << " with " << value->get_name();
			variable->set_name(value->get_name());
		}
	}
	else if (node->is_node_type(AST::NodeType::Localparam)) {
		const auto &localparam = AST::cast_to<AST::Localparam>(node);
		auto it = m_replace_map.find(localparam->get_name());
		if (it != m_replace_map.end()) {
			auto value = AST::cast_to<AST::Identifier>(it->second);
			LOG_DEBUG_N(node) << "AST::NodeType::Localparam: Renaming "
			                  << localparam->get_name() << " with " << value->get_name();
			localparam->set_name(value->get_name());
		}
	}
	else if (node->is_node_type(AST::NodeType::FunctionCall)) {
		const auto &func_call = AST::cast_to<AST::FunctionCall>(node);
		auto it = m_replace_map.find(func_call->get_name());
		if (it != m_replace_map.end()) {
			auto value = AST::cast_to<AST::Identifier>(it->second);
			LOG_DEBUG_N(node) << "AST::NodeType::FunctionCall: Renaming "
			                  << func_call->get_name() << " with " << value->get_name();
			func_call->set_name(value->get_name());
		}
	}
	else if (node->is_node_type(AST::NodeType::TaskCall)) {
		const auto &task_call = AST::cast_to<AST::TaskCall>(node);
		auto it = m_replace_map.find(task_call->get_name());
		if (it != m_replace_map.end()) {
			auto value = AST::cast_to<AST::Identifier>(it->second);
			LOG_DEBUG_N(node) << "AST::NodeType::TaskCall: Renaming "
			                  << task_call->get_name() << " with " << value->get_name();
			task_call->set_name(value->get_name());
		}
	}
	else if (node->is_node_type(AST::NodeType::Function) || node->is_node_type(AST::NodeType::Task)) {
		auto backup = m_replace_map;
		ret = rename_procs(node);
		m_replace_map = backup;
		return ret;
	}

	auto children = node->get_children();
	for (const auto &child: *children) {
		ret += rename_locals(child);
	}

	return ret;
}

int ModuleObfuscator::rename_procs(const AST::Node::Ptr &node)
{
	int ret = 0;

	if (!node) {
		return 0;
	}

	if (node->is_node_type(AST::NodeType::Function)) {
		const auto &function = AST::cast_to<AST::Function>(node);
		auto it = m_replace_map.find(function->get_name());
		if (it != m_replace_map.end()) {
			auto value = AST::cast_to<AST::Identifier>(it->second);
			function->set_name(value->get_name());
		}

		std::set<std::string> local_set;

		auto iodirs = Analysis::Function::get_iodir_nodes(node);
		if (iodirs) {
			for (const auto &iodir: *iodirs) {
				local_set.emplace(iodir->get_name());
				push_decl(iodir, iodir->get_name(), true);
			}
		}

		auto variables = Analysis::Task::get_variable_nodes(node);
		if (variables) {
			for (const auto &variable: *variables) {
				if (local_set.count(variable->get_name()) == 0) {
					push_decl(variable, variable->get_name(), true);
				}
			}
		}
	}
	else if (node->is_node_type(AST::NodeType::Task)) {
		const auto &task = AST::cast_to<AST::Task>(node);
		auto it = m_replace_map.find(task->get_name());
		if (it != m_replace_map.end()) {
			auto value = AST::cast_to<AST::Identifier>(it->second);
			task->set_name(value->get_name());
		}

		std::set<std::string> local_set;

		auto iodirs = Analysis::Task::get_iodir_nodes(node);
		if (iodirs) {
			for (const auto &iodir: *iodirs) {
				local_set.emplace(iodir->get_name());
				push_decl(iodir, iodir->get_name(), true);
			}
		}

		auto variables = Analysis::Task::get_variable_nodes(node);
		if (variables) {
			for (const auto &variable: *variables) {
				if (local_set.count(variable->get_name()) == 0) {
					push_decl(variable, variable->get_name(), true);
				}
			}
		}
	}
	else if (node->is_node_category(AST::NodeType::IODir)) {
		const auto &iodir = AST::cast_to<AST::IODir>(node);
		auto it = m_replace_map.find(iodir->get_name());
		if (it != m_replace_map.end()) {
			auto value = AST::cast_to<AST::Identifier>(it->second);
			LOG_DEBUG_N(node) << "AST::NodeType::IODir: Renaming "
			                  << iodir->get_name() << " with " << value->get_name();
			iodir->set_name(value->get_name());
		}
	}
	else if (node->is_node_category(AST::NodeType::Variable)) {
		const auto &variable = AST::cast_to<AST::Variable>(node);
		auto it = m_replace_map.find(variable->get_name());
		if (it != m_replace_map.end()) {
			auto value = AST::cast_to<AST::Identifier>(it->second);
			LOG_DEBUG_N(node) << "AST::NodeType::Variable: Renaming "
			                  << variable->get_name() << " with " << value->get_name();
			variable->set_name(value->get_name());
		}
	}
	else if (node->is_node_category(AST::NodeType::Identifier)) {
		const auto &identifier = AST::cast_to<AST::Identifier>(node);
		auto it = m_replace_map.find(identifier->get_name());
		if (it != m_replace_map.end()) {
			auto value = AST::cast_to<AST::Identifier>(it->second);
			LOG_DEBUG_N(node) << "AST::NodeType::Identifier: Renaming "
			                  << identifier->get_name() << " with " << value->get_name();
			identifier->set_name(value->get_name());
		}
	}
	else if (node->is_node_type(AST::NodeType::FunctionCall)) {
		const auto &func_call = AST::cast_to<AST::FunctionCall>(node);
		auto it = m_replace_map.find(func_call->get_name());
		if (it != m_replace_map.end()) {
			auto value = AST::cast_to<AST::Identifier>(it->second);
			LOG_DEBUG_N(node) << "AST::NodeType::FunctionCall: Renaming "
			                  << func_call->get_name() << " with " << value->get_name();
			func_call->set_name(value->get_name());
		}
	}
	else if (node->is_node_type(AST::NodeType::TaskCall)) {
		const auto &task_call = AST::cast_to<AST::TaskCall>(node);
		auto it = m_replace_map.find(task_call->get_name());
		if (it != m_replace_map.end()) {
			auto value = AST::cast_to<AST::Identifier>(it->second);
			LOG_DEBUG_N(node) << "AST::NodeType::TaskCall: Renaming "
			                  << task_call->get_name() << " with " << value->get_name();
			task_call->set_name(value->get_name());
		}
	}

	auto children = node->get_children();
	for (const auto &child: *children) {
		ret += rename_procs(child);
	}

	return ret;
}


}
}
}
