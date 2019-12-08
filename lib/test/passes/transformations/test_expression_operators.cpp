#include "../../helpers/helpers.hpp"
#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/expression_operators.hpp>
#include <gtest/gtest.h>

#include <cstdint>
#include <cmath>

using namespace Veriparse;


TEST(PassesTransformation_ExpressionOperators, UnaryUnsignedFct) {
	AST::IntConstN::Ptr x;
	AST::IntConstN::Ptr res;

	Passes::Transformations::unsigned_fct<AST::IntConstN::Ptr> unsigned_fct_intconstn;
	x = std::make_shared<AST::IntConstN>(10, 8, true, -2);
	res = unsigned_fct_intconstn(x);
	ASSERT_EQ(x->get_base(), res->get_base());
	ASSERT_EQ(x->get_size(), res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(0xFE), res->get_value());

	x = std::make_shared<AST::IntConstN>(10, -1, true, -3);
	res = unsigned_fct_intconstn(x);
	ASSERT_EQ(x->get_base(), res->get_base());
	ASSERT_EQ(32, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(0xFFFFFFFD), res->get_value());
}

TEST(PassesTransformation_ExpressionOperators, UnarySignedFct) {
	AST::IntConstN::Ptr x;
	AST::IntConstN::Ptr res;

	Passes::Transformations::signed_fct<AST::IntConstN::Ptr> signed_fct_intconstn;
	x = std::make_shared<AST::IntConstN>(10, 8, false, 0xFE);
	res = signed_fct_intconstn(x);
	ASSERT_EQ(x->get_base(), res->get_base());
	ASSERT_EQ(x->get_size(), res->get_size());
	ASSERT_EQ(true, res->get_sign());
	ASSERT_EQ(mpz_class(-2), res->get_value());

	x = std::make_shared<AST::IntConstN>(10, -1, false, 0xFFFFFFFD);
	res = signed_fct_intconstn(x);
	ASSERT_EQ(x->get_base(), res->get_base());
	ASSERT_EQ(32, res->get_size());
	ASSERT_EQ(true, res->get_sign());
	ASSERT_EQ(mpz_class(-3), res->get_value());
}

TEST(PassesTransformation_ExpressionOperators, UnaryCLog2) {
	Passes::Transformations::clog2<int32_t> clog2;
	ASSERT_EQ(0, clog2(-1));
	ASSERT_EQ(0, clog2(0));
	for(int32_t x=1; x<=1025; x++) {
		ASSERT_EQ(static_cast<int32_t>(std::ceil(std::log2(x))), clog2(x));
	}

	ASSERT_EQ(0.0, clog2(-1.0));
	ASSERT_EQ(0.0, clog2(0.0));
	ASSERT_EQ(0.0, clog2(1.0));
	ASSERT_EQ(1.0, clog2(2.0));
	ASSERT_EQ(2.0, clog2(3.0));
	ASSERT_EQ(2.0, clog2(4.0));
	ASSERT_EQ(3.0, clog2(5.0));

	Passes::Transformations::clog2<AST::FloatConst::Ptr> clog2_floatconst;
	ASSERT_EQ(3.0, clog2_floatconst(std::make_shared<AST::FloatConst>(5))->get_value());

	AST::IntConstN::Ptr x;
	AST::IntConstN::Ptr res;
	Passes::Transformations::clog2<AST::IntConstN::Ptr> clog2_intconstn;
	x = std::make_shared<AST::IntConstN>(10, -1, true, 5);
	res = clog2_intconstn(x);
	ASSERT_EQ(x->get_base(), res->get_base());
	ASSERT_EQ(x->get_size(), res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(3), res->get_value());
}

TEST(PassesTransformation_ExpressionOperators, UnaryPlus) {
	Passes::Transformations::uplus<int32_t> uplus;
	ASSERT_EQ(5, uplus(5));
	ASSERT_EQ(-5, uplus(-5));

	Passes::Transformations::uplus<double> uplus_double;
	ASSERT_EQ(5.0, uplus_double(5.0));
	ASSERT_EQ(-5.0, uplus_double(-5.0));

	Passes::Transformations::uplus<AST::FloatConst::Ptr> uplus_floatconst;
	ASSERT_EQ(5.0, uplus_floatconst(std::make_shared<AST::FloatConst>(5))->get_value());

	AST::IntConstN::Ptr x;
	AST::IntConstN::Ptr res;
	Passes::Transformations::uplus<AST::IntConstN::Ptr> uplus_intconstn;
	x = std::make_shared<AST::IntConstN>(10, -1, true, 5);
	res = uplus_intconstn(x);
	ASSERT_EQ(x->get_base(), res->get_base());
	ASSERT_EQ(x->get_size(), res->get_size());
	ASSERT_EQ(x->get_sign(), res->get_sign());
	ASSERT_EQ(mpz_class(5), res->get_value());
}

TEST(PassesTransformation_ExpressionOperators, UnaryMinus) {
	Passes::Transformations::uminus<int32_t> uminus;
	ASSERT_EQ(-5, uminus(5));
	ASSERT_EQ(5, uminus(-5));

	Passes::Transformations::uminus<double> uminus_double;
	ASSERT_EQ(-5.0, uminus_double(5.0));
	ASSERT_EQ(5.0, uminus_double(-5.0));

	Passes::Transformations::uminus<AST::FloatConst::Ptr> uplus_floatconst;
	ASSERT_EQ(-5.0, uplus_floatconst(std::make_shared<AST::FloatConst>(5))->get_value());

	AST::IntConstN::Ptr x;
	AST::IntConstN::Ptr res;
	Passes::Transformations::uminus<AST::IntConstN::Ptr> uminus_intconstn;
	x = std::make_shared<AST::IntConstN>(10, -1, true, 5);
	res = uminus_intconstn(x);
	ASSERT_EQ(x->get_base(), res->get_base());
	ASSERT_EQ(x->get_size(), res->get_size());
	ASSERT_EQ(x->get_sign(), res->get_sign());
	ASSERT_EQ(mpz_class(-5), res->get_value());
}

TEST(PassesTransformation_ExpressionOperators, UnaryLogicalNot) {
	Passes::Transformations::ulnot<int32_t> ulnot;
	ASSERT_EQ(0, ulnot(5));
	ASSERT_EQ(1, ulnot(0));

	AST::IntConstN::Ptr x;
	AST::IntConstN::Ptr res;
	Passes::Transformations::ulnot<AST::IntConstN::Ptr> ulnot_intconstn;
	x = std::make_shared<AST::IntConstN>(10, -1, true, 5);
	res = ulnot_intconstn(x);
	ASSERT_EQ(x->get_base(), res->get_base());
	ASSERT_EQ(x->get_size(), res->get_size());
	ASSERT_EQ(x->get_sign(), res->get_sign());
	ASSERT_EQ(mpz_class(0), res->get_value());
}

TEST(PassesTransformation_ExpressionOperators, UnaryNot) {
	Passes::Transformations::unot<uint32_t> unot;
	ASSERT_EQ(0xFFFFFFFA, unot(5));
	ASSERT_EQ(0xFFFFFFFF, unot(0));

	AST::IntConstN::Ptr x;
	AST::IntConstN::Ptr y;
	AST::IntConstN::Ptr res;

	Passes::Transformations::unot<AST::IntConstN::Ptr> unot_intconstn;
	x = std::make_shared<AST::IntConstN>(10, -1, true, 5);
	res = unot_intconstn(x);
	ASSERT_EQ(x->get_base(), res->get_base());
	ASSERT_EQ(x->get_size(), res->get_size());
	ASSERT_EQ(x->get_sign(), res->get_sign());
	ASSERT_EQ(mpz_class(-6), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, -1, true, 1);
	res = unot_intconstn(x);
	ASSERT_EQ(x->get_base(), res->get_base());
	ASSERT_EQ(x->get_size(), res->get_size());
	ASSERT_EQ(x->get_sign(), res->get_sign());
	ASSERT_EQ(mpz_class(-2), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, -1, true, -6);
	res = unot_intconstn(x);
	ASSERT_EQ(x->get_base(), res->get_base());
	ASSERT_EQ(x->get_size(), res->get_size());
	ASSERT_EQ(x->get_sign(), res->get_sign());
	ASSERT_EQ(mpz_class(5), res->get_value());
}

