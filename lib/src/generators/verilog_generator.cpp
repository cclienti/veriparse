// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/generators/verilog_generator.hpp>
#include <veriparse/passes/transformations/expression_operators.hpp>
#include <veriparse/misc/string_utils.hpp>
#include <veriparse/misc/math.hpp>
#include <veriparse/logger/logger.hpp>

#include <iostream>
#include <algorithm>
#include <cctype>
#include <vector>
#include <sstream>
#include <limits>
#include <iomanip>
#include <memory>

using namespace Veriparse::Misc;

namespace Veriparse
{
namespace Generators
{

typedef std::numeric_limits<double> dbl;

namespace
{
// Force the dynamic-dispatch render() overload (the one switching on node_type)
// when holding a base-class pointer (DataType, Dimension, Declaration, ...): the
// generated per-type render(const X::Ptr) overloads bind statically and would
// otherwise route a LogicType-stored-as-DataType to render_datatype (empty).
AST::Node::Ptr as_node(const AST::Node::Ptr &n) { return std::static_pointer_cast<AST::Node>(n); }
} // namespace

std::string VerilogGenerator::render_node(const AST::Node::Ptr node) const
{
    if(node->get_node_type() != AST::NodeType::Node) {
        return render(AST::to_node(node));
    }

    return std::string();
}

std::string VerilogGenerator::render_source(const AST::Source::Ptr node) const
{
    if(node) {
        return render(node->get_description());
    } else {
        return std::string();
    }
}

std::string VerilogGenerator::render_description(const AST::Description::Ptr node) const
{
    std::string result;
    if(node) {
        const AST::Node::ListPtr definitions = node->get_definitions();
        if(definitions) {
            for(const AST::Node::Ptr &def : *definitions) {
                result.append(render(def) + "\n");
            }
        }
    }
    return result;
}

std::string VerilogGenerator::render_pragmalist(const AST::Pragmalist::Ptr node) const
{
    std::string result;
    if(node) {
        const auto &stmts = node->get_statements();
        if(stmts && !stmts->empty()) {
            result += "(* ";
            const auto &pragmas = *node->get_pragmas();
            for(auto it = pragmas.begin(); it != pragmas.end(); ++it) {
                result += render(*it);
                if(std::next(it) != pragmas.end()) {
                    result += ", ";
                }
            }
            result += " *)\n";
            for(const auto &stmt : *stmts) {
                result += render(stmt);
            }
        }
    }
    return result;
}

std::string VerilogGenerator::render_pragma(const AST::Pragma::Ptr node) const
{
    std::string result;
    if(node) {
        const AST::Node::Ptr expression = node->get_expression();
        if(expression) {
            result = node->get_name() + " = " + render(expression);
        } else {
            result = node->get_name();
        }
    }
    return result;
}

std::string VerilogGenerator::render_module(const AST::Module::Ptr node) const
{
    std::string result;
    if(node) {
        std::string modname = node->get_name();
        const AST::Declaration::ListPtr params = node->get_params();
        const AST::Port::ListPtr ports = node->get_ports();
        const AST::Node::ListPtr items = node->get_items();

        result = "module " + StringUtils::escape(modname);

        int length = result.size();
        const std::string params_str = parameters_list_to_string(params, length + 1);
        const std::string ports_str = ports_list_to_string(ports, length + 1);

        if(params_str.size() != 0) {
            result += " ";
            result.append(params_str);
            if(ports_str.size() != 0) {
                result += "\n" + std::string(length, ' ');
            }
        }

        if(ports_str.size() != 0) {
            result += " ";
            result.append(ports_str);
        }

        result += ";\n\n";

        if(items) {
            for(const AST::Node::Ptr &item : *items) {
                if(item) {
                    result.append(indent(render(item)) + "\n");
                }
            }
        }

        result += "\nendmodule\n";
    }
    return result;
}

std::string VerilogGenerator::render_package(const AST::Package::Ptr node) const
{
    std::string result;
    if(node) {
        result = "package ";
        switch(node->get_lifetime()) {
        case AST::Package::LifetimeEnum::AUTOMATIC:
            result += "automatic ";
            break;
        case AST::Package::LifetimeEnum::STATIC:
            result += "static ";
            break;
        default:
            break;
        }
        result += StringUtils::escape(node->get_name()) + ";\n\n";

        const AST::Node::ListPtr items = node->get_items();
        if(items) {
            for(const AST::Node::Ptr &item : *items) {
                if(item) {
                    result.append(indent(render(item)) + "\n");
                }
            }
        }

        result += "\nendpackage\n";
    }
    return result;
}

std::string VerilogGenerator::render_import(const AST::Import::Ptr node) const
{
    std::string result;
    if(node) {
        const std::string symbol = node->get_symbol();
        result = "import " + StringUtils::escape(node->get_package()) + "::";
        // "*" is the wildcard token, every other symbol is an identifier.
        result += (symbol == "*") ? symbol : StringUtils::escape(symbol);
        result += ";";
    }
    return result;
}

std::string VerilogGenerator::render_port(const AST::Port::Ptr node) const
{
    std::string result;
    if(!node) {
        return result;
    }

    // Direction keyword (NONE = no explicit direction, e.g. a non-ANSI header
    // reference or a declaration whose direction lives in a separate statement).
    std::string dir;
    switch(node->get_direction()) {
    case AST::Port::DirectionEnum::INPUT:
        dir = "input";
        break;
    case AST::Port::DirectionEnum::OUTPUT:
        dir = "output";
        break;
    case AST::Port::DirectionEnum::INOUT:
        dir = "inout";
        break;
    case AST::Port::DirectionEnum::REF:
        dir = "ref";
        break;
    case AST::Port::DirectionEnum::CONST_REF:
        dir = "const ref";
        break;
    case AST::Port::DirectionEnum::NONE:
        break;
    }

    // Explicit ANSI connection `.p(expr)`.
    if(node->get_expr()) {
        result = "." + StringUtils::escape(node->get_name()) + "(" + render(node->get_expr()) + ")";
        return result;
    }

    // A directed inner declaration (Var/Net). Emit it as a full declaration
    // statement (trailing ';'); in a header port list the ';' is stripped by
    // ports_list_to_string, while a non-ANSI body declaration keeps it.
    if(node->get_decl()) {
        std::string decl = StringUtils::remove_last_semicolon(render(as_node(node->get_decl())));
        result = dir.empty() ? decl : dir + " " + decl;
        return result + ";";
    }

    // Non-ANSI header reference / body direction declaration: the name (optionally
    // a direction). The trailing ';' makes the body form a valid statement and is
    // stripped in the header port list.
    result = StringUtils::escape(node->get_name());
    if(!dir.empty()) {
        result = dir + " " + result;
    }
    return result + ";";
}

std::string VerilogGenerator::render_identifier(const AST::Identifier::Ptr node) const
{
    if(!node) {
        return std::string();
    }
    return identifier_to_string(node);
}

std::string VerilogGenerator::render_scopename(const AST::ScopeName::Ptr node) const
{
    std::string result;
    if(node) {
        result = StringUtils::escape(node->get_name());
        const AST::ParamArg::ListPtr params = node->get_params();
        if(params && !params->empty()) {
            result += "#(";
            auto func = [&](const AST::ParamArg::Ptr n) { return render(n) + ", "; };
            auto func_last = [&](const AST::ParamArg::Ptr n) { return render(n); };
            result += StringUtils::join<AST::ParamArg::List, AST::ParamArg::Ptr>(*params, func,
                                                                                 func_last);
            result += ")";
        }
        result += "::";
    }
    return result;
}

std::string VerilogGenerator::render_hierlabel(const AST::HierLabel::Ptr node) const
{
    std::string result;
    if(node) {
        result = StringUtils::escape(node->get_name());
        const std::string &loop = render(node->get_loop());
        if(loop.size() > 0) {
            result += "[" + loop + "]";
        }
        result += ".";
    }
    return result;
}

std::string VerilogGenerator::render_hiername(const AST::HierName::Ptr node) const
{
    std::string result;
    if(node) {
        const AST::HierLabel::ListPtr labellist = node->get_labellist();
        if(labellist) {
            auto func = [&](const AST::HierLabel::Ptr n) { return render(n); };
            result += StringUtils::join<AST::HierLabel::List, AST::HierLabel::Ptr>(*labellist, func,
                                                                                   func);
        }
    }
    return result;
}

std::string VerilogGenerator::render_intconst(const AST::IntConst::Ptr node) const
{
    std::string result;
    if(node) {
        result = node->get_value();
    }
    return result;
}

std::string VerilogGenerator::render_intconstn(const AST::IntConstN::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        int size = node->get_size();
        bool sign = node->get_sign();
        int base = node->get_base();
        mpz_class value = node->get_value();

        if(size == 0) {
            LOG_ERROR << "in file \"" << node->get_filename() << "\", line " << node->get_line()
                      << ":"
                      << "integer size if null";
        }

        if(size < 0) {
            ss << value.str();
        } else {
            char base_str = (base == 10) ? 'd' : (base == 16) ? 'h' : (base == 8) ? 'o' : 'b';

            if(value < 0) {
                Passes::Transformations::unsigned_fct<AST::IntConstN::Ptr> op;
                AST::IntConstN::Ptr n = op(node);
                value = n->get_value();
            }

            ss << size << "'";

            if(sign) {
                ss << 's';
            }

            ss << base_str << Veriparse::Misc::bignum_to_str(value, base);
        }
    }

    return ss.str();
}

std::string VerilogGenerator::render_floatconst(const AST::FloatConst::Ptr node) const
{
    std::string result;
    if(node) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(dbl::max_digits10) << node->get_value();
        result = ss.str();
    }
    return result;
}

std::string VerilogGenerator::render_stringconst(const AST::StringConst::Ptr node) const
{
    std::string result;
    if(node) {
        result = "\"" + node->get_value() + "\"";
    }
    return result;
}

//
// DataType hierarchy
//

std::string VerilogGenerator::data_type_to_string(const AST::DataType::Ptr type) const
{
    if(!type) {
        return "";
    }

    // Aggregates render their own body (+ packed dims) via dispatch.
    switch(type->get_node_type()) {
    case AST::NodeType::StructType:
    case AST::NodeType::UnionType:
    case AST::NodeType::EnumType:
        return render(as_node(type));
    default:
        break;
    }

    std::string result;
    bool takes_signing = false;

    switch(type->get_node_type()) {
    case AST::NodeType::LogicType:
        result = "logic";
        takes_signing = true;
        break;
    case AST::NodeType::RegType:
        result = "reg";
        takes_signing = true;
        break;
    case AST::NodeType::BitType:
        result = "bit";
        takes_signing = true;
        break;
    case AST::NodeType::ByteType:
        result = "byte";
        takes_signing = true;
        break;
    case AST::NodeType::ShortintType:
        result = "shortint";
        takes_signing = true;
        break;
    case AST::NodeType::IntType:
        result = "int";
        takes_signing = true;
        break;
    case AST::NodeType::LongintType:
        result = "longint";
        takes_signing = true;
        break;
    case AST::NodeType::IntegerType:
        result = "integer";
        takes_signing = true;
        break;
    case AST::NodeType::TimeType:
        result = "time";
        break;
    case AST::NodeType::RealType:
        result = "real";
        break;
    case AST::NodeType::ShortrealType:
        result = "shortreal";
        break;
    case AST::NodeType::RealtimeType:
        result = "realtime";
        break;
    case AST::NodeType::StringType:
        result = "string";
        break;
    case AST::NodeType::ChandleType:
        result = "chandle";
        break;
    case AST::NodeType::EventType:
        result = "event";
        break;
    case AST::NodeType::VoidType:
        result = "void";
        break;
    case AST::NodeType::ImplicitType:
        result = "";
        takes_signing = true;
        break;
    case AST::NodeType::NamedType: {
        const auto t = AST::cast_to<AST::NamedType>(type);
        const AST::ScopeName::ListPtr scope = t->get_scope();
        if(scope) {
            for(const AST::ScopeName::Ptr &s : *scope) {
                result += render(s);
            }
        }
        result += StringUtils::escape(t->get_name());
        break;
    }
    case AST::NodeType::TypeOpExpr:
        result = "type(" + render(AST::cast_to<AST::TypeOpExpr>(type)->get_expr()) + ")";
        break;
    case AST::NodeType::TypeOpType:
        result =
            "type(" + data_type_to_string(AST::cast_to<AST::TypeOpType>(type)->get_type()) + ")";
        break;
    default:
        break;
    }

    if(takes_signing) {
        switch(type->get_signing()) {
        case AST::DataType::SigningEnum::SIGNED:
            result += result.empty() ? "signed" : " signed";
            break;
        case AST::DataType::SigningEnum::UNSIGNED:
            result += result.empty() ? "unsigned" : " unsigned";
            break;
        case AST::DataType::SigningEnum::NONE:
            break;
        }
    }

    const std::string dims = dims_to_string(type->get_packed_dims());
    if(!dims.empty()) {
        result += result.empty() ? dims : " " + dims;
    }
    return result;
}

std::string VerilogGenerator::render_logictype(const AST::LogicType::Ptr node) const
{
    return data_type_to_string(node);
}
std::string VerilogGenerator::render_regtype(const AST::RegType::Ptr node) const
{
    return data_type_to_string(node);
}
std::string VerilogGenerator::render_bittype(const AST::BitType::Ptr node) const
{
    return data_type_to_string(node);
}
std::string VerilogGenerator::render_bytetype(const AST::ByteType::Ptr node) const
{
    return data_type_to_string(node);
}
std::string VerilogGenerator::render_shortinttype(const AST::ShortintType::Ptr node) const
{
    return data_type_to_string(node);
}
std::string VerilogGenerator::render_inttype(const AST::IntType::Ptr node) const
{
    return data_type_to_string(node);
}
std::string VerilogGenerator::render_longinttype(const AST::LongintType::Ptr node) const
{
    return data_type_to_string(node);
}
std::string VerilogGenerator::render_integertype(const AST::IntegerType::Ptr node) const
{
    return data_type_to_string(node);
}
std::string VerilogGenerator::render_timetype(const AST::TimeType::Ptr node) const
{
    return data_type_to_string(node);
}
std::string VerilogGenerator::render_realtype(const AST::RealType::Ptr node) const
{
    return data_type_to_string(node);
}
std::string VerilogGenerator::render_shortrealtype(const AST::ShortrealType::Ptr node) const
{
    return data_type_to_string(node);
}
std::string VerilogGenerator::render_realtimetype(const AST::RealtimeType::Ptr node) const
{
    return data_type_to_string(node);
}
std::string VerilogGenerator::render_stringtype(const AST::StringType::Ptr node) const
{
    return data_type_to_string(node);
}
std::string VerilogGenerator::render_chandletype(const AST::ChandleType::Ptr node) const
{
    return data_type_to_string(node);
}
std::string VerilogGenerator::render_eventtype(const AST::EventType::Ptr node) const
{
    return data_type_to_string(node);
}
std::string VerilogGenerator::render_voidtype(const AST::VoidType::Ptr node) const
{
    return data_type_to_string(node);
}
std::string VerilogGenerator::render_implicittype(const AST::ImplicitType::Ptr node) const
{
    return data_type_to_string(node);
}
std::string VerilogGenerator::render_namedtype(const AST::NamedType::Ptr node) const
{
    return data_type_to_string(node);
}
std::string VerilogGenerator::render_typeopexpr(const AST::TypeOpExpr::Ptr node) const
{
    return data_type_to_string(node);
}
std::string VerilogGenerator::render_typeoptype(const AST::TypeOpType::Ptr node) const
{
    return data_type_to_string(node);
}

//
// Dimension hierarchy
//

std::string VerilogGenerator::dims_to_string(const AST::Dimension::ListPtr dims) const
{
    std::string result;
    if(dims) {
        for(const AST::Dimension::Ptr &d : *dims) {
            result += render(as_node(d));
        }
    }
    return result;
}

std::string VerilogGenerator::render_rangedim(const AST::RangeDim::Ptr node) const
{
    std::string result;
    if(node) {
        std::string l = StringUtils::remove_whitespace(
            StringUtils::delete_surrounding_brackets(render(node->get_left())));
        std::string r = StringUtils::remove_whitespace(
            StringUtils::delete_surrounding_brackets(render(node->get_right())));
        result = "[" + l + ":" + r + "]";
    }
    return result;
}

std::string VerilogGenerator::render_sizedim(const AST::SizeDim::Ptr node) const
{
    std::string result;
    if(node) {
        result = "[" +
                 StringUtils::remove_whitespace(
                     StringUtils::delete_surrounding_brackets(render(node->get_size()))) +
                 "]";
    }
    return result;
}

std::string VerilogGenerator::render_unsizeddim(const AST::UnsizedDim::Ptr node) const
{
    return node ? "[]" : "";
}

std::string VerilogGenerator::render_queuedim(const AST::QueueDim::Ptr node) const
{
    std::string result;
    if(node) {
        if(node->get_bound()) {
            result = "[$:" +
                     StringUtils::remove_whitespace(
                         StringUtils::delete_surrounding_brackets(render(node->get_bound()))) +
                     "]";
        } else {
            result = "[$]";
        }
    }
    return result;
}

std::string VerilogGenerator::render_assocdim(const AST::AssocDim::Ptr node) const
{
    std::string result;
    if(node) {
        if(node->get_index_type()) {
            result = "[" + data_type_to_string(node->get_index_type()) + "]";
        } else {
            result = "[*]";
        }
    }
    return result;
}

//
// Declaration hierarchy
//

std::string VerilogGenerator::decl_tail_to_string(const std::string &name,
                                                  const AST::Dimension::ListPtr unpacked_dims,
                                                  const AST::Node::Ptr init) const
{
    std::string result = StringUtils::escape(name);
    const std::string dims = dims_to_string(unpacked_dims);
    if(!dims.empty()) {
        result += " " + dims;
    }
    if(init) {
        result += " = " + render(init);
    }
    return result;
}

