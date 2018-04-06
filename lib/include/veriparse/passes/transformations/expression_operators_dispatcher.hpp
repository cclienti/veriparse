#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_EXPRESSION_OPERATORS_DISPATCHER
#define VERIPARSE_PASSES_TRANSFORMATIONS_EXPRESSION_OPERATORS_DISPATCHER

#include <veriparse/AST/nodes.hpp>
#include <memory>

namespace Veriparse {
	namespace Passes {
		namespace Transformations {

			inline AST::FloatConst::Ptr intconstn_to_floatconst(AST::IntConstN::Ptr x) {
				AST::FloatConst::Ptr res;
				if(x) {
					res = std::make_shared<AST::FloatConst>(x->get_value().get_d(), x->get_filename(), x->get_line());
				}
				return res;
			}

			template <template<class> class UNARY_OP>
			inline AST::Node::Ptr expression_operator_unary_nofloat(AST::Node::Ptr x) {
				if (x) {
					if(x->get_node_type() == AST::NodeType::IntConstN) {
						UNARY_OP<AST::IntConstN::Ptr> op;
						return op(AST::cast_to<AST::IntConstN>(x));
					}
					else {
						LOG_WARNING << x->get_filename() << ":" << x->get_line()
						            << ": could not evaluate unary expression";
					}
				}
				return nullptr;
			}

			template <template<class> class UNARY_OP>
			inline AST::Node::Ptr expression_operator_unary(AST::Node::Ptr x) {
				if (x) {
					if(x->get_node_type() == AST::NodeType::FloatConst) {
						UNARY_OP<AST::FloatConst::Ptr> op;
						return op(AST::cast_to<AST::FloatConst>(x));
					}
					else if(x->get_node_type() == AST::NodeType::IntConstN) {
						UNARY_OP<AST::IntConstN::Ptr> op;
						return op(AST::cast_to<AST::IntConstN>(x));
					}
					else {
						LOG_WARNING << x->get_filename() << ":" << x->get_line()
						            << ": could not evaluate unary expression";
					}
				}
				return nullptr;
			}

			template <template<class> class BINARY_OP>
			inline AST::Node::Ptr expression_operator_binary_nofloat(AST::Node::Ptr x, AST::Node::Ptr y) {
				if(x && y) {
					if ((x->is_node_type(AST::NodeType::IntConstN)) && (y->is_node_type(AST::NodeType::IntConstN))) {
						BINARY_OP<AST::IntConstN::Ptr> op;
						return op(AST::cast_to<AST::IntConstN>(x), AST::cast_to<AST::IntConstN>(y));
					}
					else {
						LOG_WARNING << x->get_filename() << ":" << x->get_line()
						            << ": could not evaluate binary expression";
					}
				}
				return nullptr;
			}

			template <template<class> class BINARY_OP>
			inline AST::Node::Ptr expression_operator_binary(AST::Node::Ptr x, AST::Node::Ptr y) {
				if(x && y) {
					if ((x->is_node_type(AST::NodeType::IntConstN)) && (y->is_node_type(AST::NodeType::IntConstN))) {
						BINARY_OP<AST::IntConstN::Ptr> op;
						return op(AST::cast_to<AST::IntConstN>(x), AST::cast_to<AST::IntConstN>(y));
					}
					else if ((x->is_node_type(AST::NodeType::FloatConst)) && (y->is_node_type(AST::NodeType::IntConstN))) {
						BINARY_OP<AST::FloatConst::Ptr> op;
						AST::FloatConst::Ptr xc = AST::cast_to<AST::FloatConst>(x);
						AST::FloatConst::Ptr yc = intconstn_to_floatconst(AST::cast_to<AST::IntConstN>(y));
						return op(xc, yc);
					}
					else if ((x->is_node_type(AST::NodeType::IntConstN)) && (y->is_node_type(AST::NodeType::FloatConst))) {
						BINARY_OP<AST::FloatConst::Ptr> op;
						AST::FloatConst::Ptr xc = intconstn_to_floatconst(AST::cast_to<AST::IntConstN>(x));
						AST::FloatConst::Ptr yc = AST::cast_to<AST::FloatConst>(y);
						return op(xc, yc);
					}
					else if ((x->is_node_type(AST::NodeType::FloatConst)) && (y->is_node_type(AST::NodeType::FloatConst))) {
						BINARY_OP<AST::FloatConst::Ptr> op;
						AST::FloatConst::Ptr xc = AST::cast_to<AST::FloatConst>(x);
						AST::FloatConst::Ptr yc = AST::cast_to<AST::FloatConst>(y);
						return op(xc, yc);
					}
					else {
						LOG_WARNING << x->get_filename() << ":" << x->get_line()
						            << ": could not evaluate binary expression";
					}
				}
				return nullptr;
			}

			template <template<class, class> class TERNARY_OP>
			AST::Node::Ptr expression_operator_ternary(const AST::Node::Ptr x,
			                                           const AST::Node::Ptr y, const AST::Node::Ptr z) {
				if (x && y && z) {
					switch(x->get_node_type()) {
					case AST::NodeType::IntConstN:
						{
							TERNARY_OP<AST::IntConstN::Ptr, AST::Node::Ptr> op;
							return op(AST::cast_to<AST::IntConstN>(x), y, z);
						}

					case AST::NodeType::FloatConst:
						{
							TERNARY_OP<AST::FloatConst::Ptr, AST::Node::Ptr> op;
							return op(AST::cast_to<AST::FloatConst>(x), y, z);
						}

					default:
						LOG_WARNING << x->get_filename() << ":" << x->get_line()
						            << ": could not evaluate ternary expression";
					}
				}
				return nullptr;
			}


		}
	}
}


#endif
