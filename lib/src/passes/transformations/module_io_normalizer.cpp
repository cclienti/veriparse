// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/passes/transformations/module_io_normalizer.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/misc/string_utils.hpp>
#include <veriparse/logger/logger.hpp>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

namespace
{
// Effective signal name of a port: its own name, or the name carried by its
// inner declaration (ANSI typed port).
std::string port_name(const AST::Port::Ptr &p)
{
    if(!p->get_name().empty()) {
        return p->get_name();
    }
    if(p->get_decl()) {
        return p->get_decl()->get_name();
    }
    return std::string();
}

// A bare port declaration with neither a net keyword nor a data type written
// (`input x;`, `input [7:0] x;`): an ImplicitNet whose data type is ImplicitType.
// Such a port resolves to the module default net type.
bool needs_default_nettype(const AST::Declaration::Ptr &decl)
{
    if(!decl || !decl->is_node_type(AST::NodeType::ImplicitNet)) {
        return false;
    }
    const AST::DataType::Ptr type = decl->get_type();
    return type && type->is_node_type(AST::NodeType::ImplicitType);
}
} // namespace

int ModuleIONormalizer::process(AST::Node::Ptr node, AST::Node::Ptr parent)
{
    AST::Module::ListPtr module_nodes = Analysis::Module::get_module_nodes(node);

    for(AST::Module::Ptr &module : *module_nodes) {
        // Hoist overridable parameters — value and type (§6.20.1) — declared
        // in the body up to the module parameter list, then drop them from
        // the body. Localparams (value and type) keep their body position.
        AST::Declaration::ListPtr parameter_nodes =
            Analysis::Module::get_parameter_decl_nodes(module);
        remove_module_parameters(module);

        if(parameter_nodes->empty()) {
            module->set_params(nullptr);
        } else {
            module->set_params(parameter_nodes);
        }

        // Body direction declarations (non-ANSI ports). ANSI ports already live
        // in `module->get_ports()` fully typed, so they are left untouched: only
        // ports declared in the body need normalizing.
        AST::Port::ListPtr dir_ports = std::make_shared<AST::Port::List>();
        if(module->get_items()) {
            for(const AST::Node::Ptr &item : *module->get_items()) {
                if(item && item->is_node_type(AST::NodeType::Port)) {
                    AST::Port::Ptr p = AST::cast_to<AST::Port>(item);
                    if(p->get_direction() != AST::Port::DirectionEnum::NONE) {
                        dir_ports->push_back(p);
                    }
                }
            }
        }

        if(dir_ports->empty()) {
            continue;
        }

        // Collect (and detach) the standalone body declarations that back a port
        // (`wire x;`, `reg [..] x;`); port-internal declarations are skipped.
        AST::Declaration::ListPtr removed_decls = std::make_shared<AST::Declaration::List>();
        for(const AST::Port::Ptr &p : *dir_ports) {
            remove_module_variable(port_name(p), removed_decls, module);
        }

        // Remove every Port node (the body direction declarations and the
        // non-ANSI header references); the normalized list is rebuilt below.
        remove_module_ports(module);

        // Construct the normalized ports: one Port per body direction
        // declaration, carrying the matching net/var declaration.
        AST::Port::ListPtr ports = std::make_shared<AST::Port::List>();
        for(const AST::Port::Ptr &dir : *dir_ports) {
            const std::string name = port_name(dir);

            AST::Port::Ptr port = std::make_shared<AST::Port>(dir->get_filename(), dir->get_line());
            port->set_direction(dir->get_direction());
            port->set_name(name);

            // Prefer a standalone body declaration of the same name.
            AST::Declaration::Ptr decl;
            for(auto it = removed_decls->begin(); it != removed_decls->end(); ++it) {
                if((*it)->get_name() == name) {
                    decl = AST::cast_to<AST::Declaration>((*it)->clone());
                    removed_decls->erase(it);
                    break;
                }
            }

            // Otherwise reuse the port's own declaration, applying the module
            // default net type when the port is a bare (untyped) net.
            if(!decl) {
                const AST::Declaration::Ptr own = dir->get_decl();
                if(needs_default_nettype(own)) {
                    const AST::DataType::Ptr type = own->get_type();
                    decl = create_default_net_type_variable(
                        module->get_default_nettype(),
                        AST::Dimension::clone_list(type->get_packed_dims()), type->get_signing(),
                        name);
                    if(!decl) {
                        LOG_ERROR << module << port
                                  << "implicit net type, but no default net type defined";
                    }
                } else if(own) {
                    decl = AST::cast_to<AST::Declaration>(own->clone());
                }
            }

            port->set_decl(decl);
            ports->push_back(port);
        }

        module->set_ports(ports->empty() ? nullptr : ports);
    }

    return 0;
}

