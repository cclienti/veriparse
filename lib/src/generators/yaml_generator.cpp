#include <veriparse/generators/yaml_generator.hpp>
#include <veriparse/importers/yaml_specializations.hpp>



namespace Veriparse {
namespace Generators {

YAML::Node YAMLGenerator::render_node(const AST::Node::Ptr node) const {
	YAML::Node result;
	return result;
}


YAML::Node YAMLGenerator::render_source(const AST::Source::Ptr node) const {
	YAML::Node node_source;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Source) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["description"] = render(node->get_description());
	}

	node_source["Source"] = content;
	return node_source;
}



YAML::Node YAMLGenerator::render_description(const AST::Description::Ptr node) const {
	YAML::Node node_description;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Description) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		if (node->get_definitions()) {
			content["definitions"] = YAML::Load("[]");
			for(const AST::Node::Ptr &n: *node->get_definitions()) {
				content["definitions"].push_back(render(n));
			}
		}
	}

	node_description["Description"] = content;
	return node_description;
}



YAML::Node YAMLGenerator::render_pragmalist(const AST::Pragmalist::Ptr node) const {
	YAML::Node node_pragmalist;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Pragmalist) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		if (node->get_pragmas()) {
			content["pragmas"] = YAML::Load("[]");
			for(const AST::Pragma::Ptr &n: *node->get_pragmas()) {
				content["pragmas"].push_back(render(n));
			}
		}

		if (node->get_statements()) {
			content["statements"] = YAML::Load("[]");
			for(const AST::Node::Ptr &n: *node->get_statements()) {
				content["statements"].push_back(render(n));
			}
		}
	}

	node_pragmalist["Pragmalist"] = content;
	return node_pragmalist;
}



YAML::Node YAMLGenerator::render_pragma(const AST::Pragma::Ptr node) const {
	YAML::Node node_pragma;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Pragma) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();
		content["name"] = node->get_name();

		content["expression"] = render(node->get_expression());
	}

	node_pragma["Pragma"] = content;
	return node_pragma;
}



YAML::Node YAMLGenerator::render_module(const AST::Module::Ptr node) const {
	YAML::Node node_module;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Module) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();
		content["name"] = node->get_name();
		switch(node->get_default_nettype()) {
		case Veriparse::AST::Module::Default_nettypeEnum::INTEGER:
			content["default_nettype"] = "INTEGER";
			break;
		case Veriparse::AST::Module::Default_nettypeEnum::REAL:
			content["default_nettype"] = "REAL";
			break;
		case Veriparse::AST::Module::Default_nettypeEnum::REG:
			content["default_nettype"] = "REG";
			break;
		case Veriparse::AST::Module::Default_nettypeEnum::TRI:
			content["default_nettype"] = "TRI";
			break;
		case Veriparse::AST::Module::Default_nettypeEnum::WIRE:
			content["default_nettype"] = "WIRE";
			break;
		case Veriparse::AST::Module::Default_nettypeEnum::SUPPLY0:
			content["default_nettype"] = "SUPPLY0";
			break;
		case Veriparse::AST::Module::Default_nettypeEnum::SUPPLY1:
			content["default_nettype"] = "SUPPLY1";
			break;
		default: content["default_nettype"] = "NONE";
		}

		if (node->get_params()) {
			content["params"] = YAML::Load("[]");
			for(const AST::Parameter::Ptr &n: *node->get_params()) {
				content["params"].push_back(render(n));
			}
		}

		if (node->get_ports()) {
			content["ports"] = YAML::Load("[]");
			for(const AST::Node::Ptr &n: *node->get_ports()) {
				content["ports"].push_back(render(n));
			}
		}

		if (node->get_items()) {
			content["items"] = YAML::Load("[]");
			for(const AST::Node::Ptr &n: *node->get_items()) {
				content["items"].push_back(render(n));
			}
		}
	}

	node_module["Module"] = content;
	return node_module;
}



YAML::Node YAMLGenerator::render_port(const AST::Port::Ptr node) const {
	YAML::Node node_port;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Port) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();
		content["name"] = node->get_name();

		if (node->get_widths()) {
			content["widths"] = YAML::Load("[]");
			for(const AST::Width::Ptr &n: *node->get_widths()) {
				content["widths"].push_back(render(n));
			}
		}
	}

	node_port["Port"] = content;
	return node_port;
}



YAML::Node YAMLGenerator::render_width(const AST::Width::Ptr node) const {
	YAML::Node node_width;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Width) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["msb"] = render(node->get_msb());

		content["lsb"] = render(node->get_lsb());
	}

	node_width["Width"] = content;
	return node_width;
}



YAML::Node YAMLGenerator::render_length(const AST::Length::Ptr node) const {
	YAML::Node node_length;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Length) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["msb"] = render(node->get_msb());

		content["lsb"] = render(node->get_lsb());
	}

	node_length["Length"] = content;
	return node_length;
}



YAML::Node YAMLGenerator::render_identifier(const AST::Identifier::Ptr node) const {
	YAML::Node node_identifier;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Identifier) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();
		content["name"] = node->get_name();

		content["scope"] = render(node->get_scope());
	}

	node_identifier["Identifier"] = content;
	return node_identifier;
}



YAML::Node YAMLGenerator::render_constant(const AST::Constant::Ptr node) const {
	YAML::Node node_constant;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Constant) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();
	}

	node_constant["Constant"] = content;
	return node_constant;
}



YAML::Node YAMLGenerator::render_stringconst(const AST::StringConst::Ptr node) const {
	YAML::Node node_stringconst;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::StringConst) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();
		content["value"] = node->get_value();
	}

	node_stringconst["StringConst"] = content;
	return node_stringconst;
}



YAML::Node YAMLGenerator::render_intconst(const AST::IntConst::Ptr node) const {
	YAML::Node node_intconst;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::IntConst) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();
		content["value"] = node->get_value();
	}

	node_intconst["IntConst"] = content;
	return node_intconst;
}



YAML::Node YAMLGenerator::render_intconstn(const AST::IntConstN::Ptr node) const {
	YAML::Node node_intconstn;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::IntConstN) return render(AST::cast_to<AST::Node>(node));

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



YAML::Node YAMLGenerator::render_floatconst(const AST::FloatConst::Ptr node) const {
	YAML::Node node_floatconst;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::FloatConst) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();
		content["value"] = node->get_value();
	}

	node_floatconst["FloatConst"] = content;
	return node_floatconst;
}



YAML::Node YAMLGenerator::render_iodir(const AST::IODir::Ptr node) const {
	YAML::Node node_iodir;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::IODir) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();
		content["name"] = node->get_name();
		content["sign"] = node->get_sign();

		if (node->get_widths()) {
			content["widths"] = YAML::Load("[]");
			for(const AST::Width::Ptr &n: *node->get_widths()) {
				content["widths"].push_back(render(n));
			}
		}
	}

	node_iodir["IODir"] = content;
	return node_iodir;
}



YAML::Node YAMLGenerator::render_input(const AST::Input::Ptr node) const {
	YAML::Node node_input;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Input) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();
		content["name"] = node->get_name();
		content["sign"] = node->get_sign();

		if (node->get_widths()) {
			content["widths"] = YAML::Load("[]");
			for(const AST::Width::Ptr &n: *node->get_widths()) {
				content["widths"].push_back(render(n));
			}
		}
	}

	node_input["Input"] = content;
	return node_input;
}



YAML::Node YAMLGenerator::render_output(const AST::Output::Ptr node) const {
	YAML::Node node_output;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Output) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();
		content["name"] = node->get_name();
		content["sign"] = node->get_sign();

		if (node->get_widths()) {
			content["widths"] = YAML::Load("[]");
			for(const AST::Width::Ptr &n: *node->get_widths()) {
				content["widths"].push_back(render(n));
			}
		}
	}

	node_output["Output"] = content;
	return node_output;
}



YAML::Node YAMLGenerator::render_inout(const AST::Inout::Ptr node) const {
	YAML::Node node_inout;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Inout) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();
		content["name"] = node->get_name();
		content["sign"] = node->get_sign();

		if (node->get_widths()) {
			content["widths"] = YAML::Load("[]");
			for(const AST::Width::Ptr &n: *node->get_widths()) {
				content["widths"].push_back(render(n));
			}
		}
	}

	node_inout["Inout"] = content;
	return node_inout;
}



YAML::Node YAMLGenerator::render_variablebase(const AST::VariableBase::Ptr node) const {
	YAML::Node node_variablebase;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::VariableBase) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();
		content["name"] = node->get_name();
	}

	node_variablebase["VariableBase"] = content;
	return node_variablebase;
}



YAML::Node YAMLGenerator::render_genvar(const AST::Genvar::Ptr node) const {
	YAML::Node node_genvar;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Genvar) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();
		content["name"] = node->get_name();
	}

	node_genvar["Genvar"] = content;
	return node_genvar;
}



YAML::Node YAMLGenerator::render_variable(const AST::Variable::Ptr node) const {
	YAML::Node node_variable;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Variable) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();
		content["name"] = node->get_name();

		if (node->get_lengths()) {
			content["lengths"] = YAML::Load("[]");
			for(const AST::Length::Ptr &n: *node->get_lengths()) {
				content["lengths"].push_back(render(n));
			}
		}

		content["right"] = render(node->get_right());
	}

	node_variable["Variable"] = content;
	return node_variable;
}



YAML::Node YAMLGenerator::render_net(const AST::Net::Ptr node) const {
	YAML::Node node_net;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Net) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();
		content["sign"] = node->get_sign();
		content["name"] = node->get_name();

		if (node->get_widths()) {
			content["widths"] = YAML::Load("[]");
			for(const AST::Width::Ptr &n: *node->get_widths()) {
				content["widths"].push_back(render(n));
			}
		}

		content["ldelay"] = render(node->get_ldelay());

		content["rdelay"] = render(node->get_rdelay());

		if (node->get_lengths()) {
			content["lengths"] = YAML::Load("[]");
			for(const AST::Length::Ptr &n: *node->get_lengths()) {
				content["lengths"].push_back(render(n));
			}
		}

		content["right"] = render(node->get_right());
	}

	node_net["Net"] = content;
	return node_net;
}



YAML::Node YAMLGenerator::render_integer(const AST::Integer::Ptr node) const {
	YAML::Node node_integer;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Integer) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();
		content["name"] = node->get_name();

		if (node->get_lengths()) {
			content["lengths"] = YAML::Load("[]");
			for(const AST::Length::Ptr &n: *node->get_lengths()) {
				content["lengths"].push_back(render(n));
			}
		}

		content["right"] = render(node->get_right());
	}

	node_integer["Integer"] = content;
	return node_integer;
}



YAML::Node YAMLGenerator::render_real(const AST::Real::Ptr node) const {
	YAML::Node node_real;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Real) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();
		content["name"] = node->get_name();

		if (node->get_lengths()) {
			content["lengths"] = YAML::Load("[]");
			for(const AST::Length::Ptr &n: *node->get_lengths()) {
				content["lengths"].push_back(render(n));
			}
		}

		content["right"] = render(node->get_right());
	}

	node_real["Real"] = content;
	return node_real;
}



YAML::Node YAMLGenerator::render_tri(const AST::Tri::Ptr node) const {
	YAML::Node node_tri;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Tri) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();
		content["sign"] = node->get_sign();
		content["name"] = node->get_name();

		if (node->get_widths()) {
			content["widths"] = YAML::Load("[]");
			for(const AST::Width::Ptr &n: *node->get_widths()) {
				content["widths"].push_back(render(n));
			}
		}

		content["ldelay"] = render(node->get_ldelay());

		content["rdelay"] = render(node->get_rdelay());

		if (node->get_lengths()) {
			content["lengths"] = YAML::Load("[]");
			for(const AST::Length::Ptr &n: *node->get_lengths()) {
				content["lengths"].push_back(render(n));
			}
		}

		content["right"] = render(node->get_right());
	}

	node_tri["Tri"] = content;
	return node_tri;
}



YAML::Node YAMLGenerator::render_wire(const AST::Wire::Ptr node) const {
	YAML::Node node_wire;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Wire) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();
		content["sign"] = node->get_sign();
		content["name"] = node->get_name();

		if (node->get_widths()) {
			content["widths"] = YAML::Load("[]");
			for(const AST::Width::Ptr &n: *node->get_widths()) {
				content["widths"].push_back(render(n));
			}
		}

		content["ldelay"] = render(node->get_ldelay());

		content["rdelay"] = render(node->get_rdelay());

		if (node->get_lengths()) {
			content["lengths"] = YAML::Load("[]");
			for(const AST::Length::Ptr &n: *node->get_lengths()) {
				content["lengths"].push_back(render(n));
			}
		}

		content["right"] = render(node->get_right());
	}

	node_wire["Wire"] = content;
	return node_wire;
}



YAML::Node YAMLGenerator::render_supply0(const AST::Supply0::Ptr node) const {
	YAML::Node node_supply0;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Supply0) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();
		content["sign"] = node->get_sign();
		content["name"] = node->get_name();

		if (node->get_widths()) {
			content["widths"] = YAML::Load("[]");
			for(const AST::Width::Ptr &n: *node->get_widths()) {
				content["widths"].push_back(render(n));
			}
		}

		content["ldelay"] = render(node->get_ldelay());

		content["rdelay"] = render(node->get_rdelay());

		if (node->get_lengths()) {
			content["lengths"] = YAML::Load("[]");
			for(const AST::Length::Ptr &n: *node->get_lengths()) {
				content["lengths"].push_back(render(n));
			}
		}

		content["right"] = render(node->get_right());
	}

	node_supply0["Supply0"] = content;
	return node_supply0;
}



YAML::Node YAMLGenerator::render_supply1(const AST::Supply1::Ptr node) const {
	YAML::Node node_supply1;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Supply1) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();
		content["sign"] = node->get_sign();
		content["name"] = node->get_name();

		if (node->get_widths()) {
			content["widths"] = YAML::Load("[]");
			for(const AST::Width::Ptr &n: *node->get_widths()) {
				content["widths"].push_back(render(n));
			}
		}

		content["ldelay"] = render(node->get_ldelay());

		content["rdelay"] = render(node->get_rdelay());

		if (node->get_lengths()) {
			content["lengths"] = YAML::Load("[]");
			for(const AST::Length::Ptr &n: *node->get_lengths()) {
				content["lengths"].push_back(render(n));
			}
		}

		content["right"] = render(node->get_right());
	}

	node_supply1["Supply1"] = content;
	return node_supply1;
}



YAML::Node YAMLGenerator::render_reg(const AST::Reg::Ptr node) const {
	YAML::Node node_reg;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Reg) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();
		content["sign"] = node->get_sign();
		content["name"] = node->get_name();

		if (node->get_widths()) {
			content["widths"] = YAML::Load("[]");
			for(const AST::Width::Ptr &n: *node->get_widths()) {
				content["widths"].push_back(render(n));
			}
		}

		if (node->get_lengths()) {
			content["lengths"] = YAML::Load("[]");
			for(const AST::Length::Ptr &n: *node->get_lengths()) {
				content["lengths"].push_back(render(n));
			}
		}

		content["right"] = render(node->get_right());
	}

	node_reg["Reg"] = content;
	return node_reg;
}



YAML::Node YAMLGenerator::render_ioport(const AST::Ioport::Ptr node) const {
	YAML::Node node_ioport;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Ioport) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["first"] = render(node->get_first());

		content["second"] = render(node->get_second());
	}

	node_ioport["Ioport"] = content;
	return node_ioport;
}



YAML::Node YAMLGenerator::render_parameter(const AST::Parameter::Ptr node) const {
	YAML::Node node_parameter;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Parameter) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();
		content["name"] = node->get_name();
		content["sign"] = node->get_sign();
		switch(node->get_type()) {
		case Veriparse::AST::Parameter::TypeEnum::INTEGER:
			content["type"] = "INTEGER";
			break;
		case Veriparse::AST::Parameter::TypeEnum::REAL:
			content["type"] = "REAL";
			break;
		default: content["type"] = "NONE";
		}

		content["value"] = render(node->get_value());

		if (node->get_widths()) {
			content["widths"] = YAML::Load("[]");
			for(const AST::Width::Ptr &n: *node->get_widths()) {
				content["widths"].push_back(render(n));
			}
		}
	}

	node_parameter["Parameter"] = content;
	return node_parameter;
}



YAML::Node YAMLGenerator::render_localparam(const AST::Localparam::Ptr node) const {
	YAML::Node node_localparam;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Localparam) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();
		content["name"] = node->get_name();
		content["sign"] = node->get_sign();
		switch(node->get_type()) {
		case Veriparse::AST::Localparam::Parameter::TypeEnum::INTEGER:
			content["type"] = "INTEGER";
			break;
		case Veriparse::AST::Localparam::Parameter::TypeEnum::REAL:
			content["type"] = "REAL";
			break;
		default: content["type"] = "NONE";
		}

		content["value"] = render(node->get_value());

		if (node->get_widths()) {
			content["widths"] = YAML::Load("[]");
			for(const AST::Width::Ptr &n: *node->get_widths()) {
				content["widths"].push_back(render(n));
			}
		}
	}

	node_localparam["Localparam"] = content;
	return node_localparam;
}



YAML::Node YAMLGenerator::render_concat(const AST::Concat::Ptr node) const {
	YAML::Node node_concat;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Concat) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		if (node->get_list()) {
			content["list"] = YAML::Load("[]");
			for(const AST::Node::Ptr &n: *node->get_list()) {
				content["list"].push_back(render(n));
			}
		}
	}

	node_concat["Concat"] = content;
	return node_concat;
}



YAML::Node YAMLGenerator::render_lconcat(const AST::Lconcat::Ptr node) const {
	YAML::Node node_lconcat;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Lconcat) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		if (node->get_list()) {
			content["list"] = YAML::Load("[]");
			for(const AST::Node::Ptr &n: *node->get_list()) {
				content["list"].push_back(render(n));
			}
		}
	}

	node_lconcat["Lconcat"] = content;
	return node_lconcat;
}



YAML::Node YAMLGenerator::render_repeat(const AST::Repeat::Ptr node) const {
	YAML::Node node_repeat;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Repeat) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["value"] = render(node->get_value());

		content["times"] = render(node->get_times());
	}

	node_repeat["Repeat"] = content;
	return node_repeat;
}



YAML::Node YAMLGenerator::render_indirect(const AST::Indirect::Ptr node) const {
	YAML::Node node_indirect;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Indirect) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["var"] = render(node->get_var());
	}

	node_indirect["Indirect"] = content;
	return node_indirect;
}



YAML::Node YAMLGenerator::render_partselect(const AST::Partselect::Ptr node) const {
	YAML::Node node_partselect;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Partselect) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["msb"] = render(node->get_msb());

		content["lsb"] = render(node->get_lsb());

		content["var"] = render(node->get_var());
	}

	node_partselect["Partselect"] = content;
	return node_partselect;
}



YAML::Node YAMLGenerator::render_partselectindexed(const AST::PartselectIndexed::Ptr node) const {
	YAML::Node node_partselectindexed;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::PartselectIndexed) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["index"] = render(node->get_index());

		content["size"] = render(node->get_size());

		content["var"] = render(node->get_var());
	}

	node_partselectindexed["PartselectIndexed"] = content;
	return node_partselectindexed;
}



YAML::Node YAMLGenerator::render_partselectplusindexed(const AST::PartselectPlusIndexed::Ptr node) const {
	YAML::Node node_partselectplusindexed;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::PartselectPlusIndexed) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["index"] = render(node->get_index());

		content["size"] = render(node->get_size());

		content["var"] = render(node->get_var());
	}

	node_partselectplusindexed["PartselectPlusIndexed"] = content;
	return node_partselectplusindexed;
}



YAML::Node YAMLGenerator::render_partselectminusindexed(const AST::PartselectMinusIndexed::Ptr node) const {
	YAML::Node node_partselectminusindexed;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::PartselectMinusIndexed) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["index"] = render(node->get_index());

		content["size"] = render(node->get_size());

		content["var"] = render(node->get_var());
	}

	node_partselectminusindexed["PartselectMinusIndexed"] = content;
	return node_partselectminusindexed;
}



YAML::Node YAMLGenerator::render_pointer(const AST::Pointer::Ptr node) const {
	YAML::Node node_pointer;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Pointer) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["ptr"] = render(node->get_ptr());

		content["var"] = render(node->get_var());
	}

	node_pointer["Pointer"] = content;
	return node_pointer;
}



YAML::Node YAMLGenerator::render_lvalue(const AST::Lvalue::Ptr node) const {
	YAML::Node node_lvalue;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Lvalue) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["var"] = render(node->get_var());
	}

	node_lvalue["Lvalue"] = content;
	return node_lvalue;
}



YAML::Node YAMLGenerator::render_rvalue(const AST::Rvalue::Ptr node) const {
	YAML::Node node_rvalue;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Rvalue) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["var"] = render(node->get_var());
	}

	node_rvalue["Rvalue"] = content;
	return node_rvalue;
}



YAML::Node YAMLGenerator::render_unaryoperator(const AST::UnaryOperator::Ptr node) const {
	YAML::Node node_unaryoperator;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::UnaryOperator) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["right"] = render(node->get_right());
	}

	node_unaryoperator["UnaryOperator"] = content;
	return node_unaryoperator;
}



YAML::Node YAMLGenerator::render_uplus(const AST::Uplus::Ptr node) const {
	YAML::Node node_uplus;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Uplus) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["right"] = render(node->get_right());
	}

	node_uplus["Uplus"] = content;
	return node_uplus;
}



YAML::Node YAMLGenerator::render_uminus(const AST::Uminus::Ptr node) const {
	YAML::Node node_uminus;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Uminus) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["right"] = render(node->get_right());
	}

	node_uminus["Uminus"] = content;
	return node_uminus;
}



YAML::Node YAMLGenerator::render_ulnot(const AST::Ulnot::Ptr node) const {
	YAML::Node node_ulnot;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Ulnot) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["right"] = render(node->get_right());
	}

	node_ulnot["Ulnot"] = content;
	return node_ulnot;
}



YAML::Node YAMLGenerator::render_unot(const AST::Unot::Ptr node) const {
	YAML::Node node_unot;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Unot) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["right"] = render(node->get_right());
	}

	node_unot["Unot"] = content;
	return node_unot;
}



YAML::Node YAMLGenerator::render_uand(const AST::Uand::Ptr node) const {
	YAML::Node node_uand;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Uand) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["right"] = render(node->get_right());
	}

	node_uand["Uand"] = content;
	return node_uand;
}



YAML::Node YAMLGenerator::render_unand(const AST::Unand::Ptr node) const {
	YAML::Node node_unand;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Unand) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["right"] = render(node->get_right());
	}

	node_unand["Unand"] = content;
	return node_unand;
}



YAML::Node YAMLGenerator::render_uor(const AST::Uor::Ptr node) const {
	YAML::Node node_uor;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Uor) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["right"] = render(node->get_right());
	}

	node_uor["Uor"] = content;
	return node_uor;
}



YAML::Node YAMLGenerator::render_unor(const AST::Unor::Ptr node) const {
	YAML::Node node_unor;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Unor) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["right"] = render(node->get_right());
	}

	node_unor["Unor"] = content;
	return node_unor;
}



YAML::Node YAMLGenerator::render_uxor(const AST::Uxor::Ptr node) const {
	YAML::Node node_uxor;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Uxor) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["right"] = render(node->get_right());
	}

	node_uxor["Uxor"] = content;
	return node_uxor;
}



YAML::Node YAMLGenerator::render_uxnor(const AST::Uxnor::Ptr node) const {
	YAML::Node node_uxnor;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Uxnor) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["right"] = render(node->get_right());
	}

	node_uxnor["Uxnor"] = content;
	return node_uxnor;
}



YAML::Node YAMLGenerator::render_operator(const AST::Operator::Ptr node) const {
	YAML::Node node_operator;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Operator) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["left"] = render(node->get_left());

		content["right"] = render(node->get_right());
	}

	node_operator["Operator"] = content;
	return node_operator;
}



YAML::Node YAMLGenerator::render_power(const AST::Power::Ptr node) const {
	YAML::Node node_power;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Power) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["left"] = render(node->get_left());

		content["right"] = render(node->get_right());
	}

	node_power["Power"] = content;
	return node_power;
}



YAML::Node YAMLGenerator::render_times(const AST::Times::Ptr node) const {
	YAML::Node node_times;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Times) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["left"] = render(node->get_left());

		content["right"] = render(node->get_right());
	}

	node_times["Times"] = content;
	return node_times;
}



YAML::Node YAMLGenerator::render_divide(const AST::Divide::Ptr node) const {
	YAML::Node node_divide;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Divide) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["left"] = render(node->get_left());

		content["right"] = render(node->get_right());
	}

	node_divide["Divide"] = content;
	return node_divide;
}



YAML::Node YAMLGenerator::render_mod(const AST::Mod::Ptr node) const {
	YAML::Node node_mod;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Mod) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["left"] = render(node->get_left());

		content["right"] = render(node->get_right());
	}

	node_mod["Mod"] = content;
	return node_mod;
}



YAML::Node YAMLGenerator::render_plus(const AST::Plus::Ptr node) const {
	YAML::Node node_plus;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Plus) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["left"] = render(node->get_left());

		content["right"] = render(node->get_right());
	}

	node_plus["Plus"] = content;
	return node_plus;
}



YAML::Node YAMLGenerator::render_minus(const AST::Minus::Ptr node) const {
	YAML::Node node_minus;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Minus) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["left"] = render(node->get_left());

		content["right"] = render(node->get_right());
	}

	node_minus["Minus"] = content;
	return node_minus;
}



YAML::Node YAMLGenerator::render_sll(const AST::Sll::Ptr node) const {
	YAML::Node node_sll;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Sll) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["left"] = render(node->get_left());

		content["right"] = render(node->get_right());
	}

	node_sll["Sll"] = content;
	return node_sll;
}



YAML::Node YAMLGenerator::render_srl(const AST::Srl::Ptr node) const {
	YAML::Node node_srl;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Srl) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["left"] = render(node->get_left());

		content["right"] = render(node->get_right());
	}

	node_srl["Srl"] = content;
	return node_srl;
}



YAML::Node YAMLGenerator::render_sra(const AST::Sra::Ptr node) const {
	YAML::Node node_sra;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Sra) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["left"] = render(node->get_left());

		content["right"] = render(node->get_right());
	}

	node_sra["Sra"] = content;
	return node_sra;
}



YAML::Node YAMLGenerator::render_lessthan(const AST::LessThan::Ptr node) const {
	YAML::Node node_lessthan;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::LessThan) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["left"] = render(node->get_left());

		content["right"] = render(node->get_right());
	}

	node_lessthan["LessThan"] = content;
	return node_lessthan;
}



YAML::Node YAMLGenerator::render_greaterthan(const AST::GreaterThan::Ptr node) const {
	YAML::Node node_greaterthan;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::GreaterThan) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["left"] = render(node->get_left());

		content["right"] = render(node->get_right());
	}

	node_greaterthan["GreaterThan"] = content;
	return node_greaterthan;
}



YAML::Node YAMLGenerator::render_lesseq(const AST::LessEq::Ptr node) const {
	YAML::Node node_lesseq;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::LessEq) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["left"] = render(node->get_left());

		content["right"] = render(node->get_right());
	}

	node_lesseq["LessEq"] = content;
	return node_lesseq;
}



YAML::Node YAMLGenerator::render_greatereq(const AST::GreaterEq::Ptr node) const {
	YAML::Node node_greatereq;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::GreaterEq) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["left"] = render(node->get_left());

		content["right"] = render(node->get_right());
	}

	node_greatereq["GreaterEq"] = content;
	return node_greatereq;
}



YAML::Node YAMLGenerator::render_eq(const AST::Eq::Ptr node) const {
	YAML::Node node_eq;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Eq) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["left"] = render(node->get_left());

		content["right"] = render(node->get_right());
	}

	node_eq["Eq"] = content;
	return node_eq;
}



YAML::Node YAMLGenerator::render_noteq(const AST::NotEq::Ptr node) const {
	YAML::Node node_noteq;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::NotEq) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["left"] = render(node->get_left());

		content["right"] = render(node->get_right());
	}

	node_noteq["NotEq"] = content;
	return node_noteq;
}



YAML::Node YAMLGenerator::render_eql(const AST::Eql::Ptr node) const {
	YAML::Node node_eql;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Eql) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["left"] = render(node->get_left());

		content["right"] = render(node->get_right());
	}

	node_eql["Eql"] = content;
	return node_eql;
}



YAML::Node YAMLGenerator::render_noteql(const AST::NotEql::Ptr node) const {
	YAML::Node node_noteql;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::NotEql) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["left"] = render(node->get_left());

		content["right"] = render(node->get_right());
	}

	node_noteql["NotEql"] = content;
	return node_noteql;
}



YAML::Node YAMLGenerator::render_and(const AST::And::Ptr node) const {
	YAML::Node node_and;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::And) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["left"] = render(node->get_left());

		content["right"] = render(node->get_right());
	}

	node_and["And"] = content;
	return node_and;
}



