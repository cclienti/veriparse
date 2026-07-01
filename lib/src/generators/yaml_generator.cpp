// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/generators/yaml_generator.hpp>
#include <veriparse/importers/yaml_specializations.hpp>

namespace Veriparse
{
namespace Generators
{

YAML::Node YAMLGenerator::render_node(const AST::Node::Ptr node) const
{
    YAML::Node result;
    return result;
}

YAML::Node YAMLGenerator::render_source(const AST::Source::Ptr node) const
{
    YAML::Node node_source;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Source) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["description"] = render(node->get_description());
    }

    node_source["Source"] = content;
    return node_source;
}

YAML::Node YAMLGenerator::render_description(const AST::Description::Ptr node) const
{
    YAML::Node node_description;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Description) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        if(node->get_definitions()) {
            content["definitions"] = YAML::Load("[]");
            for(const AST::Node::Ptr &n : *node->get_definitions()) {
                content["definitions"].push_back(render(n));
            }
        }
    }

    node_description["Description"] = content;
    return node_description;
}

YAML::Node YAMLGenerator::render_pragmalist(const AST::Pragmalist::Ptr node) const
{
    YAML::Node node_pragmalist;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Pragmalist) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        if(node->get_pragmas()) {
            content["pragmas"] = YAML::Load("[]");
            for(const AST::Pragma::Ptr &n : *node->get_pragmas()) {
                content["pragmas"].push_back(render(n));
            }
        }

        if(node->get_statements()) {
            content["statements"] = YAML::Load("[]");
            for(const AST::Node::Ptr &n : *node->get_statements()) {
                content["statements"].push_back(render(n));
            }
        }
    }

    node_pragmalist["Pragmalist"] = content;
    return node_pragmalist;
}

YAML::Node YAMLGenerator::render_pragma(const AST::Pragma::Ptr node) const
{
    YAML::Node node_pragma;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Pragma) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["name"] = node->get_name();

        content["expression"] = render(node->get_expression());
    }

    node_pragma["Pragma"] = content;
    return node_pragma;
}

YAML::Node YAMLGenerator::render_module(const AST::Module::Ptr node) const
{
    YAML::Node node_module;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Module) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["name"] = node->get_name();
        switch(node->get_lifetime()) {
        case Veriparse::AST::Module::LifetimeEnum::NONE:
            content["lifetime"] = "NONE";
            break;
        case Veriparse::AST::Module::LifetimeEnum::AUTOMATIC:
            content["lifetime"] = "AUTOMATIC";
            break;
        default:
            content["lifetime"] = "STATIC";
        }
        switch(node->get_default_nettype()) {
        case Veriparse::AST::Module::Default_nettypeEnum::WIRE:
            content["default_nettype"] = "WIRE";
            break;
        case Veriparse::AST::Module::Default_nettypeEnum::TRI:
            content["default_nettype"] = "TRI";
            break;
        case Veriparse::AST::Module::Default_nettypeEnum::TRI0:
            content["default_nettype"] = "TRI0";
            break;
        case Veriparse::AST::Module::Default_nettypeEnum::TRI1:
            content["default_nettype"] = "TRI1";
            break;
        case Veriparse::AST::Module::Default_nettypeEnum::TRIAND:
            content["default_nettype"] = "TRIAND";
            break;
        case Veriparse::AST::Module::Default_nettypeEnum::TRIOR:
            content["default_nettype"] = "TRIOR";
            break;
        case Veriparse::AST::Module::Default_nettypeEnum::TRIREG:
            content["default_nettype"] = "TRIREG";
            break;
        case Veriparse::AST::Module::Default_nettypeEnum::WAND:
            content["default_nettype"] = "WAND";
            break;
        case Veriparse::AST::Module::Default_nettypeEnum::WOR:
            content["default_nettype"] = "WOR";
            break;
        case Veriparse::AST::Module::Default_nettypeEnum::UWIRE:
            content["default_nettype"] = "UWIRE";
            break;
        case Veriparse::AST::Module::Default_nettypeEnum::SUPPLY0:
            content["default_nettype"] = "SUPPLY0";
            break;
        case Veriparse::AST::Module::Default_nettypeEnum::SUPPLY1:
            content["default_nettype"] = "SUPPLY1";
            break;
        default:
            content["default_nettype"] = "NONE";
        }

        if(node->get_params()) {
            content["params"] = YAML::Load("[]");
            for(const AST::Declaration::Ptr &n : *node->get_params()) {
                content["params"].push_back(render(n));
            }
        }

        if(node->get_ports()) {
            content["ports"] = YAML::Load("[]");
            for(const AST::Port::Ptr &n : *node->get_ports()) {
                content["ports"].push_back(render(n));
            }
        }

        if(node->get_items()) {
            content["items"] = YAML::Load("[]");
            for(const AST::Node::Ptr &n : *node->get_items()) {
                content["items"].push_back(render(n));
            }
        }
    }

    node_module["Module"] = content;
    return node_module;
}

YAML::Node YAMLGenerator::render_port(const AST::Port::Ptr node) const
{
    YAML::Node node_port;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Port) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["name"] = node->get_name();
        switch(node->get_direction()) {
        case Veriparse::AST::Port::DirectionEnum::NONE:
            content["direction"] = "NONE";
            break;
        case Veriparse::AST::Port::DirectionEnum::INPUT:
            content["direction"] = "INPUT";
            break;
        case Veriparse::AST::Port::DirectionEnum::OUTPUT:
            content["direction"] = "OUTPUT";
            break;
        case Veriparse::AST::Port::DirectionEnum::INOUT:
            content["direction"] = "INOUT";
            break;
        case Veriparse::AST::Port::DirectionEnum::REF:
            content["direction"] = "REF";
            break;
        default:
            content["direction"] = "CONST_REF";
        }

        content["decl"] = render(node->get_decl());

        content["expr"] = render(node->get_expr());
    }

    node_port["Port"] = content;
    return node_port;
}

YAML::Node YAMLGenerator::render_package(const AST::Package::Ptr node) const
{
    YAML::Node node_package;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Package) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["name"] = node->get_name();
        switch(node->get_lifetime()) {
        case Veriparse::AST::Package::LifetimeEnum::NONE:
            content["lifetime"] = "NONE";
            break;
        case Veriparse::AST::Package::LifetimeEnum::AUTOMATIC:
            content["lifetime"] = "AUTOMATIC";
            break;
        default:
            content["lifetime"] = "STATIC";
        }

        if(node->get_items()) {
            content["items"] = YAML::Load("[]");
            for(const AST::Node::Ptr &n : *node->get_items()) {
                content["items"].push_back(render(n));
            }
        }
    }

    node_package["Package"] = content;
    return node_package;
}

YAML::Node YAMLGenerator::render_import(const AST::Import::Ptr node) const
{
    YAML::Node node_import;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Import) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["package"] = node->get_package();
        content["symbol"] = node->get_symbol();
    }

    node_import["Import"] = content;
    return node_import;
}

YAML::Node YAMLGenerator::render_export(const AST::Export::Ptr node) const
{
    YAML::Node node_export;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Export) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["package"] = node->get_package();
        content["symbol"] = node->get_symbol();
    }

    node_export["Export"] = content;
    return node_export;
}

YAML::Node YAMLGenerator::render_identifier(const AST::Identifier::Ptr node) const
{
    YAML::Node node_identifier;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Identifier) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["name"] = node->get_name();

        if(node->get_scope()) {
            content["scope"] = YAML::Load("[]");
            for(const AST::ScopeName::Ptr &n : *node->get_scope()) {
                content["scope"].push_back(render(n));
            }
        }

        content["hier"] = render(node->get_hier());
    }

    node_identifier["Identifier"] = content;
    return node_identifier;
}

YAML::Node YAMLGenerator::render_call(const AST::Call::Ptr node) const
{
    YAML::Node node_call;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Call) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["name"] = node->get_name();

        if(node->get_args()) {
            content["args"] = YAML::Load("[]");
            for(const AST::Node::Ptr &n : *node->get_args()) {
                content["args"].push_back(render(n));
            }
        }

        if(node->get_scope()) {
            content["scope"] = YAML::Load("[]");
            for(const AST::ScopeName::Ptr &n : *node->get_scope()) {
                content["scope"].push_back(render(n));
            }
        }

        content["hier"] = render(node->get_hier());
    }

    node_call["Call"] = content;
    return node_call;
}

YAML::Node YAMLGenerator::render_constant(const AST::Constant::Ptr node) const
{
    YAML::Node node_constant;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Constant) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
    }

    node_constant["Constant"] = content;
    return node_constant;
}

YAML::Node YAMLGenerator::render_stringconst(const AST::StringConst::Ptr node) const
{
    YAML::Node node_stringconst;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::StringConst) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["value"] = node->get_value();
    }

    node_stringconst["StringConst"] = content;
    return node_stringconst;
}

YAML::Node YAMLGenerator::render_intconst(const AST::IntConst::Ptr node) const
{
    YAML::Node node_intconst;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::IntConst) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["value"] = node->get_value();
    }

    node_intconst["IntConst"] = content;
    return node_intconst;
}

YAML::Node YAMLGenerator::render_intconstn(const AST::IntConstN::Ptr node) const
{
    YAML::Node node_intconstn;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::IntConstN) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["base"] = node->get_base();
        content["size"] = node->get_size();
        content["sign"] = node->get_sign();
        content["value"] = node->get_value();
    }

    node_intconstn["IntConstN"] = content;
    return node_intconstn;
}

YAML::Node YAMLGenerator::render_floatconst(const AST::FloatConst::Ptr node) const
{
    YAML::Node node_floatconst;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::FloatConst) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["value"] = node->get_value();
    }

    node_floatconst["FloatConst"] = content;
    return node_floatconst;
}

YAML::Node YAMLGenerator::render_datatype(const AST::DataType::Ptr node) const
{
    YAML::Node node_datatype;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::DataType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        switch(node->get_signing()) {
        case Veriparse::AST::DataType::SigningEnum::NONE:
            content["signing"] = "NONE";
            break;
        case Veriparse::AST::DataType::SigningEnum::SIGNED:
            content["signing"] = "SIGNED";
            break;
        default:
            content["signing"] = "UNSIGNED";
        }

        if(node->get_packed_dims()) {
            content["packed_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                content["packed_dims"].push_back(render(n));
            }
        }
    }

    node_datatype["DataType"] = content;
    return node_datatype;
}

YAML::Node YAMLGenerator::render_logictype(const AST::LogicType::Ptr node) const
{
    YAML::Node node_logictype;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::LogicType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        switch(node->get_signing()) {
        case Veriparse::AST::LogicType::DataType::SigningEnum::NONE:
            content["signing"] = "NONE";
            break;
        case Veriparse::AST::LogicType::DataType::SigningEnum::SIGNED:
            content["signing"] = "SIGNED";
            break;
        default:
            content["signing"] = "UNSIGNED";
        }

        if(node->get_packed_dims()) {
            content["packed_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                content["packed_dims"].push_back(render(n));
            }
        }
    }

    node_logictype["LogicType"] = content;
    return node_logictype;
}

