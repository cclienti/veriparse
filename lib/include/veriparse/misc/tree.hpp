#ifndef VERIPARSE_MISC_TREE_HPP
#define VERIPARSE_MISC_TREE_HPP

#include <memory>
#include <vector>
#include <sstream>
#include <string>
#include <cstdint>
#include <atomic>


namespace Veriparse
{
namespace Misc
{


template<typename T>
class TreeNode
{
public:
	using Ptr = std::unique_ptr<TreeNode<T>>;
	using Children = std::vector<Ptr>;

public:
	TreeNode(const T &value): m_value(value) {}
	virtual ~TreeNode() {}

	/**
	 * @brief Clone recursively the tree
	 */
	Ptr clone() const
	{
		auto node = std::make_unique<TreeNode>(get_value());

		for (const auto &child: m_children) {
			node->push_child(std::move(child->clone()));
		}

		return node;
	}

	/**
	 * @brief Return true if the node is a leaf.
	 */
	bool is_leaf() const {return m_children.empty();}

	/**
	 * @brief Value getter.
	 */
	const T &get_value() const {return m_value;}

	/**
	 * @brief Value setter.
	 */
	void set_value(const T& value) {m_value = value;}

	/**
	 * @brief Return the children of the current node.
	 */
	const Children &get_children() const {return m_children;}

	/**
	 * @brief Move a node into the children list.
	 */
	void push_child(Ptr node)
	{
		m_children.push_back(std::move(node));
	}

	/**
	 * @brief Generate a dot graph from the current node to the
	 * children recursively.
	 */
	std::string to_dot()
	{
		std::stringstream dot;

		dot << "digraph G {\n"
		    << "\trankdir=LR;\n";

		to_dot_recurse(dot);

		dot << "}\n";

		return dot.str();
	}

private:
	void to_dot_recurse(std::stringstream &dot)
	{
		auto id = s_tree_node_id.load();

		// Vertice
		dot << "\tn" << id << " [label=" << m_value << "];\n";

		// Edges
		for (const auto &child: m_children) {
			s_tree_node_id++;
			dot << "\tn" << id << " -> " << "n" << s_tree_node_id << "\n";
			child->to_dot_recurse(dot);
		}
	}


private:
	static std::atomic<std::uint64_t> s_tree_node_id;
	T m_value;
	Children m_children;
};

template<typename T> std::atomic<std::uint64_t> TreeNode<T>::s_tree_node_id{0};

}
}

#endif
