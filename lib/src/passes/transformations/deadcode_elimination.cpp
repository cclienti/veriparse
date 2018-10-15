#include "./transformation_helpers.hpp"

#include <veriparse/passes/analysis/taskcall.hpp>
#include <veriparse/passes/analysis/systemcall.hpp>
#include <veriparse/passes/analysis/functioncall.hpp>
#include <veriparse/passes/analysis/instance.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/passes/analysis/lvalue.hpp>
#include <veriparse/passes/transformations/deadcode_elimination.hpp>
#include <veriparse/logger/logger.hpp>

#include <cstddef>
#include <sstream>

namespace
{
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

int DeadcodeElimination::process(AST::Node::Ptr node, AST::Node::Ptr parent)
{
	while (remove_deadcode_step(node, parent).size() != 0);

	DSet identifiers;
	if (collect_identifier(identifiers, node)) {
		return 1;
	}
	while (remove_unused_decl(identifiers, node, parent) != 0);

	return 0;
}

DeadcodeElimination::DSet DeadcodeElimination::remove_deadcode_step(AST::Node::Ptr node,
                                                                    AST::Node::Ptr parent)
{
	AST::Lvalue::ListPtr lvalue_nodes = Analysis::Module::get_lvalue_nodes(node);
	DSet rvalue_in_lvalue_set;
	DSet lvalue_set;
	for(AST::Lvalue::Ptr lvalue_node: *lvalue_nodes) {
		std::vector<std::string> rvalue_names = Analysis::Lvalue::get_rvalue_names(lvalue_node);
		rvalue_in_lvalue_set.insert(rvalue_names.begin(), rvalue_names.end());
		std::vector<std::string> lvalue_names = Analysis::Lvalue::get_lvalue_names(lvalue_node);
		lvalue_set.insert(lvalue_names.begin(), lvalue_names.end());
	}

	AST::Variable::ListPtr variable_nodes = Analysis::Module::get_variable_nodes(node);
	for(AST::Variable::Ptr var_node: *variable_nodes) {
		lvalue_set.insert(var_node->get_name());
	}

	DSet iodir_set = to_set(Analysis::Module::get_iodir_names(node));
	DSet localparam_set = to_set(Analysis::Module::get_localparam_names(node));
	DSet parameter_set = to_set(Analysis::Module::get_parameter_names(node));
	DSet rvalue_set = to_set(Analysis::Module::get_rvalue_identifier_names(node));

	DSet instance_arg_set;
	AST::Instance::ListPtr instances = Analysis::Module::get_instance_nodes(node);
	for(AST::Instance::Ptr instance: *instances) {
		std::vector<std::string> vec = Analysis::Instance::get_argument_identifier_names(instance);
		instance_arg_set.insert(vec.begin(), vec.end());
	}

	DSet systemcall_arg_set;
	AST::SystemCall::ListPtr systemcalls = Analysis::Module::get_systemcall_nodes(node);
	for(AST::SystemCall::Ptr systemcall: *systemcalls) {
		std::vector<std::string> vec = Analysis::SystemCall::get_argument_identifier_names(systemcall);
		systemcall_arg_set.insert(vec.begin(), vec.end());
	}

	DSet taskcall_arg_set;
	AST::TaskCall::ListPtr taskcalls = Analysis::Module::get_taskcall_nodes(node);
	for(AST::TaskCall::Ptr taskcall: *taskcalls) {
		std::vector<std::string> vec = Analysis::TaskCall::get_argument_identifier_names(taskcall);
		taskcall_arg_set.insert(vec.begin(), vec.end());
	}

	DSet functioncall_arg_set;
	AST::FunctionCall::ListPtr functioncalls = Analysis::Module::get_functioncall_nodes(node);
	for(AST::FunctionCall::Ptr functioncall: *functioncalls) {
		std::vector<std::string> vec = Analysis::FunctionCall::get_argument_identifier_names(functioncall);
		functioncall_arg_set.insert(vec.begin(), vec.end());
	}

	LOG_DEBUG_N(node) << "iodir_set: " << ::print_set(iodir_set);
	LOG_DEBUG_N(node) << "localparam_set: " << ::print_set(localparam_set);
	LOG_DEBUG_N(node) << "parameter_set: " << ::print_set(parameter_set);
	LOG_DEBUG_N(node) << "rvalue_set: " << ::print_set(rvalue_set);
	LOG_DEBUG_N(node) << "rvalue_in_lvalue_set: " << ::print_set(rvalue_in_lvalue_set);
	LOG_DEBUG_N(node) << "instance_arg_set: " << ::print_set(instance_arg_set);
	LOG_DEBUG_N(node) << "systemcall_arg_set: " << ::print_set(systemcall_arg_set);
	LOG_DEBUG_N(node) << "taskcall_arg_set: " << ::print_set(taskcall_arg_set);
	LOG_DEBUG_N(node) << "functioncall_arg_set: " << ::print_set(functioncall_arg_set);


	DSet deadset;

	for(const std::string &a: lvalue_set) {
		std::size_t count = 0;
		count += iodir_set.count(a);
		count += localparam_set.count(a);
		count += parameter_set.count(a);
		count += rvalue_set.count(a);
		count += rvalue_in_lvalue_set.count(a);
		count += instance_arg_set.count(a);
		count += systemcall_arg_set.count(a);
		count += taskcall_arg_set.count(a);
		count += functioncall_arg_set.count(a);
		if(!count) {
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

		LOG_DEBUG_N(node) << "lvalue_set={" << ::print_set(lvalue_set) << "} - "
		                  << "inter_set={" << ::print_set(inter_set) << "}";

		// If the intersection is equal to the lvalue_set
		if(inter_set == lvalue_set) {
			// We can remove safely the stmt
			LOG_INFO_N(node) << "removing " << ::print_set(lvalue_set) << " assignation";
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


int DeadcodeElimination::remove_unused_decl(const DeadcodeElimination::DSet &identifiers,
                                            const AST::Node::Ptr &node, AST::Node::Ptr parent)
{
	int rc = 0;

	if (!node) {
		return 0;
	}

	if (node->is_node_category(AST::NodeType::VariableBase)) {
		const auto &varbase = AST::cast_to<AST::VariableBase>(node);
		if (identifiers.count(varbase->get_name()) == 0) {
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
			rc += remove_unused_decl(identifiers, child, node);
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
		identifiers.emplace(identifier->get_name());
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
