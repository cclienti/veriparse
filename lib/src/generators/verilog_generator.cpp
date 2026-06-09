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

using namespace Veriparse::Misc;

namespace Veriparse
{
namespace Generators
{

typedef std::numeric_limits<double> dbl;

namespace
{
// Integer atom types (byte/shortint/int/longint) are signed by default, so the
// type spells the non-default case: `unsigned` is emitted, the redundant
// `signed` is not (`int x;` / `int unsigned y;`, never `int signed`).
std::string atom_type_string(const std::string &keyword, bool is_signed)
{
    return is_signed ? keyword : keyword + " unsigned";
}
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
        const AST::Parameter::ListPtr params = node->get_params();
        const AST::Node::ListPtr ports = node->get_ports();
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
    if(node) {
        result = StringUtils::escape(node->get_name());
    }
    return result;
}

std::string VerilogGenerator::render_width(const AST::Width::Ptr node) const
{
    std::string result;
    if(node) {
        const AST::Node::Ptr msb = node->get_msb();
        const AST::Node::Ptr lsb = node->get_lsb();
        std::string msb_str, lsb_str;

        if(msb) {
            msb_str = StringUtils::remove_whitespace(
                StringUtils::delete_surrounding_brackets(render(msb)));
        }
        if(lsb) {
            lsb_str = StringUtils::remove_whitespace(
                StringUtils::delete_surrounding_brackets(render(lsb)));
        }

        result = "[" + msb_str + ":" + lsb_str + "]";
    }
    return result;
}

std::string VerilogGenerator::render_length(const AST::Length::Ptr node) const
{
    std::string result;
    if(node) {
        const AST::Node::Ptr msb = node->get_msb();
        const AST::Node::Ptr lsb = node->get_lsb();
        std::string msb_str, lsb_str;

        if(msb) {
            msb_str = StringUtils::remove_whitespace(
                StringUtils::delete_surrounding_brackets(render(msb)));
        }
        if(lsb) {
            lsb_str = StringUtils::remove_whitespace(
                StringUtils::delete_surrounding_brackets(render(lsb)));
        }

        // a null lsb marks the single-size unpacked dimension `[N]` (≡ [0:N-1]);
        // with both bounds it is the explicit range `[msb:lsb]`.
        if(lsb) {
            result = "[" + msb_str + ":" + lsb_str + "]";
        } else {
            result = "[" + msb_str + "]";
        }
    }
    return result;
}

std::string VerilogGenerator::render_identifier(const AST::Identifier::Ptr node) const
{
    std::string result;
    if(node) {
        const std::string &package = node->get_package();
        if(!package.empty()) {
            result = StringUtils::escape(package) + "::";
        }
        const AST::IdentifierScope::Ptr scope = node->get_scope();
        if(scope) {
            result += render(scope);
        }
        result.append(StringUtils::escape(node->get_name()));
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

std::string VerilogGenerator::render_input(const AST::Input::Ptr node) const
{
    std::string result;
    if(node) {
        result = variable_to_string("input", node->get_sign(), node->get_widths(), nullptr, nullptr,
                                    node->get_name());
        result.append(";");
    }
    return result;
}

std::string VerilogGenerator::render_output(const AST::Output::Ptr node) const
{
    std::string result;
    if(node) {
        result = variable_to_string("output", node->get_sign(), node->get_widths(), nullptr,
                                    nullptr, node->get_name());
        result.append(";");
    }
    return result;
}

std::string VerilogGenerator::render_inout(const AST::Inout::Ptr node) const
{
    std::string result;
    if(node) {
        result = variable_to_string("inout", node->get_sign(), node->get_widths(), nullptr, nullptr,
                                    node->get_name());
        result.append(";");
    }
    return result;
}

std::string VerilogGenerator::render_tri(const AST::Tri::Ptr node) const
{
    std::string result;
    if(node) {
        std::string kw = "tri";
        if(node->get_type()) {
            kw += " " + render(node->get_type());
        }
        result = variable_to_string(kw.c_str(), node->get_sign(), node->get_widths(),
                                    node->get_lengths(), node->get_right(), node->get_name());
        result.append(";");
    }
    return result;
}

std::string VerilogGenerator::render_wire(const AST::Wire::Ptr node) const
{
    std::string result;
    if(node) {
        std::string kw = "wire";
        if(node->get_type()) {
            kw += " " + render(node->get_type());
        }
        result = variable_to_string(kw.c_str(), node->get_sign(), node->get_widths(),
                                    node->get_lengths(), node->get_right(), node->get_name());
        result.append(";");
    }
    return result;
}

std::string VerilogGenerator::render_reg(const AST::Reg::Ptr node) const
{
    return render_scalar_variable(node);
}

std::string VerilogGenerator::render_supply0(const AST::Supply0::Ptr node) const
{
    std::string result;
    if(node) {
        std::string kw = "supply0";
        if(node->get_type()) {
            kw += " " + render(node->get_type());
        }
        result = variable_to_string(kw.c_str(), node->get_sign(), node->get_widths(),
                                    node->get_lengths(), node->get_right(), node->get_name());
        result.append(";");
    }
    return result;
}

std::string VerilogGenerator::render_supply1(const AST::Supply1::Ptr node) const
{
    std::string result;
    if(node) {
        std::string kw = "supply1";
        if(node->get_type()) {
            kw += " " + render(node->get_type());
        }
        result = variable_to_string(kw.c_str(), node->get_sign(), node->get_widths(),
                                    node->get_lengths(), node->get_right(), node->get_name());
        result.append(";");
    }
    return result;
}

std::string VerilogGenerator::render_logic(const AST::Logic::Ptr node) const
{
    return render_scalar_variable(node);
}

std::string VerilogGenerator::render_customtypevar(const AST::CustomTypeVar::Ptr node) const
{
    std::string result;
    if(node) {
        // The type is an unresolved Identifier reference (its `package` carries
        // any pkg:: scope) or an inline aggregate def; render it as the type
        // string and reuse the common variable formatter. `widths` carries the
        // packed dimensions of the type (e.g. `my_t [3:0]`).
        const std::string type_str = render(node->get_type());
        result = variable_to_string(type_str.c_str(), false, node->get_widths(),
                                    node->get_lengths(), node->get_right(), node->get_name());
        result.append(";");
    }
    return result;
}

std::string VerilogGenerator::render_integer(const AST::Integer::Ptr node) const
{
    return render_scalar_variable(node);
}

std::string VerilogGenerator::render_real(const AST::Real::Ptr node) const
{
    return render_scalar_variable(node);
}

std::string VerilogGenerator::render_bit(const AST::Bit::Ptr node) const
{
    return render_scalar_variable(node);
}

std::string VerilogGenerator::render_byte(const AST::Byte::Ptr node) const
{
    return render_scalar_variable(node);
}

std::string VerilogGenerator::render_shortint(const AST::Shortint::Ptr node) const
{
    return render_scalar_variable(node);
}

std::string VerilogGenerator::render_int(const AST::Int::Ptr node) const
{
    return render_scalar_variable(node);
}

std::string VerilogGenerator::render_longint(const AST::Longint::Ptr node) const
{
    return render_scalar_variable(node);
}

std::string VerilogGenerator::render_shortreal(const AST::Shortreal::Ptr node) const
{
    return render_scalar_variable(node);
}

std::string VerilogGenerator::render_realtime(const AST::Realtime::Ptr node) const
{
    return render_scalar_variable(node);
}

std::string VerilogGenerator::render_genvar(const AST::Genvar::Ptr node) const
{
    std::string result;
    if(node) {
        result = "genvar " + StringUtils::escape(node->get_name());
        result.append(";");
    }
    return result;
}

std::string VerilogGenerator::render_ioport(const AST::Ioport::Ptr node) const
{
    std::string result;
    if(node) {
        const AST::Node::Ptr first = node->get_first();
        const AST::Node::Ptr second = node->get_second();
        AST::Width::ListPtr widths;
        std::string name;
        std::string variable;
        bool sign = false;

        if(first) {
            widths = AST::cast_to<AST::IODir>(first)->get_widths();
            sign = AST::cast_to<AST::IODir>(first)->get_sign();
            name = AST::cast_to<AST::IODir>(first)->get_name();
            switch(first->get_node_type()) {
            case AST::NodeType::Input:
                variable = "input";
                break;
            case AST::NodeType::Output:
                variable = "output";
                break;
            case AST::NodeType::Inout:
                variable = "inout";
                break;
            default:
                break;
            }
        }

        if(second) {
            // A net (wire/tri/supply) prints its keyword and, when present, the
            // explicit integer_vector data type it carries: `wire logic [3:0] x`.
            auto append_net = [&](const char *kw) {
                variable.append(" ");
                variable.append(kw);
                const AST::Node::Ptr net_data_type = AST::cast_to<AST::Net>(second)->get_type();
                if(net_data_type) {
                    variable.append(" " + render(net_data_type));
                }
            };

            switch(second->get_node_type()) {
            case AST::NodeType::Wire:
                append_net("wire");
                break;
            case AST::NodeType::Tri:
                append_net("tri");
                break;
            case AST::NodeType::Supply0:
                append_net("supply0");
                break;
            case AST::NodeType::Supply1:
                append_net("supply1");
                break;
            case AST::NodeType::Logic:
                variable.append(" logic");
                break;
            case AST::NodeType::Reg:
                variable.append(" reg");
                break;
            case AST::NodeType::Bit:
                variable.append(" bit");
                break;
            // atom / non-integer / integer / real types are signed by default:
            // drop the redundant `signed`; atoms still spell explicit `unsigned`.
            case AST::NodeType::Byte:
                variable.append(
                    " " + atom_type_string("byte", AST::cast_to<AST::Byte>(second)->get_sign()));
                sign = false;
                break;
            case AST::NodeType::Shortint:
                variable.append(
                    " " +
                    atom_type_string("shortint", AST::cast_to<AST::Shortint>(second)->get_sign()));
                sign = false;
                break;
            case AST::NodeType::Int:
                variable.append(
                    " " + atom_type_string("int", AST::cast_to<AST::Int>(second)->get_sign()));
                sign = false;
                break;
            case AST::NodeType::Longint:
                variable.append(
                    " " +
                    atom_type_string("longint", AST::cast_to<AST::Longint>(second)->get_sign()));
                sign = false;
                break;
            case AST::NodeType::Integer:
                variable.append(" integer");
                sign = false;
                break;
            case AST::NodeType::Shortreal:
                variable.append(" shortreal");
                sign = false;
                break;
            case AST::NodeType::Realtime:
                variable.append(" realtime");
                sign = false;
                break;
            case AST::NodeType::Real:
                variable.append(" real");
                sign = false;
                break;
            case AST::NodeType::CustomTypeVar:
                // user-defined / package-scoped type or inline aggregate: append
                // the type (Identifier keyword/name or struct/union/enum def)
                variable.append(" " + render(AST::cast_to<AST::CustomTypeVar>(second)->get_type()));
                break;
            default:
                break;
            }
        }

        result = variable_to_string(variable.c_str(), sign, widths, nullptr, nullptr, name);
        result.append(";");
    }
    return result;
}

std::string VerilogGenerator::render_parameter(const AST::Parameter::Ptr node) const
{
    std::string result;
    if(node) {
        std::string variable;
        bool sign;
        AST::Node::Ptr value = node->get_value();
        std::string value_str;

        if(value) {
            value_str = " = " + render(value);
        }

        switch(node->get_type()) {
        case AST::Parameter::TypeEnum::INTEGER:
            variable = "integer ";
            sign = false;
            break;
        case AST::Parameter::TypeEnum::INT:
            variable = "int ";
            sign = false;
            break;
        case AST::Parameter::TypeEnum::BIT:
            variable = "bit ";
            sign = false;
            break;
        case AST::Parameter::TypeEnum::BYTE:
            variable = "byte ";
            sign = false;
            break;
        case AST::Parameter::TypeEnum::SHORTINT:
            variable = "shortint ";
            sign = false;
            break;
        case AST::Parameter::TypeEnum::LONGINT:
            variable = "longint ";
            sign = false;
            break;
        case AST::Parameter::TypeEnum::LOGIC:
            variable = "logic ";
            sign = false;
            break;
        case AST::Parameter::TypeEnum::REAL:
            variable = "real ";
            sign = false;
            break;
        default:
            sign = node->get_sign();
            break;
        }

        result = parameter_to_string("parameter ", variable, sign, node->get_widths(),
                                     node->get_name(), value_str);
        result.append(";");
    }
    return result;
}

std::string VerilogGenerator::render_localparam(const AST::Localparam::Ptr node) const
{
    std::string result;
    if(node) {
        std::string variable;
        bool sign;
        AST::Node::Ptr value = node->get_value();
        std::string value_str;

        if(value) {
            value_str = " = " + render(value);
        }

        switch(node->get_type()) {
        case AST::Localparam::TypeEnum::INTEGER:
            variable = "integer ";
            sign = false;
            break;
        case AST::Localparam::TypeEnum::INT:
            variable = "int ";
            sign = false;
            break;
        case AST::Localparam::TypeEnum::BIT:
            variable = "bit ";
            sign = false;
            break;
        case AST::Localparam::TypeEnum::BYTE:
            variable = "byte ";
            sign = false;
            break;
        case AST::Localparam::TypeEnum::SHORTINT:
            variable = "shortint ";
            sign = false;
            break;
        case AST::Localparam::TypeEnum::LONGINT:
            variable = "longint ";
            sign = false;
            break;
        case AST::Localparam::TypeEnum::LOGIC:
            variable = "logic ";
            sign = false;
            break;
        case AST::Localparam::TypeEnum::REAL:
            variable = "real ";
            sign = false;
            break;
        default:
            sign = node->get_sign();
            break;
        }

        result = parameter_to_string("localparam ", variable, sign, node->get_widths(),
                                     node->get_name(), value_str);

        result.append(";");
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

std::string VerilogGenerator::render_cast(const AST::Cast::Ptr node) const
{
    std::string result;
    if(node) {
        result = render(node->get_type()) + "'(" +
                 StringUtils::delete_surrounding_brackets(render(node->get_expr())) + ")";
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
        result += render(node->get_array());

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

std::string VerilogGenerator::render_data_type(const AST::Node::Ptr node) const
{
    if(!node) {
        return "";
    }

    // Render the bare type (no variable name, no `;`). Vector types
    // (bit/reg/logic) carry their own signing and packed dimensions; atom /
    // non-integer types are fixed-width keywords.
    std::string result;
    switch(node->get_node_type()) {
    case AST::NodeType::Bit: {
        const auto v = AST::cast_to<AST::Bit>(node);
        result = variable_to_string("bit", v->get_sign(), v->get_widths(), nullptr, nullptr, "");
        break;
    }
    case AST::NodeType::Reg: {
        const auto v = AST::cast_to<AST::Reg>(node);
        result = variable_to_string("reg", v->get_sign(), v->get_widths(), nullptr, nullptr, "");
        break;
    }
    case AST::NodeType::Logic: {
        const auto v = AST::cast_to<AST::Logic>(node);
        result = variable_to_string("logic", v->get_sign(), v->get_widths(), nullptr, nullptr, "");
        break;
    }
    case AST::NodeType::Byte:
        result = atom_type_string("byte", AST::cast_to<AST::Byte>(node)->get_sign());
        break;
    case AST::NodeType::Shortint:
        result = atom_type_string("shortint", AST::cast_to<AST::Shortint>(node)->get_sign());
        break;
    case AST::NodeType::Int:
        result = atom_type_string("int", AST::cast_to<AST::Int>(node)->get_sign());
        break;
    case AST::NodeType::Longint:
        result = atom_type_string("longint", AST::cast_to<AST::Longint>(node)->get_sign());
        break;
    case AST::NodeType::Integer:
        result = "integer";
        break;
    case AST::NodeType::Shortreal:
        result = "shortreal";
        break;
    case AST::NodeType::Real:
        result = "real";
        break;
    case AST::NodeType::Realtime:
        result = "realtime";
        break;
    case AST::NodeType::CustomTypeVar: {
        // Named / aggregate type carrying packed dimensions (e.g. `my_t [3:0]`).
        const auto v = AST::cast_to<AST::CustomTypeVar>(node);
        const std::string type_str = render(v->get_type());
        result = variable_to_string(type_str.c_str(), false, v->get_widths(), nullptr, nullptr, "");
        break;
    }
    default:
        // Inline struct/union/enum def or named-type reference (Identifier):
        // these already render as a bare type.
        result = render(node);
        break;
    }

    // Drop the trailing space left by the empty variable name so the caller
    // controls the spacing.
    while(!result.empty() && result.back() == ' ') {
        result.pop_back();
    }
    return result;
}

std::string VerilogGenerator::render_scalar_variable(const AST::Variable::Ptr node) const
{
    if(!node) {
        return "";
    }
    // The bare type (keyword + signing + packed dims) is rendered in exactly one
    // place — render_data_type. Here we only add the declared name, unpacked
    // dimensions and initializer. Passing the bare type as the `variable` keyword
    // with no extra sign/widths reproduces the per-type formatting byte-for-byte.
    std::string result =
        variable_to_string(render_data_type(AST::to_node(node)).c_str(), false, nullptr,
                           node->get_lengths(), node->get_right(), node->get_name());
    result.append(";");
    return result;
}

std::string VerilogGenerator::render_datamodifier(const AST::DataModifier::Ptr node) const
{
    if(!node) {
        return "";
    }
    // [const] [var] [lifetime] prefix (IEEE 1800-2017 §6.8), then the underlying
    // data-type declaration (which carries the name, dims and initializer).
    std::string prefix;
    if(node->get_is_const()) {
        prefix += "const ";
    }
    if(node->get_is_var()) {
        prefix += "var ";
    }
    switch(node->get_lifetime()) {
    case AST::DataModifier::LifetimeEnum::AUTOMATIC:
        prefix += "automatic ";
        break;
    case AST::DataModifier::LifetimeEnum::STATIC:
        prefix += "static ";
        break;
    case AST::DataModifier::LifetimeEnum::NONE:
        break;
    }
    return prefix + render(node->get_datatype());
}

std::string VerilogGenerator::render_implicittype(const AST::ImplicitType::Ptr node) const
{
    if(!node) {
        return "";
    }
    // data_type_or_implicit = implicit: no type keyword, only the optional
    // signing and packed dims, then name/unpacked dims/initializer. The enclosing
    // DataModifier supplies the `var`/`const` keyword.
    std::string result =
        variable_to_string("", node->get_sign(), node->get_widths(), node->get_lengths(),
                           node->get_right(), node->get_name());
    // variable_to_string injects a separator space for the (empty) type keyword;
    // drop the leading space so the `var `/`const ` prefix sits flush.
    const size_t start = result.find_first_not_of(' ');
    result = (start == std::string::npos) ? "" : result.substr(start);
    result.append(";");
    return result;
}

std::string VerilogGenerator::render_function(const AST::Function::Ptr node) const
{
    std::string result;
    if(node) {
        result = "\nfunction ";
        if(node->get_automatic()) {
            result += "automatic ";
        }

        // explicit return type (built-in scalar / struct / named) is its own
        // type node in rettype_ref, carrying its signing and packed dims; the
        // implicit form (`function [3:0] f` / `function signed [3:0] f`) has
        // only retwidths/retsign.
        if(node->get_rettype_ref()) {
            result += render_data_type(node->get_rettype_ref()) + " ";
        }
        result += widths_list_to_string(node->get_retwidths());

        result += StringUtils::escape(node->get_name());

        const std::string ports_str = ports_list_to_string(node->get_ports(), result.size());

        if(ports_str.size() != 0) {
            result += " ";
            result += ports_str;
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

std::string VerilogGenerator::render_functioncall(const AST::FunctionCall::Ptr node) const
{
    std::string result;
    if(node) {
        const std::string package = node->get_package();
        if(!package.empty()) {
            result = StringUtils::escape(package) + "::";
        }
        result += StringUtils::escape(node->get_name());
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

std::string VerilogGenerator::render_task(const AST::Task::Ptr node) const
{
    std::string result;
    if(node) {
        result = "\ntask ";

        if(node->get_automatic()) {
            result += "automatic ";
        }

        result += StringUtils::escape(node->get_name());

        const std::string ports_str = ports_list_to_string(node->get_ports(), result.size());

        if(ports_str.size() != 0) {
            result += " ";
            result += ports_str;
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

std::string VerilogGenerator::render_taskcall(const AST::TaskCall::Ptr node) const
{
    std::string result;
    if(node) {
        const std::string package = node->get_package();
        if(!package.empty()) {
            result = StringUtils::escape(package) + "::";
        }
        result += StringUtils::escape(node->get_name());

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

std::string
VerilogGenerator::render_identifierscopelabel(const AST::IdentifierScopeLabel::Ptr node) const
{
    std::string result;
    if(node) {
        result = StringUtils::escape(node->get_scope());
        const std::string &loop = render(node->get_loop());
        if(loop.size() > 0) {
            result += "[" + loop + "]";
        }
        result += ".";
    }
    return result;
}

std::string VerilogGenerator::render_identifierscope(const AST::IdentifierScope::Ptr node) const
{
    std::string result;
    if(node) {
        const AST::IdentifierScopeLabel::ListPtr labellist = node->get_labellist();
        if(labellist) {
            auto func = [&](const AST::IdentifierScopeLabel::Ptr n) { return render(n); };
            result +=
                StringUtils::join<AST::IdentifierScopeLabel::List, AST::IdentifierScopeLabel::Ptr>(
                    *labellist, func, func);
        }
    }
    return result;
}

std::string VerilogGenerator::render_disable(const AST::Disable::Ptr node) const
{
    std::string result;
    if(node) {
        result = "disable " + node->get_dest();
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

std::string VerilogGenerator::variable_to_string(const char *variable, bool is_signed,
                                                 const AST::Width::ListPtr widths,
                                                 const AST::Length::ListPtr lengths,
                                                 const AST::Rvalue::Ptr rvalue,
                                                 const std::string &name) const
{
    std::string result(variable);
    std::string length_str;
    std::string rvalue_str;

    result.append(" ");

    if(is_signed) {
        result.append("signed ");
    }

    result.append(widths_list_to_string(widths));

    result.append(StringUtils::escape(name));

    if(lengths) {
        for(const AST::Length::Ptr &length : *lengths) {
            length_str += StringUtils::remove_whitespace(render(length));
        }

        if(!length_str.empty()) {
            result = StringUtils::remove_last_semicolon(result);
            result.append(" ");
            result.append(length_str);
        }
    }

    if(rvalue) {
        result.append(" = ");
        result.append(render(rvalue));
    }

    return result;
}

std::string VerilogGenerator::parameter_to_string(const char *parameter, std::string variable,
                                                  bool is_signed, const AST::Width::ListPtr widths,
                                                  const std::string &name,
                                                  const std::string value) const
{
    std::string result;
    std::string sign_str = is_signed ? "signed " : "";
    std::string width_str;

    width_str = widths_list_to_string(widths);

    result = parameter + sign_str + variable + width_str + StringUtils::escape(name) + value;
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

std::string VerilogGenerator::widths_list_to_string(const AST::Width::ListPtr widths) const
{
    std::string result;

    if(widths) {
        for(const AST::Width::Ptr &width : *widths) {
            result += StringUtils::remove_whitespace(render(width));
            result += " ";
        }
    }

    return result;
}

std::string VerilogGenerator::ports_list_to_string(const AST::Node::ListPtr ports, int length) const
{
    std::string result;
    std::string blanks(length + 1, ' ');

    if(ports) {
        auto func = [&](const AST::Node::Ptr n) {
            return StringUtils::remove_last_semicolon(render(n)) + ",\n" + blanks;
        };
        auto func_last = [&](const AST::Node::Ptr n) {
            return StringUtils::remove_last_semicolon(render(n));
        };

        std::string strports =
            StringUtils::join<AST::Node::List, AST::Node::Ptr>(*ports, func, func_last);

        result += "(";
        result += strports + ")";
    }

    return result;
}

std::string VerilogGenerator::parameters_list_to_string(const AST::Parameter::ListPtr parameters,
                                                        int length) const
{
    std::string result;
    std::string blanks(length + 2, ' ');

    if(parameters) {
        auto func = [&](const AST::Node::Ptr n) {
            return StringUtils::remove_last_semicolon(render(n)) + ",\n" + blanks;
        };
        auto func_last = [&](const AST::Node::Ptr n) {
            return StringUtils::remove_last_semicolon(render(n));
        };

        std::string strparameters = StringUtils::join<AST::Parameter::List, AST::Parameter::Ptr>(
            *parameters, func, func_last);

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
        const AST::Node::Ptr value = node->get_value();
        if(value) {
            result += " = " + render(value);
        }
    }
    return result;
}

std::string VerilogGenerator::render_enumdef(const AST::EnumDef::Ptr node) const
{
    std::string result;
    if(node) {
        result = "enum";
        switch(node->get_base_type()) {
        case AST::EnumDef::Base_typeEnum::LOGIC:
            result += " logic";
            result += widths_list_to_string(node->get_widths());
            break;
        case AST::EnumDef::Base_typeEnum::BIT:
            result += " bit";
            result += widths_list_to_string(node->get_widths());
            break;
        case AST::EnumDef::Base_typeEnum::INT:
            result += " int";
            break;
        default:
            break;
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
    }
    return result;
}

std::string VerilogGenerator::render_typedef(const AST::Typedef::Ptr node) const
{
    std::string result;
    if(node) {
        // A net/variable def (e.g. the alias `typedef logic [3:0] t;`) renders
        // as a full declaration with a trailing ';' and padding; strip them so
        // the typedef wraps just the type.
        std::string def = render(node->get_def());
        while(!def.empty() && (def.back() == ';' || def.back() == ' ')) {
            def.pop_back();
        }
        result = "typedef " + def + " " + StringUtils::escape(node->get_name()) + ";";
    }
    return result;
}

std::string VerilogGenerator::render_structmember(const AST::StructMember::Ptr node) const
{
    std::string result;
    if(node) {
        // The member's type is a full data_type node carrying its own signing
        // and packed dimensions; render it bare (no name, no ';'). Unpacked
        // dimensions and an initializer (the variable_decl_assignment) follow
        // the member name.
        const AST::Node::Ptr type = node->get_type();
        if(type) {
            result += render_data_type(type) + " ";
        }
        result += StringUtils::escape(node->get_name());
        const AST::Length::ListPtr lengths = node->get_lengths();
        if(lengths) {
            std::string length_str;
            for(const AST::Length::Ptr &length : *lengths) {
                length_str += StringUtils::remove_whitespace(render(length));
            }
            if(!length_str.empty()) {
                result += " " + length_str;
            }
        }
        if(node->get_right()) {
            result += " = " + render(node->get_right());
        }
        result += ";";
    }
    return result;
}

std::string VerilogGenerator::render_structdef(const AST::StructDef::Ptr node) const
{
    std::string result;
    if(node) {
        result = "struct";
        if(node->get_packed()) {
            result += " packed";
            if(node->get_sign()) {
                result += " signed";
            }
        }
        result += " {\n";
        const AST::StructMember::ListPtr members = node->get_members();
        if(members) {
            for(const AST::StructMember::Ptr &m : *members) {
                result += "\t" + render(m) + "\n";
            }
        }
        result += "}";
    }
    return result;
}

std::string VerilogGenerator::render_union(const AST::Union::Ptr node) const
{
    std::string result;
    if(node) {
        result = "union";
        if(node->get_packed()) {
            result += " packed";
            if(node->get_sign()) {
                result += " signed";
            }
        }
        result += " {\n";
        const AST::StructMember::ListPtr members = node->get_members();
        if(members) {
            for(const AST::StructMember::Ptr &m : *members) {
                result += "\t" + render(m) + "\n";
            }
        }
        result += "}";
    }
    return result;
}

} // namespace Generators
} // namespace Veriparse
