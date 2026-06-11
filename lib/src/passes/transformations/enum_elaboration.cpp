// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/passes/transformations/enum_elaboration.hpp>
#include <veriparse/AST/enumtype.hpp>
#include <veriparse/AST/enumitem.hpp>
#include <veriparse/AST/intconstn.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>

#include <veriparse/misc/bignum.hpp>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

int EnumElaboration::process(AST::Node::Ptr node, AST::Node::Ptr parent)
{
    if(!node) {
        return 0;
    }

    if(node->get_node_type() == AST::NodeType::EnumType) {
        return elaborate_enumdef(AST::cast_to<AST::EnumType>(node));
    }

    return recurse_in_childs(node);
}

int EnumElaboration::elaborate_enumdef(const AST::EnumType::Ptr &enumdef)
{
    if(!enumdef) {
        return 0;
    }

    const auto &items = enumdef->get_items();
    if(!items) {
        return 0;
    }

    mpz_class counter(0);

    for(const AST::EnumItem::Ptr &item : *items) {
        if(!item) {
            continue;
        }

        const auto &val = item->get_value();

        if(val) {
            // Explicit value — must be an IntConstN after ConstantFolding
            if(val->get_node_type() != AST::NodeType::IntConstN) {
                LOG_ERROR_N(item) << "enum item '" << item->get_name()
                                  << "': explicit value is not a constant after folding";
                return 1;
            }
            counter = AST::cast_to<AST::IntConstN>(val)->get_value();
        } else {
            // Auto-increment — inject IntConstN
            auto injected = std::make_shared<AST::IntConstN>(
                10, -1, false, counter, item->get_filename(), item->get_line());
            item->set_value(injected);
        }

        counter += 1;
    }

    return 0;
}

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse
