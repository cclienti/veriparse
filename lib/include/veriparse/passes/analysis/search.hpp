#ifndef VERIPARSE_PASSES_ANALYSIS_SEARCH
#define VERIPARSE_PASSES_ANALYSIS_SEARCH

#include <vector>
#include <map>
#include <string>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/AST/nodes.hpp>


namespace Veriparse {
namespace Passes {
namespace Analysis {

template<AST::NodeType ...blacklists>
class Search {
public:
	Search() = delete;
	Search(const Search &search) = delete;

protected:
	template<class TProperty, class TNode, class TNodeGetter>
	static std::vector<TProperty> get_property_in_list(typename TNode::ListPtr nodes,
	                                                   TNodeGetter lambda_getter)
	{
		std::vector<std::string> names;
		if (nodes) {
			for(typename TNode::Ptr node: *nodes) {
				if(node)
					names.push_back(lambda_getter(node));
			}
		}
		return names;
	}

	/**
	 * Append in the given list all nodes that correspond to
	 * the given node type in the node AST. The AST is scanned
	 * recursively.
	 *
	 * The max_occur_depth argument allows to not go further
	 * when the max number of nested node_type is
	 * reached. When it is set to -1, there is no depth
	 * limit.
	 */
	template<class T>
	static void get_node_list(AST::Node::Ptr node, AST::NodeType node_type,
	                          typename T::ListPtr list, int64_t max_occur_depth=-1)
	{
		if (std::abs(max_occur_depth) > 0) {
			AST::NodeType current_nt = node->get_node_type();

			if (current_nt == node_type) {
				list->push_back(AST::cast_to<T>(node));
				max_occur_depth = (max_occur_depth == -1) ? max_occur_depth : max_occur_depth - 1;
			}

			if(blacklisted_nodetype<blacklists...>(current_nt)) return;

			AST::Node::ListPtr children = node->get_children();
			for (AST::Node::Ptr n: *children) {
				get_node_list<T>(n, node_type, list, max_occur_depth);
			}
		}
	}

	/**
	 * Append in the given list all nodes that correspond to
	 * the given node category type in the node AST. The AST
	 * is scanned recursively.
	 *
	 * The max_occur_depth argument allows to not go further
	 * when the max number of nested node_type is
	 * reached. When it is set to -1, there is no depth
	 * limit.
	 */
	template<class T>
	static void get_node_list_by_category(AST::Node::Ptr node, AST::NodeType node_category,
	                                      typename T::ListPtr list, int64_t max_occur_depth=-1)
	{
		if (std::abs(max_occur_depth) > 0) {
			if (node->is_node_category(node_category)) {
				list->push_back(AST::cast_to<T>(node));
				max_occur_depth = (max_occur_depth == -1) ? max_occur_depth : max_occur_depth - 1;
			}

			AST::NodeType current_nt = node->get_node_type();

			if(blacklisted_nodetype<blacklists...>(current_nt)) return;

			AST::Node::ListPtr children = node->get_children();
			for (AST::Node::Ptr n: *children) {
				get_node_list_by_category<T>(n, node_category, list, max_occur_depth);
			}
		}
	}

private:
	static bool blacklisted_nodetype(AST::NodeType current)
	{
		return false;
	}

	template<AST::NodeType nt0>
	static bool blacklisted_nodetype(AST::NodeType current)
	{
		return (current == nt0);
	}

	template<AST::NodeType nt0, AST::NodeType nt1, AST::NodeType ...nt_list>
	static inline bool blacklisted_nodetype(AST::NodeType current)
	{
		bool rec = blacklisted_nodetype<nt1, nt_list ...>(current);
		bool cur = (current == nt0);
		return rec || cur;
	}

};


typedef Search<AST::NodeType::Function, AST::NodeType::Task> StandardSearch;

}
}
}

#endif
