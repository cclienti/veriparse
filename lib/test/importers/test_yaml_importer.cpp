#include <gtest/gtest.h>
#include <yaml-cpp/yaml.h>
#include <veriparse/generators/yaml_generator.hpp>
#include <veriparse/logger/logger.hpp>
#include <veriparse/importers/yaml_importer.hpp>
#include <veriparse/importers/yaml_specializations.hpp>
#include <iostream>

using namespace Veriparse;


TEST(YAMLImporter, MySource) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.MySource.log");
	Logger::add_stdout_sink();

	std::string str("Source:\n"
	                "  filename:  test.v\n"
	                "  line: 1\n"
	                "  description:\n"
	                "    Description:\n"
	                "      filename: test.v\n"
	                "      line: 2\n"
	                "      definitions:\n"
	                "      - Module:\n"
	                "          filename: test.v\n"
	                "          line: 33\n"
	                "          default_nettype: REG\n"
	                "          name: module0\n"
	                "      - Module:\n"
	                "          filename: test.v\n"
	                "          line: 7\n"
	                "          default_nettype: WIRE\n"
	                "          name: module1\n");

	YAML::Node yaml_ref = YAML::Load(str);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	AST::Source::Ptr ast_cast = AST::cast_to<AST::Source>(ast);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_EQ(yaml["Source"]["filename"].as<std::string>(), "test.v");
	ASSERT_EQ(yaml["Source"]["line"].as<int>(), 1);
	ASSERT_TRUE(yaml["Source"]["description"]["Description"]);
	ASSERT_EQ(yaml["Source"]["description"]["Description"]["filename"].as<std::string>(), "test.v");
	ASSERT_EQ(yaml["Source"]["description"]["Description"]["line"].as<int>(), 2);

	YAML::Node module0 = yaml["Source"]["description"]["Description"]["definitions"][0]["Module"];
	YAML::Node module1 = yaml["Source"]["description"]["Description"]["definitions"][1]["Module"];

	ASSERT_EQ(module0["line"].as<int>(), 33);
	ASSERT_EQ(module0["filename"].as<std::string>(), "test.v");
	ASSERT_EQ(module0["name"].as<std::string>(), "module0");
	ASSERT_EQ(module0["default_nettype"].as<AST::Module::Default_nettypeEnum>(), AST::Module::Default_nettypeEnum::REG);

	ASSERT_EQ(module1["line"].as<int>(), 7);
	ASSERT_EQ(module1["filename"].as<std::string>(), "test.v");
	ASSERT_EQ(module1["name"].as<std::string>(), "module1");
	ASSERT_EQ(module1["default_nettype"].as<AST::Module::Default_nettypeEnum>(), AST::Module::Default_nettypeEnum::WIRE);
}



	

TEST(YAMLImporter, Source) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Source.log");
	Logger::add_stdout_sink();
	std::string str (
		"Source:\n"
		"  filename: source.v\n"
		"  line: 6\n"
		"  description:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Source"]);
	ASSERT_TRUE(yaml["Source"]["filename"].as<std::string>() == "source.v");
	ASSERT_TRUE(yaml["Source"]["line"].as<int>() == 6);
	ASSERT_TRUE(yaml["Source"]["description"]);
}


	

TEST(YAMLImporter, Description) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Description.log");
	Logger::add_stdout_sink();
	std::string str (
		"Description:\n"
		"  filename: description.v\n"
		"  line: 11\n"
		"  definitions:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Description"]);
	ASSERT_TRUE(yaml["Description"]["filename"].as<std::string>() == "description.v");
	ASSERT_TRUE(yaml["Description"]["line"].as<int>() == 11);
	ASSERT_TRUE(yaml["Description"]["definitions"]);
}


	

TEST(YAMLImporter, Pragma) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Pragma.log");
	Logger::add_stdout_sink();
	std::string str (
		"Pragma:\n"
		"  filename: pragma.v\n"
		"  line: 6\n"
		"  expression:\n"
		"  name: mynbiqpmzj\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Pragma"]);
	ASSERT_TRUE(yaml["Pragma"]["filename"].as<std::string>() == "pragma.v");
	ASSERT_TRUE(yaml["Pragma"]["line"].as<int>() == 6);
	ASSERT_TRUE(yaml["Pragma"]["expression"]);
	ASSERT_TRUE(yaml["Pragma"]["name"].as<std::string>() == "mynbiqpmzj");
}


	

TEST(YAMLImporter, Module) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Module.log");
	Logger::add_stdout_sink();
	std::string str (
		"Module:\n"
		"  filename: module.v\n"
		"  line: 6\n"
		"  params:\n"
		"  ports:\n"
		"  items:\n"
		"  name: mynbiqpmzj\n"
		"  default_nettype: NONE\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Module"]);
	ASSERT_TRUE(yaml["Module"]["filename"].as<std::string>() == "module.v");
	ASSERT_TRUE(yaml["Module"]["line"].as<int>() == 6);
	ASSERT_TRUE(yaml["Module"]["params"]);
	ASSERT_TRUE(yaml["Module"]["ports"]);
	ASSERT_TRUE(yaml["Module"]["items"]);
	ASSERT_TRUE(yaml["Module"]["name"].as<std::string>() == "mynbiqpmzj");
	ASSERT_TRUE(yaml["Module"]["default_nettype"].as<AST::Module::Default_nettypeEnum>() == AST::Module::Default_nettypeEnum::NONE);
}


	

TEST(YAMLImporter, Port) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Port.log");
	Logger::add_stdout_sink();
	std::string str (
		"Port:\n"
		"  filename: port.v\n"
		"  line: 4\n"
		"  widths:\n"
		"  name: mynbiqpmzj\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Port"]);
	ASSERT_TRUE(yaml["Port"]["filename"].as<std::string>() == "port.v");
	ASSERT_TRUE(yaml["Port"]["line"].as<int>() == 4);
	ASSERT_TRUE(yaml["Port"]["widths"]);
	ASSERT_TRUE(yaml["Port"]["name"].as<std::string>() == "mynbiqpmzj");
}


	

TEST(YAMLImporter, Width) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Width.log");
	Logger::add_stdout_sink();
	std::string str (
		"Width:\n"
		"  filename: width.v\n"
		"  line: 5\n"
		"  msb:\n"
		"  lsb:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Width"]);
	ASSERT_TRUE(yaml["Width"]["filename"].as<std::string>() == "width.v");
	ASSERT_TRUE(yaml["Width"]["line"].as<int>() == 5);
	ASSERT_TRUE(yaml["Width"]["msb"]);
	ASSERT_TRUE(yaml["Width"]["lsb"]);
}


	

TEST(YAMLImporter, Length) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Length.log");
	Logger::add_stdout_sink();
	std::string str (
		"Length:\n"
		"  filename: length.v\n"
		"  line: 6\n"
		"  msb:\n"
		"  lsb:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Length"]);
	ASSERT_TRUE(yaml["Length"]["filename"].as<std::string>() == "length.v");
	ASSERT_TRUE(yaml["Length"]["line"].as<int>() == 6);
	ASSERT_TRUE(yaml["Length"]["msb"]);
	ASSERT_TRUE(yaml["Length"]["lsb"]);
}


	

TEST(YAMLImporter, Identifier) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Identifier.log");
	Logger::add_stdout_sink();
	std::string str (
		"Identifier:\n"
		"  filename: identifier.v\n"
		"  line: 10\n"
		"  scope:\n"
		"  name: mynbiqpmzj\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Identifier"]);
	ASSERT_TRUE(yaml["Identifier"]["filename"].as<std::string>() == "identifier.v");
	ASSERT_TRUE(yaml["Identifier"]["line"].as<int>() == 10);
	ASSERT_TRUE(yaml["Identifier"]["scope"]);
	ASSERT_TRUE(yaml["Identifier"]["name"].as<std::string>() == "mynbiqpmzj");
}


	

TEST(YAMLImporter, Constant) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Constant.log");
	Logger::add_stdout_sink();
	std::string str (
		"Constant:\n"
		"  filename: constant.v\n"
		"  line: 8\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Constant"]);
	ASSERT_TRUE(yaml["Constant"]["filename"].as<std::string>() == "constant.v");
	ASSERT_TRUE(yaml["Constant"]["line"].as<int>() == 8);
}


	

TEST(YAMLImporter, StringConst) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.StringConst.log");
	Logger::add_stdout_sink();
	std::string str (
		"StringConst:\n"
		"  filename: stringconst.v\n"
		"  line: 11\n"
		"  value: mynbiqpmzj\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["StringConst"]);
	ASSERT_TRUE(yaml["StringConst"]["filename"].as<std::string>() == "stringconst.v");
	ASSERT_TRUE(yaml["StringConst"]["line"].as<int>() == 11);
	ASSERT_TRUE(yaml["StringConst"]["value"].as<std::string>() == "mynbiqpmzj");
}


	

TEST(YAMLImporter, IntConst) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.IntConst.log");
	Logger::add_stdout_sink();
	std::string str (
		"IntConst:\n"
		"  filename: intconst.v\n"
		"  line: 8\n"
		"  value: mynbiqpmzj\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["IntConst"]);
	ASSERT_TRUE(yaml["IntConst"]["filename"].as<std::string>() == "intconst.v");
	ASSERT_TRUE(yaml["IntConst"]["line"].as<int>() == 8);
	ASSERT_TRUE(yaml["IntConst"]["value"].as<std::string>() == "mynbiqpmzj");
}


	

TEST(YAMLImporter, IntConstN) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.IntConstN.log");
	Logger::add_stdout_sink();
	std::string str (
		"IntConstN:\n"
		"  filename: intconstn.v\n"
		"  line: 9\n"
		"  base: 98\n"
		"  size: 107\n"
		"  sign: true\n"
		"  value: 66\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["IntConstN"]);
	ASSERT_TRUE(yaml["IntConstN"]["filename"].as<std::string>() == "intconstn.v");
	ASSERT_TRUE(yaml["IntConstN"]["line"].as<int>() == 9);
	ASSERT_TRUE(yaml["IntConstN"]["base"].as<int>() == 98);
	ASSERT_TRUE(yaml["IntConstN"]["size"].as<int>() == 107);
	ASSERT_TRUE(yaml["IntConstN"]["sign"].as<bool>() == true);
	ASSERT_TRUE(yaml["IntConstN"]["value"].as<mpz_class>() == 66);
}


	

TEST(YAMLImporter, FloatConst) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.FloatConst.log");
	Logger::add_stdout_sink();
	std::string str (
		"FloatConst:\n"
		"  filename: floatconst.v\n"
		"  line: 10\n"
		"  value: 98\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["FloatConst"]);
	ASSERT_TRUE(yaml["FloatConst"]["filename"].as<std::string>() == "floatconst.v");
	ASSERT_TRUE(yaml["FloatConst"]["line"].as<int>() == 10);
	ASSERT_TRUE(yaml["FloatConst"]["value"].as<double>() == 98);
}


	

TEST(YAMLImporter, IODir) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.IODir.log");
	Logger::add_stdout_sink();
	std::string str (
		"IODir:\n"
		"  filename: iodir.v\n"
		"  line: 5\n"
		"  widths:\n"
		"  name: mynbiqpmzj\n"
		"  sign: false\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["IODir"]);
	ASSERT_TRUE(yaml["IODir"]["filename"].as<std::string>() == "iodir.v");
	ASSERT_TRUE(yaml["IODir"]["line"].as<int>() == 5);
	ASSERT_TRUE(yaml["IODir"]["widths"]);
	ASSERT_TRUE(yaml["IODir"]["name"].as<std::string>() == "mynbiqpmzj");
	ASSERT_TRUE(yaml["IODir"]["sign"].as<bool>() == false);
}


	

TEST(YAMLImporter, Input) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Input.log");
	Logger::add_stdout_sink();
	std::string str (
		"Input:\n"
		"  filename: input.v\n"
		"  line: 5\n"
		"  widths:\n"
		"  name: mynbiqpmzj\n"
		"  sign: false\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Input"]);
	ASSERT_TRUE(yaml["Input"]["filename"].as<std::string>() == "input.v");
	ASSERT_TRUE(yaml["Input"]["line"].as<int>() == 5);
	ASSERT_TRUE(yaml["Input"]["widths"]);
	ASSERT_TRUE(yaml["Input"]["name"].as<std::string>() == "mynbiqpmzj");
	ASSERT_TRUE(yaml["Input"]["sign"].as<bool>() == false);
}


	

TEST(YAMLImporter, Output) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Output.log");
	Logger::add_stdout_sink();
	std::string str (
		"Output:\n"
		"  filename: output.v\n"
		"  line: 6\n"
		"  widths:\n"
		"  name: mynbiqpmzj\n"
		"  sign: false\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Output"]);
	ASSERT_TRUE(yaml["Output"]["filename"].as<std::string>() == "output.v");
	ASSERT_TRUE(yaml["Output"]["line"].as<int>() == 6);
	ASSERT_TRUE(yaml["Output"]["widths"]);
	ASSERT_TRUE(yaml["Output"]["name"].as<std::string>() == "mynbiqpmzj");
	ASSERT_TRUE(yaml["Output"]["sign"].as<bool>() == false);
}


	

TEST(YAMLImporter, Inout) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Inout.log");
	Logger::add_stdout_sink();
	std::string str (
		"Inout:\n"
		"  filename: inout.v\n"
		"  line: 5\n"
		"  widths:\n"
		"  name: mynbiqpmzj\n"
		"  sign: false\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Inout"]);
	ASSERT_TRUE(yaml["Inout"]["filename"].as<std::string>() == "inout.v");
	ASSERT_TRUE(yaml["Inout"]["line"].as<int>() == 5);
	ASSERT_TRUE(yaml["Inout"]["widths"]);
	ASSERT_TRUE(yaml["Inout"]["name"].as<std::string>() == "mynbiqpmzj");
	ASSERT_TRUE(yaml["Inout"]["sign"].as<bool>() == false);
}


	

TEST(YAMLImporter, VariableBase) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.VariableBase.log");
	Logger::add_stdout_sink();
	std::string str (
		"VariableBase:\n"
		"  filename: variablebase.v\n"
		"  line: 12\n"
		"  name: mynbiqpmzj\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["VariableBase"]);
	ASSERT_TRUE(yaml["VariableBase"]["filename"].as<std::string>() == "variablebase.v");
	ASSERT_TRUE(yaml["VariableBase"]["line"].as<int>() == 12);
	ASSERT_TRUE(yaml["VariableBase"]["name"].as<std::string>() == "mynbiqpmzj");
}


	

TEST(YAMLImporter, Genvar) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Genvar.log");
	Logger::add_stdout_sink();
	std::string str (
		"Genvar:\n"
		"  filename: genvar.v\n"
		"  line: 6\n"
		"  name: mynbiqpmzj\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Genvar"]);
	ASSERT_TRUE(yaml["Genvar"]["filename"].as<std::string>() == "genvar.v");
	ASSERT_TRUE(yaml["Genvar"]["line"].as<int>() == 6);
	ASSERT_TRUE(yaml["Genvar"]["name"].as<std::string>() == "mynbiqpmzj");
}


	

TEST(YAMLImporter, Variable) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Variable.log");
	Logger::add_stdout_sink();
	std::string str (
		"Variable:\n"
		"  filename: variable.v\n"
		"  line: 8\n"
		"  lengths:\n"
		"  right:\n"
		"  name: mynbiqpmzj\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Variable"]);
	ASSERT_TRUE(yaml["Variable"]["filename"].as<std::string>() == "variable.v");
	ASSERT_TRUE(yaml["Variable"]["line"].as<int>() == 8);
	ASSERT_TRUE(yaml["Variable"]["lengths"]);
	ASSERT_TRUE(yaml["Variable"]["right"]);
	ASSERT_TRUE(yaml["Variable"]["name"].as<std::string>() == "mynbiqpmzj");
}


	

TEST(YAMLImporter, Net) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Net.log");
	Logger::add_stdout_sink();
	std::string str (
		"Net:\n"
		"  filename: net.v\n"
		"  line: 3\n"
		"  widths:\n"
		"  ldelay:\n"
		"  rdelay:\n"
		"  lengths:\n"
		"  right:\n"
		"  sign: false\n"
		"  name: ynbiqpmzjp\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Net"]);
	ASSERT_TRUE(yaml["Net"]["filename"].as<std::string>() == "net.v");
	ASSERT_TRUE(yaml["Net"]["line"].as<int>() == 3);
	ASSERT_TRUE(yaml["Net"]["widths"]);
	ASSERT_TRUE(yaml["Net"]["ldelay"]);
	ASSERT_TRUE(yaml["Net"]["rdelay"]);
	ASSERT_TRUE(yaml["Net"]["lengths"]);
	ASSERT_TRUE(yaml["Net"]["right"]);
	ASSERT_TRUE(yaml["Net"]["sign"].as<bool>() == false);
	ASSERT_TRUE(yaml["Net"]["name"].as<std::string>() == "ynbiqpmzjp");
}


	

TEST(YAMLImporter, Integer) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Integer.log");
	Logger::add_stdout_sink();
	std::string str (
		"Integer:\n"
		"  filename: integer.v\n"
		"  line: 7\n"
		"  lengths:\n"
		"  right:\n"
		"  name: mynbiqpmzj\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Integer"]);
	ASSERT_TRUE(yaml["Integer"]["filename"].as<std::string>() == "integer.v");
	ASSERT_TRUE(yaml["Integer"]["line"].as<int>() == 7);
	ASSERT_TRUE(yaml["Integer"]["lengths"]);
	ASSERT_TRUE(yaml["Integer"]["right"]);
	ASSERT_TRUE(yaml["Integer"]["name"].as<std::string>() == "mynbiqpmzj");
}


	

TEST(YAMLImporter, Real) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Real.log");
	Logger::add_stdout_sink();
	std::string str (
		"Real:\n"
		"  filename: real.v\n"
		"  line: 4\n"
		"  lengths:\n"
		"  right:\n"
		"  name: mynbiqpmzj\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Real"]);
	ASSERT_TRUE(yaml["Real"]["filename"].as<std::string>() == "real.v");
	ASSERT_TRUE(yaml["Real"]["line"].as<int>() == 4);
	ASSERT_TRUE(yaml["Real"]["lengths"]);
	ASSERT_TRUE(yaml["Real"]["right"]);
	ASSERT_TRUE(yaml["Real"]["name"].as<std::string>() == "mynbiqpmzj");
}


	

TEST(YAMLImporter, Tri) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Tri.log");
	Logger::add_stdout_sink();
	std::string str (
		"Tri:\n"
		"  filename: tri.v\n"
		"  line: 3\n"
		"  widths:\n"
		"  ldelay:\n"
		"  rdelay:\n"
		"  lengths:\n"
		"  right:\n"
		"  sign: false\n"
		"  name: ynbiqpmzjp\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Tri"]);
	ASSERT_TRUE(yaml["Tri"]["filename"].as<std::string>() == "tri.v");
	ASSERT_TRUE(yaml["Tri"]["line"].as<int>() == 3);
	ASSERT_TRUE(yaml["Tri"]["widths"]);
	ASSERT_TRUE(yaml["Tri"]["ldelay"]);
	ASSERT_TRUE(yaml["Tri"]["rdelay"]);
	ASSERT_TRUE(yaml["Tri"]["lengths"]);
	ASSERT_TRUE(yaml["Tri"]["right"]);
	ASSERT_TRUE(yaml["Tri"]["sign"].as<bool>() == false);
	ASSERT_TRUE(yaml["Tri"]["name"].as<std::string>() == "ynbiqpmzjp");
}


	

TEST(YAMLImporter, Wire) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Wire.log");
	Logger::add_stdout_sink();
	std::string str (
		"Wire:\n"
		"  filename: wire.v\n"
		"  line: 4\n"
		"  widths:\n"
		"  ldelay:\n"
		"  rdelay:\n"
		"  lengths:\n"
		"  right:\n"
		"  sign: false\n"
		"  name: ynbiqpmzjp\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Wire"]);
	ASSERT_TRUE(yaml["Wire"]["filename"].as<std::string>() == "wire.v");
	ASSERT_TRUE(yaml["Wire"]["line"].as<int>() == 4);
	ASSERT_TRUE(yaml["Wire"]["widths"]);
	ASSERT_TRUE(yaml["Wire"]["ldelay"]);
	ASSERT_TRUE(yaml["Wire"]["rdelay"]);
	ASSERT_TRUE(yaml["Wire"]["lengths"]);
	ASSERT_TRUE(yaml["Wire"]["right"]);
	ASSERT_TRUE(yaml["Wire"]["sign"].as<bool>() == false);
	ASSERT_TRUE(yaml["Wire"]["name"].as<std::string>() == "ynbiqpmzjp");
}


	

