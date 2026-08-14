// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_PASSES_ANALYSIS_STORAGE_KIND
#define VERIPARSE_PASSES_ANALYSIS_STORAGE_KIND

#include <veriparse/AST/nodes.hpp>
#include <string>

namespace Veriparse
{
namespace Passes
{
namespace Analysis
{

/// The IEEE 1800-2017 storage-kind rules, shared by every consumer that
/// must tell a variable from a net: §23.2.2.3 decides a port's kind from
/// its direction, declaration and data type; §13.5.2 forbids passing nets
/// — or selects into nets — by reference.
struct StorageKind
{
    /// §23.2.2.3: whether the port's storage is a variable — an explicit
    /// `var` kind (the declaration is a Var), a ref port (always a
    /// variable), or an output whose data type was written with the
    /// explicit data_type syntax. An implicit data type is exactly an
    /// ImplicitType node, so the distinction survives in the AST.
    static bool port_is_variable(const AST::Port::Ptr &port);

    /// §13.5.2 net classification of `name` in `module`: true when the
    /// name is storage the module makes definitely a net — an explicit
    /// net declaration, an input/inout port without the `var` kind, or
    /// an output port whose data type is omitted or implicit. A non-ANSI
    /// header reference defers to its body declaration.
    static bool is_net(const AST::Module::Ptr &module, const std::string &name);

    /// The base identifier under a chain of selects (bit, part, indexed),
    /// or null: §13.5.2 puts "selects into nets" beside whole nets, so a
    /// select classifies by its base.
    static AST::Identifier::Ptr select_base(const AST::Node::Ptr &node);
};

} // namespace Analysis
} // namespace Passes
} // namespace Veriparse

#endif