YAML::Node YAMLGenerator::render_regtype(const AST::RegType::Ptr node) const
{
    YAML::Node node_regtype;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::RegType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        switch(node->get_signing()) {
        case Veriparse::AST::RegType::DataType::SigningEnum::NONE:
            content["signing"] = "NONE";
            break;
        case Veriparse::AST::RegType::DataType::SigningEnum::SIGNED:
            content["signing"] = "SIGNED";
            break;
        default:
            content["signing"] = "UNSIGNED";
        }

        if(node->get_packed_dims()) {
            content["packed_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                content["packed_dims"].push_back(render(n));
            }
        }
    }

    node_regtype["RegType"] = content;
    return node_regtype;
}

YAML::Node YAMLGenerator::render_bittype(const AST::BitType::Ptr node) const
{
    YAML::Node node_bittype;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::BitType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        switch(node->get_signing()) {
        case Veriparse::AST::BitType::DataType::SigningEnum::NONE:
            content["signing"] = "NONE";
            break;
        case Veriparse::AST::BitType::DataType::SigningEnum::SIGNED:
            content["signing"] = "SIGNED";
            break;
        default:
            content["signing"] = "UNSIGNED";
        }

        if(node->get_packed_dims()) {
            content["packed_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                content["packed_dims"].push_back(render(n));
            }
        }
    }

    node_bittype["BitType"] = content;
    return node_bittype;
}

YAML::Node YAMLGenerator::render_bytetype(const AST::ByteType::Ptr node) const
{
    YAML::Node node_bytetype;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::ByteType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        switch(node->get_signing()) {
        case Veriparse::AST::ByteType::DataType::SigningEnum::NONE:
            content["signing"] = "NONE";
            break;
        case Veriparse::AST::ByteType::DataType::SigningEnum::SIGNED:
            content["signing"] = "SIGNED";
            break;
        default:
            content["signing"] = "UNSIGNED";
        }

        if(node->get_packed_dims()) {
            content["packed_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                content["packed_dims"].push_back(render(n));
            }
        }
    }

    node_bytetype["ByteType"] = content;
    return node_bytetype;
}

YAML::Node YAMLGenerator::render_shortinttype(const AST::ShortintType::Ptr node) const
{
    YAML::Node node_shortinttype;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::ShortintType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        switch(node->get_signing()) {
        case Veriparse::AST::ShortintType::DataType::SigningEnum::NONE:
            content["signing"] = "NONE";
            break;
        case Veriparse::AST::ShortintType::DataType::SigningEnum::SIGNED:
            content["signing"] = "SIGNED";
            break;
        default:
            content["signing"] = "UNSIGNED";
        }

        if(node->get_packed_dims()) {
            content["packed_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                content["packed_dims"].push_back(render(n));
            }
        }
    }

    node_shortinttype["ShortintType"] = content;
    return node_shortinttype;
}

YAML::Node YAMLGenerator::render_inttype(const AST::IntType::Ptr node) const
{
    YAML::Node node_inttype;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::IntType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        switch(node->get_signing()) {
        case Veriparse::AST::IntType::DataType::SigningEnum::NONE:
            content["signing"] = "NONE";
            break;
        case Veriparse::AST::IntType::DataType::SigningEnum::SIGNED:
            content["signing"] = "SIGNED";
            break;
        default:
            content["signing"] = "UNSIGNED";
        }

        if(node->get_packed_dims()) {
            content["packed_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                content["packed_dims"].push_back(render(n));
            }
        }
    }

    node_inttype["IntType"] = content;
    return node_inttype;
}

YAML::Node YAMLGenerator::render_longinttype(const AST::LongintType::Ptr node) const
{
    YAML::Node node_longinttype;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::LongintType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        switch(node->get_signing()) {
        case Veriparse::AST::LongintType::DataType::SigningEnum::NONE:
            content["signing"] = "NONE";
            break;
        case Veriparse::AST::LongintType::DataType::SigningEnum::SIGNED:
            content["signing"] = "SIGNED";
            break;
        default:
            content["signing"] = "UNSIGNED";
        }

        if(node->get_packed_dims()) {
            content["packed_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                content["packed_dims"].push_back(render(n));
            }
        }
    }

    node_longinttype["LongintType"] = content;
    return node_longinttype;
}

YAML::Node YAMLGenerator::render_integertype(const AST::IntegerType::Ptr node) const
{
    YAML::Node node_integertype;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::IntegerType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        switch(node->get_signing()) {
        case Veriparse::AST::IntegerType::DataType::SigningEnum::NONE:
            content["signing"] = "NONE";
            break;
        case Veriparse::AST::IntegerType::DataType::SigningEnum::SIGNED:
            content["signing"] = "SIGNED";
            break;
        default:
            content["signing"] = "UNSIGNED";
        }

        if(node->get_packed_dims()) {
            content["packed_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                content["packed_dims"].push_back(render(n));
            }
        }
    }

    node_integertype["IntegerType"] = content;
    return node_integertype;
}

YAML::Node YAMLGenerator::render_timetype(const AST::TimeType::Ptr node) const
{
    YAML::Node node_timetype;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::TimeType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        switch(node->get_signing()) {
        case Veriparse::AST::TimeType::DataType::SigningEnum::NONE:
            content["signing"] = "NONE";
            break;
        case Veriparse::AST::TimeType::DataType::SigningEnum::SIGNED:
            content["signing"] = "SIGNED";
            break;
        default:
            content["signing"] = "UNSIGNED";
        }

        if(node->get_packed_dims()) {
            content["packed_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                content["packed_dims"].push_back(render(n));
            }
        }
    }

    node_timetype["TimeType"] = content;
    return node_timetype;
}

YAML::Node YAMLGenerator::render_realtype(const AST::RealType::Ptr node) const
{
    YAML::Node node_realtype;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::RealType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        switch(node->get_signing()) {
        case Veriparse::AST::RealType::DataType::SigningEnum::NONE:
            content["signing"] = "NONE";
            break;
        case Veriparse::AST::RealType::DataType::SigningEnum::SIGNED:
            content["signing"] = "SIGNED";
            break;
        default:
            content["signing"] = "UNSIGNED";
        }

        if(node->get_packed_dims()) {
            content["packed_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                content["packed_dims"].push_back(render(n));
            }
        }
    }

    node_realtype["RealType"] = content;
    return node_realtype;
}

YAML::Node YAMLGenerator::render_shortrealtype(const AST::ShortrealType::Ptr node) const
{
    YAML::Node node_shortrealtype;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::ShortrealType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        switch(node->get_signing()) {
        case Veriparse::AST::ShortrealType::DataType::SigningEnum::NONE:
            content["signing"] = "NONE";
            break;
        case Veriparse::AST::ShortrealType::DataType::SigningEnum::SIGNED:
            content["signing"] = "SIGNED";
            break;
        default:
            content["signing"] = "UNSIGNED";
        }

        if(node->get_packed_dims()) {
            content["packed_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                content["packed_dims"].push_back(render(n));
            }
        }
    }

    node_shortrealtype["ShortrealType"] = content;
    return node_shortrealtype;
}

YAML::Node YAMLGenerator::render_realtimetype(const AST::RealtimeType::Ptr node) const
{
    YAML::Node node_realtimetype;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::RealtimeType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        switch(node->get_signing()) {
        case Veriparse::AST::RealtimeType::DataType::SigningEnum::NONE:
            content["signing"] = "NONE";
            break;
        case Veriparse::AST::RealtimeType::DataType::SigningEnum::SIGNED:
            content["signing"] = "SIGNED";
            break;
        default:
            content["signing"] = "UNSIGNED";
        }

        if(node->get_packed_dims()) {
            content["packed_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                content["packed_dims"].push_back(render(n));
            }
        }
    }

    node_realtimetype["RealtimeType"] = content;
    return node_realtimetype;
}

YAML::Node YAMLGenerator::render_stringtype(const AST::StringType::Ptr node) const
{
    YAML::Node node_stringtype;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::StringType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        switch(node->get_signing()) {
        case Veriparse::AST::StringType::DataType::SigningEnum::NONE:
            content["signing"] = "NONE";
            break;
        case Veriparse::AST::StringType::DataType::SigningEnum::SIGNED:
            content["signing"] = "SIGNED";
            break;
        default:
            content["signing"] = "UNSIGNED";
        }

        if(node->get_packed_dims()) {
            content["packed_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                content["packed_dims"].push_back(render(n));
            }
        }
    }

    node_stringtype["StringType"] = content;
    return node_stringtype;
}

YAML::Node YAMLGenerator::render_chandletype(const AST::ChandleType::Ptr node) const
{
    YAML::Node node_chandletype;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::ChandleType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        switch(node->get_signing()) {
        case Veriparse::AST::ChandleType::DataType::SigningEnum::NONE:
            content["signing"] = "NONE";
            break;
        case Veriparse::AST::ChandleType::DataType::SigningEnum::SIGNED:
            content["signing"] = "SIGNED";
            break;
        default:
            content["signing"] = "UNSIGNED";
        }

        if(node->get_packed_dims()) {
            content["packed_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                content["packed_dims"].push_back(render(n));
            }
        }
    }

    node_chandletype["ChandleType"] = content;
    return node_chandletype;
}

YAML::Node YAMLGenerator::render_eventtype(const AST::EventType::Ptr node) const
{
    YAML::Node node_eventtype;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::EventType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        switch(node->get_signing()) {
        case Veriparse::AST::EventType::DataType::SigningEnum::NONE:
            content["signing"] = "NONE";
            break;
        case Veriparse::AST::EventType::DataType::SigningEnum::SIGNED:
            content["signing"] = "SIGNED";
            break;
        default:
            content["signing"] = "UNSIGNED";
        }

        if(node->get_packed_dims()) {
            content["packed_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                content["packed_dims"].push_back(render(n));
            }
        }
    }

    node_eventtype["EventType"] = content;
    return node_eventtype;
}

YAML::Node YAMLGenerator::render_implicittype(const AST::ImplicitType::Ptr node) const
{
    YAML::Node node_implicittype;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::ImplicitType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        switch(node->get_signing()) {
        case Veriparse::AST::ImplicitType::DataType::SigningEnum::NONE:
            content["signing"] = "NONE";
            break;
        case Veriparse::AST::ImplicitType::DataType::SigningEnum::SIGNED:
            content["signing"] = "SIGNED";
            break;
        default:
            content["signing"] = "UNSIGNED";
        }

        if(node->get_packed_dims()) {
            content["packed_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                content["packed_dims"].push_back(render(n));
            }
        }
    }

    node_implicittype["ImplicitType"] = content;
    return node_implicittype;
}

YAML::Node YAMLGenerator::render_voidtype(const AST::VoidType::Ptr node) const
{
    YAML::Node node_voidtype;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::VoidType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        switch(node->get_signing()) {
        case Veriparse::AST::VoidType::DataType::SigningEnum::NONE:
            content["signing"] = "NONE";
            break;
        case Veriparse::AST::VoidType::DataType::SigningEnum::SIGNED:
            content["signing"] = "SIGNED";
            break;
        default:
            content["signing"] = "UNSIGNED";
        }

        if(node->get_packed_dims()) {
            content["packed_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                content["packed_dims"].push_back(render(n));
            }
        }
    }

    node_voidtype["VoidType"] = content;
    return node_voidtype;
}

YAML::Node YAMLGenerator::render_namedtype(const AST::NamedType::Ptr node) const
{
    YAML::Node node_namedtype;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::NamedType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["name"] = node->get_name();
        switch(node->get_signing()) {
        case Veriparse::AST::NamedType::DataType::SigningEnum::NONE:
            content["signing"] = "NONE";
            break;
        case Veriparse::AST::NamedType::DataType::SigningEnum::SIGNED:
            content["signing"] = "SIGNED";
            break;
        default:
            content["signing"] = "UNSIGNED";
        }

        if(node->get_scope()) {
            content["scope"] = YAML::Load("[]");
            for(const AST::ScopeName::Ptr &n : *node->get_scope()) {
                content["scope"].push_back(render(n));
            }
        }

        if(node->get_packed_dims()) {
            content["packed_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                content["packed_dims"].push_back(render(n));
            }
        }
    }

    node_namedtype["NamedType"] = content;
    return node_namedtype;
}

YAML::Node YAMLGenerator::render_scopename(const AST::ScopeName::Ptr node) const
{
    YAML::Node node_scopename;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::ScopeName) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["name"] = node->get_name();

        if(node->get_params()) {
            content["params"] = YAML::Load("[]");
            for(const AST::ParamArg::Ptr &n : *node->get_params()) {
                content["params"].push_back(render(n));
            }
        }
    }

    node_scopename["ScopeName"] = content;
    return node_scopename;
}

YAML::Node YAMLGenerator::render_structtype(const AST::StructType::Ptr node) const
{
    YAML::Node node_structtype;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::StructType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["is_packed"] = node->get_is_packed();
        switch(node->get_signing()) {
        case Veriparse::AST::StructType::DataType::SigningEnum::NONE:
            content["signing"] = "NONE";
            break;
        case Veriparse::AST::StructType::DataType::SigningEnum::SIGNED:
            content["signing"] = "SIGNED";
            break;
        default:
            content["signing"] = "UNSIGNED";
        }

        if(node->get_members()) {
            content["members"] = YAML::Load("[]");
            for(const AST::Member::Ptr &n : *node->get_members()) {
                content["members"].push_back(render(n));
            }
        }

        if(node->get_packed_dims()) {
            content["packed_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                content["packed_dims"].push_back(render(n));
            }
        }
    }

    node_structtype["StructType"] = content;
    return node_structtype;
}

YAML::Node YAMLGenerator::render_uniontype(const AST::UnionType::Ptr node) const
{
    YAML::Node node_uniontype;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::UnionType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["is_packed"] = node->get_is_packed();
        content["is_tagged"] = node->get_is_tagged();
        switch(node->get_signing()) {
        case Veriparse::AST::UnionType::DataType::SigningEnum::NONE:
            content["signing"] = "NONE";
            break;
        case Veriparse::AST::UnionType::DataType::SigningEnum::SIGNED:
            content["signing"] = "SIGNED";
            break;
        default:
            content["signing"] = "UNSIGNED";
        }

        if(node->get_members()) {
            content["members"] = YAML::Load("[]");
            for(const AST::Member::Ptr &n : *node->get_members()) {
                content["members"].push_back(render(n));
            }
        }

        if(node->get_packed_dims()) {
            content["packed_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                content["packed_dims"].push_back(render(n));
            }
        }
    }

    node_uniontype["UnionType"] = content;
    return node_uniontype;
}

YAML::Node YAMLGenerator::render_enumtype(const AST::EnumType::Ptr node) const
{
    YAML::Node node_enumtype;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::EnumType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        switch(node->get_signing()) {
        case Veriparse::AST::EnumType::DataType::SigningEnum::NONE:
            content["signing"] = "NONE";
            break;
        case Veriparse::AST::EnumType::DataType::SigningEnum::SIGNED:
            content["signing"] = "SIGNED";
            break;
        default:
            content["signing"] = "UNSIGNED";
        }

        content["base"] = render(node->get_base());

        if(node->get_items()) {
            content["items"] = YAML::Load("[]");
            for(const AST::EnumItem::Ptr &n : *node->get_items()) {
                content["items"].push_back(render(n));
            }
        }

        if(node->get_packed_dims()) {
            content["packed_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                content["packed_dims"].push_back(render(n));
            }
        }
    }

    node_enumtype["EnumType"] = content;
    return node_enumtype;
}

YAML::Node YAMLGenerator::render_enumitem(const AST::EnumItem::Ptr node) const
{
    YAML::Node node_enumitem;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::EnumItem) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["name"] = node->get_name();

        content["value"] = render(node->get_value());

        content["range"] = render(node->get_range());
    }

    node_enumitem["EnumItem"] = content;
    return node_enumitem;
}

YAML::Node YAMLGenerator::render_typeopexpr(const AST::TypeOpExpr::Ptr node) const
{
    YAML::Node node_typeopexpr;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::TypeOpExpr) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        switch(node->get_signing()) {
        case Veriparse::AST::TypeOpExpr::DataType::SigningEnum::NONE:
            content["signing"] = "NONE";
            break;
        case Veriparse::AST::TypeOpExpr::DataType::SigningEnum::SIGNED:
            content["signing"] = "SIGNED";
            break;
        default:
            content["signing"] = "UNSIGNED";
        }

        content["expr"] = render(node->get_expr());

        if(node->get_packed_dims()) {
            content["packed_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                content["packed_dims"].push_back(render(n));
            }
        }
    }

    node_typeopexpr["TypeOpExpr"] = content;
    return node_typeopexpr;
}

YAML::Node YAMLGenerator::render_typeoptype(const AST::TypeOpType::Ptr node) const
{
    YAML::Node node_typeoptype;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::TypeOpType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        switch(node->get_signing()) {
        case Veriparse::AST::TypeOpType::DataType::SigningEnum::NONE:
            content["signing"] = "NONE";
            break;
        case Veriparse::AST::TypeOpType::DataType::SigningEnum::SIGNED:
            content["signing"] = "SIGNED";
            break;
        default:
            content["signing"] = "UNSIGNED";
        }

        content["type"] = render(node->get_type());

        if(node->get_packed_dims()) {
            content["packed_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                content["packed_dims"].push_back(render(n));
            }
        }
    }

    node_typeoptype["TypeOpType"] = content;
    return node_typeoptype;
}

YAML::Node YAMLGenerator::render_dimension(const AST::Dimension::Ptr node) const
{
    YAML::Node node_dimension;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Dimension) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
    }

    node_dimension["Dimension"] = content;
    return node_dimension;
}

YAML::Node YAMLGenerator::render_rangedim(const AST::RangeDim::Ptr node) const
{
    YAML::Node node_rangedim;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::RangeDim) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["left"] = render(node->get_left());

        content["right"] = render(node->get_right());
    }

    node_rangedim["RangeDim"] = content;
    return node_rangedim;
}

YAML::Node YAMLGenerator::render_sizedim(const AST::SizeDim::Ptr node) const
{
    YAML::Node node_sizedim;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::SizeDim) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["size"] = render(node->get_size());
    }

    node_sizedim["SizeDim"] = content;
    return node_sizedim;
}

YAML::Node YAMLGenerator::render_unsizeddim(const AST::UnsizedDim::Ptr node) const
{
    YAML::Node node_unsizeddim;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::UnsizedDim) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
    }

    node_unsizeddim["UnsizedDim"] = content;
    return node_unsizeddim;
}

YAML::Node YAMLGenerator::render_queuedim(const AST::QueueDim::Ptr node) const
{
    YAML::Node node_queuedim;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::QueueDim) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["bound"] = render(node->get_bound());
    }

    node_queuedim["QueueDim"] = content;
    return node_queuedim;
}

YAML::Node YAMLGenerator::render_assocdim(const AST::AssocDim::Ptr node) const
{
    YAML::Node node_assocdim;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::AssocDim) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["index_type"] = render(node->get_index_type());
    }

    node_assocdim["AssocDim"] = content;
    return node_assocdim;
}

YAML::Node YAMLGenerator::render_declaration(const AST::Declaration::Ptr node) const
{
    YAML::Node node_declaration;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Declaration) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["name"] = node->get_name();

        content["type"] = render(node->get_type());
    }

    node_declaration["Declaration"] = content;
    return node_declaration;
}

YAML::Node YAMLGenerator::render_var(const AST::Var::Ptr node) const
{
    YAML::Node node_var;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Var) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["is_var"] = node->get_is_var();
        content["is_const"] = node->get_is_const();
        switch(node->get_lifetime()) {
        case Veriparse::AST::Var::LifetimeEnum::NONE:
            content["lifetime"] = "NONE";
            break;
        case Veriparse::AST::Var::LifetimeEnum::AUTOMATIC:
            content["lifetime"] = "AUTOMATIC";
            break;
        default:
            content["lifetime"] = "STATIC";
        }
        content["name"] = node->get_name();

        if(node->get_unpacked_dims()) {
            content["unpacked_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                content["unpacked_dims"].push_back(render(n));
            }
        }

        content["init"] = render(node->get_init());

        content["type"] = render(node->get_type());
    }

    node_var["Var"] = content;
    return node_var;
}

YAML::Node YAMLGenerator::render_net(const AST::Net::Ptr node) const
{
    YAML::Node node_net;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Net) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["is_vectored"] = node->get_is_vectored();
        content["is_scalared"] = node->get_is_scalared();
        content["name"] = node->get_name();

        if(node->get_unpacked_dims()) {
            content["unpacked_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                content["unpacked_dims"].push_back(render(n));
            }
        }

        content["cont_assign"] = render(node->get_cont_assign());

        content["strength"] = render(node->get_strength());

        content["ldelay"] = render(node->get_ldelay());

        content["rdelay"] = render(node->get_rdelay());

        content["type"] = render(node->get_type());
    }

    node_net["Net"] = content;
    return node_net;
}

YAML::Node YAMLGenerator::render_wirenet(const AST::WireNet::Ptr node) const
{
    YAML::Node node_wirenet;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::WireNet) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["is_vectored"] = node->get_is_vectored();
        content["is_scalared"] = node->get_is_scalared();
        content["name"] = node->get_name();

        if(node->get_unpacked_dims()) {
            content["unpacked_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                content["unpacked_dims"].push_back(render(n));
            }
        }

        content["cont_assign"] = render(node->get_cont_assign());

        content["strength"] = render(node->get_strength());

        content["ldelay"] = render(node->get_ldelay());

        content["rdelay"] = render(node->get_rdelay());

        content["type"] = render(node->get_type());
    }

    node_wirenet["WireNet"] = content;
    return node_wirenet;
}

YAML::Node YAMLGenerator::render_trinet(const AST::TriNet::Ptr node) const
{
    YAML::Node node_trinet;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::TriNet) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["is_vectored"] = node->get_is_vectored();
        content["is_scalared"] = node->get_is_scalared();
        content["name"] = node->get_name();

        if(node->get_unpacked_dims()) {
            content["unpacked_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                content["unpacked_dims"].push_back(render(n));
            }
        }

        content["cont_assign"] = render(node->get_cont_assign());

        content["strength"] = render(node->get_strength());

        content["ldelay"] = render(node->get_ldelay());

        content["rdelay"] = render(node->get_rdelay());

        content["type"] = render(node->get_type());
    }

    node_trinet["TriNet"] = content;
    return node_trinet;
}

YAML::Node YAMLGenerator::render_tri0net(const AST::Tri0Net::Ptr node) const
{
    YAML::Node node_tri0net;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Tri0Net) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["is_vectored"] = node->get_is_vectored();
        content["is_scalared"] = node->get_is_scalared();
        content["name"] = node->get_name();

        if(node->get_unpacked_dims()) {
            content["unpacked_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                content["unpacked_dims"].push_back(render(n));
            }
        }

        content["cont_assign"] = render(node->get_cont_assign());

        content["strength"] = render(node->get_strength());

        content["ldelay"] = render(node->get_ldelay());

        content["rdelay"] = render(node->get_rdelay());

        content["type"] = render(node->get_type());
    }

    node_tri0net["Tri0Net"] = content;
    return node_tri0net;
}

YAML::Node YAMLGenerator::render_tri1net(const AST::Tri1Net::Ptr node) const
{
    YAML::Node node_tri1net;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Tri1Net) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["is_vectored"] = node->get_is_vectored();
        content["is_scalared"] = node->get_is_scalared();
        content["name"] = node->get_name();

        if(node->get_unpacked_dims()) {
            content["unpacked_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                content["unpacked_dims"].push_back(render(n));
            }
        }

        content["cont_assign"] = render(node->get_cont_assign());

        content["strength"] = render(node->get_strength());

        content["ldelay"] = render(node->get_ldelay());

        content["rdelay"] = render(node->get_rdelay());

        content["type"] = render(node->get_type());
    }

    node_tri1net["Tri1Net"] = content;
    return node_tri1net;
}

YAML::Node YAMLGenerator::render_triandnet(const AST::TriandNet::Ptr node) const
{
    YAML::Node node_triandnet;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::TriandNet) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["is_vectored"] = node->get_is_vectored();
        content["is_scalared"] = node->get_is_scalared();
        content["name"] = node->get_name();

        if(node->get_unpacked_dims()) {
            content["unpacked_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                content["unpacked_dims"].push_back(render(n));
            }
        }

        content["cont_assign"] = render(node->get_cont_assign());

        content["strength"] = render(node->get_strength());

        content["ldelay"] = render(node->get_ldelay());

        content["rdelay"] = render(node->get_rdelay());

        content["type"] = render(node->get_type());
    }

    node_triandnet["TriandNet"] = content;
    return node_triandnet;
}

YAML::Node YAMLGenerator::render_triornet(const AST::TriorNet::Ptr node) const
{
    YAML::Node node_triornet;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::TriorNet) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["is_vectored"] = node->get_is_vectored();
        content["is_scalared"] = node->get_is_scalared();
        content["name"] = node->get_name();

        if(node->get_unpacked_dims()) {
            content["unpacked_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                content["unpacked_dims"].push_back(render(n));
            }
        }

        content["cont_assign"] = render(node->get_cont_assign());

        content["strength"] = render(node->get_strength());

        content["ldelay"] = render(node->get_ldelay());

        content["rdelay"] = render(node->get_rdelay());

        content["type"] = render(node->get_type());
    }

    node_triornet["TriorNet"] = content;
    return node_triornet;
}

YAML::Node YAMLGenerator::render_triregnet(const AST::TriregNet::Ptr node) const
{
    YAML::Node node_triregnet;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::TriregNet) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["is_vectored"] = node->get_is_vectored();
        content["is_scalared"] = node->get_is_scalared();
        content["name"] = node->get_name();

        if(node->get_unpacked_dims()) {
            content["unpacked_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                content["unpacked_dims"].push_back(render(n));
            }
        }

        content["cont_assign"] = render(node->get_cont_assign());

        content["strength"] = render(node->get_strength());

        content["ldelay"] = render(node->get_ldelay());

        content["rdelay"] = render(node->get_rdelay());

        content["type"] = render(node->get_type());
    }

    node_triregnet["TriregNet"] = content;
    return node_triregnet;
}

YAML::Node YAMLGenerator::render_wandnet(const AST::WandNet::Ptr node) const
{
    YAML::Node node_wandnet;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::WandNet) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["is_vectored"] = node->get_is_vectored();
        content["is_scalared"] = node->get_is_scalared();
        content["name"] = node->get_name();

        if(node->get_unpacked_dims()) {
            content["unpacked_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                content["unpacked_dims"].push_back(render(n));
            }
        }

        content["cont_assign"] = render(node->get_cont_assign());

        content["strength"] = render(node->get_strength());

        content["ldelay"] = render(node->get_ldelay());

        content["rdelay"] = render(node->get_rdelay());

        content["type"] = render(node->get_type());
    }

    node_wandnet["WandNet"] = content;
    return node_wandnet;
}

YAML::Node YAMLGenerator::render_wornet(const AST::WorNet::Ptr node) const
{
    YAML::Node node_wornet;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::WorNet) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["is_vectored"] = node->get_is_vectored();
        content["is_scalared"] = node->get_is_scalared();
        content["name"] = node->get_name();

        if(node->get_unpacked_dims()) {
            content["unpacked_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                content["unpacked_dims"].push_back(render(n));
            }
        }

        content["cont_assign"] = render(node->get_cont_assign());

        content["strength"] = render(node->get_strength());

        content["ldelay"] = render(node->get_ldelay());

        content["rdelay"] = render(node->get_rdelay());

        content["type"] = render(node->get_type());
    }

    node_wornet["WorNet"] = content;
    return node_wornet;
}

YAML::Node YAMLGenerator::render_uwirenet(const AST::UwireNet::Ptr node) const
{
    YAML::Node node_uwirenet;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::UwireNet) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["is_vectored"] = node->get_is_vectored();
        content["is_scalared"] = node->get_is_scalared();
        content["name"] = node->get_name();

        if(node->get_unpacked_dims()) {
            content["unpacked_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                content["unpacked_dims"].push_back(render(n));
            }
        }

        content["cont_assign"] = render(node->get_cont_assign());

        content["strength"] = render(node->get_strength());

        content["ldelay"] = render(node->get_ldelay());

        content["rdelay"] = render(node->get_rdelay());

        content["type"] = render(node->get_type());
    }

    node_uwirenet["UwireNet"] = content;
    return node_uwirenet;
}

YAML::Node YAMLGenerator::render_supply0net(const AST::Supply0Net::Ptr node) const
{
    YAML::Node node_supply0net;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Supply0Net) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["is_vectored"] = node->get_is_vectored();
        content["is_scalared"] = node->get_is_scalared();
        content["name"] = node->get_name();

        if(node->get_unpacked_dims()) {
            content["unpacked_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                content["unpacked_dims"].push_back(render(n));
            }
        }

        content["cont_assign"] = render(node->get_cont_assign());

        content["strength"] = render(node->get_strength());

        content["ldelay"] = render(node->get_ldelay());

        content["rdelay"] = render(node->get_rdelay());

        content["type"] = render(node->get_type());
    }

    node_supply0net["Supply0Net"] = content;
    return node_supply0net;
}

YAML::Node YAMLGenerator::render_supply1net(const AST::Supply1Net::Ptr node) const
{
    YAML::Node node_supply1net;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Supply1Net) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["is_vectored"] = node->get_is_vectored();
        content["is_scalared"] = node->get_is_scalared();
        content["name"] = node->get_name();

        if(node->get_unpacked_dims()) {
            content["unpacked_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                content["unpacked_dims"].push_back(render(n));
            }
        }

        content["cont_assign"] = render(node->get_cont_assign());

        content["strength"] = render(node->get_strength());

        content["ldelay"] = render(node->get_ldelay());

        content["rdelay"] = render(node->get_rdelay());

        content["type"] = render(node->get_type());
    }

    node_supply1net["Supply1Net"] = content;
    return node_supply1net;
}

YAML::Node YAMLGenerator::render_interconnectnet(const AST::InterconnectNet::Ptr node) const
{
    YAML::Node node_interconnectnet;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::InterconnectNet) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["is_vectored"] = node->get_is_vectored();
        content["is_scalared"] = node->get_is_scalared();
        content["name"] = node->get_name();

        if(node->get_unpacked_dims()) {
            content["unpacked_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                content["unpacked_dims"].push_back(render(n));
            }
        }

        content["cont_assign"] = render(node->get_cont_assign());

        content["strength"] = render(node->get_strength());

        content["ldelay"] = render(node->get_ldelay());

        content["rdelay"] = render(node->get_rdelay());

        content["type"] = render(node->get_type());
    }

    node_interconnectnet["InterconnectNet"] = content;
    return node_interconnectnet;
}

YAML::Node YAMLGenerator::render_usernet(const AST::UserNet::Ptr node) const
{
    YAML::Node node_usernet;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::UserNet) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["is_vectored"] = node->get_is_vectored();
        content["is_scalared"] = node->get_is_scalared();
        content["name"] = node->get_name();

        if(node->get_unpacked_dims()) {
            content["unpacked_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                content["unpacked_dims"].push_back(render(n));
            }
        }

        content["cont_assign"] = render(node->get_cont_assign());

        content["strength"] = render(node->get_strength());

        content["ldelay"] = render(node->get_ldelay());

        content["rdelay"] = render(node->get_rdelay());

        content["type"] = render(node->get_type());
    }

    node_usernet["UserNet"] = content;
    return node_usernet;
}

YAML::Node YAMLGenerator::render_implicitnet(const AST::ImplicitNet::Ptr node) const
{
    YAML::Node node_implicitnet;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::ImplicitNet) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["is_vectored"] = node->get_is_vectored();
        content["is_scalared"] = node->get_is_scalared();
        content["name"] = node->get_name();

        if(node->get_unpacked_dims()) {
            content["unpacked_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                content["unpacked_dims"].push_back(render(n));
            }
        }

        content["cont_assign"] = render(node->get_cont_assign());

        content["strength"] = render(node->get_strength());

        content["ldelay"] = render(node->get_ldelay());

        content["rdelay"] = render(node->get_rdelay());

        content["type"] = render(node->get_type());
    }

    node_implicitnet["ImplicitNet"] = content;
    return node_implicitnet;
}