TEST(YAMLImporter, Supply0) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Supply0.log");
	Logger::add_stdout_sink();
	std::string str (
		"Supply0:\n"
		"  filename: supply0.v\n"
		"  line: 7\n"
		"  widths:\n"
		"  ldelay:\n"
		"  rdelay:\n"
		"  lengths:\n"
		"  right:\n"
		"  sign: false\n"
		"  name: ynbiqpmzjp\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Supply0"]);
	ASSERT_TRUE(yaml["Supply0"]["filename"].as<std::string>() == "supply0.v");
	ASSERT_TRUE(yaml["Supply0"]["line"].as<int>() == 7);
	ASSERT_TRUE(yaml["Supply0"]["widths"]);
	ASSERT_TRUE(yaml["Supply0"]["ldelay"]);
	ASSERT_TRUE(yaml["Supply0"]["rdelay"]);
	ASSERT_TRUE(yaml["Supply0"]["lengths"]);
	ASSERT_TRUE(yaml["Supply0"]["right"]);
	ASSERT_TRUE(yaml["Supply0"]["sign"].as<bool>() == false);
	ASSERT_TRUE(yaml["Supply0"]["name"].as<std::string>() == "ynbiqpmzjp");
}


	

TEST(YAMLImporter, Supply1) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Supply1.log");
	Logger::add_stdout_sink();
	std::string str (
		"Supply1:\n"
		"  filename: supply1.v\n"
		"  line: 7\n"
		"  widths:\n"
		"  ldelay:\n"
		"  rdelay:\n"
		"  lengths:\n"
		"  right:\n"
		"  sign: false\n"
		"  name: ynbiqpmzjp\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Supply1"]);
	ASSERT_TRUE(yaml["Supply1"]["filename"].as<std::string>() == "supply1.v");
	ASSERT_TRUE(yaml["Supply1"]["line"].as<int>() == 7);
	ASSERT_TRUE(yaml["Supply1"]["widths"]);
	ASSERT_TRUE(yaml["Supply1"]["ldelay"]);
	ASSERT_TRUE(yaml["Supply1"]["rdelay"]);
	ASSERT_TRUE(yaml["Supply1"]["lengths"]);
	ASSERT_TRUE(yaml["Supply1"]["right"]);
	ASSERT_TRUE(yaml["Supply1"]["sign"].as<bool>() == false);
	ASSERT_TRUE(yaml["Supply1"]["name"].as<std::string>() == "ynbiqpmzjp");
}


	

TEST(YAMLImporter, Reg) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Reg.log");
	Logger::add_stdout_sink();
	std::string str (
		"Reg:\n"
		"  filename: reg.v\n"
		"  line: 3\n"
		"  widths:\n"
		"  lengths:\n"
		"  right:\n"
		"  sign: false\n"
		"  name: ynbiqpmzjp\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Reg"]);
	ASSERT_TRUE(yaml["Reg"]["filename"].as<std::string>() == "reg.v");
	ASSERT_TRUE(yaml["Reg"]["line"].as<int>() == 3);
	ASSERT_TRUE(yaml["Reg"]["widths"]);
	ASSERT_TRUE(yaml["Reg"]["lengths"]);
	ASSERT_TRUE(yaml["Reg"]["right"]);
	ASSERT_TRUE(yaml["Reg"]["sign"].as<bool>() == false);
	ASSERT_TRUE(yaml["Reg"]["name"].as<std::string>() == "ynbiqpmzjp");
}


	

TEST(YAMLImporter, Ioport) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Ioport.log");
	Logger::add_stdout_sink();
	std::string str (
		"Ioport:\n"
		"  filename: ioport.v\n"
		"  line: 6\n"
		"  first:\n"
		"  second:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Ioport"]);
	ASSERT_TRUE(yaml["Ioport"]["filename"].as<std::string>() == "ioport.v");
	ASSERT_TRUE(yaml["Ioport"]["line"].as<int>() == 6);
	ASSERT_TRUE(yaml["Ioport"]["first"]);
	ASSERT_TRUE(yaml["Ioport"]["second"]);
}


	

TEST(YAMLImporter, Parameter) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Parameter.log");
	Logger::add_stdout_sink();
	std::string str (
		"Parameter:\n"
		"  filename: parameter.v\n"
		"  line: 9\n"
		"  value:\n"
		"  widths:\n"
		"  name: mynbiqpmzj\n"
		"  sign: false\n"
		"  type: REAL\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Parameter"]);
	ASSERT_TRUE(yaml["Parameter"]["filename"].as<std::string>() == "parameter.v");
	ASSERT_TRUE(yaml["Parameter"]["line"].as<int>() == 9);
	ASSERT_TRUE(yaml["Parameter"]["value"]);
	ASSERT_TRUE(yaml["Parameter"]["widths"]);
	ASSERT_TRUE(yaml["Parameter"]["name"].as<std::string>() == "mynbiqpmzj");
	ASSERT_TRUE(yaml["Parameter"]["sign"].as<bool>() == false);
	ASSERT_TRUE(yaml["Parameter"]["type"].as<AST::Parameter::TypeEnum>() == AST::Parameter::TypeEnum::REAL);
}


	

TEST(YAMLImporter, Localparam) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Localparam.log");
	Logger::add_stdout_sink();
	std::string str (
		"Localparam:\n"
		"  filename: localparam.v\n"
		"  line: 10\n"
		"  value:\n"
		"  widths:\n"
		"  name: mynbiqpmzj\n"
		"  sign: false\n"
		"  type: REAL\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Localparam"]);
	ASSERT_TRUE(yaml["Localparam"]["filename"].as<std::string>() == "localparam.v");
	ASSERT_TRUE(yaml["Localparam"]["line"].as<int>() == 10);
	ASSERT_TRUE(yaml["Localparam"]["value"]);
	ASSERT_TRUE(yaml["Localparam"]["widths"]);
	ASSERT_TRUE(yaml["Localparam"]["name"].as<std::string>() == "mynbiqpmzj");
	ASSERT_TRUE(yaml["Localparam"]["sign"].as<bool>() == false);
	ASSERT_TRUE(yaml["Localparam"]["type"].as<AST::Parameter::TypeEnum>() == AST::Parameter::TypeEnum::REAL);
}


	

TEST(YAMLImporter, Concat) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Concat.log");
	Logger::add_stdout_sink();
	std::string str (
		"Concat:\n"
		"  filename: concat.v\n"
		"  line: 6\n"
		"  list:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Concat"]);
	ASSERT_TRUE(yaml["Concat"]["filename"].as<std::string>() == "concat.v");
	ASSERT_TRUE(yaml["Concat"]["line"].as<int>() == 6);
	ASSERT_TRUE(yaml["Concat"]["list"]);
}


	

TEST(YAMLImporter, Lconcat) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Lconcat.log");
	Logger::add_stdout_sink();
	std::string str (
		"Lconcat:\n"
		"  filename: lconcat.v\n"
		"  line: 7\n"
		"  list:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Lconcat"]);
	ASSERT_TRUE(yaml["Lconcat"]["filename"].as<std::string>() == "lconcat.v");
	ASSERT_TRUE(yaml["Lconcat"]["line"].as<int>() == 7);
	ASSERT_TRUE(yaml["Lconcat"]["list"]);
}


	

TEST(YAMLImporter, Repeat) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Repeat.log");
	Logger::add_stdout_sink();
	std::string str (
		"Repeat:\n"
		"  filename: repeat.v\n"
		"  line: 6\n"
		"  value:\n"
		"  times:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Repeat"]);
	ASSERT_TRUE(yaml["Repeat"]["filename"].as<std::string>() == "repeat.v");
	ASSERT_TRUE(yaml["Repeat"]["line"].as<int>() == 6);
	ASSERT_TRUE(yaml["Repeat"]["value"]);
	ASSERT_TRUE(yaml["Repeat"]["times"]);
}


	

TEST(YAMLImporter, Indirect) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Indirect.log");
	Logger::add_stdout_sink();
	std::string str (
		"Indirect:\n"
		"  filename: indirect.v\n"
		"  line: 8\n"
		"  var:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Indirect"]);
	ASSERT_TRUE(yaml["Indirect"]["filename"].as<std::string>() == "indirect.v");
	ASSERT_TRUE(yaml["Indirect"]["line"].as<int>() == 8);
	ASSERT_TRUE(yaml["Indirect"]["var"]);
}


	

TEST(YAMLImporter, Partselect) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Partselect.log");
	Logger::add_stdout_sink();
	std::string str (
		"Partselect:\n"
		"  filename: partselect.v\n"
		"  line: 10\n"
		"  msb:\n"
		"  lsb:\n"
		"  var:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Partselect"]);
	ASSERT_TRUE(yaml["Partselect"]["filename"].as<std::string>() == "partselect.v");
	ASSERT_TRUE(yaml["Partselect"]["line"].as<int>() == 10);
	ASSERT_TRUE(yaml["Partselect"]["msb"]);
	ASSERT_TRUE(yaml["Partselect"]["lsb"]);
	ASSERT_TRUE(yaml["Partselect"]["var"]);
}


	

TEST(YAMLImporter, PartselectIndexed) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.PartselectIndexed.log");
	Logger::add_stdout_sink();
	std::string str (
		"PartselectIndexed:\n"
		"  filename: partselectindexed.v\n"
		"  line: 17\n"
		"  index:\n"
		"  size:\n"
		"  var:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["PartselectIndexed"]);
	ASSERT_TRUE(yaml["PartselectIndexed"]["filename"].as<std::string>() == "partselectindexed.v");
	ASSERT_TRUE(yaml["PartselectIndexed"]["line"].as<int>() == 17);
	ASSERT_TRUE(yaml["PartselectIndexed"]["index"]);
	ASSERT_TRUE(yaml["PartselectIndexed"]["size"]);
	ASSERT_TRUE(yaml["PartselectIndexed"]["var"]);
}


	