void ModuleIONormalizer::remove_module_parameters(AST::Node::Ptr node, AST::Node::Ptr parent)
{
    if(node) {
        const bool overridable_param = (node->is_node_type(AST::NodeType::Param) &&
                                        !AST::cast_to<AST::Param>(node)->get_is_local()) ||
                                       (node->is_node_type(AST::NodeType::TypeParam) &&
                                        !AST::cast_to<AST::TypeParam>(node)->get_is_local());
        if(overridable_param) {
            if(parent) {
                parent->remove(node);
            }
            return;
        }

        switch(node->get_node_type()) {
        case AST::NodeType::Function:
            break;
        case AST::NodeType::Task:
            break;

        default: {
            AST::Node::ListPtr children = node->get_children();
            for(AST::Node::Ptr &child : *children) {
                remove_module_parameters(child, node);
            }
        }
        }
    } else {
        LOG_ERROR << "Empty node, parent is " << parent;
    }
}

void ModuleIONormalizer::remove_module_ports(AST::Node::Ptr node, AST::Node::Ptr parent)
{
    if(node) {
        switch(node->get_node_type()) {
        case AST::NodeType::Port:
            if(parent) {
                parent->remove(node);
            }
            break;

        case AST::NodeType::Function:
            break;
        case AST::NodeType::Task:
            break;

        default: {
            AST::Node::ListPtr children = node->get_children();
            for(AST::Node::Ptr &child : *children) {
                remove_module_ports(child, node);
            }
        }
        }
    } else {
        LOG_ERROR << "Empty node, parent is " << parent;
    }
}

void ModuleIONormalizer::remove_module_variable(const std::string &variable,
                                                AST::Declaration::ListPtr removed_variables,
                                                AST::Node::Ptr node, AST::Node::Ptr parent)
{
    if(node) {
        // A standalone net/var declaration (Var or any Net): collect it. Port
        // wrappers are skipped below so their inner declarations are never picked
        // up here.
        if(node->is_node_type(AST::NodeType::Var) || node->is_node_category(AST::NodeType::Net)) {
            AST::Declaration::Ptr var = AST::cast_to<AST::Declaration>(node);
            if((var->get_name() == variable) && (parent)) {
                if(removed_variables) {
                    removed_variables->push_back(var);
                }
                parent->remove(node);
            }
        } else {
            switch(node->get_node_type()) {
            case AST::NodeType::Port:
                break;
            case AST::NodeType::Function:
                break;
            case AST::NodeType::Task:
                break;
            case AST::NodeType::Initial:
                break;
            case AST::NodeType::Always:
                break;
            case AST::NodeType::AlwaysFF:
                break;
            case AST::NodeType::AlwaysComb:
                break;
            case AST::NodeType::AlwaysLatch:
                break;

            default: {
                AST::Node::ListPtr children = node->get_children();
                for(AST::Node::Ptr &child : *children) {
                    remove_module_variable(variable, removed_variables, child, node);
                }
            }
            }
        }
    } else {
        LOG_ERROR << "Empty node, parent is " << parent;
    }
}

AST::Net::Ptr ModuleIONormalizer::create_default_net_type_variable(
    AST::Module::Default_nettypeEnum defnt, AST::Dimension::ListPtr packed_dims,
    AST::DataType::SigningEnum signing, const std::string &name)
{
    AST::Net::Ptr net;

    switch(defnt) {
    case AST::Module::Default_nettypeEnum::WIRE:
        net = std::make_shared<AST::WireNet>();
        break;
    case AST::Module::Default_nettypeEnum::TRI:
        net = std::make_shared<AST::TriNet>();
        break;
    case AST::Module::Default_nettypeEnum::TRI0:
        net = std::make_shared<AST::Tri0Net>();
        break;
    case AST::Module::Default_nettypeEnum::TRI1:
        net = std::make_shared<AST::Tri1Net>();
        break;
    case AST::Module::Default_nettypeEnum::TRIAND:
        net = std::make_shared<AST::TriandNet>();
        break;
    case AST::Module::Default_nettypeEnum::TRIOR:
        net = std::make_shared<AST::TriorNet>();
        break;
    case AST::Module::Default_nettypeEnum::TRIREG:
        net = std::make_shared<AST::TriregNet>();
        break;
    case AST::Module::Default_nettypeEnum::WAND:
        net = std::make_shared<AST::WandNet>();
        break;
    case AST::Module::Default_nettypeEnum::WOR:
        net = std::make_shared<AST::WorNet>();
        break;
    case AST::Module::Default_nettypeEnum::UWIRE:
        net = std::make_shared<AST::UwireNet>();
        break;
    case AST::Module::Default_nettypeEnum::SUPPLY0:
        net = std::make_shared<AST::Supply0Net>();
        break;
    case AST::Module::Default_nettypeEnum::SUPPLY1:
        net = std::make_shared<AST::Supply1Net>();
        break;

    case AST::Module::Default_nettypeEnum::NONE:
    default:
        break;
    }

    if(net) {
        auto type = std::make_shared<AST::ImplicitType>();
        type->set_signing(signing);
        if(packed_dims) {
            type->set_packed_dims(packed_dims);
        }
        net->set_type(type);
        net->set_name(name);
    }

    return net;
}

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse
