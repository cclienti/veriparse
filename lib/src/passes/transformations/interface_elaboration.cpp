// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/passes/transformations/interface_elaboration.hpp>

#include <veriparse/AST/node_cast.hpp>
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

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse
