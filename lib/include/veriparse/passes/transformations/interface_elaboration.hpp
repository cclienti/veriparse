// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_INTERFACE_ELABORATION
#define VERIPARSE_PASSES_TRANSFORMATIONS_INTERFACE_ELABORATION

#include <map>
#include <set>
#include <string>

#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/analysis/module.hpp>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

/**
 * @brief Lower SystemVerilog interfaces to plain signals during flattening.
 *
 * An interface definition is module-like (params, header ports, body items),
 * so it is transplanted into a genuine pseudo-module and merged into the
 * flattener's modules map: the existing per-instance clone → parametrize →
 * resolve → inline path then elaborates each interface instance unchanged —
 * body logic is spliced once per instance and every user of the instance
 * aliases the same flattened signals.
 */
class InterfaceElaboration
{
public:
    /**
     * @brief Design-wide interface knowledge, prepared once and shared
     * across the flattener recursion.
     */
    struct Design
    {
        /**
         * @brief Interface definitions transplanted into pseudo-modules,
         * merged into the flattener's modules map (cloned per instance).
         */
        Analysis::Module::ModulesMap pseudo_modules;

        /**
         * @brief Interface name → modport name → member names the modport
         * lists (the visible set through a modport-qualified port).
         */
        std::map<std::string, std::map<std::string, std::set<std::string>>> modports;

        /**
         * @brief Interface name → member names (header ports plus body
         * nets, variables and enumerators). Parameter-independent.
         */
        std::map<std::string, std::set<std::string>> members;

        bool is_interface(const std::string &name) const { return pseudo_modules.count(name) != 0; }
    };

    /**
     * @brief One scope's interface symbols: what an interface-port actual
     * may legally name in the instantiating module.
     */
    struct ScopeSymbols
    {
        /** @brief Reference through an own interface port (chaining). */
        struct PortRef
        {
            std::string iface;
            std::string modport;
        };

        /** @brief Local interface instance name → interface name. */
        std::map<std::string, std::string> instances;

        /** @brief Own interface-typed port name → its type view. */
        std::map<std::string, PortRef> ports;
    };

    InterfaceElaboration() = delete;

    /**
     * @brief Validate every interface definition and transplant it into a
     * pseudo-module in the design.
     *
     * Modports are validated (every listed name is a member) and stripped;
     * an instantiation inside an interface body is a hard error (a module
     * per IEEE 1800-2017 §25.3, a nested interface as an unsupported
     * construct).
     *
     * @return zero on success
     */
    static int prepare(const Analysis::Module::InterfacesMap &interfaces, Design &design);

    /**
     * @brief Record the module's interface instances and interface-typed
     * ports — the names its children's interface ports may connect to.
     *
     * @return zero on success
     */
    static int collect_scope(const AST::Node::Ptr &module, const Design &design,
                             ScopeSymbols &symbols);

    /**
     * @brief Dissolve the interface ports of an instantiated child.
     *
     * For each interface-typed port of the (already flattened and prefixed)
     * child clone: decode and validate the connected actual (§23.3.3.4),
     * merge the header and connection modports (§25.5), rewrite every
     * child-body reference `port.member` onto the actual's base name with
     * member-visibility checks (§25.10), then drop the port and its
     * connection so the regular value-port binding never sees them —
     * connection is aliasing, not a copy (§25.3.2).
     *
     * @return zero on success
     */
    static int bind_interface_ports(const AST::Instance::Ptr &instance,
                                    const AST::Module::Ptr &child, const Design &design,
                                    const ScopeSymbols &scope);
};

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse

#endif
