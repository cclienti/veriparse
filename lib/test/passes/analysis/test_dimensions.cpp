#include "../../helpers/helpers.hpp"
#include <veriparse/parser/verilog.hpp>
#include <veriparse/passes/analysis/dimensions.hpp>
#include <veriparse/passes/analysis/module.hpp>
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
	int ret = Dimensions::analyze_decls(source, dim_map);
	ASSERT_EQ(0, ret);


	//--------------------------------------------
	Dimensions::DimList clock_ref_dims {
		Dimensions::DimList::Decl::both, // decl
	};

	ASSERT_EQ(1u, dim_map.count("clock"));
	LOG_INFO << "clock: ref: " << clock_ref_dims.list;
	LOG_INFO << "clock: got: " << dim_map["clock"].list;
	ASSERT_EQ(clock_ref_dims, dim_map["clock"]);


	//--------------------------------------------
	Dimensions::DimList valid_ref_dims {
		Dimensions::DimList::Decl::both, // decl
	};

	ASSERT_EQ(1u, dim_map.count("valid"));
	LOG_INFO << "valid: ref: " << valid_ref_dims.list;
	LOG_INFO << "valid: got: " << dim_map["valid"].list;
	ASSERT_EQ(valid_ref_dims, dim_map["valid"]);


	//--------------------------------------------
	Dimensions::DimList a_ref_dims {
		Dimensions::DimList::Decl::both, // decl
		// list
		{  // msb, lsb, width, is_big, is_packed
			{3, 0, 4, true, true}, // dim 1
			{7, 0, 8, true, true}, // dim 0
		}
	};

	ASSERT_EQ(1u, dim_map.count("a"));
	LOG_INFO << "a: ref: " << a_ref_dims.list;
	LOG_INFO << "a: got: " << dim_map["a"].list;
	ASSERT_EQ(a_ref_dims, dim_map["a"]);


	//--------------------------------------------

	Dimensions::DimList b_ref_dims {
		Dimensions::DimList::Decl::var, // decl
		// list
		{  // msb, lsb, width, is_big, is_packed
			{0, 1, 2, false, false}, // dim 3
			{2, 1, 2, true,  false}, // dim 2
			{3, 0, 4, true,  true},  // dim 1
			{4, 1, 4, true,  true},  // dim 0
		}
	};

	ASSERT_EQ(1u, dim_map.count("b"));
	LOG_INFO << "b: ref: " << b_ref_dims.list;
	LOG_INFO << "b: got: " << dim_map["b"].list;
	ASSERT_EQ(b_ref_dims, dim_map["b"]);


	//--------------------------------------------

	Dimensions::DimList c_ref_dims {
		Dimensions::DimList::Decl::var, // decl
		// list
		{  // msb, lsb, width, is_big, is_packed
			{31, 0, 32, true,  true},  // dim 0
		}
	};

	ASSERT_EQ(1u, dim_map.count("c"));
	LOG_INFO << "c: ref: " << c_ref_dims.list;
	LOG_INFO << "c: got: " << dim_map["c"].list;
	ASSERT_EQ(c_ref_dims, dim_map["c"]);


	//--------------------------------------------

	Dimensions::DimList d_ref_dims {
		Dimensions::DimList::Decl::var, // decl
		// list
		{  // msb, lsb, width, is_big, is_packed
			{31, 0, 32, true,  true},  // dim 0
			{ 7, 0,  8, true, false},  // dim 1
		}
	};

	ASSERT_EQ(1u, dim_map.count("d"));
	LOG_INFO << "d: ref: " << d_ref_dims.list;
	LOG_INFO << "d: got: " << dim_map["d"].list;
	ASSERT_EQ(d_ref_dims, dim_map["d"]);

}



