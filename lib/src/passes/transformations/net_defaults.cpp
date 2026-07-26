// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/passes/transformations/net_defaults.hpp>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

AST::Net::Ptr make_default_nettype_net(AST::Module::Default_nettypeEnum defnt,
                                       const std::string &filename, uint32_t line)
{
    switch(defnt) {
    case AST::Module::Default_nettypeEnum::WIRE:
        return std::make_shared<AST::WireNet>(filename, line);
    case AST::Module::Default_nettypeEnum::TRI:
        return std::make_shared<AST::TriNet>(filename, line);
    case AST::Module::Default_nettypeEnum::TRI0:
        return std::make_shared<AST::Tri0Net>(filename, line);
    case AST::Module::Default_nettypeEnum::TRI1:
        return std::make_shared<AST::Tri1Net>(filename, line);
    case AST::Module::Default_nettypeEnum::TRIAND:
        return std::make_shared<AST::TriandNet>(filename, line);
    case AST::Module::Default_nettypeEnum::TRIOR:
        return std::make_shared<AST::TriorNet>(filename, line);
    case AST::Module::Default_nettypeEnum::TRIREG:
        return std::make_shared<AST::TriregNet>(filename, line);
    case AST::Module::Default_nettypeEnum::WAND:
        return std::make_shared<AST::WandNet>(filename, line);
    case AST::Module::Default_nettypeEnum::WOR:
        return std::make_shared<AST::WorNet>(filename, line);
    case AST::Module::Default_nettypeEnum::UWIRE:
        return std::make_shared<AST::UwireNet>(filename, line);
    case AST::Module::Default_nettypeEnum::SUPPLY0:
        return std::make_shared<AST::Supply0Net>(filename, line);
    case AST::Module::Default_nettypeEnum::SUPPLY1:
        return std::make_shared<AST::Supply1Net>(filename, line);
    case AST::Module::Default_nettypeEnum::NONE:
        return nullptr;
    }
    return nullptr;
}

AST::Net::Ptr make_default_nettype_net(AST::Module::Default_nettypeEnum defnt,
                                       const AST::Net::Ptr &src)
{
    AST::Net::Ptr net = make_default_nettype_net(defnt, src->get_filename(), src->get_line());
    if(!net) {
        return nullptr;
    }
    // Declaration fields.
    net->set_name(src->get_name());
    net->set_type(src->get_type());
    net->set_unpacked_dims(src->get_unpacked_dims());
    // Net fields.
    net->set_cont_assign(src->get_cont_assign());
    net->set_strength(src->get_strength());
    net->set_ldelay(src->get_ldelay());
    net->set_rdelay(src->get_rdelay());
    net->set_is_vectored(src->get_is_vectored());
    net->set_is_scalared(src->get_is_scalared());
    return net;
}

// The generated per-node enums are identical value lists that astgen cannot
// share (ADR-0002 §2.1); this switch bridges them, like the parser's
// to_interface_nettype does in the other direction. It is deliberately
// exhaustive with no `default`: a new enumerator added to the yaml makes the
// switch non-exhaustive, which -Wall -Werror turns into a compile error at
// this exact spot rather than silently mapping the new value to NONE.
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

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse
