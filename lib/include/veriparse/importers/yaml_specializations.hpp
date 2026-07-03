// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_GENERATORS_YAML_SPECIALIZATION_HPP
#define VERIPARSE_GENERATORS_YAML_SPECIALIZATION_HPP

#include <veriparse/misc/bignum.hpp>
#include <yaml-cpp/yaml.h>
#include <sstream>

namespace YAML
{
template <> struct convert<Veriparse::AST::Module::LifetimeEnum>
{
    static Node encode(const Veriparse::AST::Module::LifetimeEnum &rhs)
    {
        switch(rhs) {
        case Veriparse::AST::Module::LifetimeEnum::NONE:
            return Node("NONE");
        case Veriparse::AST::Module::LifetimeEnum::AUTOMATIC:
            return Node("AUTOMATIC");
        default:
            return Node("STATIC");
        }
    }

    static bool decode(const Node &node, Veriparse::AST::Module::LifetimeEnum &rhs)
    {
        if(!node.IsScalar()) {
            return false;
        }

        if(node.as<std::string>() == "NONE") {
            rhs = Veriparse::AST::Module::LifetimeEnum::NONE;
            return true;
        }

        if(node.as<std::string>() == "AUTOMATIC") {
            rhs = Veriparse::AST::Module::LifetimeEnum::AUTOMATIC;
            return true;
        }

        if(node.as<std::string>() == "STATIC") {
            rhs = Veriparse::AST::Module::LifetimeEnum::STATIC;
            return true;
        }

        return false;
    }
};
template <> struct convert<Veriparse::AST::Module::Default_nettypeEnum>
{
    static Node encode(const Veriparse::AST::Module::Default_nettypeEnum &rhs)
    {
        switch(rhs) {
        case Veriparse::AST::Module::Default_nettypeEnum::WIRE:
            return Node("WIRE");
        case Veriparse::AST::Module::Default_nettypeEnum::TRI:
            return Node("TRI");
        case Veriparse::AST::Module::Default_nettypeEnum::TRI0:
            return Node("TRI0");
        case Veriparse::AST::Module::Default_nettypeEnum::TRI1:
            return Node("TRI1");
        case Veriparse::AST::Module::Default_nettypeEnum::TRIAND:
            return Node("TRIAND");
        case Veriparse::AST::Module::Default_nettypeEnum::TRIOR:
            return Node("TRIOR");
        case Veriparse::AST::Module::Default_nettypeEnum::TRIREG:
            return Node("TRIREG");
        case Veriparse::AST::Module::Default_nettypeEnum::WAND:
            return Node("WAND");
        case Veriparse::AST::Module::Default_nettypeEnum::WOR:
            return Node("WOR");
        case Veriparse::AST::Module::Default_nettypeEnum::UWIRE:
            return Node("UWIRE");
        case Veriparse::AST::Module::Default_nettypeEnum::SUPPLY0:
            return Node("SUPPLY0");
        case Veriparse::AST::Module::Default_nettypeEnum::SUPPLY1:
            return Node("SUPPLY1");
        default:
            return Node("NONE");
        }
    }

    static bool decode(const Node &node, Veriparse::AST::Module::Default_nettypeEnum &rhs)
    {
        if(!node.IsScalar()) {
            return false;
        }

        if(node.as<std::string>() == "WIRE") {
            rhs = Veriparse::AST::Module::Default_nettypeEnum::WIRE;
            return true;
        }

        if(node.as<std::string>() == "TRI") {
            rhs = Veriparse::AST::Module::Default_nettypeEnum::TRI;
            return true;
        }

        if(node.as<std::string>() == "TRI0") {
            rhs = Veriparse::AST::Module::Default_nettypeEnum::TRI0;
            return true;
        }

        if(node.as<std::string>() == "TRI1") {
            rhs = Veriparse::AST::Module::Default_nettypeEnum::TRI1;
            return true;
        }

        if(node.as<std::string>() == "TRIAND") {
            rhs = Veriparse::AST::Module::Default_nettypeEnum::TRIAND;
            return true;
        }

        if(node.as<std::string>() == "TRIOR") {
            rhs = Veriparse::AST::Module::Default_nettypeEnum::TRIOR;
            return true;
        }

        if(node.as<std::string>() == "TRIREG") {
            rhs = Veriparse::AST::Module::Default_nettypeEnum::TRIREG;
            return true;
        }

        if(node.as<std::string>() == "WAND") {
            rhs = Veriparse::AST::Module::Default_nettypeEnum::WAND;
            return true;
        }

        if(node.as<std::string>() == "WOR") {
            rhs = Veriparse::AST::Module::Default_nettypeEnum::WOR;
            return true;
        }

        if(node.as<std::string>() == "UWIRE") {
            rhs = Veriparse::AST::Module::Default_nettypeEnum::UWIRE;
            return true;
        }

        if(node.as<std::string>() == "SUPPLY0") {
            rhs = Veriparse::AST::Module::Default_nettypeEnum::SUPPLY0;
            return true;
        }

        if(node.as<std::string>() == "SUPPLY1") {
            rhs = Veriparse::AST::Module::Default_nettypeEnum::SUPPLY1;
            return true;
        }

        if(node.as<std::string>() == "NONE") {
            rhs = Veriparse::AST::Module::Default_nettypeEnum::NONE;
            return true;
        }

        return false;
    }
};
template <> struct convert<Veriparse::AST::Port::DirectionEnum>
{
    static Node encode(const Veriparse::AST::Port::DirectionEnum &rhs)
    {
        switch(rhs) {
        case Veriparse::AST::Port::DirectionEnum::NONE:
            return Node("NONE");
        case Veriparse::AST::Port::DirectionEnum::INPUT:
            return Node("INPUT");
        case Veriparse::AST::Port::DirectionEnum::OUTPUT:
            return Node("OUTPUT");
        case Veriparse::AST::Port::DirectionEnum::INOUT:
            return Node("INOUT");
        case Veriparse::AST::Port::DirectionEnum::REF:
            return Node("REF");
        default:
            return Node("CONST_REF");
        }
    }

    static bool decode(const Node &node, Veriparse::AST::Port::DirectionEnum &rhs)
    {
        if(!node.IsScalar()) {
            return false;
        }

        if(node.as<std::string>() == "NONE") {
            rhs = Veriparse::AST::Port::DirectionEnum::NONE;
            return true;
        }

        if(node.as<std::string>() == "INPUT") {
            rhs = Veriparse::AST::Port::DirectionEnum::INPUT;
            return true;
        }

        if(node.as<std::string>() == "OUTPUT") {
            rhs = Veriparse::AST::Port::DirectionEnum::OUTPUT;
            return true;
        }

        if(node.as<std::string>() == "INOUT") {
            rhs = Veriparse::AST::Port::DirectionEnum::INOUT;
            return true;
        }

        if(node.as<std::string>() == "REF") {
            rhs = Veriparse::AST::Port::DirectionEnum::REF;
            return true;
        }

        if(node.as<std::string>() == "CONST_REF") {
            rhs = Veriparse::AST::Port::DirectionEnum::CONST_REF;
            return true;
        }

        return false;
    }
};
template <> struct convert<Veriparse::AST::Interface::LifetimeEnum>
{
    static Node encode(const Veriparse::AST::Interface::LifetimeEnum &rhs)
    {
        switch(rhs) {
        case Veriparse::AST::Interface::LifetimeEnum::NONE:
            return Node("NONE");
        case Veriparse::AST::Interface::LifetimeEnum::AUTOMATIC:
            return Node("AUTOMATIC");
        default:
            return Node("STATIC");
        }
    }

    static bool decode(const Node &node, Veriparse::AST::Interface::LifetimeEnum &rhs)
    {
        if(!node.IsScalar()) {
            return false;
        }

        if(node.as<std::string>() == "NONE") {
            rhs = Veriparse::AST::Interface::LifetimeEnum::NONE;
            return true;
        }

        if(node.as<std::string>() == "AUTOMATIC") {
            rhs = Veriparse::AST::Interface::LifetimeEnum::AUTOMATIC;
            return true;
        }

        if(node.as<std::string>() == "STATIC") {
            rhs = Veriparse::AST::Interface::LifetimeEnum::STATIC;
            return true;
        }

        return false;
    }
};
template <> struct convert<Veriparse::AST::Interface::Default_nettypeEnum>
{
    static Node encode(const Veriparse::AST::Interface::Default_nettypeEnum &rhs)
    {
        switch(rhs) {
        case Veriparse::AST::Interface::Default_nettypeEnum::WIRE:
            return Node("WIRE");
        case Veriparse::AST::Interface::Default_nettypeEnum::TRI:
            return Node("TRI");
        case Veriparse::AST::Interface::Default_nettypeEnum::TRI0:
            return Node("TRI0");
        case Veriparse::AST::Interface::Default_nettypeEnum::TRI1:
            return Node("TRI1");
        case Veriparse::AST::Interface::Default_nettypeEnum::TRIAND:
            return Node("TRIAND");
        case Veriparse::AST::Interface::Default_nettypeEnum::TRIOR:
            return Node("TRIOR");
        case Veriparse::AST::Interface::Default_nettypeEnum::TRIREG:
            return Node("TRIREG");
        case Veriparse::AST::Interface::Default_nettypeEnum::WAND:
            return Node("WAND");
        case Veriparse::AST::Interface::Default_nettypeEnum::WOR:
            return Node("WOR");
        case Veriparse::AST::Interface::Default_nettypeEnum::UWIRE:
            return Node("UWIRE");
        case Veriparse::AST::Interface::Default_nettypeEnum::SUPPLY0:
            return Node("SUPPLY0");
        case Veriparse::AST::Interface::Default_nettypeEnum::SUPPLY1:
            return Node("SUPPLY1");
        default:
            return Node("NONE");
        }
    }

    static bool decode(const Node &node, Veriparse::AST::Interface::Default_nettypeEnum &rhs)
    {
        if(!node.IsScalar()) {
            return false;
        }

        if(node.as<std::string>() == "WIRE") {
            rhs = Veriparse::AST::Interface::Default_nettypeEnum::WIRE;
            return true;
        }

        if(node.as<std::string>() == "TRI") {
            rhs = Veriparse::AST::Interface::Default_nettypeEnum::TRI;
            return true;
        }

        if(node.as<std::string>() == "TRI0") {
            rhs = Veriparse::AST::Interface::Default_nettypeEnum::TRI0;
            return true;
        }

        if(node.as<std::string>() == "TRI1") {
            rhs = Veriparse::AST::Interface::Default_nettypeEnum::TRI1;
            return true;
        }

        if(node.as<std::string>() == "TRIAND") {
            rhs = Veriparse::AST::Interface::Default_nettypeEnum::TRIAND;
            return true;
        }

        if(node.as<std::string>() == "TRIOR") {
            rhs = Veriparse::AST::Interface::Default_nettypeEnum::TRIOR;
            return true;
        }

        if(node.as<std::string>() == "TRIREG") {
            rhs = Veriparse::AST::Interface::Default_nettypeEnum::TRIREG;
            return true;
        }

        if(node.as<std::string>() == "WAND") {
            rhs = Veriparse::AST::Interface::Default_nettypeEnum::WAND;
            return true;
        }

        if(node.as<std::string>() == "WOR") {
            rhs = Veriparse::AST::Interface::Default_nettypeEnum::WOR;
            return true;
        }

        if(node.as<std::string>() == "UWIRE") {
            rhs = Veriparse::AST::Interface::Default_nettypeEnum::UWIRE;
            return true;
        }

        if(node.as<std::string>() == "SUPPLY0") {
            rhs = Veriparse::AST::Interface::Default_nettypeEnum::SUPPLY0;
            return true;
        }

        if(node.as<std::string>() == "SUPPLY1") {
            rhs = Veriparse::AST::Interface::Default_nettypeEnum::SUPPLY1;
            return true;
        }

        if(node.as<std::string>() == "NONE") {
            rhs = Veriparse::AST::Interface::Default_nettypeEnum::NONE;
            return true;
        }

        return false;
    }
};
template <> struct convert<Veriparse::AST::ModportPort::DirectionEnum>
{
    static Node encode(const Veriparse::AST::ModportPort::DirectionEnum &rhs)
    {
        switch(rhs) {
        case Veriparse::AST::ModportPort::DirectionEnum::INPUT:
            return Node("INPUT");
        case Veriparse::AST::ModportPort::DirectionEnum::OUTPUT:
            return Node("OUTPUT");
        case Veriparse::AST::ModportPort::DirectionEnum::INOUT:
            return Node("INOUT");
        default:
            return Node("REF");
        }
    }

    static bool decode(const Node &node, Veriparse::AST::ModportPort::DirectionEnum &rhs)
    {
        if(!node.IsScalar()) {
            return false;
        }

        if(node.as<std::string>() == "INPUT") {
            rhs = Veriparse::AST::ModportPort::DirectionEnum::INPUT;
            return true;
        }

        if(node.as<std::string>() == "OUTPUT") {
            rhs = Veriparse::AST::ModportPort::DirectionEnum::OUTPUT;
            return true;
        }

        if(node.as<std::string>() == "INOUT") {
            rhs = Veriparse::AST::ModportPort::DirectionEnum::INOUT;
            return true;
        }

        if(node.as<std::string>() == "REF") {
            rhs = Veriparse::AST::ModportPort::DirectionEnum::REF;
            return true;
        }

        return false;
    }
};
template <> struct convert<Veriparse::AST::Package::LifetimeEnum>
{
    static Node encode(const Veriparse::AST::Package::LifetimeEnum &rhs)
    {
        switch(rhs) {
        case Veriparse::AST::Package::LifetimeEnum::NONE:
            return Node("NONE");
        case Veriparse::AST::Package::LifetimeEnum::AUTOMATIC:
            return Node("AUTOMATIC");
        default:
            return Node("STATIC");
        }
    }

    static bool decode(const Node &node, Veriparse::AST::Package::LifetimeEnum &rhs)
    {
        if(!node.IsScalar()) {
            return false;
        }

        if(node.as<std::string>() == "NONE") {
            rhs = Veriparse::AST::Package::LifetimeEnum::NONE;
            return true;
        }

        if(node.as<std::string>() == "AUTOMATIC") {
            rhs = Veriparse::AST::Package::LifetimeEnum::AUTOMATIC;
            return true;
        }

        if(node.as<std::string>() == "STATIC") {
            rhs = Veriparse::AST::Package::LifetimeEnum::STATIC;
            return true;
        }

        return false;
    }
};
template <> struct convert<Veriparse::AST::DataType::SigningEnum>
{
    static Node encode(const Veriparse::AST::DataType::SigningEnum &rhs)
    {
        switch(rhs) {
        case Veriparse::AST::DataType::SigningEnum::NONE:
            return Node("NONE");
        case Veriparse::AST::DataType::SigningEnum::SIGNED:
            return Node("SIGNED");
        default:
            return Node("UNSIGNED");
        }
    }

    static bool decode(const Node &node, Veriparse::AST::DataType::SigningEnum &rhs)
    {
        if(!node.IsScalar()) {
            return false;
        }

        if(node.as<std::string>() == "NONE") {
            rhs = Veriparse::AST::DataType::SigningEnum::NONE;
            return true;
        }

        if(node.as<std::string>() == "SIGNED") {
            rhs = Veriparse::AST::DataType::SigningEnum::SIGNED;
            return true;
        }

        if(node.as<std::string>() == "UNSIGNED") {
            rhs = Veriparse::AST::DataType::SigningEnum::UNSIGNED;
            return true;
        }

        return false;
    }
};
template <> struct convert<Veriparse::AST::Var::LifetimeEnum>
{
    static Node encode(const Veriparse::AST::Var::LifetimeEnum &rhs)
    {
        switch(rhs) {
        case Veriparse::AST::Var::LifetimeEnum::NONE:
            return Node("NONE");
        case Veriparse::AST::Var::LifetimeEnum::AUTOMATIC:
            return Node("AUTOMATIC");
        default:
            return Node("STATIC");
        }
    }

    static bool decode(const Node &node, Veriparse::AST::Var::LifetimeEnum &rhs)
    {
        if(!node.IsScalar()) {
            return false;
        }

        if(node.as<std::string>() == "NONE") {
            rhs = Veriparse::AST::Var::LifetimeEnum::NONE;
            return true;
        }

        if(node.as<std::string>() == "AUTOMATIC") {
            rhs = Veriparse::AST::Var::LifetimeEnum::AUTOMATIC;
            return true;
        }

        if(node.as<std::string>() == "STATIC") {
            rhs = Veriparse::AST::Var::LifetimeEnum::STATIC;
            return true;
        }

        return false;
    }
};
template <> struct convert<Veriparse::AST::DriveStrength::S0Enum>
{
    static Node encode(const Veriparse::AST::DriveStrength::S0Enum &rhs)
    {
        switch(rhs) {
        case Veriparse::AST::DriveStrength::S0Enum::SUPPLY0:
            return Node("SUPPLY0");
        case Veriparse::AST::DriveStrength::S0Enum::STRONG0:
            return Node("STRONG0");
        case Veriparse::AST::DriveStrength::S0Enum::PULL0:
            return Node("PULL0");
        case Veriparse::AST::DriveStrength::S0Enum::WEAK0:
            return Node("WEAK0");
        default:
            return Node("HIGHZ0");
        }
    }

    static bool decode(const Node &node, Veriparse::AST::DriveStrength::S0Enum &rhs)
    {
        if(!node.IsScalar()) {
            return false;
        }

        if(node.as<std::string>() == "SUPPLY0") {
            rhs = Veriparse::AST::DriveStrength::S0Enum::SUPPLY0;
            return true;
        }

        if(node.as<std::string>() == "STRONG0") {
            rhs = Veriparse::AST::DriveStrength::S0Enum::STRONG0;
            return true;
        }

        if(node.as<std::string>() == "PULL0") {
            rhs = Veriparse::AST::DriveStrength::S0Enum::PULL0;
            return true;
        }

        if(node.as<std::string>() == "WEAK0") {
            rhs = Veriparse::AST::DriveStrength::S0Enum::WEAK0;
            return true;
        }

        if(node.as<std::string>() == "HIGHZ0") {
            rhs = Veriparse::AST::DriveStrength::S0Enum::HIGHZ0;
            return true;
        }

        return false;
    }
};
template <> struct convert<Veriparse::AST::DriveStrength::S1Enum>
{
    static Node encode(const Veriparse::AST::DriveStrength::S1Enum &rhs)
    {
        switch(rhs) {
        case Veriparse::AST::DriveStrength::S1Enum::SUPPLY1:
            return Node("SUPPLY1");
        case Veriparse::AST::DriveStrength::S1Enum::STRONG1:
            return Node("STRONG1");
        case Veriparse::AST::DriveStrength::S1Enum::PULL1:
            return Node("PULL1");
        case Veriparse::AST::DriveStrength::S1Enum::WEAK1:
            return Node("WEAK1");
        default:
            return Node("HIGHZ1");
        }
    }

    static bool decode(const Node &node, Veriparse::AST::DriveStrength::S1Enum &rhs)
    {
        if(!node.IsScalar()) {
            return false;
        }

        if(node.as<std::string>() == "SUPPLY1") {
            rhs = Veriparse::AST::DriveStrength::S1Enum::SUPPLY1;
            return true;
        }

        if(node.as<std::string>() == "STRONG1") {
            rhs = Veriparse::AST::DriveStrength::S1Enum::STRONG1;
            return true;
        }

        if(node.as<std::string>() == "PULL1") {
            rhs = Veriparse::AST::DriveStrength::S1Enum::PULL1;
            return true;
        }

        if(node.as<std::string>() == "WEAK1") {
            rhs = Veriparse::AST::DriveStrength::S1Enum::WEAK1;
            return true;
        }

        if(node.as<std::string>() == "HIGHZ1") {
            rhs = Veriparse::AST::DriveStrength::S1Enum::HIGHZ1;
            return true;
        }

        return false;
    }
};
template <> struct convert<Veriparse::AST::ChargeStrength::ChargeEnum>
{
    static Node encode(const Veriparse::AST::ChargeStrength::ChargeEnum &rhs)
    {
        switch(rhs) {
        case Veriparse::AST::ChargeStrength::ChargeEnum::SMALL:
            return Node("SMALL");
        case Veriparse::AST::ChargeStrength::ChargeEnum::MEDIUM:
            return Node("MEDIUM");
        default:
            return Node("LARGE");
        }
    }

    static bool decode(const Node &node, Veriparse::AST::ChargeStrength::ChargeEnum &rhs)
    {
        if(!node.IsScalar()) {
            return false;
        }

        if(node.as<std::string>() == "SMALL") {
            rhs = Veriparse::AST::ChargeStrength::ChargeEnum::SMALL;
            return true;
        }

        if(node.as<std::string>() == "MEDIUM") {
            rhs = Veriparse::AST::ChargeStrength::ChargeEnum::MEDIUM;
            return true;
        }

        if(node.as<std::string>() == "LARGE") {
            rhs = Veriparse::AST::ChargeStrength::ChargeEnum::LARGE;
            return true;
        }

        return false;
    }
};
template <> struct convert<Veriparse::AST::Typedef::Fwd_kindEnum>
{
    static Node encode(const Veriparse::AST::Typedef::Fwd_kindEnum &rhs)
    {
        switch(rhs) {
        case Veriparse::AST::Typedef::Fwd_kindEnum::NONE:
            return Node("NONE");
        case Veriparse::AST::Typedef::Fwd_kindEnum::ENUM:
            return Node("ENUM");
        case Veriparse::AST::Typedef::Fwd_kindEnum::STRUCT:
            return Node("STRUCT");
        case Veriparse::AST::Typedef::Fwd_kindEnum::UNION:
            return Node("UNION");
        case Veriparse::AST::Typedef::Fwd_kindEnum::CLASS:
            return Node("CLASS");
        default:
            return Node("INTERFACE_CLASS");
        }
    }

    static bool decode(const Node &node, Veriparse::AST::Typedef::Fwd_kindEnum &rhs)
    {
        if(!node.IsScalar()) {
            return false;
        }

        if(node.as<std::string>() == "NONE") {
            rhs = Veriparse::AST::Typedef::Fwd_kindEnum::NONE;
            return true;
        }

        if(node.as<std::string>() == "ENUM") {
            rhs = Veriparse::AST::Typedef::Fwd_kindEnum::ENUM;
            return true;
        }

        if(node.as<std::string>() == "STRUCT") {
            rhs = Veriparse::AST::Typedef::Fwd_kindEnum::STRUCT;
            return true;
        }

        if(node.as<std::string>() == "UNION") {
            rhs = Veriparse::AST::Typedef::Fwd_kindEnum::UNION;
            return true;
        }

        if(node.as<std::string>() == "CLASS") {
            rhs = Veriparse::AST::Typedef::Fwd_kindEnum::CLASS;
            return true;
        }

        if(node.as<std::string>() == "INTERFACE_CLASS") {
            rhs = Veriparse::AST::Typedef::Fwd_kindEnum::INTERFACE_CLASS;
            return true;
        }

        return false;
    }
};
template <> struct convert<Veriparse::AST::Arg::DirectionEnum>
{
    static Node encode(const Veriparse::AST::Arg::DirectionEnum &rhs)
    {
        switch(rhs) {
        case Veriparse::AST::Arg::DirectionEnum::NONE:
            return Node("NONE");
        case Veriparse::AST::Arg::DirectionEnum::INPUT:
            return Node("INPUT");
        case Veriparse::AST::Arg::DirectionEnum::OUTPUT:
            return Node("OUTPUT");
        case Veriparse::AST::Arg::DirectionEnum::INOUT:
            return Node("INOUT");
        case Veriparse::AST::Arg::DirectionEnum::REF:
            return Node("REF");
        default:
            return Node("CONST_REF");
        }
    }

    static bool decode(const Node &node, Veriparse::AST::Arg::DirectionEnum &rhs)
    {
        if(!node.IsScalar()) {
            return false;
        }

        if(node.as<std::string>() == "NONE") {
            rhs = Veriparse::AST::Arg::DirectionEnum::NONE;
            return true;
        }

        if(node.as<std::string>() == "INPUT") {
            rhs = Veriparse::AST::Arg::DirectionEnum::INPUT;
            return true;
        }

        if(node.as<std::string>() == "OUTPUT") {
            rhs = Veriparse::AST::Arg::DirectionEnum::OUTPUT;
            return true;
        }

        if(node.as<std::string>() == "INOUT") {
            rhs = Veriparse::AST::Arg::DirectionEnum::INOUT;
            return true;
        }

        if(node.as<std::string>() == "REF") {
            rhs = Veriparse::AST::Arg::DirectionEnum::REF;
            return true;
        }

        if(node.as<std::string>() == "CONST_REF") {
            rhs = Veriparse::AST::Arg::DirectionEnum::CONST_REF;
            return true;
        }

        return false;
    }
};
template <> struct convert<Veriparse::AST::SigningCast::SigningEnum>
{
    static Node encode(const Veriparse::AST::SigningCast::SigningEnum &rhs)
    {
        switch(rhs) {
        case Veriparse::AST::SigningCast::SigningEnum::SIGNED:
            return Node("SIGNED");
        default:
            return Node("UNSIGNED");
        }
    }

    static bool decode(const Node &node, Veriparse::AST::SigningCast::SigningEnum &rhs)
    {
        if(!node.IsScalar()) {
            return false;
        }

        if(node.as<std::string>() == "SIGNED") {
            rhs = Veriparse::AST::SigningCast::SigningEnum::SIGNED;
            return true;
        }

        if(node.as<std::string>() == "UNSIGNED") {
            rhs = Veriparse::AST::SigningCast::SigningEnum::UNSIGNED;
            return true;
        }

        return false;
    }
};
template <> struct convert<Veriparse::AST::Sens::TypeEnum>
{
    static Node encode(const Veriparse::AST::Sens::TypeEnum &rhs)
    {
        switch(rhs) {
        case Veriparse::AST::Sens::TypeEnum::ALL:
            return Node("ALL");
        case Veriparse::AST::Sens::TypeEnum::POSEDGE:
            return Node("POSEDGE");
        case Veriparse::AST::Sens::TypeEnum::NEGEDGE:
            return Node("NEGEDGE");
        default:
            return Node("NONE");
        }
    }

    static bool decode(const Node &node, Veriparse::AST::Sens::TypeEnum &rhs)
    {
        if(!node.IsScalar()) {
            return false;
        }

        if(node.as<std::string>() == "ALL") {
            rhs = Veriparse::AST::Sens::TypeEnum::ALL;
            return true;
        }

        if(node.as<std::string>() == "POSEDGE") {
            rhs = Veriparse::AST::Sens::TypeEnum::POSEDGE;
            return true;
        }

        if(node.as<std::string>() == "NEGEDGE") {
            rhs = Veriparse::AST::Sens::TypeEnum::NEGEDGE;
            return true;
        }

        if(node.as<std::string>() == "NONE") {
            rhs = Veriparse::AST::Sens::TypeEnum::NONE;
            return true;
        }

        return false;
    }
};
template <> struct convert<Veriparse::AST::Function::LifetimeEnum>
{
    static Node encode(const Veriparse::AST::Function::LifetimeEnum &rhs)
    {
        switch(rhs) {
        case Veriparse::AST::Function::LifetimeEnum::NONE:
            return Node("NONE");
        case Veriparse::AST::Function::LifetimeEnum::AUTOMATIC:
            return Node("AUTOMATIC");
        default:
            return Node("STATIC");
        }
    }

    static bool decode(const Node &node, Veriparse::AST::Function::LifetimeEnum &rhs)
    {
        if(!node.IsScalar()) {
            return false;
        }

        if(node.as<std::string>() == "NONE") {
            rhs = Veriparse::AST::Function::LifetimeEnum::NONE;
            return true;
        }

        if(node.as<std::string>() == "AUTOMATIC") {
            rhs = Veriparse::AST::Function::LifetimeEnum::AUTOMATIC;
            return true;
        }

        if(node.as<std::string>() == "STATIC") {
            rhs = Veriparse::AST::Function::LifetimeEnum::STATIC;
            return true;
        }

        return false;
    }
};
template <> struct convert<Veriparse::AST::Task::LifetimeEnum>
{
    static Node encode(const Veriparse::AST::Task::LifetimeEnum &rhs)
    {
        switch(rhs) {
        case Veriparse::AST::Task::LifetimeEnum::NONE:
            return Node("NONE");
        case Veriparse::AST::Task::LifetimeEnum::AUTOMATIC:
            return Node("AUTOMATIC");
        default:
            return Node("STATIC");
        }
    }

    static bool decode(const Node &node, Veriparse::AST::Task::LifetimeEnum &rhs)
    {
        if(!node.IsScalar()) {
            return false;
        }

        if(node.as<std::string>() == "NONE") {
            rhs = Veriparse::AST::Task::LifetimeEnum::NONE;
            return true;
        }

        if(node.as<std::string>() == "AUTOMATIC") {
            rhs = Veriparse::AST::Task::LifetimeEnum::AUTOMATIC;
            return true;
        }

        if(node.as<std::string>() == "STATIC") {
            rhs = Veriparse::AST::Task::LifetimeEnum::STATIC;
            return true;
        }

        return false;
    }
};

template <> struct convert<mpz_class>
{
    static Node encode(const mpz_class &rhs) { return Node(rhs.str()); }

    static bool decode(const Node &node, mpz_class &rhs)
    {
        if(!node.IsScalar()) {
            return false;
        }

        bool result = true;

        try {
            mpz_class num(node.as<std::string>());
            rhs = num;
        } catch(const std::invalid_argument &arg) {
            result = false;
        }

        return result;
    }
};

} // namespace YAML

#endif