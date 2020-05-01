#include <veriparse/passes/transformations/scope_elevator.hpp>
#include <veriparse/passes/transformations/ast_replace.hpp>
#include <veriparse/logger/logger.hpp>
#include <veriparse/misc/utils.hpp>
#include <veriparse/AST/nodes.hpp>


namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

namespace
{

std::string render_scope_stack(const ScopeElevator::ScopeStack &stack, bool drop_top = false)
{
	std::string scope_value;

	for (const auto &scope: Misc::Utils::viter_drop(stack, false, drop_top)) {
		if (scope_value.empty()) {
			scope_value = scope;
		}
		else {
			scope_value += "." + scope;
		}
	}

	return scope_value;
}

// We basically render scoped identifier without taking into account
// ranges such as "[i]". This is not neccessay to identify properly
// the scope hierarchy and it ease the matching.
std::string render_identifier(const AST::Identifier::Ptr &id)
{
	std::string render;

	const auto &scope = id->get_scope();
	if (scope) {
		const auto &labels = scope->get_labellist();
		if (labels) {
			for (const auto &label: *labels) {
				render += label->get_scope() + ".";
			}
		}
	}

	return render + id->get_name();
}

}


int ScopeElevator::process(AST::Node::Ptr node, AST::Node::Ptr parent)
{
	m_scope_stack.clear();
	if (process_variables(node, parent)) {
		return 1;
	}

	for (const auto &elt: m_global_replace_map) {
		LOG_DEBUG_N(node) << "Global Replace Map " << elt.first << " -> "
		                  << "Scope[" << render_scope_stack(elt.second.first) << "] "
		                  << "id [" << elt.second.second << "]";
	}

	m_scope_stack.clear();
	if (process_scoped_identifiers(node, parent)) {
		return 1;
	}

	return 0;
}

int ScopeElevator::process_variables(const AST::Node::Ptr &node, AST::Node::Ptr &parent)
{
	int ret = 0;

	if (!node) {
		return 0;
	}

	auto recfct = [this](AST::Node::Ptr node, AST::Node::Ptr parent) -> int {
		              return this->process_variables(node, parent);
	              };

	switch (node->get_node_type()) {
	case AST::NodeType::Block:
		{
			const auto &block = AST::cast_to<AST::Block>(node);
			const auto &scope = block->get_scope();

			if (!scope.empty()) {
				m_scope_stack.push_back(scope);
				LOG_DEBUG_N(node) << "Current scope: "
				                  << render_scope_stack(m_scope_stack);
			}


			if (recurse_in_childs(node, recfct)) {
				LOG_ERROR_N(node) << "Error during managing scope '" << scope << "'";
				ret += 1;
			}

			if (!scope.empty()) {
				if (parent->is_node_type(AST::NodeType::Block)) {
					LocalReplaceMap rmap;
					rename_nested_variables(node, parent, scope, rmap);
					rename_nested_identifiers(node, parent, scope, rmap);
					pickup_statements(parent, node, block->get_statements());
				}
			}
			else {
				pickup_statements(parent, node, block->get_statements());
			}

			if (!scope.empty()) {
				m_scope_stack.pop_back();
			}
		}
		break;

	default:
		ret += recurse_in_childs(node, recfct);
	}

	return ret;
}