TEST(YAMLImporter, PartselectPlusIndexed) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.PartselectPlusIndexed.log");
	Logger::add_stdout_sink();
	std::string str (
		"PartselectPlusIndexed:\n"
		"  filename: partselectplusindexed.v\n"
		"  line: 21\n"
		"  index:\n"
		"  size:\n"
		"  var:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["PartselectPlusIndexed"]);
	ASSERT_TRUE(yaml["PartselectPlusIndexed"]["filename"].as<std::string>() == "partselectplusindexed.v");
	ASSERT_TRUE(yaml["PartselectPlusIndexed"]["line"].as<int>() == 21);
	ASSERT_TRUE(yaml["PartselectPlusIndexed"]["index"]);
	ASSERT_TRUE(yaml["PartselectPlusIndexed"]["size"]);
	ASSERT_TRUE(yaml["PartselectPlusIndexed"]["var"]);
}


	

TEST(YAMLImporter, PartselectMinusIndexed) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.PartselectMinusIndexed.log");
	Logger::add_stdout_sink();
	std::string str (
		"PartselectMinusIndexed:\n"
		"  filename: partselectminusindexed.v\n"
		"  line: 22\n"
		"  index:\n"
		"  size:\n"
		"  var:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["PartselectMinusIndexed"]);
	ASSERT_TRUE(yaml["PartselectMinusIndexed"]["filename"].as<std::string>() == "partselectminusindexed.v");
	ASSERT_TRUE(yaml["PartselectMinusIndexed"]["line"].as<int>() == 22);
	ASSERT_TRUE(yaml["PartselectMinusIndexed"]["index"]);
	ASSERT_TRUE(yaml["PartselectMinusIndexed"]["size"]);
	ASSERT_TRUE(yaml["PartselectMinusIndexed"]["var"]);
}


	

TEST(YAMLImporter, Pointer) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Pointer.log");
	Logger::add_stdout_sink();
	std::string str (
		"Pointer:\n"
		"  filename: pointer.v\n"
		"  line: 7\n"
		"  ptr:\n"
		"  var:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Pointer"]);
	ASSERT_TRUE(yaml["Pointer"]["filename"].as<std::string>() == "pointer.v");
	ASSERT_TRUE(yaml["Pointer"]["line"].as<int>() == 7);
	ASSERT_TRUE(yaml["Pointer"]["ptr"]);
	ASSERT_TRUE(yaml["Pointer"]["var"]);
}


	

TEST(YAMLImporter, Lvalue) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Lvalue.log");
	Logger::add_stdout_sink();
	std::string str (
		"Lvalue:\n"
		"  filename: lvalue.v\n"
		"  line: 6\n"
		"  var:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Lvalue"]);
	ASSERT_TRUE(yaml["Lvalue"]["filename"].as<std::string>() == "lvalue.v");
	ASSERT_TRUE(yaml["Lvalue"]["line"].as<int>() == 6);
	ASSERT_TRUE(yaml["Lvalue"]["var"]);
}


	

TEST(YAMLImporter, Rvalue) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Rvalue.log");
	Logger::add_stdout_sink();
	std::string str (
		"Rvalue:\n"
		"  filename: rvalue.v\n"
		"  line: 6\n"
		"  var:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Rvalue"]);
	ASSERT_TRUE(yaml["Rvalue"]["filename"].as<std::string>() == "rvalue.v");
	ASSERT_TRUE(yaml["Rvalue"]["line"].as<int>() == 6);
	ASSERT_TRUE(yaml["Rvalue"]["var"]);
}


	

TEST(YAMLImporter, UnaryOperator) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.UnaryOperator.log");
	Logger::add_stdout_sink();
	std::string str (
		"UnaryOperator:\n"
		"  filename: unaryoperator.v\n"
		"  line: 13\n"
		"  right:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["UnaryOperator"]);
	ASSERT_TRUE(yaml["UnaryOperator"]["filename"].as<std::string>() == "unaryoperator.v");
	ASSERT_TRUE(yaml["UnaryOperator"]["line"].as<int>() == 13);
	ASSERT_TRUE(yaml["UnaryOperator"]["right"]);
}


	

TEST(YAMLImporter, Uplus) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Uplus.log");
	Logger::add_stdout_sink();
	std::string str (
		"Uplus:\n"
		"  filename: uplus.v\n"
		"  line: 5\n"
		"  right:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Uplus"]);
	ASSERT_TRUE(yaml["Uplus"]["filename"].as<std::string>() == "uplus.v");
	ASSERT_TRUE(yaml["Uplus"]["line"].as<int>() == 5);
	ASSERT_TRUE(yaml["Uplus"]["right"]);
}


	

TEST(YAMLImporter, Uminus) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Uminus.log");
	Logger::add_stdout_sink();
	std::string str (
		"Uminus:\n"
		"  filename: uminus.v\n"
		"  line: 6\n"
		"  right:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Uminus"]);
	ASSERT_TRUE(yaml["Uminus"]["filename"].as<std::string>() == "uminus.v");
	ASSERT_TRUE(yaml["Uminus"]["line"].as<int>() == 6);
	ASSERT_TRUE(yaml["Uminus"]["right"]);
}


	

TEST(YAMLImporter, Ulnot) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Ulnot.log");
	Logger::add_stdout_sink();
	std::string str (
		"Ulnot:\n"
		"  filename: ulnot.v\n"
		"  line: 5\n"
		"  right:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Ulnot"]);
	ASSERT_TRUE(yaml["Ulnot"]["filename"].as<std::string>() == "ulnot.v");
	ASSERT_TRUE(yaml["Ulnot"]["line"].as<int>() == 5);
	ASSERT_TRUE(yaml["Ulnot"]["right"]);
}


	

TEST(YAMLImporter, Unot) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Unot.log");
	Logger::add_stdout_sink();
	std::string str (
		"Unot:\n"
		"  filename: unot.v\n"
		"  line: 4\n"
		"  right:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Unot"]);
	ASSERT_TRUE(yaml["Unot"]["filename"].as<std::string>() == "unot.v");
	ASSERT_TRUE(yaml["Unot"]["line"].as<int>() == 4);
	ASSERT_TRUE(yaml["Unot"]["right"]);
}


	

TEST(YAMLImporter, Uand) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Uand.log");
	Logger::add_stdout_sink();
	std::string str (
		"Uand:\n"
		"  filename: uand.v\n"
		"  line: 4\n"
		"  right:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Uand"]);
	ASSERT_TRUE(yaml["Uand"]["filename"].as<std::string>() == "uand.v");
	ASSERT_TRUE(yaml["Uand"]["line"].as<int>() == 4);
	ASSERT_TRUE(yaml["Uand"]["right"]);
}


	

TEST(YAMLImporter, Unand) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Unand.log");
	Logger::add_stdout_sink();
	std::string str (
		"Unand:\n"
		"  filename: unand.v\n"
		"  line: 5\n"
		"  right:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Unand"]);
	ASSERT_TRUE(yaml["Unand"]["filename"].as<std::string>() == "unand.v");
	ASSERT_TRUE(yaml["Unand"]["line"].as<int>() == 5);
	ASSERT_TRUE(yaml["Unand"]["right"]);
}


	

TEST(YAMLImporter, Uor) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Uor.log");
	Logger::add_stdout_sink();
	std::string str (
		"Uor:\n"
		"  filename: uor.v\n"
		"  line: 3\n"
		"  right:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Uor"]);
	ASSERT_TRUE(yaml["Uor"]["filename"].as<std::string>() == "uor.v");
	ASSERT_TRUE(yaml["Uor"]["line"].as<int>() == 3);
	ASSERT_TRUE(yaml["Uor"]["right"]);
}


	

TEST(YAMLImporter, Unor) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Unor.log");
	Logger::add_stdout_sink();
	std::string str (
		"Unor:\n"
		"  filename: unor.v\n"
		"  line: 4\n"
		"  right:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Unor"]);
	ASSERT_TRUE(yaml["Unor"]["filename"].as<std::string>() == "unor.v");
	ASSERT_TRUE(yaml["Unor"]["line"].as<int>() == 4);
	ASSERT_TRUE(yaml["Unor"]["right"]);
}


	

TEST(YAMLImporter, Uxor) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Uxor.log");
	Logger::add_stdout_sink();
	std::string str (
		"Uxor:\n"
		"  filename: uxor.v\n"
		"  line: 4\n"
		"  right:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Uxor"]);
	ASSERT_TRUE(yaml["Uxor"]["filename"].as<std::string>() == "uxor.v");
	ASSERT_TRUE(yaml["Uxor"]["line"].as<int>() == 4);
	ASSERT_TRUE(yaml["Uxor"]["right"]);
}


	

TEST(YAMLImporter, Uxnor) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Uxnor.log");
	Logger::add_stdout_sink();
	std::string str (
		"Uxnor:\n"
		"  filename: uxnor.v\n"
		"  line: 5\n"
		"  right:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Uxnor"]);
	ASSERT_TRUE(yaml["Uxnor"]["filename"].as<std::string>() == "uxnor.v");
	ASSERT_TRUE(yaml["Uxnor"]["line"].as<int>() == 5);
	ASSERT_TRUE(yaml["Uxnor"]["right"]);
}


	

TEST(YAMLImporter, Operator) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Operator.log");
	Logger::add_stdout_sink();
	std::string str (
		"Operator:\n"
		"  filename: operator.v\n"
		"  line: 8\n"
		"  left:\n"
		"  right:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Operator"]);
	ASSERT_TRUE(yaml["Operator"]["filename"].as<std::string>() == "operator.v");
	ASSERT_TRUE(yaml["Operator"]["line"].as<int>() == 8);
	ASSERT_TRUE(yaml["Operator"]["left"]);
	ASSERT_TRUE(yaml["Operator"]["right"]);
}


	

