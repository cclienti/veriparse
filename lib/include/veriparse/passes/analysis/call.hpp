// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_PASSES_ANALYSIS_CALL
#define VERIPARSE_PASSES_ANALYSIS_CALL

#include <vector>
#include <string>

#include <veriparse/AST/node_cast.hpp>
#include <veriparse/AST/nodes.hpp>

#include <veriparse/passes/analysis/search.hpp>

namespace Veriparse
{
namespace Passes
{
namespace Analysis
{

class Call : public StandardSearch
{
public:
    Call() = delete;

    /**
     * Return identifier nodes appearing in a subroutine call's arguments (any
     * Call-category node: neutral Call, FunctionCall, TaskCall).
     */
    static AST::Identifier::ListPtr get_argument_identifier_nodes(AST::Call::Ptr call);

    /**
     * Return identifier names appearing in a subroutine call's arguments.
     */
    static std::vector<std::string> get_argument_identifier_names(AST::Call::Ptr call);
};

} // namespace Analysis
} // namespace Passes
} // namespace Veriparse

#endif
