// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_APPS_FSM_MARK_HPP
#define VERIPARSE_APPS_FSM_MARK_HPP

#include <veriparse/AST/nodes.hpp>
#include <veriparse/AST/node_cast.hpp>

/**
 * @brief Whether a module holds a (* veriparse_fsm *) marked process
 * (ADR-0014 §2).
 */
inline bool has_veriparse_fsm_mark(const Veriparse::AST::Node::Ptr &module)
{
    const auto &items = Veriparse::AST::cast_to<Veriparse::AST::Module>(module)->get_items();
    if(!items) {
        return false;
    }
    for(const auto &item : *items) {
        if(!item || !item->is_node_type(Veriparse::AST::NodeType::Pragmalist)) {
            continue;
        }
        const auto &pragmas =
            Veriparse::AST::cast_to<Veriparse::AST::Pragmalist>(item)->get_pragmas();
        if(!pragmas) {
            continue;
        }
        for(const auto &pragma : *pragmas) {
            if(pragma && pragma->get_name() == "veriparse_fsm") {
                return true;
            }
        }
    }
    return false;
}

#endif
