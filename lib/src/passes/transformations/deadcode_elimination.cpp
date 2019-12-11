#include "./transformation_helpers.hpp"
#include "veriparse/AST/pragmalist.hpp"

#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/passes/analysis/task.hpp>
#include <veriparse/passes/analysis/function.hpp>
#include <veriparse/passes/analysis/lvalue.hpp>
#include <veriparse/passes/transformations/deadcode_elimination.hpp>
#include <veriparse/logger/logger.hpp>

#include <cstddef>
#include <sstream>


namespace
{

// Workaround
template<class T>
std::string print_set(const std::set<T> &set)
{
	std::stringstream ss;
	typename std::set<T>::const_iterator it = set.cbegin();
	typename std::set<T>::const_iterator it_end = set.cend();
	if(it == it_end) return ss.str();
	while(1) {
		ss << *it;
		if(++it != it_end)
			ss << ", ";
		else
			break;
	}
	return ss.str();
}
}

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

// boost::log::formatting_ostream& operator<<(boost::log::formatting_ostream& os,
//                                            const DeadcodeElimination::DSet &set)
// {
// 	auto it = set.cbegin();
// 	auto it_end = set.cend();
// 	if (it == it_end) {
// 		return os;
// 	}
// 	while (1) {
// 		os << *it;
// 		if(++it != it_end)
// 			os << ", ";
// 		else
// 			break;
// 	}
// 	return os;
// }


int DeadcodeElimination::process(AST::Node::Ptr node, AST::Node::Ptr parent)
{
	while (remove_deadcode_step(node, parent).size() != 0);

	DSet iodirs = to_set(Analysis::Module::get_iodir_names(node));
	DSet identifiers;
	if (collect_identifier(identifiers, node)) {
		return 1;
	}
	while (remove_unused_decl(identifiers, iodirs, node, parent) != 0);

	return remove_emptyattr(node, parent);
}


int DeadcodeElimination::analyze_identifiers(AST::Node::Ptr node, DSet &identifiers)
{
	int rc = 0;

	if (!node) {
		LOG_ERROR << "nullptr node";
		return 1;
	}

	if (node->is_node_category(AST::NodeType::IODir)) {
		// We do not want to remove IO so we add them
		const auto &io = AST::cast_to<AST::IODir>(node);
		identifiers.insert(io->get_name());
	}
	else if (node->is_node_type(AST::NodeType::Function)) {
		// Look for local variables
		auto locals = merge_set(to_set(Analysis::Function::get_iodir_names(node)),
		                        to_set(Analysis::Function::get_variable_names(node)));

		// Create a new identifiers and recurse
		DSet new_identifiers;
		AST::Node::ListPtr children = node->get_children();
		for (AST::Node::Ptr child: *children) {
			rc += analyze_identifiers(child, new_identifiers);
		}

		// Merge result without local variables
		for (const auto &name: new_identifiers) {
			if (locals.count(name) == 0) {
				identifiers.insert(name);
			}
		}
	}
	else if (node->is_node_type(AST::NodeType::Task)) {
		// Look for local variables
		auto locals = merge_set(to_set(Analysis::Task::get_iodir_names(node)),
		                        to_set(Analysis::Task::get_variable_names(node)));

		// Create a new identifiers and recurse
		DSet new_identifiers;
		AST::Node::ListPtr children = node->get_children();
		for (AST::Node::Ptr child: *children) {
			rc += analyze_identifiers(child, new_identifiers);
		}

		// Merge result without local variables
		for (const auto &name: new_identifiers) {
			if (locals.count(name) == 0) {
				identifiers.insert(name);
			}
		}
	}
	else if (node->is_node_type(AST::NodeType::Lvalue)) {
		const auto &lvalue = AST::cast_to<AST::Lvalue>(node);
		for (const auto &name: Analysis::Lvalue::get_rvalue_names(lvalue)) {
			identifiers.insert(name);
		}
	}
	else if (node->is_node_type(AST::NodeType::Identifier)) {
		const auto &id = AST::cast_to<AST::Identifier>(node);
		identifiers.insert(id->get_name());
	}
	else if (node->is_node_type(AST::NodeType::Parameter)) {
		const auto &param = AST::cast_to<AST::Parameter>(node);
		identifiers.insert(param->get_name());
	}
	else if (node->is_node_type(AST::NodeType::Localparam)) {
		const auto &lparam = AST::cast_to<AST::Localparam>(node);
		identifiers.insert(lparam->get_name());
	}
	else {
		AST::Node::ListPtr children = node->get_children();
		for (AST::Node::Ptr child: *children) {
			rc += analyze_identifiers(child, identifiers);
		}
	}

	return rc;
}


DeadcodeElimination::DSet DeadcodeElimination::remove_deadcode_step(AST::Node::Ptr node,
                                                                    AST::Node::Ptr parent)
{
	const auto &lvalue_nodes = Analysis::Module::get_lvalue_nodes(node);
	DSet lvalue_set;
	for(AST::Lvalue::Ptr lvalue_node: *lvalue_nodes) {
		collect_identifier(lvalue_set, node);
	}

	DSet identifiers;
	analyze_identifiers(node, identifiers);
	LOG_DEBUG_N(node) << "identifiers set: " << print_set(identifiers);

	DSet deadset;

	for(const std::string &a: lvalue_set) {
		if(!identifiers.count(a)) {
			deadset.insert(a);
		}
	}

#ifdef FULL_DEBUG
	for(const std::string &dead: deadset) {
		LOG_DEBUG_F(node->get_filename()) << "dead: " << dead;
	}
#endif
	DSet removedset;
	remove_deadstmt(deadset, removedset, node, parent);
	remove_deaddecl(removedset, node, parent);
	remove_emptyblock(node, parent);
	remove_emptystmt(node, parent);

	return removedset;
}

int DeadcodeElimination::remove_deadstmt(const DeadcodeElimination::DSet &deadset,
                                         DeadcodeElimination::DSet &removedset,
                                         AST::Node::Ptr node, AST::Node::Ptr parent)
{
	int rc = 0;

	if (!node) {
		LOG_ERROR << "Empty node, parent is " << parent;
		return 1;
	}

	if (node->is_node_type(AST::NodeType::Function)) {
		// Nothing
	}

	else if (node->is_node_type(AST::NodeType::Task)) {
		// Nothing
	}

	else if(node->is_node_category(AST::NodeType::Assign)) {
		auto assign = AST::cast_to<AST::Assign>(node);
		// Looking for lvalues for the current assignment
		DSet lvalue_set = to_set(Analysis::Lvalue::get_lvalue_names(assign->get_left()));

		// Compute the intersection with the dead set.
		std::set<std::string> inter_set;
		std::set_intersection(deadset.begin(), deadset.end(), lvalue_set.begin(), lvalue_set.end(),
		                      std::inserter(inter_set, inter_set.begin()));

		LOG_DEBUG_N(node) << "lvalue_set={" << print_set(lvalue_set) << "} - "
		                  << "inter_set={" << print_set(inter_set) << "}";

		// If the intersection is equal to the lvalue_set
		if(inter_set == lvalue_set) {
			// We can remove safely the stmt
			LOG_INFO_N(node) << "removing " << print_set(lvalue_set) << " assignation";
			parent->remove(node);
			removedset.insert(lvalue_set.begin(), lvalue_set.end());
		}
	}

	else {
		AST::Node::ListPtr children = node->get_children();
		for (AST::Node::Ptr child: *children) {
			rc |= remove_deadstmt(deadset, removedset, child, node);
		}
	}

	return rc;
}

int DeadcodeElimination::remove_deaddecl(const DeadcodeElimination::DSet &removedset,
                                         AST::Node::Ptr node, AST::Node::Ptr parent)
{
	int rc = 0;
	if (node) {
		if (node->is_node_type(AST::NodeType::Function)) {
			// Nothing
		}

		else if (node->is_node_type(AST::NodeType::Task)) {
			// Nothing
		}

		else if (node->is_node_category(Veriparse::AST::NodeType::Variable)) {
			AST::Variable::Ptr var = AST::cast_to<AST::Variable>(node);
			if(removedset.count(var->get_name()) != 0) {
				parent->remove(node);
				LOG_INFO_N(node) << "removing "
				                 << var->get_node_type() << ' '
				                 << var->get_name() << " declaration";
			}
		}
		else {
			AST::Node::ListPtr children = node->get_children();
			for (AST::Node::Ptr child: *children) {
				rc |= remove_deaddecl(removedset, child, node);
			}
		}
	}
	else {
		LOG_ERROR << "Empty node, parent is " << parent;
		return 1;
	}

	return rc;
}

int DeadcodeElimination::remove_emptyblock(AST::Node::Ptr node, AST::Node::Ptr parent)
{
	int rc = 0;

	if(node) {
		switch(node->get_node_type()) {
		case AST::NodeType::Function:
			break;

		case AST::NodeType::Task:
			break;

		case AST::NodeType::Block:
			{
				AST::Node::ListPtr children = node->get_children();
				for (AST::Node::Ptr child: *children) {
					rc |= remove_emptyblock(child, node);
				}

				AST::Block::Ptr block = AST::cast_to<AST::Block>(node);
				if(!block->get_statements()) {
					parent->remove(node);
				}
			}
			break;

		default:
			AST::Node::ListPtr children = node->get_children();
			for (AST::Node::Ptr child: *children) {
				rc |= remove_emptyblock(child, node);
			}
		}
	}

	return rc;
}

int DeadcodeElimination::remove_emptystmt(AST::Node::Ptr node, AST::Node::Ptr parent)
{
	int rc = 0;

	if(node) {
		switch(node->get_node_type()) {
		case AST::NodeType::Function:
			break;

		case AST::NodeType::Task:
			break;

		case AST::NodeType::IfStatement:
			{
				AST::Node::ListPtr children = node->get_children();
				for (AST::Node::Ptr child: *children) {
					rc |= remove_emptystmt(child, node);
				}

				AST::IfStatement::Ptr if_stmt = AST::cast_to<AST::IfStatement>(node);
				AST::Node::Ptr true_stmt = if_stmt->get_true_statement();
				AST::Node::Ptr false_stmt = if_stmt->get_false_statement();
				if(!true_stmt && !false_stmt) {
					parent->remove(node);
					LOG_INFO_N(if_stmt) << "removing if statement";
				}
				else if(!true_stmt && false_stmt) {
					true_stmt = std::make_shared<AST::Block>(if_stmt->get_filename(), if_stmt->get_line());
					if_stmt->set_true_statement(true_stmt);
				}
			}
			break;

		case AST::NodeType::Always:
			{
				AST::Node::ListPtr children = node->get_children();
				for (AST::Node::Ptr child: *children) {
					rc |= remove_emptystmt(child, node);
				}

				AST::Always::Ptr always = AST::cast_to<AST::Always>(node);
				if(!always->get_statement()) {
					parent->remove(node);
					LOG_INFO_N(always) << "removing always statement";
				}
			}
			break;

		case AST::NodeType::ForStatement:
			{
				AST::Node::ListPtr children = node->get_children();
				for (AST::Node::Ptr child: *children) {
					rc |= remove_emptystmt(child, node);
				}

				AST::ForStatement::Ptr for_stmt = AST::cast_to<AST::ForStatement>(node);
				if(!for_stmt->get_statement()) {
					parent->remove(node);
					LOG_INFO_N(for_stmt) << "removing for statement";
				}
			}
			break;

		case AST::NodeType::WhileStatement:
			{
				AST::Node::ListPtr children = node->get_children();
				for (AST::Node::Ptr child: *children) {
					rc |= remove_emptystmt(child, node);
				}

				AST::WhileStatement::Ptr while_stmt = AST::cast_to<AST::WhileStatement>(node);
				if(!while_stmt->get_statement()) {
					parent->remove(node);
					LOG_INFO_N(while_stmt) << "removing while statement";
				}
			}
			break;

		default:
			AST::Node::ListPtr children = node->get_children();
			for (AST::Node::Ptr child: *children) {
				rc |= remove_emptystmt(child, node);
			}
		}
	}

	return rc;
}

int DeadcodeElimination::remove_emptyattr(AST::Node::Ptr node, AST::Node::Ptr parent)
{
	int rc = 0;

	if(!node) {
		return rc;
	}

	if (node->is_node_type(AST::NodeType::Pragmalist)) {
		const auto &stmts = AST::cast_to<AST::Pragmalist>(node)->get_statements();
		if (!stmts || stmts->empty()) {
			parent->remove(node);
		}
	}

	AST::Node::ListPtr children = node->get_children();
	for (AST::Node::Ptr child: *children) {
		rc |= remove_emptyattr(child, node);
	}

	return rc;
}

int DeadcodeElimination::remove_unused_decl(const DeadcodeElimination::DSet &identifiers,
                                            const DeadcodeElimination::DSet &iodirs,
                                            const AST::Node::Ptr &node, AST::Node::Ptr parent)
{
	int rc = 0;

	if (!node) {
		return 0;
	}

	if (node->is_node_category(AST::NodeType::VariableBase)) {
		const auto &varbase = AST::cast_to<AST::VariableBase>(node);
		if (identifiers.count(varbase->get_name()) == 0 && iodirs.count(varbase->get_name()) == 0) {
			if (!parent) {
				LOG_ERROR_N(node) << "the parent node is null";
				return -1;
			}
			parent->remove(node);
			rc += 1;
		}
	}
	else {
		const auto &children = node->get_children();
		for (AST::Node::Ptr child: *children) {
			rc += remove_unused_decl(identifiers, iodirs, child, node);
		}
	}

	return rc;
}

int DeadcodeElimination::collect_identifier(DeadcodeElimination::DSet &identifiers, const AST::Node::Ptr &node)
{
	int rc = 0;

	if (!node) {
		return 0;
	}

	if (node->is_node_type(AST::NodeType::Identifier)) {
		const auto &identifier = AST::cast_to<AST::Identifier>(node);
		if (!identifier->get_scope()) {
			identifiers.emplace(identifier->get_name());
		}
	}
	else {
		const auto &children = node->get_children();
		for (AST::Node::Ptr child: *children) {
			rc |= collect_identifier(identifiers, child);
		}
	}

	return rc;
}


}
}
}
