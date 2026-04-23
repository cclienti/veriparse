// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/nodes.hpp>
#include <iostream>

{% for class_name in class_list %}
std::ostream& operator<<(std::ostream& stream, const Veriparse::AST::{{ class_name }} &node) {
	stream << node.to_string();
	return stream;
}
{% endfor %}