TEST(PassesAnalysis_Dimensions, dimension1) {
	ENABLE_LOGGER;

	using namespace Passes::Analysis;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	const auto &source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);
	test_helpers.render_node_to_dot_file(source, test_string + ".dot");

	int ret;

	Dimensions::DimMap dim_map;
	ret = Dimensions::analyze_decls(source, dim_map);
	ASSERT_EQ(0, ret);
	LOG_INFO_N(source) << dim_map;

	auto rvalue_nodes = Module::get_rvalue_nodes(source);
	ASSERT_NE(0u, rvalue_nodes->size());

	//----------------------------------------------------
	const auto rvalue_concat = rvalue_nodes->front();
	rvalue_nodes->pop_front();
	{
		Dimensions::DimList dims;
		ret = Dimensions::analyze_expr(rvalue_concat->get_var(), dim_map, dims);
		ASSERT_EQ(0, ret);

		LOG_INFO_N(rvalue_concat) << dims.list;

		Dimensions::DimList ref_dims {
			Dimensions::DimList::Decl::var, // decl
			// list
			{  // msb, lsb, width, is_big, is_packed
				{35, 0, 36, true, true}, // dim 0
			}
		};
		ASSERT_EQ(ref_dims, dims);
	}

	//----------------------------------------------------
	for (int i=0; i<4; i++) {
		const auto rvalue_ptr = rvalue_nodes->front();
		rvalue_nodes->pop_front();
		{
			Dimensions::DimList dims;
			ret = Dimensions::analyze_expr(rvalue_ptr->get_var(), dim_map, dims);
			ASSERT_EQ(0, ret);

			LOG_INFO_N(rvalue_ptr) << dims.list;

			Dimensions::DimList ref_dims {
				Dimensions::DimList::Decl::both, // decl
				// list
				{  // msb, lsb, width, is_big, is_packed
					{ 7, 0,  8, true, true}, // dim 0
				}
			};
			ASSERT_EQ(ref_dims, dims);
		}
	}

	//----------------------------------------------------
	const auto rvalue_repacked = rvalue_nodes->front();
	rvalue_nodes->pop_front();
	{
		Dimensions::DimList dims;
		ret = Dimensions::analyze_expr(rvalue_repacked->get_var(), dim_map, dims);
		ASSERT_EQ(0, ret);

		LOG_INFO_N(rvalue_repacked) << dims.list;

		Dimensions::DimList ref_dims {
			Dimensions::DimList::Decl::var, // decl
			// list
			{  // msb, lsb, width, is_big, is_packed
				{31, 0, 32, true, true}, // dim 0
			}
		};
		ASSERT_EQ(ref_dims, dims);
	}

	//----------------------------------------------------
	const auto rvalue_undeclared = rvalue_nodes->front();
	rvalue_nodes->pop_front();
	{
		Dimensions::DimList dims;
		ret = Dimensions::analyze_expr(rvalue_undeclared->get_var(), dim_map, dims);
		ASSERT_EQ(1, ret);
	}


	//----------------------------------------------------
	const auto rvalue_repeated_a = rvalue_nodes->front();
	rvalue_nodes->pop_front();
	{
		Dimensions::DimList dims;
		ret = Dimensions::analyze_expr(rvalue_repeated_a->get_var(), dim_map, dims);
		ASSERT_EQ(0, ret);

		LOG_INFO_N(rvalue_repeated_a) << dims.list;

		Dimensions::DimList ref_dims {
			Dimensions::DimList::Decl::var, // decl
			// list
			{  // msb, lsb, width, is_big, is_packed
				{63, 0, 64, true, true}, // dim 0
			}
		};
		ASSERT_EQ(ref_dims, dims);
	}

	//----------------------------------------------------
	const auto rvalue_repeated_b = rvalue_nodes->front();
	rvalue_nodes->pop_front();
	{
		Dimensions::DimList dims;
		ret = Dimensions::analyze_expr(rvalue_repeated_b->get_var(), dim_map, dims);
		ASSERT_EQ(0, ret);

		LOG_INFO_N(rvalue_repeated_b) << dims.list;

		Dimensions::DimList ref_dims {
			Dimensions::DimList::Decl::var, // decl
			// list
			{  // msb, lsb, width, is_big, is_packed
				{31, 0, 32, true, true}, // dim 0
			}
		};
		ASSERT_EQ(ref_dims, dims);
	}

}
