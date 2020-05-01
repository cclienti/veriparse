#include "veriparse/AST/ioport.hpp"
#include <veriparse/passes/analysis/function.hpp>
#include <veriparse/logger/logger.hpp>
#include <algorithm>


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

}
}
}
