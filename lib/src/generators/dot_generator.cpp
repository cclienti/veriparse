#include <veriparse/generators/dot_generator.hpp>
#include <sstream>
#include <fstream>
#include <cstdint>


namespace Veriparse {
	namespace Generators {

		void DotGenerator::save_dot(const std::string &dot, const std::string &filename) {
			std::ofstream fout(filename);
			fout << "digraph G {\n"
			     << "\trankdir=LR;\n"
			     << "\tnode [shape=plaintext];\n"
			     << dot << "}" << std::endl;
		}


		std::string DotGenerator::render_node(const AST::Node::Ptr node) const {
			std::stringstream ss;

			if (node) {
				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());
				ss << "\tn" << nodeID << " [label=\"{<p0>Node}"
				   << " | {line: " << node->get_line() << "}" << "\"];" << std::endl;
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_source(const AST::Source::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Source) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_description().get()) {
					ss << render(node->get_description());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_description().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_description(const AST::Description::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Description) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Description</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">definitions</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_definitions()) {
					for(const AST::Node::Ptr &n: *node->get_definitions()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				uint64_t childID;
				if (node->get_definitions()) {
					int i=0;
					for(const AST::Node::Ptr &n: *node->get_definitions()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_pragma(const AST::Pragma::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Pragma) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Pragma</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: "
				     << node->get_name() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">expression</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_expression().get()) {
					ss << render(node->get_expression());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_expression().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_module(const AST::Module::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Module) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Module</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: "
				     << node->get_name() << "</TD></TR>\n";
				switch(node->get_default_nettype()) {
				case Veriparse::AST::Module::Default_nettypeEnum::INTEGER:
					ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">default_nettype: INTEGER</TD></TR>\n";
					break;
				case Veriparse::AST::Module::Default_nettypeEnum::REAL:
					ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">default_nettype: REAL</TD></TR>\n";
					break;
				case Veriparse::AST::Module::Default_nettypeEnum::REG:
					ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">default_nettype: REG</TD></TR>\n";
					break;
				case Veriparse::AST::Module::Default_nettypeEnum::TRI:
					ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">default_nettype: TRI</TD></TR>\n";
					break;
				case Veriparse::AST::Module::Default_nettypeEnum::WIRE:
					ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">default_nettype: WIRE</TD></TR>\n";
					break;
				case Veriparse::AST::Module::Default_nettypeEnum::SUPPLY0:
					ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">default_nettype: SUPPLY0</TD></TR>\n";
					break;
				case Veriparse::AST::Module::Default_nettypeEnum::SUPPLY1:
					ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">default_nettype: SUPPLY1</TD></TR>\n";
					break;
				default: ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">default_nettype: NONE</TD></TR>\n";
				}
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">params</FONT></TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">ports</FONT></TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p3\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">items</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_params()) {
					for(const AST::Parameter::Ptr &n: *node->get_params()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				if (node->get_ports()) {
					for(const AST::Node::Ptr &n: *node->get_ports()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				if (node->get_items()) {
					for(const AST::Node::Ptr &n: *node->get_items()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				uint64_t childID;
				if (node->get_params()) {
					int i=0;
					for(const AST::Parameter::Ptr &n: *node->get_params()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
				if (node->get_ports()) {
					int i=0;
					for(const AST::Node::Ptr &n: *node->get_ports()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p2 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
				if (node->get_items()) {
					int i=0;
					for(const AST::Node::Ptr &n: *node->get_items()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p3 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_port(const AST::Port::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Port) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Port</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: "
				     << node->get_name() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">widths</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_widths()) {
					for(const AST::Width::Ptr &n: *node->get_widths()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				uint64_t childID;
				if (node->get_widths()) {
					int i=0;
					for(const AST::Width::Ptr &n: *node->get_widths()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_width(const AST::Width::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Width) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Width</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">msb</FONT></TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">lsb</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_msb().get()) {
					ss << render(node->get_msb());
				}
				if (node->get_lsb().get()) {
					ss << render(node->get_lsb());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_msb().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_lsb().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_length(const AST::Length::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Length) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Length</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">msb</FONT></TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">lsb</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_msb().get()) {
					ss << render(node->get_msb());
				}
				if (node->get_lsb().get()) {
					ss << render(node->get_lsb());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_msb().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_lsb().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_identifier(const AST::Identifier::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Identifier) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Identifier</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: "
				     << node->get_name() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">scope</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_scope().get()) {
					ss << render(node->get_scope());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_scope().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_constant(const AST::Constant::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Constant) return render(AST::cast_to<AST::Node>(node));

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
		
		

		std::string DotGenerator::render_stringconst(const AST::StringConst::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::StringConst) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">StringConst</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">value: "
				     << node->get_value() << "</TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_intconst(const AST::IntConst::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::IntConst) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">IntConst</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">value: "
				     << node->get_value() << "</TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_intconstn(const AST::IntConstN::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::IntConstN) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">IntConstN</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">base: "
				     << node->get_base() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">size: "
				     << node->get_size() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">sign: "
				     << node->get_sign() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">value: "
				     << node->get_value() << "</TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_floatconst(const AST::FloatConst::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::FloatConst) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">FloatConst</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">value: "
				     << node->get_value() << "</TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_iodir(const AST::IODir::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::IODir) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">IODir</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: "
				     << node->get_name() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">sign: "
				     << node->get_sign() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">widths</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_widths()) {
					for(const AST::Width::Ptr &n: *node->get_widths()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				uint64_t childID;
				if (node->get_widths()) {
					int i=0;
					for(const AST::Width::Ptr &n: *node->get_widths()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_input(const AST::Input::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Input) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Input</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: "
				     << node->get_name() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">sign: "
				     << node->get_sign() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">widths</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_widths()) {
					for(const AST::Width::Ptr &n: *node->get_widths()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				uint64_t childID;
				if (node->get_widths()) {
					int i=0;
					for(const AST::Width::Ptr &n: *node->get_widths()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_output(const AST::Output::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Output) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Output</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: "
				     << node->get_name() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">sign: "
				     << node->get_sign() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">widths</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_widths()) {
					for(const AST::Width::Ptr &n: *node->get_widths()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				uint64_t childID;
				if (node->get_widths()) {
					int i=0;
					for(const AST::Width::Ptr &n: *node->get_widths()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_inout(const AST::Inout::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Inout) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Inout</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: "
				     << node->get_name() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">sign: "
				     << node->get_sign() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">widths</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_widths()) {
					for(const AST::Width::Ptr &n: *node->get_widths()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				uint64_t childID;
				if (node->get_widths()) {
					int i=0;
					for(const AST::Width::Ptr &n: *node->get_widths()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_variablebase(const AST::VariableBase::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::VariableBase) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">VariableBase</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: "
				     << node->get_name() << "</TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_genvar(const AST::Genvar::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Genvar) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Genvar</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: "
				     << node->get_name() << "</TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_variable(const AST::Variable::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Variable) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Variable</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: "
				     << node->get_name() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">lengths</FONT></TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_lengths()) {
					for(const AST::Length::Ptr &n: *node->get_lengths()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				if (node->get_lengths()) {
					int i=0;
					for(const AST::Length::Ptr &n: *node->get_lengths()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_net(const AST::Net::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Net) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Net</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">sign: "
				     << node->get_sign() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: "
				     << node->get_name() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">widths</FONT></TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">ldelay</FONT></TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p3\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">rdelay</FONT></TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p4\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">lengths</FONT></TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p5\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_widths()) {
					for(const AST::Width::Ptr &n: *node->get_widths()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				if (node->get_ldelay().get()) {
					ss << render(node->get_ldelay());
				}
				if (node->get_rdelay().get()) {
					ss << render(node->get_rdelay());
				}
				if (node->get_lengths()) {
					for(const AST::Length::Ptr &n: *node->get_lengths()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				if (node->get_widths()) {
					int i=0;
					for(const AST::Width::Ptr &n: *node->get_widths()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
				childID = reinterpret_cast<uint64_t>(node->get_ldelay().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_rdelay().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
				}
				if (node->get_lengths()) {
					int i=0;
					for(const AST::Length::Ptr &n: *node->get_lengths()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p4 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p5 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_integer(const AST::Integer::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Integer) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Integer</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: "
				     << node->get_name() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">lengths</FONT></TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_lengths()) {
					for(const AST::Length::Ptr &n: *node->get_lengths()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				if (node->get_lengths()) {
					int i=0;
					for(const AST::Length::Ptr &n: *node->get_lengths()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_real(const AST::Real::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Real) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Real</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: "
				     << node->get_name() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">lengths</FONT></TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_lengths()) {
					for(const AST::Length::Ptr &n: *node->get_lengths()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				if (node->get_lengths()) {
					int i=0;
					for(const AST::Length::Ptr &n: *node->get_lengths()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_tri(const AST::Tri::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Tri) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Tri</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">sign: "
				     << node->get_sign() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: "
				     << node->get_name() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">widths</FONT></TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">ldelay</FONT></TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p3\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">rdelay</FONT></TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p4\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">lengths</FONT></TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p5\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_widths()) {
					for(const AST::Width::Ptr &n: *node->get_widths()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				if (node->get_ldelay().get()) {
					ss << render(node->get_ldelay());
				}
				if (node->get_rdelay().get()) {
					ss << render(node->get_rdelay());
				}
				if (node->get_lengths()) {
					for(const AST::Length::Ptr &n: *node->get_lengths()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				if (node->get_widths()) {
					int i=0;
					for(const AST::Width::Ptr &n: *node->get_widths()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
				childID = reinterpret_cast<uint64_t>(node->get_ldelay().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_rdelay().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
				}
				if (node->get_lengths()) {
					int i=0;
					for(const AST::Length::Ptr &n: *node->get_lengths()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p4 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p5 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_wire(const AST::Wire::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Wire) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Wire</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">sign: "
				     << node->get_sign() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: "
				     << node->get_name() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">widths</FONT></TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">ldelay</FONT></TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p3\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">rdelay</FONT></TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p4\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">lengths</FONT></TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p5\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_widths()) {
					for(const AST::Width::Ptr &n: *node->get_widths()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				if (node->get_ldelay().get()) {
					ss << render(node->get_ldelay());
				}
				if (node->get_rdelay().get()) {
					ss << render(node->get_rdelay());
				}
				if (node->get_lengths()) {
					for(const AST::Length::Ptr &n: *node->get_lengths()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				if (node->get_widths()) {
					int i=0;
					for(const AST::Width::Ptr &n: *node->get_widths()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
				childID = reinterpret_cast<uint64_t>(node->get_ldelay().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_rdelay().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
				}
				if (node->get_lengths()) {
					int i=0;
					for(const AST::Length::Ptr &n: *node->get_lengths()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p4 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p5 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_supply0(const AST::Supply0::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Supply0) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Supply0</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">sign: "
				     << node->get_sign() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: "
				     << node->get_name() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">widths</FONT></TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">ldelay</FONT></TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p3\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">rdelay</FONT></TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p4\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">lengths</FONT></TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p5\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_widths()) {
					for(const AST::Width::Ptr &n: *node->get_widths()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				if (node->get_ldelay().get()) {
					ss << render(node->get_ldelay());
				}
				if (node->get_rdelay().get()) {
					ss << render(node->get_rdelay());
				}
				if (node->get_lengths()) {
					for(const AST::Length::Ptr &n: *node->get_lengths()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				if (node->get_widths()) {
					int i=0;
					for(const AST::Width::Ptr &n: *node->get_widths()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
				childID = reinterpret_cast<uint64_t>(node->get_ldelay().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_rdelay().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
				}
				if (node->get_lengths()) {
					int i=0;
					for(const AST::Length::Ptr &n: *node->get_lengths()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p4 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p5 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_supply1(const AST::Supply1::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Supply1) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Supply1</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">sign: "
				     << node->get_sign() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: "
				     << node->get_name() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">widths</FONT></TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">ldelay</FONT></TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p3\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">rdelay</FONT></TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p4\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">lengths</FONT></TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p5\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_widths()) {
					for(const AST::Width::Ptr &n: *node->get_widths()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				if (node->get_ldelay().get()) {
					ss << render(node->get_ldelay());
				}
				if (node->get_rdelay().get()) {
					ss << render(node->get_rdelay());
				}
				if (node->get_lengths()) {
					for(const AST::Length::Ptr &n: *node->get_lengths()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				if (node->get_widths()) {
					int i=0;
					for(const AST::Width::Ptr &n: *node->get_widths()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
				childID = reinterpret_cast<uint64_t>(node->get_ldelay().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_rdelay().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
				}
				if (node->get_lengths()) {
					int i=0;
					for(const AST::Length::Ptr &n: *node->get_lengths()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p4 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p5 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_reg(const AST::Reg::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Reg) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Reg</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">sign: "
				     << node->get_sign() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: "
				     << node->get_name() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">widths</FONT></TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">lengths</FONT></TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p3\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_widths()) {
					for(const AST::Width::Ptr &n: *node->get_widths()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				if (node->get_lengths()) {
					for(const AST::Length::Ptr &n: *node->get_lengths()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				if (node->get_widths()) {
					int i=0;
					for(const AST::Width::Ptr &n: *node->get_widths()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
				if (node->get_lengths()) {
					int i=0;
					for(const AST::Length::Ptr &n: *node->get_lengths()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p2 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_ioport(const AST::Ioport::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Ioport) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Ioport</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">first</FONT></TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">second</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_first().get()) {
					ss << render(node->get_first());
				}
				if (node->get_second().get()) {
					ss << render(node->get_second());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_first().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_second().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_parameter(const AST::Parameter::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Parameter) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Parameter</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: "
				     << node->get_name() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">sign: "
				     << node->get_sign() << "</TD></TR>\n";
				switch(node->get_type()) {
				case Veriparse::AST::Parameter::TypeEnum::INTEGER:
					ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">type: INTEGER</TD></TR>\n";
					break;
				case Veriparse::AST::Parameter::TypeEnum::REAL:
					ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">type: REAL</TD></TR>\n";
					break;
				default: ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">type: NONE</TD></TR>\n";
				}
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">value</FONT></TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">widths</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_value().get()) {
					ss << render(node->get_value());
				}
				if (node->get_widths()) {
					for(const AST::Width::Ptr &n: *node->get_widths()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_value().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				if (node->get_widths()) {
					int i=0;
					for(const AST::Width::Ptr &n: *node->get_widths()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p2 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_localparam(const AST::Localparam::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Localparam) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Localparam</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: "
				     << node->get_name() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">sign: "
				     << node->get_sign() << "</TD></TR>\n";
				switch(node->get_type()) {
				case Veriparse::AST::Localparam::Parameter::TypeEnum::INTEGER:
					ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">type: INTEGER</TD></TR>\n";
					break;
				case Veriparse::AST::Localparam::Parameter::TypeEnum::REAL:
					ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">type: REAL</TD></TR>\n";
					break;
				default: ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">type: NONE</TD></TR>\n";
				}
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">value</FONT></TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">widths</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_value().get()) {
					ss << render(node->get_value());
				}
				if (node->get_widths()) {
					for(const AST::Width::Ptr &n: *node->get_widths()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_value().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				if (node->get_widths()) {
					int i=0;
					for(const AST::Width::Ptr &n: *node->get_widths()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p2 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_concat(const AST::Concat::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Concat) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Concat</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">list</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_list()) {
					for(const AST::Node::Ptr &n: *node->get_list()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				uint64_t childID;
				if (node->get_list()) {
					int i=0;
					for(const AST::Node::Ptr &n: *node->get_list()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_lconcat(const AST::Lconcat::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Lconcat) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Lconcat</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">list</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_list()) {
					for(const AST::Node::Ptr &n: *node->get_list()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				uint64_t childID;
				if (node->get_list()) {
					int i=0;
					for(const AST::Node::Ptr &n: *node->get_list()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_repeat(const AST::Repeat::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Repeat) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_value().get()) {
					ss << render(node->get_value());
				}
				if (node->get_times().get()) {
					ss << render(node->get_times());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_value().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_times().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_indirect(const AST::Indirect::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Indirect) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Indirect</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">var</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_var().get()) {
					ss << render(node->get_var());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_var().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_partselect(const AST::Partselect::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Partselect) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_msb().get()) {
					ss << render(node->get_msb());
				}
				if (node->get_lsb().get()) {
					ss << render(node->get_lsb());
				}
				if (node->get_var().get()) {
					ss << render(node->get_var());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_msb().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_lsb().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_var().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_partselectindexed(const AST::PartselectIndexed::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::PartselectIndexed) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_index().get()) {
					ss << render(node->get_index());
				}
				if (node->get_size().get()) {
					ss << render(node->get_size());
				}
				if (node->get_var().get()) {
					ss << render(node->get_var());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_index().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_size().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_var().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_partselectplusindexed(const AST::PartselectPlusIndexed::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::PartselectPlusIndexed) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_index().get()) {
					ss << render(node->get_index());
				}
				if (node->get_size().get()) {
					ss << render(node->get_size());
				}
				if (node->get_var().get()) {
					ss << render(node->get_var());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_index().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_size().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_var().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_partselectminusindexed(const AST::PartselectMinusIndexed::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::PartselectMinusIndexed) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_index().get()) {
					ss << render(node->get_index());
				}
				if (node->get_size().get()) {
					ss << render(node->get_size());
				}
				if (node->get_var().get()) {
					ss << render(node->get_var());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_index().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_size().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_var().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_pointer(const AST::Pointer::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Pointer) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_ptr().get()) {
					ss << render(node->get_ptr());
				}
				if (node->get_var().get()) {
					ss << render(node->get_var());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_ptr().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_var().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_lvalue(const AST::Lvalue::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Lvalue) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Lvalue</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">var</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_var().get()) {
					ss << render(node->get_var());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_var().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_rvalue(const AST::Rvalue::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Rvalue) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Rvalue</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">var</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_var().get()) {
					ss << render(node->get_var());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_var().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_unaryoperator(const AST::UnaryOperator::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::UnaryOperator) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">UnaryOperator</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_uplus(const AST::Uplus::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Uplus) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Uplus</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_uminus(const AST::Uminus::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Uminus) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Uminus</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_ulnot(const AST::Ulnot::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Ulnot) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Ulnot</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_unot(const AST::Unot::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Unot) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Unot</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_uand(const AST::Uand::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Uand) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Uand</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_unand(const AST::Unand::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Unand) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Unand</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_uor(const AST::Uor::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Uor) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Uor</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_unor(const AST::Unor::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Unor) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Unor</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_uxor(const AST::Uxor::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Uxor) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Uxor</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_uxnor(const AST::Uxnor::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Uxnor) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Uxnor</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">right</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_operator(const AST::Operator::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Operator) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_left().get()) {
					ss << render(node->get_left());
				}
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_left().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_power(const AST::Power::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Power) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_left().get()) {
					ss << render(node->get_left());
				}
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_left().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_times(const AST::Times::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Times) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_left().get()) {
					ss << render(node->get_left());
				}
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_left().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_divide(const AST::Divide::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Divide) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_left().get()) {
					ss << render(node->get_left());
				}
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_left().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_mod(const AST::Mod::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Mod) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_left().get()) {
					ss << render(node->get_left());
				}
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_left().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_plus(const AST::Plus::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Plus) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_left().get()) {
					ss << render(node->get_left());
				}
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_left().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_minus(const AST::Minus::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Minus) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_left().get()) {
					ss << render(node->get_left());
				}
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_left().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_sll(const AST::Sll::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Sll) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_left().get()) {
					ss << render(node->get_left());
				}
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_left().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_srl(const AST::Srl::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Srl) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_left().get()) {
					ss << render(node->get_left());
				}
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_left().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_sra(const AST::Sra::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Sra) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_left().get()) {
					ss << render(node->get_left());
				}
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_left().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_lessthan(const AST::LessThan::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::LessThan) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_left().get()) {
					ss << render(node->get_left());
				}
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_left().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_greaterthan(const AST::GreaterThan::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::GreaterThan) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_left().get()) {
					ss << render(node->get_left());
				}
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_left().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_lesseq(const AST::LessEq::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::LessEq) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_left().get()) {
					ss << render(node->get_left());
				}
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_left().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_greatereq(const AST::GreaterEq::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::GreaterEq) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_left().get()) {
					ss << render(node->get_left());
				}
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_left().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_eq(const AST::Eq::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Eq) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_left().get()) {
					ss << render(node->get_left());
				}
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_left().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_noteq(const AST::NotEq::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::NotEq) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_left().get()) {
					ss << render(node->get_left());
				}
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_left().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_eql(const AST::Eql::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Eql) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_left().get()) {
					ss << render(node->get_left());
				}
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_left().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_noteql(const AST::NotEql::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::NotEql) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_left().get()) {
					ss << render(node->get_left());
				}
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_left().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_and(const AST::And::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::And) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_left().get()) {
					ss << render(node->get_left());
				}
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_left().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_xor(const AST::Xor::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Xor) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_left().get()) {
					ss << render(node->get_left());
				}
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_left().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_xnor(const AST::Xnor::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Xnor) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_left().get()) {
					ss << render(node->get_left());
				}
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_left().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_or(const AST::Or::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Or) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_left().get()) {
					ss << render(node->get_left());
				}
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_left().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_land(const AST::Land::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Land) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_left().get()) {
					ss << render(node->get_left());
				}
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_left().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_lor(const AST::Lor::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Lor) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_left().get()) {
					ss << render(node->get_left());
				}
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_left().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_cond(const AST::Cond::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Cond) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_cond().get()) {
					ss << render(node->get_cond());
				}
				if (node->get_left().get()) {
					ss << render(node->get_left());
				}
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_cond().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_left().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_always(const AST::Always::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Always) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_senslist().get()) {
					ss << render(node->get_senslist());
				}
				if (node->get_statement().get()) {
					ss << render(node->get_statement());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_senslist().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_statement().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_senslist(const AST::Senslist::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Senslist) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Senslist</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">list</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_list()) {
					for(const AST::Sens::Ptr &n: *node->get_list()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				uint64_t childID;
				if (node->get_list()) {
					int i=0;
					for(const AST::Sens::Ptr &n: *node->get_list()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_sens(const AST::Sens::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Sens) return render(AST::cast_to<AST::Node>(node));

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
				default: ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">type: NONE</TD></TR>\n";
				}
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">sig</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_sig().get()) {
					ss << render(node->get_sig());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_sig().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_assign(const AST::Assign::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Assign) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_left().get()) {
					ss << render(node->get_left());
				}
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				if (node->get_ldelay().get()) {
					ss << render(node->get_ldelay());
				}
				if (node->get_rdelay().get()) {
					ss << render(node->get_rdelay());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_left().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_ldelay().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_rdelay().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p4 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_blockingsubstitution(const AST::BlockingSubstitution::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::BlockingSubstitution) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_left().get()) {
					ss << render(node->get_left());
				}
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				if (node->get_ldelay().get()) {
					ss << render(node->get_ldelay());
				}
				if (node->get_rdelay().get()) {
					ss << render(node->get_rdelay());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_left().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_ldelay().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_rdelay().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p4 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_nonblockingsubstitution(const AST::NonblockingSubstitution::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::NonblockingSubstitution) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_left().get()) {
					ss << render(node->get_left());
				}
				if (node->get_right().get()) {
					ss << render(node->get_right());
				}
				if (node->get_ldelay().get()) {
					ss << render(node->get_ldelay());
				}
				if (node->get_rdelay().get()) {
					ss << render(node->get_rdelay());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_left().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_right().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_ldelay().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_rdelay().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p4 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_ifstatement(const AST::IfStatement::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::IfStatement) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_cond().get()) {
					ss << render(node->get_cond());
				}
				if (node->get_true_statement().get()) {
					ss << render(node->get_true_statement());
				}
				if (node->get_false_statement().get()) {
					ss << render(node->get_false_statement());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_cond().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_true_statement().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_false_statement().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_repeatstatement(const AST::RepeatStatement::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::RepeatStatement) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_times().get()) {
					ss << render(node->get_times());
				}
				if (node->get_statement().get()) {
					ss << render(node->get_statement());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_times().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_statement().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_forstatement(const AST::ForStatement::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::ForStatement) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_pre().get()) {
					ss << render(node->get_pre());
				}
				if (node->get_cond().get()) {
					ss << render(node->get_cond());
				}
				if (node->get_post().get()) {
					ss << render(node->get_post());
				}
				if (node->get_statement().get()) {
					ss << render(node->get_statement());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_pre().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_cond().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_post().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p3 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_statement().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p4 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_whilestatement(const AST::WhileStatement::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::WhileStatement) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_cond().get()) {
					ss << render(node->get_cond());
				}
				if (node->get_statement().get()) {
					ss << render(node->get_statement());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_cond().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_statement().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_casestatement(const AST::CaseStatement::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::CaseStatement) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_comp().get()) {
					ss << render(node->get_comp());
				}
				if (node->get_caselist()) {
					for(const AST::Case::Ptr &n: *node->get_caselist()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_comp().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				if (node->get_caselist()) {
					int i=0;
					for(const AST::Case::Ptr &n: *node->get_caselist()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p2 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_casexstatement(const AST::CasexStatement::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::CasexStatement) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_comp().get()) {
					ss << render(node->get_comp());
				}
				if (node->get_caselist()) {
					for(const AST::Case::Ptr &n: *node->get_caselist()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_comp().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				if (node->get_caselist()) {
					int i=0;
					for(const AST::Case::Ptr &n: *node->get_caselist()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p2 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_casezstatement(const AST::CasezStatement::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::CasezStatement) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_comp().get()) {
					ss << render(node->get_comp());
				}
				if (node->get_caselist()) {
					for(const AST::Case::Ptr &n: *node->get_caselist()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_comp().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				if (node->get_caselist()) {
					int i=0;
					for(const AST::Case::Ptr &n: *node->get_caselist()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p2 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_case(const AST::Case::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Case) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_cond()) {
					for(const AST::Node::Ptr &n: *node->get_cond()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				if (node->get_statement().get()) {
					ss << render(node->get_statement());
				}
				uint64_t childID;
				if (node->get_cond()) {
					int i=0;
					for(const AST::Node::Ptr &n: *node->get_cond()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
				childID = reinterpret_cast<uint64_t>(node->get_statement().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_block(const AST::Block::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Block) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Block</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">scope: "
				     << node->get_scope() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">statements</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_statements()) {
					for(const AST::Node::Ptr &n: *node->get_statements()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				uint64_t childID;
				if (node->get_statements()) {
					int i=0;
					for(const AST::Node::Ptr &n: *node->get_statements()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_initial(const AST::Initial::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Initial) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Initial</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">statement</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_statement().get()) {
					ss << render(node->get_statement());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_statement().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_eventstatement(const AST::EventStatement::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::EventStatement) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_senslist().get()) {
					ss << render(node->get_senslist());
				}
				if (node->get_statement().get()) {
					ss << render(node->get_statement());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_senslist().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_statement().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_waitstatement(const AST::WaitStatement::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::WaitStatement) return render(AST::cast_to<AST::Node>(node));

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
				if (node->get_cond().get()) {
					ss << render(node->get_cond());
				}
				if (node->get_statement().get()) {
					ss << render(node->get_statement());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_cond().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_statement().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_foreverstatement(const AST::ForeverStatement::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::ForeverStatement) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">ForeverStatement</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">statement</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_statement().get()) {
					ss << render(node->get_statement());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_statement().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_delaystatement(const AST::DelayStatement::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::DelayStatement) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">DelayStatement</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">delay</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_delay().get()) {
					ss << render(node->get_delay());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_delay().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_instancelist(const AST::Instancelist::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Instancelist) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Instancelist</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">module: "
				     << node->get_module() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">parameterlist</FONT></TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">instances</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_parameterlist()) {
					for(const AST::ParamArg::Ptr &n: *node->get_parameterlist()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				if (node->get_instances()) {
					for(const AST::Instance::Ptr &n: *node->get_instances()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				uint64_t childID;
				if (node->get_parameterlist()) {
					int i=0;
					for(const AST::ParamArg::Ptr &n: *node->get_parameterlist()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
				if (node->get_instances()) {
					int i=0;
					for(const AST::Instance::Ptr &n: *node->get_instances()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p2 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_instance(const AST::Instance::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Instance) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Instance</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">module: "
				     << node->get_module() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: "
				     << node->get_name() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">array</FONT></TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">parameterlist</FONT></TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p3\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">portlist</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_array().get()) {
					ss << render(node->get_array());
				}
				if (node->get_parameterlist()) {
					for(const AST::ParamArg::Ptr &n: *node->get_parameterlist()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				if (node->get_portlist()) {
					for(const AST::PortArg::Ptr &n: *node->get_portlist()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_array().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				if (node->get_parameterlist()) {
					int i=0;
					for(const AST::ParamArg::Ptr &n: *node->get_parameterlist()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p2 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
				if (node->get_portlist()) {
					int i=0;
					for(const AST::PortArg::Ptr &n: *node->get_portlist()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p3 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_paramarg(const AST::ParamArg::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::ParamArg) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">ParamArg</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: "
				     << node->get_name() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">value</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_value().get()) {
					ss << render(node->get_value());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_value().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_portarg(const AST::PortArg::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::PortArg) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">PortArg</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: "
				     << node->get_name() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">value</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_value().get()) {
					ss << render(node->get_value());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_value().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_function(const AST::Function::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Function) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Function</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: "
				     << node->get_name() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">automatic: "
				     << node->get_automatic() << "</TD></TR>\n";
				switch(node->get_rettype()) {
				case Veriparse::AST::Function::RettypeEnum::INTEGER:
					ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">rettype: INTEGER</TD></TR>\n";
					break;
				case Veriparse::AST::Function::RettypeEnum::REAL:
					ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">rettype: REAL</TD></TR>\n";
					break;
				default: ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">rettype: NONE</TD></TR>\n";
				}
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">retsign: "
				     << node->get_retsign() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">retwidths</FONT></TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">ports</FONT></TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p3\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">statements</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_retwidths()) {
					for(const AST::Width::Ptr &n: *node->get_retwidths()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				if (node->get_ports()) {
					for(const AST::Node::Ptr &n: *node->get_ports()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				if (node->get_statements()) {
					for(const AST::Node::Ptr &n: *node->get_statements()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				uint64_t childID;
				if (node->get_retwidths()) {
					int i=0;
					for(const AST::Width::Ptr &n: *node->get_retwidths()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
				if (node->get_ports()) {
					int i=0;
					for(const AST::Node::Ptr &n: *node->get_ports()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p2 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
				if (node->get_statements()) {
					int i=0;
					for(const AST::Node::Ptr &n: *node->get_statements()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p3 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_functioncall(const AST::FunctionCall::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::FunctionCall) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">FunctionCall</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: "
				     << node->get_name() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">args</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_args()) {
					for(const AST::Node::Ptr &n: *node->get_args()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				uint64_t childID;
				if (node->get_args()) {
					int i=0;
					for(const AST::Node::Ptr &n: *node->get_args()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_task(const AST::Task::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Task) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Task</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: "
				     << node->get_name() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">automatic: "
				     << node->get_automatic() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">ports</FONT></TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">statements</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_ports()) {
					for(const AST::Node::Ptr &n: *node->get_ports()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				if (node->get_statements()) {
					for(const AST::Node::Ptr &n: *node->get_statements()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				uint64_t childID;
				if (node->get_ports()) {
					int i=0;
					for(const AST::Node::Ptr &n: *node->get_ports()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
				if (node->get_statements()) {
					int i=0;
					for(const AST::Node::Ptr &n: *node->get_statements()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p2 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_taskcall(const AST::TaskCall::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::TaskCall) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">TaskCall</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: "
				     << node->get_name() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">args</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_args()) {
					for(const AST::Node::Ptr &n: *node->get_args()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				uint64_t childID;
				if (node->get_args()) {
					int i=0;
					for(const AST::Node::Ptr &n: *node->get_args()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_generatestatement(const AST::GenerateStatement::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::GenerateStatement) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">GenerateStatement</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">items</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_items()) {
					for(const AST::Node::Ptr &n: *node->get_items()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				uint64_t childID;
				if (node->get_items()) {
					int i=0;
					for(const AST::Node::Ptr &n: *node->get_items()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_systemcall(const AST::SystemCall::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::SystemCall) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">SystemCall</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">syscall: "
				     << node->get_syscall() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">args</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_args()) {
					for(const AST::Node::Ptr &n: *node->get_args()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				uint64_t childID;
				if (node->get_args()) {
					int i=0;
					for(const AST::Node::Ptr &n: *node->get_args()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_identifierscopelabel(const AST::IdentifierScopeLabel::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::IdentifierScopeLabel) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">IdentifierScopeLabel</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">name: "
				     << node->get_name() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">loop</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_loop().get()) {
					ss << render(node->get_loop());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_loop().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_identifierscope(const AST::IdentifierScope::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::IdentifierScope) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">IdentifierScope</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">labellist</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_labellist()) {
					for(const AST::IdentifierScopeLabel::Ptr &n: *node->get_labellist()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				uint64_t childID;
				if (node->get_labellist()) {
					int i=0;
					for(const AST::IdentifierScopeLabel::Ptr &n: *node->get_labellist()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_disable(const AST::Disable::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::Disable) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">Disable</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">dest: "
				     << node->get_dest() << "</TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_parallelblock(const AST::ParallelBlock::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::ParallelBlock) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">ParallelBlock</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">scope: "
				     << node->get_scope() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">statements</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_statements()) {
					for(const AST::Node::Ptr &n: *node->get_statements()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				uint64_t childID;
				if (node->get_statements()) {
					int i=0;
					for(const AST::Node::Ptr &n: *node->get_statements()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p1 -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
			}

			return ss.str();
		}
		
		

		std::string DotGenerator::render_singlestatement(const AST::SingleStatement::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::SingleStatement) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">SingleStatement</FONT></TD></TR>\n"
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">scope: "
				     << node->get_scope() << "</TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p1\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">statement</FONT></TD></TR>\n";
				ss << "\t\t<TR><TD PORT=\"p2\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">delay</FONT></TD></TR>\n";

				ss << "\t\t</TABLE>>];" << std::endl;
				if (node->get_statement().get()) {
					ss << render(node->get_statement());
				}
				if (node->get_delay().get()) {
					ss << render(node->get_delay());
				}
				uint64_t childID;
				childID = reinterpret_cast<uint64_t>(node->get_statement().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p1 -> n" << childID << ";" << std::endl;
				}
				childID = reinterpret_cast<uint64_t>(node->get_delay().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p2 -> n" << childID << ";" << std::endl;
				}
			}

			return ss.str();
		}
		

	}
}