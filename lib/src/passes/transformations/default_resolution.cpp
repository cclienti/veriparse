// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/passes/transformations/default_resolution.hpp>
#include <veriparse/passes/transformations/net_defaults.hpp>
#include <veriparse/passes/analysis/declaration_helpers.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/passes/analysis/storage_kind.hpp>
#include <veriparse/logger/logger.hpp>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

namespace
{

// §6.8 replacement of an implicit type: a logic carrying the implicit
// type's signing and packed dims verbatim, so widths and signedness are
// unchanged by construction.
AST::DataType::Ptr make_logic_type(const AST::DataType::Ptr &implicit)
{
    AST::DataType::Ptr logic =
        std::make_shared<AST::LogicType>(implicit->get_filename(), implicit->get_line());
    logic->set_signing(implicit->get_signing());
    logic->set_packed_dims(implicit->get_packed_dims());
    return logic;
}

} // namespace

int DefaultResolution::run_design(const std::vector<AST::Node::Ptr> &sources)
{
    int ret = 0;
    for(const AST::Node::Ptr &source : sources) {
        ret += resolve_source(source);
    }
    return ret;
}

int DefaultResolution::process(AST::Node::Ptr node, AST::Node::Ptr parent)
{
    (void)parent;
    return resolve_source(node);
}

int DefaultResolution::resolve_source(const AST::Node::Ptr &source)
{
    if(!source) {
        return 0;
    }

    switch(source->get_node_type()) {
    case AST::NodeType::Source:
        return resolve_source(AST::to_node(AST::cast_to<AST::Source>(source)->get_description()));

    case AST::NodeType::Description: {
        const auto &definitions = AST::cast_to<AST::Description>(source)->get_definitions();
        int ret = 0;
        if(definitions) {
            for(const AST::Node::Ptr &def : *definitions) {
                ret += resolve_source(def);
            }
        }
        return ret;
    }

    case AST::NodeType::Module:
        return resolve_module(AST::cast_to<AST::Module>(source));

    case AST::NodeType::Interface:
        return resolve_interface(AST::cast_to<AST::Interface>(source));

    default:
        return 0;
    }
}

int DefaultResolution::resolve_module(const AST::Module::Ptr &module)
{
    return resolve_module_like(module->get_params(), module->get_ports(), module->get_items(),
                               module->get_default_nettype(), module->get_lifetime());
}

int DefaultResolution::resolve_interface(const AST::Interface::Ptr &interface)
{
    return resolve_module_like(interface->get_params(), interface->get_ports(),
                               interface->get_items(),
                               to_module_nettype(interface->get_default_nettype()),
                               to_module_lifetime(interface->get_lifetime()));
}

int DefaultResolution::resolve_module_like(const AST::Declaration::ListPtr &params,
                                           const AST::Port::ListPtr &ports,
                                           const AST::Node::ListPtr &items,
                                           AST::Module::Default_nettypeEnum defnt,
                                           AST::Module::LifetimeEnum lifetime)
{
    int ret = 0;
    static const std::set<std::string> no_names;

    // §23.2.2.3 over the ANSI header: directions first (the kind rules
    // depend on them), then each port's kind and declaration subtree. A
    // header port is its signal's only declaration, so no backed-name
    // exemption applies here.
    if(ports) {
        resolve_port_directions(ports);
        for(const AST::Port::Ptr &port : *ports) {
            ret += resolve_port_kind(port, defnt, no_names);
            const AST::Node::ListPtr children = port->get_children();
            for(const AST::Node::Ptr &child : *children) {
                ret += resolve_body(child, defnt, no_names, lifetime);
            }
        }
    }

    if(params) {
        for(const AST::Declaration::Ptr &param : *params) {
            ret += resolve_body(param, defnt, no_names, lifetime);
        }
    }

    if(items) {
        const std::set<std::string> declared = declared_signal_names(items);
        for(const AST::Node::Ptr &item : *items) {
            ret += resolve_body(item, defnt, declared, lifetime);
        }
    }
    return ret;
}

