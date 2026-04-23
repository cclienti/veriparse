// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_AST_NODES_HPP
#define VERIPARSE_AST_NODES_HPP

#include <veriparse/AST/node.hpp>
{%- for header_name in header_list %}
#include <veriparse/AST/{{ header_name.lower() }}.hpp>
{%- endfor %}
#include <veriparse/AST/node_operators.hpp>
#include <veriparse/AST/node_cast.hpp>

#endif
