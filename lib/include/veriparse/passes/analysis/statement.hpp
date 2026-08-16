// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_PASSES_ANALYSIS_STATEMENT
#define VERIPARSE_PASSES_ANALYSIS_STATEMENT

#include <veriparse/AST/nodes.hpp>
#include <set>
#include <string>

namespace Veriparse
{
namespace Passes
{
namespace Analysis
{

/// Statement-tree queries: what a subtree of procedural statements drives,
/// reads, declares and calls. The node-scoped analyses (Lvalue, Call,
/// Identifier) answer for one node; these recurse over whole statement
/// trees.
struct Statement
{
    /// Every identifier name in the tree.
    static void collect_identifier_names(const AST::Node::Ptr &node, std::set<std::string> &names);

    /// The base registers a left-hand side drives: through selects and
    /// concatenations down to the named variables, index expressions
    /// excluded. A hierarchical write targets another scope, not this
    /// module's register of the same leaf name.
    static void collect_lvalue_bases(const AST::Node::Ptr &node, std::set<std::string> &names);

    /// Every register the tree drives — procedural or continuous —
    /// harvested at the assignment nodes, their right-hand sides alone.
    static void collect_driven(const AST::Node::Ptr &node, std::set<std::string> &names);

    /// The names the tree reads: assignment right-hand sides, branch and
    /// case conditions — assignment targets excluded.
    static void collect_reads(const AST::Node::Ptr &node, std::set<std::string> &reads);

    /// The subroutine names the tree calls in statement position.
    static void collect_call_names(const AST::Node::Ptr &node, std::set<std::string> &names);

    /// Every declaration name bound anywhere in the tree.
    static void collect_declaration_names(const AST::Node::Ptr &node, std::set<std::string> &names);

    /// The canonical key of an identifier: its hierarchical labels dotted
    /// before the name (`bus.ack`), a constant label select rendered by
    /// value and a non-constant one as `[?]` — so distinct paths never
    /// share a key with each other or with a plain name.
    static std::string identifier_key(const AST::Identifier::Ptr &id);

    /// The plain-identifier target of an assignment's left-hand side, or
    /// empty when the shape is outside the subset.
    static std::string lvalue_target(const AST::Lvalue::Ptr &lvalue);

    /// The target register of a nonblocking assignment.
    static std::string nba_target(const AST::NonblockingSubstitution::Ptr &nba);

    /// Program-order verdict on the first reference to `name`: -1 read
    /// first, +1 written first, 0 absent. Within one assignment the order
    /// is right-hand side, then commit, so a self-read counts as a read;
    /// any occurrence outside an assignment — a condition, an event
    /// control, a bound, an actual — can only read.
    static int first_reference(const AST::Node::Ptr &node, const std::string &name);
};

} // namespace Analysis
} // namespace Passes
} // namespace Veriparse

#endif
