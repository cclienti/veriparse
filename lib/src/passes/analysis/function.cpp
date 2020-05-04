#include <veriparse/passes/analysis/function.hpp>
#include <veriparse/logger/logger.hpp>
#include <veriparse/AST/nodes.hpp>
#include <algorithm>
#include <set>


namespace Veriparse
{
namespace Passes
{
namespace Analysis
{

AST::Ioport::ListPtr Function::get_ioport_nodes(AST::Node::Ptr node)
{
	AST::Ioport::ListPtr list = std::make_shared<AST::Ioport::List>();
	get_node_list_by_category<AST::Ioport>(node, AST::NodeType::Ioport, list);
	return list;
}

AST::IODir::ListPtr Function::get_iodir_nodes(AST::Node::Ptr node)
{
	AST::IODir::ListPtr list = std::make_shared<AST::IODir::List>();
	get_node_list_by_category<AST::IODir>(node, AST::NodeType::IODir, list);
	return list;
}

std::vector<std::string> Function::get_iodir_names(AST::Node::Ptr node)
{
	AST::IODir::ListPtr iodirs = get_iodir_nodes(node);
	return get_property_in_list<std::string, AST::IODir>
		(iodirs, [](AST::IODir::Ptr n){return n->get_name();});
}


AST::Variable::ListPtr Function::get_variable_nodes(AST::Node::Ptr node)
{
	AST::Variable::ListPtr list = std::make_shared<AST::Variable::List>();
	get_node_list_by_category<AST::Variable>(node, AST::NodeType::Variable, list);
	return list;
}

std::vector<std::string> Function::get_variable_names(AST::Node::Ptr node)
{
	AST::Variable::ListPtr variables = get_variable_nodes(node);
	return get_property_in_list<std::string, AST::Variable>
		(variables, [](AST::Variable::Ptr n){return n->get_name();});
}

bool Function::is_like_automatic(const AST::Function::Ptr &node)
{
	if (node->get_automatic()) {
		return true;
	}

	std::set<std::string> io_set;
	std::set<std::string> var_set;

	for (const auto &var: Function::get_variable_names(node)) {
		var_set.emplace(var);
	}

	if (var_set.empty()) {
		return true;
	}

	const auto iodirs = Function::get_iodir_nodes(node);
	for (const auto &io: *iodirs) {
		if (io->is_node_type(AST::NodeType::Input)) {
			io_set.emplace(io->get_name());
		}
		else {
			return false;
		}
	}

	if (var_set == io_set) {
		return true;
	}

	return false;
}

}
}
}
