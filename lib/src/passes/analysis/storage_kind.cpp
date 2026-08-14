// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/passes/analysis/storage_kind.hpp>
#include <veriparse/AST/node_cast.hpp>

namespace Veriparse
{
namespace Passes
{
namespace Analysis
{

bool StorageKind::port_is_variable(const AST::Port::Ptr &port)
{
    const auto &decl = port->get_decl();
    if(decl && decl->is_node_type(AST::NodeType::Var)) {
        return true;
    }
    const AST::Port::DirectionEnum dir = port->get_direction();
    if(dir == AST::Port::DirectionEnum::REF || dir == AST::Port::DirectionEnum::CONST_REF) {
        return true;
    }
    if(dir == AST::Port::DirectionEnum::OUTPUT && decl &&
       decl->is_node_type(AST::NodeType::ImplicitNet)) {
        const auto &type = AST::cast_to<AST::ImplicitNet>(decl)->get_type();
        return type && !type->is_node_type(AST::NodeType::ImplicitType);
    }
    return false;
}

bool StorageKind::is_net(const AST::Module::Ptr &module, const std::string &name)
{
    const auto port_verdict = [&name](const AST::Port::Ptr &port) -> int {
        if(port->get_name() != name) {
            return -1;
        }
        const auto &decl = port->get_decl();
        if(!decl && port->get_direction() == AST::Port::DirectionEnum::NONE) {
            return -1; // non-ANSI header reference: the body declaration decides
        }
        if(port_is_variable(port)) {
            return 0;
        }
        switch(port->get_direction()) {
        case AST::Port::DirectionEnum::INPUT:
        case AST::Port::DirectionEnum::INOUT:
            return 1;
        case AST::Port::DirectionEnum::OUTPUT:
            // The explicit-data-type output classified variable above; an
            // implicit one, or a resolved net declaration, is the net kind.
            if(decl && decl->is_node_type(AST::NodeType::ImplicitNet)) {
                return 1;
            }
            return decl && decl->is_node_category(AST::NodeType::Net) ? 1 : 0;
        default:
            return 0;
        }
    };
    const auto &ports = module->get_ports();
    if(ports) {
        for(const auto &port : *ports) {
            if(!port->is_node_type(AST::NodeType::Port)) {
                continue;
            }
            const int verdict = port_verdict(AST::cast_to<AST::Port>(port));
            if(verdict >= 0) {
                return verdict == 1;
            }
        }
    }
    const auto &items = module->get_items();
    if(items) {
        for(const auto &item : *items) {
            if(item->is_node_type(AST::NodeType::Port)) {
                const int verdict = port_verdict(AST::cast_to<AST::Port>(item));
                if(verdict >= 0) {
                    return verdict == 1;
                }
                continue;
            }
            if(item->is_node_category(AST::NodeType::Net) &&
               AST::cast_to<AST::Net>(item)->get_name() == name) {
                return true;
            }
        }
    }
    return false;
}

AST::Identifier::Ptr StorageKind::select_base(const AST::Node::Ptr &node)
{
    AST::Node::Ptr cur = node;
    while(cur && cur->is_node_category(AST::NodeType::Indirect)) {
        cur = AST::cast_to<AST::Indirect>(cur)->get_var();
    }
    if(cur && cur->is_node_type(AST::NodeType::Identifier)) {
        return AST::cast_to<AST::Identifier>(cur);
    }
    return nullptr;
}

} // namespace Analysis
} // namespace Passes
} // namespace Veriparse
