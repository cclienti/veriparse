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

std::atomic<std::uint64_t> g_tree_node_id{0};


template<typename T>
class TreeNode
{
public:
	using Ptr = std::unique_ptr<TreeNode<T>>;
	using Children = std::vector<Ptr>;

public:
	TreeNode(const T &value): m_value(value) {}

	virtual ~TreeNode() {};

	bool is_leaf() const {return m_children.empty();}

	const T &get_value() const {return m_value;}
	void set_value(const T& value) {m_value = value;}

	const Children &get_children() const {return m_children;}

	void push_child(Ptr node)
	{
		m_children.push_back(std::move(node));
	}

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
		auto id = g_tree_node_id.load();

		// Vertice
		dot << "\tn" << id << " [label=" << m_value << "];\n";

		// Edges
		for (const auto &child: m_children) {
			g_tree_node_id++;
			dot << "\tn" << id << " -> " << "n" << g_tree_node_id << "\n";
			child->to_dot_recurse(dot);
		}
	}


private:
	T m_value;
	Children m_children;
};


}
}

#endif