std::string VerilogGenerator::var_qualifier_prefix(const AST::Var::Ptr node) const
{
    std::string prefix;
    if(node->get_is_const()) {
        prefix += "const ";
    }
    if(node->get_is_var()) {
        prefix += "var ";
    }
    switch(node->get_lifetime()) {
    case AST::Var::LifetimeEnum::AUTOMATIC:
        prefix += "automatic ";
        break;
    case AST::Var::LifetimeEnum::STATIC:
        prefix += "static ";
        break;
    case AST::Var::LifetimeEnum::NONE:
        break;
    }
    return prefix;
}

std::string VerilogGenerator::render_var(const AST::Var::Ptr node) const
{
    std::string result;
    if(node) {
        const std::string prefix = var_qualifier_prefix(node);
        const std::string type_str = data_type_to_string(node->get_type());
        const std::string tail =
            decl_tail_to_string(node->get_name(), node->get_unpacked_dims(), node->get_init());
        result = prefix + (type_str.empty() ? tail : type_str + " " + tail) + ";";
    }
    return result;
}

std::string VerilogGenerator::render_member(const AST::Member::Ptr node) const
{
    std::string result;
    if(node) {
        const std::string type_str = data_type_to_string(node->get_type());
        const std::string tail =
            decl_tail_to_string(node->get_name(), node->get_unpacked_dims(), node->get_init());
        result = (type_str.empty() ? tail : type_str + " " + tail) + ";";
    }
    return result;
}

std::string VerilogGenerator::net_to_string(const char *keyword, const AST::Net::Ptr node) const
{
    std::string result = keyword;

    // [drive_strength | charge_strength]
    if(node->get_strength()) {
        result += " " + render(as_node(node->get_strength()));
    }
    // [vectored | scalared]
    if(node->get_is_vectored()) {
        result += " vectored";
    }
    if(node->get_is_scalared()) {
        result += " scalared";
    }
    // data_type_or_implicit
    const std::string type_str = data_type_to_string(node->get_type());
    if(!type_str.empty()) {
        result += " " + type_str;
    }
    // [delay3]
    if(node->get_ldelay()) {
        result += " " + render(node->get_ldelay());
    }
    // name { unpacked_dimension } [ = continuous_assign ]
    result += " " + decl_tail_to_string(node->get_name(), node->get_unpacked_dims(),
                                        node->get_cont_assign());
    result += ";";
    return result;
}

std::string VerilogGenerator::render_wirenet(const AST::WireNet::Ptr node) const
{
    return node ? net_to_string("wire", node) : "";
}
std::string VerilogGenerator::render_trinet(const AST::TriNet::Ptr node) const
{
    return node ? net_to_string("tri", node) : "";
}
std::string VerilogGenerator::render_tri0net(const AST::Tri0Net::Ptr node) const
{
    return node ? net_to_string("tri0", node) : "";
}
std::string VerilogGenerator::render_tri1net(const AST::Tri1Net::Ptr node) const
{
    return node ? net_to_string("tri1", node) : "";
}
std::string VerilogGenerator::render_triandnet(const AST::TriandNet::Ptr node) const
{
    return node ? net_to_string("triand", node) : "";
}
std::string VerilogGenerator::render_triornet(const AST::TriorNet::Ptr node) const
{
    return node ? net_to_string("trior", node) : "";
}
std::string VerilogGenerator::render_triregnet(const AST::TriregNet::Ptr node) const
{
    return node ? net_to_string("trireg", node) : "";
}
std::string VerilogGenerator::render_wandnet(const AST::WandNet::Ptr node) const
{
    return node ? net_to_string("wand", node) : "";
}
std::string VerilogGenerator::render_wornet(const AST::WorNet::Ptr node) const
{
    return node ? net_to_string("wor", node) : "";
}
std::string VerilogGenerator::render_uwirenet(const AST::UwireNet::Ptr node) const
{
    return node ? net_to_string("uwire", node) : "";
}
std::string VerilogGenerator::render_supply0net(const AST::Supply0Net::Ptr node) const
{
    return node ? net_to_string("supply0", node) : "";
}
std::string VerilogGenerator::render_supply1net(const AST::Supply1Net::Ptr node) const
{
    return node ? net_to_string("supply1", node) : "";
}
std::string VerilogGenerator::render_interconnectnet(const AST::InterconnectNet::Ptr node) const
{
    return node ? net_to_string("interconnect", node) : "";
}
std::string VerilogGenerator::render_usernet(const AST::UserNet::Ptr node) const
{
    // The nettype is the (named) data type carried by the net; no extra keyword.
    if(!node) {
        return "";
    }
    const std::string type_str = data_type_to_string(node->get_type());
    std::string result =
        type_str + " " +
        decl_tail_to_string(node->get_name(), node->get_unpacked_dims(), node->get_cont_assign());
    result += ";";
    return result;
}
std::string VerilogGenerator::render_implicitnet(const AST::ImplicitNet::Ptr node) const
{
    // No net keyword written (a port default): render only the data type (if any),
    // name and dims.
    if(!node) {
        return "";
    }
    const std::string type_str = data_type_to_string(node->get_type());
    const std::string tail =
        decl_tail_to_string(node->get_name(), node->get_unpacked_dims(), node->get_cont_assign());
    std::string result = (type_str.empty() ? tail : type_str + " " + tail) + ";";
    return result;
}

namespace
{
const char *drive_strength0_str(AST::DriveStrength::S0Enum s)
{
    switch(s) {
    case AST::DriveStrength::S0Enum::SUPPLY0:
        return "supply0";
    case AST::DriveStrength::S0Enum::STRONG0:
        return "strong0";
    case AST::DriveStrength::S0Enum::PULL0:
        return "pull0";
    case AST::DriveStrength::S0Enum::WEAK0:
        return "weak0";
    case AST::DriveStrength::S0Enum::HIGHZ0:
        return "highz0";
    }
    return "";
}
const char *drive_strength1_str(AST::DriveStrength::S1Enum s)
{
    switch(s) {
    case AST::DriveStrength::S1Enum::SUPPLY1:
        return "supply1";
    case AST::DriveStrength::S1Enum::STRONG1:
        return "strong1";
    case AST::DriveStrength::S1Enum::PULL1:
        return "pull1";
    case AST::DriveStrength::S1Enum::WEAK1:
        return "weak1";
    case AST::DriveStrength::S1Enum::HIGHZ1:
        return "highz1";
    }
    return "";
}
} // namespace

std::string VerilogGenerator::render_drivestrength(const AST::DriveStrength::Ptr node) const
{
    if(!node) {
        return "";
    }
    return std::string("(") + drive_strength0_str(node->get_s0()) + ", " +
           drive_strength1_str(node->get_s1()) + ")";
}

std::string VerilogGenerator::render_chargestrength(const AST::ChargeStrength::Ptr node) const
{
    if(!node) {
        return "";
    }
    const char *c = "";
    switch(node->get_charge()) {
    case AST::ChargeStrength::ChargeEnum::SMALL:
        c = "small";
        break;
    case AST::ChargeStrength::ChargeEnum::MEDIUM:
        c = "medium";
        break;
    case AST::ChargeStrength::ChargeEnum::LARGE:
        c = "large";
        break;
    }
    return std::string("(") + c + ")";
}

std::string VerilogGenerator::render_param(const AST::Param::Ptr node) const
{
    std::string result;
    if(node) {
        result = node->get_is_local() ? "localparam " : "parameter ";
        const std::string type_str = data_type_to_string(node->get_type());
        if(!type_str.empty()) {
            result += type_str + " ";
        }
        result += StringUtils::escape(node->get_name());
        const std::string dims = dims_to_string(node->get_unpacked_dims());
        if(!dims.empty()) {
            result += " " + dims;
        }
        if(node->get_value()) {
            result += " = " + render(node->get_value());
        }
        result += ";";
    }
    return result;
}

std::string VerilogGenerator::render_typeparam(const AST::TypeParam::Ptr node) const
{
    std::string result;
    if(node) {
        result = node->get_is_local() ? "localparam type " : "parameter type ";
        result += StringUtils::escape(node->get_name());
        if(node->get_type()) {
            result += " = " + data_type_to_string(node->get_type());
        }
        result += ";";
    }
    return result;
}

std::string VerilogGenerator::render_typedef(const AST::Typedef::Ptr node) const
{
    std::string result;
    if(!node) {
        return result;
    }
    // A null type marks a forward typedef (`typedef [kind] name;`).
    if(!node->get_type()) {
        result = "typedef ";
        switch(node->get_fwd_kind()) {
        case AST::Typedef::Fwd_kindEnum::ENUM:
            result += "enum ";
            break;
        case AST::Typedef::Fwd_kindEnum::STRUCT:
            result += "struct ";
            break;
        case AST::Typedef::Fwd_kindEnum::UNION:
            result += "union ";
            break;
        case AST::Typedef::Fwd_kindEnum::CLASS:
            result += "class ";
            break;
        case AST::Typedef::Fwd_kindEnum::INTERFACE_CLASS:
            result += "interface class ";
            break;
        case AST::Typedef::Fwd_kindEnum::NONE:
            break;
        }
        result += StringUtils::escape(node->get_name()) + ";";
        return result;
    }

    result = "typedef " + data_type_to_string(node->get_type()) + " " +
             StringUtils::escape(node->get_name());
    const std::string dims = dims_to_string(node->get_unpacked_dims());
    if(!dims.empty()) {
        result += " " + dims;
    }
    result += ";";
    return result;
}

