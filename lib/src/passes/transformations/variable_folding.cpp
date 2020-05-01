#include <veriparse/passes/transformations/variable_folding.hpp>
#include <veriparse/passes/transformations/ast_replace.hpp>
#include <veriparse/passes/transformations/expression_evaluation.hpp>
#include <veriparse/passes/transformations/constant_folding.hpp>
#include <veriparse/generators/verilog_generator.hpp>
#include <veriparse/logger/logger.hpp>
#include <veriparse/AST/nodes.hpp>


namespace Veriparse {
namespace Passes {
namespace Transformations {


AST::Node::Ptr VariableFolding::get_state(const std::string &var_name)
{
	const auto &it = m_state_map.find(var_name);
	if (it != m_state_map.end()) {
		return it->second->clone();
	}

	return nullptr;
}

int VariableFolding::process(AST::Node::Ptr node, AST::Node::Ptr parent)
{
	if(node && node->is_node_type(AST::NodeType::Initial)) {
		m_state_map.clear();

		auto initial = AST::cast_to<AST::Initial>(node);
		auto ret = execute(initial->get_statement(), node);
		if (ret) {
			return ret;
		}

		// Remove empty initial
		const auto &stmt = initial->get_statement();
		if (stmt) {
			if (stmt->get_node_type() == AST::NodeType::Block) {
				const auto &block = AST::cast_to<AST::Block>(stmt);
				if (block->get_statements() == nullptr) {
					parent->replace(node, AST::Node::Ptr(nullptr));
				}
			}
		}
		else {
			if (parent) {
				parent->replace(node, AST::Node::Ptr(nullptr));
			}
		}

		return 0;
	}

	return recurse_in_childs(node);
}

int VariableFolding::execute(AST::Node::Ptr node, AST::Node::Ptr parent)
{
	if(node) {
		switch (node->get_node_type()) {
		case AST::NodeType::Block:
		case AST::NodeType::SingleStatement:
			return execute_in_childs(node);

		case AST::NodeType::Real:
		case AST::NodeType::Reg:
		case AST::NodeType::Integer:
			return execute_variable_decl(AST::cast_to<AST::Variable>(node), parent);

		case AST::NodeType::BlockingSubstitution:
			return execute_blocking_substitution(AST::cast_to<AST::BlockingSubstitution>(node), parent);

		case AST::NodeType::IfStatement:
			return execute_if(AST::cast_to<AST::IfStatement>(node), parent);

		case AST::NodeType::ForStatement:
			return execute_for(AST::cast_to<AST::ForStatement>(node), parent);

		case AST::NodeType::WhileStatement:
			return execute_while(AST::cast_to<AST::WhileStatement>(node), parent);

		case AST::NodeType::RepeatStatement:
			return execute_repeat(AST::cast_to<AST::RepeatStatement>(node), parent);

		case AST::NodeType::FunctionCall:
		case AST::NodeType::TaskCall:
		case AST::NodeType::SystemCall:
			return execute_call(node, parent);
		default:
			break;
		}
	}
	return 0;
}

int VariableFolding::execute_in_childs(AST::Node::Ptr node)
{
	int ret = 0;
	AST::Node::ListPtr children = node->get_children();
	for (AST::Node::Ptr child: *children) {
		ret += execute(child, node);
	}
	return ret;
}

int VariableFolding::execute_variable_decl(AST::Variable::Ptr var, AST::Node::Ptr parent)
{
	if (!var->get_right()) {
		return 0;
	}

	const auto &id = std::make_shared<AST::Identifier>(var->get_filename(), var->get_line());
	id->set_name(var->get_name());

	const auto &lvalue = std::make_shared<AST::Lvalue>(var->get_filename(), var->get_line());
	lvalue->set_var(id);

	const auto &subst = std::make_shared<AST::BlockingSubstitution>(var->get_filename(), var->get_line());
	subst->set_left(lvalue);
	subst->set_right(AST::cast_to<AST::Rvalue>(var->get_right()->clone()));

	return execute_blocking_substitution(subst, parent);
}

int VariableFolding::execute_blocking_substitution(AST::BlockingSubstitution::Ptr subst,
                                                   AST::Node::Ptr parent)
{
	std::string lvalue_before = Generators::VerilogGenerator().render(subst->get_left());
	std::string rvalue_before = Generators::VerilogGenerator().render(subst->get_right());

	std::string lvalue_str = analyze_lvalue(subst->get_left());
	AST::Node::Ptr const_node = analyze_rvalue(subst->get_right());

	if(lvalue_str.empty() || const_node == nullptr)
		return 0;

	LOG_DEBUG_N(subst) << "[" << lvalue_before << " = " << rvalue_before
	                   << "] evaluated to ["
	                   << lvalue_str << " = " << Generators::VerilogGenerator().render(const_node)
	                   << "]";

	m_state_map[lvalue_str] = const_node;
	subst->get_right()->set_var(const_node);
	return 0;
}

int VariableFolding::execute_if(AST::IfStatement::Ptr ifstmt, AST::Node::Ptr parent)
{
	int ret = 0;

	auto expr = analyze_expression(ifstmt->get_cond());

	if(expr && expr->is_node_type(AST::NodeType::IntConstN)) {
		ifstmt->set_cond(expr);
		auto cond = AST::cast_to<AST::IntConstN>(expr);
		AST::Node::Ptr selected_stmt;

		if(cond->get_value() != 0) {
			selected_stmt = ifstmt->get_true_statement();
		}
		else {
			selected_stmt = ifstmt->get_false_statement();
		}

		ret = execute(selected_stmt, ifstmt);
		pickup_statements(parent, ifstmt, selected_stmt);
	}

	return ret;
}

int VariableFolding::execute_for(AST::ForStatement::Ptr node, AST::Node::Ptr parent)
{
	int ret = 0;

	if(!node) return ret;

	// We create a temporary block to hold a temporary
	// unrolled result.
	//
	// We must pay attention that the list within the block
	// can be replaced! A new list can be created when the
	// "replace" or the "pickup_statements" methods are
	// called.
	auto block = std::make_shared<AST::Block>(std::make_shared<AST::Node::List>(), "");

	// Pre condition.
	auto pre = node->get_pre();
	if(pre) {
		ret += execute(pre, node);
	}

	while(1) {
		auto cloned = AST::cast_to<AST::ForStatement>(node->clone());

		// Statements within for.
		auto block_stmts = block->get_statements();
		auto current = cloned->get_statement();

		if(current->is_node_type(AST::NodeType::Block)) {
			auto current_block = AST::cast_to<AST::Block>(current);
			for(auto stmt: *current_block->get_statements()) {
				block_stmts->push_back(stmt);
				ret += execute(stmt, block);
			}
		}
		else {
			block_stmts->push_back(current);
			ret += execute(current, block);
		}

		// Post update.
		auto post = cloned->get_post();
		if(post) {
			ret += execute(post, cloned);
		}

		// Analyze condition.
		auto cond = analyze_expression(cloned->get_cond());
		if(cond && cond->is_node_type(AST::NodeType::IntConstN)) {
			if(AST::cast_to<AST::IntConstN>(cond)->get_value() == 0)
				break;
		}
		else {
			LOG_WARNING_N(node) << "condition cannot be evaluated during for loop unrolling";
			return 0;
		}
	}

	// At this point everything is unrolled and we can
	// replace the block in the parent.
	pickup_statements(parent, node, block);

	return ret;
}

int VariableFolding::execute_while(AST::WhileStatement::Ptr node, AST::Node::Ptr parent)
{
	int ret = 0;

	if(!node) return ret;

	auto expr = analyze_expression(node->get_cond()->clone());

	if(expr && expr->is_node_type(AST::NodeType::IntConstN)) {
		auto cond = AST::cast_to<AST::IntConstN>(expr);

		// We create a temporary block to hold a temporary
		// unrolled result.
		//
		// We must pay attention that the list within the block
		// can be replaced! A new list can be created when the
		// "replace" or the "pickup_statements" methods are
		// called.
		auto block = std::make_shared<AST::Block>(std::make_shared<AST::Node::List>(), "");

		while(cond->get_value() != 0) {
			// We must get the new list updated by pickup_parents.
			auto block_stmts = block->get_statements();

			// We clone the statement to analyze
			auto current = node->get_statement()->clone();

			if(current->is_node_type(AST::NodeType::Block)) {
				auto current_block = AST::cast_to<AST::Block>(current);
				for(auto stmt: *current_block->get_statements()) {
					block_stmts->push_back(stmt);
					ret += execute(stmt, block);
				}
			}
			else {
				block_stmts->push_back(current);
				ret += execute(current, block);
			}

			// We clone the condition and we evaluate it.
			expr = analyze_expression(node->get_cond()->clone());
			if(expr && expr->is_node_type(AST::NodeType::IntConstN)) {
				cond = AST::cast_to<AST::IntConstN>(expr);
			}
			else {
				LOG_WARNING_N(node) << "condition cannot be evaluated during while loop unrolling";
				return 0;
			}
		}

		// At this point everything is unrolled and we can
		// replace the block in the parent.
		pickup_statements(parent, node, block);

	}

	return ret;
}

int VariableFolding::execute_repeat(AST::RepeatStatement::Ptr node, AST::Node::Ptr parent)
{
	int ret = 0;

	auto expr = analyze_expression(node->get_times());

	if(expr && expr->is_node_type(AST::NodeType::IntConstN)) {
		// Create a temporary block to hold unrolled statements.
		//
		// We must pay attention that the list within the block
		// can be replaced! A new list can be created when the
		// "replace" or the "pickup_statements" methods are
		// called.
		auto block = std::make_shared<AST::Block>(std::make_shared<AST::Node::List>(), "");

		auto times = AST::cast_to<AST::IntConstN>(expr);
		for(mpz_class i=0; i<times->get_value(); i++) {
			auto block_stmts = block->get_statements();
			auto current = node->get_statement()->clone();

			if(current->is_node_type(AST::NodeType::Block)) {
				auto current_block = AST::cast_to<AST::Block>(current);
				for(auto stmt: *current_block->get_statements()) {
					block_stmts->push_back(stmt);
					ret += execute(stmt, block);
				}
			}
			else {
				block_stmts->push_back(current);
				ret += execute(current, block);
			}
		}

		// At this point everything is unrolled and we can
		// replace the block in the parent.
		pickup_statements(parent, node, block);
	}

	return ret;
}

int VariableFolding::execute_call(AST::Node::Ptr node, AST::Node::Ptr parent)
{
	AST::Node::ListPtr args;

	switch(node->get_node_type()) {
	case AST::NodeType::FunctionCall:
		args = AST::cast_to<AST::FunctionCall>(node)->get_args();
		break;
	case AST::NodeType::TaskCall:
		args = AST::cast_to<AST::TaskCall>(node)->get_args();
		break;
	case AST::NodeType::SystemCall:
		args = AST::cast_to<AST::SystemCall>(node)->get_args();
		break;
	default:
		LOG_ERROR_N(node) << "VariableFolding: unknown node type";
		return 1;
	}

	if(args) {
		for(auto arg: *args) {
			ASTReplace::replace_identifier(arg, m_state_map, node);
			auto expr = ExpressionEvaluation().evaluate_node(arg);
			if(expr) {
				node->replace(arg, expr);
			}
		}
	}

	return 0;
}

AST::Node::Ptr VariableFolding::analyze_rvalue(AST::Rvalue::Ptr rvalue)
{
	ASTReplace::replace_identifier(AST::to_node(rvalue), m_state_map);
	AST::Node::Ptr expr = ExpressionEvaluation().evaluate_node(rvalue->get_var());
	return expr;
}

std::string VariableFolding::analyze_lvalue(AST::Lvalue::Ptr lvalue)
{
	const auto &var = lvalue->get_var();

	switch(var->get_node_type()) {
	case AST::NodeType::Pointer:
		ASTReplace::replace_identifier(AST::cast_to<AST::Pointer>(var)->get_ptr(), m_state_map, var);
		break;

	case AST::NodeType::Partselect:
		{
			auto part = AST::cast_to<AST::Partselect>(var);
			ASTReplace::replace_identifier(part->get_msb(), m_state_map, var);
			ASTReplace::replace_identifier(part->get_lsb(), m_state_map, var);
		}
		break;

	case AST::NodeType::PartselectIndexed:
	case AST::NodeType::PartselectPlusIndexed:
	case AST::NodeType::PartselectMinusIndexed:
		{
			auto part = AST::cast_to<AST::PartselectIndexed>(var);
			ASTReplace::replace_identifier(part->get_index(), m_state_map, var);
			ASTReplace::replace_identifier(part->get_size(), m_state_map, var);
		}
		break;

	default:
		break;
	}

	ConstantFolding().run(AST::to_node(lvalue));
	return Generators::VerilogGenerator().render(lvalue->get_var());
}

AST::Node::Ptr VariableFolding::analyze_expression(AST::Node::Ptr expr)
{
	auto rvalue = std::make_shared<AST::Rvalue>(expr);
	return analyze_rvalue(rvalue);
}

}
}
}