void DefaultResolution::resolve_port_directions(const AST::Port::ListPtr &ports)
{
    AST::Port::DirectionEnum prev = AST::Port::DirectionEnum::NONE;

    for(const AST::Port::Ptr &port : *ports) {
        const AST::Declaration::Ptr decl = port->get_decl();
        if(!decl) {
            // Non-ANSI header reference: the whole list is non-ANSI and
            // §23.2.2.3's ANSI defaults do not apply.
            continue;
        }
        if(decl->is_node_type(AST::NodeType::Arg)) {
            // Interface port (or a still-deferred named-type port):
            // legitimately directionless, and a fresh inheritance group —
            // there is no direction to inherit across it.
            prev = AST::Port::DirectionEnum::NONE;
            continue;
        }
        if(port->get_direction() == AST::Port::DirectionEnum::NONE) {
            // First port (or first after a fresh group): §23.2.2.3 defaults
            // the omitted direction to inout; otherwise it is inherited.
            port->set_direction(
                prev != AST::Port::DirectionEnum::NONE ? prev : AST::Port::DirectionEnum::INOUT);
        }
        prev = port->get_direction();
    }
}

int DefaultResolution::resolve_port_kind(const AST::Port::Ptr &port,
                                         AST::Module::Default_nettypeEnum defnt,
                                         const std::set<std::string> &declared)
{
    const AST::Declaration::Ptr decl = port->get_decl();
    if(!decl || !decl->is_node_type(AST::NodeType::ImplicitNet)) {
        // No declaration (non-ANSI reference), an interface/deferred port
        // (Arg), or an explicitly declared kind: nothing to default.
        return 0;
    }
    if(declared.count(decl->get_name())) {
        // Body direction declaration backed by a standalone declaration of
        // the same name: a reference, not an implicit-net creation. The
        // ModuleIONormalizer merges the pair later.
        return 0;
    }

    const AST::Net::Ptr implicit_net = AST::cast_to<AST::Net>(decl);
    const AST::DataType::Ptr type = implicit_net->get_type();

    // §23.2.2.3 kind rules, shared with every other consumer that must
    // tell a variable from a net (Analysis::StorageKind): a ref port is
    // always a variable, an output is one when its data type was written
    // with the explicit data_type syntax, every other omitted kind is a
    // net of the default net type.
    if(Analysis::StorageKind::port_is_variable(port)) {
        auto var = std::make_shared<AST::Var>(decl->get_filename(), decl->get_line());
        var->set_name(implicit_net->get_name());
        var->set_type(type);
        var->set_unpacked_dims(implicit_net->get_unpacked_dims());
        var->set_init(implicit_net->get_cont_assign());
        port->set_decl(var);
        return 0;
    }

    AST::Net::Ptr net = make_default_nettype_net(defnt, implicit_net);
    if(!net) {
        LOG_ERROR_N(port) << "implicit net '" << implicit_net->get_name()
                          << "' but `default_nettype none is in effect";
        return 1;
    }
    port->set_decl(net);
    return 0;
}

int DefaultResolution::resolve_body(const AST::Node::Ptr &node,
                                    AST::Module::Default_nettypeEnum defnt,
                                    const std::set<std::string> &declared,
                                    AST::Module::LifetimeEnum lifetime, bool in_subroutine)
{
    if(!node) {
        return 0;
    }

    int ret = 0;

    // A Port met in the body is a non-ANSI direction declaration — but only
    // at module level. Inside a task/function the same Port shape is an
    // old-style formal, which is a variable (§13.3/§13.4), never a net: the
    // §23.2.2.3 port-kind rules do not apply and the formal stays as parsed.
    if(node->is_node_type(AST::NodeType::Port) && !in_subroutine) {
        ret += resolve_port_kind(AST::cast_to<AST::Port>(node), defnt, declared);
    }

    if(m_sv_mode) {
        if(node->is_node_category(AST::NodeType::Declaration)) {
            resolve_decl_type(AST::cast_to<AST::Declaration>(node));
        } else if(node->is_node_type(AST::NodeType::Function)) {
            const auto &function = AST::cast_to<AST::Function>(node);
            const AST::DataType::Ptr rettype = function->get_return_type();
            if(rettype && rettype->is_node_type(AST::NodeType::ImplicitType)) {
                function->set_return_type(make_logic_type(rettype));
            }
        }
    }

    const bool subroutine = in_subroutine || node->is_node_type(AST::NodeType::Function) ||
                            node->is_node_type(AST::NodeType::Task);
    if(m_sv_mode && !in_subroutine && subroutine) {
        // Outermost subroutine of this declaration: it inherits the enclosing
        // lifetime. A nested one (in_subroutine) already sits under a resolved
        // parent — SV has no nested subroutine declarations anyway.
        // SV mode only, like the type materialization (§2.1): 1364-2005 has
        // no subroutine `static` keyword (A.2.6 is `task [automatic]`), so
        // stamping one would make the output stop being Verilog.
        resolve_subroutine_lifetime(node, lifetime);
    }
    const AST::Node::ListPtr children = node->get_children();
    for(const AST::Node::Ptr &child : *children) {
        ret += resolve_body(child, defnt, declared, lifetime, subroutine);
    }
    return ret;
}

void DefaultResolution::resolve_decl_type(const AST::Declaration::Ptr &decl)
{
    if(decl->is_node_type(AST::NodeType::ImplicitNet)) {
        // Still-implicit net: a backed body direction declaration (kept as
        // a reference on purpose) or a not-yet-resolved deferral — turning
        // its type explicit would misrepresent it as a full declaration.
        return;
    }

    const AST::DataType::Ptr type = decl->get_type();
    if(!type || !type->is_node_type(AST::NodeType::ImplicitType)) {
        return;
    }

    if(decl->is_node_type(AST::NodeType::Param)) {
        // §6.20.2: only a ranged implicit parameter type is fixed by its
        // declaration (an unsigned logic vector of that range). A
        // range-less one takes the type and range of its final value —
        // resolving it here would fabricate a 1-bit type.
        const AST::Dimension::ListPtr dims = type->get_packed_dims();
        if(!dims || dims->empty()) {
            return;
        }
    }

    decl->set_type(make_logic_type(type));
}

void DefaultResolution::resolve_subroutine_lifetime(const AST::Node::Ptr &node,
                                                    AST::Module::LifetimeEnum lifetime)
{
    // §13.3.1/§13.4.2: a subroutine defaults to static; it is automatic when
    // it says so, or when the enclosing declaration does. An enclosing NONE
    // is itself static, so the effective value is always one of the two.
    const bool enclosing_auto = lifetime == AST::Module::LifetimeEnum::AUTOMATIC;

    if(node->is_node_type(AST::NodeType::Function)) {
        const auto &function = AST::cast_to<AST::Function>(node);
        if(function->get_lifetime() == AST::Function::LifetimeEnum::NONE) {
            function->set_lifetime(enclosing_auto ? AST::Function::LifetimeEnum::AUTOMATIC
                                                  : AST::Function::LifetimeEnum::STATIC);
        }
        return;
    }

    const auto &task = AST::cast_to<AST::Task>(node);
    if(task->get_lifetime() == AST::Task::LifetimeEnum::NONE) {
        task->set_lifetime(enclosing_auto ? AST::Task::LifetimeEnum::AUTOMATIC
                                          : AST::Task::LifetimeEnum::STATIC);
    }
}

std::set<std::string> DefaultResolution::declared_signal_names(const AST::Node::ListPtr &items)
{
    std::set<std::string> names;
    if(!items) {
        return names;
    }

    for(const AST::Node::Ptr &item : *items) {
        Analysis::for_each_standalone_decl(
            item, nullptr,
            [&names](const AST::Declaration::Ptr &decl, const AST::Node::Ptr & /*parent*/) {
                names.insert(decl->get_name());
            });
    }
    return names;
}

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse
