#include "../../helpers/helpers.hpp"

#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/passes/analysis/function.hpp>
#include <veriparse/parser/verilog.hpp>
#include <veriparse/logger/logger.hpp>

#include <gtest/gtest.h>

using namespace Veriparse;
using namespace Passes;

static TestHelpers test_helpers("lib/test/parser/testcases/");

TEST(PassesAnalysis_Function, function0) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	const auto &source = verilog.get_source();
	ASSERT_NE(source, nullptr);

	const auto &functions = Analysis::Module::get_function_nodes(source);
	ASSERT_NE(functions, nullptr);
	ASSERT_EQ(functions->size(), 1);

	ASSERT_TRUE(Analysis::Function::is_like_automatic(functions->front()));
}

TEST(PassesAnalysis_Function, function1) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	const auto &source = verilog.get_source();
	ASSERT_NE(source, nullptr);

	const auto &functions = Analysis::Module::get_function_nodes(source);
	ASSERT_NE(functions, nullptr);
	ASSERT_EQ(functions->size(), 1);

	ASSERT_TRUE(Analysis::Function::is_like_automatic(functions->front()));
}

TEST(PassesAnalysis_Function, function2) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	const auto &source = verilog.get_source();
	ASSERT_NE(source, nullptr);

	const auto &functions = Analysis::Module::get_function_nodes(source);
	ASSERT_NE(functions, nullptr);
	ASSERT_EQ(functions->size(), 1);

	ASSERT_FALSE(Analysis::Function::is_like_automatic(functions->front()));
}

TEST(PassesAnalysis_Function, function3) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	const auto &source = verilog.get_source();
	ASSERT_NE(source, nullptr);

	const auto &functions = Analysis::Module::get_function_nodes(source);
	ASSERT_NE(functions, nullptr);
	ASSERT_EQ(functions->size(), 1);

	ASSERT_TRUE(Analysis::Function::is_like_automatic(functions->front()));
}
