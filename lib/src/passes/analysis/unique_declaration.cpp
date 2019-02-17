#include <veriparse/passes/analysis/unique_declaration.hpp>
#include <veriparse/logger/logger.hpp>
#include <algorithm>
#include <sstream>


namespace Veriparse
{
namespace Passes
{
namespace Analysis
{


thread_local UniqueDeclaration::RandGen UniqueDeclaration::s_generator;

void UniqueDeclaration::seed(UniqueDeclaration::RandGen::result_type value)
{
	s_generator.seed(value);
}

int UniqueDeclaration::analyze(const AST::Node::Ptr &node, IdentifierSet &id_set)
{
	int ret = 0;

	if (!node) {
		return 0;
	}

	auto recurse = [&]() {
		AST::Node::ListPtr children = node->get_children();
		for (AST::Node::Ptr child: *children) {
			ret += analyze(child, id_set);
		}
	};

	if (node->is_node_type(AST::NodeType::Module)) {
		id_set.emplace(AST::cast_to<AST::Module>(node)->get_name());
		recurse();
	}
	else if (node->is_node_category(AST::NodeType::Block)) {
		id_set.emplace(AST::cast_to<AST::Block>(node)->get_scope());
		recurse();
	}
	else if (node->is_node_category(AST::NodeType::ParallelBlock)) {
		id_set.emplace(AST::cast_to<AST::ParallelBlock>(node)->get_scope());
		recurse();
	}
	else if (node->is_node_category(AST::NodeType::SingleStatement)) {
		id_set.emplace(AST::cast_to<AST::SingleStatement>(node)->get_scope());
		recurse();
	}
	else if (node->is_node_type(AST::NodeType::Task)) {
		id_set.emplace(AST::cast_to<AST::Task>(node)->get_name());
	}
	else if (node->is_node_type(AST::NodeType::Function)) {
		id_set.emplace(AST::cast_to<AST::Function>(node)->get_name());
	}
	else if (node->is_node_type(AST::NodeType::Instance)) {
		id_set.emplace(AST::cast_to<AST::Instance>(node)->get_name());
		id_set.emplace(AST::cast_to<AST::Instance>(node)->get_module());
	}
	else if (node->is_node_category(AST::NodeType::Parameter)) {
		id_set.emplace(AST::cast_to<AST::Parameter>(node)->get_name());
	}
	else if (node->is_node_category(AST::NodeType::Port)) {
		id_set.emplace(AST::cast_to<AST::Port>(node)->get_name());
	}
	else if (node->is_node_category(AST::NodeType::IODir)) {
		id_set.emplace(AST::cast_to<AST::IODir>(node)->get_name());
	}
	else if (node->is_node_category(AST::NodeType::VariableBase)) {
		id_set.emplace(AST::cast_to<AST::VariableBase>(node)->get_name());
	}
	else if (node->is_node_category(AST::NodeType::Identifier)) {
		id_set.emplace(AST::cast_to<AST::Identifier>(node)->get_name());
	}
	else if (node->is_node_category(AST::NodeType::IdentifierScopeLabel)) {
		id_set.emplace(AST::cast_to<AST::IdentifierScopeLabel>(node)->get_scope());
	}
	else if (node->is_node_category(AST::NodeType::Pragma)) {
		id_set.emplace(AST::cast_to<AST::Pragma>(node)->get_name());
	}
	else {
		recurse();
	}

	return ret;
}

bool UniqueDeclaration::identifier_declaration_exists(const std::string &identifier, const IdentifierSet &id_set)
{
	return id_set.count(identifier) != 0;
}

std::string UniqueDeclaration::get_unique_identifier(const std::string &identifier_basename, IdentifierSet &id_set, std::size_t digits)
{
	if (!identifier_declaration_exists(identifier_basename, id_set)) {
		id_set.emplace(identifier_basename);
		return identifier_basename;
	}

	std::uniform_int_distribution<int> distribution(97, 122);

	std::stringstream ss;

	do {
		ss.clear();
		ss.str("");

		ss << identifier_basename << "_";

		for (std::size_t i=0; i<digits; i++) {
			ss << static_cast<char>(distribution(s_generator));
		}
	} while (identifier_declaration_exists(ss.str(), id_set));

	id_set.emplace(ss.str());

	return ss.str();
}


}
}
}
