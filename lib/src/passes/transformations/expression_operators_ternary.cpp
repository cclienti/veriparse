#include <veriparse/passes/transformations/expression_operators.hpp>
#include <veriparse/logger/logger.hpp>

#include <memory>


namespace Veriparse {
	namespace Passes {
		namespace Transformations {

			template<>
			AST::Node::Ptr ternary<AST::IntConstN::Ptr, AST::Node::Ptr>::operator()
				(const AST::IntConstN::Ptr x, const AST::Node::Ptr y, const AST::Node::Ptr z) const {
				if (x && y && z) {
					if (x->get_value() != 0) {
						return y->clone();
					}
					else {
						return z->clone();
					}
				}
				else {
					return nullptr;
				}
			}

			template<>
			AST::Node::Ptr ternary<AST::FloatConst::Ptr, AST::Node::Ptr>::operator()
				(const AST::FloatConst::Ptr x, const AST::Node::Ptr y, const AST::Node::Ptr z) const {
				if (x && y && z) {
					if (x->get_value() != 0) {
						return y->clone();
					}
					else {
						return z->clone();
					}
				}
				else {
					return nullptr;
				}
			}

			template<>
			AST::IntConstN::Ptr ternary<AST::IntConstN::Ptr, AST::IntConstN::Ptr>::operator()
				(const AST::IntConstN::Ptr x, const AST::IntConstN::Ptr y, const AST::IntConstN::Ptr z) const {
				if (x && y && z) {
					if (x->get_value() != 0) {
						return AST::cast_to<AST::IntConstN>(y->clone());
					}
					else {
						return AST::cast_to<AST::IntConstN>(z->clone());
					}
				}
				else {
					return nullptr;
				}
			}

			template<>
			AST::FloatConst::Ptr ternary<AST::IntConstN::Ptr, AST::FloatConst::Ptr>::operator()
				(const AST::IntConstN::Ptr x, const AST::FloatConst::Ptr y, const AST::FloatConst::Ptr z) const {
				if (x && y && z) {
					if (x->get_value() != 0) {
						return AST::cast_to<AST::FloatConst>(y->clone());
					}
					else {
						return AST::cast_to<AST::FloatConst>(z->clone());
					}
				}
				else {
					return nullptr;
				}
			}

			template<>
			AST::IntConstN::Ptr ternary<AST::FloatConst::Ptr, AST::IntConstN::Ptr>::operator()
				(const AST::FloatConst::Ptr x, const AST::IntConstN::Ptr y, const AST::IntConstN::Ptr z) const {
				if (x && y && z) {
					if (x->get_value() != 0) {
						return AST::cast_to<AST::IntConstN>(y->clone());
					}
					else {
						return AST::cast_to<AST::IntConstN>(z->clone());
					}
				}
				else {
					return nullptr;
				}
			}

			template<>
			AST::FloatConst::Ptr ternary<AST::FloatConst::Ptr, AST::FloatConst::Ptr>::operator()
				(const AST::FloatConst::Ptr x, const AST::FloatConst::Ptr y, const AST::FloatConst::Ptr z) const {
				if (x && y && z) {
					if (x->get_value() != 0) {
						return AST::cast_to<AST::FloatConst>(y->clone());
					}
					else {
						return AST::cast_to<AST::FloatConst>(z->clone());
					}
				}
				else {
					return nullptr;
				}
			}


		}
	}
}