YAML::Node YAMLGenerator::render_nettypedecl(const AST::NetTypeDecl::Ptr node) const
{
    YAML::Node node_nettypedecl;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::NetTypeDecl) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["name"] = node->get_name();

        content["resolver"] = render(node->get_resolver());

        content["type"] = render(node->get_type());
    }

    node_nettypedecl["NetTypeDecl"] = content;
    return node_nettypedecl;
}

YAML::Node YAMLGenerator::render_strength(const AST::Strength::Ptr node) const
{
    YAML::Node node_strength;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Strength) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
    }

    node_strength["Strength"] = content;
    return node_strength;
}

YAML::Node YAMLGenerator::render_drivestrength(const AST::DriveStrength::Ptr node) const
{
    YAML::Node node_drivestrength;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::DriveStrength) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        switch(node->get_s0()) {
        case Veriparse::AST::DriveStrength::S0Enum::SUPPLY0:
            content["s0"] = "SUPPLY0";
            break;
        case Veriparse::AST::DriveStrength::S0Enum::STRONG0:
            content["s0"] = "STRONG0";
            break;
        case Veriparse::AST::DriveStrength::S0Enum::PULL0:
            content["s0"] = "PULL0";
            break;
        case Veriparse::AST::DriveStrength::S0Enum::WEAK0:
            content["s0"] = "WEAK0";
            break;
        default:
            content["s0"] = "HIGHZ0";
        }
        switch(node->get_s1()) {
        case Veriparse::AST::DriveStrength::S1Enum::SUPPLY1:
            content["s1"] = "SUPPLY1";
            break;
        case Veriparse::AST::DriveStrength::S1Enum::STRONG1:
            content["s1"] = "STRONG1";
            break;
        case Veriparse::AST::DriveStrength::S1Enum::PULL1:
            content["s1"] = "PULL1";
            break;
        case Veriparse::AST::DriveStrength::S1Enum::WEAK1:
            content["s1"] = "WEAK1";
            break;
        default:
            content["s1"] = "HIGHZ1";
        }
    }

    node_drivestrength["DriveStrength"] = content;
    return node_drivestrength;
}

YAML::Node YAMLGenerator::render_chargestrength(const AST::ChargeStrength::Ptr node) const
{
    YAML::Node node_chargestrength;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::ChargeStrength) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        switch(node->get_charge()) {
        case Veriparse::AST::ChargeStrength::ChargeEnum::SMALL:
            content["charge"] = "SMALL";
            break;
        case Veriparse::AST::ChargeStrength::ChargeEnum::MEDIUM:
            content["charge"] = "MEDIUM";
            break;
        default:
            content["charge"] = "LARGE";
        }
    }

    node_chargestrength["ChargeStrength"] = content;
    return node_chargestrength;
}

YAML::Node YAMLGenerator::render_param(const AST::Param::Ptr node) const
{
    YAML::Node node_param;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Param) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["is_local"] = node->get_is_local();
        content["name"] = node->get_name();

        content["value"] = render(node->get_value());

        if(node->get_unpacked_dims()) {
            content["unpacked_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                content["unpacked_dims"].push_back(render(n));
            }
        }

        content["type"] = render(node->get_type());
    }

    node_param["Param"] = content;
    return node_param;
}

YAML::Node YAMLGenerator::render_typeparam(const AST::TypeParam::Ptr node) const
{
    YAML::Node node_typeparam;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::TypeParam) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["is_local"] = node->get_is_local();
        content["name"] = node->get_name();

        content["type"] = render(node->get_type());
    }

    node_typeparam["TypeParam"] = content;
    return node_typeparam;
}

YAML::Node YAMLGenerator::render_typedef(const AST::Typedef::Ptr node) const
{
    YAML::Node node_typedef;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Typedef) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        switch(node->get_fwd_kind()) {
        case Veriparse::AST::Typedef::Fwd_kindEnum::NONE:
            content["fwd_kind"] = "NONE";
            break;
        case Veriparse::AST::Typedef::Fwd_kindEnum::ENUM:
            content["fwd_kind"] = "ENUM";
            break;
        case Veriparse::AST::Typedef::Fwd_kindEnum::STRUCT:
            content["fwd_kind"] = "STRUCT";
            break;
        case Veriparse::AST::Typedef::Fwd_kindEnum::UNION:
            content["fwd_kind"] = "UNION";
            break;
        case Veriparse::AST::Typedef::Fwd_kindEnum::CLASS:
            content["fwd_kind"] = "CLASS";
            break;
        default:
            content["fwd_kind"] = "INTERFACE_CLASS";
        }
        content["name"] = node->get_name();

        if(node->get_unpacked_dims()) {
            content["unpacked_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                content["unpacked_dims"].push_back(render(n));
            }
        }

        content["type"] = render(node->get_type());
    }

    node_typedef["Typedef"] = content;
    return node_typedef;
}

YAML::Node YAMLGenerator::render_member(const AST::Member::Ptr node) const
{
    YAML::Node node_member;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Member) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["name"] = node->get_name();

        if(node->get_unpacked_dims()) {
            content["unpacked_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                content["unpacked_dims"].push_back(render(n));
            }
        }

        content["init"] = render(node->get_init());

        content["type"] = render(node->get_type());
    }

    node_member["Member"] = content;
    return node_member;
}

YAML::Node YAMLGenerator::render_arg(const AST::Arg::Ptr node) const
{
    YAML::Node node_arg;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Arg) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["is_var"] = node->get_is_var();
        switch(node->get_direction()) {
        case Veriparse::AST::Arg::DirectionEnum::NONE:
            content["direction"] = "NONE";
            break;
        case Veriparse::AST::Arg::DirectionEnum::INPUT:
            content["direction"] = "INPUT";
            break;
        case Veriparse::AST::Arg::DirectionEnum::OUTPUT:
            content["direction"] = "OUTPUT";
            break;
        case Veriparse::AST::Arg::DirectionEnum::INOUT:
            content["direction"] = "INOUT";
            break;
        case Veriparse::AST::Arg::DirectionEnum::REF:
            content["direction"] = "REF";
            break;
        default:
            content["direction"] = "CONST_REF";
        }
        content["name"] = node->get_name();

        if(node->get_unpacked_dims()) {
            content["unpacked_dims"] = YAML::Load("[]");
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                content["unpacked_dims"].push_back(render(n));
            }
        }

        content["default_value"] = render(node->get_default_value());

        content["type"] = render(node->get_type());
    }

    node_arg["Arg"] = content;
    return node_arg;
}

YAML::Node YAMLGenerator::render_genvar(const AST::Genvar::Ptr node) const
{
    YAML::Node node_genvar;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Genvar) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["name"] = node->get_name();
    }

    node_genvar["Genvar"] = content;
    return node_genvar;
}

YAML::Node YAMLGenerator::render_concat(const AST::Concat::Ptr node) const
{
    YAML::Node node_concat;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Concat) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        if(node->get_list()) {
            content["list"] = YAML::Load("[]");
            for(const AST::Node::Ptr &n : *node->get_list()) {
                content["list"].push_back(render(n));
            }
        }
    }

    node_concat["Concat"] = content;
    return node_concat;
}

YAML::Node YAMLGenerator::render_lconcat(const AST::Lconcat::Ptr node) const
{
    YAML::Node node_lconcat;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Lconcat) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        if(node->get_list()) {
            content["list"] = YAML::Load("[]");
            for(const AST::Node::Ptr &n : *node->get_list()) {
                content["list"].push_back(render(n));
            }
        }
    }

    node_lconcat["Lconcat"] = content;
    return node_lconcat;
}

YAML::Node YAMLGenerator::render_repeat(const AST::Repeat::Ptr node) const
{
    YAML::Node node_repeat;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Repeat) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["value"] = render(node->get_value());

        content["times"] = render(node->get_times());
    }

    node_repeat["Repeat"] = content;
    return node_repeat;
}

YAML::Node YAMLGenerator::render_assignmentpattern(const AST::AssignmentPattern::Ptr node) const
{
    YAML::Node node_assignmentpattern;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::AssignmentPattern) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        if(node->get_items()) {
            content["items"] = YAML::Load("[]");
            for(const AST::Node::Ptr &n : *node->get_items()) {
                content["items"].push_back(render(n));
            }
        }

        content["times"] = render(node->get_times());
    }

    node_assignmentpattern["AssignmentPattern"] = content;
    return node_assignmentpattern;
}

YAML::Node YAMLGenerator::render_patternitem(const AST::PatternItem::Ptr node) const
{
    YAML::Node node_patternitem;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::PatternItem) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["is_default"] = node->get_is_default();

        content["key"] = render(node->get_key());

        content["value"] = render(node->get_value());
    }

    node_patternitem["PatternItem"] = content;
    return node_patternitem;
}

YAML::Node YAMLGenerator::render_cast(const AST::Cast::Ptr node) const
{
    YAML::Node node_cast;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Cast) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["expr"] = render(node->get_expr());
    }

    node_cast["Cast"] = content;
    return node_cast;
}

YAML::Node YAMLGenerator::render_typecast(const AST::TypeCast::Ptr node) const
{
    YAML::Node node_typecast;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::TypeCast) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["target"] = render(node->get_target());

        content["expr"] = render(node->get_expr());
    }

    node_typecast["TypeCast"] = content;
    return node_typecast;
}

YAML::Node YAMLGenerator::render_sizecast(const AST::SizeCast::Ptr node) const
{
    YAML::Node node_sizecast;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::SizeCast) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["size"] = render(node->get_size());

        content["expr"] = render(node->get_expr());
    }

    node_sizecast["SizeCast"] = content;
    return node_sizecast;
}

YAML::Node YAMLGenerator::render_signingcast(const AST::SigningCast::Ptr node) const
{
    YAML::Node node_signingcast;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::SigningCast) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        switch(node->get_signing()) {
        case Veriparse::AST::SigningCast::SigningEnum::SIGNED:
            content["signing"] = "SIGNED";
            break;
        default:
            content["signing"] = "UNSIGNED";
        }

        content["expr"] = render(node->get_expr());
    }

    node_signingcast["SigningCast"] = content;
    return node_signingcast;
}

YAML::Node YAMLGenerator::render_constcast(const AST::ConstCast::Ptr node) const
{
    YAML::Node node_constcast;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::ConstCast) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["expr"] = render(node->get_expr());
    }

    node_constcast["ConstCast"] = content;
    return node_constcast;
}

YAML::Node YAMLGenerator::render_indirect(const AST::Indirect::Ptr node) const
{
    YAML::Node node_indirect;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Indirect) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["var"] = render(node->get_var());
    }

    node_indirect["Indirect"] = content;
    return node_indirect;
}

YAML::Node YAMLGenerator::render_partselect(const AST::Partselect::Ptr node) const
{
    YAML::Node node_partselect;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Partselect) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["msb"] = render(node->get_msb());

        content["lsb"] = render(node->get_lsb());

        content["var"] = render(node->get_var());
    }

    node_partselect["Partselect"] = content;
    return node_partselect;
}