std::string VerilogGenerator::render_arg(const AST::Arg::Ptr node) const
{
    std::string result;
    if(!node) {
        return result;
    }
    switch(node->get_direction()) {
    case AST::Arg::DirectionEnum::INPUT:
        result = "input ";
        break;
    case AST::Arg::DirectionEnum::OUTPUT:
        result = "output ";
        break;
    case AST::Arg::DirectionEnum::INOUT:
        result = "inout ";
        break;
    case AST::Arg::DirectionEnum::REF:
        result = "ref ";
        break;
    case AST::Arg::DirectionEnum::CONST_REF:
        result = "const ref ";
        break;
    case AST::Arg::DirectionEnum::NONE:
        break;
    }
    if(node->get_is_var()) {
        result += "var ";
    }
    const std::string type_str = data_type_to_string(node->get_type());
    const std::string tail =
        decl_tail_to_string(node->get_name(), node->get_unpacked_dims(), node->get_default_value());
    result += type_str.empty() ? tail : type_str + " " + tail;
    return result;
}

std::string VerilogGenerator::render_genvar(const AST::Genvar::Ptr node) const
{
    std::string result;
    if(node) {
        result = "genvar " + StringUtils::escape(node->get_name()) + ";";
    }
    return result;
}

std::string VerilogGenerator::render_nettypedecl(const AST::NetTypeDecl::Ptr node) const
{
    std::string result;
    if(node) {
        result = "nettype " + data_type_to_string(node->get_type()) + " " +
                 StringUtils::escape(node->get_name());
        if(node->get_resolver()) {
            result += " with " + identifier_to_string(node->get_resolver());
        }
        result += ";";
    }
    return result;
}

std::string VerilogGenerator::render_concat(const AST::Concat::Ptr node) const
{
    std::string result;
    if(node) {
        const AST::Node::ListPtr items = node->get_list();
        if(items) {
            result.append("{ ");
            auto func = [&](const AST::Node::Ptr n) { return render(n) + ", "; };
            auto func_last = [&](const AST::Node::Ptr n) { return render(n); };
            result += StringUtils::join<AST::Node::List, AST::Node::Ptr>(*items, func, func_last);
            result.append(" }");
        }
    }
    return result;
}

std::string VerilogGenerator::render_lconcat(const AST::Lconcat::Ptr node) const
{
    std::string result;
    if(node) {
        const AST::Node::ListPtr items = node->get_list();
        if(items) {
            result.append("{ ");
            auto func = [&](const AST::Node::Ptr n) { return render(n) + ", "; };
            auto func_last = [&](const AST::Node::Ptr n) { return render(n); };
            result += StringUtils::join<AST::Node::List, AST::Node::Ptr>(*items, func, func_last);
            result.append(" }");
        }
    }
    return result;
}

std::string VerilogGenerator::render_repeat(const AST::Repeat::Ptr node) const
{
    std::string result;
    if(node) {
        const AST::Node::Ptr times = node->get_times();
        const AST::Node::Ptr value = node->get_value();
        result = "{ " + StringUtils::delete_surrounding_brackets(render(times)) +
                 StringUtils::delete_surrounding_brackets(render(value)) + " }";
    }
    return result;
}

std::string VerilogGenerator::render_assignmentpattern(const AST::AssignmentPattern::Ptr node) const
{
    std::string result;
    if(node) {
        result = "'{";
        // replicated form `'{N{ ... }}`
        const AST::Node::Ptr times = node->get_times();
        if(times) {
            result += StringUtils::delete_surrounding_brackets(render(times)) + "{";
        }
        const AST::Node::ListPtr items = node->get_items();
        if(items) {
            bool first = true;
            for(const AST::Node::Ptr &item : *items) {
                if(!first) {
                    result += ", ";
                }
                result += render(item);
                first = false;
            }
        }
        if(times) {
            result += "}";
        }
        result += "}";
    }
    return result;
}

std::string VerilogGenerator::render_patternitem(const AST::PatternItem::Ptr node) const
{
    std::string result;
    if(node) {
        if(node->get_is_default()) {
            result = "default";
        } else if(node->get_key()) {
            result = render(node->get_key());
        }
        result += ": " + render(node->get_value());
    }
    return result;
}

//
// Cast hierarchy (node per form)
//

std::string VerilogGenerator::render_typecast(const AST::TypeCast::Ptr node) const
{
    std::string result;
    if(node) {
        result = data_type_to_string(node->get_target()) + "'(" +
                 StringUtils::delete_surrounding_brackets(render(node->get_expr())) + ")";
    }
    return result;
}

std::string VerilogGenerator::render_sizecast(const AST::SizeCast::Ptr node) const
{
    std::string result;
    if(node) {
        result = StringUtils::remove_whitespace(
                     StringUtils::delete_surrounding_brackets(render(node->get_size()))) +
                 "'(" + StringUtils::delete_surrounding_brackets(render(node->get_expr())) + ")";
    }
    return result;
}

std::string VerilogGenerator::render_signingcast(const AST::SigningCast::Ptr node) const
{
    std::string result;
    if(node) {
        const char *s =
            (node->get_signing() == AST::SigningCast::SigningEnum::SIGNED) ? "signed" : "unsigned";
        result = std::string(s) + "'(" +
                 StringUtils::delete_surrounding_brackets(render(node->get_expr())) + ")";
    }
    return result;
}

std::string VerilogGenerator::render_constcast(const AST::ConstCast::Ptr node) const
{
    std::string result;
    if(node) {
        result =
            "const'(" + StringUtils::delete_surrounding_brackets(render(node->get_expr())) + ")";
    }
    return result;
}

std::string VerilogGenerator::render_partselect(const AST::Partselect::Ptr node) const
{
    std::string result;
    if(node) {
        std::string var = render(node->get_var());
        std::string msb = StringUtils::remove_whitespace(
            StringUtils::delete_surrounding_brackets(render(node->get_msb())));
        std::string lsb = StringUtils::remove_whitespace(
            StringUtils::delete_surrounding_brackets(render(node->get_lsb())));
        result = var + "[" + msb + ":" + lsb + "]";
    }
    return result;
}

std::string
VerilogGenerator::render_partselectplusindexed(const AST::PartselectPlusIndexed::Ptr node) const
{
    std::string result;
    if(node) {
        std::string var = render(node->get_var());
        std::string index = StringUtils::remove_whitespace(
            StringUtils::delete_surrounding_brackets(render(node->get_index())));
        std::string size = StringUtils::remove_whitespace(
            StringUtils::delete_surrounding_brackets(render(node->get_size())));
        result = var + "[" + index + "+:" + size + "]";
    }
    return result;
}

std::string
VerilogGenerator::render_partselectminusindexed(const AST::PartselectMinusIndexed::Ptr node) const
{
    std::string result;
    if(node) {
        std::string var = render(node->get_var());
        std::string index = StringUtils::remove_whitespace(
            StringUtils::delete_surrounding_brackets(render(node->get_index())));
        std::string size = StringUtils::remove_whitespace(
            StringUtils::delete_surrounding_brackets(render(node->get_size())));
        result = var + "[" + index + "-:" + size + "]";
    }
    return result;
}

std::string VerilogGenerator::render_pointer(const AST::Pointer::Ptr node) const
{
    std::string result;
    if(node) {
        std::string var = render(node->get_var());
        std::string ptr = StringUtils::delete_surrounding_brackets(render(node->get_ptr()));
        result = var + "[" + ptr + "]";
    }
    return result;
}

std::string VerilogGenerator::render_lvalue(const AST::Lvalue::Ptr node) const
{
    std::string result;
    if(node) {
        result = StringUtils::delete_surrounding_brackets(render(node->get_var()));
    }
    return result;
}

std::string VerilogGenerator::render_rvalue(const AST::Rvalue::Ptr node) const
{
    std::string result;
    if(node) {
        result = StringUtils::delete_surrounding_brackets(render(node->get_var()));
    }
    return result;
}

std::string VerilogGenerator::render_uplus(const AST::Uplus::Ptr node) const
{
    if(node) {
        return unary_operators_to_string(node->get_node_type(), node->get_right());
    } else {
        return std::string();
    }
}

std::string VerilogGenerator::render_uminus(const AST::Uminus::Ptr node) const
{
    if(node) {
        return unary_operators_to_string(node->get_node_type(), node->get_right());
    } else {
        return std::string();
    }
}

std::string VerilogGenerator::render_ulnot(const AST::Ulnot::Ptr node) const
{
    if(node) {
        return unary_operators_to_string(node->get_node_type(), node->get_right());
    } else {
        return std::string();
    }
}

std::string VerilogGenerator::render_unot(const AST::Unot::Ptr node) const
{
    if(node) {
        return unary_operators_to_string(node->get_node_type(), node->get_right());
    } else {
        return std::string();
    }
}

std::string VerilogGenerator::render_uand(const AST::Uand::Ptr node) const
{
    if(node) {
        return unary_operators_to_string(node->get_node_type(), node->get_right());
    } else {
        return std::string();
    }
}

std::string VerilogGenerator::render_unand(const AST::Unand::Ptr node) const
{
    if(node) {
        return unary_operators_to_string(node->get_node_type(), node->get_right());
    } else {
        return std::string();
    }
}

std::string VerilogGenerator::render_uor(const AST::Uor::Ptr node) const
{
    if(node) {
        return unary_operators_to_string(node->get_node_type(), node->get_right());
    } else {
        return std::string();
    }
}

std::string VerilogGenerator::render_unor(const AST::Unor::Ptr node) const
{
    if(node) {
        return unary_operators_to_string(node->get_node_type(), node->get_right());
    } else {
        return std::string();
    }
}

std::string VerilogGenerator::render_uxor(const AST::Uxor::Ptr node) const
{
    if(node) {
        return unary_operators_to_string(node->get_node_type(), node->get_right());
    } else {
        return std::string();
    }
}

std::string VerilogGenerator::render_uxnor(const AST::Uxnor::Ptr node) const
{
    if(node) {
        return unary_operators_to_string(node->get_node_type(), node->get_right());
    } else {
        return std::string();
    }
}

std::string VerilogGenerator::render_power(const AST::Power::Ptr node) const
{
    if(node) {
        return operators_to_string(node->get_node_type(), node->get_left(), node->get_right());
    } else {
        return std::string();
    }
}

std::string VerilogGenerator::render_times(const AST::Times::Ptr node) const
{
    if(node) {
        return operators_to_string(node->get_node_type(), node->get_left(), node->get_right());
    } else {
        return std::string();
    }
}

std::string VerilogGenerator::render_divide(const AST::Divide::Ptr node) const
{
    if(node) {
        return operators_to_string(node->get_node_type(), node->get_left(), node->get_right());
    } else {
        return std::string();
    }
}

std::string VerilogGenerator::render_mod(const AST::Mod::Ptr node) const
{
    if(node) {
        return operators_to_string(node->get_node_type(), node->get_left(), node->get_right());
    } else {
        return std::string();
    }
}

std::string VerilogGenerator::render_plus(const AST::Plus::Ptr node) const
{
    if(node) {
        return operators_to_string(node->get_node_type(), node->get_left(), node->get_right());
    } else {
        return std::string();
    }
}

std::string VerilogGenerator::render_minus(const AST::Minus::Ptr node) const
{
    if(node) {
        return operators_to_string(node->get_node_type(), node->get_left(), node->get_right());
    } else {
        return std::string();
    }
}

std::string VerilogGenerator::render_sll(const AST::Sll::Ptr node) const
{
    if(node) {
        return operators_to_string(node->get_node_type(), node->get_left(), node->get_right());
    } else {
        return std::string();
    }
}

std::string VerilogGenerator::render_srl(const AST::Srl::Ptr node) const
{
    if(node) {
        return operators_to_string(node->get_node_type(), node->get_left(), node->get_right());
    } else {
        return std::string();
    }
}

std::string VerilogGenerator::render_sra(const AST::Sra::Ptr node) const
{
    if(node) {
        return operators_to_string(node->get_node_type(), node->get_left(), node->get_right());
    } else {
        return std::string();
    }
}

std::string VerilogGenerator::render_lessthan(const AST::LessThan::Ptr node) const
{
    if(node) {
        return operators_to_string(node->get_node_type(), node->get_left(), node->get_right());
    } else {
        return std::string();
    }
}

std::string VerilogGenerator::render_greaterthan(const AST::GreaterThan::Ptr node) const
{
    if(node) {
        return operators_to_string(node->get_node_type(), node->get_left(), node->get_right());
    } else {
        return std::string();
    }
}

std::string VerilogGenerator::render_lesseq(const AST::LessEq::Ptr node) const
{
    if(node) {
        return operators_to_string(node->get_node_type(), node->get_left(), node->get_right());
    } else {
        return std::string();
    }
}

std::string VerilogGenerator::render_greatereq(const AST::GreaterEq::Ptr node) const
{
    if(node) {
        return operators_to_string(node->get_node_type(), node->get_left(), node->get_right());
    } else {
        return std::string();
    }
}

std::string VerilogGenerator::render_eq(const AST::Eq::Ptr node) const
{
    if(node) {
        return operators_to_string(node->get_node_type(), node->get_left(), node->get_right());
    } else {
        return std::string();
    }
}

std::string VerilogGenerator::render_noteq(const AST::NotEq::Ptr node) const
{
    if(node) {
        return operators_to_string(node->get_node_type(), node->get_left(), node->get_right());
    } else {
        return std::string();
    }
}

std::string VerilogGenerator::render_eql(const AST::Eql::Ptr node) const
{
    if(node) {
        return operators_to_string(node->get_node_type(), node->get_left(), node->get_right());
    } else {
        return std::string();
    }
}

std::string VerilogGenerator::render_noteql(const AST::NotEql::Ptr node) const
{
    if(node) {
        return operators_to_string(node->get_node_type(), node->get_left(), node->get_right());
    } else {
        return std::string();
    }
}

std::string VerilogGenerator::render_and(const AST::And::Ptr node) const
{
    if(node) {
        return operators_to_string(node->get_node_type(), node->get_left(), node->get_right());
    } else {
        return std::string();
    }
}

std::string VerilogGenerator::render_xor(const AST::Xor::Ptr node) const
{
    if(node) {
        return operators_to_string(node->get_node_type(), node->get_left(), node->get_right());
    } else {
        return std::string();
    }
}

std::string VerilogGenerator::render_xnor(const AST::Xnor::Ptr node) const
{
    if(node) {
        return operators_to_string(node->get_node_type(), node->get_left(), node->get_right());
    } else {
        return std::string();
    }
}

std::string VerilogGenerator::render_or(const AST::Or::Ptr node) const
{
    if(node) {
        return operators_to_string(node->get_node_type(), node->get_left(), node->get_right());
    } else {
        return std::string();
    }
}

std::string VerilogGenerator::render_land(const AST::Land::Ptr node) const
{
    if(node) {
        return operators_to_string(node->get_node_type(), node->get_left(), node->get_right());
    } else {
        return std::string();
    }
}

std::string VerilogGenerator::render_lor(const AST::Lor::Ptr node) const
{
    if(node) {
        return operators_to_string(node->get_node_type(), node->get_left(), node->get_right());
    } else {
        return std::string();
    }
}

std::string VerilogGenerator::render_cond(const AST::Cond::Ptr node) const
{
    std::string result;
    if(node) {
        std::string cond = StringUtils::delete_surrounding_brackets(render(node->get_cond()));
        const AST::Node::Ptr tnode = node->get_left();
        const AST::Node::Ptr fnode = node->get_right();
        std::string tval = StringUtils::delete_surrounding_brackets(render(tnode));
        std::string fval = StringUtils::delete_surrounding_brackets(render(fnode));

        if(fnode) {
            if(fnode->get_node_type() == AST::NodeType::Cond) {
                fval.insert(0, 1, '\n');
            }
        }

        result = "((" + cond + ") ? " + tval + " : " + fval + ")";
    }
    return result;
}

std::string VerilogGenerator::render_always(const AST::Always::Ptr node) const
{
    std::string result = "\nalways";
    if(node) {
        const AST::Senslist::Ptr senslist = node->get_senslist();
        const AST::Node::Ptr statement = node->get_statement();
        if(senslist) {
            result += " @(" + render(senslist) + ")";
        }
        result.append(block_or_single_statement_to_string(statement));
    }
    return result;
}

std::string VerilogGenerator::render_alwaysff(const AST::AlwaysFF::Ptr node) const
{
    std::string result = "\nalways_ff";
    if(node) {
        const AST::Senslist::Ptr senslist = node->get_senslist();
        const AST::Node::Ptr statement = node->get_statement();
        if(senslist) {
            result += " @(" + render(senslist) + ")";
        }
        result.append(block_or_single_statement_to_string(statement));
    }
    return result;
}

std::string VerilogGenerator::render_alwayscomb(const AST::AlwaysComb::Ptr node) const
{
    std::string result = "\nalways_comb";
    if(node) {
        const AST::Node::Ptr statement = node->get_statement();
        result.append(block_or_single_statement_to_string(statement));
    }
    return result;
}

std::string VerilogGenerator::render_alwayslatch(const AST::AlwaysLatch::Ptr node) const
{
    std::string result = "\nalways_latch";
    if(node) {
        const AST::Node::Ptr statement = node->get_statement();
        result.append(block_or_single_statement_to_string(statement));
    }
    return result;
}

std::string VerilogGenerator::render_senslist(const AST::Senslist::Ptr node) const
{
    std::string result;
    if(node) {
        const AST::Sens::ListPtr items = node->get_list();
        if(items) {
            auto func = [&](const AST::Sens::Ptr n) { return render(n) + " or "; };
            auto func_last = [&](const AST::Sens::Ptr n) { return render(n); };
            result += StringUtils::join<AST::Sens::List, AST::Sens::Ptr>(*items, func, func_last);
        }
    }
    return result;
}

std::string VerilogGenerator::render_sens(const AST::Sens::Ptr node) const
{
    std::string result;
    if(node) {
        AST::Sens::TypeEnum type = node->get_type();
        const AST::Node::Ptr sig = node->get_sig();

        switch(type) {
        case AST::Sens::TypeEnum::ALL:
            result = "*";
            break;
        case AST::Sens::TypeEnum::POSEDGE:
            result = "posedge " + render(sig);
            break;
        case AST::Sens::TypeEnum::NEGEDGE:
            result = "negedge " + render(sig);
            break;
        case AST::Sens::TypeEnum::NONE:
            result = render(sig);
            break;
        default:
            break;
        }
    }
    return result;
}

std::string VerilogGenerator::render_defparamlist(const AST::Defparamlist::Ptr node) const
{
    std::string result;
    if(node) {
        const auto &list = node->get_list();
        if(list) {
            auto func = [&](const AST::Defparam::Ptr n) { return render(n) + ",\n"; };
            auto func_last = [&](const AST::Defparam::Ptr n) { return render(n) + ";"; };
            result += "defparam\n" +
                      indent(StringUtils::join<AST::Defparam::List, AST::Defparam::Ptr>(*list, func,
                                                                                        func_last));
        }
    }
    return result;
}

std::string VerilogGenerator::render_defparam(const AST::Defparam::Ptr node) const
{
    std::string result;
    if(node) {
        result = render(node->get_identifier()) + " = " + render(node->get_right());
    }
    return result;
}

std::string VerilogGenerator::render_assign(const AST::Assign::Ptr node) const
{
    std::string result;
    if(node) {
        const AST::Lvalue::Ptr lvalue = node->get_left();
        const AST::Rvalue::Ptr rvalue = node->get_right();
        result = "\nassign " + render(lvalue) + " = " + render(rvalue) + ";";
    }
    return result;
}

std::string
VerilogGenerator::render_blockingsubstitution(const AST::BlockingSubstitution::Ptr node) const
{
    std::string result;
    if(node) {
        const AST::Lvalue::Ptr lvalue = node->get_left();
        const AST::Rvalue::Ptr rvalue = node->get_right();
        const AST::DelayStatement::Ptr ldelay = node->get_ldelay();
        const AST::DelayStatement::Ptr rdelay = node->get_rdelay();

        std::string ldelay_str = render(ldelay);
        if(ldelay) {
            ldelay_str.append(" ");
        }

        std::string rdelay_str = render(rdelay);
        if(rdelay) {
            rdelay_str.append(" ");
        }

        result = ldelay_str + render(lvalue) + " = " + rdelay_str + render(rvalue) + ";";
    }
    return result;
}

std::string
VerilogGenerator::render_nonblockingsubstitution(const AST::NonblockingSubstitution::Ptr node) const
{
    std::string result;
    if(node) {
        const AST::Lvalue::Ptr lvalue = node->get_left();
        const AST::Rvalue::Ptr rvalue = node->get_right();
        const AST::DelayStatement::Ptr ldelay = node->get_ldelay();
        const AST::DelayStatement::Ptr rdelay = node->get_rdelay();

        std::string ldelay_str = render(ldelay);
        if(ldelay) {
            ldelay_str.append(" ");
        }

        std::string rdelay_str = render(rdelay);
        if(rdelay) {
            rdelay_str.append(" ");
        }

        result = ldelay_str + render(lvalue) + " <= " + rdelay_str + render(rvalue) + ";";
    }
    return result;
}

std::string VerilogGenerator::render_ifstatement(const AST::IfStatement::Ptr node) const
{
    std::string result;
    if(node) {
        const AST::Node::Ptr cond = node->get_cond();
        const AST::Node::Ptr true_stmt = node->get_true_statement();
        const AST::Node::Ptr false_stmt = node->get_false_statement();

        std::string cond_string = StringUtils::delete_surrounding_brackets(render(cond));
        std::string true_stmt_string =
            true_stmt ? block_or_single_statement_to_string(true_stmt) : ";";
        std::string false_stmt_string = block_or_single_statement_to_string(false_stmt);

        result = "if(" + cond_string + ")" + true_stmt_string;

        if(false_stmt_string.size() != 0) {
            result += "\nelse" + false_stmt_string;
        }
    }
    return result;
}

std::string VerilogGenerator::render_repeatstatement(const AST::RepeatStatement::Ptr node) const
{
    std::string result;
    if(node) {
        AST::Node::Ptr times = node->get_times();
        AST::Node::Ptr stmt = node->get_statement();

        std::string times_str =
            StringUtils::delete_surrounding_brackets(StringUtils::remove_whitespace(render(times)));
        std::string stmt_str = block_or_single_statement_to_string(stmt);

        result = "repeat(" + times_str + ")" + stmt_str;
    }
    return result;
}

std::string VerilogGenerator::render_forstatement(const AST::ForStatement::Ptr node) const
{
    std::string result;
    if(node) {
        AST::BlockingSubstitution::Ptr pre = node->get_pre();
        AST::Node::Ptr cond = node->get_cond();
        AST::BlockingSubstitution::Ptr post = node->get_post();
        AST::Node::Ptr stmt = node->get_statement();

        std::string pre_str = StringUtils::remove_whitespace(render(pre));
        std::string cond_str =
            StringUtils::delete_surrounding_brackets(StringUtils::remove_whitespace(render(cond)));
        std::string post_str =
            StringUtils::remove_last_semicolon(StringUtils::remove_whitespace(render(post)));
        std::string stmt_str = block_or_single_statement_to_string(stmt);

        result = "for(" + pre_str + " " + cond_str + "; " + post_str + ")" + stmt_str;
    }
    return result;
}

std::string VerilogGenerator::render_whilestatement(const AST::WhileStatement::Ptr node) const
{
    std::string result;
    if(node) {
        AST::Node::Ptr cond = node->get_cond();
        AST::Node::Ptr stmt = node->get_statement();

        std::string cond_str =
            StringUtils::delete_surrounding_brackets(StringUtils::remove_whitespace(render(cond)));

        result = "while(" + cond_str + ")";
        result.append(block_or_single_statement_to_string(stmt));
    }
    return result;
}

std::string VerilogGenerator::render_casestatement(const AST::CaseStatement::Ptr node) const
{
    std::string result;
    if(node) {
        const AST::Case::ListPtr caselist = node->get_caselist();
        std::string comp = StringUtils::delete_surrounding_brackets(render(node->get_comp()));
        result = "case(" + comp + ")\n";

        if(caselist) {
            auto func = [&](const AST::Case::Ptr n) { return indent(render(n)) + "\n"; };
            result += StringUtils::join<AST::Case::List, AST::Case::Ptr>(*caselist, func, func);
        }

        result += "endcase";
    }
    return result;
}

std::string VerilogGenerator::render_casexstatement(const AST::CasexStatement::Ptr node) const
{
    std::string result;
    if(node) {
        const AST::Case::ListPtr caselist = node->get_caselist();
        std::string comp = StringUtils::delete_surrounding_brackets(render(node->get_comp()));
        result = "casex(" + comp + ")\n";

        if(caselist) {
            auto func = [&](const AST::Case::Ptr n) { return indent(render(n)) + "\n"; };
            result += StringUtils::join<AST::Case::List, AST::Case::Ptr>(*caselist, func, func);
        }

        result += "endcase";
    }
    return result;
}

std::string VerilogGenerator::render_casezstatement(const AST::CasezStatement::Ptr node) const
{
    std::string result;
    if(node) {
        const AST::Case::ListPtr caselist = node->get_caselist();
        std::string comp = StringUtils::delete_surrounding_brackets(render(node->get_comp()));
        result = "casez(" + comp + ")\n";

        if(caselist) {
            auto func = [&](const AST::Case::Ptr n) { return indent(render(n)) + "\n"; };
            result += StringUtils::join<AST::Case::List, AST::Case::Ptr>(*caselist, func, func);
        }

        result += "endcase";
    }
    return result;
}

std::string
VerilogGenerator::render_uniquecasestatement(const AST::UniqueCaseStatement::Ptr node) const
{
    std::string result;
    if(node) {
        const AST::Case::ListPtr caselist = node->get_caselist();
        std::string comp = StringUtils::delete_surrounding_brackets(render(node->get_comp()));
        result = "unique case(" + comp + ")\n";

        if(caselist) {
            auto func = [&](const AST::Case::Ptr n) { return indent(render(n)) + "\n"; };
            result += StringUtils::join<AST::Case::List, AST::Case::Ptr>(*caselist, func, func);
        }

        result += "endcase";
    }
    return result;
}

std::string
VerilogGenerator::render_prioritycasestatement(const AST::PriorityCaseStatement::Ptr node) const
{
    std::string result;
    if(node) {
        const AST::Case::ListPtr caselist = node->get_caselist();
        std::string comp = StringUtils::delete_surrounding_brackets(render(node->get_comp()));
        result = "priority case(" + comp + ")\n";

        if(caselist) {
            auto func = [&](const AST::Case::Ptr n) { return indent(render(n)) + "\n"; };
            result += StringUtils::join<AST::Case::List, AST::Case::Ptr>(*caselist, func, func);
        }

        result += "endcase";
    }
    return result;
}
std::string VerilogGenerator::render_case(const AST::Case::Ptr node) const
{
    std::string result;
    if(node) {
        const AST::Node::ListPtr condlist = node->get_cond();

        if(condlist) {
            auto func = [&](const AST::Node::Ptr n) { return render(n) + ", "; };
            auto func_last = [&](const AST::Node::Ptr n) { return render(n); };
            result +=
                StringUtils::join<AST::Node::List, AST::Node::Ptr>(*condlist, func, func_last);
        } else {
            result = "default";
        }

        result += ": " + render(node->get_statement());
    }
    return result;
}

std::string VerilogGenerator::render_block(const AST::Block::Ptr node) const
{
    std::string result;
    if(node) {
        result = "begin";

        const std::string &scope = node->get_scope();
        if(scope.size() != 0) {
            result += ": " + scope;
        }

        result += "\n";

        const AST::Node::ListPtr statements = node->get_statements();
        if(statements) {
            auto func = [&](const AST::Node::Ptr n) { return indent(render(n)) + "\n"; };
            result += StringUtils::join<AST::Node::List, AST::Node::Ptr>(*statements, func, func);
        }

        result += "end";
    }
    return result;
}

std::string VerilogGenerator::render_initial(const AST::Initial::Ptr node) const
{
    std::string result;
    if(node) {
        result = "\ninitial" + block_or_single_statement_to_string(node->get_statement());
    }
    return result;
}

std::string VerilogGenerator::render_eventstatement(const AST::EventStatement::Ptr node) const
{
    std::string result;
    if(node) {
        AST::Node::Ptr stmt = node->get_statement();
        std::string stmt_str = block_or_single_statement_to_string(stmt);

        result =
            "@(" + StringUtils::delete_surrounding_brackets(render(node->get_senslist())) + ")";

        if(stmt_str.size() == 0) {
            result.append(";");
        } else {
            result.append(stmt_str);
        }
    }
    return result;
}

std::string VerilogGenerator::render_waitstatement(const AST::WaitStatement::Ptr node) const
{
    std::string result;
    if(node) {
        result = "wait(" + StringUtils::delete_surrounding_brackets(render(node->get_cond())) + ")";

        std::string stmt = render(node->get_statement());
        if(stmt.size() == 0) {
            result += ";";
        } else {
            result += stmt;
        }
    }
    return result;
}

std::string VerilogGenerator::render_foreverstatement(const AST::ForeverStatement::Ptr node) const
{
    std::string result;
    if(node) {
        result = "forever " + render(node->get_statement());
    }
    return result;
}

std::string VerilogGenerator::render_delaystatement(const AST::DelayStatement::Ptr node) const
{
    std::string result;
    if(node) {
        result = "#" + render(node->get_delay());
    }
    return result;
}

std::string VerilogGenerator::render_instancelist(const AST::Instancelist::Ptr node) const
{
    std::string result;
    if(node) {
        result = "\n" + StringUtils::escape(node->get_module()) + " ";

        const AST::ParamArg::ListPtr paramlist = node->get_parameterlist();
        if(paramlist) {
            if(paramlist->size() > 0) {
                result += "#(\n";
                auto func = [&](const AST::ParamArg::Ptr n) { return indent(render(n)) + ",\n"; };
                auto func_last = [&](const AST::ParamArg::Ptr n) { return indent(render(n)); };
                result += StringUtils::join<AST::ParamArg::List, AST::ParamArg::Ptr>(
                    *paramlist, func, func_last);
                result += "\n)\n";
            }
        }

        const AST::Instance::ListPtr instances = node->get_instances();

        if(instances) {
            auto func = [&](const AST::Instance::Ptr n) { return render(n) + ",\n"; };
            auto func_last = [&](const AST::Instance::Ptr n) { return render(n); };
            result += StringUtils::join<AST::Instance::List, AST::Instance::Ptr>(*instances, func,
                                                                                 func_last);
        }

        result += ";\n";
    }
    return result;
}

std::string VerilogGenerator::render_instance(const AST::Instance::Ptr node) const
{
    std::string result;
    if(node) {
        result += StringUtils::escape(node->get_name());
        if(node->get_array()) {
            result += render(as_node(node->get_array()));
        }

        result += " (";
        const AST::PortArg::ListPtr portlist = node->get_portlist();
        if(portlist) {
            result += "\n";
            auto func = [&](const AST::PortArg::Ptr n) { return indent(render(n)) + ",\n"; };
            auto func_last = [&](const AST::PortArg::Ptr n) { return indent(render(n)) + "\n"; };
            result += StringUtils::join<AST::PortArg::List, AST::PortArg::Ptr>(*portlist, func,
                                                                               func_last);
        }
        result += ")";
    }
    return result;
}

std::string VerilogGenerator::render_paramarg(const AST::ParamArg::Ptr node) const
{
    std::string result;
    if(node) {
        std::string name = StringUtils::escape(node->get_name());

        if(name.size() != 0) {
            result = "." + name + " (";
        }

        result += StringUtils::delete_surrounding_brackets(render(node->get_value()));

        if(name.size() != 0) {
            result += ")";
        }
    }
    return result;
}

std::string VerilogGenerator::render_portarg(const AST::PortArg::Ptr node) const
{
    std::string result;
    if(node) {
        std::string name = StringUtils::escape(node->get_name());

        if(name.size() != 0) {
            result = "." + name + " (";
        }

        result += StringUtils::delete_surrounding_brackets(render(node->get_value()));

        if(name.size() != 0) {
            result += ")";
        }
    }
    return result;
}

std::string VerilogGenerator::render_function(const AST::Function::Ptr node) const
{
    std::string result;
    if(node) {
        result = "\nfunction ";
        switch(node->get_lifetime()) {
        case AST::Function::LifetimeEnum::AUTOMATIC:
            result += "automatic ";
            break;
        case AST::Function::LifetimeEnum::STATIC:
            result += "static ";
            break;
        case AST::Function::LifetimeEnum::NONE:
            break;
        }

        const std::string rt = data_type_to_string(node->get_return_type());
        if(!rt.empty()) {
            result += rt + " ";
        }

        result += StringUtils::escape(node->get_name());

        const AST::Arg::ListPtr args = node->get_args();
        if(args) {
            result += "(";
            auto func = [&](const AST::Arg::Ptr n) { return render(n) + ", "; };
            auto func_last = [&](const AST::Arg::Ptr n) { return render(n); };
            result += StringUtils::join<AST::Arg::List, AST::Arg::Ptr>(*args, func, func_last);
            result += ")";
        }

        result += ";\n";

        const AST::Node::ListPtr statements = node->get_statements();
        if(statements) {
            auto func = [&](const AST::Node::Ptr n) { return indent(render(n)) + "\n"; };
            result += StringUtils::join<AST::Node::List, AST::Node::Ptr>(*statements, func, func);
        }

        result += "endfunction\n";
    }
    return result;
}

std::string VerilogGenerator::render_task(const AST::Task::Ptr node) const
{
    std::string result;
    if(node) {
        result = "\ntask ";
        switch(node->get_lifetime()) {
        case AST::Task::LifetimeEnum::AUTOMATIC:
            result += "automatic ";
            break;
        case AST::Task::LifetimeEnum::STATIC:
            result += "static ";
            break;
        case AST::Task::LifetimeEnum::NONE:
            break;
        }

        result += StringUtils::escape(node->get_name());

        const AST::Arg::ListPtr args = node->get_args();
        if(args) {
            result += "(";
            auto func = [&](const AST::Arg::Ptr n) { return render(n) + ", "; };
            auto func_last = [&](const AST::Arg::Ptr n) { return render(n); };
            result += StringUtils::join<AST::Arg::List, AST::Arg::Ptr>(*args, func, func_last);
            result += ")";
        }

        result += ";\n";

        const AST::Node::ListPtr statements = node->get_statements();
        if(statements) {
            auto func = [&](const AST::Node::Ptr n) { return indent(render(n)) + "\n"; };
            result += StringUtils::join<AST::Node::List, AST::Node::Ptr>(*statements, func, func);
        }

        result += "endtask\n";
    }
    return result;
}

std::string VerilogGenerator::render_call(const AST::Call::Ptr node) const
{
    // Neutral subroutine call (task-or-function unresolved): render like a task
    // call (parens only when there are arguments).
    std::string result;
    if(node) {
        result = identifier_to_string(node);
        const AST::Node::ListPtr args = node->get_args();
        if(args && args->size() > 0) {
            result += "(";
            auto func = [&](const AST::Node::Ptr n) { return render(n) + ", "; };
            auto func_last = [&](const AST::Node::Ptr n) { return render(n); };
            result += StringUtils::join<AST::Node::List, AST::Node::Ptr>(*args, func, func_last);
            result += ")";
        }
    }
    return result;
}

std::string VerilogGenerator::render_functioncall(const AST::FunctionCall::Ptr node) const
{
    std::string result;
    if(node) {
        result = identifier_to_string(node);
        result += "(";
        const AST::Node::ListPtr args = node->get_args();
        if(args) {
            auto func = [&](const AST::Node::Ptr n) { return render(n) + ", "; };
            auto func_last = [&](const AST::Node::Ptr n) { return render(n); };
            result += StringUtils::join<AST::Node::List, AST::Node::Ptr>(*args, func, func_last);
        }
        result += ")";
    }
    return result;
}

std::string VerilogGenerator::render_taskcall(const AST::TaskCall::Ptr node) const
{
    std::string result;
    if(node) {
        result = identifier_to_string(node);
        const AST::Node::ListPtr args = node->get_args();
        if(args && args->size() > 0) {
            result += "(";
            auto func = [&](const AST::Node::Ptr n) { return render(n) + ", "; };
            auto func_last = [&](const AST::Node::Ptr n) { return render(n); };
            result += StringUtils::join<AST::Node::List, AST::Node::Ptr>(*args, func, func_last);
            result += ")";
        }
    }
    return result;
}

std::string VerilogGenerator::render_generatestatement(const AST::GenerateStatement::Ptr node) const
{
    std::string result;
    if(node) {
        result = "\ngenerate\n";
        const AST::Node::ListPtr items = node->get_items();
        if(items) {
            auto func = [&](const AST::Node::Ptr n) { return indent(render(n)) + "\n"; };
            result += StringUtils::join<AST::Node::List, AST::Node::Ptr>(*items, func, func);
        }
        result += "endgenerate\n";
    }
    return result;
}

std::string VerilogGenerator::render_systemcall(const AST::SystemCall::Ptr node) const
{
    std::string result;
    if(node) {
        result = "$" + StringUtils::escape(node->get_syscall());

        const AST::Node::ListPtr args = node->get_args();
        if(args) {
            if(args->size() > 0) {
                result += "(";
                auto func = [&](const AST::Node::Ptr n) { return render(n) + ", "; };
                auto func_last = [&](const AST::Node::Ptr n) { return render(n); };
                result +=
                    StringUtils::join<AST::Node::List, AST::Node::Ptr>(*args, func, func_last);
                result += ")";
            }
        }
    }
    return result;
}

std::string VerilogGenerator::render_disable(const AST::Disable::Ptr node) const
{
    std::string result;
    if(node) {
        result = "disable " + render(node->get_dest());
    }
    return result;
}

std::string VerilogGenerator::render_parallelblock(const AST::ParallelBlock::Ptr node) const
{
    std::string result;
    if(node) {
        result = "fork";

        const std::string &scope = node->get_scope();
        if(scope.size() > 0) {
            result += ": " + StringUtils::escape(scope);
        }

        result += "\n";

        const AST::Node::ListPtr statements = node->get_statements();
        if(statements) {
            auto func = [&](const AST::Node::Ptr n) { return render(n) + "\n"; };
            result += StringUtils::join<AST::Node::List, AST::Node::Ptr>(*statements, func, func);
        }

        result += "join";
    }
    return result;
}

std::string VerilogGenerator::render_singlestatement(const AST::SingleStatement::Ptr node) const
{
    const AST::DelayStatement::Ptr delay = node->get_delay();
    std::string result;

    if(delay) {
        result.append(render(delay));
        result.append(" ");
    }

    result.append(render(node->get_statement()));
    result.append(";");
    return result;
}

std::string VerilogGenerator::unary_operators_to_string(AST::NodeType node_type,
                                                        const AST::Node::Ptr right) const
{
    std::string result;
    result = "(" + std::string(AST::NodeOperators::get_char(node_type)) + render(right) + ")";
    return result;
}

std::string VerilogGenerator::indent(const std::string str) const
{
    return StringUtils::indent(str, get_indent_str());
}

std::string VerilogGenerator::operators_to_string(AST::NodeType node_type,
                                                  const AST::Node::Ptr left,
                                                  const AST::Node::Ptr right) const
{
    std::string result;
    int current_priority = AST::NodeOperators::get_priority(node_type);
    int left_priority = AST::NodeOperators::get_priority(left->get_node_type());
    int right_priority = AST::NodeOperators::get_priority(right->get_node_type());
    std::string left_str = render(left);
    std::string right_str = render(right);

    if(current_priority > left_priority) {
        left_str = StringUtils::delete_surrounding_brackets(left_str);
    }

    if(current_priority > right_priority) {
        right_str = StringUtils::delete_surrounding_brackets(right_str);
    }

    result = "(" + left_str + " " + std::string(AST::NodeOperators::get_char(node_type)) + " " +
             right_str + ")";

    return result;
}

std::string VerilogGenerator::identifier_to_string(const AST::Identifier::Ptr node) const
{
    std::string result;
    if(!node) {
        return result;
    }
    // SV `::` scope.
    const AST::ScopeName::ListPtr scope = node->get_scope();
    if(scope) {
        for(const AST::ScopeName::Ptr &s : *scope) {
            result += render(s);
        }
    }
    // Verilog `.` hierarchy.
    const AST::HierName::Ptr hier = node->get_hier();
    if(hier) {
        result += render(hier);
    }
    result += StringUtils::escape(node->get_name());
    return result;
}

std::string VerilogGenerator::block_or_single_statement_to_string(AST::Node::Ptr stmt) const
{
    std::string result;

    if(stmt) {
        std::string stmt_str = render(stmt);

        if(stmt->get_node_type() == AST::NodeType::Block) {
            result.append(" " + stmt_str);
        } else {
            result.append("\n" + indent(stmt_str));
        }
    }

    return result;
}

std::string VerilogGenerator::ports_list_to_string(const AST::Port::ListPtr ports, int length) const
{
    std::string result;
    std::string blanks(length + 1, ' ');

    if(ports) {
        auto func = [&](const AST::Port::Ptr n) {
            return StringUtils::remove_last_semicolon(render(n)) + ",\n" + blanks;
        };
        auto func_last = [&](const AST::Port::Ptr n) {
            return StringUtils::remove_last_semicolon(render(n));
        };

        std::string strports =
            StringUtils::join<AST::Port::List, AST::Port::Ptr>(*ports, func, func_last);

        result += "(";
        result += strports + ")";
    }

    return result;
}

std::string VerilogGenerator::parameters_list_to_string(const AST::Declaration::ListPtr parameters,
                                                        int length) const
{
    std::string result;
    std::string blanks(length + 2, ' ');

    if(parameters && !parameters->empty()) {
        auto func = [&](const AST::Declaration::Ptr n) {
            return StringUtils::remove_last_semicolon(render(as_node(n))) + ",\n" + blanks;
        };
        auto func_last = [&](const AST::Declaration::Ptr n) {
            return StringUtils::remove_last_semicolon(render(as_node(n)));
        };

        std::string strparameters =
            StringUtils::join<AST::Declaration::List, AST::Declaration::Ptr>(*parameters, func,
                                                                             func_last);

        result += "#(";
        result += strparameters + ")";
    }

    return result;
}

std::string VerilogGenerator::render_enumitem(const AST::EnumItem::Ptr node) const
{
    std::string result;
    if(node) {
        result = StringUtils::escape(node->get_name());
        if(node->get_range()) {
            result += render(as_node(node->get_range()));
        }
        const AST::Node::Ptr value = node->get_value();
        if(value) {
            result += " = " + render(value);
        }
    }
    return result;
}

std::string VerilogGenerator::render_enumtype(const AST::EnumType::Ptr node) const
{
    std::string result;
    if(node) {
        result = "enum";
        const std::string base = data_type_to_string(node->get_base());
        if(!base.empty()) {
            result += " " + base;
        }
        result += " {";
        const AST::EnumItem::ListPtr items = node->get_items();
        if(items) {
            bool first = true;
            for(const AST::EnumItem::Ptr &item : *items) {
                if(!first) {
                    result += ", ";
                }
                result += render(item);
                first = false;
            }
        }
        result += "}";
        const std::string dims = dims_to_string(node->get_packed_dims());
        if(!dims.empty()) {
            result += " " + dims;
        }
    }
    return result;
}

std::string VerilogGenerator::render_structtype(const AST::StructType::Ptr node) const
{
    std::string result;
    if(node) {
        result = "struct";
        if(node->get_is_packed()) {
            result += " packed";
            switch(node->get_signing()) {
            case AST::DataType::SigningEnum::SIGNED:
                result += " signed";
                break;
            case AST::DataType::SigningEnum::UNSIGNED:
                result += " unsigned";
                break;
            case AST::DataType::SigningEnum::NONE:
                break;
            }
        }
        result += " {\n";
        const AST::Member::ListPtr members = node->get_members();
        if(members) {
            for(const AST::Member::Ptr &m : *members) {
                result += "\t" + render(m) + "\n";
            }
        }
        result += "}";
        const std::string dims = dims_to_string(node->get_packed_dims());
        if(!dims.empty()) {
            result += " " + dims;
        }
    }
    return result;
}

std::string VerilogGenerator::render_uniontype(const AST::UnionType::Ptr node) const
{
    std::string result;
    if(node) {
        result = "union";
        if(node->get_is_tagged()) {
            result += " tagged";
        }
        if(node->get_is_packed()) {
            result += " packed";
            switch(node->get_signing()) {
            case AST::DataType::SigningEnum::SIGNED:
                result += " signed";
                break;
            case AST::DataType::SigningEnum::UNSIGNED:
                result += " unsigned";
                break;
            case AST::DataType::SigningEnum::NONE:
                break;
            }
        }
        result += " {\n";
        const AST::Member::ListPtr members = node->get_members();
        if(members) {
            for(const AST::Member::Ptr &m : *members) {
                result += "\t" + render(m) + "\n";
            }
        }
        result += "}";
        const std::string dims = dims_to_string(node->get_packed_dims());
        if(!dims.empty()) {
            result += " " + dims;
        }
    }
    return result;
}

} // namespace Generators
} // namespace Veriparse
