// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/passes/transformations/default_resolution.hpp>
#include <veriparse/passes/transformations/net_defaults.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>

#include <functional>

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
                               module->get_default_nettype());
}

int DefaultResolution::resolve_interface(const AST::Interface::Ptr &interface)
{
    return resolve_module_like(interface->get_params(), interface->get_ports(),
                               interface->get_items(),
                               to_module_nettype(interface->get_default_nettype()));
}

int DefaultResolution::resolve_module_like(const AST::Declaration::ListPtr &params,
                                           const AST::Port::ListPtr &ports,
                                           const AST::Node::ListPtr &items,
                                           AST::Module::Default_nettypeEnum defnt)
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
                ret += resolve_body(child, defnt, no_names);
            }
        }
    }

    if(params) {
        for(const AST::Declaration::Ptr &param : *params) {
            ret += resolve_body(param, defnt, no_names);
        }
    }

    if(items) {
        const std::set<std::string> declared = declared_signal_names(items);
        for(const AST::Node::Ptr &item : *items) {
            ret += resolve_body(item, defnt, declared);
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
    const bool implicit_type = !type || type->is_node_type(AST::NodeType::ImplicitType);
    const AST::Port::DirectionEnum dir = port->get_direction();

    // §23.2.2.3 kind rules: a ref port is always a variable; an output is
    // a variable when its data type was written with the explicit
    // data_type syntax; every other omitted kind is a net of the default
    // net type.
    const bool variable = (dir == AST::Port::DirectionEnum::REF) ||
                          (dir == AST::Port::DirectionEnum::CONST_REF) ||
                          (dir == AST::Port::DirectionEnum::OUTPUT && !implicit_type);

    if(variable) {
        auto var = std::make_shared<AST::Var>(decl->get_filename(), decl->get_line());
        var->set_name(implicit_net->get_name());
        var->set_type(type);
        var->set_unpacked_dims(implicit_net->get_unpacked_dims());
        var->set_init(implicit_net->get_cont_assign());
        port->set_decl(var);
        return 0;
    }

    AST::Net::Ptr net = make_default_nettype_net(defnt, decl->get_filename(), decl->get_line());
    if(!net) {
        LOG_ERROR_N(port) << "implicit net '" << implicit_net->get_name()
                          << "' but `default_nettype none is in effect";
        return 1;
    }
    net->set_name(implicit_net->get_name());
    net->set_type(type);
    net->set_unpacked_dims(implicit_net->get_unpacked_dims());
    net->set_cont_assign(implicit_net->get_cont_assign());
    net->set_strength(implicit_net->get_strength());
    net->set_ldelay(implicit_net->get_ldelay());
    net->set_rdelay(implicit_net->get_rdelay());
    net->set_is_vectored(implicit_net->get_is_vectored());
    net->set_is_scalared(implicit_net->get_is_scalared());
    port->set_decl(net);
    return 0;
}

int DefaultResolution::resolve_body(const AST::Node::Ptr &node,
                                    AST::Module::Default_nettypeEnum defnt,
                                    const std::set<std::string> &declared, bool in_subroutine)
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
    const AST::Node::ListPtr children = node->get_children();
    for(const AST::Node::Ptr &child : *children) {
        ret += resolve_body(child, defnt, declared, subroutine);
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

std::set<std::string> DefaultResolution::declared_signal_names(const AST::Node::ListPtr &items)
{
    std::set<std::string> names;
    if(!items) {
        return names;
    }

    // Same reach as the ModuleIONormalizer's declaration collector: a
    // standalone Var/Net anywhere a declaration statement may sit, but
    // never inside a Port wrapper (that is the direction declaration
    // itself), a subroutine, or a process.
    std::function<void(const AST::Node::Ptr &)> collect = [&names,
                                                           &collect](const AST::Node::Ptr &node) {
        if(!node) {
            return;
        }
        if(node->is_node_type(AST::NodeType::Var) || node->is_node_category(AST::NodeType::Net)) {
            names.insert(AST::cast_to<AST::Declaration>(node)->get_name());
            return;
        }
        switch(node->get_node_type()) {
        case AST::NodeType::Port:
        case AST::NodeType::Function:
        case AST::NodeType::Task:
        case AST::NodeType::Initial:
        case AST::NodeType::Always:
        case AST::NodeType::AlwaysFF:
        case AST::NodeType::AlwaysComb:
        case AST::NodeType::AlwaysLatch:
            return;
        default:
            break;
        }
        const AST::Node::ListPtr children = node->get_children();
        for(const AST::Node::Ptr &child : *children) {
            collect(child);
        }
    };

    for(const AST::Node::Ptr &item : *items) {
        collect(item);
    }
    return names;
}

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse
