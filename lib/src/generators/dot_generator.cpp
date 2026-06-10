// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/generators/dot_generator.hpp>
#include <sstream>
#include <fstream>
#include <cstdint>

namespace Veriparse
{
namespace Generators
{

void DotGenerator::save_dot(const std::string &dot, const std::string &filename)
{
    std::ofstream fout(filename);
    fout << "digraph G {\n"
         << "\trankdir=LR;\n"
         << "\tnode [shape=plaintext];\n"
         << dot << "}" << std::endl;
}

std::string DotGenerator::render_node(const AST::Node::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());
        ss << "\tn" << nodeID << " [label=\"{<p0>Node}"
           << " | {line: " << node->get_line() << "}" << "\"];" << std::endl;
    }

    return ss.str();
}

std::string DotGenerator::render_source(const AST::Source::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Source) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Source</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">file: " << node->get_filename() << "</TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">description</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_description().get()) {
            ss << render(node->get_description());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_description().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_description(const AST::Description::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Description) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Description</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">definitions</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_definitions()) {
            for(const AST::Node::Ptr &n : *node->get_definitions()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        if(node->get_definitions()) {
            int i = 0;
            for(const AST::Node::Ptr &n : *node->get_definitions()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_pragmalist(const AST::Pragmalist::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Pragmalist) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Pragmalist</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">pragmas</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">statements</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_pragmas()) {
            for(const AST::Pragma::Ptr &n : *node->get_pragmas()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        if(node->get_statements()) {
            for(const AST::Node::Ptr &n : *node->get_statements()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        if(node->get_pragmas()) {
            int i = 0;
            for(const AST::Pragma::Ptr &n : *node->get_pragmas()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
        if(node->get_statements()) {
            int i = 0;
            for(const AST::Node::Ptr &n : *node->get_statements()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p2 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_pragma(const AST::Pragma::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Pragma) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Pragma</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: " << node->get_name() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">expression</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_expression().get()) {
            ss << render(node->get_expression());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_expression().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_module(const AST::Module::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Module) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Module</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: " << node->get_name() << "</TD></TR>\n";
        switch(node->get_lifetime()) {
        case Veriparse::AST::Module::LifetimeEnum::NONE:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">lifetime: NONE</TD></TR>\n";
            break;
        case Veriparse::AST::Module::LifetimeEnum::AUTOMATIC:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">lifetime: AUTOMATIC</TD></TR>\n";
            break;
        default:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">lifetime: STATIC</TD></TR>\n";
        }
        switch(node->get_default_nettype()) {
        case Veriparse::AST::Module::Default_nettypeEnum::WIRE:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">default_nettype: WIRE</TD></TR>\n";
            break;
        case Veriparse::AST::Module::Default_nettypeEnum::TRI:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">default_nettype: TRI</TD></TR>\n";
            break;
        case Veriparse::AST::Module::Default_nettypeEnum::TRI0:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">default_nettype: TRI0</TD></TR>\n";
            break;
        case Veriparse::AST::Module::Default_nettypeEnum::TRI1:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">default_nettype: TRI1</TD></TR>\n";
            break;
        case Veriparse::AST::Module::Default_nettypeEnum::TRIAND:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">default_nettype: TRIAND</TD></TR>\n";
            break;
        case Veriparse::AST::Module::Default_nettypeEnum::TRIOR:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">default_nettype: TRIOR</TD></TR>\n";
            break;
        case Veriparse::AST::Module::Default_nettypeEnum::TRIREG:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">default_nettype: TRIREG</TD></TR>\n";
            break;
        case Veriparse::AST::Module::Default_nettypeEnum::WAND:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">default_nettype: WAND</TD></TR>\n";
            break;
        case Veriparse::AST::Module::Default_nettypeEnum::WOR:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">default_nettype: WOR</TD></TR>\n";
            break;
        case Veriparse::AST::Module::Default_nettypeEnum::UWIRE:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">default_nettype: UWIRE</TD></TR>\n";
            break;
        case Veriparse::AST::Module::Default_nettypeEnum::SUPPLY0:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">default_nettype: SUPPLY0</TD></TR>\n";
            break;
        case Veriparse::AST::Module::Default_nettypeEnum::SUPPLY1:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">default_nettype: SUPPLY1</TD></TR>\n";
            break;
        default:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">default_nettype: NONE</TD></TR>\n";
        }
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">params</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">ports</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p3\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">items</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_params()) {
            for(const AST::Declaration::Ptr &n : *node->get_params()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        if(node->get_ports()) {
            for(const AST::Port::Ptr &n : *node->get_ports()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        if(node->get_items()) {
            for(const AST::Node::Ptr &n : *node->get_items()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        if(node->get_params()) {
            int i = 0;
            for(const AST::Declaration::Ptr &n : *node->get_params()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
        if(node->get_ports()) {
            int i = 0;
            for(const AST::Port::Ptr &n : *node->get_ports()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p2 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
        if(node->get_items()) {
            int i = 0;
            for(const AST::Node::Ptr &n : *node->get_items()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p3 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_port(const AST::Port::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Port) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Port</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: " << node->get_name() << "</TD></TR>\n";
        switch(node->get_direction()) {
        case Veriparse::AST::Port::DirectionEnum::NONE:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">direction: NONE</TD></TR>\n";
            break;
        case Veriparse::AST::Port::DirectionEnum::INPUT:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">direction: INPUT</TD></TR>\n";
            break;
        case Veriparse::AST::Port::DirectionEnum::OUTPUT:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">direction: OUTPUT</TD></TR>\n";
            break;
        case Veriparse::AST::Port::DirectionEnum::INOUT:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">direction: INOUT</TD></TR>\n";
            break;
        case Veriparse::AST::Port::DirectionEnum::REF:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">direction: REF</TD></TR>\n";
            break;
        default:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">direction: CONST_REF</TD></TR>\n";
        }
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">decl</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">expr</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_decl().get()) {
            ss << render(node->get_decl());
        }
        if(node->get_expr().get()) {
            ss << render(node->get_expr());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_decl().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_expr().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_package(const AST::Package::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Package) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Package</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: " << node->get_name() << "</TD></TR>\n";
        switch(node->get_lifetime()) {
        case Veriparse::AST::Package::LifetimeEnum::NONE:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">lifetime: NONE</TD></TR>\n";
            break;
        case Veriparse::AST::Package::LifetimeEnum::AUTOMATIC:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">lifetime: AUTOMATIC</TD></TR>\n";
            break;
        default:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">lifetime: STATIC</TD></TR>\n";
        }
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">items</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_items()) {
            for(const AST::Node::Ptr &n : *node->get_items()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        if(node->get_items()) {
            int i = 0;
            for(const AST::Node::Ptr &n : *node->get_items()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_import(const AST::Import::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Import) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Import</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">package: " << node->get_package()
           << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">symbol: " << node->get_symbol() << "</TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
    }

    return ss.str();
}

std::string DotGenerator::render_identifier(const AST::Identifier::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Identifier) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Identifier</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: " << node->get_name() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">scope</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">hier</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_scope()) {
            for(const AST::ScopeName::Ptr &n : *node->get_scope()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        if(node->get_hier().get()) {
            ss << render(node->get_hier());
        }
        uint64_t childID;
        if(node->get_scope()) {
            int i = 0;
            for(const AST::ScopeName::Ptr &n : *node->get_scope()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
        childID = reinterpret_cast<uint64_t>(node->get_hier().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_constant(const AST::Constant::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Constant) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Constant</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
    }

    return ss.str();
}

std::string DotGenerator::render_stringconst(const AST::StringConst::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::StringConst) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">StringConst</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">value: " << node->get_value() << "</TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
    }

    return ss.str();
}

std::string DotGenerator::render_intconst(const AST::IntConst::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::IntConst) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">IntConst</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">value: " << node->get_value() << "</TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
    }

    return ss.str();
}

std::string DotGenerator::render_intconstn(const AST::IntConstN::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::IntConstN) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">IntConstN</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">base: " << node->get_base() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">size: " << node->get_size() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">sign: " << node->get_sign() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">value: " << node->get_value() << "</TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
    }

    return ss.str();
}

std::string DotGenerator::render_floatconst(const AST::FloatConst::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::FloatConst) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">FloatConst</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">value: " << node->get_value() << "</TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
    }

    return ss.str();
}

std::string DotGenerator::render_datatype(const AST::DataType::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::DataType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">DataType</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        switch(node->get_signing()) {
        case Veriparse::AST::DataType::SigningEnum::NONE:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: NONE</TD></TR>\n";
            break;
        case Veriparse::AST::DataType::SigningEnum::SIGNED:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: SIGNED</TD></TR>\n";
            break;
        default:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: UNSIGNED</TD></TR>\n";
        }
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">packed_dims</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_packed_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        if(node->get_packed_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_logictype(const AST::LogicType::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::LogicType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">LogicType</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        switch(node->get_signing()) {
        case Veriparse::AST::LogicType::DataType::SigningEnum::NONE:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: NONE</TD></TR>\n";
            break;
        case Veriparse::AST::LogicType::DataType::SigningEnum::SIGNED:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: SIGNED</TD></TR>\n";
            break;
        default:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: UNSIGNED</TD></TR>\n";
        }
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">packed_dims</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_packed_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        if(node->get_packed_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_regtype(const AST::RegType::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::RegType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">RegType</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        switch(node->get_signing()) {
        case Veriparse::AST::RegType::DataType::SigningEnum::NONE:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: NONE</TD></TR>\n";
            break;
        case Veriparse::AST::RegType::DataType::SigningEnum::SIGNED:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: SIGNED</TD></TR>\n";
            break;
        default:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: UNSIGNED</TD></TR>\n";
        }
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">packed_dims</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_packed_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        if(node->get_packed_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_bittype(const AST::BitType::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::BitType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">BitType</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        switch(node->get_signing()) {
        case Veriparse::AST::BitType::DataType::SigningEnum::NONE:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: NONE</TD></TR>\n";
            break;
        case Veriparse::AST::BitType::DataType::SigningEnum::SIGNED:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: SIGNED</TD></TR>\n";
            break;
        default:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: UNSIGNED</TD></TR>\n";
        }
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">packed_dims</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_packed_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        if(node->get_packed_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_bytetype(const AST::ByteType::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::ByteType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">ByteType</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        switch(node->get_signing()) {
        case Veriparse::AST::ByteType::DataType::SigningEnum::NONE:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: NONE</TD></TR>\n";
            break;
        case Veriparse::AST::ByteType::DataType::SigningEnum::SIGNED:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: SIGNED</TD></TR>\n";
            break;
        default:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: UNSIGNED</TD></TR>\n";
        }
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">packed_dims</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_packed_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        if(node->get_packed_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_shortinttype(const AST::ShortintType::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::ShortintType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">ShortintType</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        switch(node->get_signing()) {
        case Veriparse::AST::ShortintType::DataType::SigningEnum::NONE:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: NONE</TD></TR>\n";
            break;
        case Veriparse::AST::ShortintType::DataType::SigningEnum::SIGNED:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: SIGNED</TD></TR>\n";
            break;
        default:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: UNSIGNED</TD></TR>\n";
        }
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">packed_dims</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_packed_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        if(node->get_packed_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_inttype(const AST::IntType::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::IntType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">IntType</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        switch(node->get_signing()) {
        case Veriparse::AST::IntType::DataType::SigningEnum::NONE:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: NONE</TD></TR>\n";
            break;
        case Veriparse::AST::IntType::DataType::SigningEnum::SIGNED:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: SIGNED</TD></TR>\n";
            break;
        default:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: UNSIGNED</TD></TR>\n";
        }
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">packed_dims</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_packed_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        if(node->get_packed_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_longinttype(const AST::LongintType::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::LongintType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">LongintType</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        switch(node->get_signing()) {
        case Veriparse::AST::LongintType::DataType::SigningEnum::NONE:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: NONE</TD></TR>\n";
            break;
        case Veriparse::AST::LongintType::DataType::SigningEnum::SIGNED:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: SIGNED</TD></TR>\n";
            break;
        default:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: UNSIGNED</TD></TR>\n";
        }
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">packed_dims</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_packed_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        if(node->get_packed_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_integertype(const AST::IntegerType::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::IntegerType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">IntegerType</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        switch(node->get_signing()) {
        case Veriparse::AST::IntegerType::DataType::SigningEnum::NONE:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: NONE</TD></TR>\n";
            break;
        case Veriparse::AST::IntegerType::DataType::SigningEnum::SIGNED:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: SIGNED</TD></TR>\n";
            break;
        default:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: UNSIGNED</TD></TR>\n";
        }
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">packed_dims</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_packed_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        if(node->get_packed_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_timetype(const AST::TimeType::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::TimeType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">TimeType</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        switch(node->get_signing()) {
        case Veriparse::AST::TimeType::DataType::SigningEnum::NONE:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: NONE</TD></TR>\n";
            break;
        case Veriparse::AST::TimeType::DataType::SigningEnum::SIGNED:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: SIGNED</TD></TR>\n";
            break;
        default:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: UNSIGNED</TD></TR>\n";
        }
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">packed_dims</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_packed_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        if(node->get_packed_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_realtype(const AST::RealType::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::RealType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">RealType</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        switch(node->get_signing()) {
        case Veriparse::AST::RealType::DataType::SigningEnum::NONE:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: NONE</TD></TR>\n";
            break;
        case Veriparse::AST::RealType::DataType::SigningEnum::SIGNED:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: SIGNED</TD></TR>\n";
            break;
        default:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: UNSIGNED</TD></TR>\n";
        }
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">packed_dims</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_packed_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        if(node->get_packed_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_shortrealtype(const AST::ShortrealType::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::ShortrealType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">ShortrealType</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        switch(node->get_signing()) {
        case Veriparse::AST::ShortrealType::DataType::SigningEnum::NONE:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: NONE</TD></TR>\n";
            break;
        case Veriparse::AST::ShortrealType::DataType::SigningEnum::SIGNED:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: SIGNED</TD></TR>\n";
            break;
        default:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: UNSIGNED</TD></TR>\n";
        }
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">packed_dims</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_packed_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        if(node->get_packed_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_realtimetype(const AST::RealtimeType::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::RealtimeType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">RealtimeType</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        switch(node->get_signing()) {
        case Veriparse::AST::RealtimeType::DataType::SigningEnum::NONE:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: NONE</TD></TR>\n";
            break;
        case Veriparse::AST::RealtimeType::DataType::SigningEnum::SIGNED:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: SIGNED</TD></TR>\n";
            break;
        default:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: UNSIGNED</TD></TR>\n";
        }
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">packed_dims</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_packed_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        if(node->get_packed_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_stringtype(const AST::StringType::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::StringType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">StringType</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        switch(node->get_signing()) {
        case Veriparse::AST::StringType::DataType::SigningEnum::NONE:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: NONE</TD></TR>\n";
            break;
        case Veriparse::AST::StringType::DataType::SigningEnum::SIGNED:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: SIGNED</TD></TR>\n";
            break;
        default:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: UNSIGNED</TD></TR>\n";
        }
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">packed_dims</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_packed_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        if(node->get_packed_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_chandletype(const AST::ChandleType::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::ChandleType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">ChandleType</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        switch(node->get_signing()) {
        case Veriparse::AST::ChandleType::DataType::SigningEnum::NONE:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: NONE</TD></TR>\n";
            break;
        case Veriparse::AST::ChandleType::DataType::SigningEnum::SIGNED:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: SIGNED</TD></TR>\n";
            break;
        default:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: UNSIGNED</TD></TR>\n";
        }
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">packed_dims</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_packed_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        if(node->get_packed_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_eventtype(const AST::EventType::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::EventType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">EventType</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        switch(node->get_signing()) {
        case Veriparse::AST::EventType::DataType::SigningEnum::NONE:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: NONE</TD></TR>\n";
            break;
        case Veriparse::AST::EventType::DataType::SigningEnum::SIGNED:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: SIGNED</TD></TR>\n";
            break;
        default:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: UNSIGNED</TD></TR>\n";
        }
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">packed_dims</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_packed_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        if(node->get_packed_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_implicittype(const AST::ImplicitType::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::ImplicitType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">ImplicitType</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        switch(node->get_signing()) {
        case Veriparse::AST::ImplicitType::DataType::SigningEnum::NONE:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: NONE</TD></TR>\n";
            break;
        case Veriparse::AST::ImplicitType::DataType::SigningEnum::SIGNED:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: SIGNED</TD></TR>\n";
            break;
        default:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: UNSIGNED</TD></TR>\n";
        }
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">packed_dims</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_packed_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        if(node->get_packed_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_voidtype(const AST::VoidType::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::VoidType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">VoidType</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        switch(node->get_signing()) {
        case Veriparse::AST::VoidType::DataType::SigningEnum::NONE:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: NONE</TD></TR>\n";
            break;
        case Veriparse::AST::VoidType::DataType::SigningEnum::SIGNED:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: SIGNED</TD></TR>\n";
            break;
        default:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: UNSIGNED</TD></TR>\n";
        }
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">packed_dims</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_packed_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        if(node->get_packed_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_namedtype(const AST::NamedType::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::NamedType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">NamedType</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: " << node->get_name() << "</TD></TR>\n";
        switch(node->get_signing()) {
        case Veriparse::AST::NamedType::DataType::SigningEnum::NONE:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: NONE</TD></TR>\n";
            break;
        case Veriparse::AST::NamedType::DataType::SigningEnum::SIGNED:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: SIGNED</TD></TR>\n";
            break;
        default:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: UNSIGNED</TD></TR>\n";
        }
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">scope</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">packed_dims</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_scope()) {
            for(const AST::ScopeName::Ptr &n : *node->get_scope()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        if(node->get_packed_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        if(node->get_scope()) {
            int i = 0;
            for(const AST::ScopeName::Ptr &n : *node->get_scope()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
        if(node->get_packed_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p2 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_scopename(const AST::ScopeName::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::ScopeName) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">ScopeName</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: " << node->get_name() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">params</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_params()) {
            for(const AST::ParamArg::Ptr &n : *node->get_params()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        if(node->get_params()) {
            int i = 0;
            for(const AST::ParamArg::Ptr &n : *node->get_params()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_structtype(const AST::StructType::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::StructType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">StructType</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">is_packed: " << node->get_is_packed()
           << "</TD></TR>\n";
        switch(node->get_signing()) {
        case Veriparse::AST::StructType::DataType::SigningEnum::NONE:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: NONE</TD></TR>\n";
            break;
        case Veriparse::AST::StructType::DataType::SigningEnum::SIGNED:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: SIGNED</TD></TR>\n";
            break;
        default:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: UNSIGNED</TD></TR>\n";
        }
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">members</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">packed_dims</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_members()) {
            for(const AST::Member::Ptr &n : *node->get_members()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        if(node->get_packed_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        if(node->get_members()) {
            int i = 0;
            for(const AST::Member::Ptr &n : *node->get_members()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
        if(node->get_packed_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p2 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_uniontype(const AST::UnionType::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::UnionType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">UnionType</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">is_packed: " << node->get_is_packed()
           << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">is_tagged: " << node->get_is_tagged()
           << "</TD></TR>\n";
        switch(node->get_signing()) {
        case Veriparse::AST::UnionType::DataType::SigningEnum::NONE:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: NONE</TD></TR>\n";
            break;
        case Veriparse::AST::UnionType::DataType::SigningEnum::SIGNED:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: SIGNED</TD></TR>\n";
            break;
        default:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: UNSIGNED</TD></TR>\n";
        }
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">members</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">packed_dims</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_members()) {
            for(const AST::Member::Ptr &n : *node->get_members()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        if(node->get_packed_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        if(node->get_members()) {
            int i = 0;
            for(const AST::Member::Ptr &n : *node->get_members()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
        if(node->get_packed_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p2 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_enumtype(const AST::EnumType::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::EnumType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">EnumType</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        switch(node->get_signing()) {
        case Veriparse::AST::EnumType::DataType::SigningEnum::NONE:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: NONE</TD></TR>\n";
            break;
        case Veriparse::AST::EnumType::DataType::SigningEnum::SIGNED:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: SIGNED</TD></TR>\n";
            break;
        default:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: UNSIGNED</TD></TR>\n";
        }
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">base</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">items</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p3\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">packed_dims</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_base().get()) {
            ss << render(node->get_base());
        }
        if(node->get_items()) {
            for(const AST::EnumItem::Ptr &n : *node->get_items()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        if(node->get_packed_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_base().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        if(node->get_items()) {
            int i = 0;
            for(const AST::EnumItem::Ptr &n : *node->get_items()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p2 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
        if(node->get_packed_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p3 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_enumitem(const AST::EnumItem::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::EnumItem) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">EnumItem</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: " << node->get_name() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">value</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">range</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_value().get()) {
            ss << render(node->get_value());
        }
        if(node->get_range().get()) {
            ss << render(node->get_range());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_value().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_range().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_typeopexpr(const AST::TypeOpExpr::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::TypeOpExpr) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">TypeOpExpr</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        switch(node->get_signing()) {
        case Veriparse::AST::TypeOpExpr::DataType::SigningEnum::NONE:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: NONE</TD></TR>\n";
            break;
        case Veriparse::AST::TypeOpExpr::DataType::SigningEnum::SIGNED:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: SIGNED</TD></TR>\n";
            break;
        default:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: UNSIGNED</TD></TR>\n";
        }
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">expr</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">packed_dims</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_expr().get()) {
            ss << render(node->get_expr());
        }
        if(node->get_packed_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_expr().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        if(node->get_packed_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p2 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_typeoptype(const AST::TypeOpType::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::TypeOpType) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">TypeOpType</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        switch(node->get_signing()) {
        case Veriparse::AST::TypeOpType::DataType::SigningEnum::NONE:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: NONE</TD></TR>\n";
            break;
        case Veriparse::AST::TypeOpType::DataType::SigningEnum::SIGNED:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: SIGNED</TD></TR>\n";
            break;
        default:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: UNSIGNED</TD></TR>\n";
        }
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">arg_type</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">packed_dims</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_arg_type().get()) {
            ss << render(node->get_arg_type());
        }
        if(node->get_packed_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_arg_type().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        if(node->get_packed_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_packed_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p2 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_dimension(const AST::Dimension::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Dimension) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Dimension</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
    }

    return ss.str();
}

std::string DotGenerator::render_rangedim(const AST::RangeDim::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::RangeDim) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">RangeDim</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">left</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_left().get()) {
            ss << render(node->get_left());
        }
        if(node->get_right().get()) {
            ss << render(node->get_right());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_left().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_right().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_sizedim(const AST::SizeDim::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::SizeDim) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">SizeDim</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">size</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_size().get()) {
            ss << render(node->get_size());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_size().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_unsizeddim(const AST::UnsizedDim::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::UnsizedDim) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">UnsizedDim</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
    }

    return ss.str();
}

std::string DotGenerator::render_queuedim(const AST::QueueDim::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::QueueDim) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">QueueDim</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">bound</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_bound().get()) {
            ss << render(node->get_bound());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_bound().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_assocdim(const AST::AssocDim::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::AssocDim) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">AssocDim</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">index_type</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_index_type().get()) {
            ss << render(node->get_index_type());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_index_type().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_declaration(const AST::Declaration::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Declaration) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Declaration</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: " << node->get_name() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">type</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_type().get()) {
            ss << render(node->get_type());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_type().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_var(const AST::Var::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Var) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Var</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">is_var: " << node->get_is_var() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">is_const: " << node->get_is_const()
           << "</TD></TR>\n";
        switch(node->get_lifetime()) {
        case Veriparse::AST::Var::LifetimeEnum::NONE:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">lifetime: NONE</TD></TR>\n";
            break;
        case Veriparse::AST::Var::LifetimeEnum::AUTOMATIC:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">lifetime: AUTOMATIC</TD></TR>\n";
            break;
        default:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">lifetime: STATIC</TD></TR>\n";
        }
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: " << node->get_name() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">unpacked_dims</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">init</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p3\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">type</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_unpacked_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        if(node->get_init().get()) {
            ss << render(node->get_init());
        }
        if(node->get_type().get()) {
            ss << render(node->get_type());
        }
        uint64_t childID;
        if(node->get_unpacked_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
        childID = reinterpret_cast<uint64_t>(node->get_init().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_type().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_net(const AST::Net::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Net) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Net</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">is_vectored: " << node->get_is_vectored()
           << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">is_scalared: " << node->get_is_scalared()
           << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: " << node->get_name() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">unpacked_dims</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">cont_assign</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p3\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">strength</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p4\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">ldelay</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p5\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">rdelay</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p6\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">type</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_unpacked_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        if(node->get_cont_assign().get()) {
            ss << render(node->get_cont_assign());
        }
        if(node->get_strength().get()) {
            ss << render(node->get_strength());
        }
        if(node->get_ldelay().get()) {
            ss << render(node->get_ldelay());
        }
        if(node->get_rdelay().get()) {
            ss << render(node->get_rdelay());
        }
        if(node->get_type().get()) {
            ss << render(node->get_type());
        }
        uint64_t childID;
        if(node->get_unpacked_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
        childID = reinterpret_cast<uint64_t>(node->get_cont_assign().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_strength().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_ldelay().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p4 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_rdelay().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p5 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_type().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p6 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_wirenet(const AST::WireNet::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::WireNet) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">WireNet</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">is_vectored: " << node->get_is_vectored()
           << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">is_scalared: " << node->get_is_scalared()
           << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: " << node->get_name() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">unpacked_dims</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">cont_assign</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p3\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">strength</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p4\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">ldelay</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p5\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">rdelay</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p6\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">type</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_unpacked_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        if(node->get_cont_assign().get()) {
            ss << render(node->get_cont_assign());
        }
        if(node->get_strength().get()) {
            ss << render(node->get_strength());
        }
        if(node->get_ldelay().get()) {
            ss << render(node->get_ldelay());
        }
        if(node->get_rdelay().get()) {
            ss << render(node->get_rdelay());
        }
        if(node->get_type().get()) {
            ss << render(node->get_type());
        }
        uint64_t childID;
        if(node->get_unpacked_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
        childID = reinterpret_cast<uint64_t>(node->get_cont_assign().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_strength().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_ldelay().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p4 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_rdelay().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p5 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_type().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p6 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_trinet(const AST::TriNet::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::TriNet) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">TriNet</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">is_vectored: " << node->get_is_vectored()
           << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">is_scalared: " << node->get_is_scalared()
           << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: " << node->get_name() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">unpacked_dims</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">cont_assign</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p3\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">strength</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p4\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">ldelay</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p5\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">rdelay</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p6\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">type</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_unpacked_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        if(node->get_cont_assign().get()) {
            ss << render(node->get_cont_assign());
        }
        if(node->get_strength().get()) {
            ss << render(node->get_strength());
        }
        if(node->get_ldelay().get()) {
            ss << render(node->get_ldelay());
        }
        if(node->get_rdelay().get()) {
            ss << render(node->get_rdelay());
        }
        if(node->get_type().get()) {
            ss << render(node->get_type());
        }
        uint64_t childID;
        if(node->get_unpacked_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
        childID = reinterpret_cast<uint64_t>(node->get_cont_assign().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_strength().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_ldelay().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p4 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_rdelay().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p5 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_type().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p6 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_tri0net(const AST::Tri0Net::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Tri0Net) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Tri0Net</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">is_vectored: " << node->get_is_vectored()
           << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">is_scalared: " << node->get_is_scalared()
           << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: " << node->get_name() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">unpacked_dims</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">cont_assign</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p3\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">strength</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p4\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">ldelay</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p5\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">rdelay</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p6\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">type</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_unpacked_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        if(node->get_cont_assign().get()) {
            ss << render(node->get_cont_assign());
        }
        if(node->get_strength().get()) {
            ss << render(node->get_strength());
        }
        if(node->get_ldelay().get()) {
            ss << render(node->get_ldelay());
        }
        if(node->get_rdelay().get()) {
            ss << render(node->get_rdelay());
        }
        if(node->get_type().get()) {
            ss << render(node->get_type());
        }
        uint64_t childID;
        if(node->get_unpacked_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
        childID = reinterpret_cast<uint64_t>(node->get_cont_assign().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_strength().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_ldelay().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p4 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_rdelay().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p5 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_type().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p6 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_tri1net(const AST::Tri1Net::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Tri1Net) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Tri1Net</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">is_vectored: " << node->get_is_vectored()
           << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">is_scalared: " << node->get_is_scalared()
           << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: " << node->get_name() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">unpacked_dims</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">cont_assign</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p3\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">strength</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p4\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">ldelay</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p5\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">rdelay</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p6\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">type</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_unpacked_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        if(node->get_cont_assign().get()) {
            ss << render(node->get_cont_assign());
        }
        if(node->get_strength().get()) {
            ss << render(node->get_strength());
        }
        if(node->get_ldelay().get()) {
            ss << render(node->get_ldelay());
        }
        if(node->get_rdelay().get()) {
            ss << render(node->get_rdelay());
        }
        if(node->get_type().get()) {
            ss << render(node->get_type());
        }
        uint64_t childID;
        if(node->get_unpacked_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
        childID = reinterpret_cast<uint64_t>(node->get_cont_assign().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_strength().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_ldelay().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p4 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_rdelay().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p5 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_type().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p6 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_triandnet(const AST::TriandNet::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::TriandNet) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">TriandNet</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">is_vectored: " << node->get_is_vectored()
           << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">is_scalared: " << node->get_is_scalared()
           << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: " << node->get_name() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">unpacked_dims</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">cont_assign</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p3\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">strength</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p4\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">ldelay</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p5\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">rdelay</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p6\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">type</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_unpacked_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        if(node->get_cont_assign().get()) {
            ss << render(node->get_cont_assign());
        }
        if(node->get_strength().get()) {
            ss << render(node->get_strength());
        }
        if(node->get_ldelay().get()) {
            ss << render(node->get_ldelay());
        }
        if(node->get_rdelay().get()) {
            ss << render(node->get_rdelay());
        }
        if(node->get_type().get()) {
            ss << render(node->get_type());
        }
        uint64_t childID;
        if(node->get_unpacked_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
        childID = reinterpret_cast<uint64_t>(node->get_cont_assign().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_strength().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_ldelay().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p4 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_rdelay().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p5 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_type().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p6 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_triornet(const AST::TriorNet::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::TriorNet) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">TriorNet</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">is_vectored: " << node->get_is_vectored()
           << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">is_scalared: " << node->get_is_scalared()
           << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: " << node->get_name() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">unpacked_dims</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">cont_assign</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p3\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">strength</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p4\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">ldelay</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p5\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">rdelay</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p6\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">type</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_unpacked_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        if(node->get_cont_assign().get()) {
            ss << render(node->get_cont_assign());
        }
        if(node->get_strength().get()) {
            ss << render(node->get_strength());
        }
        if(node->get_ldelay().get()) {
            ss << render(node->get_ldelay());
        }
        if(node->get_rdelay().get()) {
            ss << render(node->get_rdelay());
        }
        if(node->get_type().get()) {
            ss << render(node->get_type());
        }
        uint64_t childID;
        if(node->get_unpacked_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
        childID = reinterpret_cast<uint64_t>(node->get_cont_assign().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_strength().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_ldelay().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p4 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_rdelay().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p5 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_type().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p6 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_triregnet(const AST::TriregNet::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::TriregNet) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">TriregNet</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">is_vectored: " << node->get_is_vectored()
           << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">is_scalared: " << node->get_is_scalared()
           << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: " << node->get_name() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">unpacked_dims</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">cont_assign</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p3\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">strength</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p4\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">ldelay</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p5\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">rdelay</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p6\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">type</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_unpacked_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        if(node->get_cont_assign().get()) {
            ss << render(node->get_cont_assign());
        }
        if(node->get_strength().get()) {
            ss << render(node->get_strength());
        }
        if(node->get_ldelay().get()) {
            ss << render(node->get_ldelay());
        }
        if(node->get_rdelay().get()) {
            ss << render(node->get_rdelay());
        }
        if(node->get_type().get()) {
            ss << render(node->get_type());
        }
        uint64_t childID;
        if(node->get_unpacked_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
        childID = reinterpret_cast<uint64_t>(node->get_cont_assign().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_strength().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_ldelay().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p4 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_rdelay().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p5 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_type().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p6 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_wandnet(const AST::WandNet::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::WandNet) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">WandNet</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">is_vectored: " << node->get_is_vectored()
           << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">is_scalared: " << node->get_is_scalared()
           << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: " << node->get_name() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">unpacked_dims</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">cont_assign</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p3\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">strength</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p4\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">ldelay</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p5\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">rdelay</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p6\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">type</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_unpacked_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        if(node->get_cont_assign().get()) {
            ss << render(node->get_cont_assign());
        }
        if(node->get_strength().get()) {
            ss << render(node->get_strength());
        }
        if(node->get_ldelay().get()) {
            ss << render(node->get_ldelay());
        }
        if(node->get_rdelay().get()) {
            ss << render(node->get_rdelay());
        }
        if(node->get_type().get()) {
            ss << render(node->get_type());
        }
        uint64_t childID;
        if(node->get_unpacked_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
        childID = reinterpret_cast<uint64_t>(node->get_cont_assign().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_strength().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_ldelay().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p4 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_rdelay().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p5 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_type().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p6 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_wornet(const AST::WorNet::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::WorNet) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">WorNet</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">is_vectored: " << node->get_is_vectored()
           << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">is_scalared: " << node->get_is_scalared()
           << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: " << node->get_name() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">unpacked_dims</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">cont_assign</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p3\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">strength</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p4\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">ldelay</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p5\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">rdelay</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p6\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">type</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_unpacked_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        if(node->get_cont_assign().get()) {
            ss << render(node->get_cont_assign());
        }
        if(node->get_strength().get()) {
            ss << render(node->get_strength());
        }
        if(node->get_ldelay().get()) {
            ss << render(node->get_ldelay());
        }
        if(node->get_rdelay().get()) {
            ss << render(node->get_rdelay());
        }
        if(node->get_type().get()) {
            ss << render(node->get_type());
        }
        uint64_t childID;
        if(node->get_unpacked_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
        childID = reinterpret_cast<uint64_t>(node->get_cont_assign().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_strength().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_ldelay().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p4 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_rdelay().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p5 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_type().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p6 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_uwirenet(const AST::UwireNet::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::UwireNet) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">UwireNet</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">is_vectored: " << node->get_is_vectored()
           << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">is_scalared: " << node->get_is_scalared()
           << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: " << node->get_name() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">unpacked_dims</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">cont_assign</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p3\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">strength</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p4\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">ldelay</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p5\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">rdelay</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p6\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">type</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_unpacked_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        if(node->get_cont_assign().get()) {
            ss << render(node->get_cont_assign());
        }
        if(node->get_strength().get()) {
            ss << render(node->get_strength());
        }
        if(node->get_ldelay().get()) {
            ss << render(node->get_ldelay());
        }
        if(node->get_rdelay().get()) {
            ss << render(node->get_rdelay());
        }
        if(node->get_type().get()) {
            ss << render(node->get_type());
        }
        uint64_t childID;
        if(node->get_unpacked_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
        childID = reinterpret_cast<uint64_t>(node->get_cont_assign().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_strength().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_ldelay().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p4 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_rdelay().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p5 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_type().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p6 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_supply0net(const AST::Supply0Net::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Supply0Net) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Supply0Net</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">is_vectored: " << node->get_is_vectored()
           << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">is_scalared: " << node->get_is_scalared()
           << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: " << node->get_name() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">unpacked_dims</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">cont_assign</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p3\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">strength</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p4\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">ldelay</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p5\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">rdelay</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p6\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">type</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_unpacked_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        if(node->get_cont_assign().get()) {
            ss << render(node->get_cont_assign());
        }
        if(node->get_strength().get()) {
            ss << render(node->get_strength());
        }
        if(node->get_ldelay().get()) {
            ss << render(node->get_ldelay());
        }
        if(node->get_rdelay().get()) {
            ss << render(node->get_rdelay());
        }
        if(node->get_type().get()) {
            ss << render(node->get_type());
        }
        uint64_t childID;
        if(node->get_unpacked_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
        childID = reinterpret_cast<uint64_t>(node->get_cont_assign().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_strength().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_ldelay().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p4 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_rdelay().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p5 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_type().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p6 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_supply1net(const AST::Supply1Net::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Supply1Net) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Supply1Net</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">is_vectored: " << node->get_is_vectored()
           << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">is_scalared: " << node->get_is_scalared()
           << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: " << node->get_name() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">unpacked_dims</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">cont_assign</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p3\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">strength</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p4\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">ldelay</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p5\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">rdelay</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p6\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">type</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_unpacked_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        if(node->get_cont_assign().get()) {
            ss << render(node->get_cont_assign());
        }
        if(node->get_strength().get()) {
            ss << render(node->get_strength());
        }
        if(node->get_ldelay().get()) {
            ss << render(node->get_ldelay());
        }
        if(node->get_rdelay().get()) {
            ss << render(node->get_rdelay());
        }
        if(node->get_type().get()) {
            ss << render(node->get_type());
        }
        uint64_t childID;
        if(node->get_unpacked_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
        childID = reinterpret_cast<uint64_t>(node->get_cont_assign().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_strength().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_ldelay().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p4 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_rdelay().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p5 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_type().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p6 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_interconnectnet(const AST::InterconnectNet::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::InterconnectNet) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">InterconnectNet</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">is_vectored: " << node->get_is_vectored()
           << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">is_scalared: " << node->get_is_scalared()
           << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: " << node->get_name() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">unpacked_dims</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">cont_assign</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p3\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">strength</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p4\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">ldelay</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p5\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">rdelay</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p6\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">type</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_unpacked_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        if(node->get_cont_assign().get()) {
            ss << render(node->get_cont_assign());
        }
        if(node->get_strength().get()) {
            ss << render(node->get_strength());
        }
        if(node->get_ldelay().get()) {
            ss << render(node->get_ldelay());
        }
        if(node->get_rdelay().get()) {
            ss << render(node->get_rdelay());
        }
        if(node->get_type().get()) {
            ss << render(node->get_type());
        }
        uint64_t childID;
        if(node->get_unpacked_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
        childID = reinterpret_cast<uint64_t>(node->get_cont_assign().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_strength().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_ldelay().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p4 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_rdelay().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p5 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_type().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p6 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_usernet(const AST::UserNet::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::UserNet) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">UserNet</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">is_vectored: " << node->get_is_vectored()
           << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">is_scalared: " << node->get_is_scalared()
           << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: " << node->get_name() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">unpacked_dims</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">cont_assign</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p3\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">strength</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p4\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">ldelay</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p5\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">rdelay</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p6\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">type</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_unpacked_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        if(node->get_cont_assign().get()) {
            ss << render(node->get_cont_assign());
        }
        if(node->get_strength().get()) {
            ss << render(node->get_strength());
        }
        if(node->get_ldelay().get()) {
            ss << render(node->get_ldelay());
        }
        if(node->get_rdelay().get()) {
            ss << render(node->get_rdelay());
        }
        if(node->get_type().get()) {
            ss << render(node->get_type());
        }
        uint64_t childID;
        if(node->get_unpacked_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
        childID = reinterpret_cast<uint64_t>(node->get_cont_assign().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_strength().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_ldelay().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p4 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_rdelay().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p5 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_type().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p6 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_implicitnet(const AST::ImplicitNet::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::ImplicitNet) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">ImplicitNet</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">is_vectored: " << node->get_is_vectored()
           << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">is_scalared: " << node->get_is_scalared()
           << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: " << node->get_name() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">unpacked_dims</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">cont_assign</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p3\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">strength</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p4\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">ldelay</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p5\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">rdelay</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p6\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">type</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_unpacked_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        if(node->get_cont_assign().get()) {
            ss << render(node->get_cont_assign());
        }
        if(node->get_strength().get()) {
            ss << render(node->get_strength());
        }
        if(node->get_ldelay().get()) {
            ss << render(node->get_ldelay());
        }
        if(node->get_rdelay().get()) {
            ss << render(node->get_rdelay());
        }
        if(node->get_type().get()) {
            ss << render(node->get_type());
        }
        uint64_t childID;
        if(node->get_unpacked_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
        childID = reinterpret_cast<uint64_t>(node->get_cont_assign().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_strength().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_ldelay().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p4 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_rdelay().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p5 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_type().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p6 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_nettypedecl(const AST::NetTypeDecl::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::NetTypeDecl) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">NetTypeDecl</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: " << node->get_name() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">resolver</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">type</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_resolver().get()) {
            ss << render(node->get_resolver());
        }
        if(node->get_type().get()) {
            ss << render(node->get_type());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_resolver().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_type().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_strength(const AST::Strength::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Strength) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Strength</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
    }

    return ss.str();
}

std::string DotGenerator::render_drivestrength(const AST::DriveStrength::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::DriveStrength) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">DriveStrength</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        switch(node->get_s0()) {
        case Veriparse::AST::DriveStrength::S0Enum::SUPPLY0:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">s0: SUPPLY0</TD></TR>\n";
            break;
        case Veriparse::AST::DriveStrength::S0Enum::STRONG0:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">s0: STRONG0</TD></TR>\n";
            break;
        case Veriparse::AST::DriveStrength::S0Enum::PULL0:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">s0: PULL0</TD></TR>\n";
            break;
        case Veriparse::AST::DriveStrength::S0Enum::WEAK0:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">s0: WEAK0</TD></TR>\n";
            break;
        default:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">s0: HIGHZ0</TD></TR>\n";
        }
        switch(node->get_s1()) {
        case Veriparse::AST::DriveStrength::S1Enum::SUPPLY1:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">s1: SUPPLY1</TD></TR>\n";
            break;
        case Veriparse::AST::DriveStrength::S1Enum::STRONG1:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">s1: STRONG1</TD></TR>\n";
            break;
        case Veriparse::AST::DriveStrength::S1Enum::PULL1:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">s1: PULL1</TD></TR>\n";
            break;
        case Veriparse::AST::DriveStrength::S1Enum::WEAK1:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">s1: WEAK1</TD></TR>\n";
            break;
        default:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">s1: HIGHZ1</TD></TR>\n";
        }

        ss << "\t\t</TABLE>>];" << std::endl;
    }

    return ss.str();
}

std::string DotGenerator::render_chargestrength(const AST::ChargeStrength::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::ChargeStrength) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">ChargeStrength</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        switch(node->get_charge()) {
        case Veriparse::AST::ChargeStrength::ChargeEnum::SMALL:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">charge: SMALL</TD></TR>\n";
            break;
        case Veriparse::AST::ChargeStrength::ChargeEnum::MEDIUM:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">charge: MEDIUM</TD></TR>\n";
            break;
        default:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">charge: LARGE</TD></TR>\n";
        }

        ss << "\t\t</TABLE>>];" << std::endl;
    }

    return ss.str();
}

std::string DotGenerator::render_param(const AST::Param::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Param) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Param</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">is_local: " << node->get_is_local()
           << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: " << node->get_name() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">value</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">unpacked_dims</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p3\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">type</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_value().get()) {
            ss << render(node->get_value());
        }
        if(node->get_unpacked_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        if(node->get_type().get()) {
            ss << render(node->get_type());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_value().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        if(node->get_unpacked_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p2 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
        childID = reinterpret_cast<uint64_t>(node->get_type().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_typeparam(const AST::TypeParam::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::TypeParam) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">TypeParam</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">is_local: " << node->get_is_local()
           << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: " << node->get_name() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">type</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_type().get()) {
            ss << render(node->get_type());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_type().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_typedef(const AST::Typedef::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Typedef) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Typedef</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        switch(node->get_fwd_kind()) {
        case Veriparse::AST::Typedef::Fwd_kindEnum::NONE:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">fwd_kind: NONE</TD></TR>\n";
            break;
        case Veriparse::AST::Typedef::Fwd_kindEnum::ENUM:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">fwd_kind: ENUM</TD></TR>\n";
            break;
        case Veriparse::AST::Typedef::Fwd_kindEnum::STRUCT:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">fwd_kind: STRUCT</TD></TR>\n";
            break;
        case Veriparse::AST::Typedef::Fwd_kindEnum::UNION:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">fwd_kind: UNION</TD></TR>\n";
            break;
        case Veriparse::AST::Typedef::Fwd_kindEnum::CLASS:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">fwd_kind: CLASS</TD></TR>\n";
            break;
        default:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">fwd_kind: INTERFACE_CLASS</TD></TR>\n";
        }
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: " << node->get_name() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">unpacked_dims</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">type</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_unpacked_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        if(node->get_type().get()) {
            ss << render(node->get_type());
        }
        uint64_t childID;
        if(node->get_unpacked_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
        childID = reinterpret_cast<uint64_t>(node->get_type().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_member(const AST::Member::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Member) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Member</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: " << node->get_name() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">unpacked_dims</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">init</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p3\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">type</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_unpacked_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        if(node->get_init().get()) {
            ss << render(node->get_init());
        }
        if(node->get_type().get()) {
            ss << render(node->get_type());
        }
        uint64_t childID;
        if(node->get_unpacked_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
        childID = reinterpret_cast<uint64_t>(node->get_init().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_type().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_arg(const AST::Arg::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Arg) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Arg</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">is_var: " << node->get_is_var() << "</TD></TR>\n";
        switch(node->get_direction()) {
        case Veriparse::AST::Arg::DirectionEnum::INPUT:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">direction: INPUT</TD></TR>\n";
            break;
        case Veriparse::AST::Arg::DirectionEnum::OUTPUT:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">direction: OUTPUT</TD></TR>\n";
            break;
        case Veriparse::AST::Arg::DirectionEnum::INOUT:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">direction: INOUT</TD></TR>\n";
            break;
        case Veriparse::AST::Arg::DirectionEnum::REF:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">direction: REF</TD></TR>\n";
            break;
        default:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">direction: CONST_REF</TD></TR>\n";
        }
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: " << node->get_name() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">unpacked_dims</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">default_value</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p3\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">type</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_unpacked_dims()) {
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        if(node->get_default_value().get()) {
            ss << render(node->get_default_value());
        }
        if(node->get_type().get()) {
            ss << render(node->get_type());
        }
        uint64_t childID;
        if(node->get_unpacked_dims()) {
            int i = 0;
            for(const AST::Dimension::Ptr &n : *node->get_unpacked_dims()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
        childID = reinterpret_cast<uint64_t>(node->get_default_value().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_type().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_genvar(const AST::Genvar::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Genvar) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Genvar</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: " << node->get_name() << "</TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
    }

    return ss.str();
}

std::string DotGenerator::render_concat(const AST::Concat::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Concat) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Concat</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">list</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_list()) {
            for(const AST::Node::Ptr &n : *node->get_list()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        if(node->get_list()) {
            int i = 0;
            for(const AST::Node::Ptr &n : *node->get_list()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_lconcat(const AST::Lconcat::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Lconcat) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Lconcat</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">list</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_list()) {
            for(const AST::Node::Ptr &n : *node->get_list()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        if(node->get_list()) {
            int i = 0;
            for(const AST::Node::Ptr &n : *node->get_list()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_repeat(const AST::Repeat::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Repeat) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Repeat</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">value</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">times</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_value().get()) {
            ss << render(node->get_value());
        }
        if(node->get_times().get()) {
            ss << render(node->get_times());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_value().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_times().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_assignmentpattern(const AST::AssignmentPattern::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::AssignmentPattern) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">AssignmentPattern</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">items</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">times</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_items()) {
            for(const AST::Node::Ptr &n : *node->get_items()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        if(node->get_times().get()) {
            ss << render(node->get_times());
        }
        uint64_t childID;
        if(node->get_items()) {
            int i = 0;
            for(const AST::Node::Ptr &n : *node->get_items()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
        childID = reinterpret_cast<uint64_t>(node->get_times().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_patternitem(const AST::PatternItem::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::PatternItem) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">PatternItem</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">is_default: " << node->get_is_default()
           << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">key</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">value</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_key().get()) {
            ss << render(node->get_key());
        }
        if(node->get_value().get()) {
            ss << render(node->get_value());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_key().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_value().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_cast(const AST::Cast::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Cast) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Cast</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">expr</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_expr().get()) {
            ss << render(node->get_expr());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_expr().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_typecast(const AST::TypeCast::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::TypeCast) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">TypeCast</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">target</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">expr</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_target().get()) {
            ss << render(node->get_target());
        }
        if(node->get_expr().get()) {
            ss << render(node->get_expr());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_target().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_expr().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_sizecast(const AST::SizeCast::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::SizeCast) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">SizeCast</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">size</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">expr</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_size().get()) {
            ss << render(node->get_size());
        }
        if(node->get_expr().get()) {
            ss << render(node->get_expr());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_size().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_expr().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_signingcast(const AST::SigningCast::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::SigningCast) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">SigningCast</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        switch(node->get_signing()) {
        case Veriparse::AST::SigningCast::SigningEnum::SIGNED:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: SIGNED</TD></TR>\n";
            break;
        default:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">signing: UNSIGNED</TD></TR>\n";
        }
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">expr</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_expr().get()) {
            ss << render(node->get_expr());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_expr().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_constcast(const AST::ConstCast::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::ConstCast) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">ConstCast</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">expr</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_expr().get()) {
            ss << render(node->get_expr());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_expr().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_indirect(const AST::Indirect::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Indirect) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Indirect</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">var</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_var().get()) {
            ss << render(node->get_var());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_var().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_partselect(const AST::Partselect::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Partselect) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Partselect</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">msb</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">lsb</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p3\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">var</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_msb().get()) {
            ss << render(node->get_msb());
        }
        if(node->get_lsb().get()) {
            ss << render(node->get_lsb());
        }
        if(node->get_var().get()) {
            ss << render(node->get_var());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_msb().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_lsb().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_var().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_partselectindexed(const AST::PartselectIndexed::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::PartselectIndexed) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">PartselectIndexed</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">index</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">size</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p3\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">var</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_index().get()) {
            ss << render(node->get_index());
        }
        if(node->get_size().get()) {
            ss << render(node->get_size());
        }
        if(node->get_var().get()) {
            ss << render(node->get_var());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_index().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_size().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_var().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string
DotGenerator::render_partselectplusindexed(const AST::PartselectPlusIndexed::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::PartselectPlusIndexed) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">PartselectPlusIndexed</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">index</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">size</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p3\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">var</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_index().get()) {
            ss << render(node->get_index());
        }
        if(node->get_size().get()) {
            ss << render(node->get_size());
        }
        if(node->get_var().get()) {
            ss << render(node->get_var());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_index().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_size().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_var().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string
DotGenerator::render_partselectminusindexed(const AST::PartselectMinusIndexed::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::PartselectMinusIndexed) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">PartselectMinusIndexed</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">index</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">size</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p3\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">var</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_index().get()) {
            ss << render(node->get_index());
        }
        if(node->get_size().get()) {
            ss << render(node->get_size());
        }
        if(node->get_var().get()) {
            ss << render(node->get_var());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_index().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_size().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_var().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_pointer(const AST::Pointer::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Pointer) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Pointer</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">ptr</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">var</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_ptr().get()) {
            ss << render(node->get_ptr());
        }
        if(node->get_var().get()) {
            ss << render(node->get_var());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_ptr().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_var().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_lvalue(const AST::Lvalue::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Lvalue) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Lvalue</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">var</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_var().get()) {
            ss << render(node->get_var());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_var().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_rvalue(const AST::Rvalue::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Rvalue) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Rvalue</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">var</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_var().get()) {
            ss << render(node->get_var());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_var().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_unaryoperator(const AST::UnaryOperator::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::UnaryOperator) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">UnaryOperator</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_right().get()) {
            ss << render(node->get_right());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_right().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_uplus(const AST::Uplus::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Uplus) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Uplus</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_right().get()) {
            ss << render(node->get_right());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_right().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_uminus(const AST::Uminus::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Uminus) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Uminus</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_right().get()) {
            ss << render(node->get_right());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_right().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_ulnot(const AST::Ulnot::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Ulnot) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Ulnot</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_right().get()) {
            ss << render(node->get_right());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_right().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_unot(const AST::Unot::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Unot) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Unot</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_right().get()) {
            ss << render(node->get_right());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_right().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_uand(const AST::Uand::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Uand) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Uand</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_right().get()) {
            ss << render(node->get_right());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_right().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_unand(const AST::Unand::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Unand) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Unand</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_right().get()) {
            ss << render(node->get_right());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_right().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_uor(const AST::Uor::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Uor) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Uor</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_right().get()) {
            ss << render(node->get_right());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_right().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_unor(const AST::Unor::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Unor) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Unor</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_right().get()) {
            ss << render(node->get_right());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_right().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_uxor(const AST::Uxor::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Uxor) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Uxor</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_right().get()) {
            ss << render(node->get_right());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_right().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_uxnor(const AST::Uxnor::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Uxnor) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Uxnor</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_right().get()) {
            ss << render(node->get_right());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_right().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_operator(const AST::Operator::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Operator) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Operator</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">left</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_left().get()) {
            ss << render(node->get_left());
        }
        if(node->get_right().get()) {
            ss << render(node->get_right());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_left().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_right().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_power(const AST::Power::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Power) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Power</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">left</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_left().get()) {
            ss << render(node->get_left());
        }
        if(node->get_right().get()) {
            ss << render(node->get_right());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_left().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_right().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_times(const AST::Times::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Times) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Times</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">left</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_left().get()) {
            ss << render(node->get_left());
        }
        if(node->get_right().get()) {
            ss << render(node->get_right());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_left().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_right().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_divide(const AST::Divide::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Divide) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Divide</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">left</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_left().get()) {
            ss << render(node->get_left());
        }
        if(node->get_right().get()) {
            ss << render(node->get_right());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_left().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_right().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_mod(const AST::Mod::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Mod) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Mod</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">left</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_left().get()) {
            ss << render(node->get_left());
        }
        if(node->get_right().get()) {
            ss << render(node->get_right());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_left().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_right().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_plus(const AST::Plus::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Plus) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Plus</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">left</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_left().get()) {
            ss << render(node->get_left());
        }
        if(node->get_right().get()) {
            ss << render(node->get_right());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_left().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_right().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_minus(const AST::Minus::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Minus) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Minus</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">left</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_left().get()) {
            ss << render(node->get_left());
        }
        if(node->get_right().get()) {
            ss << render(node->get_right());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_left().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_right().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_sll(const AST::Sll::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Sll) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Sll</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">left</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_left().get()) {
            ss << render(node->get_left());
        }
        if(node->get_right().get()) {
            ss << render(node->get_right());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_left().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_right().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_srl(const AST::Srl::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Srl) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Srl</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">left</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_left().get()) {
            ss << render(node->get_left());
        }
        if(node->get_right().get()) {
            ss << render(node->get_right());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_left().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_right().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_sra(const AST::Sra::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Sra) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Sra</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">left</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_left().get()) {
            ss << render(node->get_left());
        }
        if(node->get_right().get()) {
            ss << render(node->get_right());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_left().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_right().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_lessthan(const AST::LessThan::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::LessThan) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">LessThan</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">left</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_left().get()) {
            ss << render(node->get_left());
        }
        if(node->get_right().get()) {
            ss << render(node->get_right());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_left().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_right().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_greaterthan(const AST::GreaterThan::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::GreaterThan) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">GreaterThan</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">left</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_left().get()) {
            ss << render(node->get_left());
        }
        if(node->get_right().get()) {
            ss << render(node->get_right());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_left().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_right().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_lesseq(const AST::LessEq::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::LessEq) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">LessEq</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">left</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_left().get()) {
            ss << render(node->get_left());
        }
        if(node->get_right().get()) {
            ss << render(node->get_right());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_left().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_right().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_greatereq(const AST::GreaterEq::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::GreaterEq) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">GreaterEq</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">left</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_left().get()) {
            ss << render(node->get_left());
        }
        if(node->get_right().get()) {
            ss << render(node->get_right());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_left().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_right().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_eq(const AST::Eq::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Eq) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Eq</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">left</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_left().get()) {
            ss << render(node->get_left());
        }
        if(node->get_right().get()) {
            ss << render(node->get_right());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_left().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_right().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_noteq(const AST::NotEq::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::NotEq) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">NotEq</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">left</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_left().get()) {
            ss << render(node->get_left());
        }
        if(node->get_right().get()) {
            ss << render(node->get_right());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_left().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_right().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_eql(const AST::Eql::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Eql) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Eql</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">left</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_left().get()) {
            ss << render(node->get_left());
        }
        if(node->get_right().get()) {
            ss << render(node->get_right());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_left().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_right().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_noteql(const AST::NotEql::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::NotEql) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">NotEql</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">left</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_left().get()) {
            ss << render(node->get_left());
        }
        if(node->get_right().get()) {
            ss << render(node->get_right());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_left().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_right().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_and(const AST::And::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::And) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">And</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">left</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_left().get()) {
            ss << render(node->get_left());
        }
        if(node->get_right().get()) {
            ss << render(node->get_right());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_left().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_right().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_xor(const AST::Xor::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Xor) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Xor</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">left</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_left().get()) {
            ss << render(node->get_left());
        }
        if(node->get_right().get()) {
            ss << render(node->get_right());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_left().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_right().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_xnor(const AST::Xnor::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Xnor) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Xnor</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">left</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_left().get()) {
            ss << render(node->get_left());
        }
        if(node->get_right().get()) {
            ss << render(node->get_right());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_left().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_right().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_or(const AST::Or::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Or) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Or</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">left</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_left().get()) {
            ss << render(node->get_left());
        }
        if(node->get_right().get()) {
            ss << render(node->get_right());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_left().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_right().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_land(const AST::Land::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Land) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Land</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">left</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_left().get()) {
            ss << render(node->get_left());
        }
        if(node->get_right().get()) {
            ss << render(node->get_right());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_left().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_right().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_lor(const AST::Lor::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Lor) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Lor</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">left</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_left().get()) {
            ss << render(node->get_left());
        }
        if(node->get_right().get()) {
            ss << render(node->get_right());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_left().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_right().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_cond(const AST::Cond::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Cond) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Cond</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">cond</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">left</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p3\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_cond().get()) {
            ss << render(node->get_cond());
        }
        if(node->get_left().get()) {
            ss << render(node->get_left());
        }
        if(node->get_right().get()) {
            ss << render(node->get_right());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_cond().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_left().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_right().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_always(const AST::Always::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Always) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Always</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">senslist</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">statement</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_senslist().get()) {
            ss << render(node->get_senslist());
        }
        if(node->get_statement().get()) {
            ss << render(node->get_statement());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_senslist().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_statement().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_alwaysff(const AST::AlwaysFF::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::AlwaysFF) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">AlwaysFF</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">senslist</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">statement</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_senslist().get()) {
            ss << render(node->get_senslist());
        }
        if(node->get_statement().get()) {
            ss << render(node->get_statement());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_senslist().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_statement().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_alwayscomb(const AST::AlwaysComb::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::AlwaysComb) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">AlwaysComb</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">senslist</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">statement</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_senslist().get()) {
            ss << render(node->get_senslist());
        }
        if(node->get_statement().get()) {
            ss << render(node->get_statement());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_senslist().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_statement().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_alwayslatch(const AST::AlwaysLatch::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::AlwaysLatch) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">AlwaysLatch</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">senslist</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">statement</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_senslist().get()) {
            ss << render(node->get_senslist());
        }
        if(node->get_statement().get()) {
            ss << render(node->get_statement());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_senslist().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_statement().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_senslist(const AST::Senslist::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Senslist) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Senslist</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">list</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_list()) {
            for(const AST::Sens::Ptr &n : *node->get_list()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        if(node->get_list()) {
            int i = 0;
            for(const AST::Sens::Ptr &n : *node->get_list()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_sens(const AST::Sens::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Sens) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Sens</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        switch(node->get_type()) {
        case Veriparse::AST::Sens::TypeEnum::ALL:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">type: ALL</TD></TR>\n";
            break;
        case Veriparse::AST::Sens::TypeEnum::POSEDGE:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">type: POSEDGE</TD></TR>\n";
            break;
        case Veriparse::AST::Sens::TypeEnum::NEGEDGE:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">type: NEGEDGE</TD></TR>\n";
            break;
        default:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">type: NONE</TD></TR>\n";
        }
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">sig</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_sig().get()) {
            ss << render(node->get_sig());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_sig().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_defparamlist(const AST::Defparamlist::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Defparamlist) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Defparamlist</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">list</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_list()) {
            for(const AST::Defparam::Ptr &n : *node->get_list()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        if(node->get_list()) {
            int i = 0;
            for(const AST::Defparam::Ptr &n : *node->get_list()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_defparam(const AST::Defparam::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Defparam) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Defparam</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">identifier</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_identifier().get()) {
            ss << render(node->get_identifier());
        }
        if(node->get_right().get()) {
            ss << render(node->get_right());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_identifier().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_right().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_assign(const AST::Assign::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Assign) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Assign</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">left</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p3\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">ldelay</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p4\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">rdelay</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_left().get()) {
            ss << render(node->get_left());
        }
        if(node->get_right().get()) {
            ss << render(node->get_right());
        }
        if(node->get_ldelay().get()) {
            ss << render(node->get_ldelay());
        }
        if(node->get_rdelay().get()) {
            ss << render(node->get_rdelay());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_left().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_right().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_ldelay().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_rdelay().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p4 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string
DotGenerator::render_blockingsubstitution(const AST::BlockingSubstitution::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::BlockingSubstitution) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">BlockingSubstitution</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">left</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p3\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">ldelay</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p4\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">rdelay</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_left().get()) {
            ss << render(node->get_left());
        }
        if(node->get_right().get()) {
            ss << render(node->get_right());
        }
        if(node->get_ldelay().get()) {
            ss << render(node->get_ldelay());
        }
        if(node->get_rdelay().get()) {
            ss << render(node->get_rdelay());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_left().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_right().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_ldelay().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_rdelay().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p4 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string
DotGenerator::render_nonblockingsubstitution(const AST::NonblockingSubstitution::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::NonblockingSubstitution) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">NonblockingSubstitution</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">left</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p3\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">ldelay</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p4\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">rdelay</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_left().get()) {
            ss << render(node->get_left());
        }
        if(node->get_right().get()) {
            ss << render(node->get_right());
        }
        if(node->get_ldelay().get()) {
            ss << render(node->get_ldelay());
        }
        if(node->get_rdelay().get()) {
            ss << render(node->get_rdelay());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_left().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_right().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_ldelay().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_rdelay().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p4 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_ifstatement(const AST::IfStatement::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::IfStatement) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">IfStatement</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">cond</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">true_statement</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p3\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">false_statement</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_cond().get()) {
            ss << render(node->get_cond());
        }
        if(node->get_true_statement().get()) {
            ss << render(node->get_true_statement());
        }
        if(node->get_false_statement().get()) {
            ss << render(node->get_false_statement());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_cond().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_true_statement().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_false_statement().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_repeatstatement(const AST::RepeatStatement::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::RepeatStatement) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">RepeatStatement</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">times</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">statement</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_times().get()) {
            ss << render(node->get_times());
        }
        if(node->get_statement().get()) {
            ss << render(node->get_statement());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_times().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_statement().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_forstatement(const AST::ForStatement::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::ForStatement) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">ForStatement</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">pre</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">cond</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p3\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">post</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p4\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">statement</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_pre().get()) {
            ss << render(node->get_pre());
        }
        if(node->get_cond().get()) {
            ss << render(node->get_cond());
        }
        if(node->get_post().get()) {
            ss << render(node->get_post());
        }
        if(node->get_statement().get()) {
            ss << render(node->get_statement());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_pre().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_cond().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_post().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_statement().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p4 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_whilestatement(const AST::WhileStatement::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::WhileStatement) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">WhileStatement</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">cond</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">statement</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_cond().get()) {
            ss << render(node->get_cond());
        }
        if(node->get_statement().get()) {
            ss << render(node->get_statement());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_cond().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_statement().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_casestatement(const AST::CaseStatement::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::CaseStatement) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">CaseStatement</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">comp</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">caselist</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_comp().get()) {
            ss << render(node->get_comp());
        }
        if(node->get_caselist()) {
            for(const AST::Case::Ptr &n : *node->get_caselist()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_comp().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        if(node->get_caselist()) {
            int i = 0;
            for(const AST::Case::Ptr &n : *node->get_caselist()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p2 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_casexstatement(const AST::CasexStatement::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::CasexStatement) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">CasexStatement</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">comp</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">caselist</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_comp().get()) {
            ss << render(node->get_comp());
        }
        if(node->get_caselist()) {
            for(const AST::Case::Ptr &n : *node->get_caselist()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_comp().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        if(node->get_caselist()) {
            int i = 0;
            for(const AST::Case::Ptr &n : *node->get_caselist()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p2 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_casezstatement(const AST::CasezStatement::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::CasezStatement) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">CasezStatement</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">comp</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">caselist</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_comp().get()) {
            ss << render(node->get_comp());
        }
        if(node->get_caselist()) {
            for(const AST::Case::Ptr &n : *node->get_caselist()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_comp().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        if(node->get_caselist()) {
            int i = 0;
            for(const AST::Case::Ptr &n : *node->get_caselist()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p2 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_uniquecasestatement(const AST::UniqueCaseStatement::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::UniqueCaseStatement) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">UniqueCaseStatement</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">comp</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">caselist</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_comp().get()) {
            ss << render(node->get_comp());
        }
        if(node->get_caselist()) {
            for(const AST::Case::Ptr &n : *node->get_caselist()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_comp().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        if(node->get_caselist()) {
            int i = 0;
            for(const AST::Case::Ptr &n : *node->get_caselist()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p2 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string
DotGenerator::render_prioritycasestatement(const AST::PriorityCaseStatement::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::PriorityCaseStatement) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">PriorityCaseStatement</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">comp</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">caselist</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_comp().get()) {
            ss << render(node->get_comp());
        }
        if(node->get_caselist()) {
            for(const AST::Case::Ptr &n : *node->get_caselist()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_comp().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        if(node->get_caselist()) {
            int i = 0;
            for(const AST::Case::Ptr &n : *node->get_caselist()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p2 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_case(const AST::Case::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Case) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Case</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">cond</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">statement</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_cond()) {
            for(const AST::Node::Ptr &n : *node->get_cond()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        if(node->get_statement().get()) {
            ss << render(node->get_statement());
        }
        uint64_t childID;
        if(node->get_cond()) {
            int i = 0;
            for(const AST::Node::Ptr &n : *node->get_cond()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
        childID = reinterpret_cast<uint64_t>(node->get_statement().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_block(const AST::Block::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Block) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Block</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">scope: " << node->get_scope() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">statements</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_statements()) {
            for(const AST::Node::Ptr &n : *node->get_statements()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        if(node->get_statements()) {
            int i = 0;
            for(const AST::Node::Ptr &n : *node->get_statements()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_initial(const AST::Initial::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Initial) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Initial</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">statement</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_statement().get()) {
            ss << render(node->get_statement());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_statement().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_eventstatement(const AST::EventStatement::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::EventStatement) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">EventStatement</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">senslist</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">statement</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_senslist().get()) {
            ss << render(node->get_senslist());
        }
        if(node->get_statement().get()) {
            ss << render(node->get_statement());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_senslist().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_statement().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_waitstatement(const AST::WaitStatement::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::WaitStatement) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">WaitStatement</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">cond</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">statement</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_cond().get()) {
            ss << render(node->get_cond());
        }
        if(node->get_statement().get()) {
            ss << render(node->get_statement());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_cond().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_statement().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_foreverstatement(const AST::ForeverStatement::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::ForeverStatement) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">ForeverStatement</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">statement</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_statement().get()) {
            ss << render(node->get_statement());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_statement().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_delaystatement(const AST::DelayStatement::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::DelayStatement) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">DelayStatement</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">delay</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_delay().get()) {
            ss << render(node->get_delay());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_delay().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_instancelist(const AST::Instancelist::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Instancelist) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Instancelist</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">module: " << node->get_module() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">parameterlist</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">instances</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_parameterlist()) {
            for(const AST::ParamArg::Ptr &n : *node->get_parameterlist()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        if(node->get_instances()) {
            for(const AST::Instance::Ptr &n : *node->get_instances()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        if(node->get_parameterlist()) {
            int i = 0;
            for(const AST::ParamArg::Ptr &n : *node->get_parameterlist()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
        if(node->get_instances()) {
            int i = 0;
            for(const AST::Instance::Ptr &n : *node->get_instances()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p2 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_instance(const AST::Instance::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Instance) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Instance</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">module: " << node->get_module() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: " << node->get_name() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">array</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">parameterlist</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p3\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">portlist</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_array().get()) {
            ss << render(node->get_array());
        }
        if(node->get_parameterlist()) {
            for(const AST::ParamArg::Ptr &n : *node->get_parameterlist()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        if(node->get_portlist()) {
            for(const AST::PortArg::Ptr &n : *node->get_portlist()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_array().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        if(node->get_parameterlist()) {
            int i = 0;
            for(const AST::ParamArg::Ptr &n : *node->get_parameterlist()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p2 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
        if(node->get_portlist()) {
            int i = 0;
            for(const AST::PortArg::Ptr &n : *node->get_portlist()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p3 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_paramarg(const AST::ParamArg::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::ParamArg) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">ParamArg</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: " << node->get_name() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">value</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_value().get()) {
            ss << render(node->get_value());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_value().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_portarg(const AST::PortArg::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::PortArg) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">PortArg</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: " << node->get_name() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">value</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_value().get()) {
            ss << render(node->get_value());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_value().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_function(const AST::Function::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Function) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Function</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: " << node->get_name() << "</TD></TR>\n";
        switch(node->get_lifetime()) {
        case Veriparse::AST::Function::LifetimeEnum::NONE:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">lifetime: NONE</TD></TR>\n";
            break;
        case Veriparse::AST::Function::LifetimeEnum::AUTOMATIC:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">lifetime: AUTOMATIC</TD></TR>\n";
            break;
        default:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">lifetime: STATIC</TD></TR>\n";
        }
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">return_type</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">args</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p3\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">statements</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_return_type().get()) {
            ss << render(node->get_return_type());
        }
        if(node->get_args()) {
            for(const AST::Arg::Ptr &n : *node->get_args()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        if(node->get_statements()) {
            for(const AST::Node::Ptr &n : *node->get_statements()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_return_type().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        if(node->get_args()) {
            int i = 0;
            for(const AST::Arg::Ptr &n : *node->get_args()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p2 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
        if(node->get_statements()) {
            int i = 0;
            for(const AST::Node::Ptr &n : *node->get_statements()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p3 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_functioncall(const AST::FunctionCall::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::FunctionCall) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">FunctionCall</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: " << node->get_name() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">scope</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">args</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_scope()) {
            for(const AST::ScopeName::Ptr &n : *node->get_scope()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        if(node->get_args()) {
            for(const AST::Node::Ptr &n : *node->get_args()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        if(node->get_scope()) {
            int i = 0;
            for(const AST::ScopeName::Ptr &n : *node->get_scope()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
        if(node->get_args()) {
            int i = 0;
            for(const AST::Node::Ptr &n : *node->get_args()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p2 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_task(const AST::Task::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Task) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Task</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: " << node->get_name() << "</TD></TR>\n";
        switch(node->get_lifetime()) {
        case Veriparse::AST::Task::LifetimeEnum::NONE:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">lifetime: NONE</TD></TR>\n";
            break;
        case Veriparse::AST::Task::LifetimeEnum::AUTOMATIC:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">lifetime: AUTOMATIC</TD></TR>\n";
            break;
        default:
            ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">lifetime: STATIC</TD></TR>\n";
        }
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">args</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">statements</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_args()) {
            for(const AST::Arg::Ptr &n : *node->get_args()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        if(node->get_statements()) {
            for(const AST::Node::Ptr &n : *node->get_statements()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        if(node->get_args()) {
            int i = 0;
            for(const AST::Arg::Ptr &n : *node->get_args()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
        if(node->get_statements()) {
            int i = 0;
            for(const AST::Node::Ptr &n : *node->get_statements()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p2 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_taskcall(const AST::TaskCall::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::TaskCall) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">TaskCall</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: " << node->get_name() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">scope</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">args</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_scope()) {
            for(const AST::ScopeName::Ptr &n : *node->get_scope()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        if(node->get_args()) {
            for(const AST::Node::Ptr &n : *node->get_args()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        if(node->get_scope()) {
            int i = 0;
            for(const AST::ScopeName::Ptr &n : *node->get_scope()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
        if(node->get_args()) {
            int i = 0;
            for(const AST::Node::Ptr &n : *node->get_args()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p2 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_generatestatement(const AST::GenerateStatement::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::GenerateStatement) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">GenerateStatement</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">items</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_items()) {
            for(const AST::Node::Ptr &n : *node->get_items()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        if(node->get_items()) {
            int i = 0;
            for(const AST::Node::Ptr &n : *node->get_items()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_systemcall(const AST::SystemCall::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::SystemCall) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">SystemCall</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">syscall: " << node->get_syscall()
           << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">args</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_args()) {
            for(const AST::Node::Ptr &n : *node->get_args()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        if(node->get_args()) {
            int i = 0;
            for(const AST::Node::Ptr &n : *node->get_args()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_hierlabel(const AST::HierLabel::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::HierLabel) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">HierLabel</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: " << node->get_name() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">loop</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_loop().get()) {
            ss << render(node->get_loop());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_loop().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

std::string DotGenerator::render_hiername(const AST::HierName::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::HierName) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">HierName</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">labellist</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_labellist()) {
            for(const AST::HierLabel::Ptr &n : *node->get_labellist()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        if(node->get_labellist()) {
            int i = 0;
            for(const AST::HierLabel::Ptr &n : *node->get_labellist()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_disable(const AST::Disable::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::Disable) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">Disable</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">dest: " << node->get_dest() << "</TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
    }

    return ss.str();
}

std::string DotGenerator::render_parallelblock(const AST::ParallelBlock::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::ParallelBlock) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">ParallelBlock</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">scope: " << node->get_scope() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">statements</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_statements()) {
            for(const AST::Node::Ptr &n : *node->get_statements()) {
                if(n) {
                    ss << render(n);
                }
            }
        }
        uint64_t childID;
        if(node->get_statements()) {
            int i = 0;
            for(const AST::Node::Ptr &n : *node->get_statements()) {
                childID = reinterpret_cast<uint64_t>(n.get());
                if(childID) {
                    ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++
                       << "\"];" << std::endl;
                }
            }
        }
    }

    return ss.str();
}

std::string DotGenerator::render_singlestatement(const AST::SingleStatement::Ptr node) const
{
    std::stringstream ss;

    if(node) {
        if(node->get_node_type() != AST::NodeType::SingleStatement) {
            return render(AST::cast_to<AST::Node>(node));
        }

        uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

        ss << "\tn" << nodeID
           << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
           << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
           << "<FONT COLOR=\"white\">SingleStatement</FONT></TD></TR>\n"
           << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
        ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">scope: " << node->get_scope() << "</TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">statement</FONT></TD></TR>\n";
        ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
           << "<FONT COLOR=\"wheat\">delay</FONT></TD></TR>\n";

        ss << "\t\t</TABLE>>];" << std::endl;
        if(node->get_statement().get()) {
            ss << render(node->get_statement());
        }
        if(node->get_delay().get()) {
            ss << render(node->get_delay());
        }
        uint64_t childID;
        childID = reinterpret_cast<uint64_t>(node->get_statement().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
        }
        childID = reinterpret_cast<uint64_t>(node->get_delay().get());
        if(childID) {
            ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
        }
    }

    return ss.str();
}

} // namespace Generators
} // namespace Veriparse