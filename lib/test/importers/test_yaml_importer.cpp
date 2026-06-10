// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <gtest/gtest.h>
#include <yaml-cpp/yaml.h>
#include <veriparse/generators/yaml_generator.hpp>
#include <veriparse/logger/logger.hpp>
#include <veriparse/importers/yaml_importer.hpp>
#include <veriparse/importers/yaml_specializations.hpp>
#include <iostream>

using namespace Veriparse;

TEST(YAMLImporter, MySource)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.MySource.log");
    Logger::add_stderr_sink();

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
    ASSERT_EQ(module0["default_nettype"].as<AST::Module::Default_nettypeEnum>(),
              AST::Module::Default_nettypeEnum::REG);

    ASSERT_EQ(module1["line"].as<int>(), 7);
    ASSERT_EQ(module1["filename"].as<std::string>(), "test.v");
    ASSERT_EQ(module1["name"].as<std::string>(), "module1");
    ASSERT_EQ(module1["default_nettype"].as<AST::Module::Default_nettypeEnum>(),
              AST::Module::Default_nettypeEnum::WIRE);
}

TEST(YAMLImporter, Source)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Source.log");
    Logger::add_stderr_sink();
    std::string str("Source:\n"
                    "  filename: source.v\n"
                    "  line: 6\n"
                    "  description:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Source"]);
    ASSERT_TRUE(yaml["Source"]["filename"].as<std::string>() == "source.v");
    ASSERT_TRUE(yaml["Source"]["line"].as<int>() == 6);
    ASSERT_TRUE(yaml["Source"]["description"]);
}

TEST(YAMLImporter, Description)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Description.log");
    Logger::add_stderr_sink();
    std::string str("Description:\n"
                    "  filename: description.v\n"
                    "  line: 11\n"
                    "  definitions:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Description"]);
    ASSERT_TRUE(yaml["Description"]["filename"].as<std::string>() == "description.v");
    ASSERT_TRUE(yaml["Description"]["line"].as<int>() == 11);
    ASSERT_TRUE(yaml["Description"]["definitions"]);
}

TEST(YAMLImporter, Pragmalist)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Pragmalist.log");
    Logger::add_stderr_sink();
    std::string str("Pragmalist:\n"
                    "  filename: pragmalist.v\n"
                    "  line: 10\n"
                    "  pragmas:\n"
                    "  statements:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Pragmalist"]);
    ASSERT_TRUE(yaml["Pragmalist"]["filename"].as<std::string>() == "pragmalist.v");
    ASSERT_TRUE(yaml["Pragmalist"]["line"].as<int>() == 10);
    ASSERT_TRUE(yaml["Pragmalist"]["pragmas"]);
    ASSERT_TRUE(yaml["Pragmalist"]["statements"]);
}

TEST(YAMLImporter, Pragma)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Pragma.log");
    Logger::add_stderr_sink();
    std::string str("Pragma:\n"
                    "  filename: pragma.v\n"
                    "  line: 6\n"
                    "  expression:\n"
                    "  name: mynbiqpmzj\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Pragma"]);
    ASSERT_TRUE(yaml["Pragma"]["filename"].as<std::string>() == "pragma.v");
    ASSERT_TRUE(yaml["Pragma"]["line"].as<int>() == 6);
    ASSERT_TRUE(yaml["Pragma"]["expression"]);
    ASSERT_TRUE(yaml["Pragma"]["name"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLImporter, Module)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Module.log");
    Logger::add_stderr_sink();
    std::string str("Module:\n"
                    "  filename: module.v\n"
                    "  line: 6\n"
                    "  params:\n"
                    "  ports:\n"
                    "  items:\n"
                    "  name: mynbiqpmzj\n"
                    "  lifetime: AUTOMATIC\n"
                    "  default_nettype: TRIOR\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Module"]);
    ASSERT_TRUE(yaml["Module"]["filename"].as<std::string>() == "module.v");
    ASSERT_TRUE(yaml["Module"]["line"].as<int>() == 6);
    ASSERT_TRUE(yaml["Module"]["params"]);
    ASSERT_TRUE(yaml["Module"]["ports"]);
    ASSERT_TRUE(yaml["Module"]["items"]);
    ASSERT_TRUE(yaml["Module"]["name"].as<std::string>() == "mynbiqpmzj");
    ASSERT_TRUE(yaml["Module"]["lifetime"].as<AST::Module::LifetimeEnum>() ==
                AST::Module::LifetimeEnum::AUTOMATIC);
    ASSERT_TRUE(yaml["Module"]["default_nettype"].as<AST::Module::Default_nettypeEnum>() ==
                AST::Module::Default_nettypeEnum::TRIOR);
}

TEST(YAMLImporter, Port)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Port.log");
    Logger::add_stderr_sink();
    std::string str("Port:\n"
                    "  filename: port.v\n"
                    "  line: 4\n"
                    "  decl:\n"
                    "  expr:\n"
                    "  name: mynbiqpmzj\n"
                    "  direction: INOUT\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Port"]);
    ASSERT_TRUE(yaml["Port"]["filename"].as<std::string>() == "port.v");
    ASSERT_TRUE(yaml["Port"]["line"].as<int>() == 4);
    ASSERT_TRUE(yaml["Port"]["decl"]);
    ASSERT_TRUE(yaml["Port"]["expr"]);
    ASSERT_TRUE(yaml["Port"]["name"].as<std::string>() == "mynbiqpmzj");
    ASSERT_TRUE(yaml["Port"]["direction"].as<AST::Port::DirectionEnum>() ==
                AST::Port::DirectionEnum::INOUT);
}

TEST(YAMLImporter, Package)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Package.log");
    Logger::add_stderr_sink();
    std::string str("Package:\n"
                    "  filename: package.v\n"
                    "  line: 7\n"
                    "  items:\n"
                    "  name: mynbiqpmzj\n"
                    "  lifetime: AUTOMATIC\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Package"]);
    ASSERT_TRUE(yaml["Package"]["filename"].as<std::string>() == "package.v");
    ASSERT_TRUE(yaml["Package"]["line"].as<int>() == 7);
    ASSERT_TRUE(yaml["Package"]["items"]);
    ASSERT_TRUE(yaml["Package"]["name"].as<std::string>() == "mynbiqpmzj");
    ASSERT_TRUE(yaml["Package"]["lifetime"].as<AST::Package::LifetimeEnum>() ==
                AST::Package::LifetimeEnum::AUTOMATIC);
}

TEST(YAMLImporter, Import)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Import.log");
    Logger::add_stderr_sink();
    std::string str("Import:\n"
                    "  filename: import.v\n"
                    "  line: 6\n"
                    "  package: mynbiqpmzj\n"
                    "  symbol: plsgqejeyd\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Import"]);
    ASSERT_TRUE(yaml["Import"]["filename"].as<std::string>() == "import.v");
    ASSERT_TRUE(yaml["Import"]["line"].as<int>() == 6);
    ASSERT_TRUE(yaml["Import"]["package"].as<std::string>() == "mynbiqpmzj");
    ASSERT_TRUE(yaml["Import"]["symbol"].as<std::string>() == "plsgqejeyd");
}

TEST(YAMLImporter, Identifier)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Identifier.log");
    Logger::add_stderr_sink();
    std::string str("Identifier:\n"
                    "  filename: identifier.v\n"
                    "  line: 10\n"
                    "  scope:\n"
                    "  name: mynbiqpmzj\n"
                    "  package: plsgqejeyd\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Identifier"]);
    ASSERT_TRUE(yaml["Identifier"]["filename"].as<std::string>() == "identifier.v");
    ASSERT_TRUE(yaml["Identifier"]["line"].as<int>() == 10);
    ASSERT_TRUE(yaml["Identifier"]["scope"]);
    ASSERT_TRUE(yaml["Identifier"]["name"].as<std::string>() == "mynbiqpmzj");
    ASSERT_TRUE(yaml["Identifier"]["package"].as<std::string>() == "plsgqejeyd");
}

TEST(YAMLImporter, Constant)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Constant.log");
    Logger::add_stderr_sink();
    std::string str("Constant:\n"
                    "  filename: constant.v\n"
                    "  line: 8\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Constant"]);
    ASSERT_TRUE(yaml["Constant"]["filename"].as<std::string>() == "constant.v");
    ASSERT_TRUE(yaml["Constant"]["line"].as<int>() == 8);
}

TEST(YAMLImporter, StringConst)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.StringConst.log");
    Logger::add_stderr_sink();
    std::string str("StringConst:\n"
                    "  filename: stringconst.v\n"
                    "  line: 11\n"
                    "  value: mynbiqpmzj\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["StringConst"]);
    ASSERT_TRUE(yaml["StringConst"]["filename"].as<std::string>() == "stringconst.v");
    ASSERT_TRUE(yaml["StringConst"]["line"].as<int>() == 11);
    ASSERT_TRUE(yaml["StringConst"]["value"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLImporter, IntConst)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.IntConst.log");
    Logger::add_stderr_sink();
    std::string str("IntConst:\n"
                    "  filename: intconst.v\n"
                    "  line: 8\n"
                    "  value: mynbiqpmzj\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["IntConst"]);
    ASSERT_TRUE(yaml["IntConst"]["filename"].as<std::string>() == "intconst.v");
    ASSERT_TRUE(yaml["IntConst"]["line"].as<int>() == 8);
    ASSERT_TRUE(yaml["IntConst"]["value"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLImporter, IntConstN)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.IntConstN.log");
    Logger::add_stderr_sink();
    std::string str("IntConstN:\n"
                    "  filename: intconstn.v\n"
                    "  line: 9\n"
                    "  base: 98\n"
                    "  size: 107\n"
                    "  sign: true\n"
                    "  value: 66\n");

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

TEST(YAMLImporter, FloatConst)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.FloatConst.log");
    Logger::add_stderr_sink();
    std::string str("FloatConst:\n"
                    "  filename: floatconst.v\n"
                    "  line: 10\n"
                    "  value: 98\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["FloatConst"]);
    ASSERT_TRUE(yaml["FloatConst"]["filename"].as<std::string>() == "floatconst.v");
    ASSERT_TRUE(yaml["FloatConst"]["line"].as<int>() == 10);
    ASSERT_TRUE(yaml["FloatConst"]["value"].as<double>() == 98);
}

TEST(YAMLImporter, DataType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.DataType.log");
    Logger::add_stderr_sink();
    std::string str("DataType:\n"
                    "  filename: datatype.v\n"
                    "  line: 8\n"
                    "  packed_dims:\n"
                    "  signing: SIGNED\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["DataType"]);
    ASSERT_TRUE(yaml["DataType"]["filename"].as<std::string>() == "datatype.v");
    ASSERT_TRUE(yaml["DataType"]["line"].as<int>() == 8);
    ASSERT_TRUE(yaml["DataType"]["packed_dims"]);
    ASSERT_TRUE(yaml["DataType"]["signing"].as<AST::DataType::SigningEnum>() ==
                AST::DataType::SigningEnum::SIGNED);
}

TEST(YAMLImporter, LogicType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.LogicType.log");
    Logger::add_stderr_sink();
    std::string str("LogicType:\n"
                    "  filename: logictype.v\n"
                    "  line: 9\n"
                    "  packed_dims:\n"
                    "  signing: SIGNED\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["LogicType"]);
    ASSERT_TRUE(yaml["LogicType"]["filename"].as<std::string>() == "logictype.v");
    ASSERT_TRUE(yaml["LogicType"]["line"].as<int>() == 9);
    ASSERT_TRUE(yaml["LogicType"]["packed_dims"]);
    ASSERT_TRUE(yaml["LogicType"]["signing"].as<AST::DataType::SigningEnum>() ==
                AST::DataType::SigningEnum::SIGNED);
}

TEST(YAMLImporter, RegType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.RegType.log");
    Logger::add_stderr_sink();
    std::string str("RegType:\n"
                    "  filename: regtype.v\n"
                    "  line: 7\n"
                    "  packed_dims:\n"
                    "  signing: SIGNED\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["RegType"]);
    ASSERT_TRUE(yaml["RegType"]["filename"].as<std::string>() == "regtype.v");
    ASSERT_TRUE(yaml["RegType"]["line"].as<int>() == 7);
    ASSERT_TRUE(yaml["RegType"]["packed_dims"]);
    ASSERT_TRUE(yaml["RegType"]["signing"].as<AST::DataType::SigningEnum>() ==
                AST::DataType::SigningEnum::SIGNED);
}

TEST(YAMLImporter, BitType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.BitType.log");
    Logger::add_stderr_sink();
    std::string str("BitType:\n"
                    "  filename: bittype.v\n"
                    "  line: 7\n"
                    "  packed_dims:\n"
                    "  signing: SIGNED\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["BitType"]);
    ASSERT_TRUE(yaml["BitType"]["filename"].as<std::string>() == "bittype.v");
    ASSERT_TRUE(yaml["BitType"]["line"].as<int>() == 7);
    ASSERT_TRUE(yaml["BitType"]["packed_dims"]);
    ASSERT_TRUE(yaml["BitType"]["signing"].as<AST::DataType::SigningEnum>() ==
                AST::DataType::SigningEnum::SIGNED);
}

TEST(YAMLImporter, ByteType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.ByteType.log");
    Logger::add_stderr_sink();
    std::string str("ByteType:\n"
                    "  filename: bytetype.v\n"
                    "  line: 8\n"
                    "  packed_dims:\n"
                    "  signing: SIGNED\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["ByteType"]);
    ASSERT_TRUE(yaml["ByteType"]["filename"].as<std::string>() == "bytetype.v");
    ASSERT_TRUE(yaml["ByteType"]["line"].as<int>() == 8);
    ASSERT_TRUE(yaml["ByteType"]["packed_dims"]);
    ASSERT_TRUE(yaml["ByteType"]["signing"].as<AST::DataType::SigningEnum>() ==
                AST::DataType::SigningEnum::SIGNED);
}

TEST(YAMLImporter, ShortintType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.ShortintType.log");
    Logger::add_stderr_sink();
    std::string str("ShortintType:\n"
                    "  filename: shortinttype.v\n"
                    "  line: 12\n"
                    "  packed_dims:\n"
                    "  signing: SIGNED\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["ShortintType"]);
    ASSERT_TRUE(yaml["ShortintType"]["filename"].as<std::string>() == "shortinttype.v");
    ASSERT_TRUE(yaml["ShortintType"]["line"].as<int>() == 12);
    ASSERT_TRUE(yaml["ShortintType"]["packed_dims"]);
    ASSERT_TRUE(yaml["ShortintType"]["signing"].as<AST::DataType::SigningEnum>() ==
                AST::DataType::SigningEnum::SIGNED);
}

TEST(YAMLImporter, IntType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.IntType.log");
    Logger::add_stderr_sink();
    std::string str("IntType:\n"
                    "  filename: inttype.v\n"
                    "  line: 7\n"
                    "  packed_dims:\n"
                    "  signing: SIGNED\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["IntType"]);
    ASSERT_TRUE(yaml["IntType"]["filename"].as<std::string>() == "inttype.v");
    ASSERT_TRUE(yaml["IntType"]["line"].as<int>() == 7);
    ASSERT_TRUE(yaml["IntType"]["packed_dims"]);
    ASSERT_TRUE(yaml["IntType"]["signing"].as<AST::DataType::SigningEnum>() ==
                AST::DataType::SigningEnum::SIGNED);
}

TEST(YAMLImporter, LongintType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.LongintType.log");
    Logger::add_stderr_sink();
    std::string str("LongintType:\n"
                    "  filename: longinttype.v\n"
                    "  line: 11\n"
                    "  packed_dims:\n"
                    "  signing: SIGNED\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["LongintType"]);
    ASSERT_TRUE(yaml["LongintType"]["filename"].as<std::string>() == "longinttype.v");
    ASSERT_TRUE(yaml["LongintType"]["line"].as<int>() == 11);
    ASSERT_TRUE(yaml["LongintType"]["packed_dims"]);
    ASSERT_TRUE(yaml["LongintType"]["signing"].as<AST::DataType::SigningEnum>() ==
                AST::DataType::SigningEnum::SIGNED);
}

TEST(YAMLImporter, IntegerType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.IntegerType.log");
    Logger::add_stderr_sink();
    std::string str("IntegerType:\n"
                    "  filename: integertype.v\n"
                    "  line: 11\n"
                    "  packed_dims:\n"
                    "  signing: SIGNED\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["IntegerType"]);
    ASSERT_TRUE(yaml["IntegerType"]["filename"].as<std::string>() == "integertype.v");
    ASSERT_TRUE(yaml["IntegerType"]["line"].as<int>() == 11);
    ASSERT_TRUE(yaml["IntegerType"]["packed_dims"]);
    ASSERT_TRUE(yaml["IntegerType"]["signing"].as<AST::DataType::SigningEnum>() ==
                AST::DataType::SigningEnum::SIGNED);
}

TEST(YAMLImporter, TimeType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.TimeType.log");
    Logger::add_stderr_sink();
    std::string str("TimeType:\n"
                    "  filename: timetype.v\n"
                    "  line: 8\n"
                    "  packed_dims:\n"
                    "  signing: SIGNED\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["TimeType"]);
    ASSERT_TRUE(yaml["TimeType"]["filename"].as<std::string>() == "timetype.v");
    ASSERT_TRUE(yaml["TimeType"]["line"].as<int>() == 8);
    ASSERT_TRUE(yaml["TimeType"]["packed_dims"]);
    ASSERT_TRUE(yaml["TimeType"]["signing"].as<AST::DataType::SigningEnum>() ==
                AST::DataType::SigningEnum::SIGNED);
}

TEST(YAMLImporter, RealType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.RealType.log");
    Logger::add_stderr_sink();
    std::string str("RealType:\n"
                    "  filename: realtype.v\n"
                    "  line: 8\n"
                    "  packed_dims:\n"
                    "  signing: SIGNED\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["RealType"]);
    ASSERT_TRUE(yaml["RealType"]["filename"].as<std::string>() == "realtype.v");
    ASSERT_TRUE(yaml["RealType"]["line"].as<int>() == 8);
    ASSERT_TRUE(yaml["RealType"]["packed_dims"]);
    ASSERT_TRUE(yaml["RealType"]["signing"].as<AST::DataType::SigningEnum>() ==
                AST::DataType::SigningEnum::SIGNED);
}

TEST(YAMLImporter, ShortrealType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.ShortrealType.log");
    Logger::add_stderr_sink();
    std::string str("ShortrealType:\n"
                    "  filename: shortrealtype.v\n"
                    "  line: 13\n"
                    "  packed_dims:\n"
                    "  signing: SIGNED\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["ShortrealType"]);
    ASSERT_TRUE(yaml["ShortrealType"]["filename"].as<std::string>() == "shortrealtype.v");
    ASSERT_TRUE(yaml["ShortrealType"]["line"].as<int>() == 13);
    ASSERT_TRUE(yaml["ShortrealType"]["packed_dims"]);
    ASSERT_TRUE(yaml["ShortrealType"]["signing"].as<AST::DataType::SigningEnum>() ==
                AST::DataType::SigningEnum::SIGNED);
}

TEST(YAMLImporter, RealtimeType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.RealtimeType.log");
    Logger::add_stderr_sink();
    std::string str("RealtimeType:\n"
                    "  filename: realtimetype.v\n"
                    "  line: 12\n"
                    "  packed_dims:\n"
                    "  signing: SIGNED\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["RealtimeType"]);
    ASSERT_TRUE(yaml["RealtimeType"]["filename"].as<std::string>() == "realtimetype.v");
    ASSERT_TRUE(yaml["RealtimeType"]["line"].as<int>() == 12);
    ASSERT_TRUE(yaml["RealtimeType"]["packed_dims"]);
    ASSERT_TRUE(yaml["RealtimeType"]["signing"].as<AST::DataType::SigningEnum>() ==
                AST::DataType::SigningEnum::SIGNED);
}

TEST(YAMLImporter, StringType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.StringType.log");
    Logger::add_stderr_sink();
    std::string str("StringType:\n"
                    "  filename: stringtype.v\n"
                    "  line: 10\n"
                    "  packed_dims:\n"
                    "  signing: SIGNED\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["StringType"]);
    ASSERT_TRUE(yaml["StringType"]["filename"].as<std::string>() == "stringtype.v");
    ASSERT_TRUE(yaml["StringType"]["line"].as<int>() == 10);
    ASSERT_TRUE(yaml["StringType"]["packed_dims"]);
    ASSERT_TRUE(yaml["StringType"]["signing"].as<AST::DataType::SigningEnum>() ==
                AST::DataType::SigningEnum::SIGNED);
}

TEST(YAMLImporter, ChandleType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.ChandleType.log");
    Logger::add_stderr_sink();
    std::string str("ChandleType:\n"
                    "  filename: chandletype.v\n"
                    "  line: 11\n"
                    "  packed_dims:\n"
                    "  signing: SIGNED\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["ChandleType"]);
    ASSERT_TRUE(yaml["ChandleType"]["filename"].as<std::string>() == "chandletype.v");
    ASSERT_TRUE(yaml["ChandleType"]["line"].as<int>() == 11);
    ASSERT_TRUE(yaml["ChandleType"]["packed_dims"]);
    ASSERT_TRUE(yaml["ChandleType"]["signing"].as<AST::DataType::SigningEnum>() ==
                AST::DataType::SigningEnum::SIGNED);
}

TEST(YAMLImporter, EventType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.EventType.log");
    Logger::add_stderr_sink();
    std::string str("EventType:\n"
                    "  filename: eventtype.v\n"
                    "  line: 9\n"
                    "  packed_dims:\n"
                    "  signing: SIGNED\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["EventType"]);
    ASSERT_TRUE(yaml["EventType"]["filename"].as<std::string>() == "eventtype.v");
    ASSERT_TRUE(yaml["EventType"]["line"].as<int>() == 9);
    ASSERT_TRUE(yaml["EventType"]["packed_dims"]);
    ASSERT_TRUE(yaml["EventType"]["signing"].as<AST::DataType::SigningEnum>() ==
                AST::DataType::SigningEnum::SIGNED);
}

TEST(YAMLImporter, ImplicitType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.ImplicitType.log");
    Logger::add_stderr_sink();
    std::string str("ImplicitType:\n"
                    "  filename: implicittype.v\n"
                    "  line: 12\n"
                    "  packed_dims:\n"
                    "  signing: SIGNED\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["ImplicitType"]);
    ASSERT_TRUE(yaml["ImplicitType"]["filename"].as<std::string>() == "implicittype.v");
    ASSERT_TRUE(yaml["ImplicitType"]["line"].as<int>() == 12);
    ASSERT_TRUE(yaml["ImplicitType"]["packed_dims"]);
    ASSERT_TRUE(yaml["ImplicitType"]["signing"].as<AST::DataType::SigningEnum>() ==
                AST::DataType::SigningEnum::SIGNED);
}

TEST(YAMLImporter, VoidType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.VoidType.log");
    Logger::add_stderr_sink();
    std::string str("VoidType:\n"
                    "  filename: voidtype.v\n"
                    "  line: 8\n"
                    "  packed_dims:\n"
                    "  signing: SIGNED\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["VoidType"]);
    ASSERT_TRUE(yaml["VoidType"]["filename"].as<std::string>() == "voidtype.v");
    ASSERT_TRUE(yaml["VoidType"]["line"].as<int>() == 8);
    ASSERT_TRUE(yaml["VoidType"]["packed_dims"]);
    ASSERT_TRUE(yaml["VoidType"]["signing"].as<AST::DataType::SigningEnum>() ==
                AST::DataType::SigningEnum::SIGNED);
}

TEST(YAMLImporter, NamedType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.NamedType.log");
    Logger::add_stderr_sink();
    std::string str("NamedType:\n"
                    "  filename: namedtype.v\n"
                    "  line: 9\n"
                    "  scope:\n"
                    "  packed_dims:\n"
                    "  name: mynbiqpmzj\n"
                    "  signing: SIGNED\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["NamedType"]);
    ASSERT_TRUE(yaml["NamedType"]["filename"].as<std::string>() == "namedtype.v");
    ASSERT_TRUE(yaml["NamedType"]["line"].as<int>() == 9);
    ASSERT_TRUE(yaml["NamedType"]["scope"]);
    ASSERT_TRUE(yaml["NamedType"]["packed_dims"]);
    ASSERT_TRUE(yaml["NamedType"]["name"].as<std::string>() == "mynbiqpmzj");
    ASSERT_TRUE(yaml["NamedType"]["signing"].as<AST::DataType::SigningEnum>() ==
                AST::DataType::SigningEnum::SIGNED);
}

TEST(YAMLImporter, ScopeName)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.ScopeName.log");
    Logger::add_stderr_sink();
    std::string str("ScopeName:\n"
                    "  filename: scopename.v\n"
                    "  line: 9\n"
                    "  params:\n"
                    "  name: mynbiqpmzj\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["ScopeName"]);
    ASSERT_TRUE(yaml["ScopeName"]["filename"].as<std::string>() == "scopename.v");
    ASSERT_TRUE(yaml["ScopeName"]["line"].as<int>() == 9);
    ASSERT_TRUE(yaml["ScopeName"]["params"]);
    ASSERT_TRUE(yaml["ScopeName"]["name"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLImporter, StructType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.StructType.log");
    Logger::add_stderr_sink();
    std::string str("StructType:\n"
                    "  filename: structtype.v\n"
                    "  line: 10\n"
                    "  members:\n"
                    "  packed_dims:\n"
                    "  is_packed: false\n"
                    "  signing: SIGNED\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["StructType"]);
    ASSERT_TRUE(yaml["StructType"]["filename"].as<std::string>() == "structtype.v");
    ASSERT_TRUE(yaml["StructType"]["line"].as<int>() == 10);
    ASSERT_TRUE(yaml["StructType"]["members"]);
    ASSERT_TRUE(yaml["StructType"]["packed_dims"]);
    ASSERT_TRUE(yaml["StructType"]["is_packed"].as<bool>() == false);
    ASSERT_TRUE(yaml["StructType"]["signing"].as<AST::DataType::SigningEnum>() ==
                AST::DataType::SigningEnum::SIGNED);
}

TEST(YAMLImporter, UnionType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.UnionType.log");
    Logger::add_stderr_sink();
    std::string str("UnionType:\n"
                    "  filename: uniontype.v\n"
                    "  line: 9\n"
                    "  members:\n"
                    "  packed_dims:\n"
                    "  is_packed: false\n"
                    "  is_tagged: false\n"
                    "  signing: NONE\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["UnionType"]);
    ASSERT_TRUE(yaml["UnionType"]["filename"].as<std::string>() == "uniontype.v");
    ASSERT_TRUE(yaml["UnionType"]["line"].as<int>() == 9);
    ASSERT_TRUE(yaml["UnionType"]["members"]);
    ASSERT_TRUE(yaml["UnionType"]["packed_dims"]);
    ASSERT_TRUE(yaml["UnionType"]["is_packed"].as<bool>() == false);
    ASSERT_TRUE(yaml["UnionType"]["is_tagged"].as<bool>() == false);
    ASSERT_TRUE(yaml["UnionType"]["signing"].as<AST::DataType::SigningEnum>() ==
                AST::DataType::SigningEnum::NONE);
}

TEST(YAMLImporter, EnumType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.EnumType.log");
    Logger::add_stderr_sink();
    std::string str("EnumType:\n"
                    "  filename: enumtype.v\n"
                    "  line: 8\n"
                    "  base:\n"
                    "  items:\n"
                    "  packed_dims:\n"
                    "  signing: SIGNED\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["EnumType"]);
    ASSERT_TRUE(yaml["EnumType"]["filename"].as<std::string>() == "enumtype.v");
    ASSERT_TRUE(yaml["EnumType"]["line"].as<int>() == 8);
    ASSERT_TRUE(yaml["EnumType"]["base"]);
    ASSERT_TRUE(yaml["EnumType"]["items"]);
    ASSERT_TRUE(yaml["EnumType"]["packed_dims"]);
    ASSERT_TRUE(yaml["EnumType"]["signing"].as<AST::DataType::SigningEnum>() ==
                AST::DataType::SigningEnum::SIGNED);
}

TEST(YAMLImporter, EnumItem)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.EnumItem.log");
    Logger::add_stderr_sink();
    std::string str("EnumItem:\n"
                    "  filename: enumitem.v\n"
                    "  line: 8\n"
                    "  value:\n"
                    "  range:\n"
                    "  name: mynbiqpmzj\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["EnumItem"]);
    ASSERT_TRUE(yaml["EnumItem"]["filename"].as<std::string>() == "enumitem.v");
    ASSERT_TRUE(yaml["EnumItem"]["line"].as<int>() == 8);
    ASSERT_TRUE(yaml["EnumItem"]["value"]);
    ASSERT_TRUE(yaml["EnumItem"]["range"]);
    ASSERT_TRUE(yaml["EnumItem"]["name"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLImporter, TypeOpExpr)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.TypeOpExpr.log");
    Logger::add_stderr_sink();
    std::string str("TypeOpExpr:\n"
                    "  filename: typeopexpr.v\n"
                    "  line: 10\n"
                    "  expr:\n"
                    "  packed_dims:\n"
                    "  signing: SIGNED\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["TypeOpExpr"]);
    ASSERT_TRUE(yaml["TypeOpExpr"]["filename"].as<std::string>() == "typeopexpr.v");
    ASSERT_TRUE(yaml["TypeOpExpr"]["line"].as<int>() == 10);
    ASSERT_TRUE(yaml["TypeOpExpr"]["expr"]);
    ASSERT_TRUE(yaml["TypeOpExpr"]["packed_dims"]);
    ASSERT_TRUE(yaml["TypeOpExpr"]["signing"].as<AST::DataType::SigningEnum>() ==
                AST::DataType::SigningEnum::SIGNED);
}

TEST(YAMLImporter, TypeOpType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.TypeOpType.log");
    Logger::add_stderr_sink();
    std::string str("TypeOpType:\n"
                    "  filename: typeoptype.v\n"
                    "  line: 10\n"
                    "  arg_type:\n"
                    "  packed_dims:\n"
                    "  signing: SIGNED\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["TypeOpType"]);
    ASSERT_TRUE(yaml["TypeOpType"]["filename"].as<std::string>() == "typeoptype.v");
    ASSERT_TRUE(yaml["TypeOpType"]["line"].as<int>() == 10);
    ASSERT_TRUE(yaml["TypeOpType"]["arg_type"]);
    ASSERT_TRUE(yaml["TypeOpType"]["packed_dims"]);
    ASSERT_TRUE(yaml["TypeOpType"]["signing"].as<AST::DataType::SigningEnum>() ==
                AST::DataType::SigningEnum::SIGNED);
}

TEST(YAMLImporter, Dimension)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Dimension.log");
    Logger::add_stderr_sink();
    std::string str("Dimension:\n"
                    "  filename: dimension.v\n"
                    "  line: 9\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Dimension"]);
    ASSERT_TRUE(yaml["Dimension"]["filename"].as<std::string>() == "dimension.v");
    ASSERT_TRUE(yaml["Dimension"]["line"].as<int>() == 9);
}

TEST(YAMLImporter, RangeDim)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.RangeDim.log");
    Logger::add_stderr_sink();
    std::string str("RangeDim:\n"
                    "  filename: rangedim.v\n"
                    "  line: 8\n"
                    "  left:\n"
                    "  right:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["RangeDim"]);
    ASSERT_TRUE(yaml["RangeDim"]["filename"].as<std::string>() == "rangedim.v");
    ASSERT_TRUE(yaml["RangeDim"]["line"].as<int>() == 8);
    ASSERT_TRUE(yaml["RangeDim"]["left"]);
    ASSERT_TRUE(yaml["RangeDim"]["right"]);
}

TEST(YAMLImporter, SizeDim)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.SizeDim.log");
    Logger::add_stderr_sink();
    std::string str("SizeDim:\n"
                    "  filename: sizedim.v\n"
                    "  line: 7\n"
                    "  size:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["SizeDim"]);
    ASSERT_TRUE(yaml["SizeDim"]["filename"].as<std::string>() == "sizedim.v");
    ASSERT_TRUE(yaml["SizeDim"]["line"].as<int>() == 7);
    ASSERT_TRUE(yaml["SizeDim"]["size"]);
}

TEST(YAMLImporter, UnsizedDim)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.UnsizedDim.log");
    Logger::add_stderr_sink();
    std::string str("UnsizedDim:\n"
                    "  filename: unsizeddim.v\n"
                    "  line: 10\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["UnsizedDim"]);
    ASSERT_TRUE(yaml["UnsizedDim"]["filename"].as<std::string>() == "unsizeddim.v");
    ASSERT_TRUE(yaml["UnsizedDim"]["line"].as<int>() == 10);
}

TEST(YAMLImporter, QueueDim)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.QueueDim.log");
    Logger::add_stderr_sink();
    std::string str("QueueDim:\n"
                    "  filename: queuedim.v\n"
                    "  line: 8\n"
                    "  bound:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["QueueDim"]);
    ASSERT_TRUE(yaml["QueueDim"]["filename"].as<std::string>() == "queuedim.v");
    ASSERT_TRUE(yaml["QueueDim"]["line"].as<int>() == 8);
    ASSERT_TRUE(yaml["QueueDim"]["bound"]);
}

TEST(YAMLImporter, AssocDim)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.AssocDim.log");
    Logger::add_stderr_sink();
    std::string str("AssocDim:\n"
                    "  filename: assocdim.v\n"
                    "  line: 8\n"
                    "  index_type:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["AssocDim"]);
    ASSERT_TRUE(yaml["AssocDim"]["filename"].as<std::string>() == "assocdim.v");
    ASSERT_TRUE(yaml["AssocDim"]["line"].as<int>() == 8);
    ASSERT_TRUE(yaml["AssocDim"]["index_type"]);
}

TEST(YAMLImporter, Declaration)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Declaration.log");
    Logger::add_stderr_sink();
    std::string str("Declaration:\n"
                    "  filename: declaration.v\n"
                    "  line: 11\n"
                    "  type:\n"
                    "  name: mynbiqpmzj\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Declaration"]);
    ASSERT_TRUE(yaml["Declaration"]["filename"].as<std::string>() == "declaration.v");
    ASSERT_TRUE(yaml["Declaration"]["line"].as<int>() == 11);
    ASSERT_TRUE(yaml["Declaration"]["type"]);
    ASSERT_TRUE(yaml["Declaration"]["name"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLImporter, Var)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Var.log");
    Logger::add_stderr_sink();
    std::string str("Var:\n"
                    "  filename: var.v\n"
                    "  line: 3\n"
                    "  unpacked_dims:\n"
                    "  init:\n"
                    "  type:\n"
                    "  is_var: false\n"
                    "  is_const: false\n"
                    "  lifetime: NONE\n"
                    "  name: iqpmzjplsg\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Var"]);
    ASSERT_TRUE(yaml["Var"]["filename"].as<std::string>() == "var.v");
    ASSERT_TRUE(yaml["Var"]["line"].as<int>() == 3);
    ASSERT_TRUE(yaml["Var"]["unpacked_dims"]);
    ASSERT_TRUE(yaml["Var"]["init"]);
    ASSERT_TRUE(yaml["Var"]["type"]);
    ASSERT_TRUE(yaml["Var"]["is_var"].as<bool>() == false);
    ASSERT_TRUE(yaml["Var"]["is_const"].as<bool>() == false);
    ASSERT_TRUE(yaml["Var"]["lifetime"].as<AST::Var::LifetimeEnum>() ==
                AST::Var::LifetimeEnum::NONE);
    ASSERT_TRUE(yaml["Var"]["name"].as<std::string>() == "iqpmzjplsg");
}

TEST(YAMLImporter, Net)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Net.log");
    Logger::add_stderr_sink();
    std::string str("Net:\n"
                    "  filename: net.v\n"
                    "  line: 3\n"
                    "  unpacked_dims:\n"
                    "  cont_assign:\n"
                    "  strength:\n"
                    "  ldelay:\n"
                    "  rdelay:\n"
                    "  type:\n"
                    "  is_vectored: false\n"
                    "  is_scalared: false\n"
                    "  name: biqpmzjpls\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Net"]);
    ASSERT_TRUE(yaml["Net"]["filename"].as<std::string>() == "net.v");
    ASSERT_TRUE(yaml["Net"]["line"].as<int>() == 3);
    ASSERT_TRUE(yaml["Net"]["unpacked_dims"]);
    ASSERT_TRUE(yaml["Net"]["cont_assign"]);
    ASSERT_TRUE(yaml["Net"]["strength"]);
    ASSERT_TRUE(yaml["Net"]["ldelay"]);
    ASSERT_TRUE(yaml["Net"]["rdelay"]);
    ASSERT_TRUE(yaml["Net"]["type"]);
    ASSERT_TRUE(yaml["Net"]["is_vectored"].as<bool>() == false);
    ASSERT_TRUE(yaml["Net"]["is_scalared"].as<bool>() == false);
    ASSERT_TRUE(yaml["Net"]["name"].as<std::string>() == "biqpmzjpls");
}

TEST(YAMLImporter, WireNet)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.WireNet.log");
    Logger::add_stderr_sink();
    std::string str("WireNet:\n"
                    "  filename: wirenet.v\n"
                    "  line: 7\n"
                    "  unpacked_dims:\n"
                    "  cont_assign:\n"
                    "  strength:\n"
                    "  ldelay:\n"
                    "  rdelay:\n"
                    "  type:\n"
                    "  is_vectored: false\n"
                    "  is_scalared: false\n"
                    "  name: biqpmzjpls\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["WireNet"]);
    ASSERT_TRUE(yaml["WireNet"]["filename"].as<std::string>() == "wirenet.v");
    ASSERT_TRUE(yaml["WireNet"]["line"].as<int>() == 7);
    ASSERT_TRUE(yaml["WireNet"]["unpacked_dims"]);
    ASSERT_TRUE(yaml["WireNet"]["cont_assign"]);
    ASSERT_TRUE(yaml["WireNet"]["strength"]);
    ASSERT_TRUE(yaml["WireNet"]["ldelay"]);
    ASSERT_TRUE(yaml["WireNet"]["rdelay"]);
    ASSERT_TRUE(yaml["WireNet"]["type"]);
    ASSERT_TRUE(yaml["WireNet"]["is_vectored"].as<bool>() == false);
    ASSERT_TRUE(yaml["WireNet"]["is_scalared"].as<bool>() == false);
    ASSERT_TRUE(yaml["WireNet"]["name"].as<std::string>() == "biqpmzjpls");
}

TEST(YAMLImporter, TriNet)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.TriNet.log");
    Logger::add_stderr_sink();
    std::string str("TriNet:\n"
                    "  filename: trinet.v\n"
                    "  line: 6\n"
                    "  unpacked_dims:\n"
                    "  cont_assign:\n"
                    "  strength:\n"
                    "  ldelay:\n"
                    "  rdelay:\n"
                    "  type:\n"
                    "  is_vectored: false\n"
                    "  is_scalared: false\n"
                    "  name: biqpmzjpls\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["TriNet"]);
    ASSERT_TRUE(yaml["TriNet"]["filename"].as<std::string>() == "trinet.v");
    ASSERT_TRUE(yaml["TriNet"]["line"].as<int>() == 6);
    ASSERT_TRUE(yaml["TriNet"]["unpacked_dims"]);
    ASSERT_TRUE(yaml["TriNet"]["cont_assign"]);
    ASSERT_TRUE(yaml["TriNet"]["strength"]);
    ASSERT_TRUE(yaml["TriNet"]["ldelay"]);
    ASSERT_TRUE(yaml["TriNet"]["rdelay"]);
    ASSERT_TRUE(yaml["TriNet"]["type"]);
    ASSERT_TRUE(yaml["TriNet"]["is_vectored"].as<bool>() == false);
    ASSERT_TRUE(yaml["TriNet"]["is_scalared"].as<bool>() == false);
    ASSERT_TRUE(yaml["TriNet"]["name"].as<std::string>() == "biqpmzjpls");
}

TEST(YAMLImporter, Tri0Net)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Tri0Net.log");
    Logger::add_stderr_sink();
    std::string str("Tri0Net:\n"
                    "  filename: tri0net.v\n"
                    "  line: 7\n"
                    "  unpacked_dims:\n"
                    "  cont_assign:\n"
                    "  strength:\n"
                    "  ldelay:\n"
                    "  rdelay:\n"
                    "  type:\n"
                    "  is_vectored: false\n"
                    "  is_scalared: false\n"
                    "  name: biqpmzjpls\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Tri0Net"]);
    ASSERT_TRUE(yaml["Tri0Net"]["filename"].as<std::string>() == "tri0net.v");
    ASSERT_TRUE(yaml["Tri0Net"]["line"].as<int>() == 7);
    ASSERT_TRUE(yaml["Tri0Net"]["unpacked_dims"]);
    ASSERT_TRUE(yaml["Tri0Net"]["cont_assign"]);
    ASSERT_TRUE(yaml["Tri0Net"]["strength"]);
    ASSERT_TRUE(yaml["Tri0Net"]["ldelay"]);
    ASSERT_TRUE(yaml["Tri0Net"]["rdelay"]);
    ASSERT_TRUE(yaml["Tri0Net"]["type"]);
    ASSERT_TRUE(yaml["Tri0Net"]["is_vectored"].as<bool>() == false);
    ASSERT_TRUE(yaml["Tri0Net"]["is_scalared"].as<bool>() == false);
    ASSERT_TRUE(yaml["Tri0Net"]["name"].as<std::string>() == "biqpmzjpls");
}

TEST(YAMLImporter, Tri1Net)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Tri1Net.log");
    Logger::add_stderr_sink();
    std::string str("Tri1Net:\n"
                    "  filename: tri1net.v\n"
                    "  line: 7\n"
                    "  unpacked_dims:\n"
                    "  cont_assign:\n"
                    "  strength:\n"
                    "  ldelay:\n"
                    "  rdelay:\n"
                    "  type:\n"
                    "  is_vectored: false\n"
                    "  is_scalared: false\n"
                    "  name: biqpmzjpls\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Tri1Net"]);
    ASSERT_TRUE(yaml["Tri1Net"]["filename"].as<std::string>() == "tri1net.v");
    ASSERT_TRUE(yaml["Tri1Net"]["line"].as<int>() == 7);
    ASSERT_TRUE(yaml["Tri1Net"]["unpacked_dims"]);
    ASSERT_TRUE(yaml["Tri1Net"]["cont_assign"]);
    ASSERT_TRUE(yaml["Tri1Net"]["strength"]);
    ASSERT_TRUE(yaml["Tri1Net"]["ldelay"]);
    ASSERT_TRUE(yaml["Tri1Net"]["rdelay"]);
    ASSERT_TRUE(yaml["Tri1Net"]["type"]);
    ASSERT_TRUE(yaml["Tri1Net"]["is_vectored"].as<bool>() == false);
    ASSERT_TRUE(yaml["Tri1Net"]["is_scalared"].as<bool>() == false);
    ASSERT_TRUE(yaml["Tri1Net"]["name"].as<std::string>() == "biqpmzjpls");
}

TEST(YAMLImporter, TriandNet)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.TriandNet.log");
    Logger::add_stderr_sink();
    std::string str("TriandNet:\n"
                    "  filename: triandnet.v\n"
                    "  line: 9\n"
                    "  unpacked_dims:\n"
                    "  cont_assign:\n"
                    "  strength:\n"
                    "  ldelay:\n"
                    "  rdelay:\n"
                    "  type:\n"
                    "  is_vectored: false\n"
                    "  is_scalared: false\n"
                    "  name: biqpmzjpls\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["TriandNet"]);
    ASSERT_TRUE(yaml["TriandNet"]["filename"].as<std::string>() == "triandnet.v");
    ASSERT_TRUE(yaml["TriandNet"]["line"].as<int>() == 9);
    ASSERT_TRUE(yaml["TriandNet"]["unpacked_dims"]);
    ASSERT_TRUE(yaml["TriandNet"]["cont_assign"]);
    ASSERT_TRUE(yaml["TriandNet"]["strength"]);
    ASSERT_TRUE(yaml["TriandNet"]["ldelay"]);
    ASSERT_TRUE(yaml["TriandNet"]["rdelay"]);
    ASSERT_TRUE(yaml["TriandNet"]["type"]);
    ASSERT_TRUE(yaml["TriandNet"]["is_vectored"].as<bool>() == false);
    ASSERT_TRUE(yaml["TriandNet"]["is_scalared"].as<bool>() == false);
    ASSERT_TRUE(yaml["TriandNet"]["name"].as<std::string>() == "biqpmzjpls");
}

TEST(YAMLImporter, TriorNet)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.TriorNet.log");
    Logger::add_stderr_sink();
    std::string str("TriorNet:\n"
                    "  filename: triornet.v\n"
                    "  line: 8\n"
                    "  unpacked_dims:\n"
                    "  cont_assign:\n"
                    "  strength:\n"
                    "  ldelay:\n"
                    "  rdelay:\n"
                    "  type:\n"
                    "  is_vectored: false\n"
                    "  is_scalared: false\n"
                    "  name: biqpmzjpls\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["TriorNet"]);
    ASSERT_TRUE(yaml["TriorNet"]["filename"].as<std::string>() == "triornet.v");
    ASSERT_TRUE(yaml["TriorNet"]["line"].as<int>() == 8);
    ASSERT_TRUE(yaml["TriorNet"]["unpacked_dims"]);
    ASSERT_TRUE(yaml["TriorNet"]["cont_assign"]);
    ASSERT_TRUE(yaml["TriorNet"]["strength"]);
    ASSERT_TRUE(yaml["TriorNet"]["ldelay"]);
    ASSERT_TRUE(yaml["TriorNet"]["rdelay"]);
    ASSERT_TRUE(yaml["TriorNet"]["type"]);
    ASSERT_TRUE(yaml["TriorNet"]["is_vectored"].as<bool>() == false);
    ASSERT_TRUE(yaml["TriorNet"]["is_scalared"].as<bool>() == false);
    ASSERT_TRUE(yaml["TriorNet"]["name"].as<std::string>() == "biqpmzjpls");
}

TEST(YAMLImporter, TriregNet)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.TriregNet.log");
    Logger::add_stderr_sink();
    std::string str("TriregNet:\n"
                    "  filename: triregnet.v\n"
                    "  line: 9\n"
                    "  unpacked_dims:\n"
                    "  cont_assign:\n"
                    "  strength:\n"
                    "  ldelay:\n"
                    "  rdelay:\n"
                    "  type:\n"
                    "  is_vectored: false\n"
                    "  is_scalared: false\n"
                    "  name: biqpmzjpls\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["TriregNet"]);
    ASSERT_TRUE(yaml["TriregNet"]["filename"].as<std::string>() == "triregnet.v");
    ASSERT_TRUE(yaml["TriregNet"]["line"].as<int>() == 9);
    ASSERT_TRUE(yaml["TriregNet"]["unpacked_dims"]);
    ASSERT_TRUE(yaml["TriregNet"]["cont_assign"]);
    ASSERT_TRUE(yaml["TriregNet"]["strength"]);
    ASSERT_TRUE(yaml["TriregNet"]["ldelay"]);
    ASSERT_TRUE(yaml["TriregNet"]["rdelay"]);
    ASSERT_TRUE(yaml["TriregNet"]["type"]);
    ASSERT_TRUE(yaml["TriregNet"]["is_vectored"].as<bool>() == false);
    ASSERT_TRUE(yaml["TriregNet"]["is_scalared"].as<bool>() == false);
    ASSERT_TRUE(yaml["TriregNet"]["name"].as<std::string>() == "biqpmzjpls");
}

TEST(YAMLImporter, WandNet)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.WandNet.log");
    Logger::add_stderr_sink();
    std::string str("WandNet:\n"
                    "  filename: wandnet.v\n"
                    "  line: 7\n"
                    "  unpacked_dims:\n"
                    "  cont_assign:\n"
                    "  strength:\n"
                    "  ldelay:\n"
                    "  rdelay:\n"
                    "  type:\n"
                    "  is_vectored: false\n"
                    "  is_scalared: false\n"
                    "  name: biqpmzjpls\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["WandNet"]);
    ASSERT_TRUE(yaml["WandNet"]["filename"].as<std::string>() == "wandnet.v");
    ASSERT_TRUE(yaml["WandNet"]["line"].as<int>() == 7);
    ASSERT_TRUE(yaml["WandNet"]["unpacked_dims"]);
    ASSERT_TRUE(yaml["WandNet"]["cont_assign"]);
    ASSERT_TRUE(yaml["WandNet"]["strength"]);
    ASSERT_TRUE(yaml["WandNet"]["ldelay"]);
    ASSERT_TRUE(yaml["WandNet"]["rdelay"]);
    ASSERT_TRUE(yaml["WandNet"]["type"]);
    ASSERT_TRUE(yaml["WandNet"]["is_vectored"].as<bool>() == false);
    ASSERT_TRUE(yaml["WandNet"]["is_scalared"].as<bool>() == false);
    ASSERT_TRUE(yaml["WandNet"]["name"].as<std::string>() == "biqpmzjpls");
}

TEST(YAMLImporter, WorNet)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.WorNet.log");
    Logger::add_stderr_sink();
    std::string str("WorNet:\n"
                    "  filename: wornet.v\n"
                    "  line: 6\n"
                    "  unpacked_dims:\n"
                    "  cont_assign:\n"
                    "  strength:\n"
                    "  ldelay:\n"
                    "  rdelay:\n"
                    "  type:\n"
                    "  is_vectored: false\n"
                    "  is_scalared: false\n"
                    "  name: biqpmzjpls\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["WorNet"]);
    ASSERT_TRUE(yaml["WorNet"]["filename"].as<std::string>() == "wornet.v");
    ASSERT_TRUE(yaml["WorNet"]["line"].as<int>() == 6);
    ASSERT_TRUE(yaml["WorNet"]["unpacked_dims"]);
    ASSERT_TRUE(yaml["WorNet"]["cont_assign"]);
    ASSERT_TRUE(yaml["WorNet"]["strength"]);
    ASSERT_TRUE(yaml["WorNet"]["ldelay"]);
    ASSERT_TRUE(yaml["WorNet"]["rdelay"]);
    ASSERT_TRUE(yaml["WorNet"]["type"]);
    ASSERT_TRUE(yaml["WorNet"]["is_vectored"].as<bool>() == false);
    ASSERT_TRUE(yaml["WorNet"]["is_scalared"].as<bool>() == false);
    ASSERT_TRUE(yaml["WorNet"]["name"].as<std::string>() == "biqpmzjpls");
}

TEST(YAMLImporter, UwireNet)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.UwireNet.log");
    Logger::add_stderr_sink();
    std::string str("UwireNet:\n"
                    "  filename: uwirenet.v\n"
                    "  line: 8\n"
                    "  unpacked_dims:\n"
                    "  cont_assign:\n"
                    "  strength:\n"
                    "  ldelay:\n"
                    "  rdelay:\n"
                    "  type:\n"
                    "  is_vectored: false\n"
                    "  is_scalared: false\n"
                    "  name: biqpmzjpls\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["UwireNet"]);
    ASSERT_TRUE(yaml["UwireNet"]["filename"].as<std::string>() == "uwirenet.v");
    ASSERT_TRUE(yaml["UwireNet"]["line"].as<int>() == 8);
    ASSERT_TRUE(yaml["UwireNet"]["unpacked_dims"]);
    ASSERT_TRUE(yaml["UwireNet"]["cont_assign"]);
    ASSERT_TRUE(yaml["UwireNet"]["strength"]);
    ASSERT_TRUE(yaml["UwireNet"]["ldelay"]);
    ASSERT_TRUE(yaml["UwireNet"]["rdelay"]);
    ASSERT_TRUE(yaml["UwireNet"]["type"]);
    ASSERT_TRUE(yaml["UwireNet"]["is_vectored"].as<bool>() == false);
    ASSERT_TRUE(yaml["UwireNet"]["is_scalared"].as<bool>() == false);
    ASSERT_TRUE(yaml["UwireNet"]["name"].as<std::string>() == "biqpmzjpls");
}

TEST(YAMLImporter, Supply0Net)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Supply0Net.log");
    Logger::add_stderr_sink();
    std::string str("Supply0Net:\n"
                    "  filename: supply0net.v\n"
                    "  line: 10\n"
                    "  unpacked_dims:\n"
                    "  cont_assign:\n"
                    "  strength:\n"
                    "  ldelay:\n"
                    "  rdelay:\n"
                    "  type:\n"
                    "  is_vectored: false\n"
                    "  is_scalared: false\n"
                    "  name: biqpmzjpls\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Supply0Net"]);
    ASSERT_TRUE(yaml["Supply0Net"]["filename"].as<std::string>() == "supply0net.v");
    ASSERT_TRUE(yaml["Supply0Net"]["line"].as<int>() == 10);
    ASSERT_TRUE(yaml["Supply0Net"]["unpacked_dims"]);
    ASSERT_TRUE(yaml["Supply0Net"]["cont_assign"]);
    ASSERT_TRUE(yaml["Supply0Net"]["strength"]);
    ASSERT_TRUE(yaml["Supply0Net"]["ldelay"]);
    ASSERT_TRUE(yaml["Supply0Net"]["rdelay"]);
    ASSERT_TRUE(yaml["Supply0Net"]["type"]);
    ASSERT_TRUE(yaml["Supply0Net"]["is_vectored"].as<bool>() == false);
    ASSERT_TRUE(yaml["Supply0Net"]["is_scalared"].as<bool>() == false);
    ASSERT_TRUE(yaml["Supply0Net"]["name"].as<std::string>() == "biqpmzjpls");
}

TEST(YAMLImporter, Supply1Net)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Supply1Net.log");
    Logger::add_stderr_sink();
    std::string str("Supply1Net:\n"
                    "  filename: supply1net.v\n"
                    "  line: 10\n"
                    "  unpacked_dims:\n"
                    "  cont_assign:\n"
                    "  strength:\n"
                    "  ldelay:\n"
                    "  rdelay:\n"
                    "  type:\n"
                    "  is_vectored: false\n"
                    "  is_scalared: false\n"
                    "  name: biqpmzjpls\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Supply1Net"]);
    ASSERT_TRUE(yaml["Supply1Net"]["filename"].as<std::string>() == "supply1net.v");
    ASSERT_TRUE(yaml["Supply1Net"]["line"].as<int>() == 10);
    ASSERT_TRUE(yaml["Supply1Net"]["unpacked_dims"]);
    ASSERT_TRUE(yaml["Supply1Net"]["cont_assign"]);
    ASSERT_TRUE(yaml["Supply1Net"]["strength"]);
    ASSERT_TRUE(yaml["Supply1Net"]["ldelay"]);
    ASSERT_TRUE(yaml["Supply1Net"]["rdelay"]);
    ASSERT_TRUE(yaml["Supply1Net"]["type"]);
    ASSERT_TRUE(yaml["Supply1Net"]["is_vectored"].as<bool>() == false);
    ASSERT_TRUE(yaml["Supply1Net"]["is_scalared"].as<bool>() == false);
    ASSERT_TRUE(yaml["Supply1Net"]["name"].as<std::string>() == "biqpmzjpls");
}

TEST(YAMLImporter, InterconnectNet)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.InterconnectNet.log");
    Logger::add_stderr_sink();
    std::string str("InterconnectNet:\n"
                    "  filename: interconnectnet.v\n"
                    "  line: 15\n"
                    "  unpacked_dims:\n"
                    "  cont_assign:\n"
                    "  strength:\n"
                    "  ldelay:\n"
                    "  rdelay:\n"
                    "  type:\n"
                    "  is_vectored: false\n"
                    "  is_scalared: false\n"
                    "  name: biqpmzjpls\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["InterconnectNet"]);
    ASSERT_TRUE(yaml["InterconnectNet"]["filename"].as<std::string>() == "interconnectnet.v");
    ASSERT_TRUE(yaml["InterconnectNet"]["line"].as<int>() == 15);
    ASSERT_TRUE(yaml["InterconnectNet"]["unpacked_dims"]);
    ASSERT_TRUE(yaml["InterconnectNet"]["cont_assign"]);
    ASSERT_TRUE(yaml["InterconnectNet"]["strength"]);
    ASSERT_TRUE(yaml["InterconnectNet"]["ldelay"]);
    ASSERT_TRUE(yaml["InterconnectNet"]["rdelay"]);
    ASSERT_TRUE(yaml["InterconnectNet"]["type"]);
    ASSERT_TRUE(yaml["InterconnectNet"]["is_vectored"].as<bool>() == false);
    ASSERT_TRUE(yaml["InterconnectNet"]["is_scalared"].as<bool>() == false);
    ASSERT_TRUE(yaml["InterconnectNet"]["name"].as<std::string>() == "biqpmzjpls");
}

TEST(YAMLImporter, UserNet)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.UserNet.log");
    Logger::add_stderr_sink();
    std::string str("UserNet:\n"
                    "  filename: usernet.v\n"
                    "  line: 7\n"
                    "  unpacked_dims:\n"
                    "  cont_assign:\n"
                    "  strength:\n"
                    "  ldelay:\n"
                    "  rdelay:\n"
                    "  type:\n"
                    "  is_vectored: false\n"
                    "  is_scalared: false\n"
                    "  name: biqpmzjpls\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["UserNet"]);
    ASSERT_TRUE(yaml["UserNet"]["filename"].as<std::string>() == "usernet.v");
    ASSERT_TRUE(yaml["UserNet"]["line"].as<int>() == 7);
    ASSERT_TRUE(yaml["UserNet"]["unpacked_dims"]);
    ASSERT_TRUE(yaml["UserNet"]["cont_assign"]);
    ASSERT_TRUE(yaml["UserNet"]["strength"]);
    ASSERT_TRUE(yaml["UserNet"]["ldelay"]);
    ASSERT_TRUE(yaml["UserNet"]["rdelay"]);
    ASSERT_TRUE(yaml["UserNet"]["type"]);
    ASSERT_TRUE(yaml["UserNet"]["is_vectored"].as<bool>() == false);
    ASSERT_TRUE(yaml["UserNet"]["is_scalared"].as<bool>() == false);
    ASSERT_TRUE(yaml["UserNet"]["name"].as<std::string>() == "biqpmzjpls");
}

TEST(YAMLImporter, ImplicitNet)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.ImplicitNet.log");
    Logger::add_stderr_sink();
    std::string str("ImplicitNet:\n"
                    "  filename: implicitnet.v\n"
                    "  line: 11\n"
                    "  unpacked_dims:\n"
                    "  cont_assign:\n"
                    "  strength:\n"
                    "  ldelay:\n"
                    "  rdelay:\n"
                    "  type:\n"
                    "  is_vectored: false\n"
                    "  is_scalared: false\n"
                    "  name: biqpmzjpls\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["ImplicitNet"]);
    ASSERT_TRUE(yaml["ImplicitNet"]["filename"].as<std::string>() == "implicitnet.v");
    ASSERT_TRUE(yaml["ImplicitNet"]["line"].as<int>() == 11);
    ASSERT_TRUE(yaml["ImplicitNet"]["unpacked_dims"]);
    ASSERT_TRUE(yaml["ImplicitNet"]["cont_assign"]);
    ASSERT_TRUE(yaml["ImplicitNet"]["strength"]);
    ASSERT_TRUE(yaml["ImplicitNet"]["ldelay"]);
    ASSERT_TRUE(yaml["ImplicitNet"]["rdelay"]);
    ASSERT_TRUE(yaml["ImplicitNet"]["type"]);
    ASSERT_TRUE(yaml["ImplicitNet"]["is_vectored"].as<bool>() == false);
    ASSERT_TRUE(yaml["ImplicitNet"]["is_scalared"].as<bool>() == false);
    ASSERT_TRUE(yaml["ImplicitNet"]["name"].as<std::string>() == "biqpmzjpls");
}

TEST(YAMLImporter, Strength)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Strength.log");
    Logger::add_stderr_sink();
    std::string str("Strength:\n"
                    "  filename: strength.v\n"
                    "  line: 8\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Strength"]);
    ASSERT_TRUE(yaml["Strength"]["filename"].as<std::string>() == "strength.v");
    ASSERT_TRUE(yaml["Strength"]["line"].as<int>() == 8);
}

TEST(YAMLImporter, DriveStrength)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.DriveStrength.log");
    Logger::add_stderr_sink();
    std::string str("DriveStrength:\n"
                    "  filename: drivestrength.v\n"
                    "  line: 13\n"
                    "  s0: WEAK0\n"
                    "  s1: WEAK1\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["DriveStrength"]);
    ASSERT_TRUE(yaml["DriveStrength"]["filename"].as<std::string>() == "drivestrength.v");
    ASSERT_TRUE(yaml["DriveStrength"]["line"].as<int>() == 13);
    ASSERT_TRUE(yaml["DriveStrength"]["s0"].as<AST::DriveStrength::S0Enum>() ==
                AST::DriveStrength::S0Enum::WEAK0);
    ASSERT_TRUE(yaml["DriveStrength"]["s1"].as<AST::DriveStrength::S1Enum>() ==
                AST::DriveStrength::S1Enum::WEAK1);
}

TEST(YAMLImporter, ChargeStrength)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.ChargeStrength.log");
    Logger::add_stderr_sink();
    std::string str("ChargeStrength:\n"
                    "  filename: chargestrength.v\n"
                    "  line: 14\n"
                    "  charge: MEDIUM\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["ChargeStrength"]);
    ASSERT_TRUE(yaml["ChargeStrength"]["filename"].as<std::string>() == "chargestrength.v");
    ASSERT_TRUE(yaml["ChargeStrength"]["line"].as<int>() == 14);
    ASSERT_TRUE(yaml["ChargeStrength"]["charge"].as<AST::ChargeStrength::ChargeEnum>() ==
                AST::ChargeStrength::ChargeEnum::MEDIUM);
}

TEST(YAMLImporter, Param)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Param.log");
    Logger::add_stderr_sink();
    std::string str("Param:\n"
                    "  filename: param.v\n"
                    "  line: 5\n"
                    "  value:\n"
                    "  unpacked_dims:\n"
                    "  type:\n"
                    "  is_local: false\n"
                    "  name: ynbiqpmzjp\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Param"]);
    ASSERT_TRUE(yaml["Param"]["filename"].as<std::string>() == "param.v");
    ASSERT_TRUE(yaml["Param"]["line"].as<int>() == 5);
    ASSERT_TRUE(yaml["Param"]["value"]);
    ASSERT_TRUE(yaml["Param"]["unpacked_dims"]);
    ASSERT_TRUE(yaml["Param"]["type"]);
    ASSERT_TRUE(yaml["Param"]["is_local"].as<bool>() == false);
    ASSERT_TRUE(yaml["Param"]["name"].as<std::string>() == "ynbiqpmzjp");
}

TEST(YAMLImporter, TypeParam)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.TypeParam.log");
    Logger::add_stderr_sink();
    std::string str("TypeParam:\n"
                    "  filename: typeparam.v\n"
                    "  line: 9\n"
                    "  type:\n"
                    "  is_local: false\n"
                    "  name: ynbiqpmzjp\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["TypeParam"]);
    ASSERT_TRUE(yaml["TypeParam"]["filename"].as<std::string>() == "typeparam.v");
    ASSERT_TRUE(yaml["TypeParam"]["line"].as<int>() == 9);
    ASSERT_TRUE(yaml["TypeParam"]["type"]);
    ASSERT_TRUE(yaml["TypeParam"]["is_local"].as<bool>() == false);
    ASSERT_TRUE(yaml["TypeParam"]["name"].as<std::string>() == "ynbiqpmzjp");
}

TEST(YAMLImporter, Typedef)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Typedef.log");
    Logger::add_stderr_sink();
    std::string str("Typedef:\n"
                    "  filename: typedef.v\n"
                    "  line: 7\n"
                    "  unpacked_dims:\n"
                    "  type:\n"
                    "  fwd_kind: UNION\n"
                    "  name: ynbiqpmzjp\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Typedef"]);
    ASSERT_TRUE(yaml["Typedef"]["filename"].as<std::string>() == "typedef.v");
    ASSERT_TRUE(yaml["Typedef"]["line"].as<int>() == 7);
    ASSERT_TRUE(yaml["Typedef"]["unpacked_dims"]);
    ASSERT_TRUE(yaml["Typedef"]["type"]);
    ASSERT_TRUE(yaml["Typedef"]["fwd_kind"].as<AST::Typedef::Fwd_kindEnum>() ==
                AST::Typedef::Fwd_kindEnum::UNION);
    ASSERT_TRUE(yaml["Typedef"]["name"].as<std::string>() == "ynbiqpmzjp");
}

TEST(YAMLImporter, Member)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Member.log");
    Logger::add_stderr_sink();
    std::string str("Member:\n"
                    "  filename: member.v\n"
                    "  line: 6\n"
                    "  unpacked_dims:\n"
                    "  init:\n"
                    "  type:\n"
                    "  name: mynbiqpmzj\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Member"]);
    ASSERT_TRUE(yaml["Member"]["filename"].as<std::string>() == "member.v");
    ASSERT_TRUE(yaml["Member"]["line"].as<int>() == 6);
    ASSERT_TRUE(yaml["Member"]["unpacked_dims"]);
    ASSERT_TRUE(yaml["Member"]["init"]);
    ASSERT_TRUE(yaml["Member"]["type"]);
    ASSERT_TRUE(yaml["Member"]["name"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLImporter, Arg)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Arg.log");
    Logger::add_stderr_sink();
    std::string str("Arg:\n"
                    "  filename: arg.v\n"
                    "  line: 3\n"
                    "  unpacked_dims:\n"
                    "  default_value:\n"
                    "  type:\n"
                    "  is_var: false\n"
                    "  direction: REF\n"
                    "  name: biqpmzjpls\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Arg"]);
    ASSERT_TRUE(yaml["Arg"]["filename"].as<std::string>() == "arg.v");
    ASSERT_TRUE(yaml["Arg"]["line"].as<int>() == 3);
    ASSERT_TRUE(yaml["Arg"]["unpacked_dims"]);
    ASSERT_TRUE(yaml["Arg"]["default_value"]);
    ASSERT_TRUE(yaml["Arg"]["type"]);
    ASSERT_TRUE(yaml["Arg"]["is_var"].as<bool>() == false);
    ASSERT_TRUE(yaml["Arg"]["direction"].as<AST::Arg::DirectionEnum>() ==
                AST::Arg::DirectionEnum::REF);
    ASSERT_TRUE(yaml["Arg"]["name"].as<std::string>() == "biqpmzjpls");
}

TEST(YAMLImporter, Genvar)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Genvar.log");
    Logger::add_stderr_sink();
    std::string str("Genvar:\n"
                    "  filename: genvar.v\n"
                    "  line: 6\n"
                    "  name: mynbiqpmzj\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Genvar"]);
    ASSERT_TRUE(yaml["Genvar"]["filename"].as<std::string>() == "genvar.v");
    ASSERT_TRUE(yaml["Genvar"]["line"].as<int>() == 6);
    ASSERT_TRUE(yaml["Genvar"]["name"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLImporter, Concat)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Concat.log");
    Logger::add_stderr_sink();
    std::string str("Concat:\n"
                    "  filename: concat.v\n"
                    "  line: 6\n"
                    "  list:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Concat"]);
    ASSERT_TRUE(yaml["Concat"]["filename"].as<std::string>() == "concat.v");
    ASSERT_TRUE(yaml["Concat"]["line"].as<int>() == 6);
    ASSERT_TRUE(yaml["Concat"]["list"]);
}

TEST(YAMLImporter, Lconcat)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Lconcat.log");
    Logger::add_stderr_sink();
    std::string str("Lconcat:\n"
                    "  filename: lconcat.v\n"
                    "  line: 7\n"
                    "  list:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Lconcat"]);
    ASSERT_TRUE(yaml["Lconcat"]["filename"].as<std::string>() == "lconcat.v");
    ASSERT_TRUE(yaml["Lconcat"]["line"].as<int>() == 7);
    ASSERT_TRUE(yaml["Lconcat"]["list"]);
}

TEST(YAMLImporter, Repeat)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Repeat.log");
    Logger::add_stderr_sink();
    std::string str("Repeat:\n"
                    "  filename: repeat.v\n"
                    "  line: 6\n"
                    "  value:\n"
                    "  times:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Repeat"]);
    ASSERT_TRUE(yaml["Repeat"]["filename"].as<std::string>() == "repeat.v");
    ASSERT_TRUE(yaml["Repeat"]["line"].as<int>() == 6);
    ASSERT_TRUE(yaml["Repeat"]["value"]);
    ASSERT_TRUE(yaml["Repeat"]["times"]);
}

TEST(YAMLImporter, AssignmentPattern)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.AssignmentPattern.log");
    Logger::add_stderr_sink();
    std::string str("AssignmentPattern:\n"
                    "  filename: assignmentpattern.v\n"
                    "  line: 17\n"
                    "  items:\n"
                    "  times:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["AssignmentPattern"]);
    ASSERT_TRUE(yaml["AssignmentPattern"]["filename"].as<std::string>() == "assignmentpattern.v");
    ASSERT_TRUE(yaml["AssignmentPattern"]["line"].as<int>() == 17);
    ASSERT_TRUE(yaml["AssignmentPattern"]["items"]);
    ASSERT_TRUE(yaml["AssignmentPattern"]["times"]);
}

TEST(YAMLImporter, PatternItem)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.PatternItem.log");
    Logger::add_stderr_sink();
    std::string str("PatternItem:\n"
                    "  filename: patternitem.v\n"
                    "  line: 11\n"
                    "  key:\n"
                    "  value:\n"
                    "  is_default: false\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["PatternItem"]);
    ASSERT_TRUE(yaml["PatternItem"]["filename"].as<std::string>() == "patternitem.v");
    ASSERT_TRUE(yaml["PatternItem"]["line"].as<int>() == 11);
    ASSERT_TRUE(yaml["PatternItem"]["key"]);
    ASSERT_TRUE(yaml["PatternItem"]["value"]);
    ASSERT_TRUE(yaml["PatternItem"]["is_default"].as<bool>() == false);
}

TEST(YAMLImporter, Cast)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Cast.log");
    Logger::add_stderr_sink();
    std::string str("Cast:\n"
                    "  filename: cast.v\n"
                    "  line: 4\n"
                    "  expr:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Cast"]);
    ASSERT_TRUE(yaml["Cast"]["filename"].as<std::string>() == "cast.v");
    ASSERT_TRUE(yaml["Cast"]["line"].as<int>() == 4);
    ASSERT_TRUE(yaml["Cast"]["expr"]);
}

TEST(YAMLImporter, TypeCast)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.TypeCast.log");
    Logger::add_stderr_sink();
    std::string str("TypeCast:\n"
                    "  filename: typecast.v\n"
                    "  line: 8\n"
                    "  target:\n"
                    "  expr:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["TypeCast"]);
    ASSERT_TRUE(yaml["TypeCast"]["filename"].as<std::string>() == "typecast.v");
    ASSERT_TRUE(yaml["TypeCast"]["line"].as<int>() == 8);
    ASSERT_TRUE(yaml["TypeCast"]["target"]);
    ASSERT_TRUE(yaml["TypeCast"]["expr"]);
}

TEST(YAMLImporter, SizeCast)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.SizeCast.log");
    Logger::add_stderr_sink();
    std::string str("SizeCast:\n"
                    "  filename: sizecast.v\n"
                    "  line: 8\n"
                    "  size:\n"
                    "  expr:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["SizeCast"]);
    ASSERT_TRUE(yaml["SizeCast"]["filename"].as<std::string>() == "sizecast.v");
    ASSERT_TRUE(yaml["SizeCast"]["line"].as<int>() == 8);
    ASSERT_TRUE(yaml["SizeCast"]["size"]);
    ASSERT_TRUE(yaml["SizeCast"]["expr"]);
}

TEST(YAMLImporter, SigningCast)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.SigningCast.log");
    Logger::add_stderr_sink();
    std::string str("SigningCast:\n"
                    "  filename: signingcast.v\n"
                    "  line: 11\n"
                    "  expr:\n"
                    "  signing: UNSIGNED\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["SigningCast"]);
    ASSERT_TRUE(yaml["SigningCast"]["filename"].as<std::string>() == "signingcast.v");
    ASSERT_TRUE(yaml["SigningCast"]["line"].as<int>() == 11);
    ASSERT_TRUE(yaml["SigningCast"]["expr"]);
    ASSERT_TRUE(yaml["SigningCast"]["signing"].as<AST::SigningCast::SigningEnum>() ==
                AST::SigningCast::SigningEnum::UNSIGNED);
}

TEST(YAMLImporter, ConstCast)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.ConstCast.log");
    Logger::add_stderr_sink();
    std::string str("ConstCast:\n"
                    "  filename: constcast.v\n"
                    "  line: 9\n"
                    "  expr:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["ConstCast"]);
    ASSERT_TRUE(yaml["ConstCast"]["filename"].as<std::string>() == "constcast.v");
    ASSERT_TRUE(yaml["ConstCast"]["line"].as<int>() == 9);
    ASSERT_TRUE(yaml["ConstCast"]["expr"]);
}

TEST(YAMLImporter, Indirect)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Indirect.log");
    Logger::add_stderr_sink();
    std::string str("Indirect:\n"
                    "  filename: indirect.v\n"
                    "  line: 8\n"
                    "  var:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Indirect"]);
    ASSERT_TRUE(yaml["Indirect"]["filename"].as<std::string>() == "indirect.v");
    ASSERT_TRUE(yaml["Indirect"]["line"].as<int>() == 8);
    ASSERT_TRUE(yaml["Indirect"]["var"]);
}

TEST(YAMLImporter, Partselect)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Partselect.log");
    Logger::add_stderr_sink();
    std::string str("Partselect:\n"
                    "  filename: partselect.v\n"
                    "  line: 10\n"
                    "  msb:\n"
                    "  lsb:\n"
                    "  var:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Partselect"]);
    ASSERT_TRUE(yaml["Partselect"]["filename"].as<std::string>() == "partselect.v");
    ASSERT_TRUE(yaml["Partselect"]["line"].as<int>() == 10);
    ASSERT_TRUE(yaml["Partselect"]["msb"]);
    ASSERT_TRUE(yaml["Partselect"]["lsb"]);
    ASSERT_TRUE(yaml["Partselect"]["var"]);
}

TEST(YAMLImporter, PartselectIndexed)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.PartselectIndexed.log");
    Logger::add_stderr_sink();
    std::string str("PartselectIndexed:\n"
                    "  filename: partselectindexed.v\n"
                    "  line: 17\n"
                    "  index:\n"
                    "  size:\n"
                    "  var:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["PartselectIndexed"]);
    ASSERT_TRUE(yaml["PartselectIndexed"]["filename"].as<std::string>() == "partselectindexed.v");
    ASSERT_TRUE(yaml["PartselectIndexed"]["line"].as<int>() == 17);
    ASSERT_TRUE(yaml["PartselectIndexed"]["index"]);
    ASSERT_TRUE(yaml["PartselectIndexed"]["size"]);
    ASSERT_TRUE(yaml["PartselectIndexed"]["var"]);
}

TEST(YAMLImporter, PartselectPlusIndexed)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.PartselectPlusIndexed.log");
    Logger::add_stderr_sink();
    std::string str("PartselectPlusIndexed:\n"
                    "  filename: partselectplusindexed.v\n"
                    "  line: 21\n"
                    "  index:\n"
                    "  size:\n"
                    "  var:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["PartselectPlusIndexed"]);
    ASSERT_TRUE(yaml["PartselectPlusIndexed"]["filename"].as<std::string>() ==
                "partselectplusindexed.v");
    ASSERT_TRUE(yaml["PartselectPlusIndexed"]["line"].as<int>() == 21);
    ASSERT_TRUE(yaml["PartselectPlusIndexed"]["index"]);
    ASSERT_TRUE(yaml["PartselectPlusIndexed"]["size"]);
    ASSERT_TRUE(yaml["PartselectPlusIndexed"]["var"]);
}

TEST(YAMLImporter, PartselectMinusIndexed)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.PartselectMinusIndexed.log");
    Logger::add_stderr_sink();
    std::string str("PartselectMinusIndexed:\n"
                    "  filename: partselectminusindexed.v\n"
                    "  line: 22\n"
                    "  index:\n"
                    "  size:\n"
                    "  var:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["PartselectMinusIndexed"]);
    ASSERT_TRUE(yaml["PartselectMinusIndexed"]["filename"].as<std::string>() ==
                "partselectminusindexed.v");
    ASSERT_TRUE(yaml["PartselectMinusIndexed"]["line"].as<int>() == 22);
    ASSERT_TRUE(yaml["PartselectMinusIndexed"]["index"]);
    ASSERT_TRUE(yaml["PartselectMinusIndexed"]["size"]);
    ASSERT_TRUE(yaml["PartselectMinusIndexed"]["var"]);
}

TEST(YAMLImporter, Pointer)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Pointer.log");
    Logger::add_stderr_sink();
    std::string str("Pointer:\n"
                    "  filename: pointer.v\n"
                    "  line: 7\n"
                    "  ptr:\n"
                    "  var:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Pointer"]);
    ASSERT_TRUE(yaml["Pointer"]["filename"].as<std::string>() == "pointer.v");
    ASSERT_TRUE(yaml["Pointer"]["line"].as<int>() == 7);
    ASSERT_TRUE(yaml["Pointer"]["ptr"]);
    ASSERT_TRUE(yaml["Pointer"]["var"]);
}

TEST(YAMLImporter, Lvalue)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Lvalue.log");
    Logger::add_stderr_sink();
    std::string str("Lvalue:\n"
                    "  filename: lvalue.v\n"
                    "  line: 6\n"
                    "  var:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Lvalue"]);
    ASSERT_TRUE(yaml["Lvalue"]["filename"].as<std::string>() == "lvalue.v");
    ASSERT_TRUE(yaml["Lvalue"]["line"].as<int>() == 6);
    ASSERT_TRUE(yaml["Lvalue"]["var"]);
}

TEST(YAMLImporter, Rvalue)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Rvalue.log");
    Logger::add_stderr_sink();
    std::string str("Rvalue:\n"
                    "  filename: rvalue.v\n"
                    "  line: 6\n"
                    "  var:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Rvalue"]);
    ASSERT_TRUE(yaml["Rvalue"]["filename"].as<std::string>() == "rvalue.v");
    ASSERT_TRUE(yaml["Rvalue"]["line"].as<int>() == 6);
    ASSERT_TRUE(yaml["Rvalue"]["var"]);
}

TEST(YAMLImporter, UnaryOperator)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.UnaryOperator.log");
    Logger::add_stderr_sink();
    std::string str("UnaryOperator:\n"
                    "  filename: unaryoperator.v\n"
                    "  line: 13\n"
                    "  right:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["UnaryOperator"]);
    ASSERT_TRUE(yaml["UnaryOperator"]["filename"].as<std::string>() == "unaryoperator.v");
    ASSERT_TRUE(yaml["UnaryOperator"]["line"].as<int>() == 13);
    ASSERT_TRUE(yaml["UnaryOperator"]["right"]);
}

TEST(YAMLImporter, Uplus)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Uplus.log");
    Logger::add_stderr_sink();
    std::string str("Uplus:\n"
                    "  filename: uplus.v\n"
                    "  line: 5\n"
                    "  right:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Uplus"]);
    ASSERT_TRUE(yaml["Uplus"]["filename"].as<std::string>() == "uplus.v");
    ASSERT_TRUE(yaml["Uplus"]["line"].as<int>() == 5);
    ASSERT_TRUE(yaml["Uplus"]["right"]);
}

TEST(YAMLImporter, Uminus)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Uminus.log");
    Logger::add_stderr_sink();
    std::string str("Uminus:\n"
                    "  filename: uminus.v\n"
                    "  line: 6\n"
                    "  right:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Uminus"]);
    ASSERT_TRUE(yaml["Uminus"]["filename"].as<std::string>() == "uminus.v");
    ASSERT_TRUE(yaml["Uminus"]["line"].as<int>() == 6);
    ASSERT_TRUE(yaml["Uminus"]["right"]);
}

TEST(YAMLImporter, Ulnot)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Ulnot.log");
    Logger::add_stderr_sink();
    std::string str("Ulnot:\n"
                    "  filename: ulnot.v\n"
                    "  line: 5\n"
                    "  right:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Ulnot"]);
    ASSERT_TRUE(yaml["Ulnot"]["filename"].as<std::string>() == "ulnot.v");
    ASSERT_TRUE(yaml["Ulnot"]["line"].as<int>() == 5);
    ASSERT_TRUE(yaml["Ulnot"]["right"]);
}

TEST(YAMLImporter, Unot)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Unot.log");
    Logger::add_stderr_sink();
    std::string str("Unot:\n"
                    "  filename: unot.v\n"
                    "  line: 4\n"
                    "  right:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Unot"]);
    ASSERT_TRUE(yaml["Unot"]["filename"].as<std::string>() == "unot.v");
    ASSERT_TRUE(yaml["Unot"]["line"].as<int>() == 4);
    ASSERT_TRUE(yaml["Unot"]["right"]);
}

TEST(YAMLImporter, Uand)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Uand.log");
    Logger::add_stderr_sink();
    std::string str("Uand:\n"
                    "  filename: uand.v\n"
                    "  line: 4\n"
                    "  right:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Uand"]);
    ASSERT_TRUE(yaml["Uand"]["filename"].as<std::string>() == "uand.v");
    ASSERT_TRUE(yaml["Uand"]["line"].as<int>() == 4);
    ASSERT_TRUE(yaml["Uand"]["right"]);
}

TEST(YAMLImporter, Unand)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Unand.log");
    Logger::add_stderr_sink();
    std::string str("Unand:\n"
                    "  filename: unand.v\n"
                    "  line: 5\n"
                    "  right:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Unand"]);
    ASSERT_TRUE(yaml["Unand"]["filename"].as<std::string>() == "unand.v");
    ASSERT_TRUE(yaml["Unand"]["line"].as<int>() == 5);
    ASSERT_TRUE(yaml["Unand"]["right"]);
}

TEST(YAMLImporter, Uor)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Uor.log");
    Logger::add_stderr_sink();
    std::string str("Uor:\n"
                    "  filename: uor.v\n"
                    "  line: 3\n"
                    "  right:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Uor"]);
    ASSERT_TRUE(yaml["Uor"]["filename"].as<std::string>() == "uor.v");
    ASSERT_TRUE(yaml["Uor"]["line"].as<int>() == 3);
    ASSERT_TRUE(yaml["Uor"]["right"]);
}

TEST(YAMLImporter, Unor)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Unor.log");
    Logger::add_stderr_sink();
    std::string str("Unor:\n"
                    "  filename: unor.v\n"
                    "  line: 4\n"
                    "  right:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Unor"]);
    ASSERT_TRUE(yaml["Unor"]["filename"].as<std::string>() == "unor.v");
    ASSERT_TRUE(yaml["Unor"]["line"].as<int>() == 4);
    ASSERT_TRUE(yaml["Unor"]["right"]);
}

TEST(YAMLImporter, Uxor)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Uxor.log");
    Logger::add_stderr_sink();
    std::string str("Uxor:\n"
                    "  filename: uxor.v\n"
                    "  line: 4\n"
                    "  right:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Uxor"]);
    ASSERT_TRUE(yaml["Uxor"]["filename"].as<std::string>() == "uxor.v");
    ASSERT_TRUE(yaml["Uxor"]["line"].as<int>() == 4);
    ASSERT_TRUE(yaml["Uxor"]["right"]);
}

TEST(YAMLImporter, Uxnor)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Uxnor.log");
    Logger::add_stderr_sink();
    std::string str("Uxnor:\n"
                    "  filename: uxnor.v\n"
                    "  line: 5\n"
                    "  right:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Uxnor"]);
    ASSERT_TRUE(yaml["Uxnor"]["filename"].as<std::string>() == "uxnor.v");
    ASSERT_TRUE(yaml["Uxnor"]["line"].as<int>() == 5);
    ASSERT_TRUE(yaml["Uxnor"]["right"]);
}

TEST(YAMLImporter, Operator)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Operator.log");
    Logger::add_stderr_sink();
    std::string str("Operator:\n"
                    "  filename: operator.v\n"
                    "  line: 8\n"
                    "  left:\n"
                    "  right:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Operator"]);
    ASSERT_TRUE(yaml["Operator"]["filename"].as<std::string>() == "operator.v");
    ASSERT_TRUE(yaml["Operator"]["line"].as<int>() == 8);
    ASSERT_TRUE(yaml["Operator"]["left"]);
    ASSERT_TRUE(yaml["Operator"]["right"]);
}

TEST(YAMLImporter, Power)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Power.log");
    Logger::add_stderr_sink();
    std::string str("Power:\n"
                    "  filename: power.v\n"
                    "  line: 5\n"
                    "  left:\n"
                    "  right:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Power"]);
    ASSERT_TRUE(yaml["Power"]["filename"].as<std::string>() == "power.v");
    ASSERT_TRUE(yaml["Power"]["line"].as<int>() == 5);
    ASSERT_TRUE(yaml["Power"]["left"]);
    ASSERT_TRUE(yaml["Power"]["right"]);
}

TEST(YAMLImporter, Times)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Times.log");
    Logger::add_stderr_sink();
    std::string str("Times:\n"
                    "  filename: times.v\n"
                    "  line: 5\n"
                    "  left:\n"
                    "  right:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Times"]);
    ASSERT_TRUE(yaml["Times"]["filename"].as<std::string>() == "times.v");
    ASSERT_TRUE(yaml["Times"]["line"].as<int>() == 5);
    ASSERT_TRUE(yaml["Times"]["left"]);
    ASSERT_TRUE(yaml["Times"]["right"]);
}

TEST(YAMLImporter, Divide)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Divide.log");
    Logger::add_stderr_sink();
    std::string str("Divide:\n"
                    "  filename: divide.v\n"
                    "  line: 6\n"
                    "  left:\n"
                    "  right:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Divide"]);
    ASSERT_TRUE(yaml["Divide"]["filename"].as<std::string>() == "divide.v");
    ASSERT_TRUE(yaml["Divide"]["line"].as<int>() == 6);
    ASSERT_TRUE(yaml["Divide"]["left"]);
    ASSERT_TRUE(yaml["Divide"]["right"]);
}

TEST(YAMLImporter, Mod)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Mod.log");
    Logger::add_stderr_sink();
    std::string str("Mod:\n"
                    "  filename: mod.v\n"
                    "  line: 3\n"
                    "  left:\n"
                    "  right:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Mod"]);
    ASSERT_TRUE(yaml["Mod"]["filename"].as<std::string>() == "mod.v");
    ASSERT_TRUE(yaml["Mod"]["line"].as<int>() == 3);
    ASSERT_TRUE(yaml["Mod"]["left"]);
    ASSERT_TRUE(yaml["Mod"]["right"]);
}

TEST(YAMLImporter, Plus)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Plus.log");
    Logger::add_stderr_sink();
    std::string str("Plus:\n"
                    "  filename: plus.v\n"
                    "  line: 4\n"
                    "  left:\n"
                    "  right:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Plus"]);
    ASSERT_TRUE(yaml["Plus"]["filename"].as<std::string>() == "plus.v");
    ASSERT_TRUE(yaml["Plus"]["line"].as<int>() == 4);
    ASSERT_TRUE(yaml["Plus"]["left"]);
    ASSERT_TRUE(yaml["Plus"]["right"]);
}

TEST(YAMLImporter, Minus)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Minus.log");
    Logger::add_stderr_sink();
    std::string str("Minus:\n"
                    "  filename: minus.v\n"
                    "  line: 5\n"
                    "  left:\n"
                    "  right:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Minus"]);
    ASSERT_TRUE(yaml["Minus"]["filename"].as<std::string>() == "minus.v");
    ASSERT_TRUE(yaml["Minus"]["line"].as<int>() == 5);
    ASSERT_TRUE(yaml["Minus"]["left"]);
    ASSERT_TRUE(yaml["Minus"]["right"]);
}

TEST(YAMLImporter, Sll)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Sll.log");
    Logger::add_stderr_sink();
    std::string str("Sll:\n"
                    "  filename: sll.v\n"
                    "  line: 3\n"
                    "  left:\n"
                    "  right:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Sll"]);
    ASSERT_TRUE(yaml["Sll"]["filename"].as<std::string>() == "sll.v");
    ASSERT_TRUE(yaml["Sll"]["line"].as<int>() == 3);
    ASSERT_TRUE(yaml["Sll"]["left"]);
    ASSERT_TRUE(yaml["Sll"]["right"]);
}

TEST(YAMLImporter, Srl)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Srl.log");
    Logger::add_stderr_sink();
    std::string str("Srl:\n"
                    "  filename: srl.v\n"
                    "  line: 3\n"
                    "  left:\n"
                    "  right:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Srl"]);
    ASSERT_TRUE(yaml["Srl"]["filename"].as<std::string>() == "srl.v");
    ASSERT_TRUE(yaml["Srl"]["line"].as<int>() == 3);
    ASSERT_TRUE(yaml["Srl"]["left"]);
    ASSERT_TRUE(yaml["Srl"]["right"]);
}

TEST(YAMLImporter, Sra)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Sra.log");
    Logger::add_stderr_sink();
    std::string str("Sra:\n"
                    "  filename: sra.v\n"
                    "  line: 3\n"
                    "  left:\n"
                    "  right:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Sra"]);
    ASSERT_TRUE(yaml["Sra"]["filename"].as<std::string>() == "sra.v");
    ASSERT_TRUE(yaml["Sra"]["line"].as<int>() == 3);
    ASSERT_TRUE(yaml["Sra"]["left"]);
    ASSERT_TRUE(yaml["Sra"]["right"]);
}

TEST(YAMLImporter, LessThan)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.LessThan.log");
    Logger::add_stderr_sink();
    std::string str("LessThan:\n"
                    "  filename: lessthan.v\n"
                    "  line: 8\n"
                    "  left:\n"
                    "  right:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["LessThan"]);
    ASSERT_TRUE(yaml["LessThan"]["filename"].as<std::string>() == "lessthan.v");
    ASSERT_TRUE(yaml["LessThan"]["line"].as<int>() == 8);
    ASSERT_TRUE(yaml["LessThan"]["left"]);
    ASSERT_TRUE(yaml["LessThan"]["right"]);
}

TEST(YAMLImporter, GreaterThan)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.GreaterThan.log");
    Logger::add_stderr_sink();
    std::string str("GreaterThan:\n"
                    "  filename: greaterthan.v\n"
                    "  line: 11\n"
                    "  left:\n"
                    "  right:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["GreaterThan"]);
    ASSERT_TRUE(yaml["GreaterThan"]["filename"].as<std::string>() == "greaterthan.v");
    ASSERT_TRUE(yaml["GreaterThan"]["line"].as<int>() == 11);
    ASSERT_TRUE(yaml["GreaterThan"]["left"]);
    ASSERT_TRUE(yaml["GreaterThan"]["right"]);
}

TEST(YAMLImporter, LessEq)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.LessEq.log");
    Logger::add_stderr_sink();
    std::string str("LessEq:\n"
                    "  filename: lesseq.v\n"
                    "  line: 6\n"
                    "  left:\n"
                    "  right:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["LessEq"]);
    ASSERT_TRUE(yaml["LessEq"]["filename"].as<std::string>() == "lesseq.v");
    ASSERT_TRUE(yaml["LessEq"]["line"].as<int>() == 6);
    ASSERT_TRUE(yaml["LessEq"]["left"]);
    ASSERT_TRUE(yaml["LessEq"]["right"]);
}

TEST(YAMLImporter, GreaterEq)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.GreaterEq.log");
    Logger::add_stderr_sink();
    std::string str("GreaterEq:\n"
                    "  filename: greatereq.v\n"
                    "  line: 9\n"
                    "  left:\n"
                    "  right:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["GreaterEq"]);
    ASSERT_TRUE(yaml["GreaterEq"]["filename"].as<std::string>() == "greatereq.v");
    ASSERT_TRUE(yaml["GreaterEq"]["line"].as<int>() == 9);
    ASSERT_TRUE(yaml["GreaterEq"]["left"]);
    ASSERT_TRUE(yaml["GreaterEq"]["right"]);
}

TEST(YAMLImporter, Eq)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Eq.log");
    Logger::add_stderr_sink();
    std::string str("Eq:\n"
                    "  filename: eq.v\n"
                    "  line: 2\n"
                    "  left:\n"
                    "  right:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Eq"]);
    ASSERT_TRUE(yaml["Eq"]["filename"].as<std::string>() == "eq.v");
    ASSERT_TRUE(yaml["Eq"]["line"].as<int>() == 2);
    ASSERT_TRUE(yaml["Eq"]["left"]);
    ASSERT_TRUE(yaml["Eq"]["right"]);
}

TEST(YAMLImporter, NotEq)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.NotEq.log");
    Logger::add_stderr_sink();
    std::string str("NotEq:\n"
                    "  filename: noteq.v\n"
                    "  line: 5\n"
                    "  left:\n"
                    "  right:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["NotEq"]);
    ASSERT_TRUE(yaml["NotEq"]["filename"].as<std::string>() == "noteq.v");
    ASSERT_TRUE(yaml["NotEq"]["line"].as<int>() == 5);
    ASSERT_TRUE(yaml["NotEq"]["left"]);
    ASSERT_TRUE(yaml["NotEq"]["right"]);
}

TEST(YAMLImporter, Eql)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Eql.log");
    Logger::add_stderr_sink();
    std::string str("Eql:\n"
                    "  filename: eql.v\n"
                    "  line: 3\n"
                    "  left:\n"
                    "  right:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Eql"]);
    ASSERT_TRUE(yaml["Eql"]["filename"].as<std::string>() == "eql.v");
    ASSERT_TRUE(yaml["Eql"]["line"].as<int>() == 3);
    ASSERT_TRUE(yaml["Eql"]["left"]);
    ASSERT_TRUE(yaml["Eql"]["right"]);
}

TEST(YAMLImporter, NotEql)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.NotEql.log");
    Logger::add_stderr_sink();
    std::string str("NotEql:\n"
                    "  filename: noteql.v\n"
                    "  line: 6\n"
                    "  left:\n"
                    "  right:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["NotEql"]);
    ASSERT_TRUE(yaml["NotEql"]["filename"].as<std::string>() == "noteql.v");
    ASSERT_TRUE(yaml["NotEql"]["line"].as<int>() == 6);
    ASSERT_TRUE(yaml["NotEql"]["left"]);
    ASSERT_TRUE(yaml["NotEql"]["right"]);
}

TEST(YAMLImporter, And)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.And.log");
    Logger::add_stderr_sink();
    std::string str("And:\n"
                    "  filename: and.v\n"
                    "  line: 3\n"
                    "  left:\n"
                    "  right:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["And"]);
    ASSERT_TRUE(yaml["And"]["filename"].as<std::string>() == "and.v");
    ASSERT_TRUE(yaml["And"]["line"].as<int>() == 3);
    ASSERT_TRUE(yaml["And"]["left"]);
    ASSERT_TRUE(yaml["And"]["right"]);
}

TEST(YAMLImporter, Xor)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Xor.log");
    Logger::add_stderr_sink();
    std::string str("Xor:\n"
                    "  filename: xor.v\n"
                    "  line: 3\n"
                    "  left:\n"
                    "  right:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Xor"]);
    ASSERT_TRUE(yaml["Xor"]["filename"].as<std::string>() == "xor.v");
    ASSERT_TRUE(yaml["Xor"]["line"].as<int>() == 3);
    ASSERT_TRUE(yaml["Xor"]["left"]);
    ASSERT_TRUE(yaml["Xor"]["right"]);
}

TEST(YAMLImporter, Xnor)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Xnor.log");
    Logger::add_stderr_sink();
    std::string str("Xnor:\n"
                    "  filename: xnor.v\n"
                    "  line: 4\n"
                    "  left:\n"
                    "  right:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Xnor"]);
    ASSERT_TRUE(yaml["Xnor"]["filename"].as<std::string>() == "xnor.v");
    ASSERT_TRUE(yaml["Xnor"]["line"].as<int>() == 4);
    ASSERT_TRUE(yaml["Xnor"]["left"]);
    ASSERT_TRUE(yaml["Xnor"]["right"]);
}

TEST(YAMLImporter, Or)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Or.log");
    Logger::add_stderr_sink();
    std::string str("Or:\n"
                    "  filename: or.v\n"
                    "  line: 2\n"
                    "  left:\n"
                    "  right:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Or"]);
    ASSERT_TRUE(yaml["Or"]["filename"].as<std::string>() == "or.v");
    ASSERT_TRUE(yaml["Or"]["line"].as<int>() == 2);
    ASSERT_TRUE(yaml["Or"]["left"]);
    ASSERT_TRUE(yaml["Or"]["right"]);
}

TEST(YAMLImporter, Land)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Land.log");
    Logger::add_stderr_sink();
    std::string str("Land:\n"
                    "  filename: land.v\n"
                    "  line: 4\n"
                    "  left:\n"
                    "  right:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Land"]);
    ASSERT_TRUE(yaml["Land"]["filename"].as<std::string>() == "land.v");
    ASSERT_TRUE(yaml["Land"]["line"].as<int>() == 4);
    ASSERT_TRUE(yaml["Land"]["left"]);
    ASSERT_TRUE(yaml["Land"]["right"]);
}

TEST(YAMLImporter, Lor)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Lor.log");
    Logger::add_stderr_sink();
    std::string str("Lor:\n"
                    "  filename: lor.v\n"
                    "  line: 3\n"
                    "  left:\n"
                    "  right:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Lor"]);
    ASSERT_TRUE(yaml["Lor"]["filename"].as<std::string>() == "lor.v");
    ASSERT_TRUE(yaml["Lor"]["line"].as<int>() == 3);
    ASSERT_TRUE(yaml["Lor"]["left"]);
    ASSERT_TRUE(yaml["Lor"]["right"]);
}

TEST(YAMLImporter, Cond)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Cond.log");
    Logger::add_stderr_sink();
    std::string str("Cond:\n"
                    "  filename: cond.v\n"
                    "  line: 4\n"
                    "  cond:\n"
                    "  left:\n"
                    "  right:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Cond"]);
    ASSERT_TRUE(yaml["Cond"]["filename"].as<std::string>() == "cond.v");
    ASSERT_TRUE(yaml["Cond"]["line"].as<int>() == 4);
    ASSERT_TRUE(yaml["Cond"]["cond"]);
    ASSERT_TRUE(yaml["Cond"]["left"]);
    ASSERT_TRUE(yaml["Cond"]["right"]);
}

TEST(YAMLImporter, Always)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Always.log");
    Logger::add_stderr_sink();
    std::string str("Always:\n"
                    "  filename: always.v\n"
                    "  line: 6\n"
                    "  senslist:\n"
                    "  statement:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Always"]);
    ASSERT_TRUE(yaml["Always"]["filename"].as<std::string>() == "always.v");
    ASSERT_TRUE(yaml["Always"]["line"].as<int>() == 6);
    ASSERT_TRUE(yaml["Always"]["senslist"]);
    ASSERT_TRUE(yaml["Always"]["statement"]);
}

TEST(YAMLImporter, AlwaysFF)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.AlwaysFF.log");
    Logger::add_stderr_sink();
    std::string str("AlwaysFF:\n"
                    "  filename: alwaysff.v\n"
                    "  line: 8\n"
                    "  senslist:\n"
                    "  statement:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["AlwaysFF"]);
    ASSERT_TRUE(yaml["AlwaysFF"]["filename"].as<std::string>() == "alwaysff.v");
    ASSERT_TRUE(yaml["AlwaysFF"]["line"].as<int>() == 8);
    ASSERT_TRUE(yaml["AlwaysFF"]["senslist"]);
    ASSERT_TRUE(yaml["AlwaysFF"]["statement"]);
}

TEST(YAMLImporter, AlwaysComb)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.AlwaysComb.log");
    Logger::add_stderr_sink();
    std::string str("AlwaysComb:\n"
                    "  filename: alwayscomb.v\n"
                    "  line: 10\n"
                    "  senslist:\n"
                    "  statement:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["AlwaysComb"]);
    ASSERT_TRUE(yaml["AlwaysComb"]["filename"].as<std::string>() == "alwayscomb.v");
    ASSERT_TRUE(yaml["AlwaysComb"]["line"].as<int>() == 10);
    ASSERT_TRUE(yaml["AlwaysComb"]["senslist"]);
    ASSERT_TRUE(yaml["AlwaysComb"]["statement"]);
}

TEST(YAMLImporter, AlwaysLatch)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.AlwaysLatch.log");
    Logger::add_stderr_sink();
    std::string str("AlwaysLatch:\n"
                    "  filename: alwayslatch.v\n"
                    "  line: 11\n"
                    "  senslist:\n"
                    "  statement:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["AlwaysLatch"]);
    ASSERT_TRUE(yaml["AlwaysLatch"]["filename"].as<std::string>() == "alwayslatch.v");
    ASSERT_TRUE(yaml["AlwaysLatch"]["line"].as<int>() == 11);
    ASSERT_TRUE(yaml["AlwaysLatch"]["senslist"]);
    ASSERT_TRUE(yaml["AlwaysLatch"]["statement"]);
}

TEST(YAMLImporter, Senslist)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Senslist.log");
    Logger::add_stderr_sink();
    std::string str("Senslist:\n"
                    "  filename: senslist.v\n"
                    "  line: 8\n"
                    "  list:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Senslist"]);
    ASSERT_TRUE(yaml["Senslist"]["filename"].as<std::string>() == "senslist.v");
    ASSERT_TRUE(yaml["Senslist"]["line"].as<int>() == 8);
    ASSERT_TRUE(yaml["Senslist"]["list"]);
}

TEST(YAMLImporter, Sens)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Sens.log");
    Logger::add_stderr_sink();
    std::string str("Sens:\n"
                    "  filename: sens.v\n"
                    "  line: 4\n"
                    "  sig:\n"
                    "  type: NONE\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Sens"]);
    ASSERT_TRUE(yaml["Sens"]["filename"].as<std::string>() == "sens.v");
    ASSERT_TRUE(yaml["Sens"]["line"].as<int>() == 4);
    ASSERT_TRUE(yaml["Sens"]["sig"]);
    ASSERT_TRUE(yaml["Sens"]["type"].as<AST::Sens::TypeEnum>() == AST::Sens::TypeEnum::NONE);
}

TEST(YAMLImporter, Defparamlist)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Defparamlist.log");
    Logger::add_stderr_sink();
    std::string str("Defparamlist:\n"
                    "  filename: defparamlist.v\n"
                    "  line: 12\n"
                    "  list:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Defparamlist"]);
    ASSERT_TRUE(yaml["Defparamlist"]["filename"].as<std::string>() == "defparamlist.v");
    ASSERT_TRUE(yaml["Defparamlist"]["line"].as<int>() == 12);
    ASSERT_TRUE(yaml["Defparamlist"]["list"]);
}

TEST(YAMLImporter, Defparam)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Defparam.log");
    Logger::add_stderr_sink();
    std::string str("Defparam:\n"
                    "  filename: defparam.v\n"
                    "  line: 8\n"
                    "  identifier:\n"
                    "  right:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Defparam"]);
    ASSERT_TRUE(yaml["Defparam"]["filename"].as<std::string>() == "defparam.v");
    ASSERT_TRUE(yaml["Defparam"]["line"].as<int>() == 8);
    ASSERT_TRUE(yaml["Defparam"]["identifier"]);
    ASSERT_TRUE(yaml["Defparam"]["right"]);
}

TEST(YAMLImporter, Assign)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Assign.log");
    Logger::add_stderr_sink();
    std::string str("Assign:\n"
                    "  filename: assign.v\n"
                    "  line: 6\n"
                    "  left:\n"
                    "  right:\n"
                    "  ldelay:\n"
                    "  rdelay:\n");

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

TEST(YAMLImporter, BlockingSubstitution)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.BlockingSubstitution.log");
    Logger::add_stderr_sink();
    std::string str("BlockingSubstitution:\n"
                    "  filename: blockingsubstitution.v\n"
                    "  line: 20\n"
                    "  left:\n"
                    "  right:\n"
                    "  ldelay:\n"
                    "  rdelay:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["BlockingSubstitution"]);
    ASSERT_TRUE(yaml["BlockingSubstitution"]["filename"].as<std::string>() ==
                "blockingsubstitution.v");
    ASSERT_TRUE(yaml["BlockingSubstitution"]["line"].as<int>() == 20);
    ASSERT_TRUE(yaml["BlockingSubstitution"]["left"]);
    ASSERT_TRUE(yaml["BlockingSubstitution"]["right"]);
    ASSERT_TRUE(yaml["BlockingSubstitution"]["ldelay"]);
    ASSERT_TRUE(yaml["BlockingSubstitution"]["rdelay"]);
}

TEST(YAMLImporter, NonblockingSubstitution)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.NonblockingSubstitution.log");
    Logger::add_stderr_sink();
    std::string str("NonblockingSubstitution:\n"
                    "  filename: nonblockingsubstitution.v\n"
                    "  line: 23\n"
                    "  left:\n"
                    "  right:\n"
                    "  ldelay:\n"
                    "  rdelay:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["NonblockingSubstitution"]);
    ASSERT_TRUE(yaml["NonblockingSubstitution"]["filename"].as<std::string>() ==
                "nonblockingsubstitution.v");
    ASSERT_TRUE(yaml["NonblockingSubstitution"]["line"].as<int>() == 23);
    ASSERT_TRUE(yaml["NonblockingSubstitution"]["left"]);
    ASSERT_TRUE(yaml["NonblockingSubstitution"]["right"]);
    ASSERT_TRUE(yaml["NonblockingSubstitution"]["ldelay"]);
    ASSERT_TRUE(yaml["NonblockingSubstitution"]["rdelay"]);
}

TEST(YAMLImporter, IfStatement)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.IfStatement.log");
    Logger::add_stderr_sink();
    std::string str("IfStatement:\n"
                    "  filename: ifstatement.v\n"
                    "  line: 11\n"
                    "  cond:\n"
                    "  true_statement:\n"
                    "  false_statement:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["IfStatement"]);
    ASSERT_TRUE(yaml["IfStatement"]["filename"].as<std::string>() == "ifstatement.v");
    ASSERT_TRUE(yaml["IfStatement"]["line"].as<int>() == 11);
    ASSERT_TRUE(yaml["IfStatement"]["cond"]);
    ASSERT_TRUE(yaml["IfStatement"]["true_statement"]);
    ASSERT_TRUE(yaml["IfStatement"]["false_statement"]);
}

TEST(YAMLImporter, RepeatStatement)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.RepeatStatement.log");
    Logger::add_stderr_sink();
    std::string str("RepeatStatement:\n"
                    "  filename: repeatstatement.v\n"
                    "  line: 15\n"
                    "  times:\n"
                    "  statement:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["RepeatStatement"]);
    ASSERT_TRUE(yaml["RepeatStatement"]["filename"].as<std::string>() == "repeatstatement.v");
    ASSERT_TRUE(yaml["RepeatStatement"]["line"].as<int>() == 15);
    ASSERT_TRUE(yaml["RepeatStatement"]["times"]);
    ASSERT_TRUE(yaml["RepeatStatement"]["statement"]);
}

TEST(YAMLImporter, ForStatement)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.ForStatement.log");
    Logger::add_stderr_sink();
    std::string str("ForStatement:\n"
                    "  filename: forstatement.v\n"
                    "  line: 12\n"
                    "  pre:\n"
                    "  cond:\n"
                    "  post:\n"
                    "  statement:\n");

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

TEST(YAMLImporter, WhileStatement)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.WhileStatement.log");
    Logger::add_stderr_sink();
    std::string str("WhileStatement:\n"
                    "  filename: whilestatement.v\n"
                    "  line: 14\n"
                    "  cond:\n"
                    "  statement:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["WhileStatement"]);
    ASSERT_TRUE(yaml["WhileStatement"]["filename"].as<std::string>() == "whilestatement.v");
    ASSERT_TRUE(yaml["WhileStatement"]["line"].as<int>() == 14);
    ASSERT_TRUE(yaml["WhileStatement"]["cond"]);
    ASSERT_TRUE(yaml["WhileStatement"]["statement"]);
}

TEST(YAMLImporter, CaseStatement)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.CaseStatement.log");
    Logger::add_stderr_sink();
    std::string str("CaseStatement:\n"
                    "  filename: casestatement.v\n"
                    "  line: 13\n"
                    "  comp:\n"
                    "  caselist:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["CaseStatement"]);
    ASSERT_TRUE(yaml["CaseStatement"]["filename"].as<std::string>() == "casestatement.v");
    ASSERT_TRUE(yaml["CaseStatement"]["line"].as<int>() == 13);
    ASSERT_TRUE(yaml["CaseStatement"]["comp"]);
    ASSERT_TRUE(yaml["CaseStatement"]["caselist"]);
}

TEST(YAMLImporter, CasexStatement)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.CasexStatement.log");
    Logger::add_stderr_sink();
    std::string str("CasexStatement:\n"
                    "  filename: casexstatement.v\n"
                    "  line: 14\n"
                    "  comp:\n"
                    "  caselist:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["CasexStatement"]);
    ASSERT_TRUE(yaml["CasexStatement"]["filename"].as<std::string>() == "casexstatement.v");
    ASSERT_TRUE(yaml["CasexStatement"]["line"].as<int>() == 14);
    ASSERT_TRUE(yaml["CasexStatement"]["comp"]);
    ASSERT_TRUE(yaml["CasexStatement"]["caselist"]);
}

TEST(YAMLImporter, CasezStatement)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.CasezStatement.log");
    Logger::add_stderr_sink();
    std::string str("CasezStatement:\n"
                    "  filename: casezstatement.v\n"
                    "  line: 14\n"
                    "  comp:\n"
                    "  caselist:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["CasezStatement"]);
    ASSERT_TRUE(yaml["CasezStatement"]["filename"].as<std::string>() == "casezstatement.v");
    ASSERT_TRUE(yaml["CasezStatement"]["line"].as<int>() == 14);
    ASSERT_TRUE(yaml["CasezStatement"]["comp"]);
    ASSERT_TRUE(yaml["CasezStatement"]["caselist"]);
}

TEST(YAMLImporter, UniqueCaseStatement)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.UniqueCaseStatement.log");
    Logger::add_stderr_sink();
    std::string str("UniqueCaseStatement:\n"
                    "  filename: uniquecasestatement.v\n"
                    "  line: 19\n"
                    "  comp:\n"
                    "  caselist:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["UniqueCaseStatement"]);
    ASSERT_TRUE(yaml["UniqueCaseStatement"]["filename"].as<std::string>() ==
                "uniquecasestatement.v");
    ASSERT_TRUE(yaml["UniqueCaseStatement"]["line"].as<int>() == 19);
    ASSERT_TRUE(yaml["UniqueCaseStatement"]["comp"]);
    ASSERT_TRUE(yaml["UniqueCaseStatement"]["caselist"]);
}

TEST(YAMLImporter, PriorityCaseStatement)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.PriorityCaseStatement.log");
    Logger::add_stderr_sink();
    std::string str("PriorityCaseStatement:\n"
                    "  filename: prioritycasestatement.v\n"
                    "  line: 21\n"
                    "  comp:\n"
                    "  caselist:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["PriorityCaseStatement"]);
    ASSERT_TRUE(yaml["PriorityCaseStatement"]["filename"].as<std::string>() ==
                "prioritycasestatement.v");
    ASSERT_TRUE(yaml["PriorityCaseStatement"]["line"].as<int>() == 21);
    ASSERT_TRUE(yaml["PriorityCaseStatement"]["comp"]);
    ASSERT_TRUE(yaml["PriorityCaseStatement"]["caselist"]);
}

TEST(YAMLImporter, Case)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Case.log");
    Logger::add_stderr_sink();
    std::string str("Case:\n"
                    "  filename: case.v\n"
                    "  line: 4\n"
                    "  cond:\n"
                    "  statement:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Case"]);
    ASSERT_TRUE(yaml["Case"]["filename"].as<std::string>() == "case.v");
    ASSERT_TRUE(yaml["Case"]["line"].as<int>() == 4);
    ASSERT_TRUE(yaml["Case"]["cond"]);
    ASSERT_TRUE(yaml["Case"]["statement"]);
}

TEST(YAMLImporter, Block)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Block.log");
    Logger::add_stderr_sink();
    std::string str("Block:\n"
                    "  filename: block.v\n"
                    "  line: 5\n"
                    "  statements:\n"
                    "  scope: mynbiqpmzj\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Block"]);
    ASSERT_TRUE(yaml["Block"]["filename"].as<std::string>() == "block.v");
    ASSERT_TRUE(yaml["Block"]["line"].as<int>() == 5);
    ASSERT_TRUE(yaml["Block"]["statements"]);
    ASSERT_TRUE(yaml["Block"]["scope"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLImporter, Initial)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Initial.log");
    Logger::add_stderr_sink();
    std::string str("Initial:\n"
                    "  filename: initial.v\n"
                    "  line: 7\n"
                    "  statement:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Initial"]);
    ASSERT_TRUE(yaml["Initial"]["filename"].as<std::string>() == "initial.v");
    ASSERT_TRUE(yaml["Initial"]["line"].as<int>() == 7);
    ASSERT_TRUE(yaml["Initial"]["statement"]);
}

TEST(YAMLImporter, EventStatement)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.EventStatement.log");
    Logger::add_stderr_sink();
    std::string str("EventStatement:\n"
                    "  filename: eventstatement.v\n"
                    "  line: 14\n"
                    "  senslist:\n"
                    "  statement:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["EventStatement"]);
    ASSERT_TRUE(yaml["EventStatement"]["filename"].as<std::string>() == "eventstatement.v");
    ASSERT_TRUE(yaml["EventStatement"]["line"].as<int>() == 14);
    ASSERT_TRUE(yaml["EventStatement"]["senslist"]);
    ASSERT_TRUE(yaml["EventStatement"]["statement"]);
}

TEST(YAMLImporter, WaitStatement)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.WaitStatement.log");
    Logger::add_stderr_sink();
    std::string str("WaitStatement:\n"
                    "  filename: waitstatement.v\n"
                    "  line: 13\n"
                    "  cond:\n"
                    "  statement:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["WaitStatement"]);
    ASSERT_TRUE(yaml["WaitStatement"]["filename"].as<std::string>() == "waitstatement.v");
    ASSERT_TRUE(yaml["WaitStatement"]["line"].as<int>() == 13);
    ASSERT_TRUE(yaml["WaitStatement"]["cond"]);
    ASSERT_TRUE(yaml["WaitStatement"]["statement"]);
}

TEST(YAMLImporter, ForeverStatement)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.ForeverStatement.log");
    Logger::add_stderr_sink();
    std::string str("ForeverStatement:\n"
                    "  filename: foreverstatement.v\n"
                    "  line: 16\n"
                    "  statement:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["ForeverStatement"]);
    ASSERT_TRUE(yaml["ForeverStatement"]["filename"].as<std::string>() == "foreverstatement.v");
    ASSERT_TRUE(yaml["ForeverStatement"]["line"].as<int>() == 16);
    ASSERT_TRUE(yaml["ForeverStatement"]["statement"]);
}

TEST(YAMLImporter, DelayStatement)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.DelayStatement.log");
    Logger::add_stderr_sink();
    std::string str("DelayStatement:\n"
                    "  filename: delaystatement.v\n"
                    "  line: 14\n"
                    "  delay:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["DelayStatement"]);
    ASSERT_TRUE(yaml["DelayStatement"]["filename"].as<std::string>() == "delaystatement.v");
    ASSERT_TRUE(yaml["DelayStatement"]["line"].as<int>() == 14);
    ASSERT_TRUE(yaml["DelayStatement"]["delay"]);
}

TEST(YAMLImporter, Instancelist)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Instancelist.log");
    Logger::add_stderr_sink();
    std::string str("Instancelist:\n"
                    "  filename: instancelist.v\n"
                    "  line: 12\n"
                    "  parameterlist:\n"
                    "  instances:\n"
                    "  module: mynbiqpmzj\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Instancelist"]);
    ASSERT_TRUE(yaml["Instancelist"]["filename"].as<std::string>() == "instancelist.v");
    ASSERT_TRUE(yaml["Instancelist"]["line"].as<int>() == 12);
    ASSERT_TRUE(yaml["Instancelist"]["parameterlist"]);
    ASSERT_TRUE(yaml["Instancelist"]["instances"]);
    ASSERT_TRUE(yaml["Instancelist"]["module"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLImporter, Instance)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Instance.log");
    Logger::add_stderr_sink();
    std::string str("Instance:\n"
                    "  filename: instance.v\n"
                    "  line: 8\n"
                    "  array:\n"
                    "  parameterlist:\n"
                    "  portlist:\n"
                    "  module: mynbiqpmzj\n"
                    "  name: plsgqejeyd\n");

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

TEST(YAMLImporter, ParamArg)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.ParamArg.log");
    Logger::add_stderr_sink();
    std::string str("ParamArg:\n"
                    "  filename: paramarg.v\n"
                    "  line: 8\n"
                    "  value:\n"
                    "  name: mynbiqpmzj\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["ParamArg"]);
    ASSERT_TRUE(yaml["ParamArg"]["filename"].as<std::string>() == "paramarg.v");
    ASSERT_TRUE(yaml["ParamArg"]["line"].as<int>() == 8);
    ASSERT_TRUE(yaml["ParamArg"]["value"]);
    ASSERT_TRUE(yaml["ParamArg"]["name"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLImporter, PortArg)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.PortArg.log");
    Logger::add_stderr_sink();
    std::string str("PortArg:\n"
                    "  filename: portarg.v\n"
                    "  line: 7\n"
                    "  value:\n"
                    "  name: mynbiqpmzj\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["PortArg"]);
    ASSERT_TRUE(yaml["PortArg"]["filename"].as<std::string>() == "portarg.v");
    ASSERT_TRUE(yaml["PortArg"]["line"].as<int>() == 7);
    ASSERT_TRUE(yaml["PortArg"]["value"]);
    ASSERT_TRUE(yaml["PortArg"]["name"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLImporter, Function)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Function.log");
    Logger::add_stderr_sink();
    std::string str("Function:\n"
                    "  filename: function.v\n"
                    "  line: 8\n"
                    "  return_type:\n"
                    "  args:\n"
                    "  statements:\n"
                    "  name: mynbiqpmzj\n"
                    "  lifetime: AUTOMATIC\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Function"]);
    ASSERT_TRUE(yaml["Function"]["filename"].as<std::string>() == "function.v");
    ASSERT_TRUE(yaml["Function"]["line"].as<int>() == 8);
    ASSERT_TRUE(yaml["Function"]["return_type"]);
    ASSERT_TRUE(yaml["Function"]["args"]);
    ASSERT_TRUE(yaml["Function"]["statements"]);
    ASSERT_TRUE(yaml["Function"]["name"].as<std::string>() == "mynbiqpmzj");
    ASSERT_TRUE(yaml["Function"]["lifetime"].as<AST::Function::LifetimeEnum>() ==
                AST::Function::LifetimeEnum::AUTOMATIC);
}

TEST(YAMLImporter, FunctionCall)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.FunctionCall.log");
    Logger::add_stderr_sink();
    std::string str("FunctionCall:\n"
                    "  filename: functioncall.v\n"
                    "  line: 12\n"
                    "  args:\n"
                    "  name: mynbiqpmzj\n"
                    "  package: plsgqejeyd\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["FunctionCall"]);
    ASSERT_TRUE(yaml["FunctionCall"]["filename"].as<std::string>() == "functioncall.v");
    ASSERT_TRUE(yaml["FunctionCall"]["line"].as<int>() == 12);
    ASSERT_TRUE(yaml["FunctionCall"]["args"]);
    ASSERT_TRUE(yaml["FunctionCall"]["name"].as<std::string>() == "mynbiqpmzj");
    ASSERT_TRUE(yaml["FunctionCall"]["package"].as<std::string>() == "plsgqejeyd");
}

TEST(YAMLImporter, Task)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Task.log");
    Logger::add_stderr_sink();
    std::string str("Task:\n"
                    "  filename: task.v\n"
                    "  line: 4\n"
                    "  args:\n"
                    "  statements:\n"
                    "  name: mynbiqpmzj\n"
                    "  lifetime: AUTOMATIC\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Task"]);
    ASSERT_TRUE(yaml["Task"]["filename"].as<std::string>() == "task.v");
    ASSERT_TRUE(yaml["Task"]["line"].as<int>() == 4);
    ASSERT_TRUE(yaml["Task"]["args"]);
    ASSERT_TRUE(yaml["Task"]["statements"]);
    ASSERT_TRUE(yaml["Task"]["name"].as<std::string>() == "mynbiqpmzj");
    ASSERT_TRUE(yaml["Task"]["lifetime"].as<AST::Task::LifetimeEnum>() ==
                AST::Task::LifetimeEnum::AUTOMATIC);
}

TEST(YAMLImporter, TaskCall)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.TaskCall.log");
    Logger::add_stderr_sink();
    std::string str("TaskCall:\n"
                    "  filename: taskcall.v\n"
                    "  line: 8\n"
                    "  args:\n"
                    "  name: mynbiqpmzj\n"
                    "  package: plsgqejeyd\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["TaskCall"]);
    ASSERT_TRUE(yaml["TaskCall"]["filename"].as<std::string>() == "taskcall.v");
    ASSERT_TRUE(yaml["TaskCall"]["line"].as<int>() == 8);
    ASSERT_TRUE(yaml["TaskCall"]["args"]);
    ASSERT_TRUE(yaml["TaskCall"]["name"].as<std::string>() == "mynbiqpmzj");
    ASSERT_TRUE(yaml["TaskCall"]["package"].as<std::string>() == "plsgqejeyd");
}

TEST(YAMLImporter, GenerateStatement)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.GenerateStatement.log");
    Logger::add_stderr_sink();
    std::string str("GenerateStatement:\n"
                    "  filename: generatestatement.v\n"
                    "  line: 17\n"
                    "  items:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["GenerateStatement"]);
    ASSERT_TRUE(yaml["GenerateStatement"]["filename"].as<std::string>() == "generatestatement.v");
    ASSERT_TRUE(yaml["GenerateStatement"]["line"].as<int>() == 17);
    ASSERT_TRUE(yaml["GenerateStatement"]["items"]);
}

TEST(YAMLImporter, SystemCall)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.SystemCall.log");
    Logger::add_stderr_sink();
    std::string str("SystemCall:\n"
                    "  filename: systemcall.v\n"
                    "  line: 10\n"
                    "  args:\n"
                    "  syscall: mynbiqpmzj\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["SystemCall"]);
    ASSERT_TRUE(yaml["SystemCall"]["filename"].as<std::string>() == "systemcall.v");
    ASSERT_TRUE(yaml["SystemCall"]["line"].as<int>() == 10);
    ASSERT_TRUE(yaml["SystemCall"]["args"]);
    ASSERT_TRUE(yaml["SystemCall"]["syscall"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLImporter, IdentifierScopeLabel)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.IdentifierScopeLabel.log");
    Logger::add_stderr_sink();
    std::string str("IdentifierScopeLabel:\n"
                    "  filename: identifierscopelabel.v\n"
                    "  line: 20\n"
                    "  loop:\n"
                    "  scope: mynbiqpmzj\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["IdentifierScopeLabel"]);
    ASSERT_TRUE(yaml["IdentifierScopeLabel"]["filename"].as<std::string>() ==
                "identifierscopelabel.v");
    ASSERT_TRUE(yaml["IdentifierScopeLabel"]["line"].as<int>() == 20);
    ASSERT_TRUE(yaml["IdentifierScopeLabel"]["loop"]);
    ASSERT_TRUE(yaml["IdentifierScopeLabel"]["scope"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLImporter, IdentifierScope)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.IdentifierScope.log");
    Logger::add_stderr_sink();
    std::string str("IdentifierScope:\n"
                    "  filename: identifierscope.v\n"
                    "  line: 15\n"
                    "  labellist:\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["IdentifierScope"]);
    ASSERT_TRUE(yaml["IdentifierScope"]["filename"].as<std::string>() == "identifierscope.v");
    ASSERT_TRUE(yaml["IdentifierScope"]["line"].as<int>() == 15);
    ASSERT_TRUE(yaml["IdentifierScope"]["labellist"]);
}

TEST(YAMLImporter, Disable)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.Disable.log");
    Logger::add_stderr_sink();
    std::string str("Disable:\n"
                    "  filename: disable.v\n"
                    "  line: 7\n"
                    "  dest: mynbiqpmzj\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["Disable"]);
    ASSERT_TRUE(yaml["Disable"]["filename"].as<std::string>() == "disable.v");
    ASSERT_TRUE(yaml["Disable"]["line"].as<int>() == 7);
    ASSERT_TRUE(yaml["Disable"]["dest"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLImporter, ParallelBlock)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.ParallelBlock.log");
    Logger::add_stderr_sink();
    std::string str("ParallelBlock:\n"
                    "  filename: parallelblock.v\n"
                    "  line: 13\n"
                    "  statements:\n"
                    "  scope: mynbiqpmzj\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["ParallelBlock"]);
    ASSERT_TRUE(yaml["ParallelBlock"]["filename"].as<std::string>() == "parallelblock.v");
    ASSERT_TRUE(yaml["ParallelBlock"]["line"].as<int>() == 13);
    ASSERT_TRUE(yaml["ParallelBlock"]["statements"]);
    ASSERT_TRUE(yaml["ParallelBlock"]["scope"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLImporter, SingleStatement)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLImporter.SingleStatement.log");
    Logger::add_stderr_sink();
    std::string str("SingleStatement:\n"
                    "  filename: singlestatement.v\n"
                    "  line: 15\n"
                    "  statement:\n"
                    "  delay:\n"
                    "  scope: mynbiqpmzj\n");

    AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
    YAML::Node yaml = Generators::YAMLGenerator().render(ast);

    ASSERT_TRUE(yaml["SingleStatement"]);
    ASSERT_TRUE(yaml["SingleStatement"]["filename"].as<std::string>() == "singlestatement.v");
    ASSERT_TRUE(yaml["SingleStatement"]["line"].as<int>() == 15);
    ASSERT_TRUE(yaml["SingleStatement"]["statement"]);
    ASSERT_TRUE(yaml["SingleStatement"]["delay"]);
    ASSERT_TRUE(yaml["SingleStatement"]["scope"].as<std::string>() == "mynbiqpmzj");
}
