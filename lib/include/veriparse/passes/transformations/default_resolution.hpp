// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_DEFAULT_RESOLUTION
#define VERIPARSE_PASSES_TRANSFORMATIONS_DEFAULT_RESOLUTION

#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>

#include <set>
#include <string>
#include <vector>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

/**
 * @brief Implicit-default resolution (ADR-0012): replace every deferred
 * implicit default with what the standard says it means, so a
 * declaration's type, net kind and direction are all explicit and
 * downstream passes stop re-deriving them.
 *
 *  - `ImplicitType` -> `logic` with the same signing and packed dims
 *    (IEEE 1800-2017 §6.8) — SystemVerilog mode only, so plain-Verilog
 *    designs keep rendering as 1364-2005. Value parameters follow
 *    §6.20.2 instead: only a ranged implicit type resolves; a range-less
 *    one takes its type from the final value and is left alone.
 *  - `ImplicitNet` -> a net of the prevailing `` `default_nettype ``
 *    (§22.8), in both modes; `` `default_nettype none `` makes the
 *    implicit net a hard error.
 *  - The §23.2.2.3 ANSI port defaults: first-port `inout` direction,
 *    direction inheritance, and the port-kind rules (output ports with
 *    an explicit data type become variables; ref ports are variables).
 *    Interface ports are legitimately directionless and stay untouched.
 *
 * The pass is context-local (no symbol table, ADR-0006 §8) and runs
 * design-wide right after NameResolution (ADR-0009 §7).
 */
class DefaultResolution : public TransformationBase
{
public:
    explicit DefaultResolution(bool sv_mode) : m_sv_mode(sv_mode) {}

    /**
     * @brief Resolve a whole design, one source at a time (the rewrites
     * are context-local, so no cross-source state is involved).
     * @return zero on success.
     */
    int run_design(const std::vector<AST::Node::Ptr> &sources);

protected:
    /**
     * @brief Single-source entry (the multi-source case is run_design).
     */
    virtual int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

private:
    /// Walk a source down to its Module/Interface definitions.
    int resolve_source(const AST::Node::Ptr &source);

    /// Resolve one module-like definition: §23.2.2.3 over the ANSI header
    /// ports, then the parameter and body walks.
    int resolve_module(const AST::Module::Ptr &module);
    int resolve_interface(const AST::Interface::Ptr &interface);
    int resolve_module_like(const AST::Declaration::ListPtr &params,
                            const AST::Port::ListPtr &ports, const AST::Node::ListPtr &items,
                            AST::Module::Default_nettypeEnum defnt);

    /// §23.2.2.3 directions over the ANSI header port list: a first (or
    /// fresh-group) directionless port defaults to inout, a subsequent one
    /// inherits the previous port's direction. Interface ports stay
    /// directionless and start a fresh inheritance group.
    void resolve_port_directions(const AST::Port::ListPtr &ports);

    /// Resolve one port's kind (§23.2.2.3): an ImplicitNet declaration
    /// becomes a net of @p defnt, except an output with an explicit data
    /// type or a ref port, which become variables. @p declared holds the
    /// standalone body declaration names — a body direction declaration
    /// backed by one is a reference, not a net creation, and is left
    /// untouched.
    int resolve_port_kind(const AST::Port::Ptr &port, AST::Module::Default_nettypeEnum defnt,
                          const std::set<std::string> &declared);

    /// Recursive body walk: port kinds for body direction declarations,
    /// and (SV mode) ImplicitType materialization on declarations and
    /// subroutine return types. Below a Function/Task (@p in_subroutine) a
    /// Port is an old-style formal — a variable, never a net (§13.3/§13.4)
    /// — so the §23.2.2.3 kind rules stop at the subroutine boundary and
    /// the formal stays as parsed.
    int resolve_body(const AST::Node::Ptr &node, AST::Module::Default_nettypeEnum defnt,
                     const std::set<std::string> &declared, bool in_subroutine = false);

    /// §6.8 (and §6.20.2 for parameters): materialize a declaration's
    /// ImplicitType as logic, preserving signing and packed dims.
    void resolve_decl_type(const AST::Declaration::Ptr &decl);

    /// Names of the standalone body Var/Net declarations of a module-like
    /// definition (port-internal declarations excluded).
    static std::set<std::string> declared_signal_names(const AST::Node::ListPtr &items);

    bool m_sv_mode;
};

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse

#endif
