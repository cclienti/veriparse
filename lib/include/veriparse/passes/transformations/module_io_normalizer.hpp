// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_MODULE_IO_NORMALIZER
#define VERIPARSE_PASSES_TRANSFORMATIONS_MODULE_IO_NORMALIZER

#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>
#include <string>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

class ModuleIONormalizer : public TransformationBase
{
    /**
     * @return zero on success
     */
    virtual int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

    void remove_module_parameters(AST::Node::Ptr node, AST::Node::Ptr parent = nullptr);

    /// Remove every Port node found in the subtree (body direction declarations
    /// and non-ANSI header references); the normalized port list is rebuilt.
    void remove_module_ports(AST::Node::Ptr node, AST::Node::Ptr parent = nullptr);

    /// Collect (and detach) the body Var/Net declarations whose name matches
    /// @p variable. Port-internal declarations are skipped: only standalone body
    /// declarations (`wire x;`, `reg [..] x;`) are merged into the port.
    void remove_module_variable(const std::string &variable,
                                AST::Declaration::ListPtr removed_variables, AST::Node::Ptr node,
                                AST::Node::Ptr parent = nullptr);

    /// Build a net of the module `default_nettype, carrying @p packed_dims and
    /// @p signing on an ImplicitType data type. Returns null when no default net
    /// type is defined.
    AST::Net::Ptr create_default_net_type_variable(AST::Module::Default_nettypeEnum defnt,
                                                   AST::Dimension::ListPtr packed_dims,
                                                   AST::DataType::SigningEnum signing,
                                                   const std::string &name);
};

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse

#endif