YAML::Node YAMLGenerator::render_xor(const AST::Xor::Ptr node) const {
	YAML::Node node_xor;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Xor) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["left"] = render(node->get_left());

		content["right"] = render(node->get_right());
	}

	node_xor["Xor"] = content;
	return node_xor;
}



YAML::Node YAMLGenerator::render_xnor(const AST::Xnor::Ptr node) const {
	YAML::Node node_xnor;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Xnor) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["left"] = render(node->get_left());

		content["right"] = render(node->get_right());
	}

	node_xnor["Xnor"] = content;
	return node_xnor;
}



YAML::Node YAMLGenerator::render_or(const AST::Or::Ptr node) const {
	YAML::Node node_or;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Or) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["left"] = render(node->get_left());

		content["right"] = render(node->get_right());
	}

	node_or["Or"] = content;
	return node_or;
}



YAML::Node YAMLGenerator::render_land(const AST::Land::Ptr node) const {
	YAML::Node node_land;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Land) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["left"] = render(node->get_left());

		content["right"] = render(node->get_right());
	}

	node_land["Land"] = content;
	return node_land;
}



YAML::Node YAMLGenerator::render_lor(const AST::Lor::Ptr node) const {
	YAML::Node node_lor;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Lor) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["left"] = render(node->get_left());

		content["right"] = render(node->get_right());
	}

	node_lor["Lor"] = content;
	return node_lor;
}



YAML::Node YAMLGenerator::render_cond(const AST::Cond::Ptr node) const {
	YAML::Node node_cond;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Cond) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["cond"] = render(node->get_cond());

		content["left"] = render(node->get_left());

		content["right"] = render(node->get_right());
	}

	node_cond["Cond"] = content;
	return node_cond;
}



YAML::Node YAMLGenerator::render_always(const AST::Always::Ptr node) const {
	YAML::Node node_always;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Always) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["senslist"] = render(node->get_senslist());

		content["statement"] = render(node->get_statement());
	}

	node_always["Always"] = content;
	return node_always;
}



YAML::Node YAMLGenerator::render_senslist(const AST::Senslist::Ptr node) const {
	YAML::Node node_senslist;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Senslist) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		if (node->get_list()) {
			content["list"] = YAML::Load("[]");
			for(const AST::Sens::Ptr &n: *node->get_list()) {
				content["list"].push_back(render(n));
			}
		}
	}

	node_senslist["Senslist"] = content;
	return node_senslist;
}



YAML::Node YAMLGenerator::render_sens(const AST::Sens::Ptr node) const {
	YAML::Node node_sens;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Sens) return render(AST::cast_to<AST::Node>(node));

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
		default: content["type"] = "NONE";
		}

		content["sig"] = render(node->get_sig());
	}

	node_sens["Sens"] = content;
	return node_sens;
}



YAML::Node YAMLGenerator::render_defparamlist(const AST::Defparamlist::Ptr node) const {
	YAML::Node node_defparamlist;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Defparamlist) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		if (node->get_list()) {
			content["list"] = YAML::Load("[]");
			for(const AST::Defparam::Ptr &n: *node->get_list()) {
				content["list"].push_back(render(n));
			}
		}
	}

	node_defparamlist["Defparamlist"] = content;
	return node_defparamlist;
}



YAML::Node YAMLGenerator::render_defparam(const AST::Defparam::Ptr node) const {
	YAML::Node node_defparam;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Defparam) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["identifier"] = render(node->get_identifier());

		content["right"] = render(node->get_right());
	}

	node_defparam["Defparam"] = content;
	return node_defparam;
}



YAML::Node YAMLGenerator::render_assign(const AST::Assign::Ptr node) const {
	YAML::Node node_assign;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Assign) return render(AST::cast_to<AST::Node>(node));

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



YAML::Node YAMLGenerator::render_blockingsubstitution(const AST::BlockingSubstitution::Ptr node) const {
	YAML::Node node_blockingsubstitution;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::BlockingSubstitution) return render(AST::cast_to<AST::Node>(node));

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



YAML::Node YAMLGenerator::render_nonblockingsubstitution(const AST::NonblockingSubstitution::Ptr node) const {
	YAML::Node node_nonblockingsubstitution;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::NonblockingSubstitution) return render(AST::cast_to<AST::Node>(node));

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



YAML::Node YAMLGenerator::render_ifstatement(const AST::IfStatement::Ptr node) const {
	YAML::Node node_ifstatement;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::IfStatement) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["cond"] = render(node->get_cond());

		content["true_statement"] = render(node->get_true_statement());

		content["false_statement"] = render(node->get_false_statement());
	}

	node_ifstatement["IfStatement"] = content;
	return node_ifstatement;
}



YAML::Node YAMLGenerator::render_repeatstatement(const AST::RepeatStatement::Ptr node) const {
	YAML::Node node_repeatstatement;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::RepeatStatement) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["times"] = render(node->get_times());

		content["statement"] = render(node->get_statement());
	}

	node_repeatstatement["RepeatStatement"] = content;
	return node_repeatstatement;
}



YAML::Node YAMLGenerator::render_forstatement(const AST::ForStatement::Ptr node) const {
	YAML::Node node_forstatement;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::ForStatement) return render(AST::cast_to<AST::Node>(node));

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



YAML::Node YAMLGenerator::render_whilestatement(const AST::WhileStatement::Ptr node) const {
	YAML::Node node_whilestatement;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::WhileStatement) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["cond"] = render(node->get_cond());

		content["statement"] = render(node->get_statement());
	}

	node_whilestatement["WhileStatement"] = content;
	return node_whilestatement;
}



YAML::Node YAMLGenerator::render_casestatement(const AST::CaseStatement::Ptr node) const {
	YAML::Node node_casestatement;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::CaseStatement) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["comp"] = render(node->get_comp());

		if (node->get_caselist()) {
			content["caselist"] = YAML::Load("[]");
			for(const AST::Case::Ptr &n: *node->get_caselist()) {
				content["caselist"].push_back(render(n));
			}
		}
	}

	node_casestatement["CaseStatement"] = content;
	return node_casestatement;
}



YAML::Node YAMLGenerator::render_casexstatement(const AST::CasexStatement::Ptr node) const {
	YAML::Node node_casexstatement;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::CasexStatement) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["comp"] = render(node->get_comp());

		if (node->get_caselist()) {
			content["caselist"] = YAML::Load("[]");
			for(const AST::Case::Ptr &n: *node->get_caselist()) {
				content["caselist"].push_back(render(n));
			}
		}
	}

	node_casexstatement["CasexStatement"] = content;
	return node_casexstatement;
}



YAML::Node YAMLGenerator::render_casezstatement(const AST::CasezStatement::Ptr node) const {
	YAML::Node node_casezstatement;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::CasezStatement) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["comp"] = render(node->get_comp());

		if (node->get_caselist()) {
			content["caselist"] = YAML::Load("[]");
			for(const AST::Case::Ptr &n: *node->get_caselist()) {
				content["caselist"].push_back(render(n));
			}
		}
	}

	node_casezstatement["CasezStatement"] = content;
	return node_casezstatement;
}



YAML::Node YAMLGenerator::render_case(const AST::Case::Ptr node) const {
	YAML::Node node_case;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Case) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		if (node->get_cond()) {
			content["cond"] = YAML::Load("[]");
			for(const AST::Node::Ptr &n: *node->get_cond()) {
				content["cond"].push_back(render(n));
			}
		}

		content["statement"] = render(node->get_statement());
	}

	node_case["Case"] = content;
	return node_case;
}



YAML::Node YAMLGenerator::render_block(const AST::Block::Ptr node) const {
	YAML::Node node_block;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Block) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();
		content["scope"] = node->get_scope();

		if (node->get_statements()) {
			content["statements"] = YAML::Load("[]");
			for(const AST::Node::Ptr &n: *node->get_statements()) {
				content["statements"].push_back(render(n));
			}
		}
	}

	node_block["Block"] = content;
	return node_block;
}



YAML::Node YAMLGenerator::render_initial(const AST::Initial::Ptr node) const {
	YAML::Node node_initial;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Initial) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["statement"] = render(node->get_statement());
	}

	node_initial["Initial"] = content;
	return node_initial;
}



YAML::Node YAMLGenerator::render_eventstatement(const AST::EventStatement::Ptr node) const {
	YAML::Node node_eventstatement;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::EventStatement) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["senslist"] = render(node->get_senslist());

		content["statement"] = render(node->get_statement());
	}

	node_eventstatement["EventStatement"] = content;
	return node_eventstatement;
}



YAML::Node YAMLGenerator::render_waitstatement(const AST::WaitStatement::Ptr node) const {
	YAML::Node node_waitstatement;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::WaitStatement) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["cond"] = render(node->get_cond());

		content["statement"] = render(node->get_statement());
	}

	node_waitstatement["WaitStatement"] = content;
	return node_waitstatement;
}



YAML::Node YAMLGenerator::render_foreverstatement(const AST::ForeverStatement::Ptr node) const {
	YAML::Node node_foreverstatement;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::ForeverStatement) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["statement"] = render(node->get_statement());
	}

	node_foreverstatement["ForeverStatement"] = content;
	return node_foreverstatement;
}



YAML::Node YAMLGenerator::render_delaystatement(const AST::DelayStatement::Ptr node) const {
	YAML::Node node_delaystatement;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::DelayStatement) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		content["delay"] = render(node->get_delay());
	}

	node_delaystatement["DelayStatement"] = content;
	return node_delaystatement;
}



YAML::Node YAMLGenerator::render_instancelist(const AST::Instancelist::Ptr node) const {
	YAML::Node node_instancelist;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Instancelist) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();
		content["module"] = node->get_module();

		if (node->get_parameterlist()) {
			content["parameterlist"] = YAML::Load("[]");
			for(const AST::ParamArg::Ptr &n: *node->get_parameterlist()) {
				content["parameterlist"].push_back(render(n));
			}
		}

		if (node->get_instances()) {
			content["instances"] = YAML::Load("[]");
			for(const AST::Instance::Ptr &n: *node->get_instances()) {
				content["instances"].push_back(render(n));
			}
		}
	}

	node_instancelist["Instancelist"] = content;
	return node_instancelist;
}



YAML::Node YAMLGenerator::render_instance(const AST::Instance::Ptr node) const {
	YAML::Node node_instance;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Instance) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();
		content["module"] = node->get_module();
		content["name"] = node->get_name();

		content["array"] = render(node->get_array());

		if (node->get_parameterlist()) {
			content["parameterlist"] = YAML::Load("[]");
			for(const AST::ParamArg::Ptr &n: *node->get_parameterlist()) {
				content["parameterlist"].push_back(render(n));
			}
		}

		if (node->get_portlist()) {
			content["portlist"] = YAML::Load("[]");
			for(const AST::PortArg::Ptr &n: *node->get_portlist()) {
				content["portlist"].push_back(render(n));
			}
		}
	}

	node_instance["Instance"] = content;
	return node_instance;
}



YAML::Node YAMLGenerator::render_paramarg(const AST::ParamArg::Ptr node) const {
	YAML::Node node_paramarg;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::ParamArg) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();
		content["name"] = node->get_name();

		content["value"] = render(node->get_value());
	}

	node_paramarg["ParamArg"] = content;
	return node_paramarg;
}



YAML::Node YAMLGenerator::render_portarg(const AST::PortArg::Ptr node) const {
	YAML::Node node_portarg;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::PortArg) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();
		content["name"] = node->get_name();

		content["value"] = render(node->get_value());
	}

	node_portarg["PortArg"] = content;
	return node_portarg;
}



