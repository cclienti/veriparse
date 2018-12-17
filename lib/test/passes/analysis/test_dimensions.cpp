#include "../../helpers/helpers.hpp"
#include <veriparse/parser/verilog.hpp>
#include <veriparse/passes/analysis/dimensions.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/passes/transformations/ast_replace.hpp>
#include <veriparse/logger/logger.hpp>
#include <gtest/gtest.h>

using namespace Veriparse;

static TestHelpers test_helpers("lib/test/passes/analysis/testcases/");


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
	LOG_DEBUG << "clock: ref: " << clock_ref_dims.list;
	LOG_DEBUG << "clock: got: " << dim_map["clock"].list;
	ASSERT_EQ(clock_ref_dims, dim_map["clock"]);


	//--------------------------------------------
	Dimensions::DimList valid_ref_dims {
		Dimensions::DimList::Decl::both, // decl
	};

	ASSERT_EQ(1u, dim_map.count("valid"));
	LOG_DEBUG << "valid: ref: " << valid_ref_dims.list;
	LOG_DEBUG << "valid: got: " << dim_map["valid"].list;
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
	LOG_DEBUG << "a: ref: " << a_ref_dims.list;
	LOG_DEBUG << "a: got: " << dim_map["a"].list;
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
	LOG_DEBUG << "b: ref: " << b_ref_dims.list;
	LOG_DEBUG << "b: got: " << dim_map["b"].list;
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
	LOG_DEBUG << "c: ref: " << c_ref_dims.list;
	LOG_DEBUG << "c: got: " << dim_map["c"].list;
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
	LOG_DEBUG << "d: ref: " << d_ref_dims.list;
	LOG_DEBUG << "d: got: " << dim_map["d"].list;
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
	LOG_DEBUG_N(source) << dim_map;

	auto rvalue_nodes = Module::get_rvalue_nodes(source);
	ASSERT_NE(0u, rvalue_nodes->size());

	//----------------------------------------------------
	const auto rvalue_concat = rvalue_nodes->front();
	rvalue_nodes->pop_front();
	{
		Dimensions::DimList dims {Dimensions::DimList::Decl::var, {}};
		ret = Dimensions::analyze_expr(rvalue_concat->get_var(), dim_map, dims);
		ASSERT_EQ(0, ret);

		LOG_DEBUG_N(rvalue_concat) << "concat:" << dims;

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
			Dimensions::DimList dims {Dimensions::DimList::Decl::both, {}};
			ret = Dimensions::analyze_expr(rvalue_ptr->get_var(), dim_map, dims);
			ASSERT_EQ(0, ret);

			LOG_DEBUG_N(rvalue_ptr) << dims;

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
		Dimensions::DimList dims {Dimensions::DimList::Decl::var, {}};
		ret = Dimensions::analyze_expr(rvalue_repacked->get_var(), dim_map, dims);
		ASSERT_EQ(0, ret);

		LOG_DEBUG_N(rvalue_repacked) << "repacked: " << dims;

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
		Dimensions::DimList dims {Dimensions::DimList::Decl::var, {}};
		ret = Dimensions::analyze_expr(rvalue_repeated_a->get_var(), dim_map, dims);
		ASSERT_EQ(0, ret);

		LOG_DEBUG_N(rvalue_repeated_a) << "repeated_a" << dims;

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
		Dimensions::DimList dims {Dimensions::DimList::Decl::var, {}};
		ret = Dimensions::analyze_expr(rvalue_repeated_b->get_var(), dim_map, dims);
		ASSERT_EQ(0, ret);

		LOG_DEBUG_N(rvalue_repeated_b) << "repeated_b: " << dims;

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
	const auto rvalue_slice_a = rvalue_nodes->front();
	rvalue_nodes->pop_front();
	{
		Dimensions::DimList dims;
		ret = Dimensions::analyze_expr(rvalue_slice_a->get_var(), dim_map, dims);
		ASSERT_EQ(0, ret);

		LOG_DEBUG_N(rvalue_slice_a) << "slice_a: " << dims;

		Dimensions::DimList ref_dims {
			Dimensions::DimList::Decl::both, // decl
			// list
			{  // msb, lsb, width, is_big, is_packed
				{ 3,  2,  2, true, true}, // dim 1
				{ 7,  0,  8, true, true}, // dim 0
			}
		};
		ASSERT_EQ(ref_dims, dims);
	}

	//----------------------------------------------------
	const auto rvalue_slice_a_p = rvalue_nodes->front();
	rvalue_nodes->pop_front();
	{
		Dimensions::DimList dims {Dimensions::DimList::Decl::both, {}};
		ret = Dimensions::analyze_expr(rvalue_slice_a_p->get_var(), dim_map, dims);
		ASSERT_EQ(0, ret);

		LOG_DEBUG_N(rvalue_slice_a_p) << "slice_a_p: " << dims;

		Dimensions::DimList ref_dims {
			Dimensions::DimList::Decl::both, // decl
			// list
			{  // msb, lsb, width, is_big, is_packed
				{ 2,  1,  2, true, true}, // dim 1
				{ 7,  0,  8, true, true}, // dim 0
			}
		};
		ASSERT_EQ(ref_dims, dims);
	}

	//----------------------------------------------------
	const auto rvalue_slice_a_m = rvalue_nodes->front();
	rvalue_nodes->pop_front();
	{
		Dimensions::DimList dims {Dimensions::DimList::Decl::both, {}};
		ret = Dimensions::analyze_expr(rvalue_slice_a_m->get_var(), dim_map, dims);
		ASSERT_EQ(0, ret);

		LOG_DEBUG_N(rvalue_slice_a_m) << "slice_a_m: " << dims;

		Dimensions::DimList ref_dims {
			Dimensions::DimList::Decl::both, // decl
			// list
			{  // msb, lsb, width, is_big, is_packed
				{ 1,  0,  2, true, true}, // dim 1
				{ 7,  0,  8, true, true}, // dim 0
			}
		};
		ASSERT_EQ(ref_dims, dims);
	}

	//----------------------------------------------------
	rvalue_nodes->pop_front();
	const auto rvalue_int_ptr = rvalue_nodes->front();
	const auto replacement = std::make_shared<AST::IntConstN>(10, -1, true, 5);
	Passes::Transformations::ASTReplace::replace_identifier(rvalue_int_ptr->get_var(), "to_replace",
	                                                        replacement, rvalue_int_ptr);
	rvalue_nodes->pop_front();
	{
		Dimensions::DimList dims {Dimensions::DimList::Decl::both, {}};
		ret = Dimensions::analyze_expr(rvalue_int_ptr->get_var(), dim_map, dims);
		ASSERT_EQ(0, ret);

		Dimensions::DimList ref_dims {
			Dimensions::DimList::Decl::both, // decl
			// list
			{
			}
		};
		ASSERT_EQ(ref_dims, dims);
	}

}


TEST(PassesAnalysis_Dimensions, dimension2) {
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
	LOG_DEBUG_N(source) << dim_map;

	auto rvalue_nodes = Module::get_rvalue_nodes(source);
	ASSERT_NE(0u, rvalue_nodes->size());

	//----------------------------------------------------
	const auto rvalue_concat = rvalue_nodes->front();
	rvalue_nodes->pop_front();
	{
		Dimensions::DimList dims {Dimensions::DimList::Decl::var, {}};
		ret = Dimensions::analyze_expr(rvalue_concat->get_var(), dim_map, dims);
		ASSERT_EQ(0, ret);

		LOG_DEBUG_N(rvalue_concat) << "concat: " << dims;

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
			Dimensions::DimList dims {Dimensions::DimList::Decl::both, {}};
			ret = Dimensions::analyze_expr(rvalue_ptr->get_var(), dim_map, dims);
			ASSERT_EQ(0, ret);

			LOG_DEBUG_N(rvalue_ptr) << dims;

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
	const auto rvalue_repeated_a = rvalue_nodes->front();
	rvalue_nodes->pop_front();
	{
		Dimensions::DimList dims {Dimensions::DimList::Decl::var, {}};
		ret = Dimensions::analyze_expr(rvalue_repeated_a->get_var(), dim_map, dims);
		ASSERT_EQ(0, ret);

		LOG_DEBUG_N(rvalue_repeated_a) << "repeated_a: " << dims;

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
		Dimensions::DimList dims {Dimensions::DimList::Decl::var, {}};
		ret = Dimensions::analyze_expr(rvalue_repeated_b->get_var(), dim_map, dims);
		ASSERT_EQ(0, ret);

		LOG_DEBUG_N(rvalue_repeated_b) << "repeated_b: " << dims;

		Dimensions::DimList ref_dims {
			Dimensions::DimList::Decl::var, // decl
			// list
			{  // msb, lsb, width, is_big, is_packed
				{31,  0, 32, true, true}, // dim 0
			}
		};
		ASSERT_EQ(ref_dims, dims);
	}

	//----------------------------------------------------
	const auto rvalue_slice_a = rvalue_nodes->front();
	rvalue_nodes->pop_front();
	{
		Dimensions::DimList dims {Dimensions::DimList::Decl::both, {}};
		ret = Dimensions::analyze_expr(rvalue_slice_a->get_var(), dim_map, dims);
		ASSERT_EQ(0, ret);

		LOG_DEBUG_N(rvalue_slice_a) << "slice_a: " << dims;

		Dimensions::DimList ref_dims {
			Dimensions::DimList::Decl::both, // decl
			// list
			{  // msb, lsb, width, is_big, is_packed
				{ 2,  3,  2, false, true}, // dim 1
				{ 7,  0,  8, true, true}, // dim 0
			}
		};
		ASSERT_EQ(ref_dims, dims);
	}

	//----------------------------------------------------
	const auto rvalue_slice_a_p = rvalue_nodes->front();
	rvalue_nodes->pop_front();
	{
		Dimensions::DimList dims {Dimensions::DimList::Decl::both, {}};
		ret = Dimensions::analyze_expr(rvalue_slice_a_p->get_var(), dim_map, dims);
		ASSERT_EQ(0, ret);

		LOG_DEBUG_N(rvalue_slice_a_p) << "slice_a_p: " << dims;

		Dimensions::DimList ref_dims {
			Dimensions::DimList::Decl::both, // decl
			// list
			{  // msb, lsb, width, is_big, is_packed
				{ 1,  2,  2, false, true}, // dim 1
				{ 7,  0,  8, true, true}, // dim 0
			}
		};
		ASSERT_EQ(ref_dims, dims);
	}

	//----------------------------------------------------
	const auto rvalue_slice_a_m = rvalue_nodes->front();
	rvalue_nodes->pop_front();
	{
		Dimensions::DimList dims {Dimensions::DimList::Decl::both, {}};
		ret = Dimensions::analyze_expr(rvalue_slice_a_m->get_var(), dim_map, dims);
		ASSERT_EQ(0, ret);

		LOG_DEBUG_N(rvalue_slice_a_m) << "slice_a_m: " << dims;

		Dimensions::DimList ref_dims {
			Dimensions::DimList::Decl::both, // decl
			// list
			{  // msb, lsb, width, is_big, is_packed
				{ 0,  1,  2, false, true}, // dim 1
				{ 7,  0,  8, true, true}, // dim 0
			}
		};
		ASSERT_EQ(ref_dims, dims);
	}

}


TEST(PassesAnalysis_Dimensions, dimension3) {
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
	LOG_DEBUG_N(source) << dim_map;

	auto rvalue_nodes = Module::get_rvalue_nodes(source);
	ASSERT_NE(0u, rvalue_nodes->size());

	//----------------------------------------------------
	const auto rvalue_concat = rvalue_nodes->front();
	rvalue_nodes->pop_front();
	{
		Dimensions::DimList dims {Dimensions::DimList::Decl::var, {}};
		ret = Dimensions::analyze_expr(rvalue_concat->get_var(), dim_map, dims);
		ASSERT_EQ(0, ret);

		LOG_DEBUG_N(rvalue_concat) << "concat: " << dims;

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
			Dimensions::DimList dims {Dimensions::DimList::Decl::both, {}};
			ret = Dimensions::analyze_expr(rvalue_ptr->get_var(), dim_map, dims);
			ASSERT_EQ(0, ret);

			LOG_DEBUG_N(rvalue_ptr) << dims;

			Dimensions::DimList ref_dims {
				Dimensions::DimList::Decl::both, // decl
				// list
				{  // msb, lsb, width, is_big, is_packed
					{ 0, 7,  8, false, true}, // dim 0
				}
			};
			ASSERT_EQ(ref_dims, dims);
		}
	}

	//----------------------------------------------------
	const auto rvalue_repeated_a = rvalue_nodes->front();
	rvalue_nodes->pop_front();
	{
		Dimensions::DimList dims {Dimensions::DimList::Decl::var, {}};
		ret = Dimensions::analyze_expr(rvalue_repeated_a->get_var(), dim_map, dims);
		ASSERT_EQ(0, ret);

		LOG_DEBUG_N(rvalue_repeated_a) << "repeated_a: " << dims;

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
		Dimensions::DimList dims {Dimensions::DimList::Decl::var, {}};
		ret = Dimensions::analyze_expr(rvalue_repeated_b->get_var(), dim_map, dims);
		ASSERT_EQ(0, ret);

		LOG_DEBUG_N(rvalue_repeated_b) << "repeated_b: " << dims;

		Dimensions::DimList ref_dims {
			Dimensions::DimList::Decl::var, // decl
			// list
			{  // msb, lsb, width, is_big, is_packed
				{31,  0, 32, true, true}, // dim 0
			}
		};
		ASSERT_EQ(ref_dims, dims);
	}

	//----------------------------------------------------
	const auto rvalue_slice_a = rvalue_nodes->front();
	rvalue_nodes->pop_front();
	{
		Dimensions::DimList dims {Dimensions::DimList::Decl::both, {}};
		ret = Dimensions::analyze_expr(rvalue_slice_a->get_var(), dim_map, dims);
		ASSERT_EQ(0, ret);

		LOG_DEBUG_N(rvalue_slice_a) << "slice_a: " << dims;

		Dimensions::DimList ref_dims {
			Dimensions::DimList::Decl::both, // decl
			// list
			{  // msb, lsb, width, is_big, is_packed
				{ 2,  3,  2, false, true}, // dim 1
				{ 0,  7,  8, false, true}, // dim 0
			}
		};
		ASSERT_EQ(ref_dims, dims);
	}

	//----------------------------------------------------
	const auto rvalue_slice_a_p = rvalue_nodes->front();
	rvalue_nodes->pop_front();
	{
		Dimensions::DimList dims {Dimensions::DimList::Decl::both, {}};
		ret = Dimensions::analyze_expr(rvalue_slice_a_p->get_var(), dim_map, dims);
		ASSERT_EQ(0, ret);

		LOG_DEBUG_N(rvalue_slice_a_p) << "slice_a_p: " << dims;

		Dimensions::DimList ref_dims {
			Dimensions::DimList::Decl::both, // decl
			// list
			{  // msb, lsb, width, is_big, is_packed
				{ 1,  2,  2, false, true}, // dim 1
				{ 0,  7,  8, false, true}, // dim 0
			}
		};
		ASSERT_EQ(ref_dims, dims);
	}

	//----------------------------------------------------
	const auto rvalue_slice_a_m = rvalue_nodes->front();
	rvalue_nodes->pop_front();
	{
		Dimensions::DimList dims {Dimensions::DimList::Decl::both, {}};
		ret = Dimensions::analyze_expr(rvalue_slice_a_m->get_var(), dim_map, dims);
		ASSERT_EQ(0, ret);

		LOG_DEBUG_N(rvalue_slice_a_m) << "slice_a_m: " << dims;

		Dimensions::DimList ref_dims {
			Dimensions::DimList::Decl::both, // decl
			// list
			{  // msb, lsb, width, is_big, is_packed
				{ 0,  1,  2, false, true}, // dim 1
				{ 0,  7,  8, false, true}, // dim 0
			}
		};
		ASSERT_EQ(ref_dims, dims);
	}

	//----------------------------------------------------
	for (int i=0; i<4; i++) {
		const auto rvalue_ptr = rvalue_nodes->front();
		rvalue_nodes->pop_front();
		{
			Dimensions::DimList dims {Dimensions::DimList::Decl::both, {}};
			ret = Dimensions::analyze_expr(rvalue_ptr->get_var(), dim_map, dims);
			ASSERT_EQ(0, ret);

			LOG_DEBUG_N(rvalue_ptr) << "ptr: " << dims;

			Dimensions::DimList ref_dims {
				Dimensions::DimList::Decl::both, // decl
				// list
				{
				}
			};
			ASSERT_EQ(ref_dims, dims);
		}
	}

	//----------------------------------------------------
	for (int i=0; i<3; i++) {
		const auto rvalue_ptr = rvalue_nodes->front();
		rvalue_nodes->pop_front();
		{
			Dimensions::DimList dims {Dimensions::DimList::Decl::both, {}};
			ret = Dimensions::analyze_expr(rvalue_ptr->get_var(), dim_map, dims);
			ASSERT_EQ(0, ret);

			LOG_DEBUG_N(rvalue_ptr) << "ptr: " << dims;

			Dimensions::DimList ref_dims {
				Dimensions::DimList::Decl::both, // decl
				// list
				{  // msb, lsb, width, is_big, is_packed
					{ 0,  7,  8, false, true}, // dim 0
				}
			};
			ASSERT_EQ(ref_dims, dims);
		}
	}

	//----------------------------------------------------
	for (int i=0; i<4; i++) {
		const auto rvalue_invalid = rvalue_nodes->front();
		rvalue_nodes->pop_front();
		{
			Dimensions::DimList dims {Dimensions::DimList::Decl::both, {}};
			ret = Dimensions::analyze_expr(rvalue_invalid->get_var(), dim_map, dims);
			ASSERT_EQ(1, ret);
		}
	}

	//----------------------------------------------------
	const auto rvalue_string = rvalue_nodes->front();
	rvalue_nodes->pop_front();
	{
		Dimensions::DimList dims {Dimensions::DimList::Decl::both, {}};
		ret = Dimensions::analyze_expr(rvalue_string->get_var(), dim_map, dims);
		ASSERT_EQ(0, ret);

		LOG_DEBUG_N(rvalue_string) << "string: " << dims;

		Dimensions::DimList ref_dims {
			Dimensions::DimList::Decl::both, // decl
			// list
			{  // msb, lsb, width, is_big, is_packed
				{87,  0, 88, true, true}, // dim 0
			}
		};
		ASSERT_EQ(ref_dims, dims);
	}

	const auto rvalue_real = rvalue_nodes->front();
	rvalue_nodes->pop_front();
	{
		Dimensions::DimList dims {Dimensions::DimList::Decl::both, {}};
		ret = Dimensions::analyze_expr(rvalue_real->get_var(), dim_map, dims);
		ASSERT_EQ(0, ret);

		LOG_DEBUG_N(rvalue_real) << "real: " << dims;

		Dimensions::DimList ref_dims {
			Dimensions::DimList::Decl::both, // decl
			// list
			{  // msb, lsb, width, is_big, is_packed
				{63,  0, 64, true, true}, // dim 0
			}
		};
		ASSERT_EQ(ref_dims, dims);
	}

	const auto rvalue_integer = rvalue_nodes->front();
	rvalue_nodes->pop_front();
	{
		Dimensions::DimList dims {Dimensions::DimList::Decl::both, {}};
		ret = Dimensions::analyze_expr(rvalue_integer->get_var(), dim_map, dims);
		ASSERT_EQ(0, ret);

		LOG_DEBUG_N(rvalue_integer) << "integer: " << dims;

		Dimensions::DimList ref_dims {
			Dimensions::DimList::Decl::both, // decl
			// list
			{  // msb, lsb, width, is_big, is_packed
				{31,  0, 32, true, true}, // dim 0
			}
		};
		ASSERT_EQ(ref_dims, dims);
	}

	const auto rvalue_bit_vector = rvalue_nodes->front();
	rvalue_nodes->pop_front();
	{
		Dimensions::DimList dims {Dimensions::DimList::Decl::both, {}};
		ret = Dimensions::analyze_expr(rvalue_bit_vector->get_var(), dim_map, dims);
		ASSERT_EQ(0, ret);

		LOG_DEBUG_N(rvalue_bit_vector) << "bit_vector: " << dims;

		Dimensions::DimList ref_dims {
			Dimensions::DimList::Decl::both, // decl
			// list
			{  // msb, lsb, width, is_big, is_packed
				{9,  0, 10, true, true}, // dim 0
			}
		};
		ASSERT_EQ(ref_dims, dims);
	}

}


TEST(PassesAnalysis_Dimensions, gen_decl) {
	ENABLE_LOGGER;

	using namespace Passes::Analysis;

	//---------------------------------------------------
	Dimensions::DimList dims_a {
		Dimensions::DimList::Decl::var, // decl
	   // list
		{  // msb, lsb, width, is_big, is_packed
			{87,  0, 88, true, true}, // dim 0
		}
	};

	const auto gen_dims_a = Dimensions::generate_decl("dims_a", AST::NodeType::Wire, dims_a);
	ASSERT_NE(nullptr, gen_dims_a);

	std::string str_dims_a = Generators::VerilogGenerator().render(gen_dims_a);
	LOG_INFO << str_dims_a;
	ASSERT_EQ("wire [87:0] dims_a;", str_dims_a);

	//---------------------------------------------------
	Dimensions::DimList dims_b {
		Dimensions::DimList::Decl::var, // decl
	   // list
		{  // msb, lsb, width, is_big, is_packed
			{ 3,  0, 4, true, true}, // dim 1
			{ 7,  0, 8, true, true}, // dim 0
		}
	};

	const auto gen_dims_b = Dimensions::generate_decl("dims_b", AST::NodeType::Wire, dims_b);
	ASSERT_NE(nullptr, gen_dims_b);

	std::string str_dims_b = Generators::VerilogGenerator().render(gen_dims_b);
	LOG_INFO << str_dims_b;
	ASSERT_EQ("wire [3:0] [7:0] dims_b;", str_dims_b);

	//---------------------------------------------------
	Dimensions::DimList dims_c {
		Dimensions::DimList::Decl::var, // decl
	   // list
		{  // msb, lsb, width, is_big, is_packed
			{ 0,  3, 4, false, true}, // dim 1
			{ 7,  0, 8, true, true}, // dim 0
		}
	};

	const auto gen_dims_c = Dimensions::generate_decl("dims_c", AST::NodeType::Reg, dims_c);
	ASSERT_NE(nullptr, gen_dims_c);

	std::string str_dims_c = Generators::VerilogGenerator().render(gen_dims_c);
	LOG_INFO << str_dims_c;
	ASSERT_EQ("reg [0:3] [7:0] dims_c;", str_dims_c);

	//---------------------------------------------------
	Dimensions::DimList dims_d {
		Dimensions::DimList::Decl::var, // decl
	   // list
		{  // msb, lsb, width, is_big, is_packed
			{ 7,  0, 8, true, false}, // dim 2
			{ 0,  3, 4, false, true}, // dim 1
			{ 7,  0, 8, true, true},  // dim 0
		}
	};

	const auto gen_dims_d = Dimensions::generate_decl("dims_d", AST::NodeType::Wire, dims_d);
	ASSERT_NE(nullptr, gen_dims_d);

	std::string str_dims_d = Generators::VerilogGenerator().render(gen_dims_d);
	LOG_INFO << str_dims_d;
	ASSERT_EQ("wire [0:3] [7:0] dims_d [7:0];", str_dims_d);

	//---------------------------------------------------
	Dimensions::DimList dims_e {
		Dimensions::DimList::Decl::var, // decl
	   // list
		{  // msb, lsb, width, is_big, is_packed
			{ 0,  1, 8, false, false}, // dim 3
			{ 5,  0, 8, true, false},  // dim 2
			{ 0,  3, 4, false, true},  // dim 1
			{ 7,  0, 8, true, true},   // dim 0
		}
	};

	const auto gen_dims_e = Dimensions::generate_decl("dims_e", AST::NodeType::Reg, dims_e);
	ASSERT_NE(nullptr, gen_dims_e);

	std::string str_dims_e = Generators::VerilogGenerator().render(gen_dims_e);
	LOG_INFO << str_dims_e;
	ASSERT_EQ("reg [0:3] [7:0] dims_e [0:1][5:0];", str_dims_e);

}
