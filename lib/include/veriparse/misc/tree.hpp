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
	virtual Ptr clone() const
	{
		auto node = make_ptr(get_value());

		for (const auto &child: m_children) {
			node->push_child(std::move(child->clone()));
		}

		return node;
	}

	/**
	 * @brief Return true if the node is a leaf.
	 */
	virtual bool is_leaf() const {return m_children.empty();}

	/**
	 * @brief Value getter.
	 */
	virtual const T &get_value() const {return m_value;}

	/**
	 * @brief Value setter.
	 */
	virtual void set_value(const T& value) {m_value = value;}

	/**
	 * @brief Return the children of the current node.
	 */
	virtual const Children &get_children() const {return m_children;}

	/**
	 * @brief Return the children of the current node.
	 */
	virtual Children &get_children() {return m_children;}

	/**
	 * @brief Move a node into the children list.
	 */
	virtual void push_child(Ptr node)
	{
		m_children.push_back(std::move(node));
	}

	/**
	 * @brief Generate a dot graph from the current node to the
	 * children recursively.
	 */
	virtual std::string to_dot() const
	{
		std::stringstream dot;

		dot << "digraph G {\n"
		    << "\trankdir=LR;\n";

		to_dot_recurse(dot);

		dot << "}\n";

		return dot.str();
	}

protected:
	/**
	 * @brief Change the tree node id static class member
	 */
	static void set_tree_node_id(std::uint64_t value=0)
	{
		s_tree_node_id = value;
	}

private:
	/**
	 * @brief Pure virtual method to print the node value
	 */
	virtual std::string print_value() const = 0;

	/**
	 * @brief Pure virtual method that instantiates the right
	 * unique_ptr.
	 */
	virtual Ptr make_ptr(const T &value) const = 0;

	/**
	 * @brief Print recursively
	 */
	virtual void to_dot_recurse(std::stringstream &dot) const
	{
		auto id = s_tree_node_id.load();

		// Vertice
		dot << "\tn" << id << " [label=\"" << print_value() << "\"];\n";

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