YAML::Node YAMLGenerator::render_partselectindexed(const AST::PartselectIndexed::Ptr node) const
{
    YAML::Node node_partselectindexed;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::PartselectIndexed) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["index"] = render(node->get_index());

        content["size"] = render(node->get_size());

        content["var"] = render(node->get_var());
    }

    node_partselectindexed["PartselectIndexed"] = content;
    return node_partselectindexed;
}

YAML::Node
YAMLGenerator::render_partselectplusindexed(const AST::PartselectPlusIndexed::Ptr node) const
{
    YAML::Node node_partselectplusindexed;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::PartselectPlusIndexed) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["index"] = render(node->get_index());

        content["size"] = render(node->get_size());

        content["var"] = render(node->get_var());
    }

    node_partselectplusindexed["PartselectPlusIndexed"] = content;
    return node_partselectplusindexed;
}

YAML::Node
YAMLGenerator::render_partselectminusindexed(const AST::PartselectMinusIndexed::Ptr node) const
{
    YAML::Node node_partselectminusindexed;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::PartselectMinusIndexed) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["index"] = render(node->get_index());

        content["size"] = render(node->get_size());

        content["var"] = render(node->get_var());
    }

    node_partselectminusindexed["PartselectMinusIndexed"] = content;
    return node_partselectminusindexed;
}

YAML::Node YAMLGenerator::render_pointer(const AST::Pointer::Ptr node) const
{
    YAML::Node node_pointer;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Pointer) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["ptr"] = render(node->get_ptr());

        content["var"] = render(node->get_var());
    }

    node_pointer["Pointer"] = content;
    return node_pointer;
}

YAML::Node YAMLGenerator::render_lvalue(const AST::Lvalue::Ptr node) const
{
    YAML::Node node_lvalue;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Lvalue) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["var"] = render(node->get_var());
    }

    node_lvalue["Lvalue"] = content;
    return node_lvalue;
}

YAML::Node YAMLGenerator::render_rvalue(const AST::Rvalue::Ptr node) const
{
    YAML::Node node_rvalue;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Rvalue) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["var"] = render(node->get_var());
    }

    node_rvalue["Rvalue"] = content;
    return node_rvalue;
}

YAML::Node YAMLGenerator::render_unaryoperator(const AST::UnaryOperator::Ptr node) const
{
    YAML::Node node_unaryoperator;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::UnaryOperator) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["right"] = render(node->get_right());
    }

    node_unaryoperator["UnaryOperator"] = content;
    return node_unaryoperator;
}

YAML::Node YAMLGenerator::render_uplus(const AST::Uplus::Ptr node) const
{
    YAML::Node node_uplus;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Uplus) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["right"] = render(node->get_right());
    }

    node_uplus["Uplus"] = content;
    return node_uplus;
}

YAML::Node YAMLGenerator::render_uminus(const AST::Uminus::Ptr node) const
{
    YAML::Node node_uminus;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Uminus) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["right"] = render(node->get_right());
    }

    node_uminus["Uminus"] = content;
    return node_uminus;
}

YAML::Node YAMLGenerator::render_ulnot(const AST::Ulnot::Ptr node) const
{
    YAML::Node node_ulnot;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Ulnot) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["right"] = render(node->get_right());
    }

    node_ulnot["Ulnot"] = content;
    return node_ulnot;
}

YAML::Node YAMLGenerator::render_unot(const AST::Unot::Ptr node) const
{
    YAML::Node node_unot;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Unot) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["right"] = render(node->get_right());
    }

    node_unot["Unot"] = content;
    return node_unot;
}

YAML::Node YAMLGenerator::render_uand(const AST::Uand::Ptr node) const
{
    YAML::Node node_uand;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Uand) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["right"] = render(node->get_right());
    }

    node_uand["Uand"] = content;
    return node_uand;
}

YAML::Node YAMLGenerator::render_unand(const AST::Unand::Ptr node) const
{
    YAML::Node node_unand;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Unand) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["right"] = render(node->get_right());
    }

    node_unand["Unand"] = content;
    return node_unand;
}

YAML::Node YAMLGenerator::render_uor(const AST::Uor::Ptr node) const
{
    YAML::Node node_uor;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Uor) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["right"] = render(node->get_right());
    }

    node_uor["Uor"] = content;
    return node_uor;
}

YAML::Node YAMLGenerator::render_unor(const AST::Unor::Ptr node) const
{
    YAML::Node node_unor;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Unor) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["right"] = render(node->get_right());
    }

    node_unor["Unor"] = content;
    return node_unor;
}

YAML::Node YAMLGenerator::render_uxor(const AST::Uxor::Ptr node) const
{
    YAML::Node node_uxor;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Uxor) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["right"] = render(node->get_right());
    }

    node_uxor["Uxor"] = content;
    return node_uxor;
}

YAML::Node YAMLGenerator::render_uxnor(const AST::Uxnor::Ptr node) const
{
    YAML::Node node_uxnor;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Uxnor) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["right"] = render(node->get_right());
    }

    node_uxnor["Uxnor"] = content;
    return node_uxnor;
}

YAML::Node YAMLGenerator::render_operator(const AST::Operator::Ptr node) const
{
    YAML::Node node_operator;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Operator) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["left"] = render(node->get_left());

        content["right"] = render(node->get_right());
    }

    node_operator["Operator"] = content;
    return node_operator;
}

YAML::Node YAMLGenerator::render_power(const AST::Power::Ptr node) const
{
    YAML::Node node_power;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Power) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["left"] = render(node->get_left());

        content["right"] = render(node->get_right());
    }

    node_power["Power"] = content;
    return node_power;
}

YAML::Node YAMLGenerator::render_times(const AST::Times::Ptr node) const
{
    YAML::Node node_times;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Times) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["left"] = render(node->get_left());

        content["right"] = render(node->get_right());
    }

    node_times["Times"] = content;
    return node_times;
}

YAML::Node YAMLGenerator::render_divide(const AST::Divide::Ptr node) const
{
    YAML::Node node_divide;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Divide) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["left"] = render(node->get_left());

        content["right"] = render(node->get_right());
    }

    node_divide["Divide"] = content;
    return node_divide;
}

YAML::Node YAMLGenerator::render_mod(const AST::Mod::Ptr node) const
{
    YAML::Node node_mod;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Mod) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["left"] = render(node->get_left());

        content["right"] = render(node->get_right());
    }

    node_mod["Mod"] = content;
    return node_mod;
}

YAML::Node YAMLGenerator::render_plus(const AST::Plus::Ptr node) const
{
    YAML::Node node_plus;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Plus) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["left"] = render(node->get_left());

        content["right"] = render(node->get_right());
    }

    node_plus["Plus"] = content;
    return node_plus;
}

YAML::Node YAMLGenerator::render_minus(const AST::Minus::Ptr node) const
{
    YAML::Node node_minus;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Minus) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["left"] = render(node->get_left());

        content["right"] = render(node->get_right());
    }

    node_minus["Minus"] = content;
    return node_minus;
}

YAML::Node YAMLGenerator::render_sll(const AST::Sll::Ptr node) const
{
    YAML::Node node_sll;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Sll) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["left"] = render(node->get_left());

        content["right"] = render(node->get_right());
    }

    node_sll["Sll"] = content;
    return node_sll;
}

YAML::Node YAMLGenerator::render_srl(const AST::Srl::Ptr node) const
{
    YAML::Node node_srl;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Srl) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["left"] = render(node->get_left());

        content["right"] = render(node->get_right());
    }

    node_srl["Srl"] = content;
    return node_srl;
}

YAML::Node YAMLGenerator::render_sra(const AST::Sra::Ptr node) const
{
    YAML::Node node_sra;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Sra) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["left"] = render(node->get_left());

        content["right"] = render(node->get_right());
    }

    node_sra["Sra"] = content;
    return node_sra;
}

YAML::Node YAMLGenerator::render_lessthan(const AST::LessThan::Ptr node) const
{
    YAML::Node node_lessthan;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::LessThan) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["left"] = render(node->get_left());

        content["right"] = render(node->get_right());
    }

    node_lessthan["LessThan"] = content;
    return node_lessthan;
}

YAML::Node YAMLGenerator::render_greaterthan(const AST::GreaterThan::Ptr node) const
{
    YAML::Node node_greaterthan;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::GreaterThan) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["left"] = render(node->get_left());

        content["right"] = render(node->get_right());
    }

    node_greaterthan["GreaterThan"] = content;
    return node_greaterthan;
}

YAML::Node YAMLGenerator::render_lesseq(const AST::LessEq::Ptr node) const
{
    YAML::Node node_lesseq;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::LessEq) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["left"] = render(node->get_left());

        content["right"] = render(node->get_right());
    }

    node_lesseq["LessEq"] = content;
    return node_lesseq;
}

YAML::Node YAMLGenerator::render_greatereq(const AST::GreaterEq::Ptr node) const
{
    YAML::Node node_greatereq;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::GreaterEq) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["left"] = render(node->get_left());

        content["right"] = render(node->get_right());
    }

    node_greatereq["GreaterEq"] = content;
    return node_greatereq;
}

YAML::Node YAMLGenerator::render_eq(const AST::Eq::Ptr node) const
{
    YAML::Node node_eq;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Eq) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["left"] = render(node->get_left());

        content["right"] = render(node->get_right());
    }

    node_eq["Eq"] = content;
    return node_eq;
}

YAML::Node YAMLGenerator::render_noteq(const AST::NotEq::Ptr node) const
{
    YAML::Node node_noteq;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::NotEq) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["left"] = render(node->get_left());

        content["right"] = render(node->get_right());
    }

    node_noteq["NotEq"] = content;
    return node_noteq;
}

YAML::Node YAMLGenerator::render_eql(const AST::Eql::Ptr node) const
{
    YAML::Node node_eql;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Eql) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["left"] = render(node->get_left());

        content["right"] = render(node->get_right());
    }

    node_eql["Eql"] = content;
    return node_eql;
}

YAML::Node YAMLGenerator::render_noteql(const AST::NotEql::Ptr node) const
{
    YAML::Node node_noteql;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::NotEql) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["left"] = render(node->get_left());

        content["right"] = render(node->get_right());
    }

    node_noteql["NotEql"] = content;
    return node_noteql;
}

YAML::Node YAMLGenerator::render_and(const AST::And::Ptr node) const
{
    YAML::Node node_and;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::And) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["left"] = render(node->get_left());

        content["right"] = render(node->get_right());
    }

    node_and["And"] = content;
    return node_and;
}

YAML::Node YAMLGenerator::render_xor(const AST::Xor::Ptr node) const
{
    YAML::Node node_xor;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Xor) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["left"] = render(node->get_left());

        content["right"] = render(node->get_right());
    }

    node_xor["Xor"] = content;
    return node_xor;
}