TEST(PassesTransformation_ExpressionOperators, UnaryAnd) {
	Passes::Transformations::uand<uint8_t> uand;
	ASSERT_EQ(0, uand(0x00));
	ASSERT_EQ(0, uand(0xFE));
	ASSERT_EQ(1, uand(0xFF));
	Passes::Transformations::uand<int8_t> uand_s;
	ASSERT_EQ(0, uand_s(0));
	ASSERT_EQ(0, uand_s(1));
	ASSERT_EQ(1, uand_s(-1));

	AST::IntConstN::Ptr x;
	AST::IntConstN::Ptr y;
	AST::IntConstN::Ptr res;

	Passes::Transformations::uand<AST::IntConstN::Ptr> uand_intconstn;
	x = std::make_shared<AST::IntConstN>(10, -1, true, 5);
	res = uand_intconstn(x);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(0), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, -1, true, -1);
	res = uand_intconstn(x);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(1), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, 4, false, 15);
	res = uand_intconstn(x);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(1), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, 4, false, 14);
	res = uand_intconstn(x);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(0), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, 4, true, -1);
	res = uand_intconstn(x);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(1), res->get_value());
}

TEST(PassesTransformation_ExpressionOperators, UnaryNand) {
	Passes::Transformations::unand<uint8_t> unand;
	ASSERT_EQ(1, unand(0x00));
	ASSERT_EQ(1, unand(0xFE));
	ASSERT_EQ(0, unand(0xFF));
	Passes::Transformations::unand<int8_t> unand_s;
	ASSERT_EQ(1, unand_s(0));
	ASSERT_EQ(1, unand_s(1));
	ASSERT_EQ(0, unand_s(-1));

	AST::IntConstN::Ptr x;
	AST::IntConstN::Ptr res;

	Passes::Transformations::unand<AST::IntConstN::Ptr> unand_intconstn;
	x = std::make_shared<AST::IntConstN>(10, -1, true, 5);
	res = unand_intconstn(x);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(1), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, -1, true, -1);
	res = unand_intconstn(x);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(0), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, 4, false, 15);
	res = unand_intconstn(x);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(0), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, 4, false, 14);
	res = unand_intconstn(x);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(1), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, 4, true, -1);
	res = unand_intconstn(x);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(0), res->get_value());
}

TEST(PassesTransformation_ExpressionOperators, UnaryOr) {
	Passes::Transformations::uor<uint8_t> uor;
	ASSERT_EQ(0, uor(0x00));
	ASSERT_EQ(1, uor(0x01));
	ASSERT_EQ(1, uor(0xFF));

	AST::IntConstN::Ptr x;
	AST::IntConstN::Ptr res;
	Passes::Transformations::uor<AST::IntConstN::Ptr> uor_intconstn;
	x = std::make_shared<AST::IntConstN>(10, 8, true, 0);
	res = uor_intconstn(x);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(0), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, 8, true, 1);
	res = uor_intconstn(x);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(1), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, 8, true, 0xff);
	res = uor_intconstn(x);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(1), res->get_value());
}

TEST(PassesTransformation_ExpressionOperators, UnaryNotOr) {
	Passes::Transformations::unor<uint8_t> unor;
	ASSERT_EQ(1, unor(0x00));
	ASSERT_EQ(0, unor(0x01));
	ASSERT_EQ(0, unor(0xFF));

	AST::IntConstN::Ptr x;
	AST::IntConstN::Ptr res;
	Passes::Transformations::unor<AST::IntConstN::Ptr> unor_intconstn;
	x = std::make_shared<AST::IntConstN>(10, 8, true, 0);
	res = unor_intconstn(x);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(1), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, 8, true, 1);
	res = unor_intconstn(x);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(0), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, 8, true, 0xff);
	res = unor_intconstn(x);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(0), res->get_value());
}

TEST(PassesTransformation_ExpressionOperators, UnaryXor) {
	Passes::Transformations::uxor<uint8_t> uxor;
	ASSERT_EQ(0, uxor(0x00));
	ASSERT_EQ(1, uxor(0x01));
	ASSERT_EQ(1, uxor(0xEF));
	ASSERT_EQ(0, uxor(0xFF));
	Passes::Transformations::uxor<int32_t> uxor_s;
	ASSERT_EQ(0, uxor_s(-1));
	ASSERT_EQ(1, uxor_s(-2));
	ASSERT_EQ(1, uxor_s(1));
	ASSERT_EQ(0, uxor_s(5));

	AST::IntConstN::Ptr x;
	AST::IntConstN::Ptr y;
	AST::IntConstN::Ptr res;

	Passes::Transformations::uxor<AST::IntConstN::Ptr> uxor_intconstn;
	x = std::make_shared<AST::IntConstN>(10, -1, true, 0);
	res = uxor_intconstn(x);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(0), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, -1, true, 1);
	res = uxor_intconstn(x);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(1), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, -1, true, -1);
	res = uxor_intconstn(x);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(0), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, -1, true, -2);
	res = uxor_intconstn(x);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(1), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, 8, false, 0xff);
	res = uxor_intconstn(x);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(0), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, 8, false, 0xef);
	res = uxor_intconstn(x);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(1), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, 8, true, -128);
	res = uxor_intconstn(x);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(1), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, 8, true, -1);
	res = uxor_intconstn(x);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(0), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, 9, true, -1);
	res = uxor_intconstn(x);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(1), res->get_value());
}

TEST(PassesTransformation_ExpressionOperators, UnaryXnor) {
	Passes::Transformations::uxnor<uint8_t> uxnor;
	ASSERT_EQ(1, uxnor(0x00));
	ASSERT_EQ(0, uxnor(0x01));
	ASSERT_EQ(0, uxnor(0xEF));
	ASSERT_EQ(1, uxnor(0xFF));
	Passes::Transformations::uxnor<int32_t> uxnor_s;
	ASSERT_EQ(1, uxnor_s(-1));
	ASSERT_EQ(0, uxnor_s(-2));
	ASSERT_EQ(0, uxnor_s(1));
	ASSERT_EQ(1, uxnor_s(5));

	AST::IntConstN::Ptr x;
	AST::IntConstN::Ptr y;
	AST::IntConstN::Ptr res;

	Passes::Transformations::uxnor<AST::IntConstN::Ptr> uxnor_intconstn;
	x = std::make_shared<AST::IntConstN>(10, -1, true, 0);
	res = uxnor_intconstn(x);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(1), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, -1, true, 1);
	res = uxnor_intconstn(x);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(0), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, -1, true, -1);
	res = uxnor_intconstn(x);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(1), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, -1, true, -2);
	res = uxnor_intconstn(x);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(0), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, 8, false, 0xff);
	res = uxnor_intconstn(x);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(1), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, 8, false, 0xef);
	res = uxnor_intconstn(x);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(0), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, 8, true, -128);
	res = uxnor_intconstn(x);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(0), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, 8, true, -1);
	res = uxnor_intconstn(x);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(1), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, 9, true, -1);
	res = uxnor_intconstn(x);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(0), res->get_value());
}

TEST(PassesTransformation_ExpressionOperators, BinaryPointer) {
	Passes::Transformations::pointer<uint32_t> pointer;
	ASSERT_TRUE(pointer(0x89ABCDEF, 31) == 1);
	ASSERT_TRUE(pointer(0x89ABCDEF, 4) == 0);

	AST::IntConstN::Ptr x;
	AST::IntConstN::Ptr y;
	AST::IntConstN::Ptr res;
	Passes::Transformations::pointer<AST::IntConstN::Ptr> pointer_intconstn;
	x = std::make_shared<AST::IntConstN>(10, -1, true, 0x89ABCDEF);
	y = std::make_shared<AST::IntConstN>(10, -1, true, 31);
	res = pointer_intconstn(x, y);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(1), res->get_value());

	x = std::make_shared<AST::IntConstN>(10, -1, true, 0x89ABCDEF);
	y = std::make_shared<AST::IntConstN>(10, -1, true, 4);
	res = pointer_intconstn(x, y);
	LOG_INFO << res;
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(0), res->get_value());
}

TEST(PassesTransformation_ExpressionOperators, BinaryPower) {
	Passes::Transformations::power<uint32_t> power;
	ASSERT_TRUE(power(2, 0) == 1);
	ASSERT_TRUE(power(1, 2) == 1);
	ASSERT_TRUE(power(12, 5) == 248832);

	Passes::Transformations::power<int32_t> power_s;
	ASSERT_TRUE(power_s(-1, 2) == 1);
	ASSERT_TRUE(power_s(-12, 5) == -248832);

	Passes::Transformations::power<double> power_double;
	ASSERT_TRUE(power_double(1, 2) == 1);
	ASSERT_TRUE(power_double(12, 5) == 248832);

	Passes::Transformations::power<AST::FloatConst::Ptr> power_floatconst;
	ASSERT_TRUE(power_floatconst(std::make_shared<AST::FloatConst>(2.0),
	                             std::make_shared<AST::FloatConst>(5.0))->get_value() == 32.0);

	AST::IntConstN::Ptr x;
	AST::IntConstN::Ptr y;
	AST::IntConstN::Ptr res;
	Passes::Transformations::power<AST::IntConstN::Ptr> power_intconstn;
	x = std::make_shared<AST::IntConstN>(10, -1, true, 1);
	y = std::make_shared<AST::IntConstN>(10, -1, true, 2);
	res = power_intconstn(x, y);
	ASSERT_EQ(10, res->get_base());
	ASSERT_EQ(-1, res->get_size());
	ASSERT_EQ(true, res->get_sign());
	ASSERT_EQ(mpz_class(1), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, -1, true, 12);
	y = std::make_shared<AST::IntConstN>(10, -1, true, 5);
	res = power_intconstn(x, y);
	ASSERT_EQ(10, res->get_base());
	ASSERT_EQ(-1, res->get_size());
	ASSERT_EQ(true, res->get_sign());
	ASSERT_EQ(mpz_class(248832), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, -1, true, -12);
	y = std::make_shared<AST::IntConstN>(10, -1, true, 6);
	res = power_intconstn(x, y);
	ASSERT_EQ(10, res->get_base());
	ASSERT_EQ(-1, res->get_size());
	ASSERT_EQ(true, res->get_sign());
	ASSERT_EQ(mpz_class(2985984), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, -1, true, -12);
	y = std::make_shared<AST::IntConstN>(10, -1, true, 7);
	res = power_intconstn(x, y);
	ASSERT_EQ(10, res->get_base());
	ASSERT_EQ(-1, res->get_size());
	ASSERT_EQ(true, res->get_sign());
	ASSERT_EQ(mpz_class(-35831808), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, -1, true, -12);
	y = std::make_shared<AST::IntConstN>(10, -1, true, -6);
	res = power_intconstn(x, y);
	ASSERT_EQ(10, res->get_base());
	ASSERT_EQ(-1, res->get_size());
	ASSERT_EQ(true, res->get_sign());
	ASSERT_EQ(mpz_class(0), res->get_value());
}

TEST(PassesTransformation_ExpressionOperators, BinaryTimes) {
	Passes::Transformations::times<uint32_t> times;
	ASSERT_TRUE(times(1, 2) == 2);
	ASSERT_TRUE(times(12, 5) == 60);

	Passes::Transformations::times<int32_t> times_s;
	ASSERT_TRUE(times_s(-1, 2) == -2);
	ASSERT_TRUE(times_s(12, -5) == -60);
	ASSERT_TRUE(times_s(-1, -1) == 1);

	Passes::Transformations::times<double> times_double;
	ASSERT_TRUE(times_double(-1, 2) == -2);
	ASSERT_TRUE(times_double(12, -5) == -60);
	ASSERT_TRUE(times_double(-1, -1) == 1);

	Passes::Transformations::times<AST::FloatConst::Ptr> times_floatconst;
	ASSERT_TRUE(times_floatconst(std::make_shared<AST::FloatConst>(2.0),
	                             std::make_shared<AST::FloatConst>(5.0))->get_value() == 10.0);

	AST::IntConstN::Ptr x;
	AST::IntConstN::Ptr y;
	AST::IntConstN::Ptr res;
	Passes::Transformations::times<AST::IntConstN::Ptr> times_intconstn;
	x = std::make_shared<AST::IntConstN>(10, -1, true, 1);
	y = std::make_shared<AST::IntConstN>(10, -1, true, 2);
	res = times_intconstn(x, y);
	ASSERT_EQ(10, res->get_base());
	ASSERT_EQ(-1, res->get_size());
	ASSERT_EQ(true, res->get_sign());
	ASSERT_EQ(mpz_class(2), res->get_value());
	x = std::make_shared<AST::IntConstN>(16, 8, true, -1);
	y = std::make_shared<AST::IntConstN>(10, 8, true, 2);
	res = times_intconstn(x, y);
	ASSERT_EQ(16, res->get_base());
	ASSERT_EQ(16, res->get_size());
	ASSERT_EQ(true, res->get_sign());
	ASSERT_EQ(mpz_class(-2), res->get_value());
}

TEST(PassesTransformation_ExpressionOperators, BinaryDivide) {
	Passes::Transformations::divide<uint32_t> divide;
	ASSERT_TRUE(divide(1, 2) == 0);
	ASSERT_TRUE(divide(12, 5) == 2);

	Passes::Transformations::divide<int32_t> divide_s;
	ASSERT_TRUE(divide_s(-1, 2) == 0);
	ASSERT_TRUE(divide_s(12, -5) == -2);
	ASSERT_TRUE(divide_s(-1, -1) == 1);

	Passes::Transformations::divide<double> divide_double;
	ASSERT_TRUE(divide_double(-1, 2) == -0.5);
	ASSERT_TRUE(divide_double(12, -5) == -2.4);
	ASSERT_TRUE(divide_double(-1, -1) == 1);

	Passes::Transformations::divide<AST::FloatConst::Ptr> divide_floatconst;
	ASSERT_TRUE(divide_floatconst(std::make_shared<AST::FloatConst>(2.0),
	                              std::make_shared<AST::FloatConst>(5.0))->get_value() == 0.4);

	AST::IntConstN::Ptr x;
	AST::IntConstN::Ptr y;
	AST::IntConstN::Ptr res;
	Passes::Transformations::divide<AST::IntConstN::Ptr> divide_intconstn;
	x = std::make_shared<AST::IntConstN>(10, 16, false, 1);
	y = std::make_shared<AST::IntConstN>(10, -1, true, 2);
	res = divide_intconstn(x, y);
	ASSERT_EQ(10, res->get_base());
	ASSERT_EQ(16, res->get_size());
	ASSERT_EQ(true, res->get_sign());
	ASSERT_EQ(mpz_class(0), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, 17, true, -54511);
	y = std::make_shared<AST::IntConstN>(10, 8, true, -57);
	res = divide_intconstn(x, y);
	ASSERT_EQ(10, res->get_base());
	ASSERT_EQ(17, res->get_size());
	ASSERT_EQ(true, res->get_sign());
	ASSERT_EQ(mpz_class(956), res->get_value());
}

TEST(PassesTransformation_ExpressionOperators, BinaryModulus) {
	Passes::Transformations::mod<uint32_t> mod;
	ASSERT_TRUE(mod(1, 2) == 1);
	ASSERT_TRUE(mod(12, 5) == 2);

	Passes::Transformations::mod<int32_t> mod_s;
	ASSERT_TRUE(mod_s(-1, 2) == -1);
	ASSERT_TRUE(mod_s(12, -5) == 2);
	ASSERT_TRUE(mod_s(-1, -1) == 0);

	Passes::Transformations::mod<double> mod_double;
	ASSERT_TRUE(mod_double(-1, 2) == -1);
	ASSERT_TRUE(mod_double(12, -5) == 2);
	ASSERT_TRUE(mod_double(-1, -1) == 0);
	ASSERT_TRUE(mod_double(360.0, 360.0) == 0);

	Passes::Transformations::mod<AST::FloatConst::Ptr> mod_floatconst;
	ASSERT_TRUE(mod_floatconst(std::make_shared<AST::FloatConst>(12.0),
	                           std::make_shared<AST::FloatConst>(3.5))->get_value() == 1.5);

	AST::IntConstN::Ptr x;
	AST::IntConstN::Ptr y;
	AST::IntConstN::Ptr res;
	Passes::Transformations::mod<AST::IntConstN::Ptr> mod_intconstn;
	x = std::make_shared<AST::IntConstN>(10, -1, true, -9);
	y = std::make_shared<AST::IntConstN>(10, 8, true, -7);
	res = mod_intconstn(x, y);
	ASSERT_EQ(10, res->get_base());
	ASSERT_EQ(8, res->get_size());
	ASSERT_EQ(true, res->get_sign());
	ASSERT_EQ(mpz_class(-2), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, -1, true, -9);
	y = std::make_shared<AST::IntConstN>(10, 8, true, 7);
	res = mod_intconstn(x, y);
	ASSERT_EQ(10, res->get_base());
	ASSERT_EQ(8, res->get_size());
	ASSERT_EQ(true, res->get_sign());
	ASSERT_EQ(mpz_class(-2), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, -1, false, 9);
	y = std::make_shared<AST::IntConstN>(10, 8, true, -7);
	res = mod_intconstn(x, y);
	ASSERT_EQ(10, res->get_base());
	ASSERT_EQ(8, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(2), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, 7, false, 9);
	y = std::make_shared<AST::IntConstN>(10, 8, true, 7);
	res = mod_intconstn(x, y);
	ASSERT_EQ(10, res->get_base());
	ASSERT_EQ(8, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(2), res->get_value());
}

TEST(PassesTransformation_ExpressionOperators, BinaryPlus) {
	Passes::Transformations::plus<uint32_t> plus;
	ASSERT_TRUE(plus(1, 2) == 3);
	ASSERT_TRUE(plus(12, 5) == 17);
	ASSERT_TRUE(plus(1, 1) == 2);

	Passes::Transformations::plus<int32_t> plus_s;
	ASSERT_TRUE(plus_s(-1, -2) == -3);
	ASSERT_TRUE(plus_s(-12, 5) == -7);
	ASSERT_TRUE(plus_s(1, -1) == 0);

	Passes::Transformations::plus<double> plus_double;
	ASSERT_TRUE(plus_double(-1, -2) == -3);
	ASSERT_TRUE(plus_double(-12, 5) == -7);
	ASSERT_TRUE(plus_double(1, -1) == 0);

	Passes::Transformations::plus<AST::FloatConst::Ptr> plus_floatconst;
	ASSERT_TRUE(plus_floatconst(std::make_shared<AST::FloatConst>(12.0),
	                            std::make_shared<AST::FloatConst>(3.5))->get_value() == 15.5);

	AST::IntConstN::Ptr x;
	AST::IntConstN::Ptr y;
	AST::IntConstN::Ptr res;
	Passes::Transformations::plus<AST::IntConstN::Ptr> plus_intconstn;
	x = std::make_shared<AST::IntConstN>(10, -1, true, -9);
	y = std::make_shared<AST::IntConstN>(10, 8, true, -7);
	res = plus_intconstn(x, y);
	ASSERT_EQ(10, res->get_base());
	ASSERT_EQ(-1, res->get_size());
	ASSERT_EQ(true, res->get_sign());
	ASSERT_EQ(mpz_class(-16), res->get_value());
}

TEST(PassesTransformation_ExpressionOperators, BinaryMinus) {
	Passes::Transformations::minus<uint32_t> minus;
	ASSERT_TRUE(minus(1, 2) == 0xffffffff);
	ASSERT_TRUE(minus(12, 5) == 7);
	ASSERT_TRUE(minus(1, 1) == 0);

	Passes::Transformations::minus<int32_t> minus_s;
	ASSERT_TRUE(minus_s(-1, -2) == 1);
	ASSERT_TRUE(minus_s(-12, 5) == -17);
	ASSERT_TRUE(minus_s(1, -1) == 2);

	Passes::Transformations::minus<double> minus_double;
	ASSERT_TRUE(minus_double(-1, -2) == 1);
	ASSERT_TRUE(minus_double(-12, 5) == -17);
	ASSERT_TRUE(minus_double(1, -1) == 2);

	Passes::Transformations::minus<AST::FloatConst::Ptr> minus_floatconst;
	ASSERT_TRUE(minus_floatconst(std::make_shared<AST::FloatConst>(-12.0),
	                             std::make_shared<AST::FloatConst>(5.5))->get_value() == -17.5);

	AST::IntConstN::Ptr x;
	AST::IntConstN::Ptr y;
	AST::IntConstN::Ptr res;
	Passes::Transformations::minus<AST::IntConstN::Ptr> minus_intconstn;
	x = std::make_shared<AST::IntConstN>(10, -1, true, -9);
	y = std::make_shared<AST::IntConstN>(10, 8, true, -7);
	res = minus_intconstn(x, y);
	ASSERT_EQ(10, res->get_base());
	ASSERT_EQ(-1, res->get_size());
	ASSERT_EQ(true, res->get_sign());
	ASSERT_EQ(mpz_class(-2), res->get_value());
}

TEST(PassesTransformation_ExpressionOperators, BinaryShiftLogicalLeft) {
	Passes::Transformations::sll<uint32_t> sll;
	ASSERT_TRUE(sll(1, 5) == 32);

	AST::IntConstN::Ptr x;
	AST::IntConstN::Ptr y;
	AST::IntConstN::Ptr res;
	Passes::Transformations::sll<AST::IntConstN::Ptr> sll_intconstn;
	x = std::make_shared<AST::IntConstN>(10, -1, true, -9);
	y = std::make_shared<AST::IntConstN>(10, 8, true, 2);
	res = sll_intconstn(x, y);
	ASSERT_EQ(10, res->get_base());
	ASSERT_EQ(-1, res->get_size());
	ASSERT_EQ(true, res->get_sign());
	ASSERT_EQ(mpz_class(-36), res->get_value());
	x = std::make_shared<AST::IntConstN>(16, 10, false, 12);
	y = std::make_shared<AST::IntConstN>(10, 8, true, 3);
	res = sll_intconstn(x, y);
	ASSERT_EQ(16, res->get_base());
	ASSERT_EQ(13, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(96), res->get_value());
}

TEST(PassesTransformation_ExpressionOperators, BinaryShiftRightLeft) {
	Passes::Transformations::srl<uint32_t> srl;
	ASSERT_TRUE(srl(-1, 16) == 0xffff);

	Passes::Transformations::srl<int32_t> srl_s;
	ASSERT_TRUE(srl_s(-1, 16) == 0xffff);

	AST::IntConstN::Ptr x;
	AST::IntConstN::Ptr y;
	AST::IntConstN::Ptr res;
	Passes::Transformations::srl<AST::IntConstN::Ptr> srl_intconstn;
	x = std::make_shared<AST::IntConstN>(10, -1, true, -38);
	y = std::make_shared<AST::IntConstN>(10, 8, true, 2);
	res = srl_intconstn(x, y);
	ASSERT_EQ(10, res->get_base());
	ASSERT_EQ(-1, res->get_size());
	ASSERT_EQ(true, res->get_sign());
	ASSERT_EQ(mpz_class(22), res->get_value());
	x = std::make_shared<AST::IntConstN>(16, 10, false, 90);
	y = std::make_shared<AST::IntConstN>(10, 8, true, 3);
	res = srl_intconstn(x, y);
	ASSERT_EQ(16, res->get_base());
	ASSERT_EQ(7, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(11), res->get_value());
}

TEST(PassesTransformation_ExpressionOperators, BinaryShiftRightArithmetic) {
	Passes::Transformations::sra<uint32_t> sra;
	ASSERT_TRUE(sra(-1, 16) == 0xffff);

	Passes::Transformations::sra<int32_t> sra_s;
	ASSERT_TRUE(sra_s(-1, 16) == -1);

	AST::IntConstN::Ptr x;
	AST::IntConstN::Ptr y;
	AST::IntConstN::Ptr res;
	Passes::Transformations::sra<AST::IntConstN::Ptr> sra_intconstn;
	x = std::make_shared<AST::IntConstN>(10, -1, true, -38);
	y = std::make_shared<AST::IntConstN>(10, 8, true, 2);
	res = sra_intconstn(x, y);
	ASSERT_EQ(10, res->get_base());
	ASSERT_EQ(-1, res->get_size());
	ASSERT_EQ(true, res->get_sign());
	ASSERT_EQ(mpz_class(-10), res->get_value());
	x = std::make_shared<AST::IntConstN>(16, 10, false, 90);
	y = std::make_shared<AST::IntConstN>(10, 8, true, 3);
	res = sra_intconstn(x, y);
	ASSERT_EQ(16, res->get_base());
	ASSERT_EQ(7, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(11), res->get_value());
}

TEST(PassesTransformation_ExpressionOperators, BinaryLessThan) {
	Passes::Transformations::less_than<uint32_t> less_than;
	ASSERT_TRUE(less_than(-1, 1) == 0);

	Passes::Transformations::less_than<int32_t> less_than_s;
	ASSERT_TRUE(less_than_s(-1, 1) == 1);

	Passes::Transformations::less_than<double> less_than_double;
	ASSERT_TRUE(less_than_double(-1, 1) == 1);
	ASSERT_TRUE(less_than_double(-1, -1) == 0);

	Passes::Transformations::less_than<AST::FloatConst::Ptr> less_than_floatconst;
	ASSERT_TRUE(less_than_floatconst(std::make_shared<AST::FloatConst>(-12.0),
	                                 std::make_shared<AST::FloatConst>(5.5))->get_value() == 1);

	AST::IntConstN::Ptr x;
	AST::IntConstN::Ptr y;
	AST::IntConstN::Ptr res;
	Passes::Transformations::less_than<AST::IntConstN::Ptr> less_than_intconstn;
	x = std::make_shared<AST::IntConstN>(10, -1, true, -38);
	y = std::make_shared<AST::IntConstN>(10, 8, true, 2);
	res = less_than_intconstn(x, y);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(1), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, -1, true, -38);
	y = std::make_shared<AST::IntConstN>(10, 8, true, -38);
	res = less_than_intconstn(x, y);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(0), res->get_value());
}

TEST(PassesTransformation_ExpressionOperators, BinaryGreaterThan) {
	Passes::Transformations::greater_than<uint32_t> greater_than;
	ASSERT_TRUE(greater_than(-1, 1) == 1);

	Passes::Transformations::greater_than<int32_t> greater_than_s;
	ASSERT_TRUE(greater_than_s(-1, 1) == 0);

	Passes::Transformations::greater_than<double> greater_than_double;
	ASSERT_TRUE(greater_than_double(-1, 1) == 0);
	ASSERT_TRUE(greater_than_double(-1, -1) == 0);
	ASSERT_TRUE(greater_than_double(2, -1) == 1);

	Passes::Transformations::greater_than<AST::FloatConst::Ptr> greater_than_floatconst;
	ASSERT_TRUE(greater_than_floatconst(std::make_shared<AST::FloatConst>(-12.0),
	                                    std::make_shared<AST::FloatConst>(5.5))->get_value() == 0);

	AST::IntConstN::Ptr x;
	AST::IntConstN::Ptr y;
	AST::IntConstN::Ptr res;
	Passes::Transformations::greater_than<AST::IntConstN::Ptr> greater_than_intconstn;
	x = std::make_shared<AST::IntConstN>(10, -1, true, 38);
	y = std::make_shared<AST::IntConstN>(10, 8, true, 2);
	res = greater_than_intconstn(x, y);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(1), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, -1, true, -38);
	y = std::make_shared<AST::IntConstN>(10, 8, true, 2);
	res = greater_than_intconstn(x, y);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(0), res->get_value());
}

TEST(PassesTransformation_ExpressionOperators, BinaryLessEqual) {
	Passes::Transformations::less_equal<uint32_t> less_equal;
	ASSERT_TRUE(less_equal(-1, 1) == 0);
	ASSERT_TRUE(less_equal(-1, -1) == 1);

	Passes::Transformations::less_equal<int32_t> less_equal_s;
	ASSERT_TRUE(less_equal_s(-1, 1) == 1);
	ASSERT_TRUE(less_equal_s(-1, -1) == 1);

	Passes::Transformations::less_equal<double> less_equal_double;
	ASSERT_TRUE(less_equal_double(2, 1) == 0);
	ASSERT_TRUE(less_equal_double(-1, -1) == 1);

	Passes::Transformations::less_equal<AST::FloatConst::Ptr> less_equal_floatconst;
	ASSERT_TRUE(less_equal_floatconst(std::make_shared<AST::FloatConst>(-12.0),
	                                  std::make_shared<AST::FloatConst>(-12.0))->get_value() == 1);

	AST::IntConstN::Ptr x;
	AST::IntConstN::Ptr y;
	AST::IntConstN::Ptr res;
	Passes::Transformations::less_equal<AST::IntConstN::Ptr> less_equal_intconstn;
	x = std::make_shared<AST::IntConstN>(10, -1, true, -38);
	y = std::make_shared<AST::IntConstN>(10, 8, true, -38);
	res = less_equal_intconstn(x, y);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(1), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, -1, true, -38);
	y = std::make_shared<AST::IntConstN>(10, 8, true,  -39);
	res = less_equal_intconstn(x, y);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(0), res->get_value());
}

TEST(PassesTransformation_ExpressionOperators, BinaryGreaterEqual) {
	Passes::Transformations::greater_equal<uint32_t> greater_equal;
	ASSERT_TRUE(greater_equal(-1, 1) == 1);
	ASSERT_TRUE(greater_equal(-1, -1) == 1);

	Passes::Transformations::greater_equal<int32_t> greater_equal_s;
	ASSERT_TRUE(greater_equal_s(-1, 1) == 0);
	ASSERT_TRUE(greater_equal_s(-1, -1) == 1);

	Passes::Transformations::greater_equal<double> greater_equal_double;
	ASSERT_TRUE(greater_equal_double(2, 1) == 1);
	ASSERT_TRUE(greater_equal_double(-1, 0) == 0);
	ASSERT_TRUE(greater_equal_double(-1, -1) == 1);

	Passes::Transformations::greater_equal<AST::FloatConst::Ptr> greater_equal_floatconst;
	ASSERT_TRUE(greater_equal_floatconst(std::make_shared<AST::FloatConst>(-12.0),
	                                     std::make_shared<AST::FloatConst>(-12.0))->get_value() == 1);

	AST::IntConstN::Ptr x;
	AST::IntConstN::Ptr y;
	AST::IntConstN::Ptr res;
	Passes::Transformations::greater_equal<AST::IntConstN::Ptr> greater_equal_intconstn;
	x = std::make_shared<AST::IntConstN>(10, -1, true, 38);
	y = std::make_shared<AST::IntConstN>(10, 8, true, 38);
	res = greater_equal_intconstn(x, y);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(1), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, -1, true, -38);
	y = std::make_shared<AST::IntConstN>(10, 8, true, 2);
	res = greater_equal_intconstn(x, y);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(0), res->get_value());
}

TEST(PassesTransformation_ExpressionOperators, BinaryEqual) {
	Passes::Transformations::equal<uint32_t> equal;
	ASSERT_TRUE(equal(-1, 1) == 0);
	ASSERT_TRUE(equal(-1, -1) == 1);

	Passes::Transformations::equal<int32_t> equal_s;
	ASSERT_TRUE(equal_s(-1, 1) == 0);
	ASSERT_TRUE(equal_s(-1, -1) == 1);

	Passes::Transformations::equal<double> equal_double;
	ASSERT_TRUE(equal_double(-1, 1) == 0);
	ASSERT_TRUE(equal_double(-1, -1) == 1);

	Passes::Transformations::equal<AST::FloatConst::Ptr> equal_floatconst;
	ASSERT_TRUE(equal_floatconst(std::make_shared<AST::FloatConst>(-12.0),
	                             std::make_shared<AST::FloatConst>(-12.0))->get_value() == 1);

	AST::IntConstN::Ptr x;
	AST::IntConstN::Ptr y;
	AST::IntConstN::Ptr res;
	Passes::Transformations::equal<AST::IntConstN::Ptr> equal_intconstn;
	x = std::make_shared<AST::IntConstN>(10, -1, true, 38);
	y = std::make_shared<AST::IntConstN>(10, 8, true, 38);
	res = equal_intconstn(x, y);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(1), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, -1, true, -38);
	y = std::make_shared<AST::IntConstN>(10, 8, true, 2);
	res = equal_intconstn(x, y);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(0), res->get_value());
}

TEST(PassesTransformation_ExpressionOperators, BinaryNotEqual) {
	Passes::Transformations::not_equal<uint32_t> not_equal;
	ASSERT_TRUE(not_equal(-1, 1) == 1);
	ASSERT_TRUE(not_equal(-1, -1) == 0);

	Passes::Transformations::not_equal<int32_t> not_equal_s;
	ASSERT_TRUE(not_equal_s(-1, 1) == 1);
	ASSERT_TRUE(not_equal_s(-1, -1) == 0);

	Passes::Transformations::not_equal<double> not_equal_double;
	ASSERT_TRUE(not_equal_double(-1, 1) == 1);
	ASSERT_TRUE(not_equal_double(-1, -1) == 0);

	Passes::Transformations::not_equal<AST::FloatConst::Ptr> not_equal_floatconst;
	ASSERT_TRUE(not_equal_floatconst(std::make_shared<AST::FloatConst>(-12.0),
	                                 std::make_shared<AST::FloatConst>(-12.0))->get_value() == 0);

	AST::IntConstN::Ptr x;
	AST::IntConstN::Ptr y;
	AST::IntConstN::Ptr res;
	Passes::Transformations::not_equal<AST::IntConstN::Ptr> not_equal_intconstn;
	x = std::make_shared<AST::IntConstN>(10, -1, true, -38);
	y = std::make_shared<AST::IntConstN>(10, 8, true, -38);
	res = not_equal_intconstn(x, y);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(0), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, -1, true, -38);
	y = std::make_shared<AST::IntConstN>(10, 8, true, 2);
	res = not_equal_intconstn(x, y);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(1), res->get_value());
}

TEST(PassesTransformation_ExpressionOperators, BinaryCaseEqual) {
	Passes::Transformations::case_equal<uint32_t> case_equal;
	ASSERT_TRUE(case_equal(-1, 1) == 0);
	ASSERT_TRUE(case_equal(-1, -1) == 1);

	Passes::Transformations::case_equal<int32_t> case_equal_s;
	ASSERT_TRUE(case_equal_s(-1, 1) == 0);
	ASSERT_TRUE(case_equal_s(-1, -1) == 1);

	AST::IntConstN::Ptr x;
	AST::IntConstN::Ptr y;
	AST::IntConstN::Ptr res;
	Passes::Transformations::case_equal<AST::IntConstN::Ptr> case_equal_intconstn;
	x = std::make_shared<AST::IntConstN>(10, -1, true, -38);
	y = std::make_shared<AST::IntConstN>(10, 8, true, -38);
	res = case_equal_intconstn(x, y);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(1), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, -1, true, -38);
	y = std::make_shared<AST::IntConstN>(10, 8, true, 2);
	res = case_equal_intconstn(x, y);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(0), res->get_value());
}

TEST(PassesTransformation_ExpressionOperators, BinaryCaseNotEqual) {
	Passes::Transformations::case_not_equal<uint32_t> case_not_equal;
	ASSERT_TRUE(case_not_equal(-1, 1) == 1);
	ASSERT_TRUE(case_not_equal(-1, -1) == 0);

	Passes::Transformations::case_not_equal<int32_t> case_not_equal_s;
	ASSERT_TRUE(case_not_equal_s(-1, 1) == 1);
	ASSERT_TRUE(case_not_equal_s(-1, -1) == 0);

	AST::IntConstN::Ptr x;
	AST::IntConstN::Ptr y;
	AST::IntConstN::Ptr res;
	Passes::Transformations::case_not_equal<AST::IntConstN::Ptr> case_not_equal_intconstn;
	x = std::make_shared<AST::IntConstN>(10, -1, true, -38);
	y = std::make_shared<AST::IntConstN>(10, 8, true, -38);
	res = case_not_equal_intconstn(x, y);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(0), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, -1, true, -38);
	y = std::make_shared<AST::IntConstN>(10, 8, true, 2);
	res = case_not_equal_intconstn(x, y);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(1), res->get_value());
}

TEST(PassesTransformation_ExpressionOperators, BinaryBitwiseAnd) {
	Passes::Transformations::bitwise_and<uint32_t> bitwise_and;
	ASSERT_TRUE(bitwise_and(0xaa55ffff, 0x55aaaaaa) == 0x0000aaaa);

	AST::IntConstN::Ptr x;
	AST::IntConstN::Ptr y;
	AST::IntConstN::Ptr res;
	Passes::Transformations::bitwise_and<AST::IntConstN::Ptr> bitwise_and_intconstn;
	x = std::make_shared<AST::IntConstN>(10, -1, true, 0xaa55ffff);
	y = std::make_shared<AST::IntConstN>(10, 8, true, 0x55aaaaaa);
	res = bitwise_and_intconstn(x, y);
	ASSERT_EQ(10, res->get_base());
	ASSERT_EQ(-1, res->get_size());
	ASSERT_EQ(true, res->get_sign());
	ASSERT_EQ(mpz_class(0x0000aaaa), res->get_value());
}

TEST(PassesTransformation_ExpressionOperators, BinaryBitwiseXor) {
	Passes::Transformations::bitwise_xor<uint32_t> bitwise_xor;
	ASSERT_TRUE(bitwise_xor(0x11223344, 0x55667788) == 0x444444cc);

	AST::IntConstN::Ptr x;
	AST::IntConstN::Ptr y;
	AST::IntConstN::Ptr res;
	Passes::Transformations::bitwise_xor<AST::IntConstN::Ptr> bitwise_xor_intconstn;
	x = std::make_shared<AST::IntConstN>(10, -1, true, 0x11223344);
	y = std::make_shared<AST::IntConstN>(10, 32, true, 0x55667788);
	res = bitwise_xor_intconstn(x, y);
	ASSERT_EQ(10, res->get_base());
	ASSERT_EQ(-1, res->get_size());
	ASSERT_EQ(true, res->get_sign());
	ASSERT_EQ(mpz_class(0x444444cc), res->get_value());
}

TEST(PassesTransformation_ExpressionOperators, BinaryBitwiseXnor) {
	Passes::Transformations::bitwise_xnor<uint32_t> bitwise_xnor;
	ASSERT_TRUE(bitwise_xnor(0x11223344, 0x55667788) == 3149642547);

	Passes::Transformations::bitwise_xnor<int32_t> bitwise_xnor_s;
	ASSERT_TRUE(bitwise_xnor_s(0x11223344, 0x55667788) == -1145324749);

	AST::IntConstN::Ptr x;
	AST::IntConstN::Ptr y;
	AST::IntConstN::Ptr res;
	Passes::Transformations::bitwise_xnor<AST::IntConstN::Ptr> bitwise_xnor_intconstn;
	x = std::make_shared<AST::IntConstN>(10, -1, true, 0x11223344);
	y = std::make_shared<AST::IntConstN>(10, 32, true, 0x55667788);
	res = bitwise_xnor_intconstn(x, y);
	ASSERT_EQ(10, res->get_base());
	ASSERT_EQ(-1, res->get_size());
	ASSERT_EQ(true, res->get_sign());
	ASSERT_EQ(mpz_class(-1145324749), res->get_value());
}

TEST(PassesTransformation_ExpressionOperators, BinaryBitwiseOr) {
	Passes::Transformations::bitwise_or<uint32_t> bitwise_or;
	ASSERT_TRUE(bitwise_or(0xaa55fff0, 0x55aaaaa0) == 0xfffffff0);

	AST::IntConstN::Ptr x;
	AST::IntConstN::Ptr y;
	AST::IntConstN::Ptr res;
	Passes::Transformations::bitwise_or<AST::IntConstN::Ptr> bitwise_or_intconstn;
	x = std::make_shared<AST::IntConstN>(10, -1, true, 0xaa55fff0);
	y = std::make_shared<AST::IntConstN>(10, 8, true, 0x55aaaaa0);
	res = bitwise_or_intconstn(x, y);
	ASSERT_EQ(10, res->get_base());
	ASSERT_EQ(-1, res->get_size());
	ASSERT_EQ(true, res->get_sign());
	ASSERT_EQ(mpz_class(0xfffffff0), res->get_value());
}

TEST(PassesTransformation_ExpressionOperators, BinaryLogicalAnd) {
	Passes::Transformations::logical_and<uint32_t> logical_and;
	ASSERT_TRUE(logical_and(0, 0) == 0);
	ASSERT_TRUE(logical_and(0xaa55fff0, 0) == 0);
	ASSERT_TRUE(logical_and(0xaa55fff0, 2) == 1);

	Passes::Transformations::logical_and<double> logical_and_double;
	ASSERT_TRUE(logical_and_double(0, 0) == 0);
	ASSERT_TRUE(logical_and_double(0, 2.3) == 0);
	ASSERT_TRUE(logical_and_double(1.1, 2.3) == 1);

	AST::IntConstN::Ptr x;
	AST::IntConstN::Ptr y;
	AST::IntConstN::Ptr z;
	AST::IntConstN::Ptr res;
	Passes::Transformations::logical_and<AST::IntConstN::Ptr> logical_and_intconstn;
	x = std::make_shared<AST::IntConstN>(10, -1, true, 0x11223344);
	y = std::make_shared<AST::IntConstN>(10, 32, true, 0x55667788);
	z = std::make_shared<AST::IntConstN>(10, 32, true, 0);
	res = logical_and_intconstn(x, y);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(1), res->get_value());
	res = logical_and_intconstn(x, z);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(0), res->get_value());

	AST::FloatConst::Ptr xf;
	AST::FloatConst::Ptr yf;
	AST::FloatConst::Ptr zf;
	AST::FloatConst::Ptr resf;
	Passes::Transformations::logical_and<AST::FloatConst::Ptr> logical_and_floatconst;
	xf = std::make_shared<AST::FloatConst>(-1);
	yf = std::make_shared<AST::FloatConst>(2);
	zf = std::make_shared<AST::FloatConst>(0);
	resf = logical_and_floatconst(xf, yf);
	ASSERT_EQ(true, resf->get_value());
	resf = logical_and_floatconst(xf, zf);
	ASSERT_EQ(false, resf->get_value());
}

TEST(PassesTransformation_ExpressionOperators, BinaryLogicalOr) {
	Passes::Transformations::logical_or<uint32_t> logical_or;
	ASSERT_TRUE(logical_or(0, 0) == 0);
	ASSERT_TRUE(logical_or(0xaa55fff0, 0) == 1);
	ASSERT_TRUE(logical_or(0xaa55fff0, 2) == 1);

	Passes::Transformations::logical_or<double> logical_or_double;
	ASSERT_TRUE(logical_or_double(0, 0) == 0);
	ASSERT_TRUE(logical_or_double(1.2, 0) == 1);
	ASSERT_TRUE(logical_or_double(0, 1.5) == 1);

	AST::IntConstN::Ptr x;
	AST::IntConstN::Ptr y;
	AST::IntConstN::Ptr z;
	AST::IntConstN::Ptr res;
	Passes::Transformations::logical_or<AST::IntConstN::Ptr> logical_or_intconstn;
	x = std::make_shared<AST::IntConstN>(10, -1, true, 0x11223344);
	y = std::make_shared<AST::IntConstN>(10, 32, true, 0);
	z = std::make_shared<AST::IntConstN>(10, 32, true, 0);
	res = logical_or_intconstn(x, y);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(1), res->get_value());
	res = logical_or_intconstn(y, z);
	ASSERT_EQ(2, res->get_base());
	ASSERT_EQ(1, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(0), res->get_value());

	AST::FloatConst::Ptr xf;
	AST::FloatConst::Ptr yf;
	AST::FloatConst::Ptr zf;
	AST::FloatConst::Ptr resf;
	Passes::Transformations::logical_or<AST::FloatConst::Ptr> logical_or_floatconst;
	xf = std::make_shared<AST::FloatConst>(-1);
	yf = std::make_shared<AST::FloatConst>(0);
	zf = std::make_shared<AST::FloatConst>(0);
	resf = logical_or_floatconst(xf, yf);
	ASSERT_EQ(true, resf->get_value());
	resf = logical_or_floatconst(yf, zf);
	ASSERT_EQ(false, resf->get_value());
}

TEST(PassesTransformation_ExpressionOperators, Ternary) {

	Passes::Transformations::ternary<uint32_t, uint32_t> ternary_op;
	ASSERT_TRUE(ternary_op(0, 0, 1) == 1);
	ASSERT_TRUE(ternary_op(0, 0, 0xaa55fff0) == 0xaa55fff0);
	ASSERT_TRUE(ternary_op(1, 2, 0xaa55fff0) == 2);

	Passes::Transformations::ternary<uint32_t, double> ternary_op_uint32_t_double;
	ASSERT_TRUE(ternary_op_uint32_t_double(0, 0, 3.2) == 3.2);
	ASSERT_TRUE(ternary_op_uint32_t_double(1, 0, 3.2) == 0);

	Passes::Transformations::ternary<double, double> ternary_op_double_double;
	ASSERT_TRUE(ternary_op_double_double(0, 0, 3.2) == 3.2);
	ASSERT_TRUE(ternary_op_double_double(1.0, 0.5, 3.2) == 0.5);

	AST::IntConstN::Ptr x;
	AST::IntConstN::Ptr y;
	AST::IntConstN::Ptr z;
	AST::IntConstN::Ptr res;
	AST::FloatConst::Ptr xf;
	AST::FloatConst::Ptr yf;
	AST::FloatConst::Ptr zf;
	AST::FloatConst::Ptr resf;
	Passes::Transformations::ternary<AST::IntConstN::Ptr, AST::Node::Ptr> ternary_op_intconstn_node;
	Passes::Transformations::ternary<AST::IntConstN::Ptr, AST::IntConstN::Ptr> ternary_op_intconstn_intconstn;
	Passes::Transformations::ternary<AST::IntConstN::Ptr, AST::FloatConst::Ptr> ternary_op_intconstn_floatconst;
	Passes::Transformations::ternary<AST::FloatConst::Ptr, AST::IntConstN::Ptr> ternary_op_floatconst_intconstn;
	Passes::Transformations::ternary<AST::FloatConst::Ptr, AST::FloatConst::Ptr> ternary_op_floatconst_floatconst;
	Passes::Transformations::ternary<AST::FloatConst::Ptr, AST::Node::Ptr> ternary_op_floatconst_node;
	x = std::make_shared<AST::IntConstN>(10, -1, true, 0x11223344, "x");
	y = std::make_shared<AST::IntConstN>(10, 32, true, 0, "y");
	z = std::make_shared<AST::IntConstN>(10, 32, true, -2, "z");
	xf = std::make_shared<AST::FloatConst>(11223344, "xf");
	yf = std::make_shared<AST::FloatConst>(0, "yf");
	zf = std::make_shared<AST::FloatConst>(-2, "zf");

	res = ternary_op_intconstn_intconstn(x, y, z);
	ASSERT_EQ(*y, *res);
	res = ternary_op_intconstn_intconstn(y, x, z);
	ASSERT_EQ(*z, *res);

	res = ternary_op_floatconst_intconstn(xf, y, z);
	ASSERT_EQ(*y, *res);
	res = ternary_op_floatconst_intconstn(yf, x, z);
	ASSERT_EQ(*z, *res);

	resf = ternary_op_intconstn_floatconst(x, yf, zf);
	ASSERT_EQ(*yf, *resf);
	resf = ternary_op_intconstn_floatconst(y, xf, zf);
	ASSERT_EQ(*zf, *resf);

	resf = ternary_op_floatconst_floatconst(xf, yf, zf);
	ASSERT_EQ(*yf, *resf);
	resf = ternary_op_floatconst_floatconst(yf, xf, zf);
	ASSERT_EQ(*zf, *resf);

	res = AST::cast_to<AST::IntConstN>(ternary_op_intconstn_node(x, AST::to_node(y), AST::to_node(z)));
	ASSERT_EQ(*y, *res);
	res = AST::cast_to<AST::IntConstN>(ternary_op_intconstn_node(y, AST::to_node(x), AST::to_node(z)));
	ASSERT_EQ(*z, *res);

	resf = AST::cast_to<AST::FloatConst>(ternary_op_floatconst_node(xf, AST::to_node(yf), AST::to_node(zf)));
	ASSERT_EQ(*yf, *resf);
	resf = AST::cast_to<AST::FloatConst>(ternary_op_floatconst_node(yf, AST::to_node(xf), AST::to_node(zf)));
	ASSERT_EQ(*zf, *resf);
}

TEST(PassesTransformation_ExpressionOperators, Parselect) {
	Passes::Transformations::partselect<uint32_t> partselect;
	ASSERT_TRUE(partselect(0x89ABCDEF, 31, 24) == 0x89);
	ASSERT_TRUE(partselect(0x89ABCDEF, 7, 0) == 0xEF);
	ASSERT_TRUE(partselect(0x89ABCDEF, 15, 8) == 0xCD);

	AST::IntConstN::Ptr x;
	AST::IntConstN::Ptr msb;
	AST::IntConstN::Ptr lsb;
	AST::IntConstN::Ptr res;

	Passes::Transformations::partselect<AST::IntConstN::Ptr> partselect_intconstn;
	x = std::make_shared<AST::IntConstN>(10, -1, true, 0x89ABCDEF, "x");
	msb = std::make_shared<AST::IntConstN>(10, -1, true, 31, "msb");
	lsb = std::make_shared<AST::IntConstN>(10, -1, true, 24, "lsb");
	res = partselect_intconstn(x, msb, lsb);
	ASSERT_EQ(10, res->get_base());
	ASSERT_EQ(8, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(0x89), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, -1, true, 0x89ABCDEF, "x");
	msb = std::make_shared<AST::IntConstN>(10, -1, true, 7, "msb");
	lsb = std::make_shared<AST::IntConstN>(10, -1, true, 0, "lsb");
	res = partselect_intconstn(x, msb, lsb);
	ASSERT_EQ(10, res->get_base());
	ASSERT_EQ(8, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(0xEF), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, -1, true, 0x89ABCDEF, "x");
	msb = std::make_shared<AST::IntConstN>(10, -1, true, 15, "msb");
	lsb = std::make_shared<AST::IntConstN>(10, -1, true, 8, "lsb");
	res = partselect_intconstn(x, msb, lsb);
	ASSERT_EQ(10, res->get_base());
	ASSERT_EQ(8, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(0xCD), res->get_value());
}

TEST(PassesTransformation_ExpressionOperators, Parselect_minus) {
	Passes::Transformations::partselect_minus<uint32_t> partselect_minus;
	ASSERT_TRUE(partselect_minus(0x89ABCDEF, 31, 8) == 0x89);
	ASSERT_TRUE(partselect_minus(0x89ABCDEF, 7, 8) == 0xEF);
	ASSERT_TRUE(partselect_minus(0x89ABCDEF, 15, 8) == 0xCD);

	AST::IntConstN::Ptr x;
	AST::IntConstN::Ptr msb;
	AST::IntConstN::Ptr index;
	AST::IntConstN::Ptr res;

	Passes::Transformations::partselect_minus<AST::IntConstN::Ptr> partselect_minus_intconstn;
	x = std::make_shared<AST::IntConstN>(10, -1, true, 0x89ABCDEF, "x");
	msb = std::make_shared<AST::IntConstN>(10, -1, true, 31, "msb");
	index = std::make_shared<AST::IntConstN>(10, -1, true, 8, "index");
	res = partselect_minus_intconstn(x, msb, index);
	ASSERT_EQ(10, res->get_base());
	ASSERT_EQ(8, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(0x89), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, -1, true, 0x89ABCDEF, "x");
	msb = std::make_shared<AST::IntConstN>(10, -1, true, 7, "msb");
	index = std::make_shared<AST::IntConstN>(10, -1, true, 8, "index");
	res = partselect_minus_intconstn(x, msb, index);
	ASSERT_EQ(10, res->get_base());
	ASSERT_EQ(8, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(0xEF), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, -1, true, 0x89ABCDEF, "x");
	msb = std::make_shared<AST::IntConstN>(10, -1, true, 15, "msb");
	index = std::make_shared<AST::IntConstN>(10, -1, true, 8, "index");
	res = partselect_minus_intconstn(x, msb, index);
	ASSERT_EQ(10, res->get_base());
	ASSERT_EQ(8, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(0xCD), res->get_value());
}

TEST(PassesTransformation_ExpressionOperators, Parselect_plus) {
	Passes::Transformations::partselect_plus<uint32_t> partselect_plus;
	ASSERT_TRUE(partselect_plus(0x89ABCDEF, 24, 8) == 0x89);

	AST::IntConstN::Ptr x;
	AST::IntConstN::Ptr lsb;
	AST::IntConstN::Ptr index;
	AST::IntConstN::Ptr res;

	Passes::Transformations::partselect_plus<AST::IntConstN::Ptr> partselect_plus_intconstn;
	x = std::make_shared<AST::IntConstN>(10, -1, true, 0x89ABCDEF, "x");
	lsb = std::make_shared<AST::IntConstN>(10, -1, true, 24, "lsb");
	index = std::make_shared<AST::IntConstN>(10, -1, true, 8, "index");
	res = partselect_plus_intconstn(x, lsb, index);
	ASSERT_EQ(10, res->get_base());
	ASSERT_EQ(8, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(0x89), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, -1, true, 0x89ABCDEF, "x");
	lsb = std::make_shared<AST::IntConstN>(10, -1, true, 0, "lsb");
	index = std::make_shared<AST::IntConstN>(10, -1, true, 8, "index");
	res = partselect_plus_intconstn(x, lsb, index);
	ASSERT_EQ(10, res->get_base());
	ASSERT_EQ(8, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(0xEF), res->get_value());
	x = std::make_shared<AST::IntConstN>(10, -1, true, 0x89ABCDEF, "x");
	lsb = std::make_shared<AST::IntConstN>(10, -1, true, 8, "lsb");
	index = std::make_shared<AST::IntConstN>(10, -1, true, 8, "index");
	res = partselect_plus_intconstn(x, lsb, index);
	ASSERT_EQ(10, res->get_base());
	ASSERT_EQ(8, res->get_size());
	ASSERT_EQ(false, res->get_sign());
	ASSERT_EQ(mpz_class(0xCD), res->get_value());
}
