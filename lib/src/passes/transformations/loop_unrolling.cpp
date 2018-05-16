#include <veriparse/passes/transformations/loop_unrolling.hpp>
#include <veriparse/passes/transformations/ast_replace.hpp>
#include <veriparse/passes/transformations/annotate_declaration.hpp>
#include <veriparse/passes/transformations/expression_evaluation.hpp>
#include <veriparse/generators/verilog_generator.hpp>
#include <veriparse/logger/logger.hpp>


namespace Veriparse
{
	namespace Passes
	{
		namespace Transformations
		{
			namespace
			{
				typedef std::pair<std::string, std::vector<AST::Node::Ptr>> range_t;

				std::string print_for_range_t(const range_t &for_range)
				{
					std::stringstream ss;
					ss << "{name: " << for_range.first << ", range: [";
					for(uint64_t i=0; i<for_range.second.size(); ++i) {
						if((i<4) || (i==for_range.second.size()-1)) {
							AST::Node::Ptr v = for_range.second[i];
							if(v->is_node_type(AST::NodeType::FloatConst)) {
								ss << AST::cast_to<AST::FloatConst>(v)->get_value();
							}
							if(v->is_node_type(AST::NodeType::IntConstN)) {
								ss << AST::cast_to<AST::IntConstN>(v)->get_value();
							}
							if(i < for_range.second.size()-1) {
								ss << ", ";
							}
						}
						else if(i==4) {
							ss << "... ";
						}
					}
					ss << "]}";

					return ss.str();
				}
			}

			int LoopUnrolling::process(AST::Node::Ptr node, AST::Node::Ptr parent)
			{
				int ret = 0;

				if (node) {
					if(node->is_node_type(AST::NodeType::ForStatement)) {
						const AST::ForStatement::Ptr for_node = AST::cast_to<AST::ForStatement>(node);

						if(!parent) {
							LOG_ERROR_N(for_node) << "could not unroll loop, no parent";
							return 1;
						}

						// Look for the current for loop range
						LoopUnrolling::range_ptr_t for_range = LoopUnrolling::get_for_range(for_node);
						if (!for_range) {
							return 1;
						}
						LOG_TRACE_N(for_node) << print_for_range_t(*for_range);

						// Unroll the statements
						AST::Node::ListPtr stmts;
						if(for_node->get_statement()->is_node_type(AST::NodeType::Block)) {
							stmts = AST::cast_to<AST::Block>(for_node->get_statement())->get_statements();
						}
						else {
							stmts = std::make_shared<AST::Node::List>();
							stmts->push_back(for_node->get_statement());
						}

						// Replace the loop identifier in unrolled statements and annotate declarations
						AST::Node::ListPtr unrolled_stmts = std::make_shared<AST::Node::List>();
						AnnotateDeclaration annotate;

						for(AST::Node::Ptr p: for_range->second) {
							AST::Node::ListPtr stmts_copy = AST::Node::clone_list(stmts);
							ASTReplace::replace_identifier(stmts_copy, for_range->first, p);

							// We put the stmts vector in a AST::Block in order to
							// annotate all stmts at a time.
							std::ostringstream ss_replace;
							ss_replace << "$&_" << for_range->first << "_" << Generators::VerilogGenerator().render(p);
							annotate.set_search_replace("^.*$", ss_replace.str());
							AST::Block::Ptr block = std::make_shared<AST::Block>(stmts_copy, "");
							annotate.run(block);

							unrolled_stmts->splice(unrolled_stmts->end(), *stmts_copy);
						}

						// Replace the unrolled statements in the parent block
						if (parent->is_node_type(AST::NodeType::Block)) {
							parent->replace(node, unrolled_stmts);
						}
						else {
							// if the node to replace is already in a list,
							// we can directly replace the node by our
							// list. There is no need to create a block.
							bool node_in_list = parent->replace(node, unrolled_stmts);

							// We create a block to store our list.
							if (!node_in_list) {
								AST::Block::Ptr block = std::make_shared<AST::Block>(unrolled_stmts, "");
								parent->replace(node, block);
								parent = block;
							}
						}

						// Finally search for another nested for statement in the unrolled ones.
						for (AST::Node::Ptr child: *unrolled_stmts) {
							ret += process(child, parent);
						}
					}

					else if(node->is_node_type(AST::NodeType::RepeatStatement)) {
						const AST::RepeatStatement::Ptr repeat_node = AST::cast_to<AST::RepeatStatement>(node);
						AST::Node::Ptr times_node = ExpressionEvaluation().evaluate_node(repeat_node->get_times());
						if(times_node) {
							if(times_node->is_node_type(AST::NodeType::IntConstN)) {
								AST::IntConstN::Ptr times = AST::cast_to<AST::IntConstN>(times_node);

								// Unroll the statements
								AST::Node::ListPtr stmts;
								if(repeat_node->get_statement()->is_node_type(AST::NodeType::Block)) {
									stmts = AST::cast_to<AST::Block>(repeat_node->get_statement())->get_statements();
								}
								else {
									stmts = std::make_shared<AST::Node::List>();
									stmts->push_back(repeat_node->get_statement());
								}

								AST::Node::ListPtr unrolled_stmts = std::make_shared<AST::Node::List>();
								for(uint64_t x=0; x<times->get_value().get_ui(); x++) {
									AST::Node::ListPtr stmts_copy = AST::Node::clone_list(stmts);
									unrolled_stmts->splice(unrolled_stmts->end(), *stmts_copy);
								}

								// Replace the unrolled statements in the parent block
								if (parent->is_node_type(AST::NodeType::Block)) {
									parent->replace(node, unrolled_stmts);
								}
								else {
									// if the node to replace is already in a list,
									// we can directly replace the node by our
									// list. There is no need to create a block.
									bool node_in_list = parent->replace(node, unrolled_stmts);

									// We create a block to store our list.
									if (!node_in_list) {
										AST::Block::Ptr block = std::make_shared<AST::Block>(unrolled_stmts, "");
										parent->replace(node, block);
										parent = block;
									}
								}

								// Finally search for another nested for statement in the unrolled ones.
								for (AST::Node::Ptr child: *unrolled_stmts) {
									ret += process(child, parent);
								}
							}
							else {
								LOG_WARNING_N(node) << "non integer repeat";
							}
						}
					}

					else {
						AST::Node::ListPtr children = node->get_children();
						for (AST::Node::Ptr child: *children) {
							ret += process(child, node);
						}
					}
				}

				return ret;
			}

			LoopUnrolling::range_ptr_t LoopUnrolling::get_for_range(const AST::ForStatement::Ptr for_node)
			{
				const AST::BlockingSubstitution::Ptr pre_subst = for_node->get_pre();
				if(!pre_subst) {
					LOG_WARNING_N(for_node) << "No pre-condition found, could not unroll loop";
					return nullptr;
				}

				const AST::BlockingSubstitution::Ptr post_subst = for_node->get_post();
				if(!post_subst) {
					LOG_WARNING_N(for_node) << "No post-condition found, could not unroll loop";
					return nullptr;
				}

				AST::Node::Ptr loop_cond = for_node->get_cond();
				if(!loop_cond) {
					LOG_WARNING_N(for_node) << "No condition found, could not unroll loop";
					return nullptr;
				}

				const std::string loop_pre_lvalue = LoopUnrolling::get_cond_lvalue(pre_subst);
				if(!loop_pre_lvalue.size()) {
					return nullptr;
				}

				AST::Node::Ptr loop_pre_rvalue = LoopUnrolling::get_cond_rvalue(pre_subst);
				if(!loop_pre_rvalue) {
					return nullptr;
				}
				if(!loop_pre_rvalue->is_node_category(AST::NodeType::Constant)) {
					LOG_WARNING_N(for_node) << "pre-condition rvalue is not constant, could not unroll loop";
					return nullptr;
				}

				const std::string loop_post_lvalue = LoopUnrolling::get_cond_lvalue(post_subst);
				if(!loop_post_lvalue.size()) {
					return nullptr;
				}

				if (loop_post_lvalue != loop_pre_lvalue) {
					LOG_WARNING_N(for_node) << "pre-condition and post-condition lvalues are different, "
					                        << "could not unroll loop";
					return nullptr;
				}

				AST::Node::Ptr loop_post_rvalue = LoopUnrolling::get_cond_rvalue(post_subst);
				if(!loop_post_rvalue) {
					return nullptr;
				}

				ExpressionEvaluation::replace_map_t replace_map;
				replace_map[loop_pre_lvalue] = AST::cast_to<AST::Constant>(loop_pre_rvalue);

				range_ptr_t range = std::make_shared<range_t>();
				range->first = loop_pre_lvalue;

				while(true) {
					AST::Node::Ptr cond_eval = ExpressionEvaluation(replace_map).evaluate_node(loop_cond);
					if(!cond_eval) {
						LOG_WARNING_N(for_node) << "could not evaluate condition, loop not unrolled";
						return nullptr;
					}

					bool cond;

					if(cond_eval->is_node_type(AST::NodeType::FloatConst)) {
						AST::FloatConst::Ptr fconst = AST::cast_to<AST::FloatConst>(cond_eval);
						cond = fconst->get_value() != 0;
					}
					else if (cond_eval->is_node_type(AST::NodeType::IntConstN)) {
						AST::IntConstN::Ptr iconst = AST::cast_to<AST::IntConstN>(cond_eval);
						cond = iconst->get_value() != 0;
					}
					else {
						LOG_WARNING_N(for_node) << "could not evaluate condition, loop not unrolled";
						return nullptr;
					}

					if(cond) {
						range->second.push_back(replace_map[loop_pre_lvalue]);
						AST::Node::Ptr eval = ExpressionEvaluation(replace_map).evaluate_node(loop_post_rvalue);
						if(!eval) {
							LOG_WARNING_N(for_node) << "could not evaluate post-condition, loop not unrolled";
							return nullptr;
						}
						if(eval->is_node_category(AST::NodeType::Constant)) {
							replace_map[loop_pre_lvalue] = AST::cast_to<AST::Constant>(eval);
						}
						else {
							LOG_WARNING_N(for_node) << "post-condition evaluation is not constant, loop not unrolled";
							return nullptr;
						}
					}
					else {
						break;
					}

				}
				return range;
			}

			std::string LoopUnrolling::get_cond_lvalue(const AST::BlockingSubstitution::Ptr subst)
			{
				const AST::Lvalue::Ptr lvalue = subst->get_left();
				if(!lvalue) {
					return "";
				}

				AST::Node::Ptr identifier = AST::cast_to<AST::Lvalue>(lvalue)->get_var();
				if(!identifier) {
					return "";
				}
				if(!identifier->is_node_category(AST::NodeType::Identifier)) {
					return "";
				}

				return AST::cast_to<AST::Identifier>(identifier)->get_name();
			}

			AST::Node::Ptr LoopUnrolling::get_cond_rvalue(const AST::BlockingSubstitution::Ptr subst)
			{
				const AST::Rvalue::Ptr rvalue = subst->get_right();
				if(!rvalue) {
					return nullptr;
				}

				return rvalue->get_var();
			}

			AST::Node::Ptr LoopUnrolling::get_cond_rvalue(const AST::BlockingSubstitution::Ptr subst,
			                                              const ExpressionEvaluation::replace_map_t &map)
			{
				const AST::Rvalue::Ptr rvalue = subst->get_right();
				if(!rvalue) {
					return nullptr;
				}

				return ExpressionEvaluation(map).evaluate_node(rvalue->get_var());
			}

		}
	}
}
