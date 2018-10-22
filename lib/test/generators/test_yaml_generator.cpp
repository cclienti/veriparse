#include <gtest/gtest.h>
#include <yaml-cpp/yaml.h>
#include <veriparse/generators/yaml_generator.hpp>
#include <veriparse/importers/yaml_specializations.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

using namespace Veriparse;


TEST(YAMLGenerator, Source) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Source.log");
	Logger::add_stdout_sink();

	
	AST::Description::Ptr c_description(new AST::Description);

	AST::Source::Ptr m_source(new AST::Source
		(c_description, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_source);

	ASSERT_TRUE(yaml["Source"]);
	ASSERT_TRUE(yaml["Source"]["description"]);
}

TEST(YAMLGenerator, Description) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Description.log");
	Logger::add_stdout_sink();

	
	AST::Node::ListPtr c_definitions(new AST::Node::List);

	AST::Description::Ptr m_description(new AST::Description
		(c_definitions, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_description);

	ASSERT_TRUE(yaml["Description"]);
	ASSERT_TRUE(yaml["Description"]["definitions"]);
}

TEST(YAMLGenerator, Pragma) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Pragma.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_expression(new AST::Node);
	std::string p_name = "mynbiqpmzj";

	AST::Pragma::Ptr m_pragma(new AST::Pragma
		(c_expression, p_name, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_pragma);

	ASSERT_TRUE(yaml["Pragma"]);
	ASSERT_TRUE(yaml["Pragma"]["expression"]);
	ASSERT_TRUE(yaml["Pragma"]["name"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLGenerator, Module) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Module.log");
	Logger::add_stdout_sink();

	
	AST::Parameter::ListPtr c_params(new AST::Parameter::List);
	AST::Node::ListPtr c_ports(new AST::Node::List);
	AST::Node::ListPtr c_items(new AST::Node::List);
	std::string p_name = "mynbiqpmzj";
	AST::Module::Default_nettypeEnum p_default_nettype = AST::Module::Default_nettypeEnum::NONE;

	AST::Module::Ptr m_module(new AST::Module
		(c_params, c_ports, c_items, p_name, p_default_nettype, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_module);

	ASSERT_TRUE(yaml["Module"]);
	ASSERT_TRUE(yaml["Module"]["params"]);
	ASSERT_TRUE(yaml["Module"]["ports"]);
	ASSERT_TRUE(yaml["Module"]["items"]);
	ASSERT_TRUE(yaml["Module"]["name"].as<std::string>() == "mynbiqpmzj");
	ASSERT_TRUE(yaml["Module"]["default_nettype"].as<AST::Module::Default_nettypeEnum>() == AST::Module::Default_nettypeEnum::NONE);
}

TEST(YAMLGenerator, Port) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Port.log");
	Logger::add_stdout_sink();

	
	AST::Width::ListPtr c_widths(new AST::Width::List);
	std::string p_name = "mynbiqpmzj";

	AST::Port::Ptr m_port(new AST::Port
		(c_widths, p_name, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_port);

	ASSERT_TRUE(yaml["Port"]);
	ASSERT_TRUE(yaml["Port"]["widths"]);
	ASSERT_TRUE(yaml["Port"]["name"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLGenerator, Width) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Width.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_msb(new AST::Node);
	AST::Node::Ptr c_lsb(new AST::Node);

	AST::Width::Ptr m_width(new AST::Width
		(c_msb, c_lsb, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_width);

	ASSERT_TRUE(yaml["Width"]);
	ASSERT_TRUE(yaml["Width"]["msb"]);
	ASSERT_TRUE(yaml["Width"]["lsb"]);
}

TEST(YAMLGenerator, Length) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Length.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_msb(new AST::Node);
	AST::Node::Ptr c_lsb(new AST::Node);

	AST::Length::Ptr m_length(new AST::Length
		(c_msb, c_lsb, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_length);

	ASSERT_TRUE(yaml["Length"]);
	ASSERT_TRUE(yaml["Length"]["msb"]);
	ASSERT_TRUE(yaml["Length"]["lsb"]);
}

TEST(YAMLGenerator, Identifier) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Identifier.log");
	Logger::add_stdout_sink();

	
	AST::IdentifierScope::Ptr c_scope(new AST::IdentifierScope);
	std::string p_name = "mynbiqpmzj";

	AST::Identifier::Ptr m_identifier(new AST::Identifier
		(c_scope, p_name, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_identifier);

	ASSERT_TRUE(yaml["Identifier"]);
	ASSERT_TRUE(yaml["Identifier"]["scope"]);
	ASSERT_TRUE(yaml["Identifier"]["name"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLGenerator, Constant) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Constant.log");
	Logger::add_stdout_sink();

	

	AST::Constant::Ptr m_constant(new AST::Constant
		("filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_constant);

	ASSERT_TRUE(yaml["Constant"]);
}

TEST(YAMLGenerator, StringConst) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.StringConst.log");
	Logger::add_stdout_sink();

	
	std::string p_value = "mynbiqpmzj";

	AST::StringConst::Ptr m_stringconst(new AST::StringConst
		(p_value, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_stringconst);

	ASSERT_TRUE(yaml["StringConst"]);
	ASSERT_TRUE(yaml["StringConst"]["value"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLGenerator, IntConst) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.IntConst.log");
	Logger::add_stdout_sink();

	
	std::string p_value = "mynbiqpmzj";

	AST::IntConst::Ptr m_intconst(new AST::IntConst
		(p_value, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_intconst);

	ASSERT_TRUE(yaml["IntConst"]);
	ASSERT_TRUE(yaml["IntConst"]["value"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLGenerator, IntConstN) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.IntConstN.log");
	Logger::add_stdout_sink();

	
	int p_base = 98;
	int p_size = 107;
	bool p_sign = true;
	mpz_class p_value = 66;

	AST::IntConstN::Ptr m_intconstn(new AST::IntConstN
		(p_base, p_size, p_sign, p_value, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_intconstn);

	ASSERT_TRUE(yaml["IntConstN"]);
	ASSERT_TRUE(yaml["IntConstN"]["base"].as<int>() == 98);
	ASSERT_TRUE(yaml["IntConstN"]["size"].as<int>() == 107);
	ASSERT_TRUE(yaml["IntConstN"]["sign"].as<bool>() == true);
	ASSERT_TRUE(yaml["IntConstN"]["value"].as<mpz_class>() == 66);
}

TEST(YAMLGenerator, FloatConst) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.FloatConst.log");
	Logger::add_stdout_sink();

	
	double p_value = 98;

	AST::FloatConst::Ptr m_floatconst(new AST::FloatConst
		(p_value, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_floatconst);

	ASSERT_TRUE(yaml["FloatConst"]);
	ASSERT_TRUE(yaml["FloatConst"]["value"].as<double>() == 98);
}

TEST(YAMLGenerator, IODir) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.IODir.log");
	Logger::add_stdout_sink();

	
	AST::Width::ListPtr c_widths(new AST::Width::List);
	std::string p_name = "mynbiqpmzj";
	bool p_sign = false;

	AST::IODir::Ptr m_iodir(new AST::IODir
		(c_widths, p_name, p_sign, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_iodir);

	ASSERT_TRUE(yaml["IODir"]);
	ASSERT_TRUE(yaml["IODir"]["widths"]);
	ASSERT_TRUE(yaml["IODir"]["name"].as<std::string>() == "mynbiqpmzj");
	ASSERT_TRUE(yaml["IODir"]["sign"].as<bool>() == false);
}

TEST(YAMLGenerator, Input) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Input.log");
	Logger::add_stdout_sink();

	
	AST::Width::ListPtr c_widths(new AST::Width::List);
	std::string p_name = "mynbiqpmzj";
	bool p_sign = false;

	AST::Input::Ptr m_input(new AST::Input
		(c_widths, p_name, p_sign, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_input);

	ASSERT_TRUE(yaml["Input"]);
	ASSERT_TRUE(yaml["Input"]["widths"]);
	ASSERT_TRUE(yaml["Input"]["name"].as<std::string>() == "mynbiqpmzj");
	ASSERT_TRUE(yaml["Input"]["sign"].as<bool>() == false);
}

TEST(YAMLGenerator, Output) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Output.log");
	Logger::add_stdout_sink();

	
	AST::Width::ListPtr c_widths(new AST::Width::List);
	std::string p_name = "mynbiqpmzj";
	bool p_sign = false;

	AST::Output::Ptr m_output(new AST::Output
		(c_widths, p_name, p_sign, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_output);

	ASSERT_TRUE(yaml["Output"]);
	ASSERT_TRUE(yaml["Output"]["widths"]);
	ASSERT_TRUE(yaml["Output"]["name"].as<std::string>() == "mynbiqpmzj");
	ASSERT_TRUE(yaml["Output"]["sign"].as<bool>() == false);
}

TEST(YAMLGenerator, Inout) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Inout.log");
	Logger::add_stdout_sink();

	
	AST::Width::ListPtr c_widths(new AST::Width::List);
	std::string p_name = "mynbiqpmzj";
	bool p_sign = false;

	AST::Inout::Ptr m_inout(new AST::Inout
		(c_widths, p_name, p_sign, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_inout);

	ASSERT_TRUE(yaml["Inout"]);
	ASSERT_TRUE(yaml["Inout"]["widths"]);
	ASSERT_TRUE(yaml["Inout"]["name"].as<std::string>() == "mynbiqpmzj");
	ASSERT_TRUE(yaml["Inout"]["sign"].as<bool>() == false);
}

TEST(YAMLGenerator, VariableBase) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.VariableBase.log");
	Logger::add_stdout_sink();

	
	std::string p_name = "mynbiqpmzj";

	AST::VariableBase::Ptr m_variablebase(new AST::VariableBase
		(p_name, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_variablebase);

	ASSERT_TRUE(yaml["VariableBase"]);
	ASSERT_TRUE(yaml["VariableBase"]["name"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLGenerator, Genvar) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Genvar.log");
	Logger::add_stdout_sink();

	
	std::string p_name = "mynbiqpmzj";

	AST::Genvar::Ptr m_genvar(new AST::Genvar
		(p_name, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_genvar);

	ASSERT_TRUE(yaml["Genvar"]);
	ASSERT_TRUE(yaml["Genvar"]["name"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLGenerator, Variable) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Variable.log");
	Logger::add_stdout_sink();

	
	AST::Length::ListPtr c_lengths(new AST::Length::List);
	AST::Rvalue::Ptr c_right(new AST::Rvalue);
	std::string p_name = "mynbiqpmzj";

	AST::Variable::Ptr m_variable(new AST::Variable
		(c_lengths, c_right, p_name, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_variable);

	ASSERT_TRUE(yaml["Variable"]);
	ASSERT_TRUE(yaml["Variable"]["lengths"]);
	ASSERT_TRUE(yaml["Variable"]["right"]);
	ASSERT_TRUE(yaml["Variable"]["name"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLGenerator, Net) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Net.log");
	Logger::add_stdout_sink();

	
	AST::Width::ListPtr c_widths(new AST::Width::List);
	AST::DelayStatement::Ptr c_ldelay(new AST::DelayStatement);
	AST::DelayStatement::Ptr c_rdelay(new AST::DelayStatement);
	AST::Length::ListPtr c_lengths(new AST::Length::List);
	AST::Rvalue::Ptr c_right(new AST::Rvalue);
	bool p_sign = false;
	std::string p_name = "ynbiqpmzjp";

	AST::Net::Ptr m_net(new AST::Net
		(c_widths, c_ldelay, c_rdelay, c_lengths, c_right, p_sign, p_name, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_net);

	ASSERT_TRUE(yaml["Net"]);
	ASSERT_TRUE(yaml["Net"]["widths"]);
	ASSERT_TRUE(yaml["Net"]["ldelay"]);
	ASSERT_TRUE(yaml["Net"]["rdelay"]);
	ASSERT_TRUE(yaml["Net"]["lengths"]);
	ASSERT_TRUE(yaml["Net"]["right"]);
	ASSERT_TRUE(yaml["Net"]["sign"].as<bool>() == false);
	ASSERT_TRUE(yaml["Net"]["name"].as<std::string>() == "ynbiqpmzjp");
}

TEST(YAMLGenerator, Integer) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Integer.log");
	Logger::add_stdout_sink();

	
	AST::Length::ListPtr c_lengths(new AST::Length::List);
	AST::Rvalue::Ptr c_right(new AST::Rvalue);
	std::string p_name = "mynbiqpmzj";

	AST::Integer::Ptr m_integer(new AST::Integer
		(c_lengths, c_right, p_name, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_integer);

	ASSERT_TRUE(yaml["Integer"]);
	ASSERT_TRUE(yaml["Integer"]["lengths"]);
	ASSERT_TRUE(yaml["Integer"]["right"]);
	ASSERT_TRUE(yaml["Integer"]["name"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLGenerator, Real) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Real.log");
	Logger::add_stdout_sink();

	
	AST::Length::ListPtr c_lengths(new AST::Length::List);
	AST::Rvalue::Ptr c_right(new AST::Rvalue);
	std::string p_name = "mynbiqpmzj";

	AST::Real::Ptr m_real(new AST::Real
		(c_lengths, c_right, p_name, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_real);

	ASSERT_TRUE(yaml["Real"]);
	ASSERT_TRUE(yaml["Real"]["lengths"]);
	ASSERT_TRUE(yaml["Real"]["right"]);
	ASSERT_TRUE(yaml["Real"]["name"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLGenerator, Tri) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Tri.log");
	Logger::add_stdout_sink();

	
	AST::Width::ListPtr c_widths(new AST::Width::List);
	AST::DelayStatement::Ptr c_ldelay(new AST::DelayStatement);
	AST::DelayStatement::Ptr c_rdelay(new AST::DelayStatement);
	AST::Length::ListPtr c_lengths(new AST::Length::List);
	AST::Rvalue::Ptr c_right(new AST::Rvalue);
	bool p_sign = false;
	std::string p_name = "ynbiqpmzjp";

	AST::Tri::Ptr m_tri(new AST::Tri
		(c_widths, c_ldelay, c_rdelay, c_lengths, c_right, p_sign, p_name, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_tri);

	ASSERT_TRUE(yaml["Tri"]);
	ASSERT_TRUE(yaml["Tri"]["widths"]);
	ASSERT_TRUE(yaml["Tri"]["ldelay"]);
	ASSERT_TRUE(yaml["Tri"]["rdelay"]);
	ASSERT_TRUE(yaml["Tri"]["lengths"]);
	ASSERT_TRUE(yaml["Tri"]["right"]);
	ASSERT_TRUE(yaml["Tri"]["sign"].as<bool>() == false);
	ASSERT_TRUE(yaml["Tri"]["name"].as<std::string>() == "ynbiqpmzjp");
}

TEST(YAMLGenerator, Wire) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Wire.log");
	Logger::add_stdout_sink();

	
	AST::Width::ListPtr c_widths(new AST::Width::List);
	AST::DelayStatement::Ptr c_ldelay(new AST::DelayStatement);
	AST::DelayStatement::Ptr c_rdelay(new AST::DelayStatement);
	AST::Length::ListPtr c_lengths(new AST::Length::List);
	AST::Rvalue::Ptr c_right(new AST::Rvalue);
	bool p_sign = false;
	std::string p_name = "ynbiqpmzjp";

	AST::Wire::Ptr m_wire(new AST::Wire
		(c_widths, c_ldelay, c_rdelay, c_lengths, c_right, p_sign, p_name, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_wire);

	ASSERT_TRUE(yaml["Wire"]);
	ASSERT_TRUE(yaml["Wire"]["widths"]);
	ASSERT_TRUE(yaml["Wire"]["ldelay"]);
	ASSERT_TRUE(yaml["Wire"]["rdelay"]);
	ASSERT_TRUE(yaml["Wire"]["lengths"]);
	ASSERT_TRUE(yaml["Wire"]["right"]);
	ASSERT_TRUE(yaml["Wire"]["sign"].as<bool>() == false);
	ASSERT_TRUE(yaml["Wire"]["name"].as<std::string>() == "ynbiqpmzjp");
}

TEST(YAMLGenerator, Supply0) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Supply0.log");
	Logger::add_stdout_sink();

	
	AST::Width::ListPtr c_widths(new AST::Width::List);
	AST::DelayStatement::Ptr c_ldelay(new AST::DelayStatement);
	AST::DelayStatement::Ptr c_rdelay(new AST::DelayStatement);
	AST::Length::ListPtr c_lengths(new AST::Length::List);
	AST::Rvalue::Ptr c_right(new AST::Rvalue);
	bool p_sign = false;
	std::string p_name = "ynbiqpmzjp";

	AST::Supply0::Ptr m_supply0(new AST::Supply0
		(c_widths, c_ldelay, c_rdelay, c_lengths, c_right, p_sign, p_name, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_supply0);

	ASSERT_TRUE(yaml["Supply0"]);
	ASSERT_TRUE(yaml["Supply0"]["widths"]);
	ASSERT_TRUE(yaml["Supply0"]["ldelay"]);
	ASSERT_TRUE(yaml["Supply0"]["rdelay"]);
	ASSERT_TRUE(yaml["Supply0"]["lengths"]);
	ASSERT_TRUE(yaml["Supply0"]["right"]);
	ASSERT_TRUE(yaml["Supply0"]["sign"].as<bool>() == false);
	ASSERT_TRUE(yaml["Supply0"]["name"].as<std::string>() == "ynbiqpmzjp");
}

TEST(YAMLGenerator, Supply1) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Supply1.log");
	Logger::add_stdout_sink();

	
	AST::Width::ListPtr c_widths(new AST::Width::List);
	AST::DelayStatement::Ptr c_ldelay(new AST::DelayStatement);
	AST::DelayStatement::Ptr c_rdelay(new AST::DelayStatement);
	AST::Length::ListPtr c_lengths(new AST::Length::List);
	AST::Rvalue::Ptr c_right(new AST::Rvalue);
	bool p_sign = false;
	std::string p_name = "ynbiqpmzjp";

	AST::Supply1::Ptr m_supply1(new AST::Supply1
		(c_widths, c_ldelay, c_rdelay, c_lengths, c_right, p_sign, p_name, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_supply1);

	ASSERT_TRUE(yaml["Supply1"]);
	ASSERT_TRUE(yaml["Supply1"]["widths"]);
	ASSERT_TRUE(yaml["Supply1"]["ldelay"]);
	ASSERT_TRUE(yaml["Supply1"]["rdelay"]);
	ASSERT_TRUE(yaml["Supply1"]["lengths"]);
	ASSERT_TRUE(yaml["Supply1"]["right"]);
	ASSERT_TRUE(yaml["Supply1"]["sign"].as<bool>() == false);
	ASSERT_TRUE(yaml["Supply1"]["name"].as<std::string>() == "ynbiqpmzjp");
}

TEST(YAMLGenerator, Reg) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Reg.log");
	Logger::add_stdout_sink();

	
	AST::Width::ListPtr c_widths(new AST::Width::List);
	AST::Length::ListPtr c_lengths(new AST::Length::List);
	AST::Rvalue::Ptr c_right(new AST::Rvalue);
	bool p_sign = false;
	std::string p_name = "ynbiqpmzjp";

	AST::Reg::Ptr m_reg(new AST::Reg
		(c_widths, c_lengths, c_right, p_sign, p_name, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_reg);

	ASSERT_TRUE(yaml["Reg"]);
	ASSERT_TRUE(yaml["Reg"]["widths"]);
	ASSERT_TRUE(yaml["Reg"]["lengths"]);
	ASSERT_TRUE(yaml["Reg"]["right"]);
	ASSERT_TRUE(yaml["Reg"]["sign"].as<bool>() == false);
	ASSERT_TRUE(yaml["Reg"]["name"].as<std::string>() == "ynbiqpmzjp");
}

TEST(YAMLGenerator, Ioport) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Ioport.log");
	Logger::add_stdout_sink();

	
	AST::IODir::Ptr c_first(new AST::IODir);
	AST::Variable::Ptr c_second(new AST::Variable);

	AST::Ioport::Ptr m_ioport(new AST::Ioport
		(c_first, c_second, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_ioport);

	ASSERT_TRUE(yaml["Ioport"]);
	ASSERT_TRUE(yaml["Ioport"]["first"]);
	ASSERT_TRUE(yaml["Ioport"]["second"]);
}

TEST(YAMLGenerator, Parameter) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Parameter.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_value(new AST::Node);
	AST::Width::ListPtr c_widths(new AST::Width::List);
	std::string p_name = "mynbiqpmzj";
	bool p_sign = false;
	AST::Parameter::TypeEnum p_type = AST::Parameter::TypeEnum::REAL;

	AST::Parameter::Ptr m_parameter(new AST::Parameter
		(c_value, c_widths, p_name, p_sign, p_type, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_parameter);

	ASSERT_TRUE(yaml["Parameter"]);
	ASSERT_TRUE(yaml["Parameter"]["value"]);
	ASSERT_TRUE(yaml["Parameter"]["widths"]);
	ASSERT_TRUE(yaml["Parameter"]["name"].as<std::string>() == "mynbiqpmzj");
	ASSERT_TRUE(yaml["Parameter"]["sign"].as<bool>() == false);
	ASSERT_TRUE(yaml["Parameter"]["type"].as<AST::Parameter::TypeEnum>() == AST::Parameter::TypeEnum::REAL);
}

TEST(YAMLGenerator, Localparam) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Localparam.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_value(new AST::Node);
	AST::Width::ListPtr c_widths(new AST::Width::List);
	std::string p_name = "mynbiqpmzj";
	bool p_sign = false;
	AST::Localparam::Parameter::TypeEnum p_type = AST::Localparam::Parameter::TypeEnum::REAL;

	AST::Localparam::Ptr m_localparam(new AST::Localparam
		(c_value, c_widths, p_name, p_sign, p_type, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_localparam);

	ASSERT_TRUE(yaml["Localparam"]);
	ASSERT_TRUE(yaml["Localparam"]["value"]);
	ASSERT_TRUE(yaml["Localparam"]["widths"]);
	ASSERT_TRUE(yaml["Localparam"]["name"].as<std::string>() == "mynbiqpmzj");
	ASSERT_TRUE(yaml["Localparam"]["sign"].as<bool>() == false);
	ASSERT_TRUE(yaml["Localparam"]["type"].as<AST::Localparam::Parameter::TypeEnum>() == AST::Localparam::Parameter::TypeEnum::REAL);
}

TEST(YAMLGenerator, Concat) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Concat.log");
	Logger::add_stdout_sink();

	
	AST::Node::ListPtr c_list(new AST::Node::List);

	AST::Concat::Ptr m_concat(new AST::Concat
		(c_list, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_concat);

	ASSERT_TRUE(yaml["Concat"]);
	ASSERT_TRUE(yaml["Concat"]["list"]);
}

TEST(YAMLGenerator, Lconcat) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Lconcat.log");
	Logger::add_stdout_sink();

	
	AST::Node::ListPtr c_list(new AST::Node::List);

	AST::Lconcat::Ptr m_lconcat(new AST::Lconcat
		(c_list, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_lconcat);

	ASSERT_TRUE(yaml["Lconcat"]);
	ASSERT_TRUE(yaml["Lconcat"]["list"]);
}

TEST(YAMLGenerator, Repeat) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Repeat.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_value(new AST::Node);
	AST::Node::Ptr c_times(new AST::Node);

	AST::Repeat::Ptr m_repeat(new AST::Repeat
		(c_value, c_times, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_repeat);

	ASSERT_TRUE(yaml["Repeat"]);
	ASSERT_TRUE(yaml["Repeat"]["value"]);
	ASSERT_TRUE(yaml["Repeat"]["times"]);
}

TEST(YAMLGenerator, Indirect) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Indirect.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_var(new AST::Node);

	AST::Indirect::Ptr m_indirect(new AST::Indirect
		(c_var, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_indirect);

	ASSERT_TRUE(yaml["Indirect"]);
	ASSERT_TRUE(yaml["Indirect"]["var"]);
}

TEST(YAMLGenerator, Partselect) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Partselect.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_msb(new AST::Node);
	AST::Node::Ptr c_lsb(new AST::Node);
	AST::Node::Ptr c_var(new AST::Node);

	AST::Partselect::Ptr m_partselect(new AST::Partselect
		(c_msb, c_lsb, c_var, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_partselect);

	ASSERT_TRUE(yaml["Partselect"]);
	ASSERT_TRUE(yaml["Partselect"]["msb"]);
	ASSERT_TRUE(yaml["Partselect"]["lsb"]);
	ASSERT_TRUE(yaml["Partselect"]["var"]);
}

TEST(YAMLGenerator, PartselectIndexed) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.PartselectIndexed.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_index(new AST::Node);
	AST::Node::Ptr c_size(new AST::Node);
	AST::Node::Ptr c_var(new AST::Node);

	AST::PartselectIndexed::Ptr m_partselectindexed(new AST::PartselectIndexed
		(c_index, c_size, c_var, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_partselectindexed);

	ASSERT_TRUE(yaml["PartselectIndexed"]);
	ASSERT_TRUE(yaml["PartselectIndexed"]["index"]);
	ASSERT_TRUE(yaml["PartselectIndexed"]["size"]);
	ASSERT_TRUE(yaml["PartselectIndexed"]["var"]);
}

TEST(YAMLGenerator, PartselectPlusIndexed) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.PartselectPlusIndexed.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_index(new AST::Node);
	AST::Node::Ptr c_size(new AST::Node);
	AST::Node::Ptr c_var(new AST::Node);

	AST::PartselectPlusIndexed::Ptr m_partselectplusindexed(new AST::PartselectPlusIndexed
		(c_index, c_size, c_var, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_partselectplusindexed);

	ASSERT_TRUE(yaml["PartselectPlusIndexed"]);
	ASSERT_TRUE(yaml["PartselectPlusIndexed"]["index"]);
	ASSERT_TRUE(yaml["PartselectPlusIndexed"]["size"]);
	ASSERT_TRUE(yaml["PartselectPlusIndexed"]["var"]);
}

TEST(YAMLGenerator, PartselectMinusIndexed) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.PartselectMinusIndexed.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_index(new AST::Node);
	AST::Node::Ptr c_size(new AST::Node);
	AST::Node::Ptr c_var(new AST::Node);

	AST::PartselectMinusIndexed::Ptr m_partselectminusindexed(new AST::PartselectMinusIndexed
		(c_index, c_size, c_var, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_partselectminusindexed);

	ASSERT_TRUE(yaml["PartselectMinusIndexed"]);
	ASSERT_TRUE(yaml["PartselectMinusIndexed"]["index"]);
	ASSERT_TRUE(yaml["PartselectMinusIndexed"]["size"]);
	ASSERT_TRUE(yaml["PartselectMinusIndexed"]["var"]);
}

TEST(YAMLGenerator, Pointer) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Pointer.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_ptr(new AST::Node);
	AST::Node::Ptr c_var(new AST::Node);

	AST::Pointer::Ptr m_pointer(new AST::Pointer
		(c_ptr, c_var, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_pointer);

	ASSERT_TRUE(yaml["Pointer"]);
	ASSERT_TRUE(yaml["Pointer"]["ptr"]);
	ASSERT_TRUE(yaml["Pointer"]["var"]);
}

TEST(YAMLGenerator, Lvalue) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Lvalue.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_var(new AST::Node);

	AST::Lvalue::Ptr m_lvalue(new AST::Lvalue
		(c_var, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_lvalue);

	ASSERT_TRUE(yaml["Lvalue"]);
	ASSERT_TRUE(yaml["Lvalue"]["var"]);
}

TEST(YAMLGenerator, Rvalue) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Rvalue.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_var(new AST::Node);

	AST::Rvalue::Ptr m_rvalue(new AST::Rvalue
		(c_var, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_rvalue);

	ASSERT_TRUE(yaml["Rvalue"]);
	ASSERT_TRUE(yaml["Rvalue"]["var"]);
}

TEST(YAMLGenerator, UnaryOperator) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.UnaryOperator.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_right(new AST::Node);

	AST::UnaryOperator::Ptr m_unaryoperator(new AST::UnaryOperator
		(c_right, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_unaryoperator);

	ASSERT_TRUE(yaml["UnaryOperator"]);
	ASSERT_TRUE(yaml["UnaryOperator"]["right"]);
}

TEST(YAMLGenerator, Uplus) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Uplus.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_right(new AST::Node);

	AST::Uplus::Ptr m_uplus(new AST::Uplus
		(c_right, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_uplus);

	ASSERT_TRUE(yaml["Uplus"]);
	ASSERT_TRUE(yaml["Uplus"]["right"]);
}

TEST(YAMLGenerator, Uminus) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Uminus.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_right(new AST::Node);

	AST::Uminus::Ptr m_uminus(new AST::Uminus
		(c_right, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_uminus);

	ASSERT_TRUE(yaml["Uminus"]);
	ASSERT_TRUE(yaml["Uminus"]["right"]);
}

TEST(YAMLGenerator, Ulnot) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Ulnot.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_right(new AST::Node);

	AST::Ulnot::Ptr m_ulnot(new AST::Ulnot
		(c_right, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_ulnot);

	ASSERT_TRUE(yaml["Ulnot"]);
	ASSERT_TRUE(yaml["Ulnot"]["right"]);
}

TEST(YAMLGenerator, Unot) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Unot.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_right(new AST::Node);

	AST::Unot::Ptr m_unot(new AST::Unot
		(c_right, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_unot);

	ASSERT_TRUE(yaml["Unot"]);
	ASSERT_TRUE(yaml["Unot"]["right"]);
}

TEST(YAMLGenerator, Uand) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Uand.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_right(new AST::Node);

	AST::Uand::Ptr m_uand(new AST::Uand
		(c_right, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_uand);

	ASSERT_TRUE(yaml["Uand"]);
	ASSERT_TRUE(yaml["Uand"]["right"]);
}

TEST(YAMLGenerator, Unand) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Unand.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_right(new AST::Node);

	AST::Unand::Ptr m_unand(new AST::Unand
		(c_right, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_unand);

	ASSERT_TRUE(yaml["Unand"]);
	ASSERT_TRUE(yaml["Unand"]["right"]);
}

TEST(YAMLGenerator, Uor) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Uor.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_right(new AST::Node);

	AST::Uor::Ptr m_uor(new AST::Uor
		(c_right, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_uor);

	ASSERT_TRUE(yaml["Uor"]);
	ASSERT_TRUE(yaml["Uor"]["right"]);
}

TEST(YAMLGenerator, Unor) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Unor.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_right(new AST::Node);

	AST::Unor::Ptr m_unor(new AST::Unor
		(c_right, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_unor);

	ASSERT_TRUE(yaml["Unor"]);
	ASSERT_TRUE(yaml["Unor"]["right"]);
}

TEST(YAMLGenerator, Uxor) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Uxor.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_right(new AST::Node);

	AST::Uxor::Ptr m_uxor(new AST::Uxor
		(c_right, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_uxor);

	ASSERT_TRUE(yaml["Uxor"]);
	ASSERT_TRUE(yaml["Uxor"]["right"]);
}

TEST(YAMLGenerator, Uxnor) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Uxnor.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_right(new AST::Node);

	AST::Uxnor::Ptr m_uxnor(new AST::Uxnor
		(c_right, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_uxnor);

	ASSERT_TRUE(yaml["Uxnor"]);
	ASSERT_TRUE(yaml["Uxnor"]["right"]);
}

TEST(YAMLGenerator, Operator) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Operator.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_left(new AST::Node);
	AST::Node::Ptr c_right(new AST::Node);

	AST::Operator::Ptr m_operator(new AST::Operator
		(c_left, c_right, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_operator);

	ASSERT_TRUE(yaml["Operator"]);
	ASSERT_TRUE(yaml["Operator"]["left"]);
	ASSERT_TRUE(yaml["Operator"]["right"]);
}

TEST(YAMLGenerator, Power) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Power.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_left(new AST::Node);
	AST::Node::Ptr c_right(new AST::Node);

	AST::Power::Ptr m_power(new AST::Power
		(c_left, c_right, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_power);

	ASSERT_TRUE(yaml["Power"]);
	ASSERT_TRUE(yaml["Power"]["left"]);
	ASSERT_TRUE(yaml["Power"]["right"]);
}

TEST(YAMLGenerator, Times) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Times.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_left(new AST::Node);
	AST::Node::Ptr c_right(new AST::Node);

	AST::Times::Ptr m_times(new AST::Times
		(c_left, c_right, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_times);

	ASSERT_TRUE(yaml["Times"]);
	ASSERT_TRUE(yaml["Times"]["left"]);
	ASSERT_TRUE(yaml["Times"]["right"]);
}

TEST(YAMLGenerator, Divide) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Divide.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_left(new AST::Node);
	AST::Node::Ptr c_right(new AST::Node);

	AST::Divide::Ptr m_divide(new AST::Divide
		(c_left, c_right, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_divide);

	ASSERT_TRUE(yaml["Divide"]);
	ASSERT_TRUE(yaml["Divide"]["left"]);
	ASSERT_TRUE(yaml["Divide"]["right"]);
}

TEST(YAMLGenerator, Mod) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Mod.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_left(new AST::Node);
	AST::Node::Ptr c_right(new AST::Node);

	AST::Mod::Ptr m_mod(new AST::Mod
		(c_left, c_right, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_mod);

	ASSERT_TRUE(yaml["Mod"]);
	ASSERT_TRUE(yaml["Mod"]["left"]);
	ASSERT_TRUE(yaml["Mod"]["right"]);
}

TEST(YAMLGenerator, Plus) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Plus.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_left(new AST::Node);
	AST::Node::Ptr c_right(new AST::Node);

	AST::Plus::Ptr m_plus(new AST::Plus
		(c_left, c_right, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_plus);

	ASSERT_TRUE(yaml["Plus"]);
	ASSERT_TRUE(yaml["Plus"]["left"]);
	ASSERT_TRUE(yaml["Plus"]["right"]);
}

TEST(YAMLGenerator, Minus) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Minus.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_left(new AST::Node);
	AST::Node::Ptr c_right(new AST::Node);

	AST::Minus::Ptr m_minus(new AST::Minus
		(c_left, c_right, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_minus);

	ASSERT_TRUE(yaml["Minus"]);
	ASSERT_TRUE(yaml["Minus"]["left"]);
	ASSERT_TRUE(yaml["Minus"]["right"]);
}

TEST(YAMLGenerator, Sll) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Sll.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_left(new AST::Node);
	AST::Node::Ptr c_right(new AST::Node);

	AST::Sll::Ptr m_sll(new AST::Sll
		(c_left, c_right, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_sll);

	ASSERT_TRUE(yaml["Sll"]);
	ASSERT_TRUE(yaml["Sll"]["left"]);
	ASSERT_TRUE(yaml["Sll"]["right"]);
}

TEST(YAMLGenerator, Srl) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Srl.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_left(new AST::Node);
	AST::Node::Ptr c_right(new AST::Node);

	AST::Srl::Ptr m_srl(new AST::Srl
		(c_left, c_right, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_srl);

	ASSERT_TRUE(yaml["Srl"]);
	ASSERT_TRUE(yaml["Srl"]["left"]);
	ASSERT_TRUE(yaml["Srl"]["right"]);
}

TEST(YAMLGenerator, Sra) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Sra.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_left(new AST::Node);
	AST::Node::Ptr c_right(new AST::Node);

	AST::Sra::Ptr m_sra(new AST::Sra
		(c_left, c_right, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_sra);

	ASSERT_TRUE(yaml["Sra"]);
	ASSERT_TRUE(yaml["Sra"]["left"]);
	ASSERT_TRUE(yaml["Sra"]["right"]);
}

TEST(YAMLGenerator, LessThan) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.LessThan.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_left(new AST::Node);
	AST::Node::Ptr c_right(new AST::Node);

	AST::LessThan::Ptr m_lessthan(new AST::LessThan
		(c_left, c_right, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_lessthan);

	ASSERT_TRUE(yaml["LessThan"]);
	ASSERT_TRUE(yaml["LessThan"]["left"]);
	ASSERT_TRUE(yaml["LessThan"]["right"]);
}

TEST(YAMLGenerator, GreaterThan) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.GreaterThan.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_left(new AST::Node);
	AST::Node::Ptr c_right(new AST::Node);

	AST::GreaterThan::Ptr m_greaterthan(new AST::GreaterThan
		(c_left, c_right, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_greaterthan);

	ASSERT_TRUE(yaml["GreaterThan"]);
	ASSERT_TRUE(yaml["GreaterThan"]["left"]);
	ASSERT_TRUE(yaml["GreaterThan"]["right"]);
}

TEST(YAMLGenerator, LessEq) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.LessEq.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_left(new AST::Node);
	AST::Node::Ptr c_right(new AST::Node);

	AST::LessEq::Ptr m_lesseq(new AST::LessEq
		(c_left, c_right, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_lesseq);

	ASSERT_TRUE(yaml["LessEq"]);
	ASSERT_TRUE(yaml["LessEq"]["left"]);
	ASSERT_TRUE(yaml["LessEq"]["right"]);
}

TEST(YAMLGenerator, GreaterEq) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.GreaterEq.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_left(new AST::Node);
	AST::Node::Ptr c_right(new AST::Node);

	AST::GreaterEq::Ptr m_greatereq(new AST::GreaterEq
		(c_left, c_right, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_greatereq);

	ASSERT_TRUE(yaml["GreaterEq"]);
	ASSERT_TRUE(yaml["GreaterEq"]["left"]);
	ASSERT_TRUE(yaml["GreaterEq"]["right"]);
}

TEST(YAMLGenerator, Eq) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Eq.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_left(new AST::Node);
	AST::Node::Ptr c_right(new AST::Node);

	AST::Eq::Ptr m_eq(new AST::Eq
		(c_left, c_right, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_eq);

	ASSERT_TRUE(yaml["Eq"]);
	ASSERT_TRUE(yaml["Eq"]["left"]);
	ASSERT_TRUE(yaml["Eq"]["right"]);
}

TEST(YAMLGenerator, NotEq) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.NotEq.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_left(new AST::Node);
	AST::Node::Ptr c_right(new AST::Node);

	AST::NotEq::Ptr m_noteq(new AST::NotEq
		(c_left, c_right, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_noteq);

	ASSERT_TRUE(yaml["NotEq"]);
	ASSERT_TRUE(yaml["NotEq"]["left"]);
	ASSERT_TRUE(yaml["NotEq"]["right"]);
}

TEST(YAMLGenerator, Eql) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Eql.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_left(new AST::Node);
	AST::Node::Ptr c_right(new AST::Node);

	AST::Eql::Ptr m_eql(new AST::Eql
		(c_left, c_right, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_eql);

	ASSERT_TRUE(yaml["Eql"]);
	ASSERT_TRUE(yaml["Eql"]["left"]);
	ASSERT_TRUE(yaml["Eql"]["right"]);
}

TEST(YAMLGenerator, NotEql) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.NotEql.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_left(new AST::Node);
	AST::Node::Ptr c_right(new AST::Node);

	AST::NotEql::Ptr m_noteql(new AST::NotEql
		(c_left, c_right, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_noteql);

	ASSERT_TRUE(yaml["NotEql"]);
	ASSERT_TRUE(yaml["NotEql"]["left"]);
	ASSERT_TRUE(yaml["NotEql"]["right"]);
}

TEST(YAMLGenerator, And) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.And.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_left(new AST::Node);
	AST::Node::Ptr c_right(new AST::Node);

	AST::And::Ptr m_and(new AST::And
		(c_left, c_right, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_and);

	ASSERT_TRUE(yaml["And"]);
	ASSERT_TRUE(yaml["And"]["left"]);
	ASSERT_TRUE(yaml["And"]["right"]);
}

TEST(YAMLGenerator, Xor) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Xor.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_left(new AST::Node);
	AST::Node::Ptr c_right(new AST::Node);

	AST::Xor::Ptr m_xor(new AST::Xor
		(c_left, c_right, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_xor);

	ASSERT_TRUE(yaml["Xor"]);
	ASSERT_TRUE(yaml["Xor"]["left"]);
	ASSERT_TRUE(yaml["Xor"]["right"]);
}

TEST(YAMLGenerator, Xnor) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Xnor.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_left(new AST::Node);
	AST::Node::Ptr c_right(new AST::Node);

	AST::Xnor::Ptr m_xnor(new AST::Xnor
		(c_left, c_right, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_xnor);

	ASSERT_TRUE(yaml["Xnor"]);
	ASSERT_TRUE(yaml["Xnor"]["left"]);
	ASSERT_TRUE(yaml["Xnor"]["right"]);
}

TEST(YAMLGenerator, Or) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Or.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_left(new AST::Node);
	AST::Node::Ptr c_right(new AST::Node);

	AST::Or::Ptr m_or(new AST::Or
		(c_left, c_right, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_or);

	ASSERT_TRUE(yaml["Or"]);
	ASSERT_TRUE(yaml["Or"]["left"]);
	ASSERT_TRUE(yaml["Or"]["right"]);
}

TEST(YAMLGenerator, Land) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Land.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_left(new AST::Node);
	AST::Node::Ptr c_right(new AST::Node);

	AST::Land::Ptr m_land(new AST::Land
		(c_left, c_right, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_land);

	ASSERT_TRUE(yaml["Land"]);
	ASSERT_TRUE(yaml["Land"]["left"]);
	ASSERT_TRUE(yaml["Land"]["right"]);
}

TEST(YAMLGenerator, Lor) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Lor.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_left(new AST::Node);
	AST::Node::Ptr c_right(new AST::Node);

	AST::Lor::Ptr m_lor(new AST::Lor
		(c_left, c_right, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_lor);

	ASSERT_TRUE(yaml["Lor"]);
	ASSERT_TRUE(yaml["Lor"]["left"]);
	ASSERT_TRUE(yaml["Lor"]["right"]);
}

TEST(YAMLGenerator, Cond) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Cond.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_cond(new AST::Node);
	AST::Node::Ptr c_left(new AST::Node);
	AST::Node::Ptr c_right(new AST::Node);

	AST::Cond::Ptr m_cond(new AST::Cond
		(c_cond, c_left, c_right, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_cond);

	ASSERT_TRUE(yaml["Cond"]);
	ASSERT_TRUE(yaml["Cond"]["cond"]);
	ASSERT_TRUE(yaml["Cond"]["left"]);
	ASSERT_TRUE(yaml["Cond"]["right"]);
}

TEST(YAMLGenerator, Always) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Always.log");
	Logger::add_stdout_sink();

	
	AST::Senslist::Ptr c_senslist(new AST::Senslist);
	AST::Node::Ptr c_statement(new AST::Node);

	AST::Always::Ptr m_always(new AST::Always
		(c_senslist, c_statement, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_always);

	ASSERT_TRUE(yaml["Always"]);
	ASSERT_TRUE(yaml["Always"]["senslist"]);
	ASSERT_TRUE(yaml["Always"]["statement"]);
}

TEST(YAMLGenerator, Senslist) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Senslist.log");
	Logger::add_stdout_sink();

	
	AST::Sens::ListPtr c_list(new AST::Sens::List);

	AST::Senslist::Ptr m_senslist(new AST::Senslist
		(c_list, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_senslist);

	ASSERT_TRUE(yaml["Senslist"]);
	ASSERT_TRUE(yaml["Senslist"]["list"]);
}

TEST(YAMLGenerator, Sens) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Sens.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_sig(new AST::Node);
	AST::Sens::TypeEnum p_type = AST::Sens::TypeEnum::NONE;

	AST::Sens::Ptr m_sens(new AST::Sens
		(c_sig, p_type, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_sens);

	ASSERT_TRUE(yaml["Sens"]);
	ASSERT_TRUE(yaml["Sens"]["sig"]);
	ASSERT_TRUE(yaml["Sens"]["type"].as<AST::Sens::TypeEnum>() == AST::Sens::TypeEnum::NONE);
}

TEST(YAMLGenerator, Defparamlist) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Defparamlist.log");
	Logger::add_stdout_sink();

	
	AST::Defparam::ListPtr c_list(new AST::Defparam::List);

	AST::Defparamlist::Ptr m_defparamlist(new AST::Defparamlist
		(c_list, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_defparamlist);

	ASSERT_TRUE(yaml["Defparamlist"]);
	ASSERT_TRUE(yaml["Defparamlist"]["list"]);
}

TEST(YAMLGenerator, Defparam) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Defparam.log");
	Logger::add_stdout_sink();

	
	AST::Identifier::Ptr c_identifier(new AST::Identifier);
	AST::Rvalue::Ptr c_right(new AST::Rvalue);

	AST::Defparam::Ptr m_defparam(new AST::Defparam
		(c_identifier, c_right, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_defparam);

	ASSERT_TRUE(yaml["Defparam"]);
	ASSERT_TRUE(yaml["Defparam"]["identifier"]);
	ASSERT_TRUE(yaml["Defparam"]["right"]);
}

TEST(YAMLGenerator, Assign) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Assign.log");
	Logger::add_stdout_sink();

	
	AST::Lvalue::Ptr c_left(new AST::Lvalue);
	AST::Rvalue::Ptr c_right(new AST::Rvalue);
	AST::DelayStatement::Ptr c_ldelay(new AST::DelayStatement);
	AST::DelayStatement::Ptr c_rdelay(new AST::DelayStatement);

	AST::Assign::Ptr m_assign(new AST::Assign
		(c_left, c_right, c_ldelay, c_rdelay, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_assign);

	ASSERT_TRUE(yaml["Assign"]);
	ASSERT_TRUE(yaml["Assign"]["left"]);
	ASSERT_TRUE(yaml["Assign"]["right"]);
	ASSERT_TRUE(yaml["Assign"]["ldelay"]);
	ASSERT_TRUE(yaml["Assign"]["rdelay"]);
}

TEST(YAMLGenerator, BlockingSubstitution) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.BlockingSubstitution.log");
	Logger::add_stdout_sink();

	
	AST::Lvalue::Ptr c_left(new AST::Lvalue);
	AST::Rvalue::Ptr c_right(new AST::Rvalue);
	AST::DelayStatement::Ptr c_ldelay(new AST::DelayStatement);
	AST::DelayStatement::Ptr c_rdelay(new AST::DelayStatement);

	AST::BlockingSubstitution::Ptr m_blockingsubstitution(new AST::BlockingSubstitution
		(c_left, c_right, c_ldelay, c_rdelay, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_blockingsubstitution);

	ASSERT_TRUE(yaml["BlockingSubstitution"]);
	ASSERT_TRUE(yaml["BlockingSubstitution"]["left"]);
	ASSERT_TRUE(yaml["BlockingSubstitution"]["right"]);
	ASSERT_TRUE(yaml["BlockingSubstitution"]["ldelay"]);
	ASSERT_TRUE(yaml["BlockingSubstitution"]["rdelay"]);
}

TEST(YAMLGenerator, NonblockingSubstitution) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.NonblockingSubstitution.log");
	Logger::add_stdout_sink();

	
	AST::Lvalue::Ptr c_left(new AST::Lvalue);
	AST::Rvalue::Ptr c_right(new AST::Rvalue);
	AST::DelayStatement::Ptr c_ldelay(new AST::DelayStatement);
	AST::DelayStatement::Ptr c_rdelay(new AST::DelayStatement);

	AST::NonblockingSubstitution::Ptr m_nonblockingsubstitution(new AST::NonblockingSubstitution
		(c_left, c_right, c_ldelay, c_rdelay, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_nonblockingsubstitution);

	ASSERT_TRUE(yaml["NonblockingSubstitution"]);
	ASSERT_TRUE(yaml["NonblockingSubstitution"]["left"]);
	ASSERT_TRUE(yaml["NonblockingSubstitution"]["right"]);
	ASSERT_TRUE(yaml["NonblockingSubstitution"]["ldelay"]);
	ASSERT_TRUE(yaml["NonblockingSubstitution"]["rdelay"]);
}

TEST(YAMLGenerator, IfStatement) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.IfStatement.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_cond(new AST::Node);
	AST::Node::Ptr c_true_statement(new AST::Node);
	AST::Node::Ptr c_false_statement(new AST::Node);

	AST::IfStatement::Ptr m_ifstatement(new AST::IfStatement
		(c_cond, c_true_statement, c_false_statement, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_ifstatement);

	ASSERT_TRUE(yaml["IfStatement"]);
	ASSERT_TRUE(yaml["IfStatement"]["cond"]);
	ASSERT_TRUE(yaml["IfStatement"]["true_statement"]);
	ASSERT_TRUE(yaml["IfStatement"]["false_statement"]);
}

TEST(YAMLGenerator, RepeatStatement) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.RepeatStatement.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_times(new AST::Node);
	AST::Node::Ptr c_statement(new AST::Node);

	AST::RepeatStatement::Ptr m_repeatstatement(new AST::RepeatStatement
		(c_times, c_statement, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_repeatstatement);

	ASSERT_TRUE(yaml["RepeatStatement"]);
	ASSERT_TRUE(yaml["RepeatStatement"]["times"]);
	ASSERT_TRUE(yaml["RepeatStatement"]["statement"]);
}

TEST(YAMLGenerator, ForStatement) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.ForStatement.log");
	Logger::add_stdout_sink();

	
	AST::BlockingSubstitution::Ptr c_pre(new AST::BlockingSubstitution);
	AST::Node::Ptr c_cond(new AST::Node);
	AST::BlockingSubstitution::Ptr c_post(new AST::BlockingSubstitution);
	AST::Node::Ptr c_statement(new AST::Node);

	AST::ForStatement::Ptr m_forstatement(new AST::ForStatement
		(c_pre, c_cond, c_post, c_statement, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_forstatement);

	ASSERT_TRUE(yaml["ForStatement"]);
	ASSERT_TRUE(yaml["ForStatement"]["pre"]);
	ASSERT_TRUE(yaml["ForStatement"]["cond"]);
	ASSERT_TRUE(yaml["ForStatement"]["post"]);
	ASSERT_TRUE(yaml["ForStatement"]["statement"]);
}

TEST(YAMLGenerator, WhileStatement) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.WhileStatement.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_cond(new AST::Node);
	AST::Node::Ptr c_statement(new AST::Node);

	AST::WhileStatement::Ptr m_whilestatement(new AST::WhileStatement
		(c_cond, c_statement, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_whilestatement);

	ASSERT_TRUE(yaml["WhileStatement"]);
	ASSERT_TRUE(yaml["WhileStatement"]["cond"]);
	ASSERT_TRUE(yaml["WhileStatement"]["statement"]);
}

TEST(YAMLGenerator, CaseStatement) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.CaseStatement.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_comp(new AST::Node);
	AST::Case::ListPtr c_caselist(new AST::Case::List);

	AST::CaseStatement::Ptr m_casestatement(new AST::CaseStatement
		(c_comp, c_caselist, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_casestatement);

	ASSERT_TRUE(yaml["CaseStatement"]);
	ASSERT_TRUE(yaml["CaseStatement"]["comp"]);
	ASSERT_TRUE(yaml["CaseStatement"]["caselist"]);
}

TEST(YAMLGenerator, CasexStatement) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.CasexStatement.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_comp(new AST::Node);
	AST::Case::ListPtr c_caselist(new AST::Case::List);

	AST::CasexStatement::Ptr m_casexstatement(new AST::CasexStatement
		(c_comp, c_caselist, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_casexstatement);

	ASSERT_TRUE(yaml["CasexStatement"]);
	ASSERT_TRUE(yaml["CasexStatement"]["comp"]);
	ASSERT_TRUE(yaml["CasexStatement"]["caselist"]);
}

TEST(YAMLGenerator, CasezStatement) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.CasezStatement.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_comp(new AST::Node);
	AST::Case::ListPtr c_caselist(new AST::Case::List);

	AST::CasezStatement::Ptr m_casezstatement(new AST::CasezStatement
		(c_comp, c_caselist, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_casezstatement);

	ASSERT_TRUE(yaml["CasezStatement"]);
	ASSERT_TRUE(yaml["CasezStatement"]["comp"]);
	ASSERT_TRUE(yaml["CasezStatement"]["caselist"]);
}

TEST(YAMLGenerator, Case) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Case.log");
	Logger::add_stdout_sink();

	
	AST::Node::ListPtr c_cond(new AST::Node::List);
	AST::Node::Ptr c_statement(new AST::Node);

	AST::Case::Ptr m_case(new AST::Case
		(c_cond, c_statement, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_case);

	ASSERT_TRUE(yaml["Case"]);
	ASSERT_TRUE(yaml["Case"]["cond"]);
	ASSERT_TRUE(yaml["Case"]["statement"]);
}

TEST(YAMLGenerator, Block) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Block.log");
	Logger::add_stdout_sink();

	
	AST::Node::ListPtr c_statements(new AST::Node::List);
	std::string p_scope = "mynbiqpmzj";

	AST::Block::Ptr m_block(new AST::Block
		(c_statements, p_scope, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_block);

	ASSERT_TRUE(yaml["Block"]);
	ASSERT_TRUE(yaml["Block"]["statements"]);
	ASSERT_TRUE(yaml["Block"]["scope"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLGenerator, Initial) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Initial.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_statement(new AST::Node);

	AST::Initial::Ptr m_initial(new AST::Initial
		(c_statement, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_initial);

	ASSERT_TRUE(yaml["Initial"]);
	ASSERT_TRUE(yaml["Initial"]["statement"]);
}

TEST(YAMLGenerator, EventStatement) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.EventStatement.log");
	Logger::add_stdout_sink();

	
	AST::Senslist::Ptr c_senslist(new AST::Senslist);
	AST::Node::Ptr c_statement(new AST::Node);

	AST::EventStatement::Ptr m_eventstatement(new AST::EventStatement
		(c_senslist, c_statement, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_eventstatement);

	ASSERT_TRUE(yaml["EventStatement"]);
	ASSERT_TRUE(yaml["EventStatement"]["senslist"]);
	ASSERT_TRUE(yaml["EventStatement"]["statement"]);
}

TEST(YAMLGenerator, WaitStatement) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.WaitStatement.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_cond(new AST::Node);
	AST::Node::Ptr c_statement(new AST::Node);

	AST::WaitStatement::Ptr m_waitstatement(new AST::WaitStatement
		(c_cond, c_statement, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_waitstatement);

	ASSERT_TRUE(yaml["WaitStatement"]);
	ASSERT_TRUE(yaml["WaitStatement"]["cond"]);
	ASSERT_TRUE(yaml["WaitStatement"]["statement"]);
}

TEST(YAMLGenerator, ForeverStatement) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.ForeverStatement.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_statement(new AST::Node);

	AST::ForeverStatement::Ptr m_foreverstatement(new AST::ForeverStatement
		(c_statement, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_foreverstatement);

	ASSERT_TRUE(yaml["ForeverStatement"]);
	ASSERT_TRUE(yaml["ForeverStatement"]["statement"]);
}

TEST(YAMLGenerator, DelayStatement) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.DelayStatement.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_delay(new AST::Node);

	AST::DelayStatement::Ptr m_delaystatement(new AST::DelayStatement
		(c_delay, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_delaystatement);

	ASSERT_TRUE(yaml["DelayStatement"]);
	ASSERT_TRUE(yaml["DelayStatement"]["delay"]);
}

TEST(YAMLGenerator, Instancelist) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Instancelist.log");
	Logger::add_stdout_sink();

	
	AST::ParamArg::ListPtr c_parameterlist(new AST::ParamArg::List);
	AST::Instance::ListPtr c_instances(new AST::Instance::List);
	std::string p_module = "mynbiqpmzj";

	AST::Instancelist::Ptr m_instancelist(new AST::Instancelist
		(c_parameterlist, c_instances, p_module, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_instancelist);

	ASSERT_TRUE(yaml["Instancelist"]);
	ASSERT_TRUE(yaml["Instancelist"]["parameterlist"]);
	ASSERT_TRUE(yaml["Instancelist"]["instances"]);
	ASSERT_TRUE(yaml["Instancelist"]["module"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLGenerator, Instance) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Instance.log");
	Logger::add_stdout_sink();

	
	AST::Length::Ptr c_array(new AST::Length);
	AST::ParamArg::ListPtr c_parameterlist(new AST::ParamArg::List);
	AST::PortArg::ListPtr c_portlist(new AST::PortArg::List);
	std::string p_module = "mynbiqpmzj";
	std::string p_name = "plsgqejeyd";

	AST::Instance::Ptr m_instance(new AST::Instance
		(c_array, c_parameterlist, c_portlist, p_module, p_name, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_instance);

	ASSERT_TRUE(yaml["Instance"]);
	ASSERT_TRUE(yaml["Instance"]["array"]);
	ASSERT_TRUE(yaml["Instance"]["parameterlist"]);
	ASSERT_TRUE(yaml["Instance"]["portlist"]);
	ASSERT_TRUE(yaml["Instance"]["module"].as<std::string>() == "mynbiqpmzj");
	ASSERT_TRUE(yaml["Instance"]["name"].as<std::string>() == "plsgqejeyd");
}

TEST(YAMLGenerator, ParamArg) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.ParamArg.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_value(new AST::Node);
	std::string p_name = "mynbiqpmzj";

	AST::ParamArg::Ptr m_paramarg(new AST::ParamArg
		(c_value, p_name, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_paramarg);

	ASSERT_TRUE(yaml["ParamArg"]);
	ASSERT_TRUE(yaml["ParamArg"]["value"]);
	ASSERT_TRUE(yaml["ParamArg"]["name"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLGenerator, PortArg) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.PortArg.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_value(new AST::Node);
	std::string p_name = "mynbiqpmzj";

	AST::PortArg::Ptr m_portarg(new AST::PortArg
		(c_value, p_name, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_portarg);

	ASSERT_TRUE(yaml["PortArg"]);
	ASSERT_TRUE(yaml["PortArg"]["value"]);
	ASSERT_TRUE(yaml["PortArg"]["name"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLGenerator, Function) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Function.log");
	Logger::add_stdout_sink();

	
	AST::Width::ListPtr c_retwidths(new AST::Width::List);
	AST::Node::ListPtr c_ports(new AST::Node::List);
	AST::Node::ListPtr c_statements(new AST::Node::List);
	std::string p_name = "mynbiqpmzj";
	bool p_automatic = false;
	AST::Function::RettypeEnum p_rettype = AST::Function::RettypeEnum::REAL;
	bool p_retsign = true;

	AST::Function::Ptr m_function(new AST::Function
		(c_retwidths, c_ports, c_statements, p_name, p_automatic, p_rettype, p_retsign, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_function);

	ASSERT_TRUE(yaml["Function"]);
	ASSERT_TRUE(yaml["Function"]["retwidths"]);
	ASSERT_TRUE(yaml["Function"]["ports"]);
	ASSERT_TRUE(yaml["Function"]["statements"]);
	ASSERT_TRUE(yaml["Function"]["name"].as<std::string>() == "mynbiqpmzj");
	ASSERT_TRUE(yaml["Function"]["automatic"].as<bool>() == false);
	ASSERT_TRUE(yaml["Function"]["rettype"].as<AST::Function::RettypeEnum>() == AST::Function::RettypeEnum::REAL);
	ASSERT_TRUE(yaml["Function"]["retsign"].as<bool>() == true);
}

TEST(YAMLGenerator, FunctionCall) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.FunctionCall.log");
	Logger::add_stdout_sink();

	
	AST::Node::ListPtr c_args(new AST::Node::List);
	std::string p_name = "mynbiqpmzj";

	AST::FunctionCall::Ptr m_functioncall(new AST::FunctionCall
		(c_args, p_name, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_functioncall);

	ASSERT_TRUE(yaml["FunctionCall"]);
	ASSERT_TRUE(yaml["FunctionCall"]["args"]);
	ASSERT_TRUE(yaml["FunctionCall"]["name"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLGenerator, Task) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Task.log");
	Logger::add_stdout_sink();

	
	AST::Node::ListPtr c_ports(new AST::Node::List);
	AST::Node::ListPtr c_statements(new AST::Node::List);
	std::string p_name = "mynbiqpmzj";
	bool p_automatic = false;

	AST::Task::Ptr m_task(new AST::Task
		(c_ports, c_statements, p_name, p_automatic, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_task);

	ASSERT_TRUE(yaml["Task"]);
	ASSERT_TRUE(yaml["Task"]["ports"]);
	ASSERT_TRUE(yaml["Task"]["statements"]);
	ASSERT_TRUE(yaml["Task"]["name"].as<std::string>() == "mynbiqpmzj");
	ASSERT_TRUE(yaml["Task"]["automatic"].as<bool>() == false);
}

TEST(YAMLGenerator, TaskCall) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.TaskCall.log");
	Logger::add_stdout_sink();

	
	AST::Node::ListPtr c_args(new AST::Node::List);
	std::string p_name = "mynbiqpmzj";

	AST::TaskCall::Ptr m_taskcall(new AST::TaskCall
		(c_args, p_name, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_taskcall);

	ASSERT_TRUE(yaml["TaskCall"]);
	ASSERT_TRUE(yaml["TaskCall"]["args"]);
	ASSERT_TRUE(yaml["TaskCall"]["name"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLGenerator, GenerateStatement) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.GenerateStatement.log");
	Logger::add_stdout_sink();

	
	AST::Node::ListPtr c_items(new AST::Node::List);

	AST::GenerateStatement::Ptr m_generatestatement(new AST::GenerateStatement
		(c_items, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_generatestatement);

	ASSERT_TRUE(yaml["GenerateStatement"]);
	ASSERT_TRUE(yaml["GenerateStatement"]["items"]);
}

TEST(YAMLGenerator, SystemCall) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.SystemCall.log");
	Logger::add_stdout_sink();

	
	AST::Node::ListPtr c_args(new AST::Node::List);
	std::string p_syscall = "mynbiqpmzj";

	AST::SystemCall::Ptr m_systemcall(new AST::SystemCall
		(c_args, p_syscall, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_systemcall);

	ASSERT_TRUE(yaml["SystemCall"]);
	ASSERT_TRUE(yaml["SystemCall"]["args"]);
	ASSERT_TRUE(yaml["SystemCall"]["syscall"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLGenerator, IdentifierScopeLabel) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.IdentifierScopeLabel.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_loop(new AST::Node);
	std::string p_name = "mynbiqpmzj";

	AST::IdentifierScopeLabel::Ptr m_identifierscopelabel(new AST::IdentifierScopeLabel
		(c_loop, p_name, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_identifierscopelabel);

	ASSERT_TRUE(yaml["IdentifierScopeLabel"]);
	ASSERT_TRUE(yaml["IdentifierScopeLabel"]["loop"]);
	ASSERT_TRUE(yaml["IdentifierScopeLabel"]["name"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLGenerator, IdentifierScope) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.IdentifierScope.log");
	Logger::add_stdout_sink();

	
	AST::IdentifierScopeLabel::ListPtr c_labellist(new AST::IdentifierScopeLabel::List);

	AST::IdentifierScope::Ptr m_identifierscope(new AST::IdentifierScope
		(c_labellist, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_identifierscope);

	ASSERT_TRUE(yaml["IdentifierScope"]);
	ASSERT_TRUE(yaml["IdentifierScope"]["labellist"]);
}

TEST(YAMLGenerator, Disable) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.Disable.log");
	Logger::add_stdout_sink();

	
	std::string p_dest = "mynbiqpmzj";

	AST::Disable::Ptr m_disable(new AST::Disable
		(p_dest, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_disable);

	ASSERT_TRUE(yaml["Disable"]);
	ASSERT_TRUE(yaml["Disable"]["dest"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLGenerator, ParallelBlock) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.ParallelBlock.log");
	Logger::add_stdout_sink();

	
	AST::Node::ListPtr c_statements(new AST::Node::List);
	std::string p_scope = "mynbiqpmzj";

	AST::ParallelBlock::Ptr m_parallelblock(new AST::ParallelBlock
		(c_statements, p_scope, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_parallelblock);

	ASSERT_TRUE(yaml["ParallelBlock"]);
	ASSERT_TRUE(yaml["ParallelBlock"]["statements"]);
	ASSERT_TRUE(yaml["ParallelBlock"]["scope"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLGenerator, SingleStatement) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.SingleStatement.log");
	Logger::add_stdout_sink();

	
	AST::Node::Ptr c_statement(new AST::Node);
	AST::DelayStatement::Ptr c_delay(new AST::DelayStatement);
	std::string p_scope = "mynbiqpmzj";

	AST::SingleStatement::Ptr m_singlestatement(new AST::SingleStatement
		(c_statement, c_delay, p_scope, "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_singlestatement);

	ASSERT_TRUE(yaml["SingleStatement"]);
	ASSERT_TRUE(yaml["SingleStatement"]["statement"]);
	ASSERT_TRUE(yaml["SingleStatement"]["delay"]);
	ASSERT_TRUE(yaml["SingleStatement"]["scope"].as<std::string>() == "mynbiqpmzj");
}