TEST(YAMLImporter, Power) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Power.log");
	Logger::add_stdout_sink();
	std::string str (
		"Power:\n"
		"  filename: power.v\n"
		"  line: 5\n"
		"  left:\n"
		"  right:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Power"]);
	ASSERT_TRUE(yaml["Power"]["filename"].as<std::string>() == "power.v");
	ASSERT_TRUE(yaml["Power"]["line"].as<int>() == 5);
	ASSERT_TRUE(yaml["Power"]["left"]);
	ASSERT_TRUE(yaml["Power"]["right"]);
}


	

TEST(YAMLImporter, Times) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Times.log");
	Logger::add_stdout_sink();
	std::string str (
		"Times:\n"
		"  filename: times.v\n"
		"  line: 5\n"
		"  left:\n"
		"  right:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Times"]);
	ASSERT_TRUE(yaml["Times"]["filename"].as<std::string>() == "times.v");
	ASSERT_TRUE(yaml["Times"]["line"].as<int>() == 5);
	ASSERT_TRUE(yaml["Times"]["left"]);
	ASSERT_TRUE(yaml["Times"]["right"]);
}


	

TEST(YAMLImporter, Divide) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Divide.log");
	Logger::add_stdout_sink();
	std::string str (
		"Divide:\n"
		"  filename: divide.v\n"
		"  line: 6\n"
		"  left:\n"
		"  right:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Divide"]);
	ASSERT_TRUE(yaml["Divide"]["filename"].as<std::string>() == "divide.v");
	ASSERT_TRUE(yaml["Divide"]["line"].as<int>() == 6);
	ASSERT_TRUE(yaml["Divide"]["left"]);
	ASSERT_TRUE(yaml["Divide"]["right"]);
}


	

TEST(YAMLImporter, Mod) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Mod.log");
	Logger::add_stdout_sink();
	std::string str (
		"Mod:\n"
		"  filename: mod.v\n"
		"  line: 3\n"
		"  left:\n"
		"  right:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Mod"]);
	ASSERT_TRUE(yaml["Mod"]["filename"].as<std::string>() == "mod.v");
	ASSERT_TRUE(yaml["Mod"]["line"].as<int>() == 3);
	ASSERT_TRUE(yaml["Mod"]["left"]);
	ASSERT_TRUE(yaml["Mod"]["right"]);
}


	

TEST(YAMLImporter, Plus) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Plus.log");
	Logger::add_stdout_sink();
	std::string str (
		"Plus:\n"
		"  filename: plus.v\n"
		"  line: 4\n"
		"  left:\n"
		"  right:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Plus"]);
	ASSERT_TRUE(yaml["Plus"]["filename"].as<std::string>() == "plus.v");
	ASSERT_TRUE(yaml["Plus"]["line"].as<int>() == 4);
	ASSERT_TRUE(yaml["Plus"]["left"]);
	ASSERT_TRUE(yaml["Plus"]["right"]);
}


	

TEST(YAMLImporter, Minus) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Minus.log");
	Logger::add_stdout_sink();
	std::string str (
		"Minus:\n"
		"  filename: minus.v\n"
		"  line: 5\n"
		"  left:\n"
		"  right:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Minus"]);
	ASSERT_TRUE(yaml["Minus"]["filename"].as<std::string>() == "minus.v");
	ASSERT_TRUE(yaml["Minus"]["line"].as<int>() == 5);
	ASSERT_TRUE(yaml["Minus"]["left"]);
	ASSERT_TRUE(yaml["Minus"]["right"]);
}


	

TEST(YAMLImporter, Sll) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Sll.log");
	Logger::add_stdout_sink();
	std::string str (
		"Sll:\n"
		"  filename: sll.v\n"
		"  line: 3\n"
		"  left:\n"
		"  right:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Sll"]);
	ASSERT_TRUE(yaml["Sll"]["filename"].as<std::string>() == "sll.v");
	ASSERT_TRUE(yaml["Sll"]["line"].as<int>() == 3);
	ASSERT_TRUE(yaml["Sll"]["left"]);
	ASSERT_TRUE(yaml["Sll"]["right"]);
}


	

TEST(YAMLImporter, Srl) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Srl.log");
	Logger::add_stdout_sink();
	std::string str (
		"Srl:\n"
		"  filename: srl.v\n"
		"  line: 3\n"
		"  left:\n"
		"  right:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Srl"]);
	ASSERT_TRUE(yaml["Srl"]["filename"].as<std::string>() == "srl.v");
	ASSERT_TRUE(yaml["Srl"]["line"].as<int>() == 3);
	ASSERT_TRUE(yaml["Srl"]["left"]);
	ASSERT_TRUE(yaml["Srl"]["right"]);
}


	

TEST(YAMLImporter, Sra) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Sra.log");
	Logger::add_stdout_sink();
	std::string str (
		"Sra:\n"
		"  filename: sra.v\n"
		"  line: 3\n"
		"  left:\n"
		"  right:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Sra"]);
	ASSERT_TRUE(yaml["Sra"]["filename"].as<std::string>() == "sra.v");
	ASSERT_TRUE(yaml["Sra"]["line"].as<int>() == 3);
	ASSERT_TRUE(yaml["Sra"]["left"]);
	ASSERT_TRUE(yaml["Sra"]["right"]);
}


	

TEST(YAMLImporter, LessThan) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.LessThan.log");
	Logger::add_stdout_sink();
	std::string str (
		"LessThan:\n"
		"  filename: lessthan.v\n"
		"  line: 8\n"
		"  left:\n"
		"  right:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["LessThan"]);
	ASSERT_TRUE(yaml["LessThan"]["filename"].as<std::string>() == "lessthan.v");
	ASSERT_TRUE(yaml["LessThan"]["line"].as<int>() == 8);
	ASSERT_TRUE(yaml["LessThan"]["left"]);
	ASSERT_TRUE(yaml["LessThan"]["right"]);
}


	

TEST(YAMLImporter, GreaterThan) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.GreaterThan.log");
	Logger::add_stdout_sink();
	std::string str (
		"GreaterThan:\n"
		"  filename: greaterthan.v\n"
		"  line: 11\n"
		"  left:\n"
		"  right:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["GreaterThan"]);
	ASSERT_TRUE(yaml["GreaterThan"]["filename"].as<std::string>() == "greaterthan.v");
	ASSERT_TRUE(yaml["GreaterThan"]["line"].as<int>() == 11);
	ASSERT_TRUE(yaml["GreaterThan"]["left"]);
	ASSERT_TRUE(yaml["GreaterThan"]["right"]);
}


	

TEST(YAMLImporter, LessEq) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.LessEq.log");
	Logger::add_stdout_sink();
	std::string str (
		"LessEq:\n"
		"  filename: lesseq.v\n"
		"  line: 6\n"
		"  left:\n"
		"  right:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["LessEq"]);
	ASSERT_TRUE(yaml["LessEq"]["filename"].as<std::string>() == "lesseq.v");
	ASSERT_TRUE(yaml["LessEq"]["line"].as<int>() == 6);
	ASSERT_TRUE(yaml["LessEq"]["left"]);
	ASSERT_TRUE(yaml["LessEq"]["right"]);
}


	

TEST(YAMLImporter, GreaterEq) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.GreaterEq.log");
	Logger::add_stdout_sink();
	std::string str (
		"GreaterEq:\n"
		"  filename: greatereq.v\n"
		"  line: 9\n"
		"  left:\n"
		"  right:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["GreaterEq"]);
	ASSERT_TRUE(yaml["GreaterEq"]["filename"].as<std::string>() == "greatereq.v");
	ASSERT_TRUE(yaml["GreaterEq"]["line"].as<int>() == 9);
	ASSERT_TRUE(yaml["GreaterEq"]["left"]);
	ASSERT_TRUE(yaml["GreaterEq"]["right"]);
}


	

TEST(YAMLImporter, Eq) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Eq.log");
	Logger::add_stdout_sink();
	std::string str (
		"Eq:\n"
		"  filename: eq.v\n"
		"  line: 2\n"
		"  left:\n"
		"  right:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Eq"]);
	ASSERT_TRUE(yaml["Eq"]["filename"].as<std::string>() == "eq.v");
	ASSERT_TRUE(yaml["Eq"]["line"].as<int>() == 2);
	ASSERT_TRUE(yaml["Eq"]["left"]);
	ASSERT_TRUE(yaml["Eq"]["right"]);
}


	

TEST(YAMLImporter, NotEq) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.NotEq.log");
	Logger::add_stdout_sink();
	std::string str (
		"NotEq:\n"
		"  filename: noteq.v\n"
		"  line: 5\n"
		"  left:\n"
		"  right:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["NotEq"]);
	ASSERT_TRUE(yaml["NotEq"]["filename"].as<std::string>() == "noteq.v");
	ASSERT_TRUE(yaml["NotEq"]["line"].as<int>() == 5);
	ASSERT_TRUE(yaml["NotEq"]["left"]);
	ASSERT_TRUE(yaml["NotEq"]["right"]);
}


	

TEST(YAMLImporter, Eql) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Eql.log");
	Logger::add_stdout_sink();
	std::string str (
		"Eql:\n"
		"  filename: eql.v\n"
		"  line: 3\n"
		"  left:\n"
		"  right:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Eql"]);
	ASSERT_TRUE(yaml["Eql"]["filename"].as<std::string>() == "eql.v");
	ASSERT_TRUE(yaml["Eql"]["line"].as<int>() == 3);
	ASSERT_TRUE(yaml["Eql"]["left"]);
	ASSERT_TRUE(yaml["Eql"]["right"]);
}


	

TEST(YAMLImporter, NotEql) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.NotEql.log");
	Logger::add_stdout_sink();
	std::string str (
		"NotEql:\n"
		"  filename: noteql.v\n"
		"  line: 6\n"
		"  left:\n"
		"  right:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["NotEql"]);
	ASSERT_TRUE(yaml["NotEql"]["filename"].as<std::string>() == "noteql.v");
	ASSERT_TRUE(yaml["NotEql"]["line"].as<int>() == 6);
	ASSERT_TRUE(yaml["NotEql"]["left"]);
	ASSERT_TRUE(yaml["NotEql"]["right"]);
}


	

TEST(YAMLImporter, And) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.And.log");
	Logger::add_stdout_sink();
	std::string str (
		"And:\n"
		"  filename: and.v\n"
		"  line: 3\n"
		"  left:\n"
		"  right:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["And"]);
	ASSERT_TRUE(yaml["And"]["filename"].as<std::string>() == "and.v");
	ASSERT_TRUE(yaml["And"]["line"].as<int>() == 3);
	ASSERT_TRUE(yaml["And"]["left"]);
	ASSERT_TRUE(yaml["And"]["right"]);
}


	

TEST(YAMLImporter, Xor) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Xor.log");
	Logger::add_stdout_sink();
	std::string str (
		"Xor:\n"
		"  filename: xor.v\n"
		"  line: 3\n"
		"  left:\n"
		"  right:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Xor"]);
	ASSERT_TRUE(yaml["Xor"]["filename"].as<std::string>() == "xor.v");
	ASSERT_TRUE(yaml["Xor"]["line"].as<int>() == 3);
	ASSERT_TRUE(yaml["Xor"]["left"]);
	ASSERT_TRUE(yaml["Xor"]["right"]);
}


	

TEST(YAMLImporter, Xnor) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Xnor.log");
	Logger::add_stdout_sink();
	std::string str (
		"Xnor:\n"
		"  filename: xnor.v\n"
		"  line: 4\n"
		"  left:\n"
		"  right:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Xnor"]);
	ASSERT_TRUE(yaml["Xnor"]["filename"].as<std::string>() == "xnor.v");
	ASSERT_TRUE(yaml["Xnor"]["line"].as<int>() == 4);
	ASSERT_TRUE(yaml["Xnor"]["left"]);
	ASSERT_TRUE(yaml["Xnor"]["right"]);
}


	

TEST(YAMLImporter, Or) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Or.log");
	Logger::add_stdout_sink();
	std::string str (
		"Or:\n"
		"  filename: or.v\n"
		"  line: 2\n"
		"  left:\n"
		"  right:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Or"]);
	ASSERT_TRUE(yaml["Or"]["filename"].as<std::string>() == "or.v");
	ASSERT_TRUE(yaml["Or"]["line"].as<int>() == 2);
	ASSERT_TRUE(yaml["Or"]["left"]);
	ASSERT_TRUE(yaml["Or"]["right"]);
}


	

TEST(YAMLImporter, Land) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Land.log");
	Logger::add_stdout_sink();
	std::string str (
		"Land:\n"
		"  filename: land.v\n"
		"  line: 4\n"
		"  left:\n"
		"  right:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Land"]);
	ASSERT_TRUE(yaml["Land"]["filename"].as<std::string>() == "land.v");
	ASSERT_TRUE(yaml["Land"]["line"].as<int>() == 4);
	ASSERT_TRUE(yaml["Land"]["left"]);
	ASSERT_TRUE(yaml["Land"]["right"]);
}


	

TEST(YAMLImporter, Lor) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Lor.log");
	Logger::add_stdout_sink();
	std::string str (
		"Lor:\n"
		"  filename: lor.v\n"
		"  line: 3\n"
		"  left:\n"
		"  right:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Lor"]);
	ASSERT_TRUE(yaml["Lor"]["filename"].as<std::string>() == "lor.v");
	ASSERT_TRUE(yaml["Lor"]["line"].as<int>() == 3);
	ASSERT_TRUE(yaml["Lor"]["left"]);
	ASSERT_TRUE(yaml["Lor"]["right"]);
}


	

TEST(YAMLImporter, Cond) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Cond.log");
	Logger::add_stdout_sink();
	std::string str (
		"Cond:\n"
		"  filename: cond.v\n"
		"  line: 4\n"
		"  cond:\n"
		"  left:\n"
		"  right:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Cond"]);
	ASSERT_TRUE(yaml["Cond"]["filename"].as<std::string>() == "cond.v");
	ASSERT_TRUE(yaml["Cond"]["line"].as<int>() == 4);
	ASSERT_TRUE(yaml["Cond"]["cond"]);
	ASSERT_TRUE(yaml["Cond"]["left"]);
	ASSERT_TRUE(yaml["Cond"]["right"]);
}


	

TEST(YAMLImporter, Always) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Always.log");
	Logger::add_stdout_sink();
	std::string str (
		"Always:\n"
		"  filename: always.v\n"
		"  line: 6\n"
		"  senslist:\n"
		"  statement:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Always"]);
	ASSERT_TRUE(yaml["Always"]["filename"].as<std::string>() == "always.v");
	ASSERT_TRUE(yaml["Always"]["line"].as<int>() == 6);
	ASSERT_TRUE(yaml["Always"]["senslist"]);
	ASSERT_TRUE(yaml["Always"]["statement"]);
}


	

TEST(YAMLImporter, Senslist) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Senslist.log");
	Logger::add_stdout_sink();
	std::string str (
		"Senslist:\n"
		"  filename: senslist.v\n"
		"  line: 8\n"
		"  list:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Senslist"]);
	ASSERT_TRUE(yaml["Senslist"]["filename"].as<std::string>() == "senslist.v");
	ASSERT_TRUE(yaml["Senslist"]["line"].as<int>() == 8);
	ASSERT_TRUE(yaml["Senslist"]["list"]);
}


	

TEST(YAMLImporter, Sens) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Sens.log");
	Logger::add_stdout_sink();
	std::string str (
		"Sens:\n"
		"  filename: sens.v\n"
		"  line: 4\n"
		"  sig:\n"
		"  type: NONE\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Sens"]);
	ASSERT_TRUE(yaml["Sens"]["filename"].as<std::string>() == "sens.v");
	ASSERT_TRUE(yaml["Sens"]["line"].as<int>() == 4);
	ASSERT_TRUE(yaml["Sens"]["sig"]);
	ASSERT_TRUE(yaml["Sens"]["type"].as<AST::Sens::TypeEnum>() == AST::Sens::TypeEnum::NONE);
}


	

TEST(YAMLImporter, Defparamlist) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Defparamlist.log");
	Logger::add_stdout_sink();
	std::string str (
		"Defparamlist:\n"
		"  filename: defparamlist.v\n"
		"  line: 12\n"
		"  list:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Defparamlist"]);
	ASSERT_TRUE(yaml["Defparamlist"]["filename"].as<std::string>() == "defparamlist.v");
	ASSERT_TRUE(yaml["Defparamlist"]["line"].as<int>() == 12);
	ASSERT_TRUE(yaml["Defparamlist"]["list"]);
}


	

TEST(YAMLImporter, Defparam) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Defparam.log");
	Logger::add_stdout_sink();
	std::string str (
		"Defparam:\n"
		"  filename: defparam.v\n"
		"  line: 8\n"
		"  identifier:\n"
		"  right:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Defparam"]);
	ASSERT_TRUE(yaml["Defparam"]["filename"].as<std::string>() == "defparam.v");
	ASSERT_TRUE(yaml["Defparam"]["line"].as<int>() == 8);
	ASSERT_TRUE(yaml["Defparam"]["identifier"]);
	ASSERT_TRUE(yaml["Defparam"]["right"]);
}


	

TEST(YAMLImporter, Assign) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Assign.log");
	Logger::add_stdout_sink();
	std::string str (
		"Assign:\n"
		"  filename: assign.v\n"
		"  line: 6\n"
		"  left:\n"
		"  right:\n"
		"  ldelay:\n"
		"  rdelay:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Assign"]);
	ASSERT_TRUE(yaml["Assign"]["filename"].as<std::string>() == "assign.v");
	ASSERT_TRUE(yaml["Assign"]["line"].as<int>() == 6);
	ASSERT_TRUE(yaml["Assign"]["left"]);
	ASSERT_TRUE(yaml["Assign"]["right"]);
	ASSERT_TRUE(yaml["Assign"]["ldelay"]);
	ASSERT_TRUE(yaml["Assign"]["rdelay"]);
}


	

TEST(YAMLImporter, BlockingSubstitution) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.BlockingSubstitution.log");
	Logger::add_stdout_sink();
	std::string str (
		"BlockingSubstitution:\n"
		"  filename: blockingsubstitution.v\n"
		"  line: 20\n"
		"  left:\n"
		"  right:\n"
		"  ldelay:\n"
		"  rdelay:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["BlockingSubstitution"]);
	ASSERT_TRUE(yaml["BlockingSubstitution"]["filename"].as<std::string>() == "blockingsubstitution.v");
	ASSERT_TRUE(yaml["BlockingSubstitution"]["line"].as<int>() == 20);
	ASSERT_TRUE(yaml["BlockingSubstitution"]["left"]);
	ASSERT_TRUE(yaml["BlockingSubstitution"]["right"]);
	ASSERT_TRUE(yaml["BlockingSubstitution"]["ldelay"]);
	ASSERT_TRUE(yaml["BlockingSubstitution"]["rdelay"]);
}


	

TEST(YAMLImporter, NonblockingSubstitution) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.NonblockingSubstitution.log");
	Logger::add_stdout_sink();
	std::string str (
		"NonblockingSubstitution:\n"
		"  filename: nonblockingsubstitution.v\n"
		"  line: 23\n"
		"  left:\n"
		"  right:\n"
		"  ldelay:\n"
		"  rdelay:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["NonblockingSubstitution"]);
	ASSERT_TRUE(yaml["NonblockingSubstitution"]["filename"].as<std::string>() == "nonblockingsubstitution.v");
	ASSERT_TRUE(yaml["NonblockingSubstitution"]["line"].as<int>() == 23);
	ASSERT_TRUE(yaml["NonblockingSubstitution"]["left"]);
	ASSERT_TRUE(yaml["NonblockingSubstitution"]["right"]);
	ASSERT_TRUE(yaml["NonblockingSubstitution"]["ldelay"]);
	ASSERT_TRUE(yaml["NonblockingSubstitution"]["rdelay"]);
}


	

TEST(YAMLImporter, IfStatement) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.IfStatement.log");
	Logger::add_stdout_sink();
	std::string str (
		"IfStatement:\n"
		"  filename: ifstatement.v\n"
		"  line: 11\n"
		"  cond:\n"
		"  true_statement:\n"
		"  false_statement:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["IfStatement"]);
	ASSERT_TRUE(yaml["IfStatement"]["filename"].as<std::string>() == "ifstatement.v");
	ASSERT_TRUE(yaml["IfStatement"]["line"].as<int>() == 11);
	ASSERT_TRUE(yaml["IfStatement"]["cond"]);
	ASSERT_TRUE(yaml["IfStatement"]["true_statement"]);
	ASSERT_TRUE(yaml["IfStatement"]["false_statement"]);
}


	

