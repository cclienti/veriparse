// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/passes/transformations/interface_elaboration.hpp>

#include <veriparse/AST/node_cast.hpp>
#include <veriparse/passes/transformations/expression_evaluation.hpp>
#include <veriparse/passes/transformations/scope_table.hpp>
#include <veriparse/logger/logger.hpp>

#include <algorithm>
#include <cctype>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

namespace
{

// The generated per-node enums are identical value lists that astgen cannot
// share (ADR-0002 §2.1); these switches bridge them, like the parser's
// to_interface_nettype does in the other direction. They are deliberately
// exhaustive with no `default`: a new enumerator added to the yaml makes the
// switch non-exhaustive, which -Wall -Werror turns into a compile error at
// this exact spot rather than silently mapping the new value to NONE.
AST::Module::LifetimeEnum to_module_lifetime(AST::Interface::LifetimeEnum lifetime)
{
    switch(lifetime) {
    case AST::Interface::LifetimeEnum::NONE:
        return AST::Module::LifetimeEnum::NONE;
    case AST::Interface::LifetimeEnum::AUTOMATIC:
        return AST::Module::LifetimeEnum::AUTOMATIC;
    case AST::Interface::LifetimeEnum::STATIC:
        return AST::Module::LifetimeEnum::STATIC;
    }
    return AST::Module::LifetimeEnum::NONE;
}

AST::Module::Default_nettypeEnum to_module_nettype(AST::Interface::Default_nettypeEnum nettype)
{
    switch(nettype) {
    case AST::Interface::Default_nettypeEnum::WIRE:
        return AST::Module::Default_nettypeEnum::WIRE;
    case AST::Interface::Default_nettypeEnum::TRI:
        return AST::Module::Default_nettypeEnum::TRI;
    case AST::Interface::Default_nettypeEnum::TRI0:
        return AST::Module::Default_nettypeEnum::TRI0;
    case AST::Interface::Default_nettypeEnum::TRI1:
        return AST::Module::Default_nettypeEnum::TRI1;
    case AST::Interface::Default_nettypeEnum::TRIAND:
        return AST::Module::Default_nettypeEnum::TRIAND;
    case AST::Interface::Default_nettypeEnum::TRIOR:
        return AST::Module::Default_nettypeEnum::TRIOR;
    case AST::Interface::Default_nettypeEnum::TRIREG:
        return AST::Module::Default_nettypeEnum::TRIREG;
    case AST::Interface::Default_nettypeEnum::WAND:
        return AST::Module::Default_nettypeEnum::WAND;
    case AST::Interface::Default_nettypeEnum::WOR:
        return AST::Module::Default_nettypeEnum::WOR;
    case AST::Interface::Default_nettypeEnum::UWIRE:
        return AST::Module::Default_nettypeEnum::UWIRE;
    case AST::Interface::Default_nettypeEnum::SUPPLY0:
        return AST::Module::Default_nettypeEnum::SUPPLY0;
    case AST::Interface::Default_nettypeEnum::SUPPLY1:
        return AST::Module::Default_nettypeEnum::SUPPLY1;
    case AST::Interface::Default_nettypeEnum::NONE:
        return AST::Module::Default_nettypeEnum::NONE;
    }
    return AST::Module::Default_nettypeEnum::NONE;
}

/// Member names of one interface: header ports plus the nets and variables
/// its body binds (§25.10 objects), collected into `members`. Enumerators go
/// into `enumerators` instead: they are constants, not aliasable signals, so
/// a reference through a port must be rejected, not rewritten to a net.
/// Parameters, typedefs and subroutines are deliberately excluded: parameters
/// inline away during the pseudo-module resolution and subroutines are not
/// externally accessible (ADR-0008 §8) — a reference through the port to such
/// a name fails the member check loudly instead of dangling.
void collect_members(const AST::Interface::Ptr &interface, std::set<std::string> &members,
                     std::set<std::string> &enumerators)
{
    const auto visit = [&members, &enumerators](const std::string &name,
                                                const AST::Node::Ptr &denoted) {
        if(!denoted) {
            return;
        }
        if(denoted->is_node_type(AST::NodeType::EnumItem)) {
            enumerators.insert(name);
        } else if(denoted->is_node_type(AST::NodeType::Var) ||
                  denoted->is_node_category(AST::NodeType::Net)) {
            members.insert(name);
        }
    };

    const auto &ports = interface->get_ports();
    if(ports) {
        for(const AST::Port::Ptr &port : *ports) {
            if(port->get_decl()) {
                ScopeTable::for_each_bound_name(port->get_decl(), visit);
            }
        }
    }

    const auto &items = interface->get_items();
    if(items) {
        for(const AST::Node::Ptr &item : *items) {
            ScopeTable::for_each_bound_name(item, visit);
        }
    }
}

/// Validate one modport (§25.5: every listed name shall be declared by the
/// same interface) and record its visible member set.
int collect_modport(const AST::Modport::Ptr &modport, const std::string &interface_name,
                    const std::set<std::string> &members,
                    std::map<std::string, std::set<std::string>> &modports)
{
    if(modports.count(modport->get_name()) != 0) {
        LOG_ERROR_N(modport) << "modport '" << modport->get_name() << "' already declared in '"
                             << interface_name << "'";
        return 1;
    }

    std::set<std::string> visible;
    const auto &ports = modport->get_ports();
    if(ports) {
        for(const AST::ModportPort::Ptr &mp_port : *ports) {
            if(members.count(mp_port->get_name()) == 0) {
                LOG_ERROR_N(mp_port)
                    << "modport '" << modport->get_name() << "' lists '" << mp_port->get_name()
                    << "', which interface '" << interface_name
                    << "' does not declare (IEEE 1800-2017 25.5)";
                return 1;
            }
            visible.insert(mp_port->get_name());
        }
    }

    modports.emplace(modport->get_name(), std::move(visible));
    return 0;
}

/// Depth-first check that no interface reaches itself through nested
/// instantiations — elaboration would recurse forever.
int check_nested_acyclic(const std::string &name,
                         const std::map<std::string, std::map<std::string, std::string>> &nested,
                         std::set<std::string> &path, std::set<std::string> &done)
{
    if(done.count(name) != 0) {
        return 0;
    }
    if(!path.insert(name).second) {
        LOG_ERROR << "interface '" << name << "' instantiates itself through its nested interfaces";
        return 1;
    }
    const auto it = nested.find(name);
    if(it != nested.end()) {
        for(const auto &sub : it->second) {
            if(check_nested_acyclic(sub.second, nested, path, done)) {
                return 1;
            }
        }
    }
    path.erase(name);
    done.insert(name);
    return 0;
}

} // namespace