YAML::Node YAMLGenerator::render_function(const AST::Function::Ptr node) const {
	YAML::Node node_function;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Function) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();
		content["name"] = node->get_name();
		content["automatic"] = node->get_automatic();
		switch(node->get_rettype()) {
		case Veriparse::AST::Function::RettypeEnum::INTEGER:
			content["rettype"] = "INTEGER";
			break;
		case Veriparse::AST::Function::RettypeEnum::REAL:
			content["rettype"] = "REAL";
			break;
		default: content["rettype"] = "NONE";
		}
		content["retsign"] = node->get_retsign();

		if (node->get_retwidths()) {
			content["retwidths"] = YAML::Load("[]");
			for(const AST::Width::Ptr &n: *node->get_retwidths()) {
				content["retwidths"].push_back(render(n));
			}
		}

		if (node->get_ports()) {
			content["ports"] = YAML::Load("[]");
			for(const AST::Node::Ptr &n: *node->get_ports()) {
				content["ports"].push_back(render(n));
			}
		}

		if (node->get_statements()) {
			content["statements"] = YAML::Load("[]");
			for(const AST::Node::Ptr &n: *node->get_statements()) {
				content["statements"].push_back(render(n));
			}
		}
	}

	node_function["Function"] = content;
	return node_function;
}



YAML::Node YAMLGenerator::render_functioncall(const AST::FunctionCall::Ptr node) const {
	YAML::Node node_functioncall;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::FunctionCall) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();
		content["name"] = node->get_name();

		if (node->get_args()) {
			content["args"] = YAML::Load("[]");
			for(const AST::Node::Ptr &n: *node->get_args()) {
				content["args"].push_back(render(n));
			}
		}
	}

	node_functioncall["FunctionCall"] = content;
	return node_functioncall;
}



YAML::Node YAMLGenerator::render_task(const AST::Task::Ptr node) const {
	YAML::Node node_task;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Task) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();
		content["name"] = node->get_name();
		content["automatic"] = node->get_automatic();

		if (node->get_ports()) {
			content["ports"] = YAML::Load("[]");
			for(const AST::Node::Ptr &n: *node->get_ports()) {
				content["ports"].push_back(render(n));
			}
		}

		if (node->get_statements()) {
			content["statements"] = YAML::Load("[]");
			for(const AST::Node::Ptr &n: *node->get_statements()) {
				content["statements"].push_back(render(n));
			}
		}
	}

	node_task["Task"] = content;
	return node_task;
}



YAML::Node YAMLGenerator::render_taskcall(const AST::TaskCall::Ptr node) const {
	YAML::Node node_taskcall;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::TaskCall) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();
		content["name"] = node->get_name();

		if (node->get_args()) {
			content["args"] = YAML::Load("[]");
			for(const AST::Node::Ptr &n: *node->get_args()) {
				content["args"].push_back(render(n));
			}
		}
	}

	node_taskcall["TaskCall"] = content;
	return node_taskcall;
}



YAML::Node YAMLGenerator::render_generatestatement(const AST::GenerateStatement::Ptr node) const {
	YAML::Node node_generatestatement;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::GenerateStatement) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		if (node->get_items()) {
			content["items"] = YAML::Load("[]");
			for(const AST::Node::Ptr &n: *node->get_items()) {
				content["items"].push_back(render(n));
			}
		}
	}

	node_generatestatement["GenerateStatement"] = content;
	return node_generatestatement;
}



YAML::Node YAMLGenerator::render_systemcall(const AST::SystemCall::Ptr node) const {
	YAML::Node node_systemcall;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::SystemCall) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();
		content["syscall"] = node->get_syscall();

		if (node->get_args()) {
			content["args"] = YAML::Load("[]");
			for(const AST::Node::Ptr &n: *node->get_args()) {
				content["args"].push_back(render(n));
			}
		}
	}

	node_systemcall["SystemCall"] = content;
	return node_systemcall;
}



YAML::Node YAMLGenerator::render_identifierscopelabel(const AST::IdentifierScopeLabel::Ptr node) const {
	YAML::Node node_identifierscopelabel;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::IdentifierScopeLabel) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();
		content["scope"] = node->get_scope();

		content["loop"] = render(node->get_loop());
	}

	node_identifierscopelabel["IdentifierScopeLabel"] = content;
	return node_identifierscopelabel;
}



YAML::Node YAMLGenerator::render_identifierscope(const AST::IdentifierScope::Ptr node) const {
	YAML::Node node_identifierscope;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::IdentifierScope) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();

		if (node->get_labellist()) {
			content["labellist"] = YAML::Load("[]");
			for(const AST::IdentifierScopeLabel::Ptr &n: *node->get_labellist()) {
				content["labellist"].push_back(render(n));
			}
		}
	}

	node_identifierscope["IdentifierScope"] = content;
	return node_identifierscope;
}



YAML::Node YAMLGenerator::render_disable(const AST::Disable::Ptr node) const {
	YAML::Node node_disable;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::Disable) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();
		content["dest"] = node->get_dest();
	}

	node_disable["Disable"] = content;
	return node_disable;
}



YAML::Node YAMLGenerator::render_parallelblock(const AST::ParallelBlock::Ptr node) const {
	YAML::Node node_parallelblock;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::ParallelBlock) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();
		content["scope"] = node->get_scope();

		if (node->get_statements()) {
			content["statements"] = YAML::Load("[]");
			for(const AST::Node::Ptr &n: *node->get_statements()) {
				content["statements"].push_back(render(n));
			}
		}
	}

	node_parallelblock["ParallelBlock"] = content;
	return node_parallelblock;
}



YAML::Node YAMLGenerator::render_singlestatement(const AST::SingleStatement::Ptr node) const {
	YAML::Node node_singlestatement;
	YAML::Node content;

	if (node) {
		if (node->get_node_type() != AST::NodeType::SingleStatement) return render(AST::cast_to<AST::Node>(node));

		content["filename"] = node->get_filename();
		content["line"] = node->get_line();
		content["scope"] = node->get_scope();

		content["statement"] = render(node->get_statement());

		content["delay"] = render(node->get_delay());
	}

	node_singlestatement["SingleStatement"] = content;
	return node_singlestatement;
}


}
}