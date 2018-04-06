#ifndef VERIPARSE_AST_NODE_OSTREAM_HPP
#define VERIPARSE_AST_NODE_OSTREAM_HPP

#include <veriparse/AST/node.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		std::ostream & operator<<(std::ostream &os, const Node &p);

		std::ostream & operator<<(std::ostream &os, const typename Node::Ptr p);

	}
}

#endif