YAML::Node YAMLGenerator::render_xnor(const AST::Xnor::Ptr node) const
{
    YAML::Node node_xnor;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Xnor) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["left"] = render(node->get_left());

        content["right"] = render(node->get_right());
    }

    node_xnor["Xnor"] = content;
    return node_xnor;
}

YAML::Node YAMLGenerator::render_or(const AST::Or::Ptr node) const
{
    YAML::Node node_or;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Or) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["left"] = render(node->get_left());

        content["right"] = render(node->get_right());
    }

    node_or["Or"] = content;
    return node_or;
}

YAML::Node YAMLGenerator::render_land(const AST::Land::Ptr node) const
{
    YAML::Node node_land;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Land) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["left"] = render(node->get_left());

        content["right"] = render(node->get_right());
    }

    node_land["Land"] = content;
    return node_land;
}

YAML::Node YAMLGenerator::render_lor(const AST::Lor::Ptr node) const
{
    YAML::Node node_lor;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Lor) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["left"] = render(node->get_left());

        content["right"] = render(node->get_right());
    }

    node_lor["Lor"] = content;
    return node_lor;
}

YAML::Node YAMLGenerator::render_cond(const AST::Cond::Ptr node) const
{
    YAML::Node node_cond;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Cond) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["cond"] = render(node->get_cond());

        content["left"] = render(node->get_left());

        content["right"] = render(node->get_right());
    }

    node_cond["Cond"] = content;
    return node_cond;
}

YAML::Node YAMLGenerator::render_always(const AST::Always::Ptr node) const
{
    YAML::Node node_always;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Always) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["senslist"] = render(node->get_senslist());

        content["statement"] = render(node->get_statement());
    }

    node_always["Always"] = content;
    return node_always;
}

YAML::Node YAMLGenerator::render_alwaysff(const AST::AlwaysFF::Ptr node) const
{
    YAML::Node node_alwaysff;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::AlwaysFF) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["senslist"] = render(node->get_senslist());

        content["statement"] = render(node->get_statement());
    }

    node_alwaysff["AlwaysFF"] = content;
    return node_alwaysff;
}

YAML::Node YAMLGenerator::render_alwayscomb(const AST::AlwaysComb::Ptr node) const
{
    YAML::Node node_alwayscomb;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::AlwaysComb) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["senslist"] = render(node->get_senslist());

        content["statement"] = render(node->get_statement());
    }

    node_alwayscomb["AlwaysComb"] = content;
    return node_alwayscomb;
}

YAML::Node YAMLGenerator::render_alwayslatch(const AST::AlwaysLatch::Ptr node) const
{
    YAML::Node node_alwayslatch;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::AlwaysLatch) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["senslist"] = render(node->get_senslist());

        content["statement"] = render(node->get_statement());
    }

    node_alwayslatch["AlwaysLatch"] = content;
    return node_alwayslatch;
}

YAML::Node YAMLGenerator::render_senslist(const AST::Senslist::Ptr node) const
{
    YAML::Node node_senslist;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Senslist) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        if(node->get_list()) {
            content["list"] = YAML::Load("[]");
            for(const AST::Sens::Ptr &n : *node->get_list()) {
                content["list"].push_back(render(n));
            }
        }
    }

    node_senslist["Senslist"] = content;
    return node_senslist;
}

YAML::Node YAMLGenerator::render_sens(const AST::Sens::Ptr node) const
{
    YAML::Node node_sens;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Sens) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        switch(node->get_type()) {
        case Veriparse::AST::Sens::TypeEnum::ALL:
            content["type"] = "ALL";
            break;
        case Veriparse::AST::Sens::TypeEnum::POSEDGE:
            content["type"] = "POSEDGE";
            break;
        case Veriparse::AST::Sens::TypeEnum::NEGEDGE:
            content["type"] = "NEGEDGE";
            break;
        default:
            content["type"] = "NONE";
        }

        content["sig"] = render(node->get_sig());
    }

    node_sens["Sens"] = content;
    return node_sens;
}

YAML::Node YAMLGenerator::render_defparamlist(const AST::Defparamlist::Ptr node) const
{
    YAML::Node node_defparamlist;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Defparamlist) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        if(node->get_list()) {
            content["list"] = YAML::Load("[]");
            for(const AST::Defparam::Ptr &n : *node->get_list()) {
                content["list"].push_back(render(n));
            }
        }
    }

    node_defparamlist["Defparamlist"] = content;
    return node_defparamlist;
}

YAML::Node YAMLGenerator::render_defparam(const AST::Defparam::Ptr node) const
{
    YAML::Node node_defparam;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Defparam) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["identifier"] = render(node->get_identifier());

        content["right"] = render(node->get_right());
    }

    node_defparam["Defparam"] = content;
    return node_defparam;
}

YAML::Node YAMLGenerator::render_assign(const AST::Assign::Ptr node) const
{
    YAML::Node node_assign;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Assign) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["left"] = render(node->get_left());

        content["right"] = render(node->get_right());

        content["ldelay"] = render(node->get_ldelay());

        content["rdelay"] = render(node->get_rdelay());
    }

    node_assign["Assign"] = content;
    return node_assign;
}

YAML::Node
YAMLGenerator::render_blockingsubstitution(const AST::BlockingSubstitution::Ptr node) const
{
    YAML::Node node_blockingsubstitution;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::BlockingSubstitution) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["left"] = render(node->get_left());

        content["right"] = render(node->get_right());

        content["ldelay"] = render(node->get_ldelay());

        content["rdelay"] = render(node->get_rdelay());
    }

    node_blockingsubstitution["BlockingSubstitution"] = content;
    return node_blockingsubstitution;
}

YAML::Node
YAMLGenerator::render_nonblockingsubstitution(const AST::NonblockingSubstitution::Ptr node) const
{
    YAML::Node node_nonblockingsubstitution;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::NonblockingSubstitution) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["left"] = render(node->get_left());

        content["right"] = render(node->get_right());

        content["ldelay"] = render(node->get_ldelay());

        content["rdelay"] = render(node->get_rdelay());
    }

    node_nonblockingsubstitution["NonblockingSubstitution"] = content;
    return node_nonblockingsubstitution;
}

YAML::Node YAMLGenerator::render_ifstatement(const AST::IfStatement::Ptr node) const
{
    YAML::Node node_ifstatement;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::IfStatement) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["cond"] = render(node->get_cond());

        content["true_statement"] = render(node->get_true_statement());

        content["false_statement"] = render(node->get_false_statement());
    }

    node_ifstatement["IfStatement"] = content;
    return node_ifstatement;
}

YAML::Node YAMLGenerator::render_repeatstatement(const AST::RepeatStatement::Ptr node) const
{
    YAML::Node node_repeatstatement;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::RepeatStatement) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["times"] = render(node->get_times());

        content["statement"] = render(node->get_statement());
    }

    node_repeatstatement["RepeatStatement"] = content;
    return node_repeatstatement;
}

YAML::Node YAMLGenerator::render_forstatement(const AST::ForStatement::Ptr node) const
{
    YAML::Node node_forstatement;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::ForStatement) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["pre"] = render(node->get_pre());

        content["cond"] = render(node->get_cond());

        content["post"] = render(node->get_post());

        content["statement"] = render(node->get_statement());
    }

    node_forstatement["ForStatement"] = content;
    return node_forstatement;
}

YAML::Node YAMLGenerator::render_whilestatement(const AST::WhileStatement::Ptr node) const
{
    YAML::Node node_whilestatement;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::WhileStatement) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["cond"] = render(node->get_cond());

        content["statement"] = render(node->get_statement());
    }

    node_whilestatement["WhileStatement"] = content;
    return node_whilestatement;
}

YAML::Node YAMLGenerator::render_casestatement(const AST::CaseStatement::Ptr node) const
{
    YAML::Node node_casestatement;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::CaseStatement) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["comp"] = render(node->get_comp());

        if(node->get_caselist()) {
            content["caselist"] = YAML::Load("[]");
            for(const AST::Case::Ptr &n : *node->get_caselist()) {
                content["caselist"].push_back(render(n));
            }
        }
    }

    node_casestatement["CaseStatement"] = content;
    return node_casestatement;
}

YAML::Node YAMLGenerator::render_casexstatement(const AST::CasexStatement::Ptr node) const
{
    YAML::Node node_casexstatement;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::CasexStatement) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["comp"] = render(node->get_comp());

        if(node->get_caselist()) {
            content["caselist"] = YAML::Load("[]");
            for(const AST::Case::Ptr &n : *node->get_caselist()) {
                content["caselist"].push_back(render(n));
            }
        }
    }

    node_casexstatement["CasexStatement"] = content;
    return node_casexstatement;
}

YAML::Node YAMLGenerator::render_casezstatement(const AST::CasezStatement::Ptr node) const
{
    YAML::Node node_casezstatement;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::CasezStatement) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["comp"] = render(node->get_comp());

        if(node->get_caselist()) {
            content["caselist"] = YAML::Load("[]");
            for(const AST::Case::Ptr &n : *node->get_caselist()) {
                content["caselist"].push_back(render(n));
            }
        }
    }

    node_casezstatement["CasezStatement"] = content;
    return node_casezstatement;
}

YAML::Node YAMLGenerator::render_uniquecasestatement(const AST::UniqueCaseStatement::Ptr node) const
{
    YAML::Node node_uniquecasestatement;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::UniqueCaseStatement) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["comp"] = render(node->get_comp());

        if(node->get_caselist()) {
            content["caselist"] = YAML::Load("[]");
            for(const AST::Case::Ptr &n : *node->get_caselist()) {
                content["caselist"].push_back(render(n));
            }
        }
    }

    node_uniquecasestatement["UniqueCaseStatement"] = content;
    return node_uniquecasestatement;
}

YAML::Node
YAMLGenerator::render_prioritycasestatement(const AST::PriorityCaseStatement::Ptr node) const
{
    YAML::Node node_prioritycasestatement;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::PriorityCaseStatement) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["comp"] = render(node->get_comp());

        if(node->get_caselist()) {
            content["caselist"] = YAML::Load("[]");
            for(const AST::Case::Ptr &n : *node->get_caselist()) {
                content["caselist"].push_back(render(n));
            }
        }
    }

    node_prioritycasestatement["PriorityCaseStatement"] = content;
    return node_prioritycasestatement;
}

YAML::Node YAMLGenerator::render_case(const AST::Case::Ptr node) const
{
    YAML::Node node_case;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Case) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        if(node->get_cond()) {
            content["cond"] = YAML::Load("[]");
            for(const AST::Node::Ptr &n : *node->get_cond()) {
                content["cond"].push_back(render(n));
            }
        }

        content["statement"] = render(node->get_statement());
    }

    node_case["Case"] = content;
    return node_case;
}

YAML::Node YAMLGenerator::render_block(const AST::Block::Ptr node) const
{
    YAML::Node node_block;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Block) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["scope"] = node->get_scope();

        if(node->get_statements()) {
            content["statements"] = YAML::Load("[]");
            for(const AST::Node::Ptr &n : *node->get_statements()) {
                content["statements"].push_back(render(n));
            }
        }
    }

    node_block["Block"] = content;
    return node_block;
}

YAML::Node YAMLGenerator::render_initial(const AST::Initial::Ptr node) const
{
    YAML::Node node_initial;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Initial) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["statement"] = render(node->get_statement());
    }

    node_initial["Initial"] = content;
    return node_initial;
}

YAML::Node YAMLGenerator::render_eventstatement(const AST::EventStatement::Ptr node) const
{
    YAML::Node node_eventstatement;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::EventStatement) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["senslist"] = render(node->get_senslist());

        content["statement"] = render(node->get_statement());
    }

    node_eventstatement["EventStatement"] = content;
    return node_eventstatement;
}

YAML::Node YAMLGenerator::render_waitstatement(const AST::WaitStatement::Ptr node) const
{
    YAML::Node node_waitstatement;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::WaitStatement) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["cond"] = render(node->get_cond());

        content["statement"] = render(node->get_statement());
    }

    node_waitstatement["WaitStatement"] = content;
    return node_waitstatement;
}

YAML::Node YAMLGenerator::render_foreverstatement(const AST::ForeverStatement::Ptr node) const
{
    YAML::Node node_foreverstatement;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::ForeverStatement) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["statement"] = render(node->get_statement());
    }

    node_foreverstatement["ForeverStatement"] = content;
    return node_foreverstatement;
}

YAML::Node YAMLGenerator::render_delaystatement(const AST::DelayStatement::Ptr node) const
{
    YAML::Node node_delaystatement;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::DelayStatement) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["delay"] = render(node->get_delay());
    }

    node_delaystatement["DelayStatement"] = content;
    return node_delaystatement;
}

YAML::Node YAMLGenerator::render_instancelist(const AST::Instancelist::Ptr node) const
{
    YAML::Node node_instancelist;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Instancelist) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["module"] = node->get_module();

        if(node->get_parameterlist()) {
            content["parameterlist"] = YAML::Load("[]");
            for(const AST::ParamArg::Ptr &n : *node->get_parameterlist()) {
                content["parameterlist"].push_back(render(n));
            }
        }

        if(node->get_instances()) {
            content["instances"] = YAML::Load("[]");
            for(const AST::Instance::Ptr &n : *node->get_instances()) {
                content["instances"].push_back(render(n));
            }
        }
    }

    node_instancelist["Instancelist"] = content;
    return node_instancelist;
}

YAML::Node YAMLGenerator::render_instance(const AST::Instance::Ptr node) const
{
    YAML::Node node_instance;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Instance) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["module"] = node->get_module();
        content["name"] = node->get_name();

        content["array"] = render(node->get_array());

        if(node->get_parameterlist()) {
            content["parameterlist"] = YAML::Load("[]");
            for(const AST::ParamArg::Ptr &n : *node->get_parameterlist()) {
                content["parameterlist"].push_back(render(n));
            }
        }

        if(node->get_portlist()) {
            content["portlist"] = YAML::Load("[]");
            for(const AST::PortArg::Ptr &n : *node->get_portlist()) {
                content["portlist"].push_back(render(n));
            }
        }
    }

    node_instance["Instance"] = content;
    return node_instance;
}

YAML::Node YAMLGenerator::render_paramarg(const AST::ParamArg::Ptr node) const
{
    YAML::Node node_paramarg;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::ParamArg) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["name"] = node->get_name();

        content["value"] = render(node->get_value());
    }

    node_paramarg["ParamArg"] = content;
    return node_paramarg;
}

YAML::Node YAMLGenerator::render_portarg(const AST::PortArg::Ptr node) const
{
    YAML::Node node_portarg;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::PortArg) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["name"] = node->get_name();

        content["value"] = render(node->get_value());
    }

    node_portarg["PortArg"] = content;
    return node_portarg;
}

YAML::Node YAMLGenerator::render_function(const AST::Function::Ptr node) const
{
    YAML::Node node_function;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Function) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["name"] = node->get_name();
        switch(node->get_lifetime()) {
        case Veriparse::AST::Function::LifetimeEnum::NONE:
            content["lifetime"] = "NONE";
            break;
        case Veriparse::AST::Function::LifetimeEnum::AUTOMATIC:
            content["lifetime"] = "AUTOMATIC";
            break;
        default:
            content["lifetime"] = "STATIC";
        }

        content["return_type"] = render(node->get_return_type());

        if(node->get_args()) {
            content["args"] = YAML::Load("[]");
            for(const AST::Arg::Ptr &n : *node->get_args()) {
                content["args"].push_back(render(n));
            }
        }

        if(node->get_statements()) {
            content["statements"] = YAML::Load("[]");
            for(const AST::Node::Ptr &n : *node->get_statements()) {
                content["statements"].push_back(render(n));
            }
        }
    }

    node_function["Function"] = content;
    return node_function;
}

YAML::Node YAMLGenerator::render_functioncall(const AST::FunctionCall::Ptr node) const
{
    YAML::Node node_functioncall;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::FunctionCall) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["name"] = node->get_name();

        if(node->get_args()) {
            content["args"] = YAML::Load("[]");
            for(const AST::Node::Ptr &n : *node->get_args()) {
                content["args"].push_back(render(n));
            }
        }

        if(node->get_scope()) {
            content["scope"] = YAML::Load("[]");
            for(const AST::ScopeName::Ptr &n : *node->get_scope()) {
                content["scope"].push_back(render(n));
            }
        }

        content["hier"] = render(node->get_hier());
    }

    node_functioncall["FunctionCall"] = content;
    return node_functioncall;
}

YAML::Node YAMLGenerator::render_task(const AST::Task::Ptr node) const
{
    YAML::Node node_task;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Task) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["name"] = node->get_name();
        switch(node->get_lifetime()) {
        case Veriparse::AST::Task::LifetimeEnum::NONE:
            content["lifetime"] = "NONE";
            break;
        case Veriparse::AST::Task::LifetimeEnum::AUTOMATIC:
            content["lifetime"] = "AUTOMATIC";
            break;
        default:
            content["lifetime"] = "STATIC";
        }

        if(node->get_args()) {
            content["args"] = YAML::Load("[]");
            for(const AST::Arg::Ptr &n : *node->get_args()) {
                content["args"].push_back(render(n));
            }
        }

        if(node->get_statements()) {
            content["statements"] = YAML::Load("[]");
            for(const AST::Node::Ptr &n : *node->get_statements()) {
                content["statements"].push_back(render(n));
            }
        }
    }

    node_task["Task"] = content;
    return node_task;
}

YAML::Node YAMLGenerator::render_taskcall(const AST::TaskCall::Ptr node) const
{
    YAML::Node node_taskcall;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::TaskCall) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["name"] = node->get_name();

        if(node->get_args()) {
            content["args"] = YAML::Load("[]");
            for(const AST::Node::Ptr &n : *node->get_args()) {
                content["args"].push_back(render(n));
            }
        }

        if(node->get_scope()) {
            content["scope"] = YAML::Load("[]");
            for(const AST::ScopeName::Ptr &n : *node->get_scope()) {
                content["scope"].push_back(render(n));
            }
        }

        content["hier"] = render(node->get_hier());
    }

    node_taskcall["TaskCall"] = content;
    return node_taskcall;
}

YAML::Node YAMLGenerator::render_generatestatement(const AST::GenerateStatement::Ptr node) const
{
    YAML::Node node_generatestatement;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::GenerateStatement) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        if(node->get_items()) {
            content["items"] = YAML::Load("[]");
            for(const AST::Node::Ptr &n : *node->get_items()) {
                content["items"].push_back(render(n));
            }
        }
    }

    node_generatestatement["GenerateStatement"] = content;
    return node_generatestatement;
}

YAML::Node YAMLGenerator::render_systemcall(const AST::SystemCall::Ptr node) const
{
    YAML::Node node_systemcall;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::SystemCall) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["syscall"] = node->get_syscall();

        if(node->get_args()) {
            content["args"] = YAML::Load("[]");
            for(const AST::Node::Ptr &n : *node->get_args()) {
                content["args"].push_back(render(n));
            }
        }
    }

    node_systemcall["SystemCall"] = content;
    return node_systemcall;
}

YAML::Node YAMLGenerator::render_hierlabel(const AST::HierLabel::Ptr node) const
{
    YAML::Node node_hierlabel;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::HierLabel) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["name"] = node->get_name();

        content["loop"] = render(node->get_loop());
    }

    node_hierlabel["HierLabel"] = content;
    return node_hierlabel;
}

YAML::Node YAMLGenerator::render_hiername(const AST::HierName::Ptr node) const
{
    YAML::Node node_hiername;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::HierName) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        if(node->get_labellist()) {
            content["labellist"] = YAML::Load("[]");
            for(const AST::HierLabel::Ptr &n : *node->get_labellist()) {
                content["labellist"].push_back(render(n));
            }
        }
    }

    node_hiername["HierName"] = content;
    return node_hiername;
}

YAML::Node YAMLGenerator::render_disable(const AST::Disable::Ptr node) const
{
    YAML::Node node_disable;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Disable) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["dest"] = render(node->get_dest());
    }

    node_disable["Disable"] = content;
    return node_disable;
}

YAML::Node YAMLGenerator::render_return(const AST::Return::Ptr node) const
{
    YAML::Node node_return;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Return) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();

        content["value"] = render(node->get_value());
    }

    node_return["Return"] = content;
    return node_return;
}

YAML::Node YAMLGenerator::render_break(const AST::Break::Ptr node) const
{
    YAML::Node node_break;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Break) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
    }

    node_break["Break"] = content;
    return node_break;
}

YAML::Node YAMLGenerator::render_continue(const AST::Continue::Ptr node) const
{
    YAML::Node node_continue;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Continue) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
    }

    node_continue["Continue"] = content;
    return node_continue;
}

YAML::Node YAMLGenerator::render_parallelblock(const AST::ParallelBlock::Ptr node) const
{
    YAML::Node node_parallelblock;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::ParallelBlock) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["scope"] = node->get_scope();

        if(node->get_statements()) {
            content["statements"] = YAML::Load("[]");
            for(const AST::Node::Ptr &n : *node->get_statements()) {
                content["statements"].push_back(render(n));
            }
        }
    }

    node_parallelblock["ParallelBlock"] = content;
    return node_parallelblock;
}

YAML::Node YAMLGenerator::render_singlestatement(const AST::SingleStatement::Ptr node) const
{
    YAML::Node node_singlestatement;
    YAML::Node content;

    if(node) {
        if(node->get_node_type() != AST::NodeType::SingleStatement) {
            return render(AST::cast_to<AST::Node>(node));
        }

        content["filename"] = node->get_filename();
        content["line"] = node->get_line();
        content["scope"] = node->get_scope();

        content["statement"] = render(node->get_statement());

        content["delay"] = render(node->get_delay());
    }

    node_singlestatement["SingleStatement"] = content;
    return node_singlestatement;
}

} // namespace Generators
} // namespace Veriparse