TEST(YAMLImporter, RepeatStatement) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.RepeatStatement.log");
	Logger::add_stdout_sink();
	std::string str (
		"RepeatStatement:\n"
		"  filename: repeatstatement.v\n"
		"  line: 15\n"
		"  times:\n"
		"  statement:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["RepeatStatement"]);
	ASSERT_TRUE(yaml["RepeatStatement"]["filename"].as<std::string>() == "repeatstatement.v");
	ASSERT_TRUE(yaml["RepeatStatement"]["line"].as<int>() == 15);
	ASSERT_TRUE(yaml["RepeatStatement"]["times"]);
	ASSERT_TRUE(yaml["RepeatStatement"]["statement"]);
}


	

TEST(YAMLImporter, ForStatement) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.ForStatement.log");
	Logger::add_stdout_sink();
	std::string str (
		"ForStatement:\n"
		"  filename: forstatement.v\n"
		"  line: 12\n"
		"  pre:\n"
		"  cond:\n"
		"  post:\n"
		"  statement:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["ForStatement"]);
	ASSERT_TRUE(yaml["ForStatement"]["filename"].as<std::string>() == "forstatement.v");
	ASSERT_TRUE(yaml["ForStatement"]["line"].as<int>() == 12);
	ASSERT_TRUE(yaml["ForStatement"]["pre"]);
	ASSERT_TRUE(yaml["ForStatement"]["cond"]);
	ASSERT_TRUE(yaml["ForStatement"]["post"]);
	ASSERT_TRUE(yaml["ForStatement"]["statement"]);
}


	

TEST(YAMLImporter, WhileStatement) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.WhileStatement.log");
	Logger::add_stdout_sink();
	std::string str (
		"WhileStatement:\n"
		"  filename: whilestatement.v\n"
		"  line: 14\n"
		"  cond:\n"
		"  statement:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["WhileStatement"]);
	ASSERT_TRUE(yaml["WhileStatement"]["filename"].as<std::string>() == "whilestatement.v");
	ASSERT_TRUE(yaml["WhileStatement"]["line"].as<int>() == 14);
	ASSERT_TRUE(yaml["WhileStatement"]["cond"]);
	ASSERT_TRUE(yaml["WhileStatement"]["statement"]);
}


	

TEST(YAMLImporter, CaseStatement) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.CaseStatement.log");
	Logger::add_stdout_sink();
	std::string str (
		"CaseStatement:\n"
		"  filename: casestatement.v\n"
		"  line: 13\n"
		"  comp:\n"
		"  caselist:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["CaseStatement"]);
	ASSERT_TRUE(yaml["CaseStatement"]["filename"].as<std::string>() == "casestatement.v");
	ASSERT_TRUE(yaml["CaseStatement"]["line"].as<int>() == 13);
	ASSERT_TRUE(yaml["CaseStatement"]["comp"]);
	ASSERT_TRUE(yaml["CaseStatement"]["caselist"]);
}


	

TEST(YAMLImporter, CasexStatement) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.CasexStatement.log");
	Logger::add_stdout_sink();
	std::string str (
		"CasexStatement:\n"
		"  filename: casexstatement.v\n"
		"  line: 14\n"
		"  comp:\n"
		"  caselist:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["CasexStatement"]);
	ASSERT_TRUE(yaml["CasexStatement"]["filename"].as<std::string>() == "casexstatement.v");
	ASSERT_TRUE(yaml["CasexStatement"]["line"].as<int>() == 14);
	ASSERT_TRUE(yaml["CasexStatement"]["comp"]);
	ASSERT_TRUE(yaml["CasexStatement"]["caselist"]);
}


	

TEST(YAMLImporter, CasezStatement) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.CasezStatement.log");
	Logger::add_stdout_sink();
	std::string str (
		"CasezStatement:\n"
		"  filename: casezstatement.v\n"
		"  line: 14\n"
		"  comp:\n"
		"  caselist:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["CasezStatement"]);
	ASSERT_TRUE(yaml["CasezStatement"]["filename"].as<std::string>() == "casezstatement.v");
	ASSERT_TRUE(yaml["CasezStatement"]["line"].as<int>() == 14);
	ASSERT_TRUE(yaml["CasezStatement"]["comp"]);
	ASSERT_TRUE(yaml["CasezStatement"]["caselist"]);
}


	

TEST(YAMLImporter, Case) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Case.log");
	Logger::add_stdout_sink();
	std::string str (
		"Case:\n"
		"  filename: case.v\n"
		"  line: 4\n"
		"  cond:\n"
		"  statement:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Case"]);
	ASSERT_TRUE(yaml["Case"]["filename"].as<std::string>() == "case.v");
	ASSERT_TRUE(yaml["Case"]["line"].as<int>() == 4);
	ASSERT_TRUE(yaml["Case"]["cond"]);
	ASSERT_TRUE(yaml["Case"]["statement"]);
}


	

TEST(YAMLImporter, Block) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Block.log");
	Logger::add_stdout_sink();
	std::string str (
		"Block:\n"
		"  filename: block.v\n"
		"  line: 5\n"
		"  statements:\n"
		"  scope: mynbiqpmzj\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Block"]);
	ASSERT_TRUE(yaml["Block"]["filename"].as<std::string>() == "block.v");
	ASSERT_TRUE(yaml["Block"]["line"].as<int>() == 5);
	ASSERT_TRUE(yaml["Block"]["statements"]);
	ASSERT_TRUE(yaml["Block"]["scope"].as<std::string>() == "mynbiqpmzj");
}


	

TEST(YAMLImporter, Initial) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Initial.log");
	Logger::add_stdout_sink();
	std::string str (
		"Initial:\n"
		"  filename: initial.v\n"
		"  line: 7\n"
		"  statement:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Initial"]);
	ASSERT_TRUE(yaml["Initial"]["filename"].as<std::string>() == "initial.v");
	ASSERT_TRUE(yaml["Initial"]["line"].as<int>() == 7);
	ASSERT_TRUE(yaml["Initial"]["statement"]);
}


	

TEST(YAMLImporter, EventStatement) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.EventStatement.log");
	Logger::add_stdout_sink();
	std::string str (
		"EventStatement:\n"
		"  filename: eventstatement.v\n"
		"  line: 14\n"
		"  senslist:\n"
		"  statement:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["EventStatement"]);
	ASSERT_TRUE(yaml["EventStatement"]["filename"].as<std::string>() == "eventstatement.v");
	ASSERT_TRUE(yaml["EventStatement"]["line"].as<int>() == 14);
	ASSERT_TRUE(yaml["EventStatement"]["senslist"]);
	ASSERT_TRUE(yaml["EventStatement"]["statement"]);
}


	

TEST(YAMLImporter, WaitStatement) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.WaitStatement.log");
	Logger::add_stdout_sink();
	std::string str (
		"WaitStatement:\n"
		"  filename: waitstatement.v\n"
		"  line: 13\n"
		"  cond:\n"
		"  statement:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["WaitStatement"]);
	ASSERT_TRUE(yaml["WaitStatement"]["filename"].as<std::string>() == "waitstatement.v");
	ASSERT_TRUE(yaml["WaitStatement"]["line"].as<int>() == 13);
	ASSERT_TRUE(yaml["WaitStatement"]["cond"]);
	ASSERT_TRUE(yaml["WaitStatement"]["statement"]);
}


	

TEST(YAMLImporter, ForeverStatement) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.ForeverStatement.log");
	Logger::add_stdout_sink();
	std::string str (
		"ForeverStatement:\n"
		"  filename: foreverstatement.v\n"
		"  line: 16\n"
		"  statement:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["ForeverStatement"]);
	ASSERT_TRUE(yaml["ForeverStatement"]["filename"].as<std::string>() == "foreverstatement.v");
	ASSERT_TRUE(yaml["ForeverStatement"]["line"].as<int>() == 16);
	ASSERT_TRUE(yaml["ForeverStatement"]["statement"]);
}


	

TEST(YAMLImporter, DelayStatement) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.DelayStatement.log");
	Logger::add_stdout_sink();
	std::string str (
		"DelayStatement:\n"
		"  filename: delaystatement.v\n"
		"  line: 14\n"
		"  delay:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["DelayStatement"]);
	ASSERT_TRUE(yaml["DelayStatement"]["filename"].as<std::string>() == "delaystatement.v");
	ASSERT_TRUE(yaml["DelayStatement"]["line"].as<int>() == 14);
	ASSERT_TRUE(yaml["DelayStatement"]["delay"]);
}


	

TEST(YAMLImporter, Instancelist) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Instancelist.log");
	Logger::add_stdout_sink();
	std::string str (
		"Instancelist:\n"
		"  filename: instancelist.v\n"
		"  line: 12\n"
		"  parameterlist:\n"
		"  instances:\n"
		"  module: mynbiqpmzj\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Instancelist"]);
	ASSERT_TRUE(yaml["Instancelist"]["filename"].as<std::string>() == "instancelist.v");
	ASSERT_TRUE(yaml["Instancelist"]["line"].as<int>() == 12);
	ASSERT_TRUE(yaml["Instancelist"]["parameterlist"]);
	ASSERT_TRUE(yaml["Instancelist"]["instances"]);
	ASSERT_TRUE(yaml["Instancelist"]["module"].as<std::string>() == "mynbiqpmzj");
}


	

TEST(YAMLImporter, Instance) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Instance.log");
	Logger::add_stdout_sink();
	std::string str (
		"Instance:\n"
		"  filename: instance.v\n"
		"  line: 8\n"
		"  array:\n"
		"  parameterlist:\n"
		"  portlist:\n"
		"  module: mynbiqpmzj\n"
		"  name: plsgqejeyd\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Instance"]);
	ASSERT_TRUE(yaml["Instance"]["filename"].as<std::string>() == "instance.v");
	ASSERT_TRUE(yaml["Instance"]["line"].as<int>() == 8);
	ASSERT_TRUE(yaml["Instance"]["array"]);
	ASSERT_TRUE(yaml["Instance"]["parameterlist"]);
	ASSERT_TRUE(yaml["Instance"]["portlist"]);
	ASSERT_TRUE(yaml["Instance"]["module"].as<std::string>() == "mynbiqpmzj");
	ASSERT_TRUE(yaml["Instance"]["name"].as<std::string>() == "plsgqejeyd");
}


	

