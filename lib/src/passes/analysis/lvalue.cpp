#include <veriparse/passes/analysis/lvalue.hpp>
#include <veriparse/logger/logger.hpp>
#include <algorithm>
#include <set>


namespace Veriparse {
	namespace Passes {
		namespace Analysis {

			AST::Identifier::ListPtr Lvalue::get_identifier_nodes(AST::Lvalue::Ptr lvalue)
			{
				AST::Identifier::ListPtr list = std::make_shared<AST::Identifier::List>();
				get_node_list<AST::Identifier>(lvalue, AST::NodeType::Identifier, list);
				return list;
			}

			std::vector<std::string> Lvalue::get_identifier_names(AST::Lvalue::Ptr lvalue)
			{
				AST::Identifier::ListPtr identifiers = get_identifier_nodes(lvalue);
				return get_property_in_list<std::string, AST::Identifier>
					(identifiers, [](AST::Identifier::Ptr n){return n->get_name();});
			}

			AST::Identifier::ListPtr Lvalue::get_lvalue_nodes(AST::Lvalue::Ptr lvalue)
			{
				AST::Identifier::ListPtr identifiers = get_identifier_nodes(lvalue);
				AST::Identifier::ListPtr rvalues = get_rvalue_nodes(lvalue);
				AST::Identifier::ListPtr list = std::make_shared<AST::Identifier::List>();

				// Create a new list where rvalues found in identifiers are removed.
				using TID = AST::Identifier::Ptr;
				using TCMP = std::function<bool(const TID, const TID)>;

				TCMP cmp = [](const TID &id0, const TID &id1) {return (id0->get_name() < id1->get_name());};
				std::set<AST::Identifier::Ptr, TCMP> rvalues_set(rvalues->begin(), rvalues->end(), cmp);

				for(AST::Identifier::Ptr id: *identifiers) {
					if(rvalues_set.count(id) == 0) list->push_back(id);
				}

				return list;
			}

			std::vector<std::string> Lvalue::get_lvalue_names(AST::Lvalue::Ptr lvalue)
			{
				AST::Identifier::ListPtr identifiers = get_lvalue_nodes(lvalue);
				return get_property_in_list<std::string, AST::Identifier>
					(identifiers, [](AST::Identifier::Ptr n){return n->get_name();});
			}

			AST::Identifier::ListPtr Lvalue::get_rvalue_nodes(AST::Lvalue::Ptr lvalue)
			{
				AST::Identifier::ListPtr list = std::make_shared<AST::Identifier::List>();
				get_indirect_node_list(AST::to_node(lvalue), list);
				return list;
			}

			std::vector<std::string> Lvalue::get_rvalue_names(AST::Lvalue::Ptr lvalue)
			{
				AST::Identifier::ListPtr identifiers = get_rvalue_nodes(lvalue);
				return get_property_in_list<std::string, AST::Identifier>
					(identifiers, [](AST::Identifier::Ptr n){return n->get_name();});
			}

			void Lvalue::get_indirect_node_list(AST::Node::Ptr node, AST::Identifier::ListPtr list,
			                                    std::size_t occur_depth)
			{
				if(node) {
					switch(node->get_node_type()) {
					case AST::NodeType::Identifier:
						if(occur_depth) {
							AST::Identifier::Ptr n = AST::cast_to<AST::Identifier>(node);
							list->push_back(n);
						}
						break;

					case AST::NodeType::Pointer:
						{
							AST::Pointer::Ptr p = AST::cast_to<AST::Pointer>(node);
							AST::Node::Ptr var = p->get_var();
							if(occur_depth) {
								get_indirect_node_list(var, list, occur_depth);
							}
							else {
								if(var) {
									if (var->is_node_category(AST::NodeType::Indirect)) {
										get_indirect_node_list(var, list, occur_depth);
									}
								}
							}
							get_indirect_node_list(p->get_ptr(), list, occur_depth+1);
						}
						break;

					case AST::NodeType::Partselect:
						{
							AST::Partselect::Ptr p = AST::cast_to<AST::Partselect>(node);
							AST::Node::Ptr var = p->get_var();
							if (occur_depth) {
								get_indirect_node_list(var, list, occur_depth+1);
							}
							else {
								if(var) {
									if (var->is_node_category(AST::NodeType::Indirect)) {
										get_indirect_node_list(var, list, occur_depth);
									}
								}
							}
							get_indirect_node_list(p->get_msb(), list, occur_depth+1);
							get_indirect_node_list(p->get_lsb(), list, occur_depth+1);
						}
						break;

					case AST::NodeType::PartselectIndexed:
					case AST::NodeType::PartselectPlusIndexed:
					case AST::NodeType::PartselectMinusIndexed:
						{
							AST::PartselectIndexed::Ptr p = AST::cast_to<AST::PartselectIndexed>(node);
							AST::Node::Ptr var = p->get_var();
							if (occur_depth) {
								get_indirect_node_list(var, list, occur_depth+1);
							}
							else {
								if(var) {
									if (var->is_node_category(AST::NodeType::Indirect)) {
										get_indirect_node_list(var, list, occur_depth);
									}
								}
							}
							get_indirect_node_list(AST::to_node(p->get_index()), list, occur_depth+1);
							get_indirect_node_list(AST::to_node(p->get_size()), list, occur_depth+1);
						}
						break;

					default:
						{
							AST::Node::ListPtr children = node->get_children();
							for (AST::Node::Ptr n: *children) {
								get_indirect_node_list(n, list, occur_depth);
							}
						}
						break;
					}
				}
			}

		}
	}
}
