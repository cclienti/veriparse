// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/passes/transformations/interface_elaboration.hpp>

#include <veriparse/AST/node_cast.hpp>
#include <veriparse/passes/transformations/expression_evaluation.hpp>
#include <veriparse/passes/transformations/scope_table.hpp>
#include <veriparse/logger/logger.hpp>

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
// to_interface_nettype does in the other direction.
AST::Module::LifetimeEnum to_module_lifetime(AST::Interface::LifetimeEnum lifetime)
{
    switch(lifetime) {
    case AST::Interface::LifetimeEnum::AUTOMATIC:
        return AST::Module::LifetimeEnum::AUTOMATIC;
    case AST::Interface::LifetimeEnum::STATIC:
        return AST::Module::LifetimeEnum::STATIC;
    default:
        return AST::Module::LifetimeEnum::NONE;
    }
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
    default:
        return AST::Module::Default_nettypeEnum::NONE;
    }
}

/// Member names of one interface: header ports plus the nets, variables and
/// enumerators its body binds (§25.10 objects). Parameters, typedefs and
/// subroutines are deliberately excluded: parameters inline away during the
/// pseudo-module resolution and subroutines are not externally accessible
/// (ADR-0008 §8) — a reference through the port to such a name fails the
/// member check loudly instead of dangling.
void collect_members(const AST::Interface::Ptr &interface, std::set<std::string> &members)
{
    const auto visit = [&members](const std::string &name, const AST::Node::Ptr &denoted) {
        if(!denoted) {
            return;
        }
        if(denoted->is_node_type(AST::NodeType::Var) ||
           denoted->is_node_category(AST::NodeType::Net) ||
           denoted->is_node_type(AST::NodeType::EnumItem)) {
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

} // namespace

int InterfaceElaboration::prepare(const Analysis::Module::InterfacesMap &interfaces, Design &design)
{
    for(const auto &entry : interfaces) {
        const std::string &name = entry.first;
        // The definition is cloned so extracting the modports below never
        // mutates the source tree.
        const auto &interface = AST::cast_to<AST::Interface>(entry.second->clone());

        auto &members = design.members[name];
        collect_members(interface, members);

        // An instantiation inside an interface body cannot be elaborated: a
        // module is illegal there (§25.3), a nested interface is not
        // supported yet (ADR-0008 §8), and an unknown target could be
        // either.
        const auto &instances = Analysis::Module::get_instance_nodes(interface);
        for(const auto &instance : *instances) {
            if(interfaces.count(instance->get_module()) != 0) {
                LOG_ERROR_N(instance)
                    << "interface '" << instance->get_module() << "' instantiated in interface '"
                    << name << "': nested interfaces are not supported";
            } else {
                LOG_ERROR_N(instance)
                    << "'" << instance->get_module() << "' instantiated in interface '" << name
                    << "': a module cannot be instantiated in an interface (IEEE 1800-2017 25.3)";
            }
            return 1;
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

    return 0;
}

namespace
{

/// The InterfaceType of an interface-typed port declaration, or nullptr.
AST::InterfaceType::Ptr port_interface_type(const AST::Port::Ptr &port)
{
    const auto &decl = port->get_decl();
    const auto &type = decl ? decl->get_type() : nullptr;
    if(type && type->is_node_type(AST::NodeType::InterfaceType)) {
        return AST::cast_to<AST::InterfaceType>(type);
    }
    return nullptr;
}

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

/// How references through one dissolved interface port rewrite.
struct PortRewrite
{
    std::string port_name;
    /// Target label for a scalar or chained port; array-base prefix when the
    /// formal is arrayed and connected to a split instance array.
    std::string base;
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
};

/// Rewrite every `port.member` reference in the subtree onto the actual's
/// base name, checking member existence and modport visibility (§25.10).
/// Calls inherit Identifier, so an external subroutine access (p.method())
/// fails the member check here (ADR-0008 §8).
int rewrite_port_refs(const AST::Node::Ptr &node, const PortRewrite &rw)
{
    if(!node) {
        return 0;
    }

    if(node->is_node_category(AST::NodeType::Identifier)) {
        const auto &identifier = AST::cast_to<AST::Identifier>(node);
        const auto &hier = identifier->get_hier();
        const auto &labels = hier ? hier->get_labellist() : nullptr;
        if(labels && !labels->empty() && labels->front()->get_name() == rw.port_name) {
            if(labels->size() > 1) {
                LOG_ERROR_N(node) << "nested interface path through port '" << rw.port_name
                                  << "' is not supported";
                return 1;
            }
            if(rw.visible->count(identifier->get_name()) == 0) {
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
        }
    }

    int ret = 0;
    const AST::Node::ListPtr children = node->get_children();
    if(children) {
        for(const AST::Node::Ptr &child : *children) {
            ret |= rewrite_port_refs(child, rw);
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
                const auto &type = port_interface_type(port);
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

    for(auto port_it = ports->begin(); port_it != ports->end();) {
        const auto &port = *port_it;
        const auto &type = port_interface_type(port);
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
        // optionally restricted by a connection modport (base.mp).
        const auto &value = portarg->get_value();
        std::string base;
        std::string actual_iface;
        std::string actual_modport;
        bool chained = false;

        std::string ref_name;
        std::string conn_modport;

        if(value->is_node_type(AST::NodeType::Identifier)) {
            const auto &identifier = AST::cast_to<AST::Identifier>(value);
            const auto &hier = identifier->get_hier();
            const auto &labels = hier ? hier->get_labellist() : nullptr;

            if(!labels) {
                ref_name = identifier->get_name();
            } else if(labels->size() == 1) {
                // base.mp or v[k].mp: the leaf is the connection modport.
                conn_modport = identifier->get_name();
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
                if(arrayed) {
                    LOG_ERROR_N(portarg)
                        << "arrayed interface port '" << port_name << "' of '" << child->get_name()
                        << "' cannot connect to the single instance '" << ref_name
                        << "' (IEEE 1800-2017 23.3.3.5)";
                    return 1;
                }
                actual_iface = inst_it->second;
            } else {
                const auto port_ref_it = scope.ports.find(ref_name);
                if(port_ref_it != scope.ports.end()) {
                    actual_iface = port_ref_it->second.iface;
                    ref_modport = port_ref_it->second.modport;
                    chained = true;
                } else if(arrayed) {
                    // Assumed base of a split instance array; every element
                    // is checked per reference (§23.3.3.5).
                    actual_iface = iface;
                }
            }

            if(!actual_iface.empty()) {
                if(!conn_modport.empty()) {
                    const auto amp_it = design.modports.find(actual_iface);
                    if(amp_it == design.modports.end() || amp_it->second.count(conn_modport) == 0) {
                        LOG_ERROR_N(value) << "interface '" << actual_iface << "' has no modport '"
                                           << conn_modport << "' (IEEE 1800-2017 25.5)";
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

        const auto members_it = design.members.find(iface);
        const std::set<std::string> &visible =
            effective.empty() ? members_it->second : modports_it->second.at(effective);

        PortRewrite rewrite;
        rewrite.port_name = port_name;
        rewrite.base = base;
        rewrite.iface = iface;
        rewrite.visible = &visible;
        rewrite.modport = effective;
        rewrite.arrayed = arrayed;
        rewrite.chained = chained;
        rewrite.instances = &scope.instances;

        if(rewrite_port_refs(child, rewrite)) {
            return 1;
        }

        // Drop the dissolved port and its connection: the value-port
        // binding must never clone the interface Arg nor synthesize an
        // assignment for it — connection is aliasing (§25.3.2).
        portargs->remove(portarg);
        port_it = ports->erase(port_it);
    }

    return 0;
}

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse
