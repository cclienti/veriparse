#include "../../helpers/helpers.hpp"
#include <veriparse/AST/nodes.hpp>
#include <veriparse/parser/verilog.hpp>
#include <veriparse/passes/transformations/expression_evaluation.hpp>
#include <gtest/gtest.h>

using namespace Veriparse;

static TestHelpers test_helpers("lib/test/passes/transformations/testcases/");




TEST(PassesTransformation_ExpressionEvaluation, expression_evaluation0) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);
	test_helpers.render_node_to_dot_file(source, test_string + ".dot");

	AST::Node::ListPtr node_defs = AST::cast_to<AST::Source>(source)->get_description()->get_definitions();
	AST::Node::ListPtr module_stmt = AST::cast_to<AST::Module>(node_defs->front())->get_items();

	// TEST1 localparam
	AST::Node::Ptr rvalue1 = AST::cast_to<AST::Localparam>(module_stmt->front())->get_value();
	AST::Node::Ptr expr1 = AST::cast_to<AST::Rvalue>(rvalue1)->get_var();
	AST::Node::Ptr eval1 = Passes::Transformations::ExpressionEvaluation().evaluate_node(expr1);
	ASSERT_TRUE(eval1 != nullptr);
	ASSERT_EQ(7, AST::cast_to<AST::IntConstN>(eval1)->get_value());

	// X localparam
	module_stmt->pop_front();
	AST::Node::Ptr rvalue2 = AST::cast_to<AST::Localparam>(module_stmt->front())->get_value();
	AST::Node::Ptr expr2 = AST::cast_to<AST::Rvalue>(rvalue2)->get_var();
	Passes::Transformations::ExpressionEvaluation::ReplaceMap map_id;
	map_id["X"] = AST::cast_to<AST::Constant>(expr2);

	// Y localparam
	module_stmt->pop_front();
	AST::Node::Ptr rvalue3 = AST::cast_to<AST::Localparam>(module_stmt->front())->get_value();
	AST::Node::Ptr expr3 = AST::cast_to<AST::Rvalue>(rvalue3)->get_var();
	AST::Node::Ptr eval3 = Passes::Transformations::ExpressionEvaluation(map_id).evaluate_node(expr3);
	ASSERT_TRUE(eval3 != nullptr);
	ASSERT_EQ(5.0, AST::cast_to<AST::FloatConst>(eval3)->get_value());

	// Z localparam
	map_id["i"] =  std::make_shared<AST::IntConstN>(10, -1, true, 1);
	module_stmt->pop_front();
	AST::Node::Ptr rvalue4 = AST::cast_to<AST::Localparam>(module_stmt->front())->get_value();
	AST::Node::Ptr expr4 = AST::cast_to<AST::Rvalue>(rvalue4)->get_var();
	AST::Node::Ptr eval4 = Passes::Transformations::ExpressionEvaluation(map_id).evaluate_node(expr4);
	ASSERT_TRUE(eval4 != nullptr);
	ASSERT_EQ(-228, AST::cast_to<AST::IntConstN>(eval4)->get_value());

	map_id["i"] =  std::make_shared<AST::IntConstN>(10, -1, true, 0);
	AST::Node::Ptr eval4_2 = Passes::Transformations::ExpressionEvaluation(map_id).evaluate_node(expr4);
	ASSERT_TRUE(eval4_2 != nullptr);
	ASSERT_EQ(-218, AST::cast_to<AST::IntConstN>(eval4_2)->get_value());

	// A localparam
	module_stmt->pop_front();
	AST::Node::Ptr rvalueA = AST::cast_to<AST::Localparam>(module_stmt->front())->get_value();
	AST::Node::Ptr exprA = AST::cast_to<AST::Rvalue>(rvalueA)->get_var();
	AST::Node::Ptr evalA = Passes::Transformations::ExpressionEvaluation(map_id).evaluate_node(exprA);
	ASSERT_TRUE(evalA != nullptr);
	ASSERT_EQ(6, AST::cast_to<AST::IntConstN>(evalA)->get_value());

	// B localparam
	module_stmt->pop_front();
	AST::Node::Ptr rvalueB = AST::cast_to<AST::Localparam>(module_stmt->front())->get_value();
	AST::Node::Ptr exprB = AST::cast_to<AST::Rvalue>(rvalueB)->get_var();
	AST::Node::Ptr evalB = Passes::Transformations::ExpressionEvaluation(map_id).evaluate_node(exprB);
	ASSERT_TRUE(evalB != nullptr);
	ASSERT_EQ(0xFFFFFFFF, AST::cast_to<AST::IntConstN>(evalB)->get_value());

	// C localparam
	module_stmt->pop_front();
	AST::Node::Ptr rvalueC = AST::cast_to<AST::Localparam>(module_stmt->front())->get_value();
	AST::Node::Ptr exprC = AST::cast_to<AST::Rvalue>(rvalueC)->get_var();
	AST::Node::Ptr evalC = Passes::Transformations::ExpressionEvaluation(map_id).evaluate_node(exprC);
	ASSERT_TRUE(evalC != nullptr);
	ASSERT_EQ(14, AST::cast_to<AST::IntConstN>(evalC)->get_value());

	// D localparam
	module_stmt->pop_front();
	AST::Node::Ptr rvalueD = AST::cast_to<AST::Localparam>(module_stmt->front())->get_value();
	AST::Node::Ptr exprD = AST::cast_to<AST::Rvalue>(rvalueD)->get_var();
	AST::Node::Ptr evalD = Passes::Transformations::ExpressionEvaluation(map_id).evaluate_node(exprD);
	ASSERT_TRUE(evalD != nullptr);
	ASSERT_EQ(-2, AST::cast_to<AST::IntConstN>(evalD)->get_value());

	// E localparam
	module_stmt->pop_front();
	AST::Node::Ptr rvalueE = AST::cast_to<AST::Localparam>(module_stmt->front())->get_value();
	AST::Node::Ptr exprE = AST::cast_to<AST::Rvalue>(rvalueE)->get_var();
	AST::Node::Ptr evalE = Passes::Transformations::ExpressionEvaluation(map_id).evaluate_node(exprE);
	ASSERT_TRUE(evalE != nullptr);
	ASSERT_EQ(2, AST::cast_to<AST::IntConstN>(evalE)->get_value());

	// F localparam
	module_stmt->pop_front();
	map_id["MYVAR"] = std::make_shared<AST::IntConstN>(10, -1, true, 5);
	AST::Node::Ptr rvalueF = AST::cast_to<AST::Localparam>(module_stmt->front())->get_value();
	AST::Node::Ptr exprF = AST::cast_to<AST::Rvalue>(rvalueF)->get_var();
	AST::Node::Ptr evalF = Passes::Transformations::ExpressionEvaluation(map_id).evaluate_node(exprF);
	ASSERT_TRUE(evalF != nullptr);
	ASSERT_EQ(1, AST::cast_to<AST::IntConstN>(evalF)->get_value());

	// G localparam
	module_stmt->pop_front();
	map_id["MYVAR"] = std::make_shared<AST::IntConstN>(10, 4, true, 5);
	AST::Node::Ptr rvalueG = AST::cast_to<AST::Localparam>(module_stmt->front())->get_value();
	AST::Node::Ptr exprG = AST::cast_to<AST::Rvalue>(rvalueG)->get_var();
	AST::Node::Ptr evalG = Passes::Transformations::ExpressionEvaluation(map_id).evaluate_node(exprG);
	ASSERT_TRUE(evalG != nullptr);
	ASSERT_EQ(0, AST::cast_to<AST::IntConstN>(evalG)->get_value());

	// H localparam
	module_stmt->pop_front();
	map_id["MYVAR"] = std::make_shared<AST::IntConstN>(10, 4, true, 47);
	AST::Node::Ptr rvalueH = AST::cast_to<AST::Localparam>(module_stmt->front())->get_value();
	AST::Node::Ptr exprH = AST::cast_to<AST::Rvalue>(rvalueH)->get_var();
	AST::Node::Ptr evalH = Passes::Transformations::ExpressionEvaluation(map_id).evaluate_node(exprH);
	ASSERT_TRUE(evalH != nullptr);
	ASSERT_EQ(5, AST::cast_to<AST::IntConstN>(evalH)->get_value());

	// I localparam
	module_stmt->pop_front();
	map_id["MYVAR"] = std::make_shared<AST::IntConstN>(10, 32, false, 0x12345678);
	AST::Node::Ptr rvalueI = AST::cast_to<AST::Localparam>(module_stmt->front())->get_value();
	AST::Node::Ptr exprI = AST::cast_to<AST::Rvalue>(rvalueI)->get_var();
	AST::Node::Ptr evalI = Passes::Transformations::ExpressionEvaluation(map_id).evaluate_node(exprI);
	ASSERT_TRUE(evalI != nullptr);
	ASSERT_EQ(0x34, AST::cast_to<AST::IntConstN>(evalI)->get_value());

	// J localparam
	module_stmt->pop_front();
	map_id["MYVAR"] = std::make_shared<AST::IntConstN>(10, 32, false, 0x12345678);
	AST::Node::Ptr rvalueJ = AST::cast_to<AST::Localparam>(module_stmt->front())->get_value();
	AST::Node::Ptr exprJ = AST::cast_to<AST::Rvalue>(rvalueJ)->get_var();
	AST::Node::Ptr evalJ = Passes::Transformations::ExpressionEvaluation(map_id).evaluate_node(exprJ);
	ASSERT_TRUE(evalJ != nullptr);
	ASSERT_EQ(0x12, AST::cast_to<AST::IntConstN>(evalJ)->get_value());

	// K localparam
	module_stmt->pop_front();
	AST::Node::Ptr rvalueK = AST::cast_to<AST::Localparam>(module_stmt->front())->get_value();
	AST::Node::Ptr exprK = AST::cast_to<AST::Rvalue>(rvalueK)->get_var();
	AST::Node::Ptr evalK = Passes::Transformations::ExpressionEvaluation(map_id).evaluate_node(exprK);
	ASSERT_TRUE(evalK != nullptr);
	ASSERT_EQ(-1, AST::cast_to<AST::IntConstN>(evalK)->get_value());
}