int ScopeElevator::process_scoped_identifiers(const AST::Node::Ptr &node, AST::Node::Ptr &parent)
{
	int ret = 0;

	if (!node) {
		return 0;
	}

	auto recfct = [this](AST::Node::Ptr node, AST::Node::Ptr parent) -> int {
		              return this->process_scoped_identifiers(node, parent);
	              };

	switch (node->get_node_type()) {
	case AST::NodeType::Block:
		{
			const auto &block = AST::cast_to<AST::Block>(node);
			const auto &scope = block->get_scope();

			if (!scope.empty()) {
				m_scope_stack.push_back(scope);
			}

			if (recurse_in_childs(node, recfct)) {
				LOG_ERROR_N(node) << "Error during managing scope '" << scope << "'";
				ret += 1;
			}

			if (!scope.empty()) {
				m_scope_stack.pop_back();
			}
		}
		break;

	case AST::NodeType::Identifier:
		{
			const auto &id = AST::cast_to<AST::Identifier>(node);
			const auto &scope = id->get_scope();
			if (!scope) {
				break;
			}

			// We must process one scope level at a time. The
			// process_variables function may removed multiple nested
			// scopes but the global map recorded each removed scope
			// independently (ie the map represent a graph). See the
			// m_global_replace_map affectation in
			// rename_nested_variables() function.
			bool search = true;
			while(search) {
				search = false;

				std::list<GlobalReplaceMap::iterator> search_list;
				const auto &id_str = render_identifier(id);
				search_list.push_front(m_global_replace_map.find(id_str));

				std::string acc_scope;
				for (const auto &elt: m_scope_stack) {
					acc_scope += elt + ".";
					search_list.push_front(m_global_replace_map.find(acc_scope + id_str));
				}

				auto itend = m_global_replace_map.end();
				for (const auto &elt: search_list) {
					if (elt != itend) {
						auto matched_scope = elt->second.first;
						auto matched_id = elt->second.second;
						LOG_DEBUG_N(node) << "matched scope: " << render_scope_stack(matched_scope);
						LOG_DEBUG_N(node) << "matched id: " << matched_id;
						id->set_name(matched_id);

						// remove the last scope as the id matched
						const auto &list = id->get_scope()->get_labellist();
						list->pop_back();

						// remove current scope at the beginning
						auto scope_it = m_scope_stack.cbegin();
						auto scope_end = m_scope_stack.cend();
						while (!list->empty() && scope_it != scope_end) {
							if (*scope_it == list->front()->get_scope()) {
								list->pop_front();
							}
							++scope_it;
						}

						// The matched occured, we continue to search as we
						// process only one scope level at a time.
						search = true;
						break;
					}
				}
			}
		}
		break;

	default:
		ret += recurse_in_childs(node, recfct);
	}

	return ret;
}

int ScopeElevator::rename_nested_variables(const AST::Node::Ptr &node, const AST::Node::Ptr &parent,
                                           const std::string &scope, LocalReplaceMap &replace_map)
{
	int ret = 0;

	if (!node) {
		return 0;
	}

	if (node->is_node_category(AST::NodeType::Variable)) {
		const auto &var = AST::cast_to<AST::Variable>(node);
		const std::string var_name = var->get_name(); // no reference here.
		const std::string &new_var_name = scope + "__" + var->get_name();

		LOG_INFO_N(node) << "Removing scope " << scope
		                 << ", renaming " << var_name << " to " << new_var_name;

		var->set_name(new_var_name);
		replace_map.emplace(std::make_pair(var_name, new_var_name));

		const auto &stack = render_scope_stack(m_scope_stack, true);

		m_global_replace_map.emplace(stack + "." + scope + "." + var_name,
		                             std::make_pair(m_scope_stack, new_var_name));
	}
	else {
		auto fct = [this, &scope, & replace_map](AST::Node::Ptr node, AST::Node::Ptr parent) -> int {
			           return this->rename_nested_variables(node, parent, scope, replace_map);
		           };
		ret = recurse_in_childs(node, fct);
	}

	return ret;
}

int ScopeElevator::rename_nested_identifiers(const AST::Node::Ptr &node, const AST::Node::Ptr &parent,
                                             const std::string &scope, const LocalReplaceMap &replace_map)
{
	int ret = 0;

	if (!node) {
		return 0;
	}

	if (node->is_node_type(AST::NodeType::Identifier)) {
		const auto &id = AST::cast_to<AST::Identifier>(node);
		if (!id->get_scope()) {
			auto it = replace_map.find(id->get_name());
			if (it != replace_map.end()) {
				id->set_name(it->second);
			}
		}
	}
	else {
		auto fct = [this, &scope, & replace_map](AST::Node::Ptr node, AST::Node::Ptr parent) -> int {
			           return this->rename_nested_identifiers(node, parent, scope, replace_map);
		           };
		ret = recurse_in_childs(node, fct);
	}

	return ret;
}

void ScopeElevator::remove_current_scope(ScopeStack &scope_stack)
{
	auto current_it = m_scope_stack.begin();
	while (!scope_stack.empty() && current_it != m_scope_stack.end()) {
		if (*current_it == scope_stack.front()) {
			scope_stack.pop_front();
		}
		++current_it;
	}
}

}
}
}
