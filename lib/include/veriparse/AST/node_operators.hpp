// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_AST_NODE_OPERATORS
#define VERIPARSE_AST_NODE_OPERATORS

#include <veriparse/AST/node_type.hpp>
#include <cstdint>


namespace Veriparse {
	namespace AST {

		struct NodeOperators {
			static const char *get_char(NodeType node_type);
			static int get_priority(NodeType node_type);
		};

	}
}

#endif
