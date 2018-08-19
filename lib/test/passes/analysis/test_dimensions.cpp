#include "../../helpers/helpers.hpp"
#include <veriparse/parser/verilog.hpp>
#include <veriparse/passes/analysis/dimensions.hpp>
#include <veriparse/logger/logger.hpp>
#include <gtest/gtest.h>

using namespace Veriparse;

static TestHelpers test_helpers("../../test/passes/analysis/testcases/");


TEST(PassesAnalysis_Dimensions, dimension0) {
	ENABLE_LOGGER;

	using namespace Passes::Analysis;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);
	test_helpers.render_node_to_dot_file(source, test_string + ".dot");

	Dimensions::DimMap dim_map;
	int ret = Dimensions::analyze(source, dim_map);
	ASSERT_EQ(0, ret);


	Dimensions::DimList a_ref_dims {
		Dimensions::DimList::Decl::io, // decl
		// list
		{
			{7, 0, 8, true, true}, // dim 0
			{3, 0, 4, true, true}, // dim 1
		}
	};

	ASSERT_EQ(1u, dim_map.count("a"));
	LOG_INFO << "ref: " << a_ref_dims.list;
	LOG_INFO << "got: " << dim_map["a"].list;
	ASSERT_EQ(a_ref_dims, dim_map["a"]);


	Dimensions::DimList b_ref_dims {
		Dimensions::DimList::Decl::io, // decl
		// list
		{
			{4, 1, 4, true,  true},  // dim 0
			{3, 0, 4, true,  true},  // dim 1
			{2, 1, 2, true,  false}, // dim 2
			{0, 1, 2, false, false}, // dim 3
		}
	};

	ASSERT_EQ(1u, dim_map.count("b"));
	LOG_INFO << "ref: " << b_ref_dims.list;
	LOG_INFO << "got: " << dim_map["b"].list;
	ASSERT_EQ(b_ref_dims, dim_map["b"]);

}