int InterfaceElaboration::prepare(const Analysis::Module::InterfacesMap &interfaces, Design &design)
{
    for(const auto &entry : interfaces) {
        const std::string &name = entry.first;
        // The definition is cloned so extracting the modports below never
        // mutates the source tree.
        const auto &interface = AST::cast_to<AST::Interface>(entry.second->clone());

        auto &members = design.members[name];
        collect_members(interface, members, design.enumerators[name]);

        // A nested interface instance elaborates through the pseudo-module
        // recursion; its name is recorded so port-path references
        // (p.tx.field) validate structurally (§8.1). A module instantiation
        // stays illegal (§25.3), and so does an unknown target (it could be
        // either).
        auto &nested = design.nested[name];
        const auto &instances = Analysis::Module::get_instance_nodes(interface);
        for(const auto &instance : *instances) {
            if(interfaces.count(instance->get_module()) != 0) {
                nested.emplace(instance->get_name(), instance->get_module());
            } else {
                LOG_ERROR_N(instance)
                    << "'" << instance->get_module() << "' instantiated in interface '" << name
                    << "': a module cannot be instantiated in an interface (IEEE 1800-2017 25.3)";
                return 1;
            }
        }

        // Extract and validate the modports; they are direction views, not
        // items the pseudo-module may keep.
        auto &modports = design.modports[name];
        const auto &items = interface->get_items();
        if(items) {
            for(auto it = items->begin(); it != items->end();) {
                if((*it)->is_node_type(AST::NodeType::Modport)) {
                    const auto &modport = AST::cast_to<AST::Modport>(*it);
                    if(collect_modport(modport, name, members, modports)) {
                        return 1;
                    }
                    it = items->erase(it);
                } else {
                    ++it;
                }
            }
        }

        // Transplant into a pseudo-module: the definition is module-shaped
        // (ADR-0002 §2.1), so the existing per-instance flatten path
        // elaborates it unchanged.
        const auto &pseudo =
            std::make_shared<AST::Module>(interface->get_filename(), interface->get_line());
        pseudo->set_name(interface->get_name());
        pseudo->set_lifetime(to_module_lifetime(interface->get_lifetime()));
        pseudo->set_default_nettype(to_module_nettype(interface->get_default_nettype()));
        pseudo->set_params(interface->get_params());
        pseudo->set_ports(interface->get_ports());
        pseudo->set_items(interface->get_items());

        design.pseudo_modules.emplace(name, pseudo);
    }

    std::set<std::string> path;
    std::set<std::string> done;
    for(const auto &entry : interfaces) {
        if(check_nested_acyclic(entry.first, design.nested, path, done)) {
            return 1;
        }
    }

    return 0;
}

namespace
{

/// §25.5: a connection may name a modport in the port declaration, in the
/// port connection, or both — in which case the two names shall be
/// identical.
int merge_modports(const std::string &header, const std::string &connection,
                   const AST::Node::Ptr &where, std::string &effective)
{
    if(header.empty() || connection.empty() || header == connection) {
        effective = header.empty() ? connection : header;
        return 0;
    }
    LOG_ERROR_N(where) << "modport '" << connection << "' of the connection does not match "
                       << "modport '" << header << "' of the port declaration "
                       << "(IEEE 1800-2017 25.5)";
    return 1;
}

/// The decimal value of a constant index expression, or false.
bool constant_index(const AST::Node::Ptr &node, std::string &index_str)
{
    mpz_class value;
    if(!node || !ExpressionEvaluation().evaluate_node(node, value)) {
        return false;
    }
    index_str = value.str();
    return true;
}

/// True when `base` names a split interface array in scope: at least one
/// element `base<index>` is a scope instance of interface `iface`. The
/// normalizer names split elements `base` + decimal index (§23.3.3.5), so a
/// bare arrayed-port actual is a valid array base only if such an element
/// exists — otherwise it names nothing and must be rejected.
bool names_array_instance(const std::map<std::string, std::string> &instances,
                          const std::string &base, const std::string &iface)
{
    for(const auto &entry : instances) {
        const std::string &name = entry.first;
        if(name.size() <= base.size() || name.compare(0, base.size(), base) != 0 ||
           entry.second != iface) {
            continue;
        }
        if(std::all_of(name.begin() + base.size(), name.end(),
                       [](unsigned char c) { return std::isdigit(c) != 0; })) {
            return true;
        }
    }
    return false;
}

/// How references through one dissolved interface port rewrite.
struct PortRewrite
{
    std::string port_name;
    /// Target label for a scalar or chained port; array-base prefix when the
    /// formal is arrayed and connected to a split instance array.
    std::string base;
    /// Path labels spliced after the base when the actual is a nested
    /// interface element (.c(bus.tx) → base bus, extra tx).
    std::vector<std::string> extra_labels;
    std::string iface;
    const std::set<std::string> *visible{nullptr};
    std::string modport;
    /// The formal carries unpacked dims: references must be indexed.
    bool arrayed{false};
    /// The base is an own (chained) port: the reference index is kept for
    /// the enclosing level to resolve.
    bool chained{false};
    /// Scope instances, for the per-index element checks (§23.3.3.5).
    const std::map<std::string, std::string> *instances{nullptr};
    /// Interface knowledge, for the nested-path walk (§8.1).
    const InterfaceElaboration::Design *design{nullptr};
};

/// Rewrite every `port.member` reference in the subtree onto the actual's
/// base name, checking member existence and modport visibility (§25.10).
/// Calls inherit Identifier, so an external subroutine access (p.method())
/// fails the member check here (ADR-0008 §8).
///
/// All of a child's interface ports are rewritten in a single walk, keyed by
/// port name: a per-port sequence of full-tree passes would re-capture an
/// earlier rewrite whose base coincides with a later port name (a port named
/// like another port's actual).
int rewrite_port_refs(const AST::Node::Ptr &node,
                      const std::map<std::string, PortRewrite> &rewrites)
{
    if(!node) {
        return 0;
    }

    if(node->is_node_category(AST::NodeType::Identifier)) {
        const auto &identifier = AST::cast_to<AST::Identifier>(node);
        const auto &hier = identifier->get_hier();
        const auto &labels = hier ? hier->get_labellist() : nullptr;
        const auto rw_it = labels && !labels->empty() ? rewrites.find(labels->front()->get_name())
                                                      : rewrites.end();
        if(rw_it != rewrites.end()) {
            const PortRewrite &rw = rw_it->second;
            // Walk the reference path (§8.1): intermediate labels descend the
            // nested-instance table, the leaf must be a member of the final
            // interface. A modport restricts the first level of the
            // port-reference path (§25.10) and cannot list a nested instance,
            // so any nested path through a modport-qualified port is
            // rejected.
            if(labels->size() > 1 && !rw.modport.empty()) {
                LOG_ERROR_N(node) << "a nested interface path is not visible through modport '"
                                  << rw.modport << "' of interface '" << rw.iface
                                  << "' (IEEE 1800-2017 25.10)";
                return 1;
            }

            std::string leaf_iface = rw.iface;
            for(auto lit = std::next(labels->begin()); lit != labels->end(); ++lit) {
                const auto &tail = *lit;
                if(tail->get_loop()) {
                    LOG_ERROR_N(node) << "indexed nested interface path through port '"
                                      << rw.port_name << "' is not supported";
                    return 1;
                }
                bool found = false;
                const auto nested_it = rw.design->nested.find(leaf_iface);
                if(nested_it != rw.design->nested.end()) {
                    const auto sub_it = nested_it->second.find(tail->get_name());
                    if(sub_it != nested_it->second.end()) {
                        leaf_iface = sub_it->second;
                        found = true;
                    }
                }
                if(!found) {
                    LOG_ERROR_N(node) << "'" << tail->get_name()
                                      << "' is not a nested interface of '" << leaf_iface << "'";
                    return 1;
                }
            }

            if(labels->size() == 1) {
                if(rw.visible->count(identifier->get_name()) == 0) {
                    const auto en_it = rw.design->enumerators.find(rw.iface);
                    if(en_it != rw.design->enumerators.end() &&
                       en_it->second.count(identifier->get_name()) != 0) {
                        LOG_ERROR_N(node)
                            << "enumerator '" << identifier->get_name() << "' of interface '"
                            << rw.iface << "' cannot be accessed through an interface port: "
                            << "it is a constant, not a signal";
                        return 1;
                    }
                    if(rw.modport.empty()) {
                        LOG_ERROR_N(node) << "interface '" << rw.iface << "' has no member '"
                                          << identifier->get_name() << "'";
                    } else {
                        LOG_ERROR_N(node)
                            << "member '" << identifier->get_name() << "' is not visible through "
                            << "modport '" << rw.modport << "' of interface '" << rw.iface
                            << "' (IEEE 1800-2017 25.10)";
                    }
                    return 1;
                }
            } else {
                const auto mem_it = rw.design->members.find(leaf_iface);
                if(mem_it == rw.design->members.end() ||
                   mem_it->second.count(identifier->get_name()) == 0) {
                    LOG_ERROR_N(node) << "interface '" << leaf_iface << "' has no member '"
                                      << identifier->get_name() << "'";
                    return 1;
                }
            }

            const auto &label = labels->front();
            if(!rw.arrayed) {
                if(label->get_loop()) {
                    LOG_ERROR_N(node) << "indexed reference through scalar interface port '"
                                      << rw.port_name << "'";
                    return 1;
                }
                label->set_name(rw.base);
            } else if(rw.chained) {
                // The index rides along: the level owning the array resolves
                // it against its own split elements.
                if(!label->get_loop()) {
                    LOG_ERROR_N(node)
                        << "arrayed interface port '" << rw.port_name << "' requires an index";
                    return 1;
                }
                label->set_name(rw.base);
            } else {
                if(!label->get_loop()) {
                    LOG_ERROR_N(node)
                        << "arrayed interface port '" << rw.port_name << "' requires an index";
                    return 1;
                }
                std::string index_str;
                if(!constant_index(label->get_loop(), index_str)) {
                    LOG_ERROR_N(node)
                        << "non-constant index on arrayed interface port '" << rw.port_name << "'";
                    return 1;
                }
                const std::string element = rw.base + index_str;
                const auto element_it = rw.instances->find(element);
                if(element_it == rw.instances->end() || element_it->second != rw.iface) {
                    LOG_ERROR_N(node)
                        << "'" << rw.base << "[" << index_str << "]' does not name an instance "
                        << "of interface '" << rw.iface << "' (IEEE 1800-2017 23.3.3.5)";
                    return 1;
                }
                label->set_name(element);
                label->set_loop(nullptr);
            }

            // A nested actual splices its path after the base label
            // (.c(bus.tx): c.field → bus.tx.field).
            if(!rw.extra_labels.empty()) {
                const auto insert_pos = std::next(labels->begin());
                for(const std::string &extra : rw.extra_labels) {
                    const auto &hier_label =
                        std::make_shared<AST::HierLabel>(node->get_filename(), node->get_line());
                    hier_label->set_name(extra);
                    labels->insert(insert_pos, hier_label);
                }
            }
        }
    }

    int ret = 0;
    const AST::Node::ListPtr children = node->get_children();
    if(children) {
        for(const AST::Node::Ptr &child : *children) {
            ret |= rewrite_port_refs(child, rewrites);
        }
    }
    return ret;
}

} // namespace

int InterfaceElaboration::collect_scope(const AST::Node::Ptr &module, const Design &design,
                                        ScopeSymbols &symbols)
{
    symbols.instances.clear();
    symbols.ports.clear();

    const auto &instances = Analysis::Module::get_instance_nodes(module);
    for(const auto &instance : *instances) {
        if(design.is_interface(instance->get_module())) {
            symbols.instances.emplace(instance->get_name(), instance->get_module());
        }
    }

    if(module->is_node_type(AST::NodeType::Module)) {
        const auto &ports = AST::cast_to<AST::Module>(module)->get_ports();
        if(ports) {
            for(const AST::Port::Ptr &port : *ports) {
                const auto &type = Analysis::Module::get_port_interface_type(port);
                if(type) {
                    symbols.ports.emplace(
                        port->get_decl()->get_name(),
                        ScopeSymbols::PortRef{type->get_name(), type->get_modport()});
                }
            }
        }
    }

    return 0;
}

int InterfaceElaboration::bind_interface_ports(const AST::Instance::Ptr &instance,
                                               const AST::Module::Ptr &child, const Design &design,
                                               const ScopeSymbols &scope)
{
    const auto &ports = child->get_ports();
    if(!ports) {
        return 0;
    }

    // All interface ports of the child are decoded and validated first, then
    // rewritten in a single pass (see rewrite_port_refs): sequential per-port
    // passes would re-capture across ports.
    std::map<std::string, PortRewrite> rewrites;

    for(auto port_it = ports->begin(); port_it != ports->end();) {
        const auto &port = *port_it;
        const auto &type = Analysis::Module::get_port_interface_type(port);
        if(!type) {
            ++port_it;
            continue;
        }

        const std::string &port_name = port->get_decl()->get_name();
        const std::string &iface = type->get_name();

        const bool arrayed =
            port->get_decl()->is_node_type(AST::NodeType::Arg) &&
            AST::cast_to<AST::Arg>(port->get_decl())->get_unpacked_dims() != nullptr;

        const auto modports_it = design.modports.find(iface);
        if(!type->get_modport().empty() && (modports_it == design.modports.end() ||
                                            modports_it->second.count(type->get_modport()) == 0)) {
            LOG_ERROR_N(port) << "interface '" << iface << "' has no modport '"
                              << type->get_modport() << "' (IEEE 1800-2017 25.5)";
            return 1;
        }

        // Locate the connection. An interface port cannot be left
        // unconnected (§23.3.3.4).
        const auto &portargs = instance->get_portlist();
        AST::PortArg::Ptr portarg;
        if(portargs) {
            for(const AST::PortArg::Ptr &arg : *portargs) {
                if(arg->get_name() == port_name) {
                    portarg = arg;
                    break;
                }
            }
        }
        if(!portarg || !portarg->get_value()) {
            LOG_ERROR_N(instance) << "interface port '" << port_name << "' of '"
                                  << child->get_name()
                                  << "' is not connected (IEEE 1800-2017 23.3.3.4)";
            return 1;
        }

        // Decode the actual: a bare interface instance, an element of a
        // split instance array (v[k]), or an own interface port (chaining) —
        // optionally restricted by a connection modport (base.mp), or naming
        // a nested interface element (base.tx, §8.1).
        const auto &value = portarg->get_value();
        std::string base;
        std::vector<std::string> extra_labels;
        std::string actual_iface;
        std::string actual_modport;
        bool chained = false;
        bool array_base = false;

        std::string ref_name;
        std::string leaf;

        if(value->is_node_type(AST::NodeType::Identifier)) {
            const auto &identifier = AST::cast_to<AST::Identifier>(value);
            const auto &hier = identifier->get_hier();
            const auto &labels = hier ? hier->get_labellist() : nullptr;

            if(!labels) {
                ref_name = identifier->get_name();
            } else if(labels->size() == 1) {
                // base.X or v[k].X: the leaf X is a connection modport or a
                // nested interface instance — classified once the base's
                // interface is known.
                leaf = identifier->get_name();
                std::string index_str;
                if(!labels->front()->get_loop()) {
                    ref_name = labels->front()->get_name();
                } else if(constant_index(labels->front()->get_loop(), index_str)) {
                    ref_name = labels->front()->get_name() + index_str;
                }
            }
        } else if(value->is_node_type(AST::NodeType::Pointer)) {
            // v[k]: an element of a split instance array.
            const auto &pointer = AST::cast_to<AST::Pointer>(value);
            const auto &var = pointer->get_var();
            std::string index_str;
            if(var && var->is_node_type(AST::NodeType::Identifier) &&
               !AST::cast_to<AST::Identifier>(var)->get_hier() &&
               constant_index(pointer->get_ptr(), index_str)) {
                ref_name = AST::cast_to<AST::Identifier>(var)->get_name() + index_str;
            }
        }

        if(!ref_name.empty()) {
            std::string ref_modport;
            const auto inst_it = scope.instances.find(ref_name);
            if(inst_it != scope.instances.end()) {
                actual_iface = inst_it->second;
            } else {
                const auto port_ref_it = scope.ports.find(ref_name);
                if(port_ref_it != scope.ports.end()) {
                    actual_iface = port_ref_it->second.iface;
                    ref_modport = port_ref_it->second.modport;
                    chained = true;
                } else if(arrayed && leaf.empty() &&
                          names_array_instance(scope.instances, ref_name, iface)) {
                    // Base of a split interface array: at least one element
                    // exists in scope, and each referenced element is checked
                    // per reference (§23.3.3.5). Without a matching element the
                    // actual names nothing and falls through to the
                    // does-not-name-an-instance error below.
                    actual_iface = iface;
                    array_base = true;
                }
            }

            if(!actual_iface.empty()) {
                std::string conn_modport;
                if(!leaf.empty()) {
                    // Classify the leaf against the base's interface: a
                    // connection modport (§25.5) or a nested interface
                    // element (§8.1).
                    const auto amp_it = design.modports.find(actual_iface);
                    const auto nn_it = design.nested.find(actual_iface);
                    if(amp_it != design.modports.end() && amp_it->second.count(leaf) != 0) {
                        conn_modport = leaf;
                    } else if(nn_it != design.nested.end() && nn_it->second.count(leaf) != 0) {
                        if(!ref_modport.empty()) {
                            LOG_ERROR_N(value)
                                << "nested interface '" << leaf << "' is not visible through "
                                << "modport '" << ref_modport << "' of port '" << ref_name
                                << "' (IEEE 1800-2017 25.10)";
                            return 1;
                        }
                        extra_labels.push_back(leaf);
                        actual_iface = nn_it->second.at(leaf);
                    } else {
                        LOG_ERROR_N(value)
                            << "interface '" << actual_iface << "' has no modport or nested "
                            << "interface '" << leaf << "' (IEEE 1800-2017 25.5)";
                        return 1;
                    }
                }
                // A chained port may itself be modport-restricted; the
                // views must agree (§25.5).
                if(merge_modports(ref_modport, conn_modport, value, actual_modport)) {
                    return 1;
                }
                base = ref_name;
            }
        }

        // A scalar actual — single instance or nested element — cannot drive
        // an arrayed formal (§23.3.3.5).
        if(arrayed && !base.empty() && !array_base && !chained) {
            LOG_ERROR_N(portarg) << "arrayed interface port '" << port_name << "' of '"
                                 << child->get_name()
                                 << "' cannot connect to a single interface instance "
                                 << "(IEEE 1800-2017 23.3.3.5)";
            return 1;
        }

        if(base.empty()) {
            LOG_ERROR_N(portarg) << "the actual of interface port '" << port_name << "' of '"
                                 << child->get_name() << "' does not name an interface instance "
                                 << "(IEEE 1800-2017 23.3.3.4)";
            return 1;
        }

        if(actual_iface != iface) {
            LOG_ERROR_N(portarg) << "interface port '" << port_name << "' of '" << child->get_name()
                                 << "' has type '" << iface
                                 << "' but is connected to an instance of '" << actual_iface
                                 << "' (IEEE 1800-2017 23.3.3.4)";
            return 1;
        }

        std::string effective;
        if(merge_modports(type->get_modport(), actual_modport, portarg, effective)) {
            return 1;
        }

        // The effective modport must exist: it may have come from a chained
        // port declared with a bogus modport, which the header check above
        // does not cover. Validate before indexing the modport table.
        if(!effective.empty() &&
           (modports_it == design.modports.end() || modports_it->second.count(effective) == 0)) {
            LOG_ERROR_N(portarg) << "interface '" << iface << "' has no modport '" << effective
                                 << "' (IEEE 1800-2017 25.5)";
            return 1;
        }

        const auto members_it = design.members.find(iface);
        const std::set<std::string> &visible =
            effective.empty() ? members_it->second : modports_it->second.at(effective);

        PortRewrite rewrite;
        rewrite.port_name = port_name;
        rewrite.base = base;
        rewrite.extra_labels = extra_labels;
        rewrite.iface = iface;
        rewrite.visible = &visible;
        rewrite.modport = effective;
        rewrite.arrayed = arrayed;
        rewrite.chained = chained;
        rewrite.instances = &scope.instances;
        rewrite.design = &design;
        rewrites.emplace(port_name, std::move(rewrite));

        // Drop the dissolved port and its connection: the value-port
        // binding must never clone the interface Arg nor synthesize an
        // assignment for it — connection is aliasing (§25.3.2).
        portargs->remove(portarg);
        port_it = ports->erase(port_it);
    }

    // Single rewrite pass over the child once every port's target is known.
    if(!rewrites.empty() && rewrite_port_refs(child, rewrites)) {
        return 1;
    }

    return 0;
}

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse
