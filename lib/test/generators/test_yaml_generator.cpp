// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <gtest/gtest.h>
#include <yaml-cpp/yaml.h>
#include <veriparse/generators/yaml_generator.hpp>
#include <veriparse/importers/yaml_specializations.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

using namespace Veriparse;

TEST(YAMLGenerator, Source)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Source.log");
    Logger::add_stderr_sink();

    AST::Description::Ptr c_description(new AST::Description);

    AST::Source::Ptr m_source(new AST::Source(c_description, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_source);

    ASSERT_TRUE(yaml["Source"]);
    ASSERT_TRUE(yaml["Source"]["description"]);
}

TEST(YAMLGenerator, Description)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Description.log");
    Logger::add_stderr_sink();

    AST::Node::ListPtr c_definitions(new AST::Node::List);

    AST::Description::Ptr m_description(new AST::Description(c_definitions, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_description);

    ASSERT_TRUE(yaml["Description"]);
    ASSERT_TRUE(yaml["Description"]["definitions"]);
}

TEST(YAMLGenerator, Pragmalist)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Pragmalist.log");
    Logger::add_stderr_sink();

    AST::Pragma::ListPtr c_pragmas(new AST::Pragma::List);
    AST::Node::ListPtr c_statements(new AST::Node::List);

    AST::Pragmalist::Ptr m_pragmalist(new AST::Pragmalist(c_pragmas, c_statements, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_pragmalist);

    ASSERT_TRUE(yaml["Pragmalist"]);
    ASSERT_TRUE(yaml["Pragmalist"]["pragmas"]);
    ASSERT_TRUE(yaml["Pragmalist"]["statements"]);
}

TEST(YAMLGenerator, Pragma)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Pragma.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_expression(new AST::Node);
    std::string p_name = "mynbiqpmzj";

    AST::Pragma::Ptr m_pragma(new AST::Pragma(c_expression, p_name, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_pragma);

    ASSERT_TRUE(yaml["Pragma"]);
    ASSERT_TRUE(yaml["Pragma"]["expression"]);
    ASSERT_TRUE(yaml["Pragma"]["name"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLGenerator, Module)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Module.log");
    Logger::add_stderr_sink();

    AST::Declaration::ListPtr c_params(new AST::Declaration::List);
    AST::Port::ListPtr c_ports(new AST::Port::List);
    AST::Node::ListPtr c_items(new AST::Node::List);
    std::string p_name = "mynbiqpmzj";
    AST::Module::LifetimeEnum p_lifetime = AST::Module::LifetimeEnum::AUTOMATIC;
    AST::Module::Default_nettypeEnum p_default_nettype = AST::Module::Default_nettypeEnum::TRIOR;

    AST::Module::Ptr m_module(new AST::Module(c_params, c_ports, c_items, p_name, p_lifetime,
                                              p_default_nettype, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_module);

    ASSERT_TRUE(yaml["Module"]);
    ASSERT_TRUE(yaml["Module"]["params"]);
    ASSERT_TRUE(yaml["Module"]["ports"]);
    ASSERT_TRUE(yaml["Module"]["items"]);
    ASSERT_TRUE(yaml["Module"]["name"].as<std::string>() == "mynbiqpmzj");
    ASSERT_TRUE(yaml["Module"]["lifetime"].as<AST::Module::LifetimeEnum>() ==
                AST::Module::LifetimeEnum::AUTOMATIC);
    ASSERT_TRUE(yaml["Module"]["default_nettype"].as<AST::Module::Default_nettypeEnum>() ==
                AST::Module::Default_nettypeEnum::TRIOR);
}

TEST(YAMLGenerator, Port)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Port.log");
    Logger::add_stderr_sink();

    AST::Declaration::Ptr c_decl(new AST::Declaration);
    AST::Node::Ptr c_expr(new AST::Node);
    std::string p_name = "mynbiqpmzj";
    AST::Port::DirectionEnum p_direction = AST::Port::DirectionEnum::INOUT;

    AST::Port::Ptr m_port(new AST::Port(c_decl, c_expr, p_name, p_direction, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_port);

    ASSERT_TRUE(yaml["Port"]);
    ASSERT_TRUE(yaml["Port"]["decl"]);
    ASSERT_TRUE(yaml["Port"]["expr"]);
    ASSERT_TRUE(yaml["Port"]["name"].as<std::string>() == "mynbiqpmzj");
    ASSERT_TRUE(yaml["Port"]["direction"].as<AST::Port::DirectionEnum>() ==
                AST::Port::DirectionEnum::INOUT);
}

TEST(YAMLGenerator, Package)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Package.log");
    Logger::add_stderr_sink();

    AST::Node::ListPtr c_items(new AST::Node::List);
    std::string p_name = "mynbiqpmzj";
    AST::Package::LifetimeEnum p_lifetime = AST::Package::LifetimeEnum::AUTOMATIC;

    AST::Package::Ptr m_package(new AST::Package(c_items, p_name, p_lifetime, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_package);

    ASSERT_TRUE(yaml["Package"]);
    ASSERT_TRUE(yaml["Package"]["items"]);
    ASSERT_TRUE(yaml["Package"]["name"].as<std::string>() == "mynbiqpmzj");
    ASSERT_TRUE(yaml["Package"]["lifetime"].as<AST::Package::LifetimeEnum>() ==
                AST::Package::LifetimeEnum::AUTOMATIC);
}

TEST(YAMLGenerator, Import)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Import.log");
    Logger::add_stderr_sink();

    std::string p_package = "mynbiqpmzj";
    std::string p_symbol = "plsgqejeyd";

    AST::Import::Ptr m_import(new AST::Import(p_package, p_symbol, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_import);

    ASSERT_TRUE(yaml["Import"]);
    ASSERT_TRUE(yaml["Import"]["package"].as<std::string>() == "mynbiqpmzj");
    ASSERT_TRUE(yaml["Import"]["symbol"].as<std::string>() == "plsgqejeyd");
}

TEST(YAMLGenerator, Identifier)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Identifier.log");
    Logger::add_stderr_sink();

    AST::ScopeName::ListPtr c_scope(new AST::ScopeName::List);
    AST::HierName::Ptr c_hier(new AST::HierName);
    std::string p_name = "mynbiqpmzj";

    AST::Identifier::Ptr m_identifier(new AST::Identifier(c_scope, c_hier, p_name, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_identifier);

    ASSERT_TRUE(yaml["Identifier"]);
    ASSERT_TRUE(yaml["Identifier"]["scope"]);
    ASSERT_TRUE(yaml["Identifier"]["hier"]);
    ASSERT_TRUE(yaml["Identifier"]["name"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLGenerator, Call)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Call.log");
    Logger::add_stderr_sink();

    AST::Node::ListPtr c_args(new AST::Node::List);
    AST::ScopeName::ListPtr c_scope(new AST::ScopeName::List);
    AST::HierName::Ptr c_hier(new AST::HierName);
    std::string p_name = "mynbiqpmzj";

    AST::Call::Ptr m_call(new AST::Call(c_args, c_scope, c_hier, p_name, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_call);

    ASSERT_TRUE(yaml["Call"]);
    ASSERT_TRUE(yaml["Call"]["args"]);
    ASSERT_TRUE(yaml["Call"]["scope"]);
    ASSERT_TRUE(yaml["Call"]["hier"]);
    ASSERT_TRUE(yaml["Call"]["name"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLGenerator, Constant)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Constant.log");
    Logger::add_stderr_sink();

    AST::Constant::Ptr m_constant(new AST::Constant("filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_constant);

    ASSERT_TRUE(yaml["Constant"]);
}

TEST(YAMLGenerator, StringConst)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.StringConst.log");
    Logger::add_stderr_sink();

    std::string p_value = "mynbiqpmzj";

    AST::StringConst::Ptr m_stringconst(new AST::StringConst(p_value, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_stringconst);

    ASSERT_TRUE(yaml["StringConst"]);
    ASSERT_TRUE(yaml["StringConst"]["value"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLGenerator, IntConst)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.IntConst.log");
    Logger::add_stderr_sink();

    std::string p_value = "mynbiqpmzj";

    AST::IntConst::Ptr m_intconst(new AST::IntConst(p_value, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_intconst);

    ASSERT_TRUE(yaml["IntConst"]);
    ASSERT_TRUE(yaml["IntConst"]["value"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLGenerator, IntConstN)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.IntConstN.log");
    Logger::add_stderr_sink();

    int p_base = 98;
    int p_size = 107;
    bool p_sign = true;
    mpz_class p_value = 66;

    AST::IntConstN::Ptr m_intconstn(
        new AST::IntConstN(p_base, p_size, p_sign, p_value, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_intconstn);

    ASSERT_TRUE(yaml["IntConstN"]);
    ASSERT_TRUE(yaml["IntConstN"]["base"].as<int>() == 98);
    ASSERT_TRUE(yaml["IntConstN"]["size"].as<int>() == 107);
    ASSERT_TRUE(yaml["IntConstN"]["sign"].as<bool>() == true);
    ASSERT_TRUE(yaml["IntConstN"]["value"].as<mpz_class>() == 66);
}

TEST(YAMLGenerator, FloatConst)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.FloatConst.log");
    Logger::add_stderr_sink();

    double p_value = 98;

    AST::FloatConst::Ptr m_floatconst(new AST::FloatConst(p_value, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_floatconst);

    ASSERT_TRUE(yaml["FloatConst"]);
    ASSERT_TRUE(yaml["FloatConst"]["value"].as<double>() == 98);
}

TEST(YAMLGenerator, DataType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.DataType.log");
    Logger::add_stderr_sink();

    AST::Dimension::ListPtr c_packed_dims(new AST::Dimension::List);
    AST::DataType::SigningEnum p_signing = AST::DataType::SigningEnum::SIGNED;

    AST::DataType::Ptr m_datatype(new AST::DataType(c_packed_dims, p_signing, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_datatype);

    ASSERT_TRUE(yaml["DataType"]);
    ASSERT_TRUE(yaml["DataType"]["packed_dims"]);
    ASSERT_TRUE(yaml["DataType"]["signing"].as<AST::DataType::SigningEnum>() ==
                AST::DataType::SigningEnum::SIGNED);
}

TEST(YAMLGenerator, LogicType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.LogicType.log");
    Logger::add_stderr_sink();

    AST::Dimension::ListPtr c_packed_dims(new AST::Dimension::List);
    AST::LogicType::DataType::SigningEnum p_signing = AST::LogicType::DataType::SigningEnum::SIGNED;

    AST::LogicType::Ptr m_logictype(new AST::LogicType(c_packed_dims, p_signing, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_logictype);

    ASSERT_TRUE(yaml["LogicType"]);
    ASSERT_TRUE(yaml["LogicType"]["packed_dims"]);
    ASSERT_TRUE(yaml["LogicType"]["signing"].as<AST::LogicType::DataType::SigningEnum>() ==
                AST::LogicType::DataType::SigningEnum::SIGNED);
}

TEST(YAMLGenerator, RegType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.RegType.log");
    Logger::add_stderr_sink();

    AST::Dimension::ListPtr c_packed_dims(new AST::Dimension::List);
    AST::RegType::DataType::SigningEnum p_signing = AST::RegType::DataType::SigningEnum::SIGNED;

    AST::RegType::Ptr m_regtype(new AST::RegType(c_packed_dims, p_signing, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_regtype);

    ASSERT_TRUE(yaml["RegType"]);
    ASSERT_TRUE(yaml["RegType"]["packed_dims"]);
    ASSERT_TRUE(yaml["RegType"]["signing"].as<AST::RegType::DataType::SigningEnum>() ==
                AST::RegType::DataType::SigningEnum::SIGNED);
}

TEST(YAMLGenerator, BitType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.BitType.log");
    Logger::add_stderr_sink();

    AST::Dimension::ListPtr c_packed_dims(new AST::Dimension::List);
    AST::BitType::DataType::SigningEnum p_signing = AST::BitType::DataType::SigningEnum::SIGNED;

    AST::BitType::Ptr m_bittype(new AST::BitType(c_packed_dims, p_signing, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_bittype);

    ASSERT_TRUE(yaml["BitType"]);
    ASSERT_TRUE(yaml["BitType"]["packed_dims"]);
    ASSERT_TRUE(yaml["BitType"]["signing"].as<AST::BitType::DataType::SigningEnum>() ==
                AST::BitType::DataType::SigningEnum::SIGNED);
}

TEST(YAMLGenerator, ByteType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.ByteType.log");
    Logger::add_stderr_sink();

    AST::Dimension::ListPtr c_packed_dims(new AST::Dimension::List);
    AST::ByteType::DataType::SigningEnum p_signing = AST::ByteType::DataType::SigningEnum::SIGNED;

    AST::ByteType::Ptr m_bytetype(new AST::ByteType(c_packed_dims, p_signing, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_bytetype);

    ASSERT_TRUE(yaml["ByteType"]);
    ASSERT_TRUE(yaml["ByteType"]["packed_dims"]);
    ASSERT_TRUE(yaml["ByteType"]["signing"].as<AST::ByteType::DataType::SigningEnum>() ==
                AST::ByteType::DataType::SigningEnum::SIGNED);
}

TEST(YAMLGenerator, ShortintType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.ShortintType.log");
    Logger::add_stderr_sink();

    AST::Dimension::ListPtr c_packed_dims(new AST::Dimension::List);
    AST::ShortintType::DataType::SigningEnum p_signing =
        AST::ShortintType::DataType::SigningEnum::SIGNED;

    AST::ShortintType::Ptr m_shortinttype(
        new AST::ShortintType(c_packed_dims, p_signing, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_shortinttype);

    ASSERT_TRUE(yaml["ShortintType"]);
    ASSERT_TRUE(yaml["ShortintType"]["packed_dims"]);
    ASSERT_TRUE(yaml["ShortintType"]["signing"].as<AST::ShortintType::DataType::SigningEnum>() ==
                AST::ShortintType::DataType::SigningEnum::SIGNED);
}

TEST(YAMLGenerator, IntType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.IntType.log");
    Logger::add_stderr_sink();

    AST::Dimension::ListPtr c_packed_dims(new AST::Dimension::List);
    AST::IntType::DataType::SigningEnum p_signing = AST::IntType::DataType::SigningEnum::SIGNED;

    AST::IntType::Ptr m_inttype(new AST::IntType(c_packed_dims, p_signing, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_inttype);

    ASSERT_TRUE(yaml["IntType"]);
    ASSERT_TRUE(yaml["IntType"]["packed_dims"]);
    ASSERT_TRUE(yaml["IntType"]["signing"].as<AST::IntType::DataType::SigningEnum>() ==
                AST::IntType::DataType::SigningEnum::SIGNED);
}

TEST(YAMLGenerator, LongintType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.LongintType.log");
    Logger::add_stderr_sink();

    AST::Dimension::ListPtr c_packed_dims(new AST::Dimension::List);
    AST::LongintType::DataType::SigningEnum p_signing =
        AST::LongintType::DataType::SigningEnum::SIGNED;

    AST::LongintType::Ptr m_longinttype(
        new AST::LongintType(c_packed_dims, p_signing, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_longinttype);

    ASSERT_TRUE(yaml["LongintType"]);
    ASSERT_TRUE(yaml["LongintType"]["packed_dims"]);
    ASSERT_TRUE(yaml["LongintType"]["signing"].as<AST::LongintType::DataType::SigningEnum>() ==
                AST::LongintType::DataType::SigningEnum::SIGNED);
}

TEST(YAMLGenerator, IntegerType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.IntegerType.log");
    Logger::add_stderr_sink();

    AST::Dimension::ListPtr c_packed_dims(new AST::Dimension::List);
    AST::IntegerType::DataType::SigningEnum p_signing =
        AST::IntegerType::DataType::SigningEnum::SIGNED;

    AST::IntegerType::Ptr m_integertype(
        new AST::IntegerType(c_packed_dims, p_signing, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_integertype);

    ASSERT_TRUE(yaml["IntegerType"]);
    ASSERT_TRUE(yaml["IntegerType"]["packed_dims"]);
    ASSERT_TRUE(yaml["IntegerType"]["signing"].as<AST::IntegerType::DataType::SigningEnum>() ==
                AST::IntegerType::DataType::SigningEnum::SIGNED);
}

TEST(YAMLGenerator, TimeType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.TimeType.log");
    Logger::add_stderr_sink();

    AST::Dimension::ListPtr c_packed_dims(new AST::Dimension::List);
    AST::TimeType::DataType::SigningEnum p_signing = AST::TimeType::DataType::SigningEnum::SIGNED;

    AST::TimeType::Ptr m_timetype(new AST::TimeType(c_packed_dims, p_signing, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_timetype);

    ASSERT_TRUE(yaml["TimeType"]);
    ASSERT_TRUE(yaml["TimeType"]["packed_dims"]);
    ASSERT_TRUE(yaml["TimeType"]["signing"].as<AST::TimeType::DataType::SigningEnum>() ==
                AST::TimeType::DataType::SigningEnum::SIGNED);
}

TEST(YAMLGenerator, RealType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.RealType.log");
    Logger::add_stderr_sink();

    AST::Dimension::ListPtr c_packed_dims(new AST::Dimension::List);
    AST::RealType::DataType::SigningEnum p_signing = AST::RealType::DataType::SigningEnum::SIGNED;

    AST::RealType::Ptr m_realtype(new AST::RealType(c_packed_dims, p_signing, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_realtype);

    ASSERT_TRUE(yaml["RealType"]);
    ASSERT_TRUE(yaml["RealType"]["packed_dims"]);
    ASSERT_TRUE(yaml["RealType"]["signing"].as<AST::RealType::DataType::SigningEnum>() ==
                AST::RealType::DataType::SigningEnum::SIGNED);
}

TEST(YAMLGenerator, ShortrealType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.ShortrealType.log");
    Logger::add_stderr_sink();

    AST::Dimension::ListPtr c_packed_dims(new AST::Dimension::List);
    AST::ShortrealType::DataType::SigningEnum p_signing =
        AST::ShortrealType::DataType::SigningEnum::SIGNED;

    AST::ShortrealType::Ptr m_shortrealtype(
        new AST::ShortrealType(c_packed_dims, p_signing, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_shortrealtype);

    ASSERT_TRUE(yaml["ShortrealType"]);
    ASSERT_TRUE(yaml["ShortrealType"]["packed_dims"]);
    ASSERT_TRUE(yaml["ShortrealType"]["signing"].as<AST::ShortrealType::DataType::SigningEnum>() ==
                AST::ShortrealType::DataType::SigningEnum::SIGNED);
}

TEST(YAMLGenerator, RealtimeType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.RealtimeType.log");
    Logger::add_stderr_sink();

    AST::Dimension::ListPtr c_packed_dims(new AST::Dimension::List);
    AST::RealtimeType::DataType::SigningEnum p_signing =
        AST::RealtimeType::DataType::SigningEnum::SIGNED;

    AST::RealtimeType::Ptr m_realtimetype(
        new AST::RealtimeType(c_packed_dims, p_signing, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_realtimetype);

    ASSERT_TRUE(yaml["RealtimeType"]);
    ASSERT_TRUE(yaml["RealtimeType"]["packed_dims"]);
    ASSERT_TRUE(yaml["RealtimeType"]["signing"].as<AST::RealtimeType::DataType::SigningEnum>() ==
                AST::RealtimeType::DataType::SigningEnum::SIGNED);
}

TEST(YAMLGenerator, StringType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.StringType.log");
    Logger::add_stderr_sink();

    AST::Dimension::ListPtr c_packed_dims(new AST::Dimension::List);
    AST::StringType::DataType::SigningEnum p_signing =
        AST::StringType::DataType::SigningEnum::SIGNED;

    AST::StringType::Ptr m_stringtype(new AST::StringType(c_packed_dims, p_signing, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_stringtype);

    ASSERT_TRUE(yaml["StringType"]);
    ASSERT_TRUE(yaml["StringType"]["packed_dims"]);
    ASSERT_TRUE(yaml["StringType"]["signing"].as<AST::StringType::DataType::SigningEnum>() ==
                AST::StringType::DataType::SigningEnum::SIGNED);
}

TEST(YAMLGenerator, ChandleType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.ChandleType.log");
    Logger::add_stderr_sink();

    AST::Dimension::ListPtr c_packed_dims(new AST::Dimension::List);
    AST::ChandleType::DataType::SigningEnum p_signing =
        AST::ChandleType::DataType::SigningEnum::SIGNED;

    AST::ChandleType::Ptr m_chandletype(
        new AST::ChandleType(c_packed_dims, p_signing, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_chandletype);

    ASSERT_TRUE(yaml["ChandleType"]);
    ASSERT_TRUE(yaml["ChandleType"]["packed_dims"]);
    ASSERT_TRUE(yaml["ChandleType"]["signing"].as<AST::ChandleType::DataType::SigningEnum>() ==
                AST::ChandleType::DataType::SigningEnum::SIGNED);
}

TEST(YAMLGenerator, EventType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.EventType.log");
    Logger::add_stderr_sink();

    AST::Dimension::ListPtr c_packed_dims(new AST::Dimension::List);
    AST::EventType::DataType::SigningEnum p_signing = AST::EventType::DataType::SigningEnum::SIGNED;

    AST::EventType::Ptr m_eventtype(new AST::EventType(c_packed_dims, p_signing, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_eventtype);

    ASSERT_TRUE(yaml["EventType"]);
    ASSERT_TRUE(yaml["EventType"]["packed_dims"]);
    ASSERT_TRUE(yaml["EventType"]["signing"].as<AST::EventType::DataType::SigningEnum>() ==
                AST::EventType::DataType::SigningEnum::SIGNED);
}

TEST(YAMLGenerator, ImplicitType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.ImplicitType.log");
    Logger::add_stderr_sink();

    AST::Dimension::ListPtr c_packed_dims(new AST::Dimension::List);
    AST::ImplicitType::DataType::SigningEnum p_signing =
        AST::ImplicitType::DataType::SigningEnum::SIGNED;

    AST::ImplicitType::Ptr m_implicittype(
        new AST::ImplicitType(c_packed_dims, p_signing, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_implicittype);

    ASSERT_TRUE(yaml["ImplicitType"]);
    ASSERT_TRUE(yaml["ImplicitType"]["packed_dims"]);
    ASSERT_TRUE(yaml["ImplicitType"]["signing"].as<AST::ImplicitType::DataType::SigningEnum>() ==
                AST::ImplicitType::DataType::SigningEnum::SIGNED);
}

TEST(YAMLGenerator, VoidType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.VoidType.log");
    Logger::add_stderr_sink();

    AST::Dimension::ListPtr c_packed_dims(new AST::Dimension::List);
    AST::VoidType::DataType::SigningEnum p_signing = AST::VoidType::DataType::SigningEnum::SIGNED;

    AST::VoidType::Ptr m_voidtype(new AST::VoidType(c_packed_dims, p_signing, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_voidtype);

    ASSERT_TRUE(yaml["VoidType"]);
    ASSERT_TRUE(yaml["VoidType"]["packed_dims"]);
    ASSERT_TRUE(yaml["VoidType"]["signing"].as<AST::VoidType::DataType::SigningEnum>() ==
                AST::VoidType::DataType::SigningEnum::SIGNED);
}

TEST(YAMLGenerator, NamedType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.NamedType.log");
    Logger::add_stderr_sink();

    AST::ScopeName::ListPtr c_scope(new AST::ScopeName::List);
    AST::Dimension::ListPtr c_packed_dims(new AST::Dimension::List);
    std::string p_name = "mynbiqpmzj";
    AST::NamedType::DataType::SigningEnum p_signing = AST::NamedType::DataType::SigningEnum::SIGNED;

    AST::NamedType::Ptr m_namedtype(
        new AST::NamedType(c_scope, c_packed_dims, p_name, p_signing, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_namedtype);

    ASSERT_TRUE(yaml["NamedType"]);
    ASSERT_TRUE(yaml["NamedType"]["scope"]);
    ASSERT_TRUE(yaml["NamedType"]["packed_dims"]);
    ASSERT_TRUE(yaml["NamedType"]["name"].as<std::string>() == "mynbiqpmzj");
    ASSERT_TRUE(yaml["NamedType"]["signing"].as<AST::NamedType::DataType::SigningEnum>() ==
                AST::NamedType::DataType::SigningEnum::SIGNED);
}

TEST(YAMLGenerator, ScopeName)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.ScopeName.log");
    Logger::add_stderr_sink();

    AST::ParamArg::ListPtr c_params(new AST::ParamArg::List);
    std::string p_name = "mynbiqpmzj";

    AST::ScopeName::Ptr m_scopename(new AST::ScopeName(c_params, p_name, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_scopename);

    ASSERT_TRUE(yaml["ScopeName"]);
    ASSERT_TRUE(yaml["ScopeName"]["params"]);
    ASSERT_TRUE(yaml["ScopeName"]["name"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLGenerator, StructType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.StructType.log");
    Logger::add_stderr_sink();

    AST::Member::ListPtr c_members(new AST::Member::List);
    AST::Dimension::ListPtr c_packed_dims(new AST::Dimension::List);
    bool p_is_packed = false;
    AST::StructType::DataType::SigningEnum p_signing =
        AST::StructType::DataType::SigningEnum::SIGNED;

    AST::StructType::Ptr m_structtype(
        new AST::StructType(c_members, c_packed_dims, p_is_packed, p_signing, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_structtype);

    ASSERT_TRUE(yaml["StructType"]);
    ASSERT_TRUE(yaml["StructType"]["members"]);
    ASSERT_TRUE(yaml["StructType"]["packed_dims"]);
    ASSERT_TRUE(yaml["StructType"]["is_packed"].as<bool>() == false);
    ASSERT_TRUE(yaml["StructType"]["signing"].as<AST::StructType::DataType::SigningEnum>() ==
                AST::StructType::DataType::SigningEnum::SIGNED);
}

TEST(YAMLGenerator, UnionType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.UnionType.log");
    Logger::add_stderr_sink();

    AST::Member::ListPtr c_members(new AST::Member::List);
    AST::Dimension::ListPtr c_packed_dims(new AST::Dimension::List);
    bool p_is_packed = false;
    bool p_is_tagged = false;
    AST::UnionType::DataType::SigningEnum p_signing = AST::UnionType::DataType::SigningEnum::NONE;

    AST::UnionType::Ptr m_uniontype(new AST::UnionType(c_members, c_packed_dims, p_is_packed,
                                                       p_is_tagged, p_signing, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_uniontype);

    ASSERT_TRUE(yaml["UnionType"]);
    ASSERT_TRUE(yaml["UnionType"]["members"]);
    ASSERT_TRUE(yaml["UnionType"]["packed_dims"]);
    ASSERT_TRUE(yaml["UnionType"]["is_packed"].as<bool>() == false);
    ASSERT_TRUE(yaml["UnionType"]["is_tagged"].as<bool>() == false);
    ASSERT_TRUE(yaml["UnionType"]["signing"].as<AST::UnionType::DataType::SigningEnum>() ==
                AST::UnionType::DataType::SigningEnum::NONE);
}

TEST(YAMLGenerator, EnumType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.EnumType.log");
    Logger::add_stderr_sink();

    AST::DataType::Ptr c_base(new AST::DataType);
    AST::EnumItem::ListPtr c_items(new AST::EnumItem::List);
    AST::Dimension::ListPtr c_packed_dims(new AST::Dimension::List);
    AST::EnumType::DataType::SigningEnum p_signing = AST::EnumType::DataType::SigningEnum::SIGNED;

    AST::EnumType::Ptr m_enumtype(
        new AST::EnumType(c_base, c_items, c_packed_dims, p_signing, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_enumtype);

    ASSERT_TRUE(yaml["EnumType"]);
    ASSERT_TRUE(yaml["EnumType"]["base"]);
    ASSERT_TRUE(yaml["EnumType"]["items"]);
    ASSERT_TRUE(yaml["EnumType"]["packed_dims"]);
    ASSERT_TRUE(yaml["EnumType"]["signing"].as<AST::EnumType::DataType::SigningEnum>() ==
                AST::EnumType::DataType::SigningEnum::SIGNED);
}

TEST(YAMLGenerator, EnumItem)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.EnumItem.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_value(new AST::Node);
    AST::Dimension::Ptr c_range(new AST::Dimension);
    std::string p_name = "mynbiqpmzj";

    AST::EnumItem::Ptr m_enumitem(new AST::EnumItem(c_value, c_range, p_name, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_enumitem);

    ASSERT_TRUE(yaml["EnumItem"]);
    ASSERT_TRUE(yaml["EnumItem"]["value"]);
    ASSERT_TRUE(yaml["EnumItem"]["range"]);
    ASSERT_TRUE(yaml["EnumItem"]["name"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLGenerator, TypeOpExpr)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.TypeOpExpr.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_expr(new AST::Node);
    AST::Dimension::ListPtr c_packed_dims(new AST::Dimension::List);
    AST::TypeOpExpr::DataType::SigningEnum p_signing =
        AST::TypeOpExpr::DataType::SigningEnum::SIGNED;

    AST::TypeOpExpr::Ptr m_typeopexpr(
        new AST::TypeOpExpr(c_expr, c_packed_dims, p_signing, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_typeopexpr);

    ASSERT_TRUE(yaml["TypeOpExpr"]);
    ASSERT_TRUE(yaml["TypeOpExpr"]["expr"]);
    ASSERT_TRUE(yaml["TypeOpExpr"]["packed_dims"]);
    ASSERT_TRUE(yaml["TypeOpExpr"]["signing"].as<AST::TypeOpExpr::DataType::SigningEnum>() ==
                AST::TypeOpExpr::DataType::SigningEnum::SIGNED);
}

TEST(YAMLGenerator, TypeOpType)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.TypeOpType.log");
    Logger::add_stderr_sink();

    AST::DataType::Ptr c_type(new AST::DataType);
    AST::Dimension::ListPtr c_packed_dims(new AST::Dimension::List);
    AST::TypeOpType::DataType::SigningEnum p_signing =
        AST::TypeOpType::DataType::SigningEnum::SIGNED;

    AST::TypeOpType::Ptr m_typeoptype(
        new AST::TypeOpType(c_type, c_packed_dims, p_signing, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_typeoptype);

    ASSERT_TRUE(yaml["TypeOpType"]);
    ASSERT_TRUE(yaml["TypeOpType"]["type"]);
    ASSERT_TRUE(yaml["TypeOpType"]["packed_dims"]);
    ASSERT_TRUE(yaml["TypeOpType"]["signing"].as<AST::TypeOpType::DataType::SigningEnum>() ==
                AST::TypeOpType::DataType::SigningEnum::SIGNED);
}

TEST(YAMLGenerator, Dimension)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Dimension.log");
    Logger::add_stderr_sink();

    AST::Dimension::Ptr m_dimension(new AST::Dimension("filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_dimension);

    ASSERT_TRUE(yaml["Dimension"]);
}

TEST(YAMLGenerator, RangeDim)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.RangeDim.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_left(new AST::Node);
    AST::Node::Ptr c_right(new AST::Node);

    AST::RangeDim::Ptr m_rangedim(new AST::RangeDim(c_left, c_right, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_rangedim);

    ASSERT_TRUE(yaml["RangeDim"]);
    ASSERT_TRUE(yaml["RangeDim"]["left"]);
    ASSERT_TRUE(yaml["RangeDim"]["right"]);
}

TEST(YAMLGenerator, SizeDim)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.SizeDim.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_size(new AST::Node);

    AST::SizeDim::Ptr m_sizedim(new AST::SizeDim(c_size, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_sizedim);

    ASSERT_TRUE(yaml["SizeDim"]);
    ASSERT_TRUE(yaml["SizeDim"]["size"]);
}

TEST(YAMLGenerator, UnsizedDim)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.UnsizedDim.log");
    Logger::add_stderr_sink();

    AST::UnsizedDim::Ptr m_unsizeddim(new AST::UnsizedDim("filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_unsizeddim);

    ASSERT_TRUE(yaml["UnsizedDim"]);
}

TEST(YAMLGenerator, QueueDim)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.QueueDim.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_bound(new AST::Node);

    AST::QueueDim::Ptr m_queuedim(new AST::QueueDim(c_bound, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_queuedim);

    ASSERT_TRUE(yaml["QueueDim"]);
    ASSERT_TRUE(yaml["QueueDim"]["bound"]);
}

TEST(YAMLGenerator, AssocDim)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.AssocDim.log");
    Logger::add_stderr_sink();

    AST::DataType::Ptr c_index_type(new AST::DataType);

    AST::AssocDim::Ptr m_assocdim(new AST::AssocDim(c_index_type, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_assocdim);

    ASSERT_TRUE(yaml["AssocDim"]);
    ASSERT_TRUE(yaml["AssocDim"]["index_type"]);
}

TEST(YAMLGenerator, Declaration)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Declaration.log");
    Logger::add_stderr_sink();

    AST::DataType::Ptr c_type(new AST::DataType);
    std::string p_name = "mynbiqpmzj";

    AST::Declaration::Ptr m_declaration(new AST::Declaration(c_type, p_name, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_declaration);

    ASSERT_TRUE(yaml["Declaration"]);
    ASSERT_TRUE(yaml["Declaration"]["type"]);
    ASSERT_TRUE(yaml["Declaration"]["name"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLGenerator, Var)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Var.log");
    Logger::add_stderr_sink();

    AST::Dimension::ListPtr c_unpacked_dims(new AST::Dimension::List);
    AST::Rvalue::Ptr c_init(new AST::Rvalue);
    AST::DataType::Ptr c_type(new AST::DataType);
    bool p_is_var = false;
    bool p_is_const = false;
    AST::Var::LifetimeEnum p_lifetime = AST::Var::LifetimeEnum::NONE;
    std::string p_name = "iqpmzjplsg";

    AST::Var::Ptr m_var(new AST::Var(c_unpacked_dims, c_init, c_type, p_is_var, p_is_const,
                                     p_lifetime, p_name, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_var);

    ASSERT_TRUE(yaml["Var"]);
    ASSERT_TRUE(yaml["Var"]["unpacked_dims"]);
    ASSERT_TRUE(yaml["Var"]["init"]);
    ASSERT_TRUE(yaml["Var"]["type"]);
    ASSERT_TRUE(yaml["Var"]["is_var"].as<bool>() == false);
    ASSERT_TRUE(yaml["Var"]["is_const"].as<bool>() == false);
    ASSERT_TRUE(yaml["Var"]["lifetime"].as<AST::Var::LifetimeEnum>() ==
                AST::Var::LifetimeEnum::NONE);
    ASSERT_TRUE(yaml["Var"]["name"].as<std::string>() == "iqpmzjplsg");
}

TEST(YAMLGenerator, Net)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Net.log");
    Logger::add_stderr_sink();

    AST::Dimension::ListPtr c_unpacked_dims(new AST::Dimension::List);
    AST::Rvalue::Ptr c_cont_assign(new AST::Rvalue);
    AST::Strength::Ptr c_strength(new AST::Strength);
    AST::DelayStatement::Ptr c_ldelay(new AST::DelayStatement);
    AST::DelayStatement::Ptr c_rdelay(new AST::DelayStatement);
    AST::DataType::Ptr c_type(new AST::DataType);
    bool p_is_vectored = false;
    bool p_is_scalared = false;
    std::string p_name = "biqpmzjpls";

    AST::Net::Ptr m_net(new AST::Net(c_unpacked_dims, c_cont_assign, c_strength, c_ldelay, c_rdelay,
                                     c_type, p_is_vectored, p_is_scalared, p_name, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_net);

    ASSERT_TRUE(yaml["Net"]);
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

TEST(YAMLGenerator, WireNet)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.WireNet.log");
    Logger::add_stderr_sink();

    AST::Dimension::ListPtr c_unpacked_dims(new AST::Dimension::List);
    AST::Rvalue::Ptr c_cont_assign(new AST::Rvalue);
    AST::Strength::Ptr c_strength(new AST::Strength);
    AST::DelayStatement::Ptr c_ldelay(new AST::DelayStatement);
    AST::DelayStatement::Ptr c_rdelay(new AST::DelayStatement);
    AST::DataType::Ptr c_type(new AST::DataType);
    bool p_is_vectored = false;
    bool p_is_scalared = false;
    std::string p_name = "biqpmzjpls";

    AST::WireNet::Ptr m_wirenet(new AST::WireNet(c_unpacked_dims, c_cont_assign, c_strength,
                                                 c_ldelay, c_rdelay, c_type, p_is_vectored,
                                                 p_is_scalared, p_name, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_wirenet);

    ASSERT_TRUE(yaml["WireNet"]);
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

TEST(YAMLGenerator, TriNet)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.TriNet.log");
    Logger::add_stderr_sink();

    AST::Dimension::ListPtr c_unpacked_dims(new AST::Dimension::List);
    AST::Rvalue::Ptr c_cont_assign(new AST::Rvalue);
    AST::Strength::Ptr c_strength(new AST::Strength);
    AST::DelayStatement::Ptr c_ldelay(new AST::DelayStatement);
    AST::DelayStatement::Ptr c_rdelay(new AST::DelayStatement);
    AST::DataType::Ptr c_type(new AST::DataType);
    bool p_is_vectored = false;
    bool p_is_scalared = false;
    std::string p_name = "biqpmzjpls";

    AST::TriNet::Ptr m_trinet(new AST::TriNet(c_unpacked_dims, c_cont_assign, c_strength, c_ldelay,
                                              c_rdelay, c_type, p_is_vectored, p_is_scalared,
                                              p_name, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_trinet);

    ASSERT_TRUE(yaml["TriNet"]);
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

TEST(YAMLGenerator, Tri0Net)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Tri0Net.log");
    Logger::add_stderr_sink();

    AST::Dimension::ListPtr c_unpacked_dims(new AST::Dimension::List);
    AST::Rvalue::Ptr c_cont_assign(new AST::Rvalue);
    AST::Strength::Ptr c_strength(new AST::Strength);
    AST::DelayStatement::Ptr c_ldelay(new AST::DelayStatement);
    AST::DelayStatement::Ptr c_rdelay(new AST::DelayStatement);
    AST::DataType::Ptr c_type(new AST::DataType);
    bool p_is_vectored = false;
    bool p_is_scalared = false;
    std::string p_name = "biqpmzjpls";

    AST::Tri0Net::Ptr m_tri0net(new AST::Tri0Net(c_unpacked_dims, c_cont_assign, c_strength,
                                                 c_ldelay, c_rdelay, c_type, p_is_vectored,
                                                 p_is_scalared, p_name, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_tri0net);

    ASSERT_TRUE(yaml["Tri0Net"]);
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

TEST(YAMLGenerator, Tri1Net)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Tri1Net.log");
    Logger::add_stderr_sink();

    AST::Dimension::ListPtr c_unpacked_dims(new AST::Dimension::List);
    AST::Rvalue::Ptr c_cont_assign(new AST::Rvalue);
    AST::Strength::Ptr c_strength(new AST::Strength);
    AST::DelayStatement::Ptr c_ldelay(new AST::DelayStatement);
    AST::DelayStatement::Ptr c_rdelay(new AST::DelayStatement);
    AST::DataType::Ptr c_type(new AST::DataType);
    bool p_is_vectored = false;
    bool p_is_scalared = false;
    std::string p_name = "biqpmzjpls";

    AST::Tri1Net::Ptr m_tri1net(new AST::Tri1Net(c_unpacked_dims, c_cont_assign, c_strength,
                                                 c_ldelay, c_rdelay, c_type, p_is_vectored,
                                                 p_is_scalared, p_name, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_tri1net);

    ASSERT_TRUE(yaml["Tri1Net"]);
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

TEST(YAMLGenerator, TriandNet)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.TriandNet.log");
    Logger::add_stderr_sink();

    AST::Dimension::ListPtr c_unpacked_dims(new AST::Dimension::List);
    AST::Rvalue::Ptr c_cont_assign(new AST::Rvalue);
    AST::Strength::Ptr c_strength(new AST::Strength);
    AST::DelayStatement::Ptr c_ldelay(new AST::DelayStatement);
    AST::DelayStatement::Ptr c_rdelay(new AST::DelayStatement);
    AST::DataType::Ptr c_type(new AST::DataType);
    bool p_is_vectored = false;
    bool p_is_scalared = false;
    std::string p_name = "biqpmzjpls";

    AST::TriandNet::Ptr m_triandnet(new AST::TriandNet(c_unpacked_dims, c_cont_assign, c_strength,
                                                       c_ldelay, c_rdelay, c_type, p_is_vectored,
                                                       p_is_scalared, p_name, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_triandnet);

    ASSERT_TRUE(yaml["TriandNet"]);
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

TEST(YAMLGenerator, TriorNet)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.TriorNet.log");
    Logger::add_stderr_sink();

    AST::Dimension::ListPtr c_unpacked_dims(new AST::Dimension::List);
    AST::Rvalue::Ptr c_cont_assign(new AST::Rvalue);
    AST::Strength::Ptr c_strength(new AST::Strength);
    AST::DelayStatement::Ptr c_ldelay(new AST::DelayStatement);
    AST::DelayStatement::Ptr c_rdelay(new AST::DelayStatement);
    AST::DataType::Ptr c_type(new AST::DataType);
    bool p_is_vectored = false;
    bool p_is_scalared = false;
    std::string p_name = "biqpmzjpls";

    AST::TriorNet::Ptr m_triornet(new AST::TriorNet(c_unpacked_dims, c_cont_assign, c_strength,
                                                    c_ldelay, c_rdelay, c_type, p_is_vectored,
                                                    p_is_scalared, p_name, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_triornet);

    ASSERT_TRUE(yaml["TriorNet"]);
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

TEST(YAMLGenerator, TriregNet)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.TriregNet.log");
    Logger::add_stderr_sink();

    AST::Dimension::ListPtr c_unpacked_dims(new AST::Dimension::List);
    AST::Rvalue::Ptr c_cont_assign(new AST::Rvalue);
    AST::Strength::Ptr c_strength(new AST::Strength);
    AST::DelayStatement::Ptr c_ldelay(new AST::DelayStatement);
    AST::DelayStatement::Ptr c_rdelay(new AST::DelayStatement);
    AST::DataType::Ptr c_type(new AST::DataType);
    bool p_is_vectored = false;
    bool p_is_scalared = false;
    std::string p_name = "biqpmzjpls";

    AST::TriregNet::Ptr m_triregnet(new AST::TriregNet(c_unpacked_dims, c_cont_assign, c_strength,
                                                       c_ldelay, c_rdelay, c_type, p_is_vectored,
                                                       p_is_scalared, p_name, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_triregnet);

    ASSERT_TRUE(yaml["TriregNet"]);
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

TEST(YAMLGenerator, WandNet)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.WandNet.log");
    Logger::add_stderr_sink();

    AST::Dimension::ListPtr c_unpacked_dims(new AST::Dimension::List);
    AST::Rvalue::Ptr c_cont_assign(new AST::Rvalue);
    AST::Strength::Ptr c_strength(new AST::Strength);
    AST::DelayStatement::Ptr c_ldelay(new AST::DelayStatement);
    AST::DelayStatement::Ptr c_rdelay(new AST::DelayStatement);
    AST::DataType::Ptr c_type(new AST::DataType);
    bool p_is_vectored = false;
    bool p_is_scalared = false;
    std::string p_name = "biqpmzjpls";

    AST::WandNet::Ptr m_wandnet(new AST::WandNet(c_unpacked_dims, c_cont_assign, c_strength,
                                                 c_ldelay, c_rdelay, c_type, p_is_vectored,
                                                 p_is_scalared, p_name, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_wandnet);

    ASSERT_TRUE(yaml["WandNet"]);
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

TEST(YAMLGenerator, WorNet)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.WorNet.log");
    Logger::add_stderr_sink();

    AST::Dimension::ListPtr c_unpacked_dims(new AST::Dimension::List);
    AST::Rvalue::Ptr c_cont_assign(new AST::Rvalue);
    AST::Strength::Ptr c_strength(new AST::Strength);
    AST::DelayStatement::Ptr c_ldelay(new AST::DelayStatement);
    AST::DelayStatement::Ptr c_rdelay(new AST::DelayStatement);
    AST::DataType::Ptr c_type(new AST::DataType);
    bool p_is_vectored = false;
    bool p_is_scalared = false;
    std::string p_name = "biqpmzjpls";

    AST::WorNet::Ptr m_wornet(new AST::WorNet(c_unpacked_dims, c_cont_assign, c_strength, c_ldelay,
                                              c_rdelay, c_type, p_is_vectored, p_is_scalared,
                                              p_name, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_wornet);

    ASSERT_TRUE(yaml["WorNet"]);
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

TEST(YAMLGenerator, UwireNet)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.UwireNet.log");
    Logger::add_stderr_sink();

    AST::Dimension::ListPtr c_unpacked_dims(new AST::Dimension::List);
    AST::Rvalue::Ptr c_cont_assign(new AST::Rvalue);
    AST::Strength::Ptr c_strength(new AST::Strength);
    AST::DelayStatement::Ptr c_ldelay(new AST::DelayStatement);
    AST::DelayStatement::Ptr c_rdelay(new AST::DelayStatement);
    AST::DataType::Ptr c_type(new AST::DataType);
    bool p_is_vectored = false;
    bool p_is_scalared = false;
    std::string p_name = "biqpmzjpls";

    AST::UwireNet::Ptr m_uwirenet(new AST::UwireNet(c_unpacked_dims, c_cont_assign, c_strength,
                                                    c_ldelay, c_rdelay, c_type, p_is_vectored,
                                                    p_is_scalared, p_name, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_uwirenet);

    ASSERT_TRUE(yaml["UwireNet"]);
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

TEST(YAMLGenerator, Supply0Net)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Supply0Net.log");
    Logger::add_stderr_sink();

    AST::Dimension::ListPtr c_unpacked_dims(new AST::Dimension::List);
    AST::Rvalue::Ptr c_cont_assign(new AST::Rvalue);
    AST::Strength::Ptr c_strength(new AST::Strength);
    AST::DelayStatement::Ptr c_ldelay(new AST::DelayStatement);
    AST::DelayStatement::Ptr c_rdelay(new AST::DelayStatement);
    AST::DataType::Ptr c_type(new AST::DataType);
    bool p_is_vectored = false;
    bool p_is_scalared = false;
    std::string p_name = "biqpmzjpls";

    AST::Supply0Net::Ptr m_supply0net(
        new AST::Supply0Net(c_unpacked_dims, c_cont_assign, c_strength, c_ldelay, c_rdelay, c_type,
                            p_is_vectored, p_is_scalared, p_name, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_supply0net);

    ASSERT_TRUE(yaml["Supply0Net"]);
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

TEST(YAMLGenerator, Supply1Net)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Supply1Net.log");
    Logger::add_stderr_sink();

    AST::Dimension::ListPtr c_unpacked_dims(new AST::Dimension::List);
    AST::Rvalue::Ptr c_cont_assign(new AST::Rvalue);
    AST::Strength::Ptr c_strength(new AST::Strength);
    AST::DelayStatement::Ptr c_ldelay(new AST::DelayStatement);
    AST::DelayStatement::Ptr c_rdelay(new AST::DelayStatement);
    AST::DataType::Ptr c_type(new AST::DataType);
    bool p_is_vectored = false;
    bool p_is_scalared = false;
    std::string p_name = "biqpmzjpls";

    AST::Supply1Net::Ptr m_supply1net(
        new AST::Supply1Net(c_unpacked_dims, c_cont_assign, c_strength, c_ldelay, c_rdelay, c_type,
                            p_is_vectored, p_is_scalared, p_name, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_supply1net);

    ASSERT_TRUE(yaml["Supply1Net"]);
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

TEST(YAMLGenerator, InterconnectNet)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.InterconnectNet.log");
    Logger::add_stderr_sink();

    AST::Dimension::ListPtr c_unpacked_dims(new AST::Dimension::List);
    AST::Rvalue::Ptr c_cont_assign(new AST::Rvalue);
    AST::Strength::Ptr c_strength(new AST::Strength);
    AST::DelayStatement::Ptr c_ldelay(new AST::DelayStatement);
    AST::DelayStatement::Ptr c_rdelay(new AST::DelayStatement);
    AST::DataType::Ptr c_type(new AST::DataType);
    bool p_is_vectored = false;
    bool p_is_scalared = false;
    std::string p_name = "biqpmzjpls";

    AST::InterconnectNet::Ptr m_interconnectnet(
        new AST::InterconnectNet(c_unpacked_dims, c_cont_assign, c_strength, c_ldelay, c_rdelay,
                                 c_type, p_is_vectored, p_is_scalared, p_name, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_interconnectnet);

    ASSERT_TRUE(yaml["InterconnectNet"]);
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

TEST(YAMLGenerator, UserNet)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.UserNet.log");
    Logger::add_stderr_sink();

    AST::Dimension::ListPtr c_unpacked_dims(new AST::Dimension::List);
    AST::Rvalue::Ptr c_cont_assign(new AST::Rvalue);
    AST::Strength::Ptr c_strength(new AST::Strength);
    AST::DelayStatement::Ptr c_ldelay(new AST::DelayStatement);
    AST::DelayStatement::Ptr c_rdelay(new AST::DelayStatement);
    AST::DataType::Ptr c_type(new AST::DataType);
    bool p_is_vectored = false;
    bool p_is_scalared = false;
    std::string p_name = "biqpmzjpls";

    AST::UserNet::Ptr m_usernet(new AST::UserNet(c_unpacked_dims, c_cont_assign, c_strength,
                                                 c_ldelay, c_rdelay, c_type, p_is_vectored,
                                                 p_is_scalared, p_name, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_usernet);

    ASSERT_TRUE(yaml["UserNet"]);
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

TEST(YAMLGenerator, ImplicitNet)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.ImplicitNet.log");
    Logger::add_stderr_sink();

    AST::Dimension::ListPtr c_unpacked_dims(new AST::Dimension::List);
    AST::Rvalue::Ptr c_cont_assign(new AST::Rvalue);
    AST::Strength::Ptr c_strength(new AST::Strength);
    AST::DelayStatement::Ptr c_ldelay(new AST::DelayStatement);
    AST::DelayStatement::Ptr c_rdelay(new AST::DelayStatement);
    AST::DataType::Ptr c_type(new AST::DataType);
    bool p_is_vectored = false;
    bool p_is_scalared = false;
    std::string p_name = "biqpmzjpls";

    AST::ImplicitNet::Ptr m_implicitnet(
        new AST::ImplicitNet(c_unpacked_dims, c_cont_assign, c_strength, c_ldelay, c_rdelay, c_type,
                             p_is_vectored, p_is_scalared, p_name, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_implicitnet);

    ASSERT_TRUE(yaml["ImplicitNet"]);
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

TEST(YAMLGenerator, NetTypeDecl)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.NetTypeDecl.log");
    Logger::add_stderr_sink();

    AST::Identifier::Ptr c_resolver(new AST::Identifier);
    AST::DataType::Ptr c_type(new AST::DataType);
    std::string p_name = "mynbiqpmzj";

    AST::NetTypeDecl::Ptr m_nettypedecl(
        new AST::NetTypeDecl(c_resolver, c_type, p_name, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_nettypedecl);

    ASSERT_TRUE(yaml["NetTypeDecl"]);
    ASSERT_TRUE(yaml["NetTypeDecl"]["resolver"]);
    ASSERT_TRUE(yaml["NetTypeDecl"]["type"]);
    ASSERT_TRUE(yaml["NetTypeDecl"]["name"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLGenerator, Strength)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Strength.log");
    Logger::add_stderr_sink();

    AST::Strength::Ptr m_strength(new AST::Strength("filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_strength);

    ASSERT_TRUE(yaml["Strength"]);
}

TEST(YAMLGenerator, DriveStrength)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.DriveStrength.log");
    Logger::add_stderr_sink();

    AST::DriveStrength::S0Enum p_s0 = AST::DriveStrength::S0Enum::WEAK0;
    AST::DriveStrength::S1Enum p_s1 = AST::DriveStrength::S1Enum::WEAK1;

    AST::DriveStrength::Ptr m_drivestrength(new AST::DriveStrength(p_s0, p_s1, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_drivestrength);

    ASSERT_TRUE(yaml["DriveStrength"]);
    ASSERT_TRUE(yaml["DriveStrength"]["s0"].as<AST::DriveStrength::S0Enum>() ==
                AST::DriveStrength::S0Enum::WEAK0);
    ASSERT_TRUE(yaml["DriveStrength"]["s1"].as<AST::DriveStrength::S1Enum>() ==
                AST::DriveStrength::S1Enum::WEAK1);
}

TEST(YAMLGenerator, ChargeStrength)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.ChargeStrength.log");
    Logger::add_stderr_sink();

    AST::ChargeStrength::ChargeEnum p_charge = AST::ChargeStrength::ChargeEnum::MEDIUM;

    AST::ChargeStrength::Ptr m_chargestrength(new AST::ChargeStrength(p_charge, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_chargestrength);

    ASSERT_TRUE(yaml["ChargeStrength"]);
    ASSERT_TRUE(yaml["ChargeStrength"]["charge"].as<AST::ChargeStrength::ChargeEnum>() ==
                AST::ChargeStrength::ChargeEnum::MEDIUM);
}

TEST(YAMLGenerator, Param)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Param.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_value(new AST::Node);
    AST::Dimension::ListPtr c_unpacked_dims(new AST::Dimension::List);
    AST::DataType::Ptr c_type(new AST::DataType);
    bool p_is_local = false;
    std::string p_name = "ynbiqpmzjp";

    AST::Param::Ptr m_param(
        new AST::Param(c_value, c_unpacked_dims, c_type, p_is_local, p_name, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_param);

    ASSERT_TRUE(yaml["Param"]);
    ASSERT_TRUE(yaml["Param"]["value"]);
    ASSERT_TRUE(yaml["Param"]["unpacked_dims"]);
    ASSERT_TRUE(yaml["Param"]["type"]);
    ASSERT_TRUE(yaml["Param"]["is_local"].as<bool>() == false);
    ASSERT_TRUE(yaml["Param"]["name"].as<std::string>() == "ynbiqpmzjp");
}

TEST(YAMLGenerator, TypeParam)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.TypeParam.log");
    Logger::add_stderr_sink();

    AST::DataType::Ptr c_type(new AST::DataType);
    bool p_is_local = false;
    std::string p_name = "ynbiqpmzjp";

    AST::TypeParam::Ptr m_typeparam(new AST::TypeParam(c_type, p_is_local, p_name, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_typeparam);

    ASSERT_TRUE(yaml["TypeParam"]);
    ASSERT_TRUE(yaml["TypeParam"]["type"]);
    ASSERT_TRUE(yaml["TypeParam"]["is_local"].as<bool>() == false);
    ASSERT_TRUE(yaml["TypeParam"]["name"].as<std::string>() == "ynbiqpmzjp");
}

TEST(YAMLGenerator, Typedef)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Typedef.log");
    Logger::add_stderr_sink();

    AST::Dimension::ListPtr c_unpacked_dims(new AST::Dimension::List);
    AST::DataType::Ptr c_type(new AST::DataType);
    AST::Typedef::Fwd_kindEnum p_fwd_kind = AST::Typedef::Fwd_kindEnum::UNION;
    std::string p_name = "ynbiqpmzjp";

    AST::Typedef::Ptr m_typedef(
        new AST::Typedef(c_unpacked_dims, c_type, p_fwd_kind, p_name, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_typedef);

    ASSERT_TRUE(yaml["Typedef"]);
    ASSERT_TRUE(yaml["Typedef"]["unpacked_dims"]);
    ASSERT_TRUE(yaml["Typedef"]["type"]);
    ASSERT_TRUE(yaml["Typedef"]["fwd_kind"].as<AST::Typedef::Fwd_kindEnum>() ==
                AST::Typedef::Fwd_kindEnum::UNION);
    ASSERT_TRUE(yaml["Typedef"]["name"].as<std::string>() == "ynbiqpmzjp");
}

TEST(YAMLGenerator, Member)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Member.log");
    Logger::add_stderr_sink();

    AST::Dimension::ListPtr c_unpacked_dims(new AST::Dimension::List);
    AST::Rvalue::Ptr c_init(new AST::Rvalue);
    AST::DataType::Ptr c_type(new AST::DataType);
    std::string p_name = "mynbiqpmzj";

    AST::Member::Ptr m_member(
        new AST::Member(c_unpacked_dims, c_init, c_type, p_name, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_member);

    ASSERT_TRUE(yaml["Member"]);
    ASSERT_TRUE(yaml["Member"]["unpacked_dims"]);
    ASSERT_TRUE(yaml["Member"]["init"]);
    ASSERT_TRUE(yaml["Member"]["type"]);
    ASSERT_TRUE(yaml["Member"]["name"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLGenerator, Arg)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Arg.log");
    Logger::add_stderr_sink();

    AST::Dimension::ListPtr c_unpacked_dims(new AST::Dimension::List);
    AST::Node::Ptr c_default_value(new AST::Node);
    AST::DataType::Ptr c_type(new AST::DataType);
    bool p_is_var = false;
    AST::Arg::DirectionEnum p_direction = AST::Arg::DirectionEnum::REF;
    std::string p_name = "biqpmzjpls";

    AST::Arg::Ptr m_arg(new AST::Arg(c_unpacked_dims, c_default_value, c_type, p_is_var,
                                     p_direction, p_name, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_arg);

    ASSERT_TRUE(yaml["Arg"]);
    ASSERT_TRUE(yaml["Arg"]["unpacked_dims"]);
    ASSERT_TRUE(yaml["Arg"]["default_value"]);
    ASSERT_TRUE(yaml["Arg"]["type"]);
    ASSERT_TRUE(yaml["Arg"]["is_var"].as<bool>() == false);
    ASSERT_TRUE(yaml["Arg"]["direction"].as<AST::Arg::DirectionEnum>() ==
                AST::Arg::DirectionEnum::REF);
    ASSERT_TRUE(yaml["Arg"]["name"].as<std::string>() == "biqpmzjpls");
}

TEST(YAMLGenerator, Genvar)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Genvar.log");
    Logger::add_stderr_sink();

    std::string p_name = "mynbiqpmzj";

    AST::Genvar::Ptr m_genvar(new AST::Genvar(p_name, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_genvar);

    ASSERT_TRUE(yaml["Genvar"]);
    ASSERT_TRUE(yaml["Genvar"]["name"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLGenerator, Concat)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Concat.log");
    Logger::add_stderr_sink();

    AST::Node::ListPtr c_list(new AST::Node::List);

    AST::Concat::Ptr m_concat(new AST::Concat(c_list, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_concat);

    ASSERT_TRUE(yaml["Concat"]);
    ASSERT_TRUE(yaml["Concat"]["list"]);
}

TEST(YAMLGenerator, Lconcat)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Lconcat.log");
    Logger::add_stderr_sink();

    AST::Node::ListPtr c_list(new AST::Node::List);

    AST::Lconcat::Ptr m_lconcat(new AST::Lconcat(c_list, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_lconcat);

    ASSERT_TRUE(yaml["Lconcat"]);
    ASSERT_TRUE(yaml["Lconcat"]["list"]);
}

TEST(YAMLGenerator, Repeat)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Repeat.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_value(new AST::Node);
    AST::Node::Ptr c_times(new AST::Node);

    AST::Repeat::Ptr m_repeat(new AST::Repeat(c_value, c_times, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_repeat);

    ASSERT_TRUE(yaml["Repeat"]);
    ASSERT_TRUE(yaml["Repeat"]["value"]);
    ASSERT_TRUE(yaml["Repeat"]["times"]);
}

TEST(YAMLGenerator, AssignmentPattern)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.AssignmentPattern.log");
    Logger::add_stderr_sink();

    AST::Node::ListPtr c_items(new AST::Node::List);
    AST::Node::Ptr c_times(new AST::Node);

    AST::AssignmentPattern::Ptr m_assignmentpattern(
        new AST::AssignmentPattern(c_items, c_times, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_assignmentpattern);

    ASSERT_TRUE(yaml["AssignmentPattern"]);
    ASSERT_TRUE(yaml["AssignmentPattern"]["items"]);
    ASSERT_TRUE(yaml["AssignmentPattern"]["times"]);
}

TEST(YAMLGenerator, PatternItem)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.PatternItem.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_key(new AST::Node);
    AST::Node::Ptr c_value(new AST::Node);
    bool p_is_default = false;

    AST::PatternItem::Ptr m_patternitem(
        new AST::PatternItem(c_key, c_value, p_is_default, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_patternitem);

    ASSERT_TRUE(yaml["PatternItem"]);
    ASSERT_TRUE(yaml["PatternItem"]["key"]);
    ASSERT_TRUE(yaml["PatternItem"]["value"]);
    ASSERT_TRUE(yaml["PatternItem"]["is_default"].as<bool>() == false);
}

TEST(YAMLGenerator, Cast)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Cast.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_expr(new AST::Node);

    AST::Cast::Ptr m_cast(new AST::Cast(c_expr, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_cast);

    ASSERT_TRUE(yaml["Cast"]);
    ASSERT_TRUE(yaml["Cast"]["expr"]);
}

TEST(YAMLGenerator, TypeCast)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.TypeCast.log");
    Logger::add_stderr_sink();

    AST::DataType::Ptr c_target(new AST::DataType);
    AST::Node::Ptr c_expr(new AST::Node);

    AST::TypeCast::Ptr m_typecast(new AST::TypeCast(c_target, c_expr, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_typecast);

    ASSERT_TRUE(yaml["TypeCast"]);
    ASSERT_TRUE(yaml["TypeCast"]["target"]);
    ASSERT_TRUE(yaml["TypeCast"]["expr"]);
}

TEST(YAMLGenerator, SizeCast)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.SizeCast.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_size(new AST::Node);
    AST::Node::Ptr c_expr(new AST::Node);

    AST::SizeCast::Ptr m_sizecast(new AST::SizeCast(c_size, c_expr, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_sizecast);

    ASSERT_TRUE(yaml["SizeCast"]);
    ASSERT_TRUE(yaml["SizeCast"]["size"]);
    ASSERT_TRUE(yaml["SizeCast"]["expr"]);
}

TEST(YAMLGenerator, SigningCast)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.SigningCast.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_expr(new AST::Node);
    AST::SigningCast::SigningEnum p_signing = AST::SigningCast::SigningEnum::UNSIGNED;

    AST::SigningCast::Ptr m_signingcast(new AST::SigningCast(c_expr, p_signing, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_signingcast);

    ASSERT_TRUE(yaml["SigningCast"]);
    ASSERT_TRUE(yaml["SigningCast"]["expr"]);
    ASSERT_TRUE(yaml["SigningCast"]["signing"].as<AST::SigningCast::SigningEnum>() ==
                AST::SigningCast::SigningEnum::UNSIGNED);
}

TEST(YAMLGenerator, ConstCast)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.ConstCast.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_expr(new AST::Node);

    AST::ConstCast::Ptr m_constcast(new AST::ConstCast(c_expr, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_constcast);

    ASSERT_TRUE(yaml["ConstCast"]);
    ASSERT_TRUE(yaml["ConstCast"]["expr"]);
}

TEST(YAMLGenerator, Indirect)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Indirect.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_var(new AST::Node);

    AST::Indirect::Ptr m_indirect(new AST::Indirect(c_var, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_indirect);

    ASSERT_TRUE(yaml["Indirect"]);
    ASSERT_TRUE(yaml["Indirect"]["var"]);
}

TEST(YAMLGenerator, Partselect)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Partselect.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_msb(new AST::Node);
    AST::Node::Ptr c_lsb(new AST::Node);
    AST::Node::Ptr c_var(new AST::Node);

    AST::Partselect::Ptr m_partselect(new AST::Partselect(c_msb, c_lsb, c_var, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_partselect);

    ASSERT_TRUE(yaml["Partselect"]);
    ASSERT_TRUE(yaml["Partselect"]["msb"]);
    ASSERT_TRUE(yaml["Partselect"]["lsb"]);
    ASSERT_TRUE(yaml["Partselect"]["var"]);
}

TEST(YAMLGenerator, PartselectIndexed)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.PartselectIndexed.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_index(new AST::Node);
    AST::Node::Ptr c_size(new AST::Node);
    AST::Node::Ptr c_var(new AST::Node);

    AST::PartselectIndexed::Ptr m_partselectindexed(
        new AST::PartselectIndexed(c_index, c_size, c_var, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_partselectindexed);

    ASSERT_TRUE(yaml["PartselectIndexed"]);
    ASSERT_TRUE(yaml["PartselectIndexed"]["index"]);
    ASSERT_TRUE(yaml["PartselectIndexed"]["size"]);
    ASSERT_TRUE(yaml["PartselectIndexed"]["var"]);
}

TEST(YAMLGenerator, PartselectPlusIndexed)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.PartselectPlusIndexed.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_index(new AST::Node);
    AST::Node::Ptr c_size(new AST::Node);
    AST::Node::Ptr c_var(new AST::Node);

    AST::PartselectPlusIndexed::Ptr m_partselectplusindexed(
        new AST::PartselectPlusIndexed(c_index, c_size, c_var, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_partselectplusindexed);

    ASSERT_TRUE(yaml["PartselectPlusIndexed"]);
    ASSERT_TRUE(yaml["PartselectPlusIndexed"]["index"]);
    ASSERT_TRUE(yaml["PartselectPlusIndexed"]["size"]);
    ASSERT_TRUE(yaml["PartselectPlusIndexed"]["var"]);
}

TEST(YAMLGenerator, PartselectMinusIndexed)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.PartselectMinusIndexed.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_index(new AST::Node);
    AST::Node::Ptr c_size(new AST::Node);
    AST::Node::Ptr c_var(new AST::Node);

    AST::PartselectMinusIndexed::Ptr m_partselectminusindexed(
        new AST::PartselectMinusIndexed(c_index, c_size, c_var, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_partselectminusindexed);

    ASSERT_TRUE(yaml["PartselectMinusIndexed"]);
    ASSERT_TRUE(yaml["PartselectMinusIndexed"]["index"]);
    ASSERT_TRUE(yaml["PartselectMinusIndexed"]["size"]);
    ASSERT_TRUE(yaml["PartselectMinusIndexed"]["var"]);
}

TEST(YAMLGenerator, Pointer)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Pointer.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_ptr(new AST::Node);
    AST::Node::Ptr c_var(new AST::Node);

    AST::Pointer::Ptr m_pointer(new AST::Pointer(c_ptr, c_var, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_pointer);

    ASSERT_TRUE(yaml["Pointer"]);
    ASSERT_TRUE(yaml["Pointer"]["ptr"]);
    ASSERT_TRUE(yaml["Pointer"]["var"]);
}

TEST(YAMLGenerator, Lvalue)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Lvalue.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_var(new AST::Node);

    AST::Lvalue::Ptr m_lvalue(new AST::Lvalue(c_var, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_lvalue);

    ASSERT_TRUE(yaml["Lvalue"]);
    ASSERT_TRUE(yaml["Lvalue"]["var"]);
}

TEST(YAMLGenerator, Rvalue)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Rvalue.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_var(new AST::Node);

    AST::Rvalue::Ptr m_rvalue(new AST::Rvalue(c_var, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_rvalue);

    ASSERT_TRUE(yaml["Rvalue"]);
    ASSERT_TRUE(yaml["Rvalue"]["var"]);
}

TEST(YAMLGenerator, UnaryOperator)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.UnaryOperator.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_right(new AST::Node);

    AST::UnaryOperator::Ptr m_unaryoperator(new AST::UnaryOperator(c_right, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_unaryoperator);

    ASSERT_TRUE(yaml["UnaryOperator"]);
    ASSERT_TRUE(yaml["UnaryOperator"]["right"]);
}

TEST(YAMLGenerator, Uplus)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Uplus.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_right(new AST::Node);

    AST::Uplus::Ptr m_uplus(new AST::Uplus(c_right, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_uplus);

    ASSERT_TRUE(yaml["Uplus"]);
    ASSERT_TRUE(yaml["Uplus"]["right"]);
}

TEST(YAMLGenerator, Uminus)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Uminus.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_right(new AST::Node);

    AST::Uminus::Ptr m_uminus(new AST::Uminus(c_right, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_uminus);

    ASSERT_TRUE(yaml["Uminus"]);
    ASSERT_TRUE(yaml["Uminus"]["right"]);
}

TEST(YAMLGenerator, Ulnot)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Ulnot.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_right(new AST::Node);

    AST::Ulnot::Ptr m_ulnot(new AST::Ulnot(c_right, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_ulnot);

    ASSERT_TRUE(yaml["Ulnot"]);
    ASSERT_TRUE(yaml["Ulnot"]["right"]);
}

TEST(YAMLGenerator, Unot)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Unot.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_right(new AST::Node);

    AST::Unot::Ptr m_unot(new AST::Unot(c_right, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_unot);

    ASSERT_TRUE(yaml["Unot"]);
    ASSERT_TRUE(yaml["Unot"]["right"]);
}

TEST(YAMLGenerator, Uand)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Uand.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_right(new AST::Node);

    AST::Uand::Ptr m_uand(new AST::Uand(c_right, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_uand);

    ASSERT_TRUE(yaml["Uand"]);
    ASSERT_TRUE(yaml["Uand"]["right"]);
}

TEST(YAMLGenerator, Unand)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Unand.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_right(new AST::Node);

    AST::Unand::Ptr m_unand(new AST::Unand(c_right, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_unand);

    ASSERT_TRUE(yaml["Unand"]);
    ASSERT_TRUE(yaml["Unand"]["right"]);
}

TEST(YAMLGenerator, Uor)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Uor.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_right(new AST::Node);

    AST::Uor::Ptr m_uor(new AST::Uor(c_right, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_uor);

    ASSERT_TRUE(yaml["Uor"]);
    ASSERT_TRUE(yaml["Uor"]["right"]);
}

TEST(YAMLGenerator, Unor)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Unor.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_right(new AST::Node);

    AST::Unor::Ptr m_unor(new AST::Unor(c_right, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_unor);

    ASSERT_TRUE(yaml["Unor"]);
    ASSERT_TRUE(yaml["Unor"]["right"]);
}

TEST(YAMLGenerator, Uxor)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Uxor.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_right(new AST::Node);

    AST::Uxor::Ptr m_uxor(new AST::Uxor(c_right, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_uxor);

    ASSERT_TRUE(yaml["Uxor"]);
    ASSERT_TRUE(yaml["Uxor"]["right"]);
}

TEST(YAMLGenerator, Uxnor)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Uxnor.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_right(new AST::Node);

    AST::Uxnor::Ptr m_uxnor(new AST::Uxnor(c_right, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_uxnor);

    ASSERT_TRUE(yaml["Uxnor"]);
    ASSERT_TRUE(yaml["Uxnor"]["right"]);
}

TEST(YAMLGenerator, Operator)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Operator.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_left(new AST::Node);
    AST::Node::Ptr c_right(new AST::Node);

    AST::Operator::Ptr m_operator(new AST::Operator(c_left, c_right, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_operator);

    ASSERT_TRUE(yaml["Operator"]);
    ASSERT_TRUE(yaml["Operator"]["left"]);
    ASSERT_TRUE(yaml["Operator"]["right"]);
}

TEST(YAMLGenerator, Power)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Power.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_left(new AST::Node);
    AST::Node::Ptr c_right(new AST::Node);

    AST::Power::Ptr m_power(new AST::Power(c_left, c_right, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_power);

    ASSERT_TRUE(yaml["Power"]);
    ASSERT_TRUE(yaml["Power"]["left"]);
    ASSERT_TRUE(yaml["Power"]["right"]);
}

TEST(YAMLGenerator, Times)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Times.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_left(new AST::Node);
    AST::Node::Ptr c_right(new AST::Node);

    AST::Times::Ptr m_times(new AST::Times(c_left, c_right, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_times);

    ASSERT_TRUE(yaml["Times"]);
    ASSERT_TRUE(yaml["Times"]["left"]);
    ASSERT_TRUE(yaml["Times"]["right"]);
}

TEST(YAMLGenerator, Divide)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Divide.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_left(new AST::Node);
    AST::Node::Ptr c_right(new AST::Node);

    AST::Divide::Ptr m_divide(new AST::Divide(c_left, c_right, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_divide);

    ASSERT_TRUE(yaml["Divide"]);
    ASSERT_TRUE(yaml["Divide"]["left"]);
    ASSERT_TRUE(yaml["Divide"]["right"]);
}

TEST(YAMLGenerator, Mod)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Mod.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_left(new AST::Node);
    AST::Node::Ptr c_right(new AST::Node);

    AST::Mod::Ptr m_mod(new AST::Mod(c_left, c_right, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_mod);

    ASSERT_TRUE(yaml["Mod"]);
    ASSERT_TRUE(yaml["Mod"]["left"]);
    ASSERT_TRUE(yaml["Mod"]["right"]);
}

TEST(YAMLGenerator, Plus)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Plus.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_left(new AST::Node);
    AST::Node::Ptr c_right(new AST::Node);

    AST::Plus::Ptr m_plus(new AST::Plus(c_left, c_right, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_plus);

    ASSERT_TRUE(yaml["Plus"]);
    ASSERT_TRUE(yaml["Plus"]["left"]);
    ASSERT_TRUE(yaml["Plus"]["right"]);
}

TEST(YAMLGenerator, Minus)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Minus.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_left(new AST::Node);
    AST::Node::Ptr c_right(new AST::Node);

    AST::Minus::Ptr m_minus(new AST::Minus(c_left, c_right, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_minus);

    ASSERT_TRUE(yaml["Minus"]);
    ASSERT_TRUE(yaml["Minus"]["left"]);
    ASSERT_TRUE(yaml["Minus"]["right"]);
}

TEST(YAMLGenerator, Sll)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Sll.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_left(new AST::Node);
    AST::Node::Ptr c_right(new AST::Node);

    AST::Sll::Ptr m_sll(new AST::Sll(c_left, c_right, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_sll);

    ASSERT_TRUE(yaml["Sll"]);
    ASSERT_TRUE(yaml["Sll"]["left"]);
    ASSERT_TRUE(yaml["Sll"]["right"]);
}

TEST(YAMLGenerator, Srl)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Srl.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_left(new AST::Node);
    AST::Node::Ptr c_right(new AST::Node);

    AST::Srl::Ptr m_srl(new AST::Srl(c_left, c_right, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_srl);

    ASSERT_TRUE(yaml["Srl"]);
    ASSERT_TRUE(yaml["Srl"]["left"]);
    ASSERT_TRUE(yaml["Srl"]["right"]);
}

TEST(YAMLGenerator, Sra)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Sra.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_left(new AST::Node);
    AST::Node::Ptr c_right(new AST::Node);

    AST::Sra::Ptr m_sra(new AST::Sra(c_left, c_right, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_sra);

    ASSERT_TRUE(yaml["Sra"]);
    ASSERT_TRUE(yaml["Sra"]["left"]);
    ASSERT_TRUE(yaml["Sra"]["right"]);
}

TEST(YAMLGenerator, LessThan)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.LessThan.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_left(new AST::Node);
    AST::Node::Ptr c_right(new AST::Node);

    AST::LessThan::Ptr m_lessthan(new AST::LessThan(c_left, c_right, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_lessthan);

    ASSERT_TRUE(yaml["LessThan"]);
    ASSERT_TRUE(yaml["LessThan"]["left"]);
    ASSERT_TRUE(yaml["LessThan"]["right"]);
}

TEST(YAMLGenerator, GreaterThan)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.GreaterThan.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_left(new AST::Node);
    AST::Node::Ptr c_right(new AST::Node);

    AST::GreaterThan::Ptr m_greaterthan(new AST::GreaterThan(c_left, c_right, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_greaterthan);

    ASSERT_TRUE(yaml["GreaterThan"]);
    ASSERT_TRUE(yaml["GreaterThan"]["left"]);
    ASSERT_TRUE(yaml["GreaterThan"]["right"]);
}

TEST(YAMLGenerator, LessEq)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.LessEq.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_left(new AST::Node);
    AST::Node::Ptr c_right(new AST::Node);

    AST::LessEq::Ptr m_lesseq(new AST::LessEq(c_left, c_right, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_lesseq);

    ASSERT_TRUE(yaml["LessEq"]);
    ASSERT_TRUE(yaml["LessEq"]["left"]);
    ASSERT_TRUE(yaml["LessEq"]["right"]);
}

TEST(YAMLGenerator, GreaterEq)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.GreaterEq.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_left(new AST::Node);
    AST::Node::Ptr c_right(new AST::Node);

    AST::GreaterEq::Ptr m_greatereq(new AST::GreaterEq(c_left, c_right, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_greatereq);

    ASSERT_TRUE(yaml["GreaterEq"]);
    ASSERT_TRUE(yaml["GreaterEq"]["left"]);
    ASSERT_TRUE(yaml["GreaterEq"]["right"]);
}

TEST(YAMLGenerator, Eq)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Eq.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_left(new AST::Node);
    AST::Node::Ptr c_right(new AST::Node);

    AST::Eq::Ptr m_eq(new AST::Eq(c_left, c_right, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_eq);

    ASSERT_TRUE(yaml["Eq"]);
    ASSERT_TRUE(yaml["Eq"]["left"]);
    ASSERT_TRUE(yaml["Eq"]["right"]);
}

TEST(YAMLGenerator, NotEq)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.NotEq.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_left(new AST::Node);
    AST::Node::Ptr c_right(new AST::Node);

    AST::NotEq::Ptr m_noteq(new AST::NotEq(c_left, c_right, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_noteq);

    ASSERT_TRUE(yaml["NotEq"]);
    ASSERT_TRUE(yaml["NotEq"]["left"]);
    ASSERT_TRUE(yaml["NotEq"]["right"]);
}

TEST(YAMLGenerator, Eql)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Eql.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_left(new AST::Node);
    AST::Node::Ptr c_right(new AST::Node);

    AST::Eql::Ptr m_eql(new AST::Eql(c_left, c_right, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_eql);

    ASSERT_TRUE(yaml["Eql"]);
    ASSERT_TRUE(yaml["Eql"]["left"]);
    ASSERT_TRUE(yaml["Eql"]["right"]);
}

TEST(YAMLGenerator, NotEql)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.NotEql.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_left(new AST::Node);
    AST::Node::Ptr c_right(new AST::Node);

    AST::NotEql::Ptr m_noteql(new AST::NotEql(c_left, c_right, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_noteql);

    ASSERT_TRUE(yaml["NotEql"]);
    ASSERT_TRUE(yaml["NotEql"]["left"]);
    ASSERT_TRUE(yaml["NotEql"]["right"]);
}

TEST(YAMLGenerator, And)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.And.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_left(new AST::Node);
    AST::Node::Ptr c_right(new AST::Node);

    AST::And::Ptr m_and(new AST::And(c_left, c_right, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_and);

    ASSERT_TRUE(yaml["And"]);
    ASSERT_TRUE(yaml["And"]["left"]);
    ASSERT_TRUE(yaml["And"]["right"]);
}

TEST(YAMLGenerator, Xor)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Xor.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_left(new AST::Node);
    AST::Node::Ptr c_right(new AST::Node);

    AST::Xor::Ptr m_xor(new AST::Xor(c_left, c_right, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_xor);

    ASSERT_TRUE(yaml["Xor"]);
    ASSERT_TRUE(yaml["Xor"]["left"]);
    ASSERT_TRUE(yaml["Xor"]["right"]);
}

TEST(YAMLGenerator, Xnor)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Xnor.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_left(new AST::Node);
    AST::Node::Ptr c_right(new AST::Node);

    AST::Xnor::Ptr m_xnor(new AST::Xnor(c_left, c_right, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_xnor);

    ASSERT_TRUE(yaml["Xnor"]);
    ASSERT_TRUE(yaml["Xnor"]["left"]);
    ASSERT_TRUE(yaml["Xnor"]["right"]);
}

TEST(YAMLGenerator, Or)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Or.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_left(new AST::Node);
    AST::Node::Ptr c_right(new AST::Node);

    AST::Or::Ptr m_or(new AST::Or(c_left, c_right, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_or);

    ASSERT_TRUE(yaml["Or"]);
    ASSERT_TRUE(yaml["Or"]["left"]);
    ASSERT_TRUE(yaml["Or"]["right"]);
}

TEST(YAMLGenerator, Land)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Land.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_left(new AST::Node);
    AST::Node::Ptr c_right(new AST::Node);

    AST::Land::Ptr m_land(new AST::Land(c_left, c_right, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_land);

    ASSERT_TRUE(yaml["Land"]);
    ASSERT_TRUE(yaml["Land"]["left"]);
    ASSERT_TRUE(yaml["Land"]["right"]);
}

TEST(YAMLGenerator, Lor)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Lor.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_left(new AST::Node);
    AST::Node::Ptr c_right(new AST::Node);

    AST::Lor::Ptr m_lor(new AST::Lor(c_left, c_right, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_lor);

    ASSERT_TRUE(yaml["Lor"]);
    ASSERT_TRUE(yaml["Lor"]["left"]);
    ASSERT_TRUE(yaml["Lor"]["right"]);
}

TEST(YAMLGenerator, Cond)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Cond.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_cond(new AST::Node);
    AST::Node::Ptr c_left(new AST::Node);
    AST::Node::Ptr c_right(new AST::Node);

    AST::Cond::Ptr m_cond(new AST::Cond(c_cond, c_left, c_right, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_cond);

    ASSERT_TRUE(yaml["Cond"]);
    ASSERT_TRUE(yaml["Cond"]["cond"]);
    ASSERT_TRUE(yaml["Cond"]["left"]);
    ASSERT_TRUE(yaml["Cond"]["right"]);
}

TEST(YAMLGenerator, Always)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Always.log");
    Logger::add_stderr_sink();

    AST::Senslist::Ptr c_senslist(new AST::Senslist);
    AST::Node::Ptr c_statement(new AST::Node);

    AST::Always::Ptr m_always(new AST::Always(c_senslist, c_statement, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_always);

    ASSERT_TRUE(yaml["Always"]);
    ASSERT_TRUE(yaml["Always"]["senslist"]);
    ASSERT_TRUE(yaml["Always"]["statement"]);
}

TEST(YAMLGenerator, AlwaysFF)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.AlwaysFF.log");
    Logger::add_stderr_sink();

    AST::Senslist::Ptr c_senslist(new AST::Senslist);
    AST::Node::Ptr c_statement(new AST::Node);

    AST::AlwaysFF::Ptr m_alwaysff(new AST::AlwaysFF(c_senslist, c_statement, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_alwaysff);

    ASSERT_TRUE(yaml["AlwaysFF"]);
    ASSERT_TRUE(yaml["AlwaysFF"]["senslist"]);
    ASSERT_TRUE(yaml["AlwaysFF"]["statement"]);
}

TEST(YAMLGenerator, AlwaysComb)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.AlwaysComb.log");
    Logger::add_stderr_sink();

    AST::Senslist::Ptr c_senslist(new AST::Senslist);
    AST::Node::Ptr c_statement(new AST::Node);

    AST::AlwaysComb::Ptr m_alwayscomb(new AST::AlwaysComb(c_senslist, c_statement, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_alwayscomb);

    ASSERT_TRUE(yaml["AlwaysComb"]);
    ASSERT_TRUE(yaml["AlwaysComb"]["senslist"]);
    ASSERT_TRUE(yaml["AlwaysComb"]["statement"]);
}

TEST(YAMLGenerator, AlwaysLatch)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.AlwaysLatch.log");
    Logger::add_stderr_sink();

    AST::Senslist::Ptr c_senslist(new AST::Senslist);
    AST::Node::Ptr c_statement(new AST::Node);

    AST::AlwaysLatch::Ptr m_alwayslatch(
        new AST::AlwaysLatch(c_senslist, c_statement, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_alwayslatch);

    ASSERT_TRUE(yaml["AlwaysLatch"]);
    ASSERT_TRUE(yaml["AlwaysLatch"]["senslist"]);
    ASSERT_TRUE(yaml["AlwaysLatch"]["statement"]);
}

TEST(YAMLGenerator, Senslist)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Senslist.log");
    Logger::add_stderr_sink();

    AST::Sens::ListPtr c_list(new AST::Sens::List);

    AST::Senslist::Ptr m_senslist(new AST::Senslist(c_list, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_senslist);

    ASSERT_TRUE(yaml["Senslist"]);
    ASSERT_TRUE(yaml["Senslist"]["list"]);
}

TEST(YAMLGenerator, Sens)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Sens.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_sig(new AST::Node);
    AST::Sens::TypeEnum p_type = AST::Sens::TypeEnum::NONE;

    AST::Sens::Ptr m_sens(new AST::Sens(c_sig, p_type, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_sens);

    ASSERT_TRUE(yaml["Sens"]);
    ASSERT_TRUE(yaml["Sens"]["sig"]);
    ASSERT_TRUE(yaml["Sens"]["type"].as<AST::Sens::TypeEnum>() == AST::Sens::TypeEnum::NONE);
}

TEST(YAMLGenerator, Defparamlist)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Defparamlist.log");
    Logger::add_stderr_sink();

    AST::Defparam::ListPtr c_list(new AST::Defparam::List);

    AST::Defparamlist::Ptr m_defparamlist(new AST::Defparamlist(c_list, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_defparamlist);

    ASSERT_TRUE(yaml["Defparamlist"]);
    ASSERT_TRUE(yaml["Defparamlist"]["list"]);
}

TEST(YAMLGenerator, Defparam)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Defparam.log");
    Logger::add_stderr_sink();

    AST::Identifier::Ptr c_identifier(new AST::Identifier);
    AST::Rvalue::Ptr c_right(new AST::Rvalue);

    AST::Defparam::Ptr m_defparam(new AST::Defparam(c_identifier, c_right, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_defparam);

    ASSERT_TRUE(yaml["Defparam"]);
    ASSERT_TRUE(yaml["Defparam"]["identifier"]);
    ASSERT_TRUE(yaml["Defparam"]["right"]);
}

TEST(YAMLGenerator, Assign)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Assign.log");
    Logger::add_stderr_sink();

    AST::Lvalue::Ptr c_left(new AST::Lvalue);
    AST::Rvalue::Ptr c_right(new AST::Rvalue);
    AST::DelayStatement::Ptr c_ldelay(new AST::DelayStatement);
    AST::DelayStatement::Ptr c_rdelay(new AST::DelayStatement);

    AST::Assign::Ptr m_assign(new AST::Assign(c_left, c_right, c_ldelay, c_rdelay, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_assign);

    ASSERT_TRUE(yaml["Assign"]);
    ASSERT_TRUE(yaml["Assign"]["left"]);
    ASSERT_TRUE(yaml["Assign"]["right"]);
    ASSERT_TRUE(yaml["Assign"]["ldelay"]);
    ASSERT_TRUE(yaml["Assign"]["rdelay"]);
}

TEST(YAMLGenerator, BlockingSubstitution)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.BlockingSubstitution.log");
    Logger::add_stderr_sink();

    AST::Lvalue::Ptr c_left(new AST::Lvalue);
    AST::Rvalue::Ptr c_right(new AST::Rvalue);
    AST::DelayStatement::Ptr c_ldelay(new AST::DelayStatement);
    AST::DelayStatement::Ptr c_rdelay(new AST::DelayStatement);

    AST::BlockingSubstitution::Ptr m_blockingsubstitution(
        new AST::BlockingSubstitution(c_left, c_right, c_ldelay, c_rdelay, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_blockingsubstitution);

    ASSERT_TRUE(yaml["BlockingSubstitution"]);
    ASSERT_TRUE(yaml["BlockingSubstitution"]["left"]);
    ASSERT_TRUE(yaml["BlockingSubstitution"]["right"]);
    ASSERT_TRUE(yaml["BlockingSubstitution"]["ldelay"]);
    ASSERT_TRUE(yaml["BlockingSubstitution"]["rdelay"]);
}

TEST(YAMLGenerator, NonblockingSubstitution)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.NonblockingSubstitution.log");
    Logger::add_stderr_sink();

    AST::Lvalue::Ptr c_left(new AST::Lvalue);
    AST::Rvalue::Ptr c_right(new AST::Rvalue);
    AST::DelayStatement::Ptr c_ldelay(new AST::DelayStatement);
    AST::DelayStatement::Ptr c_rdelay(new AST::DelayStatement);

    AST::NonblockingSubstitution::Ptr m_nonblockingsubstitution(
        new AST::NonblockingSubstitution(c_left, c_right, c_ldelay, c_rdelay, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_nonblockingsubstitution);

    ASSERT_TRUE(yaml["NonblockingSubstitution"]);
    ASSERT_TRUE(yaml["NonblockingSubstitution"]["left"]);
    ASSERT_TRUE(yaml["NonblockingSubstitution"]["right"]);
    ASSERT_TRUE(yaml["NonblockingSubstitution"]["ldelay"]);
    ASSERT_TRUE(yaml["NonblockingSubstitution"]["rdelay"]);
}

TEST(YAMLGenerator, IfStatement)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.IfStatement.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_cond(new AST::Node);
    AST::Node::Ptr c_true_statement(new AST::Node);
    AST::Node::Ptr c_false_statement(new AST::Node);

    AST::IfStatement::Ptr m_ifstatement(
        new AST::IfStatement(c_cond, c_true_statement, c_false_statement, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_ifstatement);

    ASSERT_TRUE(yaml["IfStatement"]);
    ASSERT_TRUE(yaml["IfStatement"]["cond"]);
    ASSERT_TRUE(yaml["IfStatement"]["true_statement"]);
    ASSERT_TRUE(yaml["IfStatement"]["false_statement"]);
}

TEST(YAMLGenerator, RepeatStatement)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.RepeatStatement.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_times(new AST::Node);
    AST::Node::Ptr c_statement(new AST::Node);

    AST::RepeatStatement::Ptr m_repeatstatement(
        new AST::RepeatStatement(c_times, c_statement, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_repeatstatement);

    ASSERT_TRUE(yaml["RepeatStatement"]);
    ASSERT_TRUE(yaml["RepeatStatement"]["times"]);
    ASSERT_TRUE(yaml["RepeatStatement"]["statement"]);
}

TEST(YAMLGenerator, ForStatement)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.ForStatement.log");
    Logger::add_stderr_sink();

    AST::BlockingSubstitution::Ptr c_pre(new AST::BlockingSubstitution);
    AST::Node::Ptr c_cond(new AST::Node);
    AST::BlockingSubstitution::Ptr c_post(new AST::BlockingSubstitution);
    AST::Node::Ptr c_statement(new AST::Node);

    AST::ForStatement::Ptr m_forstatement(
        new AST::ForStatement(c_pre, c_cond, c_post, c_statement, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_forstatement);

    ASSERT_TRUE(yaml["ForStatement"]);
    ASSERT_TRUE(yaml["ForStatement"]["pre"]);
    ASSERT_TRUE(yaml["ForStatement"]["cond"]);
    ASSERT_TRUE(yaml["ForStatement"]["post"]);
    ASSERT_TRUE(yaml["ForStatement"]["statement"]);
}

TEST(YAMLGenerator, WhileStatement)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.WhileStatement.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_cond(new AST::Node);
    AST::Node::Ptr c_statement(new AST::Node);

    AST::WhileStatement::Ptr m_whilestatement(
        new AST::WhileStatement(c_cond, c_statement, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_whilestatement);

    ASSERT_TRUE(yaml["WhileStatement"]);
    ASSERT_TRUE(yaml["WhileStatement"]["cond"]);
    ASSERT_TRUE(yaml["WhileStatement"]["statement"]);
}

TEST(YAMLGenerator, CaseStatement)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.CaseStatement.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_comp(new AST::Node);
    AST::Case::ListPtr c_caselist(new AST::Case::List);

    AST::CaseStatement::Ptr m_casestatement(
        new AST::CaseStatement(c_comp, c_caselist, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_casestatement);

    ASSERT_TRUE(yaml["CaseStatement"]);
    ASSERT_TRUE(yaml["CaseStatement"]["comp"]);
    ASSERT_TRUE(yaml["CaseStatement"]["caselist"]);
}

TEST(YAMLGenerator, CasexStatement)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.CasexStatement.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_comp(new AST::Node);
    AST::Case::ListPtr c_caselist(new AST::Case::List);

    AST::CasexStatement::Ptr m_casexstatement(
        new AST::CasexStatement(c_comp, c_caselist, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_casexstatement);

    ASSERT_TRUE(yaml["CasexStatement"]);
    ASSERT_TRUE(yaml["CasexStatement"]["comp"]);
    ASSERT_TRUE(yaml["CasexStatement"]["caselist"]);
}

TEST(YAMLGenerator, CasezStatement)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.CasezStatement.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_comp(new AST::Node);
    AST::Case::ListPtr c_caselist(new AST::Case::List);

    AST::CasezStatement::Ptr m_casezstatement(
        new AST::CasezStatement(c_comp, c_caselist, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_casezstatement);

    ASSERT_TRUE(yaml["CasezStatement"]);
    ASSERT_TRUE(yaml["CasezStatement"]["comp"]);
    ASSERT_TRUE(yaml["CasezStatement"]["caselist"]);
}

TEST(YAMLGenerator, UniqueCaseStatement)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.UniqueCaseStatement.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_comp(new AST::Node);
    AST::Case::ListPtr c_caselist(new AST::Case::List);

    AST::UniqueCaseStatement::Ptr m_uniquecasestatement(
        new AST::UniqueCaseStatement(c_comp, c_caselist, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_uniquecasestatement);

    ASSERT_TRUE(yaml["UniqueCaseStatement"]);
    ASSERT_TRUE(yaml["UniqueCaseStatement"]["comp"]);
    ASSERT_TRUE(yaml["UniqueCaseStatement"]["caselist"]);
}

TEST(YAMLGenerator, PriorityCaseStatement)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.PriorityCaseStatement.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_comp(new AST::Node);
    AST::Case::ListPtr c_caselist(new AST::Case::List);

    AST::PriorityCaseStatement::Ptr m_prioritycasestatement(
        new AST::PriorityCaseStatement(c_comp, c_caselist, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_prioritycasestatement);

    ASSERT_TRUE(yaml["PriorityCaseStatement"]);
    ASSERT_TRUE(yaml["PriorityCaseStatement"]["comp"]);
    ASSERT_TRUE(yaml["PriorityCaseStatement"]["caselist"]);
}

TEST(YAMLGenerator, Case)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Case.log");
    Logger::add_stderr_sink();

    AST::Node::ListPtr c_cond(new AST::Node::List);
    AST::Node::Ptr c_statement(new AST::Node);

    AST::Case::Ptr m_case(new AST::Case(c_cond, c_statement, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_case);

    ASSERT_TRUE(yaml["Case"]);
    ASSERT_TRUE(yaml["Case"]["cond"]);
    ASSERT_TRUE(yaml["Case"]["statement"]);
}

TEST(YAMLGenerator, Block)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Block.log");
    Logger::add_stderr_sink();

    AST::Node::ListPtr c_statements(new AST::Node::List);
    std::string p_scope = "mynbiqpmzj";

    AST::Block::Ptr m_block(new AST::Block(c_statements, p_scope, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_block);

    ASSERT_TRUE(yaml["Block"]);
    ASSERT_TRUE(yaml["Block"]["statements"]);
    ASSERT_TRUE(yaml["Block"]["scope"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLGenerator, Initial)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Initial.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_statement(new AST::Node);

    AST::Initial::Ptr m_initial(new AST::Initial(c_statement, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_initial);

    ASSERT_TRUE(yaml["Initial"]);
    ASSERT_TRUE(yaml["Initial"]["statement"]);
}

TEST(YAMLGenerator, EventStatement)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.EventStatement.log");
    Logger::add_stderr_sink();

    AST::Senslist::Ptr c_senslist(new AST::Senslist);
    AST::Node::Ptr c_statement(new AST::Node);

    AST::EventStatement::Ptr m_eventstatement(
        new AST::EventStatement(c_senslist, c_statement, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_eventstatement);

    ASSERT_TRUE(yaml["EventStatement"]);
    ASSERT_TRUE(yaml["EventStatement"]["senslist"]);
    ASSERT_TRUE(yaml["EventStatement"]["statement"]);
}

TEST(YAMLGenerator, WaitStatement)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.WaitStatement.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_cond(new AST::Node);
    AST::Node::Ptr c_statement(new AST::Node);

    AST::WaitStatement::Ptr m_waitstatement(
        new AST::WaitStatement(c_cond, c_statement, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_waitstatement);

    ASSERT_TRUE(yaml["WaitStatement"]);
    ASSERT_TRUE(yaml["WaitStatement"]["cond"]);
    ASSERT_TRUE(yaml["WaitStatement"]["statement"]);
}

TEST(YAMLGenerator, ForeverStatement)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.ForeverStatement.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_statement(new AST::Node);

    AST::ForeverStatement::Ptr m_foreverstatement(
        new AST::ForeverStatement(c_statement, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_foreverstatement);

    ASSERT_TRUE(yaml["ForeverStatement"]);
    ASSERT_TRUE(yaml["ForeverStatement"]["statement"]);
}

TEST(YAMLGenerator, DelayStatement)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.DelayStatement.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_delay(new AST::Node);

    AST::DelayStatement::Ptr m_delaystatement(new AST::DelayStatement(c_delay, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_delaystatement);

    ASSERT_TRUE(yaml["DelayStatement"]);
    ASSERT_TRUE(yaml["DelayStatement"]["delay"]);
}

TEST(YAMLGenerator, Instancelist)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Instancelist.log");
    Logger::add_stderr_sink();

    AST::ParamArg::ListPtr c_parameterlist(new AST::ParamArg::List);
    AST::Instance::ListPtr c_instances(new AST::Instance::List);
    std::string p_module = "mynbiqpmzj";

    AST::Instancelist::Ptr m_instancelist(
        new AST::Instancelist(c_parameterlist, c_instances, p_module, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_instancelist);

    ASSERT_TRUE(yaml["Instancelist"]);
    ASSERT_TRUE(yaml["Instancelist"]["parameterlist"]);
    ASSERT_TRUE(yaml["Instancelist"]["instances"]);
    ASSERT_TRUE(yaml["Instancelist"]["module"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLGenerator, Instance)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Instance.log");
    Logger::add_stderr_sink();

    AST::Dimension::Ptr c_array(new AST::Dimension);
    AST::ParamArg::ListPtr c_parameterlist(new AST::ParamArg::List);
    AST::PortArg::ListPtr c_portlist(new AST::PortArg::List);
    std::string p_module = "mynbiqpmzj";
    std::string p_name = "plsgqejeyd";

    AST::Instance::Ptr m_instance(
        new AST::Instance(c_array, c_parameterlist, c_portlist, p_module, p_name, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_instance);

    ASSERT_TRUE(yaml["Instance"]);
    ASSERT_TRUE(yaml["Instance"]["array"]);
    ASSERT_TRUE(yaml["Instance"]["parameterlist"]);
    ASSERT_TRUE(yaml["Instance"]["portlist"]);
    ASSERT_TRUE(yaml["Instance"]["module"].as<std::string>() == "mynbiqpmzj");
    ASSERT_TRUE(yaml["Instance"]["name"].as<std::string>() == "plsgqejeyd");
}

TEST(YAMLGenerator, ParamArg)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.ParamArg.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_value(new AST::Node);
    std::string p_name = "mynbiqpmzj";

    AST::ParamArg::Ptr m_paramarg(new AST::ParamArg(c_value, p_name, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_paramarg);

    ASSERT_TRUE(yaml["ParamArg"]);
    ASSERT_TRUE(yaml["ParamArg"]["value"]);
    ASSERT_TRUE(yaml["ParamArg"]["name"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLGenerator, PortArg)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.PortArg.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_value(new AST::Node);
    std::string p_name = "mynbiqpmzj";

    AST::PortArg::Ptr m_portarg(new AST::PortArg(c_value, p_name, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_portarg);

    ASSERT_TRUE(yaml["PortArg"]);
    ASSERT_TRUE(yaml["PortArg"]["value"]);
    ASSERT_TRUE(yaml["PortArg"]["name"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLGenerator, Function)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Function.log");
    Logger::add_stderr_sink();

    AST::DataType::Ptr c_return_type(new AST::DataType);
    AST::Arg::ListPtr c_args(new AST::Arg::List);
    AST::Node::ListPtr c_statements(new AST::Node::List);
    std::string p_name = "mynbiqpmzj";
    AST::Function::LifetimeEnum p_lifetime = AST::Function::LifetimeEnum::AUTOMATIC;

    AST::Function::Ptr m_function(
        new AST::Function(c_return_type, c_args, c_statements, p_name, p_lifetime, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_function);

    ASSERT_TRUE(yaml["Function"]);
    ASSERT_TRUE(yaml["Function"]["return_type"]);
    ASSERT_TRUE(yaml["Function"]["args"]);
    ASSERT_TRUE(yaml["Function"]["statements"]);
    ASSERT_TRUE(yaml["Function"]["name"].as<std::string>() == "mynbiqpmzj");
    ASSERT_TRUE(yaml["Function"]["lifetime"].as<AST::Function::LifetimeEnum>() ==
                AST::Function::LifetimeEnum::AUTOMATIC);
}

TEST(YAMLGenerator, FunctionCall)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.FunctionCall.log");
    Logger::add_stderr_sink();

    AST::Node::ListPtr c_args(new AST::Node::List);
    AST::ScopeName::ListPtr c_scope(new AST::ScopeName::List);
    AST::HierName::Ptr c_hier(new AST::HierName);
    std::string p_name = "mynbiqpmzj";

    AST::FunctionCall::Ptr m_functioncall(
        new AST::FunctionCall(c_args, c_scope, c_hier, p_name, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_functioncall);

    ASSERT_TRUE(yaml["FunctionCall"]);
    ASSERT_TRUE(yaml["FunctionCall"]["args"]);
    ASSERT_TRUE(yaml["FunctionCall"]["scope"]);
    ASSERT_TRUE(yaml["FunctionCall"]["hier"]);
    ASSERT_TRUE(yaml["FunctionCall"]["name"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLGenerator, Task)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Task.log");
    Logger::add_stderr_sink();

    AST::Arg::ListPtr c_args(new AST::Arg::List);
    AST::Node::ListPtr c_statements(new AST::Node::List);
    std::string p_name = "mynbiqpmzj";
    AST::Task::LifetimeEnum p_lifetime = AST::Task::LifetimeEnum::AUTOMATIC;

    AST::Task::Ptr m_task(new AST::Task(c_args, c_statements, p_name, p_lifetime, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_task);

    ASSERT_TRUE(yaml["Task"]);
    ASSERT_TRUE(yaml["Task"]["args"]);
    ASSERT_TRUE(yaml["Task"]["statements"]);
    ASSERT_TRUE(yaml["Task"]["name"].as<std::string>() == "mynbiqpmzj");
    ASSERT_TRUE(yaml["Task"]["lifetime"].as<AST::Task::LifetimeEnum>() ==
                AST::Task::LifetimeEnum::AUTOMATIC);
}

TEST(YAMLGenerator, TaskCall)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.TaskCall.log");
    Logger::add_stderr_sink();

    AST::Node::ListPtr c_args(new AST::Node::List);
    AST::ScopeName::ListPtr c_scope(new AST::ScopeName::List);
    AST::HierName::Ptr c_hier(new AST::HierName);
    std::string p_name = "mynbiqpmzj";

    AST::TaskCall::Ptr m_taskcall(
        new AST::TaskCall(c_args, c_scope, c_hier, p_name, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_taskcall);

    ASSERT_TRUE(yaml["TaskCall"]);
    ASSERT_TRUE(yaml["TaskCall"]["args"]);
    ASSERT_TRUE(yaml["TaskCall"]["scope"]);
    ASSERT_TRUE(yaml["TaskCall"]["hier"]);
    ASSERT_TRUE(yaml["TaskCall"]["name"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLGenerator, GenerateStatement)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.GenerateStatement.log");
    Logger::add_stderr_sink();

    AST::Node::ListPtr c_items(new AST::Node::List);

    AST::GenerateStatement::Ptr m_generatestatement(
        new AST::GenerateStatement(c_items, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_generatestatement);

    ASSERT_TRUE(yaml["GenerateStatement"]);
    ASSERT_TRUE(yaml["GenerateStatement"]["items"]);
}

TEST(YAMLGenerator, SystemCall)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.SystemCall.log");
    Logger::add_stderr_sink();

    AST::Node::ListPtr c_args(new AST::Node::List);
    std::string p_syscall = "mynbiqpmzj";

    AST::SystemCall::Ptr m_systemcall(new AST::SystemCall(c_args, p_syscall, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_systemcall);

    ASSERT_TRUE(yaml["SystemCall"]);
    ASSERT_TRUE(yaml["SystemCall"]["args"]);
    ASSERT_TRUE(yaml["SystemCall"]["syscall"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLGenerator, HierLabel)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.HierLabel.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_loop(new AST::Node);
    std::string p_name = "mynbiqpmzj";

    AST::HierLabel::Ptr m_hierlabel(new AST::HierLabel(c_loop, p_name, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_hierlabel);

    ASSERT_TRUE(yaml["HierLabel"]);
    ASSERT_TRUE(yaml["HierLabel"]["loop"]);
    ASSERT_TRUE(yaml["HierLabel"]["name"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLGenerator, HierName)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.HierName.log");
    Logger::add_stderr_sink();

    AST::HierLabel::ListPtr c_labellist(new AST::HierLabel::List);

    AST::HierName::Ptr m_hiername(new AST::HierName(c_labellist, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_hiername);

    ASSERT_TRUE(yaml["HierName"]);
    ASSERT_TRUE(yaml["HierName"]["labellist"]);
}

TEST(YAMLGenerator, Disable)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.Disable.log");
    Logger::add_stderr_sink();

    AST::Identifier::Ptr c_dest(new AST::Identifier);

    AST::Disable::Ptr m_disable(new AST::Disable(c_dest, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_disable);

    ASSERT_TRUE(yaml["Disable"]);
    ASSERT_TRUE(yaml["Disable"]["dest"]);
}

TEST(YAMLGenerator, ParallelBlock)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.ParallelBlock.log");
    Logger::add_stderr_sink();

    AST::Node::ListPtr c_statements(new AST::Node::List);
    std::string p_scope = "mynbiqpmzj";

    AST::ParallelBlock::Ptr m_parallelblock(
        new AST::ParallelBlock(c_statements, p_scope, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_parallelblock);

    ASSERT_TRUE(yaml["ParallelBlock"]);
    ASSERT_TRUE(yaml["ParallelBlock"]["statements"]);
    ASSERT_TRUE(yaml["ParallelBlock"]["scope"].as<std::string>() == "mynbiqpmzj");
}

TEST(YAMLGenerator, SingleStatement)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("YAMLGenerator.SingleStatement.log");
    Logger::add_stderr_sink();

    AST::Node::Ptr c_statement(new AST::Node);
    AST::DelayStatement::Ptr c_delay(new AST::DelayStatement);
    std::string p_scope = "mynbiqpmzj";

    AST::SingleStatement::Ptr m_singlestatement(
        new AST::SingleStatement(c_statement, c_delay, p_scope, "filename", 0));

    YAML::Node yaml = Generators::YAMLGenerator().render(m_singlestatement);

    ASSERT_TRUE(yaml["SingleStatement"]);
    ASSERT_TRUE(yaml["SingleStatement"]["statement"]);
    ASSERT_TRUE(yaml["SingleStatement"]["delay"]);
    ASSERT_TRUE(yaml["SingleStatement"]["scope"].as<std::string>() == "mynbiqpmzj");
}