TEST(YAMLImporter, ParamArg) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.ParamArg.log");
	Logger::add_stdout_sink();
	std::string str (
		"ParamArg:\n"
		"  filename: paramarg.v\n"
		"  line: 8\n"
		"  value:\n"
		"  name: mynbiqpmzj\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["ParamArg"]);
	ASSERT_TRUE(yaml["ParamArg"]["filename"].as<std::string>() == "paramarg.v");
	ASSERT_TRUE(yaml["ParamArg"]["line"].as<int>() == 8);
	ASSERT_TRUE(yaml["ParamArg"]["value"]);
	ASSERT_TRUE(yaml["ParamArg"]["name"].as<std::string>() == "mynbiqpmzj");
}


	

TEST(YAMLImporter, PortArg) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.PortArg.log");
	Logger::add_stdout_sink();
	std::string str (
		"PortArg:\n"
		"  filename: portarg.v\n"
		"  line: 7\n"
		"  value:\n"
		"  name: mynbiqpmzj\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["PortArg"]);
	ASSERT_TRUE(yaml["PortArg"]["filename"].as<std::string>() == "portarg.v");
	ASSERT_TRUE(yaml["PortArg"]["line"].as<int>() == 7);
	ASSERT_TRUE(yaml["PortArg"]["value"]);
	ASSERT_TRUE(yaml["PortArg"]["name"].as<std::string>() == "mynbiqpmzj");
}


	

TEST(YAMLImporter, Function) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Function.log");
	Logger::add_stdout_sink();
	std::string str (
		"Function:\n"
		"  filename: function.v\n"
		"  line: 8\n"
		"  retwidths:\n"
		"  ports:\n"
		"  statements:\n"
		"  name: mynbiqpmzj\n"
		"  automatic: false\n"
		"  rettype: REAL\n"
		"  retsign: true\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Function"]);
	ASSERT_TRUE(yaml["Function"]["filename"].as<std::string>() == "function.v");
	ASSERT_TRUE(yaml["Function"]["line"].as<int>() == 8);
	ASSERT_TRUE(yaml["Function"]["retwidths"]);
	ASSERT_TRUE(yaml["Function"]["ports"]);
	ASSERT_TRUE(yaml["Function"]["statements"]);
	ASSERT_TRUE(yaml["Function"]["name"].as<std::string>() == "mynbiqpmzj");
	ASSERT_TRUE(yaml["Function"]["automatic"].as<bool>() == false);
	ASSERT_TRUE(yaml["Function"]["rettype"].as<AST::Function::RettypeEnum>() == AST::Function::RettypeEnum::REAL);
	ASSERT_TRUE(yaml["Function"]["retsign"].as<bool>() == true);
}


	

TEST(YAMLImporter, FunctionCall) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.FunctionCall.log");
	Logger::add_stdout_sink();
	std::string str (
		"FunctionCall:\n"
		"  filename: functioncall.v\n"
		"  line: 12\n"
		"  args:\n"
		"  name: mynbiqpmzj\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["FunctionCall"]);
	ASSERT_TRUE(yaml["FunctionCall"]["filename"].as<std::string>() == "functioncall.v");
	ASSERT_TRUE(yaml["FunctionCall"]["line"].as<int>() == 12);
	ASSERT_TRUE(yaml["FunctionCall"]["args"]);
	ASSERT_TRUE(yaml["FunctionCall"]["name"].as<std::string>() == "mynbiqpmzj");
}


	

TEST(YAMLImporter, Task) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Task.log");
	Logger::add_stdout_sink();
	std::string str (
		"Task:\n"
		"  filename: task.v\n"
		"  line: 4\n"
		"  ports:\n"
		"  statements:\n"
		"  name: mynbiqpmzj\n"
		"  automatic: false\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Task"]);
	ASSERT_TRUE(yaml["Task"]["filename"].as<std::string>() == "task.v");
	ASSERT_TRUE(yaml["Task"]["line"].as<int>() == 4);
	ASSERT_TRUE(yaml["Task"]["ports"]);
	ASSERT_TRUE(yaml["Task"]["statements"]);
	ASSERT_TRUE(yaml["Task"]["name"].as<std::string>() == "mynbiqpmzj");
	ASSERT_TRUE(yaml["Task"]["automatic"].as<bool>() == false);
}


	

TEST(YAMLImporter, TaskCall) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.TaskCall.log");
	Logger::add_stdout_sink();
	std::string str (
		"TaskCall:\n"
		"  filename: taskcall.v\n"
		"  line: 8\n"
		"  args:\n"
		"  name: mynbiqpmzj\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["TaskCall"]);
	ASSERT_TRUE(yaml["TaskCall"]["filename"].as<std::string>() == "taskcall.v");
	ASSERT_TRUE(yaml["TaskCall"]["line"].as<int>() == 8);
	ASSERT_TRUE(yaml["TaskCall"]["args"]);
	ASSERT_TRUE(yaml["TaskCall"]["name"].as<std::string>() == "mynbiqpmzj");
}


	

TEST(YAMLImporter, GenerateStatement) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.GenerateStatement.log");
	Logger::add_stdout_sink();
	std::string str (
		"GenerateStatement:\n"
		"  filename: generatestatement.v\n"
		"  line: 17\n"
		"  items:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["GenerateStatement"]);
	ASSERT_TRUE(yaml["GenerateStatement"]["filename"].as<std::string>() == "generatestatement.v");
	ASSERT_TRUE(yaml["GenerateStatement"]["line"].as<int>() == 17);
	ASSERT_TRUE(yaml["GenerateStatement"]["items"]);
}


	

TEST(YAMLImporter, SystemCall) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.SystemCall.log");
	Logger::add_stdout_sink();
	std::string str (
		"SystemCall:\n"
		"  filename: systemcall.v\n"
		"  line: 10\n"
		"  args:\n"
		"  syscall: mynbiqpmzj\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["SystemCall"]);
	ASSERT_TRUE(yaml["SystemCall"]["filename"].as<std::string>() == "systemcall.v");
	ASSERT_TRUE(yaml["SystemCall"]["line"].as<int>() == 10);
	ASSERT_TRUE(yaml["SystemCall"]["args"]);
	ASSERT_TRUE(yaml["SystemCall"]["syscall"].as<std::string>() == "mynbiqpmzj");
}


	

TEST(YAMLImporter, IdentifierScopeLabel) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.IdentifierScopeLabel.log");
	Logger::add_stdout_sink();
	std::string str (
		"IdentifierScopeLabel:\n"
		"  filename: identifierscopelabel.v\n"
		"  line: 20\n"
		"  loop:\n"
		"  name: mynbiqpmzj\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["IdentifierScopeLabel"]);
	ASSERT_TRUE(yaml["IdentifierScopeLabel"]["filename"].as<std::string>() == "identifierscopelabel.v");
	ASSERT_TRUE(yaml["IdentifierScopeLabel"]["line"].as<int>() == 20);
	ASSERT_TRUE(yaml["IdentifierScopeLabel"]["loop"]);
	ASSERT_TRUE(yaml["IdentifierScopeLabel"]["name"].as<std::string>() == "mynbiqpmzj");
}


	

TEST(YAMLImporter, IdentifierScope) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.IdentifierScope.log");
	Logger::add_stdout_sink();
	std::string str (
		"IdentifierScope:\n"
		"  filename: identifierscope.v\n"
		"  line: 15\n"
		"  labellist:\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["IdentifierScope"]);
	ASSERT_TRUE(yaml["IdentifierScope"]["filename"].as<std::string>() == "identifierscope.v");
	ASSERT_TRUE(yaml["IdentifierScope"]["line"].as<int>() == 15);
	ASSERT_TRUE(yaml["IdentifierScope"]["labellist"]);
}


	

TEST(YAMLImporter, Disable) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.Disable.log");
	Logger::add_stdout_sink();
	std::string str (
		"Disable:\n"
		"  filename: disable.v\n"
		"  line: 7\n"
		"  dest: mynbiqpmzj\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["Disable"]);
	ASSERT_TRUE(yaml["Disable"]["filename"].as<std::string>() == "disable.v");
	ASSERT_TRUE(yaml["Disable"]["line"].as<int>() == 7);
	ASSERT_TRUE(yaml["Disable"]["dest"].as<std::string>() == "mynbiqpmzj");
}


	

TEST(YAMLImporter, ParallelBlock) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.ParallelBlock.log");
	Logger::add_stdout_sink();
	std::string str (
		"ParallelBlock:\n"
		"  filename: parallelblock.v\n"
		"  line: 13\n"
		"  statements:\n"
		"  scope: mynbiqpmzj\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["ParallelBlock"]);
	ASSERT_TRUE(yaml["ParallelBlock"]["filename"].as<std::string>() == "parallelblock.v");
	ASSERT_TRUE(yaml["ParallelBlock"]["line"].as<int>() == 13);
	ASSERT_TRUE(yaml["ParallelBlock"]["statements"]);
	ASSERT_TRUE(yaml["ParallelBlock"]["scope"].as<std::string>() == "mynbiqpmzj");
}


	

TEST(YAMLImporter, SingleStatement) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.SingleStatement.log");
	Logger::add_stdout_sink();
	std::string str (
		"SingleStatement:\n"
		"  filename: singlestatement.v\n"
		"  line: 15\n"
		"  statement:\n"
		"  delay:\n"
		"  scope: mynbiqpmzj\n"
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["SingleStatement"]);
	ASSERT_TRUE(yaml["SingleStatement"]["filename"].as<std::string>() == "singlestatement.v");
	ASSERT_TRUE(yaml["SingleStatement"]["line"].as<int>() == 15);
	ASSERT_TRUE(yaml["SingleStatement"]["statement"]);
	ASSERT_TRUE(yaml["SingleStatement"]["delay"]);
	ASSERT_TRUE(yaml["SingleStatement"]["scope"].as<std::string>() == "mynbiqpmzj");
}
