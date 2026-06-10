// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/importers/yaml_importer.hpp>
#include <veriparse/importers/yaml_specializations.hpp>
#include <veriparse/AST/node_cast.hpp>

#include <memory>
#include <iostream>
#include <exception>

namespace Veriparse
{
namespace Importers
{

AST::Node::Ptr YAMLImporter::import(const char *filename) const
{
    YAML::Node node;

    try {
        node = YAML::LoadFile(filename);
    } catch(const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    return convert(node);
}

AST::Node::Ptr YAMLImporter::import(std::istream &iss) const
{
    YAML::Node node;

    try {
        node = YAML::Load(iss);
    } catch(const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    return convert(node);
}

AST::Node::Ptr YAMLImporter::import(const std::string &str) const
{
    YAML::Node node;

    try {
        node = YAML::Load(str);
    } catch(const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    return convert(node);
}

AST::Node::Ptr YAMLImporter::convert(const YAML::Node node) const
{
    if(node.IsMap()) {
        if(node["Source"]) {
            return convert_source(node["Source"]);
        }
        if(node["Description"]) {
            return convert_description(node["Description"]);
        }
        if(node["Pragmalist"]) {
            return convert_pragmalist(node["Pragmalist"]);
        }
        if(node["Pragma"]) {
            return convert_pragma(node["Pragma"]);
        }
        if(node["Module"]) {
            return convert_module(node["Module"]);
        }
        if(node["Port"]) {
            return convert_port(node["Port"]);
        }
        if(node["Package"]) {
            return convert_package(node["Package"]);
        }
        if(node["Import"]) {
            return convert_import(node["Import"]);
        }
        if(node["Identifier"]) {
            return convert_identifier(node["Identifier"]);
        }
        if(node["Constant"]) {
            return convert_constant(node["Constant"]);
        }
        if(node["StringConst"]) {
            return convert_stringconst(node["StringConst"]);
        }
        if(node["IntConst"]) {
            return convert_intconst(node["IntConst"]);
        }
        if(node["IntConstN"]) {
            return convert_intconstn(node["IntConstN"]);
        }
        if(node["FloatConst"]) {
            return convert_floatconst(node["FloatConst"]);
        }
        if(node["DataType"]) {
            return convert_datatype(node["DataType"]);
        }
        if(node["LogicType"]) {
            return convert_logictype(node["LogicType"]);
        }
        if(node["RegType"]) {
            return convert_regtype(node["RegType"]);
        }
        if(node["BitType"]) {
            return convert_bittype(node["BitType"]);
        }
        if(node["ByteType"]) {
            return convert_bytetype(node["ByteType"]);
        }
        if(node["ShortintType"]) {
            return convert_shortinttype(node["ShortintType"]);
        }
        if(node["IntType"]) {
            return convert_inttype(node["IntType"]);
        }
        if(node["LongintType"]) {
            return convert_longinttype(node["LongintType"]);
        }
        if(node["IntegerType"]) {
            return convert_integertype(node["IntegerType"]);
        }
        if(node["TimeType"]) {
            return convert_timetype(node["TimeType"]);
        }
        if(node["RealType"]) {
            return convert_realtype(node["RealType"]);
        }
        if(node["ShortrealType"]) {
            return convert_shortrealtype(node["ShortrealType"]);
        }
        if(node["RealtimeType"]) {
            return convert_realtimetype(node["RealtimeType"]);
        }
        if(node["StringType"]) {
            return convert_stringtype(node["StringType"]);
        }
        if(node["ChandleType"]) {
            return convert_chandletype(node["ChandleType"]);
        }
        if(node["EventType"]) {
            return convert_eventtype(node["EventType"]);
        }
        if(node["ImplicitType"]) {
            return convert_implicittype(node["ImplicitType"]);
        }
        if(node["VoidType"]) {
            return convert_voidtype(node["VoidType"]);
        }
        if(node["NamedType"]) {
            return convert_namedtype(node["NamedType"]);
        }
        if(node["ScopeName"]) {
            return convert_scopename(node["ScopeName"]);
        }
        if(node["StructType"]) {
            return convert_structtype(node["StructType"]);
        }
        if(node["UnionType"]) {
            return convert_uniontype(node["UnionType"]);
        }
        if(node["EnumType"]) {
            return convert_enumtype(node["EnumType"]);
        }
        if(node["EnumItem"]) {
            return convert_enumitem(node["EnumItem"]);
        }
        if(node["TypeOpExpr"]) {
            return convert_typeopexpr(node["TypeOpExpr"]);
        }
        if(node["TypeOpType"]) {
            return convert_typeoptype(node["TypeOpType"]);
        }
        if(node["Dimension"]) {
            return convert_dimension(node["Dimension"]);
        }
        if(node["RangeDim"]) {
            return convert_rangedim(node["RangeDim"]);
        }
        if(node["SizeDim"]) {
            return convert_sizedim(node["SizeDim"]);
        }
        if(node["UnsizedDim"]) {
            return convert_unsizeddim(node["UnsizedDim"]);
        }
        if(node["QueueDim"]) {
            return convert_queuedim(node["QueueDim"]);
        }
        if(node["AssocDim"]) {
            return convert_assocdim(node["AssocDim"]);
        }
        if(node["Declaration"]) {
            return convert_declaration(node["Declaration"]);
        }
        if(node["Var"]) {
            return convert_var(node["Var"]);
        }
        if(node["Net"]) {
            return convert_net(node["Net"]);
        }
        if(node["WireNet"]) {
            return convert_wirenet(node["WireNet"]);
        }
        if(node["TriNet"]) {
            return convert_trinet(node["TriNet"]);
        }
        if(node["Tri0Net"]) {
            return convert_tri0net(node["Tri0Net"]);
        }
        if(node["Tri1Net"]) {
            return convert_tri1net(node["Tri1Net"]);
        }
        if(node["TriandNet"]) {
            return convert_triandnet(node["TriandNet"]);
        }
        if(node["TriorNet"]) {
            return convert_triornet(node["TriorNet"]);
        }
        if(node["TriregNet"]) {
            return convert_triregnet(node["TriregNet"]);
        }
        if(node["WandNet"]) {
            return convert_wandnet(node["WandNet"]);
        }
        if(node["WorNet"]) {
            return convert_wornet(node["WorNet"]);
        }
        if(node["UwireNet"]) {
            return convert_uwirenet(node["UwireNet"]);
        }
        if(node["Supply0Net"]) {
            return convert_supply0net(node["Supply0Net"]);
        }
        if(node["Supply1Net"]) {
            return convert_supply1net(node["Supply1Net"]);
        }
        if(node["InterconnectNet"]) {
            return convert_interconnectnet(node["InterconnectNet"]);
        }
        if(node["UserNet"]) {
            return convert_usernet(node["UserNet"]);
        }
        if(node["ImplicitNet"]) {
            return convert_implicitnet(node["ImplicitNet"]);
        }
        if(node["Strength"]) {
            return convert_strength(node["Strength"]);
        }
        if(node["DriveStrength"]) {
            return convert_drivestrength(node["DriveStrength"]);
        }
        if(node["ChargeStrength"]) {
            return convert_chargestrength(node["ChargeStrength"]);
        }
        if(node["Param"]) {
            return convert_param(node["Param"]);
        }
        if(node["TypeParam"]) {
            return convert_typeparam(node["TypeParam"]);
        }
        if(node["Typedef"]) {
            return convert_typedef(node["Typedef"]);
        }
        if(node["Member"]) {
            return convert_member(node["Member"]);
        }
        if(node["Arg"]) {
            return convert_arg(node["Arg"]);
        }
        if(node["Genvar"]) {
            return convert_genvar(node["Genvar"]);
        }
        if(node["Concat"]) {
            return convert_concat(node["Concat"]);
        }
        if(node["Lconcat"]) {
            return convert_lconcat(node["Lconcat"]);
        }
        if(node["Repeat"]) {
            return convert_repeat(node["Repeat"]);
        }
        if(node["AssignmentPattern"]) {
            return convert_assignmentpattern(node["AssignmentPattern"]);
        }
        if(node["PatternItem"]) {
            return convert_patternitem(node["PatternItem"]);
        }
        if(node["Cast"]) {
            return convert_cast(node["Cast"]);
        }
        if(node["TypeCast"]) {
            return convert_typecast(node["TypeCast"]);
        }
        if(node["SizeCast"]) {
            return convert_sizecast(node["SizeCast"]);
        }
        if(node["SigningCast"]) {
            return convert_signingcast(node["SigningCast"]);
        }
        if(node["ConstCast"]) {
            return convert_constcast(node["ConstCast"]);
        }
        if(node["Indirect"]) {
            return convert_indirect(node["Indirect"]);
        }
        if(node["Partselect"]) {
            return convert_partselect(node["Partselect"]);
        }
        if(node["PartselectIndexed"]) {
            return convert_partselectindexed(node["PartselectIndexed"]);
        }
        if(node["PartselectPlusIndexed"]) {
            return convert_partselectplusindexed(node["PartselectPlusIndexed"]);
        }
        if(node["PartselectMinusIndexed"]) {
            return convert_partselectminusindexed(node["PartselectMinusIndexed"]);
        }
        if(node["Pointer"]) {
            return convert_pointer(node["Pointer"]);
        }
        if(node["Lvalue"]) {
            return convert_lvalue(node["Lvalue"]);
        }
        if(node["Rvalue"]) {
            return convert_rvalue(node["Rvalue"]);
        }
        if(node["UnaryOperator"]) {
            return convert_unaryoperator(node["UnaryOperator"]);
        }
        if(node["Uplus"]) {
            return convert_uplus(node["Uplus"]);
        }
        if(node["Uminus"]) {
            return convert_uminus(node["Uminus"]);
        }
        if(node["Ulnot"]) {
            return convert_ulnot(node["Ulnot"]);
        }
        if(node["Unot"]) {
            return convert_unot(node["Unot"]);
        }
        if(node["Uand"]) {
            return convert_uand(node["Uand"]);
        }
        if(node["Unand"]) {
            return convert_unand(node["Unand"]);
        }
        if(node["Uor"]) {
            return convert_uor(node["Uor"]);
        }
        if(node["Unor"]) {
            return convert_unor(node["Unor"]);
        }
        if(node["Uxor"]) {
            return convert_uxor(node["Uxor"]);
        }
        if(node["Uxnor"]) {
            return convert_uxnor(node["Uxnor"]);
        }
        if(node["Operator"]) {
            return convert_operator(node["Operator"]);
        }
        if(node["Power"]) {
            return convert_power(node["Power"]);
        }
        if(node["Times"]) {
            return convert_times(node["Times"]);
        }
        if(node["Divide"]) {
            return convert_divide(node["Divide"]);
        }
        if(node["Mod"]) {
            return convert_mod(node["Mod"]);
        }
        if(node["Plus"]) {
            return convert_plus(node["Plus"]);
        }
        if(node["Minus"]) {
            return convert_minus(node["Minus"]);
        }
        if(node["Sll"]) {
            return convert_sll(node["Sll"]);
        }
        if(node["Srl"]) {
            return convert_srl(node["Srl"]);
        }
        if(node["Sra"]) {
            return convert_sra(node["Sra"]);
        }
        if(node["LessThan"]) {
            return convert_lessthan(node["LessThan"]);
        }
        if(node["GreaterThan"]) {
            return convert_greaterthan(node["GreaterThan"]);
        }
        if(node["LessEq"]) {
            return convert_lesseq(node["LessEq"]);
        }
        if(node["GreaterEq"]) {
            return convert_greatereq(node["GreaterEq"]);
        }
        if(node["Eq"]) {
            return convert_eq(node["Eq"]);
        }
        if(node["NotEq"]) {
            return convert_noteq(node["NotEq"]);
        }
        if(node["Eql"]) {
            return convert_eql(node["Eql"]);
        }
        if(node["NotEql"]) {
            return convert_noteql(node["NotEql"]);
        }
        if(node["And"]) {
            return convert_and(node["And"]);
        }
        if(node["Xor"]) {
            return convert_xor(node["Xor"]);
        }
        if(node["Xnor"]) {
            return convert_xnor(node["Xnor"]);
        }
        if(node["Or"]) {
            return convert_or(node["Or"]);
        }
        if(node["Land"]) {
            return convert_land(node["Land"]);
        }
        if(node["Lor"]) {
            return convert_lor(node["Lor"]);
        }
        if(node["Cond"]) {
            return convert_cond(node["Cond"]);
        }
        if(node["Always"]) {
            return convert_always(node["Always"]);
        }
        if(node["AlwaysFF"]) {
            return convert_alwaysff(node["AlwaysFF"]);
        }
        if(node["AlwaysComb"]) {
            return convert_alwayscomb(node["AlwaysComb"]);
        }
        if(node["AlwaysLatch"]) {
            return convert_alwayslatch(node["AlwaysLatch"]);
        }
        if(node["Senslist"]) {
            return convert_senslist(node["Senslist"]);
        }
        if(node["Sens"]) {
            return convert_sens(node["Sens"]);
        }
        if(node["Defparamlist"]) {
            return convert_defparamlist(node["Defparamlist"]);
        }
        if(node["Defparam"]) {
            return convert_defparam(node["Defparam"]);
        }
        if(node["Assign"]) {
            return convert_assign(node["Assign"]);
        }
        if(node["BlockingSubstitution"]) {
            return convert_blockingsubstitution(node["BlockingSubstitution"]);
        }
        if(node["NonblockingSubstitution"]) {
            return convert_nonblockingsubstitution(node["NonblockingSubstitution"]);
        }
        if(node["IfStatement"]) {
            return convert_ifstatement(node["IfStatement"]);
        }
        if(node["RepeatStatement"]) {
            return convert_repeatstatement(node["RepeatStatement"]);
        }
        if(node["ForStatement"]) {
            return convert_forstatement(node["ForStatement"]);
        }
        if(node["WhileStatement"]) {
            return convert_whilestatement(node["WhileStatement"]);
        }
        if(node["CaseStatement"]) {
            return convert_casestatement(node["CaseStatement"]);
        }
        if(node["CasexStatement"]) {
            return convert_casexstatement(node["CasexStatement"]);
        }
        if(node["CasezStatement"]) {
            return convert_casezstatement(node["CasezStatement"]);
        }
        if(node["UniqueCaseStatement"]) {
            return convert_uniquecasestatement(node["UniqueCaseStatement"]);
        }
        if(node["PriorityCaseStatement"]) {
            return convert_prioritycasestatement(node["PriorityCaseStatement"]);
        }
        if(node["Case"]) {
            return convert_case(node["Case"]);
        }
        if(node["Block"]) {
            return convert_block(node["Block"]);
        }
        if(node["Initial"]) {
            return convert_initial(node["Initial"]);
        }
        if(node["EventStatement"]) {
            return convert_eventstatement(node["EventStatement"]);
        }
        if(node["WaitStatement"]) {
            return convert_waitstatement(node["WaitStatement"]);
        }
        if(node["ForeverStatement"]) {
            return convert_foreverstatement(node["ForeverStatement"]);
        }
        if(node["DelayStatement"]) {
            return convert_delaystatement(node["DelayStatement"]);
        }
        if(node["Instancelist"]) {
            return convert_instancelist(node["Instancelist"]);
        }
        if(node["Instance"]) {
            return convert_instance(node["Instance"]);
        }
        if(node["ParamArg"]) {
            return convert_paramarg(node["ParamArg"]);
        }
        if(node["PortArg"]) {
            return convert_portarg(node["PortArg"]);
        }
        if(node["Function"]) {
            return convert_function(node["Function"]);
        }
        if(node["FunctionCall"]) {
            return convert_functioncall(node["FunctionCall"]);
        }
        if(node["Task"]) {
            return convert_task(node["Task"]);
        }
        if(node["TaskCall"]) {
            return convert_taskcall(node["TaskCall"]);
        }
        if(node["GenerateStatement"]) {
            return convert_generatestatement(node["GenerateStatement"]);
        }
        if(node["SystemCall"]) {
            return convert_systemcall(node["SystemCall"]);
        }
        if(node["IdentifierScopeLabel"]) {
            return convert_identifierscopelabel(node["IdentifierScopeLabel"]);
        }
        if(node["IdentifierScope"]) {
            return convert_identifierscope(node["IdentifierScope"]);
        }
        if(node["Disable"]) {
            return convert_disable(node["Disable"]);
        }
        if(node["ParallelBlock"]) {
            return convert_parallelblock(node["ParallelBlock"]);
        }
        if(node["SingleStatement"]) {
            return convert_singlestatement(node["SingleStatement"]);
        }
    }

    return AST::Node::Ptr(nullptr);
}

AST::Node::Ptr YAMLImporter::convert_node(const YAML::Node node) const
{
    AST::Node::Ptr result(new AST::Node);
    return result;
}

AST::Node::Ptr YAMLImporter::convert_source(const YAML::Node node) const
{
    AST::Source::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Source>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Source>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child description
        if(node["description"]) {
            const YAML::Node node_description = node["description"];
            // Set the child
            AST::Node::Ptr child = convert(node_description);
            if(child) {
                AST::Description::Ptr child_cast = AST::cast_to<AST::Description>(child);
                if(!result) {
                    result = std::make_shared<AST::Source>();
                }
                result->set_description(child_cast);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Source>(result);
}

AST::Node::Ptr YAMLImporter::convert_description(const YAML::Node node) const
{
    AST::Description::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Description>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Description>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child definitions
        if(node["definitions"]) {
            const YAML::Node node_definitions = node["definitions"];
            // Fill the list of children
            AST::Node::ListPtr definitions_list(new AST::Node::List);
            if(node_definitions.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_definitions.begin();
                    it != node_definitions.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        definitions_list->push_back(child);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_definitions);
                if(child) {
                    definitions_list->push_back(child);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::Description>();
            }
            result->set_definitions(definitions_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::Description>(result);
}

AST::Node::Ptr YAMLImporter::convert_pragmalist(const YAML::Node node) const
{
    AST::Pragmalist::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Pragmalist>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Pragmalist>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child pragmas
        if(node["pragmas"]) {
            const YAML::Node node_pragmas = node["pragmas"];
            // Fill the list of children
            AST::Pragma::ListPtr pragmas_list(new AST::Pragma::List);
            if(node_pragmas.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_pragmas.begin(); it != node_pragmas.end();
                    ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Pragma::Ptr child_cast = AST::cast_to<AST::Pragma>(child);
                        pragmas_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_pragmas);
                if(child) {
                    AST::Pragma::Ptr child_cast = AST::cast_to<AST::Pragma>(child);
                    pragmas_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::Pragmalist>();
            }
            result->set_pragmas(pragmas_list);
        }

        // Manage Child statements
        if(node["statements"]) {
            const YAML::Node node_statements = node["statements"];
            // Fill the list of children
            AST::Node::ListPtr statements_list(new AST::Node::List);
            if(node_statements.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_statements.begin(); it != node_statements.end();
                    ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        statements_list->push_back(child);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_statements);
                if(child) {
                    statements_list->push_back(child);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::Pragmalist>();
            }
            result->set_statements(statements_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::Pragmalist>(result);
}

AST::Node::Ptr YAMLImporter::convert_pragma(const YAML::Node node) const
{
    AST::Pragma::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Pragma>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Pragma>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property name
        if(node["name"]) {
            if(node["name"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Pragma>();
                }
                result->set_name(node["name"].as<std::string>());
            }
        }

        // Manage Child expression
        if(node["expression"]) {
            const YAML::Node node_expression = node["expression"];
            // Set the child
            AST::Node::Ptr child = convert(node_expression);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Pragma>();
                }
                result->set_expression(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Pragma>(result);
}

AST::Node::Ptr YAMLImporter::convert_module(const YAML::Node node) const
{
    AST::Module::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Module>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Module>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property name
        if(node["name"]) {
            if(node["name"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Module>();
                }
                result->set_name(node["name"].as<std::string>());
            }
        }
        // Manage property lifetime
        if(node["lifetime"]) {
            if(node["lifetime"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Module>();
                }
                result->set_lifetime(node["lifetime"].as<AST::Module::LifetimeEnum>());
            }
        }
        // Manage property default_nettype
        if(node["default_nettype"]) {
            if(node["default_nettype"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Module>();
                }
                result->set_default_nettype(
                    node["default_nettype"].as<AST::Module::Default_nettypeEnum>());
            }
        }

        // Manage Child params
        if(node["params"]) {
            const YAML::Node node_params = node["params"];
            // Fill the list of children
            AST::Declaration::ListPtr params_list(new AST::Declaration::List);
            if(node_params.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_params.begin(); it != node_params.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Declaration::Ptr child_cast = AST::cast_to<AST::Declaration>(child);
                        params_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_params);
                if(child) {
                    AST::Declaration::Ptr child_cast = AST::cast_to<AST::Declaration>(child);
                    params_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::Module>();
            }
            result->set_params(params_list);
        }

        // Manage Child ports
        if(node["ports"]) {
            const YAML::Node node_ports = node["ports"];
            // Fill the list of children
            AST::Port::ListPtr ports_list(new AST::Port::List);
            if(node_ports.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_ports.begin(); it != node_ports.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Port::Ptr child_cast = AST::cast_to<AST::Port>(child);
                        ports_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_ports);
                if(child) {
                    AST::Port::Ptr child_cast = AST::cast_to<AST::Port>(child);
                    ports_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::Module>();
            }
            result->set_ports(ports_list);
        }

        // Manage Child items
        if(node["items"]) {
            const YAML::Node node_items = node["items"];
            // Fill the list of children
            AST::Node::ListPtr items_list(new AST::Node::List);
            if(node_items.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_items.begin(); it != node_items.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        items_list->push_back(child);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_items);
                if(child) {
                    items_list->push_back(child);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::Module>();
            }
            result->set_items(items_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::Module>(result);
}

AST::Node::Ptr YAMLImporter::convert_port(const YAML::Node node) const
{
    AST::Port::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Port>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Port>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property name
        if(node["name"]) {
            if(node["name"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Port>();
                }
                result->set_name(node["name"].as<std::string>());
            }
        }
        // Manage property direction
        if(node["direction"]) {
            if(node["direction"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Port>();
                }
                result->set_direction(node["direction"].as<AST::Port::DirectionEnum>());
            }
        }

        // Manage Child decl
        if(node["decl"]) {
            const YAML::Node node_decl = node["decl"];
            // Set the child
            AST::Node::Ptr child = convert(node_decl);
            if(child) {
                AST::Declaration::Ptr child_cast = AST::cast_to<AST::Declaration>(child);
                if(!result) {
                    result = std::make_shared<AST::Port>();
                }
                result->set_decl(child_cast);
            }
        }

        // Manage Child expr
        if(node["expr"]) {
            const YAML::Node node_expr = node["expr"];
            // Set the child
            AST::Node::Ptr child = convert(node_expr);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Port>();
                }
                result->set_expr(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Port>(result);
}

AST::Node::Ptr YAMLImporter::convert_package(const YAML::Node node) const
{
    AST::Package::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Package>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Package>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property name
        if(node["name"]) {
            if(node["name"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Package>();
                }
                result->set_name(node["name"].as<std::string>());
            }
        }
        // Manage property lifetime
        if(node["lifetime"]) {
            if(node["lifetime"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Package>();
                }
                result->set_lifetime(node["lifetime"].as<AST::Package::LifetimeEnum>());
            }
        }

        // Manage Child items
        if(node["items"]) {
            const YAML::Node node_items = node["items"];
            // Fill the list of children
            AST::Node::ListPtr items_list(new AST::Node::List);
            if(node_items.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_items.begin(); it != node_items.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        items_list->push_back(child);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_items);
                if(child) {
                    items_list->push_back(child);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::Package>();
            }
            result->set_items(items_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::Package>(result);
}

AST::Node::Ptr YAMLImporter::convert_import(const YAML::Node node) const
{
    AST::Import::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Import>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Import>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property package
        if(node["package"]) {
            if(node["package"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Import>();
                }
                result->set_package(node["package"].as<std::string>());
            }
        }
        // Manage property symbol
        if(node["symbol"]) {
            if(node["symbol"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Import>();
                }
                result->set_symbol(node["symbol"].as<std::string>());
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Import>(result);
}

AST::Node::Ptr YAMLImporter::convert_identifier(const YAML::Node node) const
{
    AST::Identifier::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Identifier>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Identifier>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property name
        if(node["name"]) {
            if(node["name"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Identifier>();
                }
                result->set_name(node["name"].as<std::string>());
            }
        }
        // Manage property package
        if(node["package"]) {
            if(node["package"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Identifier>();
                }
                result->set_package(node["package"].as<std::string>());
            }
        }

        // Manage Child scope
        if(node["scope"]) {
            const YAML::Node node_scope = node["scope"];
            // Set the child
            AST::Node::Ptr child = convert(node_scope);
            if(child) {
                AST::IdentifierScope::Ptr child_cast = AST::cast_to<AST::IdentifierScope>(child);
                if(!result) {
                    result = std::make_shared<AST::Identifier>();
                }
                result->set_scope(child_cast);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Identifier>(result);
}

AST::Node::Ptr YAMLImporter::convert_constant(const YAML::Node node) const
{
    AST::Constant::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Constant>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Constant>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Constant>(result);
}

AST::Node::Ptr YAMLImporter::convert_stringconst(const YAML::Node node) const
{
    AST::StringConst::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::StringConst>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::StringConst>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property value
        if(node["value"]) {
            if(node["value"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::StringConst>();
                }
                result->set_value(node["value"].as<std::string>());
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::StringConst>(result);
}

AST::Node::Ptr YAMLImporter::convert_intconst(const YAML::Node node) const
{
    AST::IntConst::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::IntConst>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::IntConst>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property value
        if(node["value"]) {
            if(node["value"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::IntConst>();
                }
                result->set_value(node["value"].as<std::string>());
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::IntConst>(result);
}

AST::Node::Ptr YAMLImporter::convert_intconstn(const YAML::Node node) const
{
    AST::IntConstN::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::IntConstN>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::IntConstN>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property base
        if(node["base"]) {
            if(node["base"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::IntConstN>();
                }
                result->set_base(node["base"].as<int>());
            }
        }
        // Manage property size
        if(node["size"]) {
            if(node["size"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::IntConstN>();
                }
                result->set_size(node["size"].as<int>());
            }
        }
        // Manage property sign
        if(node["sign"]) {
            if(node["sign"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::IntConstN>();
                }
                result->set_sign(node["sign"].as<bool>());
            }
        }
        // Manage property value
        if(node["value"]) {
            if(node["value"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::IntConstN>();
                }
                result->set_value(node["value"].as<mpz_class>());
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::IntConstN>(result);
}

AST::Node::Ptr YAMLImporter::convert_floatconst(const YAML::Node node) const
{
    AST::FloatConst::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::FloatConst>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::FloatConst>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property value
        if(node["value"]) {
            if(node["value"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::FloatConst>();
                }
                result->set_value(node["value"].as<double>());
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::FloatConst>(result);
}

AST::Node::Ptr YAMLImporter::convert_datatype(const YAML::Node node) const
{
    AST::DataType::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::DataType>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::DataType>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property signing
        if(node["signing"]) {
            if(node["signing"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::DataType>();
                }
                result->set_signing(node["signing"].as<AST::DataType::SigningEnum>());
            }
        }

        // Manage Child packed_dims
        if(node["packed_dims"]) {
            const YAML::Node node_packed_dims = node["packed_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr packed_dims_list(new AST::Dimension::List);
            if(node_packed_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_packed_dims.begin();
                    it != node_packed_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        packed_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_packed_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    packed_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::DataType>();
            }
            result->set_packed_dims(packed_dims_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::DataType>(result);
}

AST::Node::Ptr YAMLImporter::convert_logictype(const YAML::Node node) const
{
    AST::LogicType::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::LogicType>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::LogicType>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property signing
        if(node["signing"]) {
            if(node["signing"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::LogicType>();
                }
                result->set_signing(node["signing"].as<AST::DataType::SigningEnum>());
            }
        }

        // Manage Child packed_dims
        if(node["packed_dims"]) {
            const YAML::Node node_packed_dims = node["packed_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr packed_dims_list(new AST::Dimension::List);
            if(node_packed_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_packed_dims.begin();
                    it != node_packed_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        packed_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_packed_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    packed_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::LogicType>();
            }
            result->set_packed_dims(packed_dims_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::LogicType>(result);
}

AST::Node::Ptr YAMLImporter::convert_regtype(const YAML::Node node) const
{
    AST::RegType::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::RegType>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::RegType>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property signing
        if(node["signing"]) {
            if(node["signing"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::RegType>();
                }
                result->set_signing(node["signing"].as<AST::DataType::SigningEnum>());
            }
        }

        // Manage Child packed_dims
        if(node["packed_dims"]) {
            const YAML::Node node_packed_dims = node["packed_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr packed_dims_list(new AST::Dimension::List);
            if(node_packed_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_packed_dims.begin();
                    it != node_packed_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        packed_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_packed_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    packed_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::RegType>();
            }
            result->set_packed_dims(packed_dims_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::RegType>(result);
}

AST::Node::Ptr YAMLImporter::convert_bittype(const YAML::Node node) const
{
    AST::BitType::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::BitType>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::BitType>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property signing
        if(node["signing"]) {
            if(node["signing"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::BitType>();
                }
                result->set_signing(node["signing"].as<AST::DataType::SigningEnum>());
            }
        }

        // Manage Child packed_dims
        if(node["packed_dims"]) {
            const YAML::Node node_packed_dims = node["packed_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr packed_dims_list(new AST::Dimension::List);
            if(node_packed_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_packed_dims.begin();
                    it != node_packed_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        packed_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_packed_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    packed_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::BitType>();
            }
            result->set_packed_dims(packed_dims_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::BitType>(result);
}

AST::Node::Ptr YAMLImporter::convert_bytetype(const YAML::Node node) const
{
    AST::ByteType::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::ByteType>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::ByteType>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property signing
        if(node["signing"]) {
            if(node["signing"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::ByteType>();
                }
                result->set_signing(node["signing"].as<AST::DataType::SigningEnum>());
            }
        }

        // Manage Child packed_dims
        if(node["packed_dims"]) {
            const YAML::Node node_packed_dims = node["packed_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr packed_dims_list(new AST::Dimension::List);
            if(node_packed_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_packed_dims.begin();
                    it != node_packed_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        packed_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_packed_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    packed_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::ByteType>();
            }
            result->set_packed_dims(packed_dims_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::ByteType>(result);
}

AST::Node::Ptr YAMLImporter::convert_shortinttype(const YAML::Node node) const
{
    AST::ShortintType::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::ShortintType>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::ShortintType>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property signing
        if(node["signing"]) {
            if(node["signing"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::ShortintType>();
                }
                result->set_signing(node["signing"].as<AST::DataType::SigningEnum>());
            }
        }

        // Manage Child packed_dims
        if(node["packed_dims"]) {
            const YAML::Node node_packed_dims = node["packed_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr packed_dims_list(new AST::Dimension::List);
            if(node_packed_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_packed_dims.begin();
                    it != node_packed_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        packed_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_packed_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    packed_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::ShortintType>();
            }
            result->set_packed_dims(packed_dims_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::ShortintType>(result);
}

AST::Node::Ptr YAMLImporter::convert_inttype(const YAML::Node node) const
{
    AST::IntType::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::IntType>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::IntType>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property signing
        if(node["signing"]) {
            if(node["signing"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::IntType>();
                }
                result->set_signing(node["signing"].as<AST::DataType::SigningEnum>());
            }
        }

        // Manage Child packed_dims
        if(node["packed_dims"]) {
            const YAML::Node node_packed_dims = node["packed_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr packed_dims_list(new AST::Dimension::List);
            if(node_packed_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_packed_dims.begin();
                    it != node_packed_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        packed_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_packed_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    packed_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::IntType>();
            }
            result->set_packed_dims(packed_dims_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::IntType>(result);
}

AST::Node::Ptr YAMLImporter::convert_longinttype(const YAML::Node node) const
{
    AST::LongintType::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::LongintType>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::LongintType>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property signing
        if(node["signing"]) {
            if(node["signing"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::LongintType>();
                }
                result->set_signing(node["signing"].as<AST::DataType::SigningEnum>());
            }
        }

        // Manage Child packed_dims
        if(node["packed_dims"]) {
            const YAML::Node node_packed_dims = node["packed_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr packed_dims_list(new AST::Dimension::List);
            if(node_packed_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_packed_dims.begin();
                    it != node_packed_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        packed_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_packed_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    packed_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::LongintType>();
            }
            result->set_packed_dims(packed_dims_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::LongintType>(result);
}

AST::Node::Ptr YAMLImporter::convert_integertype(const YAML::Node node) const
{
    AST::IntegerType::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::IntegerType>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::IntegerType>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property signing
        if(node["signing"]) {
            if(node["signing"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::IntegerType>();
                }
                result->set_signing(node["signing"].as<AST::DataType::SigningEnum>());
            }
        }

        // Manage Child packed_dims
        if(node["packed_dims"]) {
            const YAML::Node node_packed_dims = node["packed_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr packed_dims_list(new AST::Dimension::List);
            if(node_packed_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_packed_dims.begin();
                    it != node_packed_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        packed_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_packed_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    packed_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::IntegerType>();
            }
            result->set_packed_dims(packed_dims_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::IntegerType>(result);
}

AST::Node::Ptr YAMLImporter::convert_timetype(const YAML::Node node) const
{
    AST::TimeType::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::TimeType>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::TimeType>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property signing
        if(node["signing"]) {
            if(node["signing"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::TimeType>();
                }
                result->set_signing(node["signing"].as<AST::DataType::SigningEnum>());
            }
        }

        // Manage Child packed_dims
        if(node["packed_dims"]) {
            const YAML::Node node_packed_dims = node["packed_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr packed_dims_list(new AST::Dimension::List);
            if(node_packed_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_packed_dims.begin();
                    it != node_packed_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        packed_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_packed_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    packed_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::TimeType>();
            }
            result->set_packed_dims(packed_dims_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::TimeType>(result);
}

AST::Node::Ptr YAMLImporter::convert_realtype(const YAML::Node node) const
{
    AST::RealType::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::RealType>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::RealType>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property signing
        if(node["signing"]) {
            if(node["signing"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::RealType>();
                }
                result->set_signing(node["signing"].as<AST::DataType::SigningEnum>());
            }
        }

        // Manage Child packed_dims
        if(node["packed_dims"]) {
            const YAML::Node node_packed_dims = node["packed_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr packed_dims_list(new AST::Dimension::List);
            if(node_packed_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_packed_dims.begin();
                    it != node_packed_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        packed_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_packed_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    packed_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::RealType>();
            }
            result->set_packed_dims(packed_dims_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::RealType>(result);
}

AST::Node::Ptr YAMLImporter::convert_shortrealtype(const YAML::Node node) const
{
    AST::ShortrealType::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::ShortrealType>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::ShortrealType>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property signing
        if(node["signing"]) {
            if(node["signing"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::ShortrealType>();
                }
                result->set_signing(node["signing"].as<AST::DataType::SigningEnum>());
            }
        }

        // Manage Child packed_dims
        if(node["packed_dims"]) {
            const YAML::Node node_packed_dims = node["packed_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr packed_dims_list(new AST::Dimension::List);
            if(node_packed_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_packed_dims.begin();
                    it != node_packed_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        packed_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_packed_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    packed_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::ShortrealType>();
            }
            result->set_packed_dims(packed_dims_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::ShortrealType>(result);
}

AST::Node::Ptr YAMLImporter::convert_realtimetype(const YAML::Node node) const
{
    AST::RealtimeType::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::RealtimeType>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::RealtimeType>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property signing
        if(node["signing"]) {
            if(node["signing"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::RealtimeType>();
                }
                result->set_signing(node["signing"].as<AST::DataType::SigningEnum>());
            }
        }

        // Manage Child packed_dims
        if(node["packed_dims"]) {
            const YAML::Node node_packed_dims = node["packed_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr packed_dims_list(new AST::Dimension::List);
            if(node_packed_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_packed_dims.begin();
                    it != node_packed_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        packed_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_packed_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    packed_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::RealtimeType>();
            }
            result->set_packed_dims(packed_dims_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::RealtimeType>(result);
}

AST::Node::Ptr YAMLImporter::convert_stringtype(const YAML::Node node) const
{
    AST::StringType::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::StringType>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::StringType>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property signing
        if(node["signing"]) {
            if(node["signing"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::StringType>();
                }
                result->set_signing(node["signing"].as<AST::DataType::SigningEnum>());
            }
        }

        // Manage Child packed_dims
        if(node["packed_dims"]) {
            const YAML::Node node_packed_dims = node["packed_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr packed_dims_list(new AST::Dimension::List);
            if(node_packed_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_packed_dims.begin();
                    it != node_packed_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        packed_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_packed_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    packed_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::StringType>();
            }
            result->set_packed_dims(packed_dims_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::StringType>(result);
}

AST::Node::Ptr YAMLImporter::convert_chandletype(const YAML::Node node) const
{
    AST::ChandleType::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::ChandleType>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::ChandleType>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property signing
        if(node["signing"]) {
            if(node["signing"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::ChandleType>();
                }
                result->set_signing(node["signing"].as<AST::DataType::SigningEnum>());
            }
        }

        // Manage Child packed_dims
        if(node["packed_dims"]) {
            const YAML::Node node_packed_dims = node["packed_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr packed_dims_list(new AST::Dimension::List);
            if(node_packed_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_packed_dims.begin();
                    it != node_packed_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        packed_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_packed_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    packed_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::ChandleType>();
            }
            result->set_packed_dims(packed_dims_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::ChandleType>(result);
}

AST::Node::Ptr YAMLImporter::convert_eventtype(const YAML::Node node) const
{
    AST::EventType::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::EventType>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::EventType>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property signing
        if(node["signing"]) {
            if(node["signing"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::EventType>();
                }
                result->set_signing(node["signing"].as<AST::DataType::SigningEnum>());
            }
        }

        // Manage Child packed_dims
        if(node["packed_dims"]) {
            const YAML::Node node_packed_dims = node["packed_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr packed_dims_list(new AST::Dimension::List);
            if(node_packed_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_packed_dims.begin();
                    it != node_packed_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        packed_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_packed_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    packed_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::EventType>();
            }
            result->set_packed_dims(packed_dims_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::EventType>(result);
}

AST::Node::Ptr YAMLImporter::convert_implicittype(const YAML::Node node) const
{
    AST::ImplicitType::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::ImplicitType>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::ImplicitType>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property signing
        if(node["signing"]) {
            if(node["signing"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::ImplicitType>();
                }
                result->set_signing(node["signing"].as<AST::DataType::SigningEnum>());
            }
        }

        // Manage Child packed_dims
        if(node["packed_dims"]) {
            const YAML::Node node_packed_dims = node["packed_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr packed_dims_list(new AST::Dimension::List);
            if(node_packed_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_packed_dims.begin();
                    it != node_packed_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        packed_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_packed_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    packed_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::ImplicitType>();
            }
            result->set_packed_dims(packed_dims_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::ImplicitType>(result);
}

AST::Node::Ptr YAMLImporter::convert_voidtype(const YAML::Node node) const
{
    AST::VoidType::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::VoidType>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::VoidType>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property signing
        if(node["signing"]) {
            if(node["signing"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::VoidType>();
                }
                result->set_signing(node["signing"].as<AST::DataType::SigningEnum>());
            }
        }

        // Manage Child packed_dims
        if(node["packed_dims"]) {
            const YAML::Node node_packed_dims = node["packed_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr packed_dims_list(new AST::Dimension::List);
            if(node_packed_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_packed_dims.begin();
                    it != node_packed_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        packed_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_packed_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    packed_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::VoidType>();
            }
            result->set_packed_dims(packed_dims_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::VoidType>(result);
}

AST::Node::Ptr YAMLImporter::convert_namedtype(const YAML::Node node) const
{
    AST::NamedType::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::NamedType>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::NamedType>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property name
        if(node["name"]) {
            if(node["name"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::NamedType>();
                }
                result->set_name(node["name"].as<std::string>());
            }
        }
        // Manage property signing
        if(node["signing"]) {
            if(node["signing"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::NamedType>();
                }
                result->set_signing(node["signing"].as<AST::DataType::SigningEnum>());
            }
        }

        // Manage Child scope
        if(node["scope"]) {
            const YAML::Node node_scope = node["scope"];
            // Fill the list of children
            AST::ScopeName::ListPtr scope_list(new AST::ScopeName::List);
            if(node_scope.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_scope.begin(); it != node_scope.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::ScopeName::Ptr child_cast = AST::cast_to<AST::ScopeName>(child);
                        scope_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_scope);
                if(child) {
                    AST::ScopeName::Ptr child_cast = AST::cast_to<AST::ScopeName>(child);
                    scope_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::NamedType>();
            }
            result->set_scope(scope_list);
        }

        // Manage Child packed_dims
        if(node["packed_dims"]) {
            const YAML::Node node_packed_dims = node["packed_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr packed_dims_list(new AST::Dimension::List);
            if(node_packed_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_packed_dims.begin();
                    it != node_packed_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        packed_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_packed_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    packed_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::NamedType>();
            }
            result->set_packed_dims(packed_dims_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::NamedType>(result);
}

AST::Node::Ptr YAMLImporter::convert_scopename(const YAML::Node node) const
{
    AST::ScopeName::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::ScopeName>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::ScopeName>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property name
        if(node["name"]) {
            if(node["name"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::ScopeName>();
                }
                result->set_name(node["name"].as<std::string>());
            }
        }

        // Manage Child params
        if(node["params"]) {
            const YAML::Node node_params = node["params"];
            // Fill the list of children
            AST::ParamArg::ListPtr params_list(new AST::ParamArg::List);
            if(node_params.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_params.begin(); it != node_params.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::ParamArg::Ptr child_cast = AST::cast_to<AST::ParamArg>(child);
                        params_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_params);
                if(child) {
                    AST::ParamArg::Ptr child_cast = AST::cast_to<AST::ParamArg>(child);
                    params_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::ScopeName>();
            }
            result->set_params(params_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::ScopeName>(result);
}

AST::Node::Ptr YAMLImporter::convert_structtype(const YAML::Node node) const
{
    AST::StructType::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::StructType>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::StructType>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property is_packed
        if(node["is_packed"]) {
            if(node["is_packed"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::StructType>();
                }
                result->set_is_packed(node["is_packed"].as<bool>());
            }
        }
        // Manage property signing
        if(node["signing"]) {
            if(node["signing"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::StructType>();
                }
                result->set_signing(node["signing"].as<AST::DataType::SigningEnum>());
            }
        }

        // Manage Child members
        if(node["members"]) {
            const YAML::Node node_members = node["members"];
            // Fill the list of children
            AST::Member::ListPtr members_list(new AST::Member::List);
            if(node_members.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_members.begin(); it != node_members.end();
                    ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Member::Ptr child_cast = AST::cast_to<AST::Member>(child);
                        members_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_members);
                if(child) {
                    AST::Member::Ptr child_cast = AST::cast_to<AST::Member>(child);
                    members_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::StructType>();
            }
            result->set_members(members_list);
        }

        // Manage Child packed_dims
        if(node["packed_dims"]) {
            const YAML::Node node_packed_dims = node["packed_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr packed_dims_list(new AST::Dimension::List);
            if(node_packed_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_packed_dims.begin();
                    it != node_packed_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        packed_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_packed_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    packed_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::StructType>();
            }
            result->set_packed_dims(packed_dims_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::StructType>(result);
}

AST::Node::Ptr YAMLImporter::convert_uniontype(const YAML::Node node) const
{
    AST::UnionType::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::UnionType>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::UnionType>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property is_packed
        if(node["is_packed"]) {
            if(node["is_packed"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::UnionType>();
                }
                result->set_is_packed(node["is_packed"].as<bool>());
            }
        }
        // Manage property is_tagged
        if(node["is_tagged"]) {
            if(node["is_tagged"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::UnionType>();
                }
                result->set_is_tagged(node["is_tagged"].as<bool>());
            }
        }
        // Manage property signing
        if(node["signing"]) {
            if(node["signing"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::UnionType>();
                }
                result->set_signing(node["signing"].as<AST::DataType::SigningEnum>());
            }
        }

        // Manage Child members
        if(node["members"]) {
            const YAML::Node node_members = node["members"];
            // Fill the list of children
            AST::Member::ListPtr members_list(new AST::Member::List);
            if(node_members.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_members.begin(); it != node_members.end();
                    ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Member::Ptr child_cast = AST::cast_to<AST::Member>(child);
                        members_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_members);
                if(child) {
                    AST::Member::Ptr child_cast = AST::cast_to<AST::Member>(child);
                    members_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::UnionType>();
            }
            result->set_members(members_list);
        }

        // Manage Child packed_dims
        if(node["packed_dims"]) {
            const YAML::Node node_packed_dims = node["packed_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr packed_dims_list(new AST::Dimension::List);
            if(node_packed_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_packed_dims.begin();
                    it != node_packed_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        packed_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_packed_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    packed_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::UnionType>();
            }
            result->set_packed_dims(packed_dims_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::UnionType>(result);
}

AST::Node::Ptr YAMLImporter::convert_enumtype(const YAML::Node node) const
{
    AST::EnumType::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::EnumType>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::EnumType>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property signing
        if(node["signing"]) {
            if(node["signing"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::EnumType>();
                }
                result->set_signing(node["signing"].as<AST::DataType::SigningEnum>());
            }
        }

        // Manage Child base
        if(node["base"]) {
            const YAML::Node node_base = node["base"];
            // Set the child
            AST::Node::Ptr child = convert(node_base);
            if(child) {
                AST::DataType::Ptr child_cast = AST::cast_to<AST::DataType>(child);
                if(!result) {
                    result = std::make_shared<AST::EnumType>();
                }
                result->set_base(child_cast);
            }
        }

        // Manage Child items
        if(node["items"]) {
            const YAML::Node node_items = node["items"];
            // Fill the list of children
            AST::EnumItem::ListPtr items_list(new AST::EnumItem::List);
            if(node_items.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_items.begin(); it != node_items.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::EnumItem::Ptr child_cast = AST::cast_to<AST::EnumItem>(child);
                        items_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_items);
                if(child) {
                    AST::EnumItem::Ptr child_cast = AST::cast_to<AST::EnumItem>(child);
                    items_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::EnumType>();
            }
            result->set_items(items_list);
        }

        // Manage Child packed_dims
        if(node["packed_dims"]) {
            const YAML::Node node_packed_dims = node["packed_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr packed_dims_list(new AST::Dimension::List);
            if(node_packed_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_packed_dims.begin();
                    it != node_packed_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        packed_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_packed_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    packed_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::EnumType>();
            }
            result->set_packed_dims(packed_dims_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::EnumType>(result);
}

AST::Node::Ptr YAMLImporter::convert_enumitem(const YAML::Node node) const
{
    AST::EnumItem::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::EnumItem>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::EnumItem>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property name
        if(node["name"]) {
            if(node["name"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::EnumItem>();
                }
                result->set_name(node["name"].as<std::string>());
            }
        }

        // Manage Child value
        if(node["value"]) {
            const YAML::Node node_value = node["value"];
            // Set the child
            AST::Node::Ptr child = convert(node_value);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::EnumItem>();
                }
                result->set_value(child);
            }
        }

        // Manage Child range
        if(node["range"]) {
            const YAML::Node node_range = node["range"];
            // Set the child
            AST::Node::Ptr child = convert(node_range);
            if(child) {
                AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                if(!result) {
                    result = std::make_shared<AST::EnumItem>();
                }
                result->set_range(child_cast);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::EnumItem>(result);
}

AST::Node::Ptr YAMLImporter::convert_typeopexpr(const YAML::Node node) const
{
    AST::TypeOpExpr::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::TypeOpExpr>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::TypeOpExpr>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property signing
        if(node["signing"]) {
            if(node["signing"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::TypeOpExpr>();
                }
                result->set_signing(node["signing"].as<AST::DataType::SigningEnum>());
            }
        }

        // Manage Child expr
        if(node["expr"]) {
            const YAML::Node node_expr = node["expr"];
            // Set the child
            AST::Node::Ptr child = convert(node_expr);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::TypeOpExpr>();
                }
                result->set_expr(child);
            }
        }

        // Manage Child packed_dims
        if(node["packed_dims"]) {
            const YAML::Node node_packed_dims = node["packed_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr packed_dims_list(new AST::Dimension::List);
            if(node_packed_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_packed_dims.begin();
                    it != node_packed_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        packed_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_packed_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    packed_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::TypeOpExpr>();
            }
            result->set_packed_dims(packed_dims_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::TypeOpExpr>(result);
}

AST::Node::Ptr YAMLImporter::convert_typeoptype(const YAML::Node node) const
{
    AST::TypeOpType::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::TypeOpType>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::TypeOpType>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property signing
        if(node["signing"]) {
            if(node["signing"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::TypeOpType>();
                }
                result->set_signing(node["signing"].as<AST::DataType::SigningEnum>());
            }
        }

        // Manage Child arg_type
        if(node["arg_type"]) {
            const YAML::Node node_arg_type = node["arg_type"];
            // Set the child
            AST::Node::Ptr child = convert(node_arg_type);
            if(child) {
                AST::DataType::Ptr child_cast = AST::cast_to<AST::DataType>(child);
                if(!result) {
                    result = std::make_shared<AST::TypeOpType>();
                }
                result->set_arg_type(child_cast);
            }
        }

        // Manage Child packed_dims
        if(node["packed_dims"]) {
            const YAML::Node node_packed_dims = node["packed_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr packed_dims_list(new AST::Dimension::List);
            if(node_packed_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_packed_dims.begin();
                    it != node_packed_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        packed_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_packed_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    packed_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::TypeOpType>();
            }
            result->set_packed_dims(packed_dims_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::TypeOpType>(result);
}

AST::Node::Ptr YAMLImporter::convert_dimension(const YAML::Node node) const
{
    AST::Dimension::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Dimension>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Dimension>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Dimension>(result);
}

AST::Node::Ptr YAMLImporter::convert_rangedim(const YAML::Node node) const
{
    AST::RangeDim::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::RangeDim>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::RangeDim>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child left
        if(node["left"]) {
            const YAML::Node node_left = node["left"];
            // Set the child
            AST::Node::Ptr child = convert(node_left);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::RangeDim>();
                }
                result->set_left(child);
            }
        }

        // Manage Child right
        if(node["right"]) {
            const YAML::Node node_right = node["right"];
            // Set the child
            AST::Node::Ptr child = convert(node_right);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::RangeDim>();
                }
                result->set_right(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::RangeDim>(result);
}

AST::Node::Ptr YAMLImporter::convert_sizedim(const YAML::Node node) const
{
    AST::SizeDim::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::SizeDim>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::SizeDim>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child size
        if(node["size"]) {
            const YAML::Node node_size = node["size"];
            // Set the child
            AST::Node::Ptr child = convert(node_size);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::SizeDim>();
                }
                result->set_size(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::SizeDim>(result);
}

AST::Node::Ptr YAMLImporter::convert_unsizeddim(const YAML::Node node) const
{
    AST::UnsizedDim::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::UnsizedDim>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::UnsizedDim>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::UnsizedDim>(result);
}

AST::Node::Ptr YAMLImporter::convert_queuedim(const YAML::Node node) const
{
    AST::QueueDim::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::QueueDim>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::QueueDim>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child bound
        if(node["bound"]) {
            const YAML::Node node_bound = node["bound"];
            // Set the child
            AST::Node::Ptr child = convert(node_bound);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::QueueDim>();
                }
                result->set_bound(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::QueueDim>(result);
}

AST::Node::Ptr YAMLImporter::convert_assocdim(const YAML::Node node) const
{
    AST::AssocDim::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::AssocDim>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::AssocDim>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child index_type
        if(node["index_type"]) {
            const YAML::Node node_index_type = node["index_type"];
            // Set the child
            AST::Node::Ptr child = convert(node_index_type);
            if(child) {
                AST::DataType::Ptr child_cast = AST::cast_to<AST::DataType>(child);
                if(!result) {
                    result = std::make_shared<AST::AssocDim>();
                }
                result->set_index_type(child_cast);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::AssocDim>(result);
}

AST::Node::Ptr YAMLImporter::convert_declaration(const YAML::Node node) const
{
    AST::Declaration::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Declaration>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Declaration>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property name
        if(node["name"]) {
            if(node["name"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Declaration>();
                }
                result->set_name(node["name"].as<std::string>());
            }
        }

        // Manage Child type
        if(node["type"]) {
            const YAML::Node node_type = node["type"];
            // Set the child
            AST::Node::Ptr child = convert(node_type);
            if(child) {
                AST::DataType::Ptr child_cast = AST::cast_to<AST::DataType>(child);
                if(!result) {
                    result = std::make_shared<AST::Declaration>();
                }
                result->set_type(child_cast);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Declaration>(result);
}

AST::Node::Ptr YAMLImporter::convert_var(const YAML::Node node) const
{
    AST::Var::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Var>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Var>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property is_var
        if(node["is_var"]) {
            if(node["is_var"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Var>();
                }
                result->set_is_var(node["is_var"].as<bool>());
            }
        }
        // Manage property is_const
        if(node["is_const"]) {
            if(node["is_const"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Var>();
                }
                result->set_is_const(node["is_const"].as<bool>());
            }
        }
        // Manage property lifetime
        if(node["lifetime"]) {
            if(node["lifetime"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Var>();
                }
                result->set_lifetime(node["lifetime"].as<AST::Var::LifetimeEnum>());
            }
        }
        // Manage property name
        if(node["name"]) {
            if(node["name"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Var>();
                }
                result->set_name(node["name"].as<std::string>());
            }
        }

        // Manage Child unpacked_dims
        if(node["unpacked_dims"]) {
            const YAML::Node node_unpacked_dims = node["unpacked_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr unpacked_dims_list(new AST::Dimension::List);
            if(node_unpacked_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_unpacked_dims.begin();
                    it != node_unpacked_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        unpacked_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_unpacked_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    unpacked_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::Var>();
            }
            result->set_unpacked_dims(unpacked_dims_list);
        }

        // Manage Child init
        if(node["init"]) {
            const YAML::Node node_init = node["init"];
            // Set the child
            AST::Node::Ptr child = convert(node_init);
            if(child) {
                AST::Rvalue::Ptr child_cast = AST::cast_to<AST::Rvalue>(child);
                if(!result) {
                    result = std::make_shared<AST::Var>();
                }
                result->set_init(child_cast);
            }
        }

        // Manage Child type
        if(node["type"]) {
            const YAML::Node node_type = node["type"];
            // Set the child
            AST::Node::Ptr child = convert(node_type);
            if(child) {
                AST::DataType::Ptr child_cast = AST::cast_to<AST::DataType>(child);
                if(!result) {
                    result = std::make_shared<AST::Var>();
                }
                result->set_type(child_cast);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Var>(result);
}

AST::Node::Ptr YAMLImporter::convert_net(const YAML::Node node) const
{
    AST::Net::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Net>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Net>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property is_vectored
        if(node["is_vectored"]) {
            if(node["is_vectored"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Net>();
                }
                result->set_is_vectored(node["is_vectored"].as<bool>());
            }
        }
        // Manage property is_scalared
        if(node["is_scalared"]) {
            if(node["is_scalared"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Net>();
                }
                result->set_is_scalared(node["is_scalared"].as<bool>());
            }
        }
        // Manage property name
        if(node["name"]) {
            if(node["name"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Net>();
                }
                result->set_name(node["name"].as<std::string>());
            }
        }

        // Manage Child unpacked_dims
        if(node["unpacked_dims"]) {
            const YAML::Node node_unpacked_dims = node["unpacked_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr unpacked_dims_list(new AST::Dimension::List);
            if(node_unpacked_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_unpacked_dims.begin();
                    it != node_unpacked_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        unpacked_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_unpacked_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    unpacked_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::Net>();
            }
            result->set_unpacked_dims(unpacked_dims_list);
        }

        // Manage Child cont_assign
        if(node["cont_assign"]) {
            const YAML::Node node_cont_assign = node["cont_assign"];
            // Set the child
            AST::Node::Ptr child = convert(node_cont_assign);
            if(child) {
                AST::Rvalue::Ptr child_cast = AST::cast_to<AST::Rvalue>(child);
                if(!result) {
                    result = std::make_shared<AST::Net>();
                }
                result->set_cont_assign(child_cast);
            }
        }

        // Manage Child strength
        if(node["strength"]) {
            const YAML::Node node_strength = node["strength"];
            // Set the child
            AST::Node::Ptr child = convert(node_strength);
            if(child) {
                AST::Strength::Ptr child_cast = AST::cast_to<AST::Strength>(child);
                if(!result) {
                    result = std::make_shared<AST::Net>();
                }
                result->set_strength(child_cast);
            }
        }

        // Manage Child ldelay
        if(node["ldelay"]) {
            const YAML::Node node_ldelay = node["ldelay"];
            // Set the child
            AST::Node::Ptr child = convert(node_ldelay);
            if(child) {
                AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
                if(!result) {
                    result = std::make_shared<AST::Net>();
                }
                result->set_ldelay(child_cast);
            }
        }

        // Manage Child rdelay
        if(node["rdelay"]) {
            const YAML::Node node_rdelay = node["rdelay"];
            // Set the child
            AST::Node::Ptr child = convert(node_rdelay);
            if(child) {
                AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
                if(!result) {
                    result = std::make_shared<AST::Net>();
                }
                result->set_rdelay(child_cast);
            }
        }

        // Manage Child type
        if(node["type"]) {
            const YAML::Node node_type = node["type"];
            // Set the child
            AST::Node::Ptr child = convert(node_type);
            if(child) {
                AST::DataType::Ptr child_cast = AST::cast_to<AST::DataType>(child);
                if(!result) {
                    result = std::make_shared<AST::Net>();
                }
                result->set_type(child_cast);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Net>(result);
}

AST::Node::Ptr YAMLImporter::convert_wirenet(const YAML::Node node) const
{
    AST::WireNet::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::WireNet>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::WireNet>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property is_vectored
        if(node["is_vectored"]) {
            if(node["is_vectored"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::WireNet>();
                }
                result->set_is_vectored(node["is_vectored"].as<bool>());
            }
        }
        // Manage property is_scalared
        if(node["is_scalared"]) {
            if(node["is_scalared"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::WireNet>();
                }
                result->set_is_scalared(node["is_scalared"].as<bool>());
            }
        }
        // Manage property name
        if(node["name"]) {
            if(node["name"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::WireNet>();
                }
                result->set_name(node["name"].as<std::string>());
            }
        }

        // Manage Child unpacked_dims
        if(node["unpacked_dims"]) {
            const YAML::Node node_unpacked_dims = node["unpacked_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr unpacked_dims_list(new AST::Dimension::List);
            if(node_unpacked_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_unpacked_dims.begin();
                    it != node_unpacked_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        unpacked_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_unpacked_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    unpacked_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::WireNet>();
            }
            result->set_unpacked_dims(unpacked_dims_list);
        }

        // Manage Child cont_assign
        if(node["cont_assign"]) {
            const YAML::Node node_cont_assign = node["cont_assign"];
            // Set the child
            AST::Node::Ptr child = convert(node_cont_assign);
            if(child) {
                AST::Rvalue::Ptr child_cast = AST::cast_to<AST::Rvalue>(child);
                if(!result) {
                    result = std::make_shared<AST::WireNet>();
                }
                result->set_cont_assign(child_cast);
            }
        }

        // Manage Child strength
        if(node["strength"]) {
            const YAML::Node node_strength = node["strength"];
            // Set the child
            AST::Node::Ptr child = convert(node_strength);
            if(child) {
                AST::Strength::Ptr child_cast = AST::cast_to<AST::Strength>(child);
                if(!result) {
                    result = std::make_shared<AST::WireNet>();
                }
                result->set_strength(child_cast);
            }
        }

        // Manage Child ldelay
        if(node["ldelay"]) {
            const YAML::Node node_ldelay = node["ldelay"];
            // Set the child
            AST::Node::Ptr child = convert(node_ldelay);
            if(child) {
                AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
                if(!result) {
                    result = std::make_shared<AST::WireNet>();
                }
                result->set_ldelay(child_cast);
            }
        }

        // Manage Child rdelay
        if(node["rdelay"]) {
            const YAML::Node node_rdelay = node["rdelay"];
            // Set the child
            AST::Node::Ptr child = convert(node_rdelay);
            if(child) {
                AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
                if(!result) {
                    result = std::make_shared<AST::WireNet>();
                }
                result->set_rdelay(child_cast);
            }
        }

        // Manage Child type
        if(node["type"]) {
            const YAML::Node node_type = node["type"];
            // Set the child
            AST::Node::Ptr child = convert(node_type);
            if(child) {
                AST::DataType::Ptr child_cast = AST::cast_to<AST::DataType>(child);
                if(!result) {
                    result = std::make_shared<AST::WireNet>();
                }
                result->set_type(child_cast);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::WireNet>(result);
}

AST::Node::Ptr YAMLImporter::convert_trinet(const YAML::Node node) const
{
    AST::TriNet::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::TriNet>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::TriNet>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property is_vectored
        if(node["is_vectored"]) {
            if(node["is_vectored"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::TriNet>();
                }
                result->set_is_vectored(node["is_vectored"].as<bool>());
            }
        }
        // Manage property is_scalared
        if(node["is_scalared"]) {
            if(node["is_scalared"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::TriNet>();
                }
                result->set_is_scalared(node["is_scalared"].as<bool>());
            }
        }
        // Manage property name
        if(node["name"]) {
            if(node["name"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::TriNet>();
                }
                result->set_name(node["name"].as<std::string>());
            }
        }

        // Manage Child unpacked_dims
        if(node["unpacked_dims"]) {
            const YAML::Node node_unpacked_dims = node["unpacked_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr unpacked_dims_list(new AST::Dimension::List);
            if(node_unpacked_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_unpacked_dims.begin();
                    it != node_unpacked_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        unpacked_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_unpacked_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    unpacked_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::TriNet>();
            }
            result->set_unpacked_dims(unpacked_dims_list);
        }

        // Manage Child cont_assign
        if(node["cont_assign"]) {
            const YAML::Node node_cont_assign = node["cont_assign"];
            // Set the child
            AST::Node::Ptr child = convert(node_cont_assign);
            if(child) {
                AST::Rvalue::Ptr child_cast = AST::cast_to<AST::Rvalue>(child);
                if(!result) {
                    result = std::make_shared<AST::TriNet>();
                }
                result->set_cont_assign(child_cast);
            }
        }

        // Manage Child strength
        if(node["strength"]) {
            const YAML::Node node_strength = node["strength"];
            // Set the child
            AST::Node::Ptr child = convert(node_strength);
            if(child) {
                AST::Strength::Ptr child_cast = AST::cast_to<AST::Strength>(child);
                if(!result) {
                    result = std::make_shared<AST::TriNet>();
                }
                result->set_strength(child_cast);
            }
        }

        // Manage Child ldelay
        if(node["ldelay"]) {
            const YAML::Node node_ldelay = node["ldelay"];
            // Set the child
            AST::Node::Ptr child = convert(node_ldelay);
            if(child) {
                AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
                if(!result) {
                    result = std::make_shared<AST::TriNet>();
                }
                result->set_ldelay(child_cast);
            }
        }

        // Manage Child rdelay
        if(node["rdelay"]) {
            const YAML::Node node_rdelay = node["rdelay"];
            // Set the child
            AST::Node::Ptr child = convert(node_rdelay);
            if(child) {
                AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
                if(!result) {
                    result = std::make_shared<AST::TriNet>();
                }
                result->set_rdelay(child_cast);
            }
        }

        // Manage Child type
        if(node["type"]) {
            const YAML::Node node_type = node["type"];
            // Set the child
            AST::Node::Ptr child = convert(node_type);
            if(child) {
                AST::DataType::Ptr child_cast = AST::cast_to<AST::DataType>(child);
                if(!result) {
                    result = std::make_shared<AST::TriNet>();
                }
                result->set_type(child_cast);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::TriNet>(result);
}

AST::Node::Ptr YAMLImporter::convert_tri0net(const YAML::Node node) const
{
    AST::Tri0Net::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Tri0Net>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Tri0Net>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property is_vectored
        if(node["is_vectored"]) {
            if(node["is_vectored"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Tri0Net>();
                }
                result->set_is_vectored(node["is_vectored"].as<bool>());
            }
        }
        // Manage property is_scalared
        if(node["is_scalared"]) {
            if(node["is_scalared"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Tri0Net>();
                }
                result->set_is_scalared(node["is_scalared"].as<bool>());
            }
        }
        // Manage property name
        if(node["name"]) {
            if(node["name"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Tri0Net>();
                }
                result->set_name(node["name"].as<std::string>());
            }
        }

        // Manage Child unpacked_dims
        if(node["unpacked_dims"]) {
            const YAML::Node node_unpacked_dims = node["unpacked_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr unpacked_dims_list(new AST::Dimension::List);
            if(node_unpacked_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_unpacked_dims.begin();
                    it != node_unpacked_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        unpacked_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_unpacked_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    unpacked_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::Tri0Net>();
            }
            result->set_unpacked_dims(unpacked_dims_list);
        }

        // Manage Child cont_assign
        if(node["cont_assign"]) {
            const YAML::Node node_cont_assign = node["cont_assign"];
            // Set the child
            AST::Node::Ptr child = convert(node_cont_assign);
            if(child) {
                AST::Rvalue::Ptr child_cast = AST::cast_to<AST::Rvalue>(child);
                if(!result) {
                    result = std::make_shared<AST::Tri0Net>();
                }
                result->set_cont_assign(child_cast);
            }
        }

        // Manage Child strength
        if(node["strength"]) {
            const YAML::Node node_strength = node["strength"];
            // Set the child
            AST::Node::Ptr child = convert(node_strength);
            if(child) {
                AST::Strength::Ptr child_cast = AST::cast_to<AST::Strength>(child);
                if(!result) {
                    result = std::make_shared<AST::Tri0Net>();
                }
                result->set_strength(child_cast);
            }
        }

        // Manage Child ldelay
        if(node["ldelay"]) {
            const YAML::Node node_ldelay = node["ldelay"];
            // Set the child
            AST::Node::Ptr child = convert(node_ldelay);
            if(child) {
                AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
                if(!result) {
                    result = std::make_shared<AST::Tri0Net>();
                }
                result->set_ldelay(child_cast);
            }
        }

        // Manage Child rdelay
        if(node["rdelay"]) {
            const YAML::Node node_rdelay = node["rdelay"];
            // Set the child
            AST::Node::Ptr child = convert(node_rdelay);
            if(child) {
                AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
                if(!result) {
                    result = std::make_shared<AST::Tri0Net>();
                }
                result->set_rdelay(child_cast);
            }
        }

        // Manage Child type
        if(node["type"]) {
            const YAML::Node node_type = node["type"];
            // Set the child
            AST::Node::Ptr child = convert(node_type);
            if(child) {
                AST::DataType::Ptr child_cast = AST::cast_to<AST::DataType>(child);
                if(!result) {
                    result = std::make_shared<AST::Tri0Net>();
                }
                result->set_type(child_cast);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Tri0Net>(result);
}

AST::Node::Ptr YAMLImporter::convert_tri1net(const YAML::Node node) const
{
    AST::Tri1Net::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Tri1Net>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Tri1Net>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property is_vectored
        if(node["is_vectored"]) {
            if(node["is_vectored"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Tri1Net>();
                }
                result->set_is_vectored(node["is_vectored"].as<bool>());
            }
        }
        // Manage property is_scalared
        if(node["is_scalared"]) {
            if(node["is_scalared"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Tri1Net>();
                }
                result->set_is_scalared(node["is_scalared"].as<bool>());
            }
        }
        // Manage property name
        if(node["name"]) {
            if(node["name"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Tri1Net>();
                }
                result->set_name(node["name"].as<std::string>());
            }
        }

        // Manage Child unpacked_dims
        if(node["unpacked_dims"]) {
            const YAML::Node node_unpacked_dims = node["unpacked_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr unpacked_dims_list(new AST::Dimension::List);
            if(node_unpacked_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_unpacked_dims.begin();
                    it != node_unpacked_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        unpacked_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_unpacked_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    unpacked_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::Tri1Net>();
            }
            result->set_unpacked_dims(unpacked_dims_list);
        }

        // Manage Child cont_assign
        if(node["cont_assign"]) {
            const YAML::Node node_cont_assign = node["cont_assign"];
            // Set the child
            AST::Node::Ptr child = convert(node_cont_assign);
            if(child) {
                AST::Rvalue::Ptr child_cast = AST::cast_to<AST::Rvalue>(child);
                if(!result) {
                    result = std::make_shared<AST::Tri1Net>();
                }
                result->set_cont_assign(child_cast);
            }
        }

        // Manage Child strength
        if(node["strength"]) {
            const YAML::Node node_strength = node["strength"];
            // Set the child
            AST::Node::Ptr child = convert(node_strength);
            if(child) {
                AST::Strength::Ptr child_cast = AST::cast_to<AST::Strength>(child);
                if(!result) {
                    result = std::make_shared<AST::Tri1Net>();
                }
                result->set_strength(child_cast);
            }
        }

        // Manage Child ldelay
        if(node["ldelay"]) {
            const YAML::Node node_ldelay = node["ldelay"];
            // Set the child
            AST::Node::Ptr child = convert(node_ldelay);
            if(child) {
                AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
                if(!result) {
                    result = std::make_shared<AST::Tri1Net>();
                }
                result->set_ldelay(child_cast);
            }
        }

        // Manage Child rdelay
        if(node["rdelay"]) {
            const YAML::Node node_rdelay = node["rdelay"];
            // Set the child
            AST::Node::Ptr child = convert(node_rdelay);
            if(child) {
                AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
                if(!result) {
                    result = std::make_shared<AST::Tri1Net>();
                }
                result->set_rdelay(child_cast);
            }
        }

        // Manage Child type
        if(node["type"]) {
            const YAML::Node node_type = node["type"];
            // Set the child
            AST::Node::Ptr child = convert(node_type);
            if(child) {
                AST::DataType::Ptr child_cast = AST::cast_to<AST::DataType>(child);
                if(!result) {
                    result = std::make_shared<AST::Tri1Net>();
                }
                result->set_type(child_cast);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Tri1Net>(result);
}

AST::Node::Ptr YAMLImporter::convert_triandnet(const YAML::Node node) const
{
    AST::TriandNet::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::TriandNet>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::TriandNet>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property is_vectored
        if(node["is_vectored"]) {
            if(node["is_vectored"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::TriandNet>();
                }
                result->set_is_vectored(node["is_vectored"].as<bool>());
            }
        }
        // Manage property is_scalared
        if(node["is_scalared"]) {
            if(node["is_scalared"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::TriandNet>();
                }
                result->set_is_scalared(node["is_scalared"].as<bool>());
            }
        }
        // Manage property name
        if(node["name"]) {
            if(node["name"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::TriandNet>();
                }
                result->set_name(node["name"].as<std::string>());
            }
        }

        // Manage Child unpacked_dims
        if(node["unpacked_dims"]) {
            const YAML::Node node_unpacked_dims = node["unpacked_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr unpacked_dims_list(new AST::Dimension::List);
            if(node_unpacked_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_unpacked_dims.begin();
                    it != node_unpacked_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        unpacked_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_unpacked_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    unpacked_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::TriandNet>();
            }
            result->set_unpacked_dims(unpacked_dims_list);
        }

        // Manage Child cont_assign
        if(node["cont_assign"]) {
            const YAML::Node node_cont_assign = node["cont_assign"];
            // Set the child
            AST::Node::Ptr child = convert(node_cont_assign);
            if(child) {
                AST::Rvalue::Ptr child_cast = AST::cast_to<AST::Rvalue>(child);
                if(!result) {
                    result = std::make_shared<AST::TriandNet>();
                }
                result->set_cont_assign(child_cast);
            }
        }

        // Manage Child strength
        if(node["strength"]) {
            const YAML::Node node_strength = node["strength"];
            // Set the child
            AST::Node::Ptr child = convert(node_strength);
            if(child) {
                AST::Strength::Ptr child_cast = AST::cast_to<AST::Strength>(child);
                if(!result) {
                    result = std::make_shared<AST::TriandNet>();
                }
                result->set_strength(child_cast);
            }
        }

        // Manage Child ldelay
        if(node["ldelay"]) {
            const YAML::Node node_ldelay = node["ldelay"];
            // Set the child
            AST::Node::Ptr child = convert(node_ldelay);
            if(child) {
                AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
                if(!result) {
                    result = std::make_shared<AST::TriandNet>();
                }
                result->set_ldelay(child_cast);
            }
        }

        // Manage Child rdelay
        if(node["rdelay"]) {
            const YAML::Node node_rdelay = node["rdelay"];
            // Set the child
            AST::Node::Ptr child = convert(node_rdelay);
            if(child) {
                AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
                if(!result) {
                    result = std::make_shared<AST::TriandNet>();
                }
                result->set_rdelay(child_cast);
            }
        }

        // Manage Child type
        if(node["type"]) {
            const YAML::Node node_type = node["type"];
            // Set the child
            AST::Node::Ptr child = convert(node_type);
            if(child) {
                AST::DataType::Ptr child_cast = AST::cast_to<AST::DataType>(child);
                if(!result) {
                    result = std::make_shared<AST::TriandNet>();
                }
                result->set_type(child_cast);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::TriandNet>(result);
}

AST::Node::Ptr YAMLImporter::convert_triornet(const YAML::Node node) const
{
    AST::TriorNet::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::TriorNet>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::TriorNet>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property is_vectored
        if(node["is_vectored"]) {
            if(node["is_vectored"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::TriorNet>();
                }
                result->set_is_vectored(node["is_vectored"].as<bool>());
            }
        }
        // Manage property is_scalared
        if(node["is_scalared"]) {
            if(node["is_scalared"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::TriorNet>();
                }
                result->set_is_scalared(node["is_scalared"].as<bool>());
            }
        }
        // Manage property name
        if(node["name"]) {
            if(node["name"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::TriorNet>();
                }
                result->set_name(node["name"].as<std::string>());
            }
        }

        // Manage Child unpacked_dims
        if(node["unpacked_dims"]) {
            const YAML::Node node_unpacked_dims = node["unpacked_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr unpacked_dims_list(new AST::Dimension::List);
            if(node_unpacked_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_unpacked_dims.begin();
                    it != node_unpacked_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        unpacked_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_unpacked_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    unpacked_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::TriorNet>();
            }
            result->set_unpacked_dims(unpacked_dims_list);
        }

        // Manage Child cont_assign
        if(node["cont_assign"]) {
            const YAML::Node node_cont_assign = node["cont_assign"];
            // Set the child
            AST::Node::Ptr child = convert(node_cont_assign);
            if(child) {
                AST::Rvalue::Ptr child_cast = AST::cast_to<AST::Rvalue>(child);
                if(!result) {
                    result = std::make_shared<AST::TriorNet>();
                }
                result->set_cont_assign(child_cast);
            }
        }

        // Manage Child strength
        if(node["strength"]) {
            const YAML::Node node_strength = node["strength"];
            // Set the child
            AST::Node::Ptr child = convert(node_strength);
            if(child) {
                AST::Strength::Ptr child_cast = AST::cast_to<AST::Strength>(child);
                if(!result) {
                    result = std::make_shared<AST::TriorNet>();
                }
                result->set_strength(child_cast);
            }
        }

        // Manage Child ldelay
        if(node["ldelay"]) {
            const YAML::Node node_ldelay = node["ldelay"];
            // Set the child
            AST::Node::Ptr child = convert(node_ldelay);
            if(child) {
                AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
                if(!result) {
                    result = std::make_shared<AST::TriorNet>();
                }
                result->set_ldelay(child_cast);
            }
        }

        // Manage Child rdelay
        if(node["rdelay"]) {
            const YAML::Node node_rdelay = node["rdelay"];
            // Set the child
            AST::Node::Ptr child = convert(node_rdelay);
            if(child) {
                AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
                if(!result) {
                    result = std::make_shared<AST::TriorNet>();
                }
                result->set_rdelay(child_cast);
            }
        }

        // Manage Child type
        if(node["type"]) {
            const YAML::Node node_type = node["type"];
            // Set the child
            AST::Node::Ptr child = convert(node_type);
            if(child) {
                AST::DataType::Ptr child_cast = AST::cast_to<AST::DataType>(child);
                if(!result) {
                    result = std::make_shared<AST::TriorNet>();
                }
                result->set_type(child_cast);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::TriorNet>(result);
}

AST::Node::Ptr YAMLImporter::convert_triregnet(const YAML::Node node) const
{
    AST::TriregNet::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::TriregNet>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::TriregNet>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property is_vectored
        if(node["is_vectored"]) {
            if(node["is_vectored"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::TriregNet>();
                }
                result->set_is_vectored(node["is_vectored"].as<bool>());
            }
        }
        // Manage property is_scalared
        if(node["is_scalared"]) {
            if(node["is_scalared"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::TriregNet>();
                }
                result->set_is_scalared(node["is_scalared"].as<bool>());
            }
        }
        // Manage property name
        if(node["name"]) {
            if(node["name"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::TriregNet>();
                }
                result->set_name(node["name"].as<std::string>());
            }
        }

        // Manage Child unpacked_dims
        if(node["unpacked_dims"]) {
            const YAML::Node node_unpacked_dims = node["unpacked_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr unpacked_dims_list(new AST::Dimension::List);
            if(node_unpacked_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_unpacked_dims.begin();
                    it != node_unpacked_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        unpacked_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_unpacked_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    unpacked_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::TriregNet>();
            }
            result->set_unpacked_dims(unpacked_dims_list);
        }

        // Manage Child cont_assign
        if(node["cont_assign"]) {
            const YAML::Node node_cont_assign = node["cont_assign"];
            // Set the child
            AST::Node::Ptr child = convert(node_cont_assign);
            if(child) {
                AST::Rvalue::Ptr child_cast = AST::cast_to<AST::Rvalue>(child);
                if(!result) {
                    result = std::make_shared<AST::TriregNet>();
                }
                result->set_cont_assign(child_cast);
            }
        }

        // Manage Child strength
        if(node["strength"]) {
            const YAML::Node node_strength = node["strength"];
            // Set the child
            AST::Node::Ptr child = convert(node_strength);
            if(child) {
                AST::Strength::Ptr child_cast = AST::cast_to<AST::Strength>(child);
                if(!result) {
                    result = std::make_shared<AST::TriregNet>();
                }
                result->set_strength(child_cast);
            }
        }

        // Manage Child ldelay
        if(node["ldelay"]) {
            const YAML::Node node_ldelay = node["ldelay"];
            // Set the child
            AST::Node::Ptr child = convert(node_ldelay);
            if(child) {
                AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
                if(!result) {
                    result = std::make_shared<AST::TriregNet>();
                }
                result->set_ldelay(child_cast);
            }
        }

        // Manage Child rdelay
        if(node["rdelay"]) {
            const YAML::Node node_rdelay = node["rdelay"];
            // Set the child
            AST::Node::Ptr child = convert(node_rdelay);
            if(child) {
                AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
                if(!result) {
                    result = std::make_shared<AST::TriregNet>();
                }
                result->set_rdelay(child_cast);
            }
        }

        // Manage Child type
        if(node["type"]) {
            const YAML::Node node_type = node["type"];
            // Set the child
            AST::Node::Ptr child = convert(node_type);
            if(child) {
                AST::DataType::Ptr child_cast = AST::cast_to<AST::DataType>(child);
                if(!result) {
                    result = std::make_shared<AST::TriregNet>();
                }
                result->set_type(child_cast);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::TriregNet>(result);
}

AST::Node::Ptr YAMLImporter::convert_wandnet(const YAML::Node node) const
{
    AST::WandNet::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::WandNet>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::WandNet>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property is_vectored
        if(node["is_vectored"]) {
            if(node["is_vectored"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::WandNet>();
                }
                result->set_is_vectored(node["is_vectored"].as<bool>());
            }
        }
        // Manage property is_scalared
        if(node["is_scalared"]) {
            if(node["is_scalared"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::WandNet>();
                }
                result->set_is_scalared(node["is_scalared"].as<bool>());
            }
        }
        // Manage property name
        if(node["name"]) {
            if(node["name"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::WandNet>();
                }
                result->set_name(node["name"].as<std::string>());
            }
        }

        // Manage Child unpacked_dims
        if(node["unpacked_dims"]) {
            const YAML::Node node_unpacked_dims = node["unpacked_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr unpacked_dims_list(new AST::Dimension::List);
            if(node_unpacked_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_unpacked_dims.begin();
                    it != node_unpacked_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        unpacked_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_unpacked_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    unpacked_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::WandNet>();
            }
            result->set_unpacked_dims(unpacked_dims_list);
        }

        // Manage Child cont_assign
        if(node["cont_assign"]) {
            const YAML::Node node_cont_assign = node["cont_assign"];
            // Set the child
            AST::Node::Ptr child = convert(node_cont_assign);
            if(child) {
                AST::Rvalue::Ptr child_cast = AST::cast_to<AST::Rvalue>(child);
                if(!result) {
                    result = std::make_shared<AST::WandNet>();
                }
                result->set_cont_assign(child_cast);
            }
        }

        // Manage Child strength
        if(node["strength"]) {
            const YAML::Node node_strength = node["strength"];
            // Set the child
            AST::Node::Ptr child = convert(node_strength);
            if(child) {
                AST::Strength::Ptr child_cast = AST::cast_to<AST::Strength>(child);
                if(!result) {
                    result = std::make_shared<AST::WandNet>();
                }
                result->set_strength(child_cast);
            }
        }

        // Manage Child ldelay
        if(node["ldelay"]) {
            const YAML::Node node_ldelay = node["ldelay"];
            // Set the child
            AST::Node::Ptr child = convert(node_ldelay);
            if(child) {
                AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
                if(!result) {
                    result = std::make_shared<AST::WandNet>();
                }
                result->set_ldelay(child_cast);
            }
        }

        // Manage Child rdelay
        if(node["rdelay"]) {
            const YAML::Node node_rdelay = node["rdelay"];
            // Set the child
            AST::Node::Ptr child = convert(node_rdelay);
            if(child) {
                AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
                if(!result) {
                    result = std::make_shared<AST::WandNet>();
                }
                result->set_rdelay(child_cast);
            }
        }

        // Manage Child type
        if(node["type"]) {
            const YAML::Node node_type = node["type"];
            // Set the child
            AST::Node::Ptr child = convert(node_type);
            if(child) {
                AST::DataType::Ptr child_cast = AST::cast_to<AST::DataType>(child);
                if(!result) {
                    result = std::make_shared<AST::WandNet>();
                }
                result->set_type(child_cast);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::WandNet>(result);
}

AST::Node::Ptr YAMLImporter::convert_wornet(const YAML::Node node) const
{
    AST::WorNet::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::WorNet>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::WorNet>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property is_vectored
        if(node["is_vectored"]) {
            if(node["is_vectored"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::WorNet>();
                }
                result->set_is_vectored(node["is_vectored"].as<bool>());
            }
        }
        // Manage property is_scalared
        if(node["is_scalared"]) {
            if(node["is_scalared"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::WorNet>();
                }
                result->set_is_scalared(node["is_scalared"].as<bool>());
            }
        }
        // Manage property name
        if(node["name"]) {
            if(node["name"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::WorNet>();
                }
                result->set_name(node["name"].as<std::string>());
            }
        }

        // Manage Child unpacked_dims
        if(node["unpacked_dims"]) {
            const YAML::Node node_unpacked_dims = node["unpacked_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr unpacked_dims_list(new AST::Dimension::List);
            if(node_unpacked_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_unpacked_dims.begin();
                    it != node_unpacked_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        unpacked_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_unpacked_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    unpacked_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::WorNet>();
            }
            result->set_unpacked_dims(unpacked_dims_list);
        }

        // Manage Child cont_assign
        if(node["cont_assign"]) {
            const YAML::Node node_cont_assign = node["cont_assign"];
            // Set the child
            AST::Node::Ptr child = convert(node_cont_assign);
            if(child) {
                AST::Rvalue::Ptr child_cast = AST::cast_to<AST::Rvalue>(child);
                if(!result) {
                    result = std::make_shared<AST::WorNet>();
                }
                result->set_cont_assign(child_cast);
            }
        }

        // Manage Child strength
        if(node["strength"]) {
            const YAML::Node node_strength = node["strength"];
            // Set the child
            AST::Node::Ptr child = convert(node_strength);
            if(child) {
                AST::Strength::Ptr child_cast = AST::cast_to<AST::Strength>(child);
                if(!result) {
                    result = std::make_shared<AST::WorNet>();
                }
                result->set_strength(child_cast);
            }
        }

        // Manage Child ldelay
        if(node["ldelay"]) {
            const YAML::Node node_ldelay = node["ldelay"];
            // Set the child
            AST::Node::Ptr child = convert(node_ldelay);
            if(child) {
                AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
                if(!result) {
                    result = std::make_shared<AST::WorNet>();
                }
                result->set_ldelay(child_cast);
            }
        }

        // Manage Child rdelay
        if(node["rdelay"]) {
            const YAML::Node node_rdelay = node["rdelay"];
            // Set the child
            AST::Node::Ptr child = convert(node_rdelay);
            if(child) {
                AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
                if(!result) {
                    result = std::make_shared<AST::WorNet>();
                }
                result->set_rdelay(child_cast);
            }
        }

        // Manage Child type
        if(node["type"]) {
            const YAML::Node node_type = node["type"];
            // Set the child
            AST::Node::Ptr child = convert(node_type);
            if(child) {
                AST::DataType::Ptr child_cast = AST::cast_to<AST::DataType>(child);
                if(!result) {
                    result = std::make_shared<AST::WorNet>();
                }
                result->set_type(child_cast);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::WorNet>(result);
}

AST::Node::Ptr YAMLImporter::convert_uwirenet(const YAML::Node node) const
{
    AST::UwireNet::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::UwireNet>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::UwireNet>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property is_vectored
        if(node["is_vectored"]) {
            if(node["is_vectored"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::UwireNet>();
                }
                result->set_is_vectored(node["is_vectored"].as<bool>());
            }
        }
        // Manage property is_scalared
        if(node["is_scalared"]) {
            if(node["is_scalared"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::UwireNet>();
                }
                result->set_is_scalared(node["is_scalared"].as<bool>());
            }
        }
        // Manage property name
        if(node["name"]) {
            if(node["name"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::UwireNet>();
                }
                result->set_name(node["name"].as<std::string>());
            }
        }

        // Manage Child unpacked_dims
        if(node["unpacked_dims"]) {
            const YAML::Node node_unpacked_dims = node["unpacked_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr unpacked_dims_list(new AST::Dimension::List);
            if(node_unpacked_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_unpacked_dims.begin();
                    it != node_unpacked_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        unpacked_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_unpacked_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    unpacked_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::UwireNet>();
            }
            result->set_unpacked_dims(unpacked_dims_list);
        }

        // Manage Child cont_assign
        if(node["cont_assign"]) {
            const YAML::Node node_cont_assign = node["cont_assign"];
            // Set the child
            AST::Node::Ptr child = convert(node_cont_assign);
            if(child) {
                AST::Rvalue::Ptr child_cast = AST::cast_to<AST::Rvalue>(child);
                if(!result) {
                    result = std::make_shared<AST::UwireNet>();
                }
                result->set_cont_assign(child_cast);
            }
        }

        // Manage Child strength
        if(node["strength"]) {
            const YAML::Node node_strength = node["strength"];
            // Set the child
            AST::Node::Ptr child = convert(node_strength);
            if(child) {
                AST::Strength::Ptr child_cast = AST::cast_to<AST::Strength>(child);
                if(!result) {
                    result = std::make_shared<AST::UwireNet>();
                }
                result->set_strength(child_cast);
            }
        }

        // Manage Child ldelay
        if(node["ldelay"]) {
            const YAML::Node node_ldelay = node["ldelay"];
            // Set the child
            AST::Node::Ptr child = convert(node_ldelay);
            if(child) {
                AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
                if(!result) {
                    result = std::make_shared<AST::UwireNet>();
                }
                result->set_ldelay(child_cast);
            }
        }

        // Manage Child rdelay
        if(node["rdelay"]) {
            const YAML::Node node_rdelay = node["rdelay"];
            // Set the child
            AST::Node::Ptr child = convert(node_rdelay);
            if(child) {
                AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
                if(!result) {
                    result = std::make_shared<AST::UwireNet>();
                }
                result->set_rdelay(child_cast);
            }
        }

        // Manage Child type
        if(node["type"]) {
            const YAML::Node node_type = node["type"];
            // Set the child
            AST::Node::Ptr child = convert(node_type);
            if(child) {
                AST::DataType::Ptr child_cast = AST::cast_to<AST::DataType>(child);
                if(!result) {
                    result = std::make_shared<AST::UwireNet>();
                }
                result->set_type(child_cast);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::UwireNet>(result);
}

AST::Node::Ptr YAMLImporter::convert_supply0net(const YAML::Node node) const
{
    AST::Supply0Net::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Supply0Net>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Supply0Net>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property is_vectored
        if(node["is_vectored"]) {
            if(node["is_vectored"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Supply0Net>();
                }
                result->set_is_vectored(node["is_vectored"].as<bool>());
            }
        }
        // Manage property is_scalared
        if(node["is_scalared"]) {
            if(node["is_scalared"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Supply0Net>();
                }
                result->set_is_scalared(node["is_scalared"].as<bool>());
            }
        }
        // Manage property name
        if(node["name"]) {
            if(node["name"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Supply0Net>();
                }
                result->set_name(node["name"].as<std::string>());
            }
        }

        // Manage Child unpacked_dims
        if(node["unpacked_dims"]) {
            const YAML::Node node_unpacked_dims = node["unpacked_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr unpacked_dims_list(new AST::Dimension::List);
            if(node_unpacked_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_unpacked_dims.begin();
                    it != node_unpacked_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        unpacked_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_unpacked_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    unpacked_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::Supply0Net>();
            }
            result->set_unpacked_dims(unpacked_dims_list);
        }

        // Manage Child cont_assign
        if(node["cont_assign"]) {
            const YAML::Node node_cont_assign = node["cont_assign"];
            // Set the child
            AST::Node::Ptr child = convert(node_cont_assign);
            if(child) {
                AST::Rvalue::Ptr child_cast = AST::cast_to<AST::Rvalue>(child);
                if(!result) {
                    result = std::make_shared<AST::Supply0Net>();
                }
                result->set_cont_assign(child_cast);
            }
        }

        // Manage Child strength
        if(node["strength"]) {
            const YAML::Node node_strength = node["strength"];
            // Set the child
            AST::Node::Ptr child = convert(node_strength);
            if(child) {
                AST::Strength::Ptr child_cast = AST::cast_to<AST::Strength>(child);
                if(!result) {
                    result = std::make_shared<AST::Supply0Net>();
                }
                result->set_strength(child_cast);
            }
        }

        // Manage Child ldelay
        if(node["ldelay"]) {
            const YAML::Node node_ldelay = node["ldelay"];
            // Set the child
            AST::Node::Ptr child = convert(node_ldelay);
            if(child) {
                AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
                if(!result) {
                    result = std::make_shared<AST::Supply0Net>();
                }
                result->set_ldelay(child_cast);
            }
        }

        // Manage Child rdelay
        if(node["rdelay"]) {
            const YAML::Node node_rdelay = node["rdelay"];
            // Set the child
            AST::Node::Ptr child = convert(node_rdelay);
            if(child) {
                AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
                if(!result) {
                    result = std::make_shared<AST::Supply0Net>();
                }
                result->set_rdelay(child_cast);
            }
        }

        // Manage Child type
        if(node["type"]) {
            const YAML::Node node_type = node["type"];
            // Set the child
            AST::Node::Ptr child = convert(node_type);
            if(child) {
                AST::DataType::Ptr child_cast = AST::cast_to<AST::DataType>(child);
                if(!result) {
                    result = std::make_shared<AST::Supply0Net>();
                }
                result->set_type(child_cast);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Supply0Net>(result);
}

AST::Node::Ptr YAMLImporter::convert_supply1net(const YAML::Node node) const
{
    AST::Supply1Net::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Supply1Net>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Supply1Net>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property is_vectored
        if(node["is_vectored"]) {
            if(node["is_vectored"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Supply1Net>();
                }
                result->set_is_vectored(node["is_vectored"].as<bool>());
            }
        }
        // Manage property is_scalared
        if(node["is_scalared"]) {
            if(node["is_scalared"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Supply1Net>();
                }
                result->set_is_scalared(node["is_scalared"].as<bool>());
            }
        }
        // Manage property name
        if(node["name"]) {
            if(node["name"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Supply1Net>();
                }
                result->set_name(node["name"].as<std::string>());
            }
        }

        // Manage Child unpacked_dims
        if(node["unpacked_dims"]) {
            const YAML::Node node_unpacked_dims = node["unpacked_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr unpacked_dims_list(new AST::Dimension::List);
            if(node_unpacked_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_unpacked_dims.begin();
                    it != node_unpacked_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        unpacked_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_unpacked_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    unpacked_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::Supply1Net>();
            }
            result->set_unpacked_dims(unpacked_dims_list);
        }

        // Manage Child cont_assign
        if(node["cont_assign"]) {
            const YAML::Node node_cont_assign = node["cont_assign"];
            // Set the child
            AST::Node::Ptr child = convert(node_cont_assign);
            if(child) {
                AST::Rvalue::Ptr child_cast = AST::cast_to<AST::Rvalue>(child);
                if(!result) {
                    result = std::make_shared<AST::Supply1Net>();
                }
                result->set_cont_assign(child_cast);
            }
        }

        // Manage Child strength
        if(node["strength"]) {
            const YAML::Node node_strength = node["strength"];
            // Set the child
            AST::Node::Ptr child = convert(node_strength);
            if(child) {
                AST::Strength::Ptr child_cast = AST::cast_to<AST::Strength>(child);
                if(!result) {
                    result = std::make_shared<AST::Supply1Net>();
                }
                result->set_strength(child_cast);
            }
        }

        // Manage Child ldelay
        if(node["ldelay"]) {
            const YAML::Node node_ldelay = node["ldelay"];
            // Set the child
            AST::Node::Ptr child = convert(node_ldelay);
            if(child) {
                AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
                if(!result) {
                    result = std::make_shared<AST::Supply1Net>();
                }
                result->set_ldelay(child_cast);
            }
        }

        // Manage Child rdelay
        if(node["rdelay"]) {
            const YAML::Node node_rdelay = node["rdelay"];
            // Set the child
            AST::Node::Ptr child = convert(node_rdelay);
            if(child) {
                AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
                if(!result) {
                    result = std::make_shared<AST::Supply1Net>();
                }
                result->set_rdelay(child_cast);
            }
        }

        // Manage Child type
        if(node["type"]) {
            const YAML::Node node_type = node["type"];
            // Set the child
            AST::Node::Ptr child = convert(node_type);
            if(child) {
                AST::DataType::Ptr child_cast = AST::cast_to<AST::DataType>(child);
                if(!result) {
                    result = std::make_shared<AST::Supply1Net>();
                }
                result->set_type(child_cast);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Supply1Net>(result);
}

AST::Node::Ptr YAMLImporter::convert_interconnectnet(const YAML::Node node) const
{
    AST::InterconnectNet::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::InterconnectNet>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::InterconnectNet>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property is_vectored
        if(node["is_vectored"]) {
            if(node["is_vectored"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::InterconnectNet>();
                }
                result->set_is_vectored(node["is_vectored"].as<bool>());
            }
        }
        // Manage property is_scalared
        if(node["is_scalared"]) {
            if(node["is_scalared"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::InterconnectNet>();
                }
                result->set_is_scalared(node["is_scalared"].as<bool>());
            }
        }
        // Manage property name
        if(node["name"]) {
            if(node["name"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::InterconnectNet>();
                }
                result->set_name(node["name"].as<std::string>());
            }
        }

        // Manage Child unpacked_dims
        if(node["unpacked_dims"]) {
            const YAML::Node node_unpacked_dims = node["unpacked_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr unpacked_dims_list(new AST::Dimension::List);
            if(node_unpacked_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_unpacked_dims.begin();
                    it != node_unpacked_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        unpacked_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_unpacked_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    unpacked_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::InterconnectNet>();
            }
            result->set_unpacked_dims(unpacked_dims_list);
        }

        // Manage Child cont_assign
        if(node["cont_assign"]) {
            const YAML::Node node_cont_assign = node["cont_assign"];
            // Set the child
            AST::Node::Ptr child = convert(node_cont_assign);
            if(child) {
                AST::Rvalue::Ptr child_cast = AST::cast_to<AST::Rvalue>(child);
                if(!result) {
                    result = std::make_shared<AST::InterconnectNet>();
                }
                result->set_cont_assign(child_cast);
            }
        }

        // Manage Child strength
        if(node["strength"]) {
            const YAML::Node node_strength = node["strength"];
            // Set the child
            AST::Node::Ptr child = convert(node_strength);
            if(child) {
                AST::Strength::Ptr child_cast = AST::cast_to<AST::Strength>(child);
                if(!result) {
                    result = std::make_shared<AST::InterconnectNet>();
                }
                result->set_strength(child_cast);
            }
        }

        // Manage Child ldelay
        if(node["ldelay"]) {
            const YAML::Node node_ldelay = node["ldelay"];
            // Set the child
            AST::Node::Ptr child = convert(node_ldelay);
            if(child) {
                AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
                if(!result) {
                    result = std::make_shared<AST::InterconnectNet>();
                }
                result->set_ldelay(child_cast);
            }
        }

        // Manage Child rdelay
        if(node["rdelay"]) {
            const YAML::Node node_rdelay = node["rdelay"];
            // Set the child
            AST::Node::Ptr child = convert(node_rdelay);
            if(child) {
                AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
                if(!result) {
                    result = std::make_shared<AST::InterconnectNet>();
                }
                result->set_rdelay(child_cast);
            }
        }

        // Manage Child type
        if(node["type"]) {
            const YAML::Node node_type = node["type"];
            // Set the child
            AST::Node::Ptr child = convert(node_type);
            if(child) {
                AST::DataType::Ptr child_cast = AST::cast_to<AST::DataType>(child);
                if(!result) {
                    result = std::make_shared<AST::InterconnectNet>();
                }
                result->set_type(child_cast);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::InterconnectNet>(result);
}

AST::Node::Ptr YAMLImporter::convert_usernet(const YAML::Node node) const
{
    AST::UserNet::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::UserNet>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::UserNet>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property is_vectored
        if(node["is_vectored"]) {
            if(node["is_vectored"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::UserNet>();
                }
                result->set_is_vectored(node["is_vectored"].as<bool>());
            }
        }
        // Manage property is_scalared
        if(node["is_scalared"]) {
            if(node["is_scalared"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::UserNet>();
                }
                result->set_is_scalared(node["is_scalared"].as<bool>());
            }
        }
        // Manage property name
        if(node["name"]) {
            if(node["name"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::UserNet>();
                }
                result->set_name(node["name"].as<std::string>());
            }
        }

        // Manage Child unpacked_dims
        if(node["unpacked_dims"]) {
            const YAML::Node node_unpacked_dims = node["unpacked_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr unpacked_dims_list(new AST::Dimension::List);
            if(node_unpacked_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_unpacked_dims.begin();
                    it != node_unpacked_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        unpacked_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_unpacked_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    unpacked_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::UserNet>();
            }
            result->set_unpacked_dims(unpacked_dims_list);
        }

        // Manage Child cont_assign
        if(node["cont_assign"]) {
            const YAML::Node node_cont_assign = node["cont_assign"];
            // Set the child
            AST::Node::Ptr child = convert(node_cont_assign);
            if(child) {
                AST::Rvalue::Ptr child_cast = AST::cast_to<AST::Rvalue>(child);
                if(!result) {
                    result = std::make_shared<AST::UserNet>();
                }
                result->set_cont_assign(child_cast);
            }
        }

        // Manage Child strength
        if(node["strength"]) {
            const YAML::Node node_strength = node["strength"];
            // Set the child
            AST::Node::Ptr child = convert(node_strength);
            if(child) {
                AST::Strength::Ptr child_cast = AST::cast_to<AST::Strength>(child);
                if(!result) {
                    result = std::make_shared<AST::UserNet>();
                }
                result->set_strength(child_cast);
            }
        }

        // Manage Child ldelay
        if(node["ldelay"]) {
            const YAML::Node node_ldelay = node["ldelay"];
            // Set the child
            AST::Node::Ptr child = convert(node_ldelay);
            if(child) {
                AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
                if(!result) {
                    result = std::make_shared<AST::UserNet>();
                }
                result->set_ldelay(child_cast);
            }
        }

        // Manage Child rdelay
        if(node["rdelay"]) {
            const YAML::Node node_rdelay = node["rdelay"];
            // Set the child
            AST::Node::Ptr child = convert(node_rdelay);
            if(child) {
                AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
                if(!result) {
                    result = std::make_shared<AST::UserNet>();
                }
                result->set_rdelay(child_cast);
            }
        }

        // Manage Child type
        if(node["type"]) {
            const YAML::Node node_type = node["type"];
            // Set the child
            AST::Node::Ptr child = convert(node_type);
            if(child) {
                AST::DataType::Ptr child_cast = AST::cast_to<AST::DataType>(child);
                if(!result) {
                    result = std::make_shared<AST::UserNet>();
                }
                result->set_type(child_cast);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::UserNet>(result);
}

AST::Node::Ptr YAMLImporter::convert_implicitnet(const YAML::Node node) const
{
    AST::ImplicitNet::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::ImplicitNet>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::ImplicitNet>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property is_vectored
        if(node["is_vectored"]) {
            if(node["is_vectored"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::ImplicitNet>();
                }
                result->set_is_vectored(node["is_vectored"].as<bool>());
            }
        }
        // Manage property is_scalared
        if(node["is_scalared"]) {
            if(node["is_scalared"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::ImplicitNet>();
                }
                result->set_is_scalared(node["is_scalared"].as<bool>());
            }
        }
        // Manage property name
        if(node["name"]) {
            if(node["name"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::ImplicitNet>();
                }
                result->set_name(node["name"].as<std::string>());
            }
        }

        // Manage Child unpacked_dims
        if(node["unpacked_dims"]) {
            const YAML::Node node_unpacked_dims = node["unpacked_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr unpacked_dims_list(new AST::Dimension::List);
            if(node_unpacked_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_unpacked_dims.begin();
                    it != node_unpacked_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        unpacked_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_unpacked_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    unpacked_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::ImplicitNet>();
            }
            result->set_unpacked_dims(unpacked_dims_list);
        }

        // Manage Child cont_assign
        if(node["cont_assign"]) {
            const YAML::Node node_cont_assign = node["cont_assign"];
            // Set the child
            AST::Node::Ptr child = convert(node_cont_assign);
            if(child) {
                AST::Rvalue::Ptr child_cast = AST::cast_to<AST::Rvalue>(child);
                if(!result) {
                    result = std::make_shared<AST::ImplicitNet>();
                }
                result->set_cont_assign(child_cast);
            }
        }

        // Manage Child strength
        if(node["strength"]) {
            const YAML::Node node_strength = node["strength"];
            // Set the child
            AST::Node::Ptr child = convert(node_strength);
            if(child) {
                AST::Strength::Ptr child_cast = AST::cast_to<AST::Strength>(child);
                if(!result) {
                    result = std::make_shared<AST::ImplicitNet>();
                }
                result->set_strength(child_cast);
            }
        }

        // Manage Child ldelay
        if(node["ldelay"]) {
            const YAML::Node node_ldelay = node["ldelay"];
            // Set the child
            AST::Node::Ptr child = convert(node_ldelay);
            if(child) {
                AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
                if(!result) {
                    result = std::make_shared<AST::ImplicitNet>();
                }
                result->set_ldelay(child_cast);
            }
        }

        // Manage Child rdelay
        if(node["rdelay"]) {
            const YAML::Node node_rdelay = node["rdelay"];
            // Set the child
            AST::Node::Ptr child = convert(node_rdelay);
            if(child) {
                AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
                if(!result) {
                    result = std::make_shared<AST::ImplicitNet>();
                }
                result->set_rdelay(child_cast);
            }
        }

        // Manage Child type
        if(node["type"]) {
            const YAML::Node node_type = node["type"];
            // Set the child
            AST::Node::Ptr child = convert(node_type);
            if(child) {
                AST::DataType::Ptr child_cast = AST::cast_to<AST::DataType>(child);
                if(!result) {
                    result = std::make_shared<AST::ImplicitNet>();
                }
                result->set_type(child_cast);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::ImplicitNet>(result);
}

AST::Node::Ptr YAMLImporter::convert_strength(const YAML::Node node) const
{
    AST::Strength::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Strength>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Strength>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Strength>(result);
}

AST::Node::Ptr YAMLImporter::convert_drivestrength(const YAML::Node node) const
{
    AST::DriveStrength::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::DriveStrength>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::DriveStrength>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property s0
        if(node["s0"]) {
            if(node["s0"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::DriveStrength>();
                }
                result->set_s0(node["s0"].as<AST::DriveStrength::S0Enum>());
            }
        }
        // Manage property s1
        if(node["s1"]) {
            if(node["s1"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::DriveStrength>();
                }
                result->set_s1(node["s1"].as<AST::DriveStrength::S1Enum>());
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::DriveStrength>(result);
}

AST::Node::Ptr YAMLImporter::convert_chargestrength(const YAML::Node node) const
{
    AST::ChargeStrength::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::ChargeStrength>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::ChargeStrength>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property charge
        if(node["charge"]) {
            if(node["charge"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::ChargeStrength>();
                }
                result->set_charge(node["charge"].as<AST::ChargeStrength::ChargeEnum>());
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::ChargeStrength>(result);
}

AST::Node::Ptr YAMLImporter::convert_param(const YAML::Node node) const
{
    AST::Param::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Param>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Param>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property is_local
        if(node["is_local"]) {
            if(node["is_local"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Param>();
                }
                result->set_is_local(node["is_local"].as<bool>());
            }
        }
        // Manage property name
        if(node["name"]) {
            if(node["name"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Param>();
                }
                result->set_name(node["name"].as<std::string>());
            }
        }

        // Manage Child value
        if(node["value"]) {
            const YAML::Node node_value = node["value"];
            // Set the child
            AST::Node::Ptr child = convert(node_value);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Param>();
                }
                result->set_value(child);
            }
        }

        // Manage Child unpacked_dims
        if(node["unpacked_dims"]) {
            const YAML::Node node_unpacked_dims = node["unpacked_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr unpacked_dims_list(new AST::Dimension::List);
            if(node_unpacked_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_unpacked_dims.begin();
                    it != node_unpacked_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        unpacked_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_unpacked_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    unpacked_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::Param>();
            }
            result->set_unpacked_dims(unpacked_dims_list);
        }

        // Manage Child type
        if(node["type"]) {
            const YAML::Node node_type = node["type"];
            // Set the child
            AST::Node::Ptr child = convert(node_type);
            if(child) {
                AST::DataType::Ptr child_cast = AST::cast_to<AST::DataType>(child);
                if(!result) {
                    result = std::make_shared<AST::Param>();
                }
                result->set_type(child_cast);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Param>(result);
}

AST::Node::Ptr YAMLImporter::convert_typeparam(const YAML::Node node) const
{
    AST::TypeParam::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::TypeParam>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::TypeParam>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property is_local
        if(node["is_local"]) {
            if(node["is_local"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::TypeParam>();
                }
                result->set_is_local(node["is_local"].as<bool>());
            }
        }
        // Manage property name
        if(node["name"]) {
            if(node["name"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::TypeParam>();
                }
                result->set_name(node["name"].as<std::string>());
            }
        }

        // Manage Child type
        if(node["type"]) {
            const YAML::Node node_type = node["type"];
            // Set the child
            AST::Node::Ptr child = convert(node_type);
            if(child) {
                AST::DataType::Ptr child_cast = AST::cast_to<AST::DataType>(child);
                if(!result) {
                    result = std::make_shared<AST::TypeParam>();
                }
                result->set_type(child_cast);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::TypeParam>(result);
}

AST::Node::Ptr YAMLImporter::convert_typedef(const YAML::Node node) const
{
    AST::Typedef::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Typedef>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Typedef>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property fwd_kind
        if(node["fwd_kind"]) {
            if(node["fwd_kind"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Typedef>();
                }
                result->set_fwd_kind(node["fwd_kind"].as<AST::Typedef::Fwd_kindEnum>());
            }
        }
        // Manage property name
        if(node["name"]) {
            if(node["name"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Typedef>();
                }
                result->set_name(node["name"].as<std::string>());
            }
        }

        // Manage Child unpacked_dims
        if(node["unpacked_dims"]) {
            const YAML::Node node_unpacked_dims = node["unpacked_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr unpacked_dims_list(new AST::Dimension::List);
            if(node_unpacked_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_unpacked_dims.begin();
                    it != node_unpacked_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        unpacked_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_unpacked_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    unpacked_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::Typedef>();
            }
            result->set_unpacked_dims(unpacked_dims_list);
        }

        // Manage Child type
        if(node["type"]) {
            const YAML::Node node_type = node["type"];
            // Set the child
            AST::Node::Ptr child = convert(node_type);
            if(child) {
                AST::DataType::Ptr child_cast = AST::cast_to<AST::DataType>(child);
                if(!result) {
                    result = std::make_shared<AST::Typedef>();
                }
                result->set_type(child_cast);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Typedef>(result);
}

AST::Node::Ptr YAMLImporter::convert_member(const YAML::Node node) const
{
    AST::Member::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Member>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Member>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property name
        if(node["name"]) {
            if(node["name"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Member>();
                }
                result->set_name(node["name"].as<std::string>());
            }
        }

        // Manage Child unpacked_dims
        if(node["unpacked_dims"]) {
            const YAML::Node node_unpacked_dims = node["unpacked_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr unpacked_dims_list(new AST::Dimension::List);
            if(node_unpacked_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_unpacked_dims.begin();
                    it != node_unpacked_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        unpacked_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_unpacked_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    unpacked_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::Member>();
            }
            result->set_unpacked_dims(unpacked_dims_list);
        }

        // Manage Child init
        if(node["init"]) {
            const YAML::Node node_init = node["init"];
            // Set the child
            AST::Node::Ptr child = convert(node_init);
            if(child) {
                AST::Rvalue::Ptr child_cast = AST::cast_to<AST::Rvalue>(child);
                if(!result) {
                    result = std::make_shared<AST::Member>();
                }
                result->set_init(child_cast);
            }
        }

        // Manage Child type
        if(node["type"]) {
            const YAML::Node node_type = node["type"];
            // Set the child
            AST::Node::Ptr child = convert(node_type);
            if(child) {
                AST::DataType::Ptr child_cast = AST::cast_to<AST::DataType>(child);
                if(!result) {
                    result = std::make_shared<AST::Member>();
                }
                result->set_type(child_cast);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Member>(result);
}

AST::Node::Ptr YAMLImporter::convert_arg(const YAML::Node node) const
{
    AST::Arg::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Arg>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Arg>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property is_var
        if(node["is_var"]) {
            if(node["is_var"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Arg>();
                }
                result->set_is_var(node["is_var"].as<bool>());
            }
        }
        // Manage property direction
        if(node["direction"]) {
            if(node["direction"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Arg>();
                }
                result->set_direction(node["direction"].as<AST::Arg::DirectionEnum>());
            }
        }
        // Manage property name
        if(node["name"]) {
            if(node["name"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Arg>();
                }
                result->set_name(node["name"].as<std::string>());
            }
        }

        // Manage Child unpacked_dims
        if(node["unpacked_dims"]) {
            const YAML::Node node_unpacked_dims = node["unpacked_dims"];
            // Fill the list of children
            AST::Dimension::ListPtr unpacked_dims_list(new AST::Dimension::List);
            if(node_unpacked_dims.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_unpacked_dims.begin();
                    it != node_unpacked_dims.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                        unpacked_dims_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_unpacked_dims);
                if(child) {
                    AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                    unpacked_dims_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::Arg>();
            }
            result->set_unpacked_dims(unpacked_dims_list);
        }

        // Manage Child default_value
        if(node["default_value"]) {
            const YAML::Node node_default_value = node["default_value"];
            // Set the child
            AST::Node::Ptr child = convert(node_default_value);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Arg>();
                }
                result->set_default_value(child);
            }
        }

        // Manage Child type
        if(node["type"]) {
            const YAML::Node node_type = node["type"];
            // Set the child
            AST::Node::Ptr child = convert(node_type);
            if(child) {
                AST::DataType::Ptr child_cast = AST::cast_to<AST::DataType>(child);
                if(!result) {
                    result = std::make_shared<AST::Arg>();
                }
                result->set_type(child_cast);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Arg>(result);
}

AST::Node::Ptr YAMLImporter::convert_genvar(const YAML::Node node) const
{
    AST::Genvar::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Genvar>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Genvar>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property name
        if(node["name"]) {
            if(node["name"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Genvar>();
                }
                result->set_name(node["name"].as<std::string>());
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Genvar>(result);
}

AST::Node::Ptr YAMLImporter::convert_concat(const YAML::Node node) const
{
    AST::Concat::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Concat>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Concat>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child list
        if(node["list"]) {
            const YAML::Node node_list = node["list"];
            // Fill the list of children
            AST::Node::ListPtr list_list(new AST::Node::List);
            if(node_list.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_list.begin(); it != node_list.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        list_list->push_back(child);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_list);
                if(child) {
                    list_list->push_back(child);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::Concat>();
            }
            result->set_list(list_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::Concat>(result);
}

AST::Node::Ptr YAMLImporter::convert_lconcat(const YAML::Node node) const
{
    AST::Lconcat::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Lconcat>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Lconcat>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child list
        if(node["list"]) {
            const YAML::Node node_list = node["list"];
            // Fill the list of children
            AST::Node::ListPtr list_list(new AST::Node::List);
            if(node_list.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_list.begin(); it != node_list.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        list_list->push_back(child);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_list);
                if(child) {
                    list_list->push_back(child);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::Lconcat>();
            }
            result->set_list(list_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::Lconcat>(result);
}

AST::Node::Ptr YAMLImporter::convert_repeat(const YAML::Node node) const
{
    AST::Repeat::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Repeat>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Repeat>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child value
        if(node["value"]) {
            const YAML::Node node_value = node["value"];
            // Set the child
            AST::Node::Ptr child = convert(node_value);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Repeat>();
                }
                result->set_value(child);
            }
        }

        // Manage Child times
        if(node["times"]) {
            const YAML::Node node_times = node["times"];
            // Set the child
            AST::Node::Ptr child = convert(node_times);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Repeat>();
                }
                result->set_times(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Repeat>(result);
}

AST::Node::Ptr YAMLImporter::convert_assignmentpattern(const YAML::Node node) const
{
    AST::AssignmentPattern::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::AssignmentPattern>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::AssignmentPattern>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child items
        if(node["items"]) {
            const YAML::Node node_items = node["items"];
            // Fill the list of children
            AST::Node::ListPtr items_list(new AST::Node::List);
            if(node_items.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_items.begin(); it != node_items.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        items_list->push_back(child);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_items);
                if(child) {
                    items_list->push_back(child);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::AssignmentPattern>();
            }
            result->set_items(items_list);
        }

        // Manage Child times
        if(node["times"]) {
            const YAML::Node node_times = node["times"];
            // Set the child
            AST::Node::Ptr child = convert(node_times);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::AssignmentPattern>();
                }
                result->set_times(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::AssignmentPattern>(result);
}

AST::Node::Ptr YAMLImporter::convert_patternitem(const YAML::Node node) const
{
    AST::PatternItem::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::PatternItem>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::PatternItem>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property is_default
        if(node["is_default"]) {
            if(node["is_default"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::PatternItem>();
                }
                result->set_is_default(node["is_default"].as<bool>());
            }
        }

        // Manage Child key
        if(node["key"]) {
            const YAML::Node node_key = node["key"];
            // Set the child
            AST::Node::Ptr child = convert(node_key);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::PatternItem>();
                }
                result->set_key(child);
            }
        }

        // Manage Child value
        if(node["value"]) {
            const YAML::Node node_value = node["value"];
            // Set the child
            AST::Node::Ptr child = convert(node_value);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::PatternItem>();
                }
                result->set_value(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::PatternItem>(result);
}

AST::Node::Ptr YAMLImporter::convert_cast(const YAML::Node node) const
{
    AST::Cast::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Cast>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Cast>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child expr
        if(node["expr"]) {
            const YAML::Node node_expr = node["expr"];
            // Set the child
            AST::Node::Ptr child = convert(node_expr);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Cast>();
                }
                result->set_expr(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Cast>(result);
}

AST::Node::Ptr YAMLImporter::convert_typecast(const YAML::Node node) const
{
    AST::TypeCast::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::TypeCast>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::TypeCast>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child target
        if(node["target"]) {
            const YAML::Node node_target = node["target"];
            // Set the child
            AST::Node::Ptr child = convert(node_target);
            if(child) {
                AST::DataType::Ptr child_cast = AST::cast_to<AST::DataType>(child);
                if(!result) {
                    result = std::make_shared<AST::TypeCast>();
                }
                result->set_target(child_cast);
            }
        }

        // Manage Child expr
        if(node["expr"]) {
            const YAML::Node node_expr = node["expr"];
            // Set the child
            AST::Node::Ptr child = convert(node_expr);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::TypeCast>();
                }
                result->set_expr(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::TypeCast>(result);
}

AST::Node::Ptr YAMLImporter::convert_sizecast(const YAML::Node node) const
{
    AST::SizeCast::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::SizeCast>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::SizeCast>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child size
        if(node["size"]) {
            const YAML::Node node_size = node["size"];
            // Set the child
            AST::Node::Ptr child = convert(node_size);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::SizeCast>();
                }
                result->set_size(child);
            }
        }

        // Manage Child expr
        if(node["expr"]) {
            const YAML::Node node_expr = node["expr"];
            // Set the child
            AST::Node::Ptr child = convert(node_expr);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::SizeCast>();
                }
                result->set_expr(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::SizeCast>(result);
}

AST::Node::Ptr YAMLImporter::convert_signingcast(const YAML::Node node) const
{
    AST::SigningCast::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::SigningCast>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::SigningCast>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property signing
        if(node["signing"]) {
            if(node["signing"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::SigningCast>();
                }
                result->set_signing(node["signing"].as<AST::SigningCast::SigningEnum>());
            }
        }

        // Manage Child expr
        if(node["expr"]) {
            const YAML::Node node_expr = node["expr"];
            // Set the child
            AST::Node::Ptr child = convert(node_expr);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::SigningCast>();
                }
                result->set_expr(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::SigningCast>(result);
}

AST::Node::Ptr YAMLImporter::convert_constcast(const YAML::Node node) const
{
    AST::ConstCast::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::ConstCast>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::ConstCast>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child expr
        if(node["expr"]) {
            const YAML::Node node_expr = node["expr"];
            // Set the child
            AST::Node::Ptr child = convert(node_expr);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::ConstCast>();
                }
                result->set_expr(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::ConstCast>(result);
}

AST::Node::Ptr YAMLImporter::convert_indirect(const YAML::Node node) const
{
    AST::Indirect::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Indirect>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Indirect>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child var
        if(node["var"]) {
            const YAML::Node node_var = node["var"];
            // Set the child
            AST::Node::Ptr child = convert(node_var);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Indirect>();
                }
                result->set_var(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Indirect>(result);
}

AST::Node::Ptr YAMLImporter::convert_partselect(const YAML::Node node) const
{
    AST::Partselect::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Partselect>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Partselect>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child msb
        if(node["msb"]) {
            const YAML::Node node_msb = node["msb"];
            // Set the child
            AST::Node::Ptr child = convert(node_msb);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Partselect>();
                }
                result->set_msb(child);
            }
        }

        // Manage Child lsb
        if(node["lsb"]) {
            const YAML::Node node_lsb = node["lsb"];
            // Set the child
            AST::Node::Ptr child = convert(node_lsb);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Partselect>();
                }
                result->set_lsb(child);
            }
        }

        // Manage Child var
        if(node["var"]) {
            const YAML::Node node_var = node["var"];
            // Set the child
            AST::Node::Ptr child = convert(node_var);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Partselect>();
                }
                result->set_var(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Partselect>(result);
}

AST::Node::Ptr YAMLImporter::convert_partselectindexed(const YAML::Node node) const
{
    AST::PartselectIndexed::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::PartselectIndexed>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::PartselectIndexed>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child index
        if(node["index"]) {
            const YAML::Node node_index = node["index"];
            // Set the child
            AST::Node::Ptr child = convert(node_index);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::PartselectIndexed>();
                }
                result->set_index(child);
            }
        }

        // Manage Child size
        if(node["size"]) {
            const YAML::Node node_size = node["size"];
            // Set the child
            AST::Node::Ptr child = convert(node_size);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::PartselectIndexed>();
                }
                result->set_size(child);
            }
        }

        // Manage Child var
        if(node["var"]) {
            const YAML::Node node_var = node["var"];
            // Set the child
            AST::Node::Ptr child = convert(node_var);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::PartselectIndexed>();
                }
                result->set_var(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::PartselectIndexed>(result);
}

AST::Node::Ptr YAMLImporter::convert_partselectplusindexed(const YAML::Node node) const
{
    AST::PartselectPlusIndexed::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::PartselectPlusIndexed>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::PartselectPlusIndexed>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child index
        if(node["index"]) {
            const YAML::Node node_index = node["index"];
            // Set the child
            AST::Node::Ptr child = convert(node_index);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::PartselectPlusIndexed>();
                }
                result->set_index(child);
            }
        }

        // Manage Child size
        if(node["size"]) {
            const YAML::Node node_size = node["size"];
            // Set the child
            AST::Node::Ptr child = convert(node_size);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::PartselectPlusIndexed>();
                }
                result->set_size(child);
            }
        }

        // Manage Child var
        if(node["var"]) {
            const YAML::Node node_var = node["var"];
            // Set the child
            AST::Node::Ptr child = convert(node_var);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::PartselectPlusIndexed>();
                }
                result->set_var(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::PartselectPlusIndexed>(result);
}

AST::Node::Ptr YAMLImporter::convert_partselectminusindexed(const YAML::Node node) const
{
    AST::PartselectMinusIndexed::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::PartselectMinusIndexed>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::PartselectMinusIndexed>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child index
        if(node["index"]) {
            const YAML::Node node_index = node["index"];
            // Set the child
            AST::Node::Ptr child = convert(node_index);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::PartselectMinusIndexed>();
                }
                result->set_index(child);
            }
        }

        // Manage Child size
        if(node["size"]) {
            const YAML::Node node_size = node["size"];
            // Set the child
            AST::Node::Ptr child = convert(node_size);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::PartselectMinusIndexed>();
                }
                result->set_size(child);
            }
        }

        // Manage Child var
        if(node["var"]) {
            const YAML::Node node_var = node["var"];
            // Set the child
            AST::Node::Ptr child = convert(node_var);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::PartselectMinusIndexed>();
                }
                result->set_var(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::PartselectMinusIndexed>(result);
}

AST::Node::Ptr YAMLImporter::convert_pointer(const YAML::Node node) const
{
    AST::Pointer::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Pointer>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Pointer>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child ptr
        if(node["ptr"]) {
            const YAML::Node node_ptr = node["ptr"];
            // Set the child
            AST::Node::Ptr child = convert(node_ptr);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Pointer>();
                }
                result->set_ptr(child);
            }
        }

        // Manage Child var
        if(node["var"]) {
            const YAML::Node node_var = node["var"];
            // Set the child
            AST::Node::Ptr child = convert(node_var);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Pointer>();
                }
                result->set_var(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Pointer>(result);
}

AST::Node::Ptr YAMLImporter::convert_lvalue(const YAML::Node node) const
{
    AST::Lvalue::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Lvalue>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Lvalue>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child var
        if(node["var"]) {
            const YAML::Node node_var = node["var"];
            // Set the child
            AST::Node::Ptr child = convert(node_var);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Lvalue>();
                }
                result->set_var(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Lvalue>(result);
}

AST::Node::Ptr YAMLImporter::convert_rvalue(const YAML::Node node) const
{
    AST::Rvalue::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Rvalue>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Rvalue>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child var
        if(node["var"]) {
            const YAML::Node node_var = node["var"];
            // Set the child
            AST::Node::Ptr child = convert(node_var);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Rvalue>();
                }
                result->set_var(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Rvalue>(result);
}

AST::Node::Ptr YAMLImporter::convert_unaryoperator(const YAML::Node node) const
{
    AST::UnaryOperator::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::UnaryOperator>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::UnaryOperator>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child right
        if(node["right"]) {
            const YAML::Node node_right = node["right"];
            // Set the child
            AST::Node::Ptr child = convert(node_right);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::UnaryOperator>();
                }
                result->set_right(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::UnaryOperator>(result);
}

AST::Node::Ptr YAMLImporter::convert_uplus(const YAML::Node node) const
{
    AST::Uplus::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Uplus>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Uplus>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child right
        if(node["right"]) {
            const YAML::Node node_right = node["right"];
            // Set the child
            AST::Node::Ptr child = convert(node_right);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Uplus>();
                }
                result->set_right(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Uplus>(result);
}

AST::Node::Ptr YAMLImporter::convert_uminus(const YAML::Node node) const
{
    AST::Uminus::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Uminus>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Uminus>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child right
        if(node["right"]) {
            const YAML::Node node_right = node["right"];
            // Set the child
            AST::Node::Ptr child = convert(node_right);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Uminus>();
                }
                result->set_right(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Uminus>(result);
}

AST::Node::Ptr YAMLImporter::convert_ulnot(const YAML::Node node) const
{
    AST::Ulnot::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Ulnot>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Ulnot>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child right
        if(node["right"]) {
            const YAML::Node node_right = node["right"];
            // Set the child
            AST::Node::Ptr child = convert(node_right);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Ulnot>();
                }
                result->set_right(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Ulnot>(result);
}

AST::Node::Ptr YAMLImporter::convert_unot(const YAML::Node node) const
{
    AST::Unot::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Unot>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Unot>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child right
        if(node["right"]) {
            const YAML::Node node_right = node["right"];
            // Set the child
            AST::Node::Ptr child = convert(node_right);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Unot>();
                }
                result->set_right(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Unot>(result);
}

AST::Node::Ptr YAMLImporter::convert_uand(const YAML::Node node) const
{
    AST::Uand::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Uand>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Uand>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child right
        if(node["right"]) {
            const YAML::Node node_right = node["right"];
            // Set the child
            AST::Node::Ptr child = convert(node_right);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Uand>();
                }
                result->set_right(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Uand>(result);
}

AST::Node::Ptr YAMLImporter::convert_unand(const YAML::Node node) const
{
    AST::Unand::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Unand>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Unand>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child right
        if(node["right"]) {
            const YAML::Node node_right = node["right"];
            // Set the child
            AST::Node::Ptr child = convert(node_right);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Unand>();
                }
                result->set_right(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Unand>(result);
}

AST::Node::Ptr YAMLImporter::convert_uor(const YAML::Node node) const
{
    AST::Uor::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Uor>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Uor>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child right
        if(node["right"]) {
            const YAML::Node node_right = node["right"];
            // Set the child
            AST::Node::Ptr child = convert(node_right);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Uor>();
                }
                result->set_right(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Uor>(result);
}

AST::Node::Ptr YAMLImporter::convert_unor(const YAML::Node node) const
{
    AST::Unor::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Unor>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Unor>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child right
        if(node["right"]) {
            const YAML::Node node_right = node["right"];
            // Set the child
            AST::Node::Ptr child = convert(node_right);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Unor>();
                }
                result->set_right(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Unor>(result);
}

AST::Node::Ptr YAMLImporter::convert_uxor(const YAML::Node node) const
{
    AST::Uxor::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Uxor>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Uxor>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child right
        if(node["right"]) {
            const YAML::Node node_right = node["right"];
            // Set the child
            AST::Node::Ptr child = convert(node_right);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Uxor>();
                }
                result->set_right(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Uxor>(result);
}

AST::Node::Ptr YAMLImporter::convert_uxnor(const YAML::Node node) const
{
    AST::Uxnor::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Uxnor>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Uxnor>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child right
        if(node["right"]) {
            const YAML::Node node_right = node["right"];
            // Set the child
            AST::Node::Ptr child = convert(node_right);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Uxnor>();
                }
                result->set_right(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Uxnor>(result);
}

AST::Node::Ptr YAMLImporter::convert_operator(const YAML::Node node) const
{
    AST::Operator::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Operator>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Operator>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child left
        if(node["left"]) {
            const YAML::Node node_left = node["left"];
            // Set the child
            AST::Node::Ptr child = convert(node_left);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Operator>();
                }
                result->set_left(child);
            }
        }

        // Manage Child right
        if(node["right"]) {
            const YAML::Node node_right = node["right"];
            // Set the child
            AST::Node::Ptr child = convert(node_right);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Operator>();
                }
                result->set_right(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Operator>(result);
}

AST::Node::Ptr YAMLImporter::convert_power(const YAML::Node node) const
{
    AST::Power::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Power>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Power>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child left
        if(node["left"]) {
            const YAML::Node node_left = node["left"];
            // Set the child
            AST::Node::Ptr child = convert(node_left);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Power>();
                }
                result->set_left(child);
            }
        }

        // Manage Child right
        if(node["right"]) {
            const YAML::Node node_right = node["right"];
            // Set the child
            AST::Node::Ptr child = convert(node_right);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Power>();
                }
                result->set_right(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Power>(result);
}

AST::Node::Ptr YAMLImporter::convert_times(const YAML::Node node) const
{
    AST::Times::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Times>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Times>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child left
        if(node["left"]) {
            const YAML::Node node_left = node["left"];
            // Set the child
            AST::Node::Ptr child = convert(node_left);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Times>();
                }
                result->set_left(child);
            }
        }

        // Manage Child right
        if(node["right"]) {
            const YAML::Node node_right = node["right"];
            // Set the child
            AST::Node::Ptr child = convert(node_right);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Times>();
                }
                result->set_right(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Times>(result);
}

AST::Node::Ptr YAMLImporter::convert_divide(const YAML::Node node) const
{
    AST::Divide::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Divide>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Divide>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child left
        if(node["left"]) {
            const YAML::Node node_left = node["left"];
            // Set the child
            AST::Node::Ptr child = convert(node_left);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Divide>();
                }
                result->set_left(child);
            }
        }

        // Manage Child right
        if(node["right"]) {
            const YAML::Node node_right = node["right"];
            // Set the child
            AST::Node::Ptr child = convert(node_right);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Divide>();
                }
                result->set_right(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Divide>(result);
}

AST::Node::Ptr YAMLImporter::convert_mod(const YAML::Node node) const
{
    AST::Mod::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Mod>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Mod>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child left
        if(node["left"]) {
            const YAML::Node node_left = node["left"];
            // Set the child
            AST::Node::Ptr child = convert(node_left);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Mod>();
                }
                result->set_left(child);
            }
        }

        // Manage Child right
        if(node["right"]) {
            const YAML::Node node_right = node["right"];
            // Set the child
            AST::Node::Ptr child = convert(node_right);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Mod>();
                }
                result->set_right(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Mod>(result);
}

AST::Node::Ptr YAMLImporter::convert_plus(const YAML::Node node) const
{
    AST::Plus::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Plus>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Plus>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child left
        if(node["left"]) {
            const YAML::Node node_left = node["left"];
            // Set the child
            AST::Node::Ptr child = convert(node_left);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Plus>();
                }
                result->set_left(child);
            }
        }

        // Manage Child right
        if(node["right"]) {
            const YAML::Node node_right = node["right"];
            // Set the child
            AST::Node::Ptr child = convert(node_right);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Plus>();
                }
                result->set_right(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Plus>(result);
}

AST::Node::Ptr YAMLImporter::convert_minus(const YAML::Node node) const
{
    AST::Minus::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Minus>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Minus>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child left
        if(node["left"]) {
            const YAML::Node node_left = node["left"];
            // Set the child
            AST::Node::Ptr child = convert(node_left);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Minus>();
                }
                result->set_left(child);
            }
        }

        // Manage Child right
        if(node["right"]) {
            const YAML::Node node_right = node["right"];
            // Set the child
            AST::Node::Ptr child = convert(node_right);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Minus>();
                }
                result->set_right(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Minus>(result);
}

AST::Node::Ptr YAMLImporter::convert_sll(const YAML::Node node) const
{
    AST::Sll::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Sll>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Sll>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child left
        if(node["left"]) {
            const YAML::Node node_left = node["left"];
            // Set the child
            AST::Node::Ptr child = convert(node_left);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Sll>();
                }
                result->set_left(child);
            }
        }

        // Manage Child right
        if(node["right"]) {
            const YAML::Node node_right = node["right"];
            // Set the child
            AST::Node::Ptr child = convert(node_right);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Sll>();
                }
                result->set_right(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Sll>(result);
}

AST::Node::Ptr YAMLImporter::convert_srl(const YAML::Node node) const
{
    AST::Srl::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Srl>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Srl>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child left
        if(node["left"]) {
            const YAML::Node node_left = node["left"];
            // Set the child
            AST::Node::Ptr child = convert(node_left);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Srl>();
                }
                result->set_left(child);
            }
        }

        // Manage Child right
        if(node["right"]) {
            const YAML::Node node_right = node["right"];
            // Set the child
            AST::Node::Ptr child = convert(node_right);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Srl>();
                }
                result->set_right(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Srl>(result);
}

AST::Node::Ptr YAMLImporter::convert_sra(const YAML::Node node) const
{
    AST::Sra::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Sra>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Sra>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child left
        if(node["left"]) {
            const YAML::Node node_left = node["left"];
            // Set the child
            AST::Node::Ptr child = convert(node_left);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Sra>();
                }
                result->set_left(child);
            }
        }

        // Manage Child right
        if(node["right"]) {
            const YAML::Node node_right = node["right"];
            // Set the child
            AST::Node::Ptr child = convert(node_right);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Sra>();
                }
                result->set_right(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Sra>(result);
}

AST::Node::Ptr YAMLImporter::convert_lessthan(const YAML::Node node) const
{
    AST::LessThan::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::LessThan>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::LessThan>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child left
        if(node["left"]) {
            const YAML::Node node_left = node["left"];
            // Set the child
            AST::Node::Ptr child = convert(node_left);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::LessThan>();
                }
                result->set_left(child);
            }
        }

        // Manage Child right
        if(node["right"]) {
            const YAML::Node node_right = node["right"];
            // Set the child
            AST::Node::Ptr child = convert(node_right);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::LessThan>();
                }
                result->set_right(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::LessThan>(result);
}

AST::Node::Ptr YAMLImporter::convert_greaterthan(const YAML::Node node) const
{
    AST::GreaterThan::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::GreaterThan>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::GreaterThan>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child left
        if(node["left"]) {
            const YAML::Node node_left = node["left"];
            // Set the child
            AST::Node::Ptr child = convert(node_left);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::GreaterThan>();
                }
                result->set_left(child);
            }
        }

        // Manage Child right
        if(node["right"]) {
            const YAML::Node node_right = node["right"];
            // Set the child
            AST::Node::Ptr child = convert(node_right);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::GreaterThan>();
                }
                result->set_right(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::GreaterThan>(result);
}

AST::Node::Ptr YAMLImporter::convert_lesseq(const YAML::Node node) const
{
    AST::LessEq::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::LessEq>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::LessEq>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child left
        if(node["left"]) {
            const YAML::Node node_left = node["left"];
            // Set the child
            AST::Node::Ptr child = convert(node_left);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::LessEq>();
                }
                result->set_left(child);
            }
        }

        // Manage Child right
        if(node["right"]) {
            const YAML::Node node_right = node["right"];
            // Set the child
            AST::Node::Ptr child = convert(node_right);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::LessEq>();
                }
                result->set_right(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::LessEq>(result);
}

AST::Node::Ptr YAMLImporter::convert_greatereq(const YAML::Node node) const
{
    AST::GreaterEq::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::GreaterEq>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::GreaterEq>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child left
        if(node["left"]) {
            const YAML::Node node_left = node["left"];
            // Set the child
            AST::Node::Ptr child = convert(node_left);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::GreaterEq>();
                }
                result->set_left(child);
            }
        }

        // Manage Child right
        if(node["right"]) {
            const YAML::Node node_right = node["right"];
            // Set the child
            AST::Node::Ptr child = convert(node_right);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::GreaterEq>();
                }
                result->set_right(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::GreaterEq>(result);
}

AST::Node::Ptr YAMLImporter::convert_eq(const YAML::Node node) const
{
    AST::Eq::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Eq>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Eq>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child left
        if(node["left"]) {
            const YAML::Node node_left = node["left"];
            // Set the child
            AST::Node::Ptr child = convert(node_left);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Eq>();
                }
                result->set_left(child);
            }
        }

        // Manage Child right
        if(node["right"]) {
            const YAML::Node node_right = node["right"];
            // Set the child
            AST::Node::Ptr child = convert(node_right);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Eq>();
                }
                result->set_right(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Eq>(result);
}

AST::Node::Ptr YAMLImporter::convert_noteq(const YAML::Node node) const
{
    AST::NotEq::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::NotEq>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::NotEq>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child left
        if(node["left"]) {
            const YAML::Node node_left = node["left"];
            // Set the child
            AST::Node::Ptr child = convert(node_left);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::NotEq>();
                }
                result->set_left(child);
            }
        }

        // Manage Child right
        if(node["right"]) {
            const YAML::Node node_right = node["right"];
            // Set the child
            AST::Node::Ptr child = convert(node_right);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::NotEq>();
                }
                result->set_right(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::NotEq>(result);
}

AST::Node::Ptr YAMLImporter::convert_eql(const YAML::Node node) const
{
    AST::Eql::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Eql>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Eql>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child left
        if(node["left"]) {
            const YAML::Node node_left = node["left"];
            // Set the child
            AST::Node::Ptr child = convert(node_left);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Eql>();
                }
                result->set_left(child);
            }
        }

        // Manage Child right
        if(node["right"]) {
            const YAML::Node node_right = node["right"];
            // Set the child
            AST::Node::Ptr child = convert(node_right);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Eql>();
                }
                result->set_right(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Eql>(result);
}

AST::Node::Ptr YAMLImporter::convert_noteql(const YAML::Node node) const
{
    AST::NotEql::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::NotEql>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::NotEql>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child left
        if(node["left"]) {
            const YAML::Node node_left = node["left"];
            // Set the child
            AST::Node::Ptr child = convert(node_left);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::NotEql>();
                }
                result->set_left(child);
            }
        }

        // Manage Child right
        if(node["right"]) {
            const YAML::Node node_right = node["right"];
            // Set the child
            AST::Node::Ptr child = convert(node_right);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::NotEql>();
                }
                result->set_right(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::NotEql>(result);
}

AST::Node::Ptr YAMLImporter::convert_and(const YAML::Node node) const
{
    AST::And::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::And>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::And>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child left
        if(node["left"]) {
            const YAML::Node node_left = node["left"];
            // Set the child
            AST::Node::Ptr child = convert(node_left);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::And>();
                }
                result->set_left(child);
            }
        }

        // Manage Child right
        if(node["right"]) {
            const YAML::Node node_right = node["right"];
            // Set the child
            AST::Node::Ptr child = convert(node_right);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::And>();
                }
                result->set_right(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::And>(result);
}

AST::Node::Ptr YAMLImporter::convert_xor(const YAML::Node node) const
{
    AST::Xor::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Xor>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Xor>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child left
        if(node["left"]) {
            const YAML::Node node_left = node["left"];
            // Set the child
            AST::Node::Ptr child = convert(node_left);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Xor>();
                }
                result->set_left(child);
            }
        }

        // Manage Child right
        if(node["right"]) {
            const YAML::Node node_right = node["right"];
            // Set the child
            AST::Node::Ptr child = convert(node_right);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Xor>();
                }
                result->set_right(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Xor>(result);
}

AST::Node::Ptr YAMLImporter::convert_xnor(const YAML::Node node) const
{
    AST::Xnor::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Xnor>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Xnor>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child left
        if(node["left"]) {
            const YAML::Node node_left = node["left"];
            // Set the child
            AST::Node::Ptr child = convert(node_left);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Xnor>();
                }
                result->set_left(child);
            }
        }

        // Manage Child right
        if(node["right"]) {
            const YAML::Node node_right = node["right"];
            // Set the child
            AST::Node::Ptr child = convert(node_right);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Xnor>();
                }
                result->set_right(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Xnor>(result);
}

AST::Node::Ptr YAMLImporter::convert_or(const YAML::Node node) const
{
    AST::Or::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Or>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Or>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child left
        if(node["left"]) {
            const YAML::Node node_left = node["left"];
            // Set the child
            AST::Node::Ptr child = convert(node_left);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Or>();
                }
                result->set_left(child);
            }
        }

        // Manage Child right
        if(node["right"]) {
            const YAML::Node node_right = node["right"];
            // Set the child
            AST::Node::Ptr child = convert(node_right);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Or>();
                }
                result->set_right(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Or>(result);
}

AST::Node::Ptr YAMLImporter::convert_land(const YAML::Node node) const
{
    AST::Land::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Land>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Land>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child left
        if(node["left"]) {
            const YAML::Node node_left = node["left"];
            // Set the child
            AST::Node::Ptr child = convert(node_left);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Land>();
                }
                result->set_left(child);
            }
        }

        // Manage Child right
        if(node["right"]) {
            const YAML::Node node_right = node["right"];
            // Set the child
            AST::Node::Ptr child = convert(node_right);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Land>();
                }
                result->set_right(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Land>(result);
}

AST::Node::Ptr YAMLImporter::convert_lor(const YAML::Node node) const
{
    AST::Lor::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Lor>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Lor>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child left
        if(node["left"]) {
            const YAML::Node node_left = node["left"];
            // Set the child
            AST::Node::Ptr child = convert(node_left);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Lor>();
                }
                result->set_left(child);
            }
        }

        // Manage Child right
        if(node["right"]) {
            const YAML::Node node_right = node["right"];
            // Set the child
            AST::Node::Ptr child = convert(node_right);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Lor>();
                }
                result->set_right(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Lor>(result);
}

AST::Node::Ptr YAMLImporter::convert_cond(const YAML::Node node) const
{
    AST::Cond::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Cond>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Cond>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child cond
        if(node["cond"]) {
            const YAML::Node node_cond = node["cond"];
            // Set the child
            AST::Node::Ptr child = convert(node_cond);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Cond>();
                }
                result->set_cond(child);
            }
        }

        // Manage Child left
        if(node["left"]) {
            const YAML::Node node_left = node["left"];
            // Set the child
            AST::Node::Ptr child = convert(node_left);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Cond>();
                }
                result->set_left(child);
            }
        }

        // Manage Child right
        if(node["right"]) {
            const YAML::Node node_right = node["right"];
            // Set the child
            AST::Node::Ptr child = convert(node_right);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Cond>();
                }
                result->set_right(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Cond>(result);
}

AST::Node::Ptr YAMLImporter::convert_always(const YAML::Node node) const
{
    AST::Always::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Always>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Always>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child senslist
        if(node["senslist"]) {
            const YAML::Node node_senslist = node["senslist"];
            // Set the child
            AST::Node::Ptr child = convert(node_senslist);
            if(child) {
                AST::Senslist::Ptr child_cast = AST::cast_to<AST::Senslist>(child);
                if(!result) {
                    result = std::make_shared<AST::Always>();
                }
                result->set_senslist(child_cast);
            }
        }

        // Manage Child statement
        if(node["statement"]) {
            const YAML::Node node_statement = node["statement"];
            // Set the child
            AST::Node::Ptr child = convert(node_statement);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Always>();
                }
                result->set_statement(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Always>(result);
}

AST::Node::Ptr YAMLImporter::convert_alwaysff(const YAML::Node node) const
{
    AST::AlwaysFF::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::AlwaysFF>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::AlwaysFF>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child senslist
        if(node["senslist"]) {
            const YAML::Node node_senslist = node["senslist"];
            // Set the child
            AST::Node::Ptr child = convert(node_senslist);
            if(child) {
                AST::Senslist::Ptr child_cast = AST::cast_to<AST::Senslist>(child);
                if(!result) {
                    result = std::make_shared<AST::AlwaysFF>();
                }
                result->set_senslist(child_cast);
            }
        }

        // Manage Child statement
        if(node["statement"]) {
            const YAML::Node node_statement = node["statement"];
            // Set the child
            AST::Node::Ptr child = convert(node_statement);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::AlwaysFF>();
                }
                result->set_statement(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::AlwaysFF>(result);
}

AST::Node::Ptr YAMLImporter::convert_alwayscomb(const YAML::Node node) const
{
    AST::AlwaysComb::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::AlwaysComb>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::AlwaysComb>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child senslist
        if(node["senslist"]) {
            const YAML::Node node_senslist = node["senslist"];
            // Set the child
            AST::Node::Ptr child = convert(node_senslist);
            if(child) {
                AST::Senslist::Ptr child_cast = AST::cast_to<AST::Senslist>(child);
                if(!result) {
                    result = std::make_shared<AST::AlwaysComb>();
                }
                result->set_senslist(child_cast);
            }
        }

        // Manage Child statement
        if(node["statement"]) {
            const YAML::Node node_statement = node["statement"];
            // Set the child
            AST::Node::Ptr child = convert(node_statement);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::AlwaysComb>();
                }
                result->set_statement(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::AlwaysComb>(result);
}

AST::Node::Ptr YAMLImporter::convert_alwayslatch(const YAML::Node node) const
{
    AST::AlwaysLatch::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::AlwaysLatch>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::AlwaysLatch>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child senslist
        if(node["senslist"]) {
            const YAML::Node node_senslist = node["senslist"];
            // Set the child
            AST::Node::Ptr child = convert(node_senslist);
            if(child) {
                AST::Senslist::Ptr child_cast = AST::cast_to<AST::Senslist>(child);
                if(!result) {
                    result = std::make_shared<AST::AlwaysLatch>();
                }
                result->set_senslist(child_cast);
            }
        }

        // Manage Child statement
        if(node["statement"]) {
            const YAML::Node node_statement = node["statement"];
            // Set the child
            AST::Node::Ptr child = convert(node_statement);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::AlwaysLatch>();
                }
                result->set_statement(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::AlwaysLatch>(result);
}

AST::Node::Ptr YAMLImporter::convert_senslist(const YAML::Node node) const
{
    AST::Senslist::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Senslist>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Senslist>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child list
        if(node["list"]) {
            const YAML::Node node_list = node["list"];
            // Fill the list of children
            AST::Sens::ListPtr list_list(new AST::Sens::List);
            if(node_list.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_list.begin(); it != node_list.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Sens::Ptr child_cast = AST::cast_to<AST::Sens>(child);
                        list_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_list);
                if(child) {
                    AST::Sens::Ptr child_cast = AST::cast_to<AST::Sens>(child);
                    list_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::Senslist>();
            }
            result->set_list(list_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::Senslist>(result);
}

AST::Node::Ptr YAMLImporter::convert_sens(const YAML::Node node) const
{
    AST::Sens::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Sens>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Sens>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property type
        if(node["type"]) {
            if(node["type"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Sens>();
                }
                result->set_type(node["type"].as<AST::Sens::TypeEnum>());
            }
        }

        // Manage Child sig
        if(node["sig"]) {
            const YAML::Node node_sig = node["sig"];
            // Set the child
            AST::Node::Ptr child = convert(node_sig);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Sens>();
                }
                result->set_sig(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Sens>(result);
}

AST::Node::Ptr YAMLImporter::convert_defparamlist(const YAML::Node node) const
{
    AST::Defparamlist::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Defparamlist>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Defparamlist>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child list
        if(node["list"]) {
            const YAML::Node node_list = node["list"];
            // Fill the list of children
            AST::Defparam::ListPtr list_list(new AST::Defparam::List);
            if(node_list.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_list.begin(); it != node_list.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Defparam::Ptr child_cast = AST::cast_to<AST::Defparam>(child);
                        list_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_list);
                if(child) {
                    AST::Defparam::Ptr child_cast = AST::cast_to<AST::Defparam>(child);
                    list_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::Defparamlist>();
            }
            result->set_list(list_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::Defparamlist>(result);
}

AST::Node::Ptr YAMLImporter::convert_defparam(const YAML::Node node) const
{
    AST::Defparam::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Defparam>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Defparam>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child identifier
        if(node["identifier"]) {
            const YAML::Node node_identifier = node["identifier"];
            // Set the child
            AST::Node::Ptr child = convert(node_identifier);
            if(child) {
                AST::Identifier::Ptr child_cast = AST::cast_to<AST::Identifier>(child);
                if(!result) {
                    result = std::make_shared<AST::Defparam>();
                }
                result->set_identifier(child_cast);
            }
        }

        // Manage Child right
        if(node["right"]) {
            const YAML::Node node_right = node["right"];
            // Set the child
            AST::Node::Ptr child = convert(node_right);
            if(child) {
                AST::Rvalue::Ptr child_cast = AST::cast_to<AST::Rvalue>(child);
                if(!result) {
                    result = std::make_shared<AST::Defparam>();
                }
                result->set_right(child_cast);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Defparam>(result);
}

AST::Node::Ptr YAMLImporter::convert_assign(const YAML::Node node) const
{
    AST::Assign::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Assign>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Assign>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child left
        if(node["left"]) {
            const YAML::Node node_left = node["left"];
            // Set the child
            AST::Node::Ptr child = convert(node_left);
            if(child) {
                AST::Lvalue::Ptr child_cast = AST::cast_to<AST::Lvalue>(child);
                if(!result) {
                    result = std::make_shared<AST::Assign>();
                }
                result->set_left(child_cast);
            }
        }

        // Manage Child right
        if(node["right"]) {
            const YAML::Node node_right = node["right"];
            // Set the child
            AST::Node::Ptr child = convert(node_right);
            if(child) {
                AST::Rvalue::Ptr child_cast = AST::cast_to<AST::Rvalue>(child);
                if(!result) {
                    result = std::make_shared<AST::Assign>();
                }
                result->set_right(child_cast);
            }
        }

        // Manage Child ldelay
        if(node["ldelay"]) {
            const YAML::Node node_ldelay = node["ldelay"];
            // Set the child
            AST::Node::Ptr child = convert(node_ldelay);
            if(child) {
                AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
                if(!result) {
                    result = std::make_shared<AST::Assign>();
                }
                result->set_ldelay(child_cast);
            }
        }

        // Manage Child rdelay
        if(node["rdelay"]) {
            const YAML::Node node_rdelay = node["rdelay"];
            // Set the child
            AST::Node::Ptr child = convert(node_rdelay);
            if(child) {
                AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
                if(!result) {
                    result = std::make_shared<AST::Assign>();
                }
                result->set_rdelay(child_cast);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Assign>(result);
}

AST::Node::Ptr YAMLImporter::convert_blockingsubstitution(const YAML::Node node) const
{
    AST::BlockingSubstitution::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::BlockingSubstitution>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::BlockingSubstitution>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child left
        if(node["left"]) {
            const YAML::Node node_left = node["left"];
            // Set the child
            AST::Node::Ptr child = convert(node_left);
            if(child) {
                AST::Lvalue::Ptr child_cast = AST::cast_to<AST::Lvalue>(child);
                if(!result) {
                    result = std::make_shared<AST::BlockingSubstitution>();
                }
                result->set_left(child_cast);
            }
        }

        // Manage Child right
        if(node["right"]) {
            const YAML::Node node_right = node["right"];
            // Set the child
            AST::Node::Ptr child = convert(node_right);
            if(child) {
                AST::Rvalue::Ptr child_cast = AST::cast_to<AST::Rvalue>(child);
                if(!result) {
                    result = std::make_shared<AST::BlockingSubstitution>();
                }
                result->set_right(child_cast);
            }
        }

        // Manage Child ldelay
        if(node["ldelay"]) {
            const YAML::Node node_ldelay = node["ldelay"];
            // Set the child
            AST::Node::Ptr child = convert(node_ldelay);
            if(child) {
                AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
                if(!result) {
                    result = std::make_shared<AST::BlockingSubstitution>();
                }
                result->set_ldelay(child_cast);
            }
        }

        // Manage Child rdelay
        if(node["rdelay"]) {
            const YAML::Node node_rdelay = node["rdelay"];
            // Set the child
            AST::Node::Ptr child = convert(node_rdelay);
            if(child) {
                AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
                if(!result) {
                    result = std::make_shared<AST::BlockingSubstitution>();
                }
                result->set_rdelay(child_cast);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::BlockingSubstitution>(result);
}

AST::Node::Ptr YAMLImporter::convert_nonblockingsubstitution(const YAML::Node node) const
{
    AST::NonblockingSubstitution::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::NonblockingSubstitution>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::NonblockingSubstitution>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child left
        if(node["left"]) {
            const YAML::Node node_left = node["left"];
            // Set the child
            AST::Node::Ptr child = convert(node_left);
            if(child) {
                AST::Lvalue::Ptr child_cast = AST::cast_to<AST::Lvalue>(child);
                if(!result) {
                    result = std::make_shared<AST::NonblockingSubstitution>();
                }
                result->set_left(child_cast);
            }
        }

        // Manage Child right
        if(node["right"]) {
            const YAML::Node node_right = node["right"];
            // Set the child
            AST::Node::Ptr child = convert(node_right);
            if(child) {
                AST::Rvalue::Ptr child_cast = AST::cast_to<AST::Rvalue>(child);
                if(!result) {
                    result = std::make_shared<AST::NonblockingSubstitution>();
                }
                result->set_right(child_cast);
            }
        }

        // Manage Child ldelay
        if(node["ldelay"]) {
            const YAML::Node node_ldelay = node["ldelay"];
            // Set the child
            AST::Node::Ptr child = convert(node_ldelay);
            if(child) {
                AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
                if(!result) {
                    result = std::make_shared<AST::NonblockingSubstitution>();
                }
                result->set_ldelay(child_cast);
            }
        }

        // Manage Child rdelay
        if(node["rdelay"]) {
            const YAML::Node node_rdelay = node["rdelay"];
            // Set the child
            AST::Node::Ptr child = convert(node_rdelay);
            if(child) {
                AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
                if(!result) {
                    result = std::make_shared<AST::NonblockingSubstitution>();
                }
                result->set_rdelay(child_cast);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::NonblockingSubstitution>(result);
}

AST::Node::Ptr YAMLImporter::convert_ifstatement(const YAML::Node node) const
{
    AST::IfStatement::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::IfStatement>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::IfStatement>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child cond
        if(node["cond"]) {
            const YAML::Node node_cond = node["cond"];
            // Set the child
            AST::Node::Ptr child = convert(node_cond);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::IfStatement>();
                }
                result->set_cond(child);
            }
        }

        // Manage Child true_statement
        if(node["true_statement"]) {
            const YAML::Node node_true_statement = node["true_statement"];
            // Set the child
            AST::Node::Ptr child = convert(node_true_statement);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::IfStatement>();
                }
                result->set_true_statement(child);
            }
        }

        // Manage Child false_statement
        if(node["false_statement"]) {
            const YAML::Node node_false_statement = node["false_statement"];
            // Set the child
            AST::Node::Ptr child = convert(node_false_statement);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::IfStatement>();
                }
                result->set_false_statement(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::IfStatement>(result);
}

AST::Node::Ptr YAMLImporter::convert_repeatstatement(const YAML::Node node) const
{
    AST::RepeatStatement::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::RepeatStatement>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::RepeatStatement>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child times
        if(node["times"]) {
            const YAML::Node node_times = node["times"];
            // Set the child
            AST::Node::Ptr child = convert(node_times);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::RepeatStatement>();
                }
                result->set_times(child);
            }
        }

        // Manage Child statement
        if(node["statement"]) {
            const YAML::Node node_statement = node["statement"];
            // Set the child
            AST::Node::Ptr child = convert(node_statement);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::RepeatStatement>();
                }
                result->set_statement(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::RepeatStatement>(result);
}

AST::Node::Ptr YAMLImporter::convert_forstatement(const YAML::Node node) const
{
    AST::ForStatement::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::ForStatement>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::ForStatement>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child pre
        if(node["pre"]) {
            const YAML::Node node_pre = node["pre"];
            // Set the child
            AST::Node::Ptr child = convert(node_pre);
            if(child) {
                AST::BlockingSubstitution::Ptr child_cast =
                    AST::cast_to<AST::BlockingSubstitution>(child);
                if(!result) {
                    result = std::make_shared<AST::ForStatement>();
                }
                result->set_pre(child_cast);
            }
        }

        // Manage Child cond
        if(node["cond"]) {
            const YAML::Node node_cond = node["cond"];
            // Set the child
            AST::Node::Ptr child = convert(node_cond);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::ForStatement>();
                }
                result->set_cond(child);
            }
        }

        // Manage Child post
        if(node["post"]) {
            const YAML::Node node_post = node["post"];
            // Set the child
            AST::Node::Ptr child = convert(node_post);
            if(child) {
                AST::BlockingSubstitution::Ptr child_cast =
                    AST::cast_to<AST::BlockingSubstitution>(child);
                if(!result) {
                    result = std::make_shared<AST::ForStatement>();
                }
                result->set_post(child_cast);
            }
        }

        // Manage Child statement
        if(node["statement"]) {
            const YAML::Node node_statement = node["statement"];
            // Set the child
            AST::Node::Ptr child = convert(node_statement);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::ForStatement>();
                }
                result->set_statement(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::ForStatement>(result);
}

AST::Node::Ptr YAMLImporter::convert_whilestatement(const YAML::Node node) const
{
    AST::WhileStatement::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::WhileStatement>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::WhileStatement>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child cond
        if(node["cond"]) {
            const YAML::Node node_cond = node["cond"];
            // Set the child
            AST::Node::Ptr child = convert(node_cond);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::WhileStatement>();
                }
                result->set_cond(child);
            }
        }

        // Manage Child statement
        if(node["statement"]) {
            const YAML::Node node_statement = node["statement"];
            // Set the child
            AST::Node::Ptr child = convert(node_statement);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::WhileStatement>();
                }
                result->set_statement(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::WhileStatement>(result);
}

AST::Node::Ptr YAMLImporter::convert_casestatement(const YAML::Node node) const
{
    AST::CaseStatement::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::CaseStatement>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::CaseStatement>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child comp
        if(node["comp"]) {
            const YAML::Node node_comp = node["comp"];
            // Set the child
            AST::Node::Ptr child = convert(node_comp);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::CaseStatement>();
                }
                result->set_comp(child);
            }
        }

        // Manage Child caselist
        if(node["caselist"]) {
            const YAML::Node node_caselist = node["caselist"];
            // Fill the list of children
            AST::Case::ListPtr caselist_list(new AST::Case::List);
            if(node_caselist.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_caselist.begin(); it != node_caselist.end();
                    ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Case::Ptr child_cast = AST::cast_to<AST::Case>(child);
                        caselist_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_caselist);
                if(child) {
                    AST::Case::Ptr child_cast = AST::cast_to<AST::Case>(child);
                    caselist_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::CaseStatement>();
            }
            result->set_caselist(caselist_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::CaseStatement>(result);
}

AST::Node::Ptr YAMLImporter::convert_casexstatement(const YAML::Node node) const
{
    AST::CasexStatement::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::CasexStatement>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::CasexStatement>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child comp
        if(node["comp"]) {
            const YAML::Node node_comp = node["comp"];
            // Set the child
            AST::Node::Ptr child = convert(node_comp);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::CasexStatement>();
                }
                result->set_comp(child);
            }
        }

        // Manage Child caselist
        if(node["caselist"]) {
            const YAML::Node node_caselist = node["caselist"];
            // Fill the list of children
            AST::Case::ListPtr caselist_list(new AST::Case::List);
            if(node_caselist.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_caselist.begin(); it != node_caselist.end();
                    ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Case::Ptr child_cast = AST::cast_to<AST::Case>(child);
                        caselist_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_caselist);
                if(child) {
                    AST::Case::Ptr child_cast = AST::cast_to<AST::Case>(child);
                    caselist_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::CasexStatement>();
            }
            result->set_caselist(caselist_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::CasexStatement>(result);
}

AST::Node::Ptr YAMLImporter::convert_casezstatement(const YAML::Node node) const
{
    AST::CasezStatement::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::CasezStatement>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::CasezStatement>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child comp
        if(node["comp"]) {
            const YAML::Node node_comp = node["comp"];
            // Set the child
            AST::Node::Ptr child = convert(node_comp);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::CasezStatement>();
                }
                result->set_comp(child);
            }
        }

        // Manage Child caselist
        if(node["caselist"]) {
            const YAML::Node node_caselist = node["caselist"];
            // Fill the list of children
            AST::Case::ListPtr caselist_list(new AST::Case::List);
            if(node_caselist.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_caselist.begin(); it != node_caselist.end();
                    ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Case::Ptr child_cast = AST::cast_to<AST::Case>(child);
                        caselist_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_caselist);
                if(child) {
                    AST::Case::Ptr child_cast = AST::cast_to<AST::Case>(child);
                    caselist_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::CasezStatement>();
            }
            result->set_caselist(caselist_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::CasezStatement>(result);
}

AST::Node::Ptr YAMLImporter::convert_uniquecasestatement(const YAML::Node node) const
{
    AST::UniqueCaseStatement::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::UniqueCaseStatement>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::UniqueCaseStatement>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child comp
        if(node["comp"]) {
            const YAML::Node node_comp = node["comp"];
            // Set the child
            AST::Node::Ptr child = convert(node_comp);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::UniqueCaseStatement>();
                }
                result->set_comp(child);
            }
        }

        // Manage Child caselist
        if(node["caselist"]) {
            const YAML::Node node_caselist = node["caselist"];
            // Fill the list of children
            AST::Case::ListPtr caselist_list(new AST::Case::List);
            if(node_caselist.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_caselist.begin(); it != node_caselist.end();
                    ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Case::Ptr child_cast = AST::cast_to<AST::Case>(child);
                        caselist_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_caselist);
                if(child) {
                    AST::Case::Ptr child_cast = AST::cast_to<AST::Case>(child);
                    caselist_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::UniqueCaseStatement>();
            }
            result->set_caselist(caselist_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::UniqueCaseStatement>(result);
}

AST::Node::Ptr YAMLImporter::convert_prioritycasestatement(const YAML::Node node) const
{
    AST::PriorityCaseStatement::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::PriorityCaseStatement>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::PriorityCaseStatement>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child comp
        if(node["comp"]) {
            const YAML::Node node_comp = node["comp"];
            // Set the child
            AST::Node::Ptr child = convert(node_comp);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::PriorityCaseStatement>();
                }
                result->set_comp(child);
            }
        }

        // Manage Child caselist
        if(node["caselist"]) {
            const YAML::Node node_caselist = node["caselist"];
            // Fill the list of children
            AST::Case::ListPtr caselist_list(new AST::Case::List);
            if(node_caselist.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_caselist.begin(); it != node_caselist.end();
                    ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Case::Ptr child_cast = AST::cast_to<AST::Case>(child);
                        caselist_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_caselist);
                if(child) {
                    AST::Case::Ptr child_cast = AST::cast_to<AST::Case>(child);
                    caselist_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::PriorityCaseStatement>();
            }
            result->set_caselist(caselist_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::PriorityCaseStatement>(result);
}

AST::Node::Ptr YAMLImporter::convert_case(const YAML::Node node) const
{
    AST::Case::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Case>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Case>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child cond
        if(node["cond"]) {
            const YAML::Node node_cond = node["cond"];
            // Fill the list of children
            AST::Node::ListPtr cond_list(new AST::Node::List);
            if(node_cond.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_cond.begin(); it != node_cond.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        cond_list->push_back(child);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_cond);
                if(child) {
                    cond_list->push_back(child);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::Case>();
            }
            result->set_cond(cond_list);
        }

        // Manage Child statement
        if(node["statement"]) {
            const YAML::Node node_statement = node["statement"];
            // Set the child
            AST::Node::Ptr child = convert(node_statement);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Case>();
                }
                result->set_statement(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Case>(result);
}

AST::Node::Ptr YAMLImporter::convert_block(const YAML::Node node) const
{
    AST::Block::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Block>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Block>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property scope
        if(node["scope"]) {
            if(node["scope"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Block>();
                }
                result->set_scope(node["scope"].as<std::string>());
            }
        }

        // Manage Child statements
        if(node["statements"]) {
            const YAML::Node node_statements = node["statements"];
            // Fill the list of children
            AST::Node::ListPtr statements_list(new AST::Node::List);
            if(node_statements.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_statements.begin(); it != node_statements.end();
                    ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        statements_list->push_back(child);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_statements);
                if(child) {
                    statements_list->push_back(child);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::Block>();
            }
            result->set_statements(statements_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::Block>(result);
}

AST::Node::Ptr YAMLImporter::convert_initial(const YAML::Node node) const
{
    AST::Initial::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Initial>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Initial>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child statement
        if(node["statement"]) {
            const YAML::Node node_statement = node["statement"];
            // Set the child
            AST::Node::Ptr child = convert(node_statement);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::Initial>();
                }
                result->set_statement(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Initial>(result);
}

AST::Node::Ptr YAMLImporter::convert_eventstatement(const YAML::Node node) const
{
    AST::EventStatement::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::EventStatement>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::EventStatement>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child senslist
        if(node["senslist"]) {
            const YAML::Node node_senslist = node["senslist"];
            // Set the child
            AST::Node::Ptr child = convert(node_senslist);
            if(child) {
                AST::Senslist::Ptr child_cast = AST::cast_to<AST::Senslist>(child);
                if(!result) {
                    result = std::make_shared<AST::EventStatement>();
                }
                result->set_senslist(child_cast);
            }
        }

        // Manage Child statement
        if(node["statement"]) {
            const YAML::Node node_statement = node["statement"];
            // Set the child
            AST::Node::Ptr child = convert(node_statement);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::EventStatement>();
                }
                result->set_statement(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::EventStatement>(result);
}

AST::Node::Ptr YAMLImporter::convert_waitstatement(const YAML::Node node) const
{
    AST::WaitStatement::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::WaitStatement>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::WaitStatement>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child cond
        if(node["cond"]) {
            const YAML::Node node_cond = node["cond"];
            // Set the child
            AST::Node::Ptr child = convert(node_cond);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::WaitStatement>();
                }
                result->set_cond(child);
            }
        }

        // Manage Child statement
        if(node["statement"]) {
            const YAML::Node node_statement = node["statement"];
            // Set the child
            AST::Node::Ptr child = convert(node_statement);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::WaitStatement>();
                }
                result->set_statement(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::WaitStatement>(result);
}

AST::Node::Ptr YAMLImporter::convert_foreverstatement(const YAML::Node node) const
{
    AST::ForeverStatement::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::ForeverStatement>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::ForeverStatement>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child statement
        if(node["statement"]) {
            const YAML::Node node_statement = node["statement"];
            // Set the child
            AST::Node::Ptr child = convert(node_statement);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::ForeverStatement>();
                }
                result->set_statement(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::ForeverStatement>(result);
}

AST::Node::Ptr YAMLImporter::convert_delaystatement(const YAML::Node node) const
{
    AST::DelayStatement::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::DelayStatement>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::DelayStatement>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child delay
        if(node["delay"]) {
            const YAML::Node node_delay = node["delay"];
            // Set the child
            AST::Node::Ptr child = convert(node_delay);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::DelayStatement>();
                }
                result->set_delay(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::DelayStatement>(result);
}

AST::Node::Ptr YAMLImporter::convert_instancelist(const YAML::Node node) const
{
    AST::Instancelist::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Instancelist>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Instancelist>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property module
        if(node["module"]) {
            if(node["module"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Instancelist>();
                }
                result->set_module(node["module"].as<std::string>());
            }
        }

        // Manage Child parameterlist
        if(node["parameterlist"]) {
            const YAML::Node node_parameterlist = node["parameterlist"];
            // Fill the list of children
            AST::ParamArg::ListPtr parameterlist_list(new AST::ParamArg::List);
            if(node_parameterlist.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_parameterlist.begin();
                    it != node_parameterlist.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::ParamArg::Ptr child_cast = AST::cast_to<AST::ParamArg>(child);
                        parameterlist_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_parameterlist);
                if(child) {
                    AST::ParamArg::Ptr child_cast = AST::cast_to<AST::ParamArg>(child);
                    parameterlist_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::Instancelist>();
            }
            result->set_parameterlist(parameterlist_list);
        }

        // Manage Child instances
        if(node["instances"]) {
            const YAML::Node node_instances = node["instances"];
            // Fill the list of children
            AST::Instance::ListPtr instances_list(new AST::Instance::List);
            if(node_instances.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_instances.begin(); it != node_instances.end();
                    ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Instance::Ptr child_cast = AST::cast_to<AST::Instance>(child);
                        instances_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_instances);
                if(child) {
                    AST::Instance::Ptr child_cast = AST::cast_to<AST::Instance>(child);
                    instances_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::Instancelist>();
            }
            result->set_instances(instances_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::Instancelist>(result);
}

AST::Node::Ptr YAMLImporter::convert_instance(const YAML::Node node) const
{
    AST::Instance::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Instance>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Instance>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property module
        if(node["module"]) {
            if(node["module"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Instance>();
                }
                result->set_module(node["module"].as<std::string>());
            }
        }
        // Manage property name
        if(node["name"]) {
            if(node["name"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Instance>();
                }
                result->set_name(node["name"].as<std::string>());
            }
        }

        // Manage Child array
        if(node["array"]) {
            const YAML::Node node_array = node["array"];
            // Set the child
            AST::Node::Ptr child = convert(node_array);
            if(child) {
                AST::Dimension::Ptr child_cast = AST::cast_to<AST::Dimension>(child);
                if(!result) {
                    result = std::make_shared<AST::Instance>();
                }
                result->set_array(child_cast);
            }
        }

        // Manage Child parameterlist
        if(node["parameterlist"]) {
            const YAML::Node node_parameterlist = node["parameterlist"];
            // Fill the list of children
            AST::ParamArg::ListPtr parameterlist_list(new AST::ParamArg::List);
            if(node_parameterlist.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_parameterlist.begin();
                    it != node_parameterlist.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::ParamArg::Ptr child_cast = AST::cast_to<AST::ParamArg>(child);
                        parameterlist_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_parameterlist);
                if(child) {
                    AST::ParamArg::Ptr child_cast = AST::cast_to<AST::ParamArg>(child);
                    parameterlist_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::Instance>();
            }
            result->set_parameterlist(parameterlist_list);
        }

        // Manage Child portlist
        if(node["portlist"]) {
            const YAML::Node node_portlist = node["portlist"];
            // Fill the list of children
            AST::PortArg::ListPtr portlist_list(new AST::PortArg::List);
            if(node_portlist.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_portlist.begin(); it != node_portlist.end();
                    ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::PortArg::Ptr child_cast = AST::cast_to<AST::PortArg>(child);
                        portlist_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_portlist);
                if(child) {
                    AST::PortArg::Ptr child_cast = AST::cast_to<AST::PortArg>(child);
                    portlist_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::Instance>();
            }
            result->set_portlist(portlist_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::Instance>(result);
}

AST::Node::Ptr YAMLImporter::convert_paramarg(const YAML::Node node) const
{
    AST::ParamArg::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::ParamArg>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::ParamArg>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property name
        if(node["name"]) {
            if(node["name"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::ParamArg>();
                }
                result->set_name(node["name"].as<std::string>());
            }
        }

        // Manage Child value
        if(node["value"]) {
            const YAML::Node node_value = node["value"];
            // Set the child
            AST::Node::Ptr child = convert(node_value);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::ParamArg>();
                }
                result->set_value(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::ParamArg>(result);
}

AST::Node::Ptr YAMLImporter::convert_portarg(const YAML::Node node) const
{
    AST::PortArg::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::PortArg>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::PortArg>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property name
        if(node["name"]) {
            if(node["name"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::PortArg>();
                }
                result->set_name(node["name"].as<std::string>());
            }
        }

        // Manage Child value
        if(node["value"]) {
            const YAML::Node node_value = node["value"];
            // Set the child
            AST::Node::Ptr child = convert(node_value);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::PortArg>();
                }
                result->set_value(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::PortArg>(result);
}

AST::Node::Ptr YAMLImporter::convert_function(const YAML::Node node) const
{
    AST::Function::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Function>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Function>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property name
        if(node["name"]) {
            if(node["name"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Function>();
                }
                result->set_name(node["name"].as<std::string>());
            }
        }
        // Manage property lifetime
        if(node["lifetime"]) {
            if(node["lifetime"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Function>();
                }
                result->set_lifetime(node["lifetime"].as<AST::Function::LifetimeEnum>());
            }
        }

        // Manage Child return_type
        if(node["return_type"]) {
            const YAML::Node node_return_type = node["return_type"];
            // Set the child
            AST::Node::Ptr child = convert(node_return_type);
            if(child) {
                AST::DataType::Ptr child_cast = AST::cast_to<AST::DataType>(child);
                if(!result) {
                    result = std::make_shared<AST::Function>();
                }
                result->set_return_type(child_cast);
            }
        }

        // Manage Child args
        if(node["args"]) {
            const YAML::Node node_args = node["args"];
            // Fill the list of children
            AST::Arg::ListPtr args_list(new AST::Arg::List);
            if(node_args.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_args.begin(); it != node_args.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Arg::Ptr child_cast = AST::cast_to<AST::Arg>(child);
                        args_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_args);
                if(child) {
                    AST::Arg::Ptr child_cast = AST::cast_to<AST::Arg>(child);
                    args_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::Function>();
            }
            result->set_args(args_list);
        }

        // Manage Child statements
        if(node["statements"]) {
            const YAML::Node node_statements = node["statements"];
            // Fill the list of children
            AST::Node::ListPtr statements_list(new AST::Node::List);
            if(node_statements.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_statements.begin(); it != node_statements.end();
                    ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        statements_list->push_back(child);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_statements);
                if(child) {
                    statements_list->push_back(child);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::Function>();
            }
            result->set_statements(statements_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::Function>(result);
}

AST::Node::Ptr YAMLImporter::convert_functioncall(const YAML::Node node) const
{
    AST::FunctionCall::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::FunctionCall>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::FunctionCall>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property name
        if(node["name"]) {
            if(node["name"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::FunctionCall>();
                }
                result->set_name(node["name"].as<std::string>());
            }
        }
        // Manage property package
        if(node["package"]) {
            if(node["package"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::FunctionCall>();
                }
                result->set_package(node["package"].as<std::string>());
            }
        }

        // Manage Child args
        if(node["args"]) {
            const YAML::Node node_args = node["args"];
            // Fill the list of children
            AST::Node::ListPtr args_list(new AST::Node::List);
            if(node_args.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_args.begin(); it != node_args.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        args_list->push_back(child);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_args);
                if(child) {
                    args_list->push_back(child);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::FunctionCall>();
            }
            result->set_args(args_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::FunctionCall>(result);
}

AST::Node::Ptr YAMLImporter::convert_task(const YAML::Node node) const
{
    AST::Task::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Task>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Task>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property name
        if(node["name"]) {
            if(node["name"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Task>();
                }
                result->set_name(node["name"].as<std::string>());
            }
        }
        // Manage property lifetime
        if(node["lifetime"]) {
            if(node["lifetime"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Task>();
                }
                result->set_lifetime(node["lifetime"].as<AST::Task::LifetimeEnum>());
            }
        }

        // Manage Child args
        if(node["args"]) {
            const YAML::Node node_args = node["args"];
            // Fill the list of children
            AST::Arg::ListPtr args_list(new AST::Arg::List);
            if(node_args.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_args.begin(); it != node_args.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::Arg::Ptr child_cast = AST::cast_to<AST::Arg>(child);
                        args_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_args);
                if(child) {
                    AST::Arg::Ptr child_cast = AST::cast_to<AST::Arg>(child);
                    args_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::Task>();
            }
            result->set_args(args_list);
        }

        // Manage Child statements
        if(node["statements"]) {
            const YAML::Node node_statements = node["statements"];
            // Fill the list of children
            AST::Node::ListPtr statements_list(new AST::Node::List);
            if(node_statements.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_statements.begin(); it != node_statements.end();
                    ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        statements_list->push_back(child);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_statements);
                if(child) {
                    statements_list->push_back(child);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::Task>();
            }
            result->set_statements(statements_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::Task>(result);
}

AST::Node::Ptr YAMLImporter::convert_taskcall(const YAML::Node node) const
{
    AST::TaskCall::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::TaskCall>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::TaskCall>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property name
        if(node["name"]) {
            if(node["name"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::TaskCall>();
                }
                result->set_name(node["name"].as<std::string>());
            }
        }
        // Manage property package
        if(node["package"]) {
            if(node["package"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::TaskCall>();
                }
                result->set_package(node["package"].as<std::string>());
            }
        }

        // Manage Child args
        if(node["args"]) {
            const YAML::Node node_args = node["args"];
            // Fill the list of children
            AST::Node::ListPtr args_list(new AST::Node::List);
            if(node_args.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_args.begin(); it != node_args.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        args_list->push_back(child);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_args);
                if(child) {
                    args_list->push_back(child);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::TaskCall>();
            }
            result->set_args(args_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::TaskCall>(result);
}

AST::Node::Ptr YAMLImporter::convert_generatestatement(const YAML::Node node) const
{
    AST::GenerateStatement::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::GenerateStatement>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::GenerateStatement>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child items
        if(node["items"]) {
            const YAML::Node node_items = node["items"];
            // Fill the list of children
            AST::Node::ListPtr items_list(new AST::Node::List);
            if(node_items.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_items.begin(); it != node_items.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        items_list->push_back(child);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_items);
                if(child) {
                    items_list->push_back(child);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::GenerateStatement>();
            }
            result->set_items(items_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::GenerateStatement>(result);
}

AST::Node::Ptr YAMLImporter::convert_systemcall(const YAML::Node node) const
{
    AST::SystemCall::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::SystemCall>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::SystemCall>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property syscall
        if(node["syscall"]) {
            if(node["syscall"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::SystemCall>();
                }
                result->set_syscall(node["syscall"].as<std::string>());
            }
        }

        // Manage Child args
        if(node["args"]) {
            const YAML::Node node_args = node["args"];
            // Fill the list of children
            AST::Node::ListPtr args_list(new AST::Node::List);
            if(node_args.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_args.begin(); it != node_args.end(); ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        args_list->push_back(child);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_args);
                if(child) {
                    args_list->push_back(child);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::SystemCall>();
            }
            result->set_args(args_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::SystemCall>(result);
}

AST::Node::Ptr YAMLImporter::convert_identifierscopelabel(const YAML::Node node) const
{
    AST::IdentifierScopeLabel::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::IdentifierScopeLabel>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::IdentifierScopeLabel>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property scope
        if(node["scope"]) {
            if(node["scope"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::IdentifierScopeLabel>();
                }
                result->set_scope(node["scope"].as<std::string>());
            }
        }

        // Manage Child loop
        if(node["loop"]) {
            const YAML::Node node_loop = node["loop"];
            // Set the child
            AST::Node::Ptr child = convert(node_loop);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::IdentifierScopeLabel>();
                }
                result->set_loop(child);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::IdentifierScopeLabel>(result);
}

AST::Node::Ptr YAMLImporter::convert_identifierscope(const YAML::Node node) const
{
    AST::IdentifierScope::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::IdentifierScope>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::IdentifierScope>();
                }
                result->set_line(node["line"].as<int>());
            }
        }

        // Manage Child labellist
        if(node["labellist"]) {
            const YAML::Node node_labellist = node["labellist"];
            // Fill the list of children
            AST::IdentifierScopeLabel::ListPtr labellist_list(new AST::IdentifierScopeLabel::List);
            if(node_labellist.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_labellist.begin(); it != node_labellist.end();
                    ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        AST::IdentifierScopeLabel::Ptr child_cast =
                            AST::cast_to<AST::IdentifierScopeLabel>(child);
                        labellist_list->push_back(child_cast);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_labellist);
                if(child) {
                    AST::IdentifierScopeLabel::Ptr child_cast =
                        AST::cast_to<AST::IdentifierScopeLabel>(child);
                    labellist_list->push_back(child_cast);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::IdentifierScope>();
            }
            result->set_labellist(labellist_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::IdentifierScope>(result);
}

AST::Node::Ptr YAMLImporter::convert_disable(const YAML::Node node) const
{
    AST::Disable::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Disable>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::Disable>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property dest
        if(node["dest"]) {
            if(node["dest"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::Disable>();
                }
                result->set_dest(node["dest"].as<std::string>());
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::Disable>(result);
}

AST::Node::Ptr YAMLImporter::convert_parallelblock(const YAML::Node node) const
{
    AST::ParallelBlock::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::ParallelBlock>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::ParallelBlock>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property scope
        if(node["scope"]) {
            if(node["scope"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::ParallelBlock>();
                }
                result->set_scope(node["scope"].as<std::string>());
            }
        }

        // Manage Child statements
        if(node["statements"]) {
            const YAML::Node node_statements = node["statements"];
            // Fill the list of children
            AST::Node::ListPtr statements_list(new AST::Node::List);
            if(node_statements.IsSequence()) {
                // The YAML node is a sequence
                for(YAML::const_iterator it = node_statements.begin(); it != node_statements.end();
                    ++it) {
                    AST::Node::Ptr child = convert(*it);
                    if(child) {
                        statements_list->push_back(child);
                    }
                }
            } else {
                AST::Node::Ptr child = convert(node_statements);
                if(child) {
                    statements_list->push_back(child);
                }
            }
            // Set the list
            if(!result) {
                result = std::make_shared<AST::ParallelBlock>();
            }
            result->set_statements(statements_list);
        }
    }

    // Return the result
    return AST::cast_to<AST::ParallelBlock>(result);
}

AST::Node::Ptr YAMLImporter::convert_singlestatement(const YAML::Node node) const
{
    AST::SingleStatement::Ptr result;
    if(node.IsMap()) {
        if(node["filename"]) {
            if(node["filename"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::SingleStatement>();
                }
                result->set_filename(node["filename"].as<std::string>());
            }
        }
        if(node["line"]) {
            if(node["line"].IsScalar()) {
                if(!result) {
                    result = std::make_shared<AST::SingleStatement>();
                }
                result->set_line(node["line"].as<int>());
            }
        }
        // Manage property scope
        if(node["scope"]) {
            if(node["scope"].IsScalar()) {

                if(!result) {
                    result = std::make_shared<AST::SingleStatement>();
                }
                result->set_scope(node["scope"].as<std::string>());
            }
        }

        // Manage Child statement
        if(node["statement"]) {
            const YAML::Node node_statement = node["statement"];
            // Set the child
            AST::Node::Ptr child = convert(node_statement);
            if(child) {
                if(!result) {
                    result = std::make_shared<AST::SingleStatement>();
                }
                result->set_statement(child);
            }
        }

        // Manage Child delay
        if(node["delay"]) {
            const YAML::Node node_delay = node["delay"];
            // Set the child
            AST::Node::Ptr child = convert(node_delay);
            if(child) {
                AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
                if(!result) {
                    result = std::make_shared<AST::SingleStatement>();
                }
                result->set_delay(child_cast);
            }
        }
    }

    // Return the result
    return AST::cast_to<AST::SingleStatement>(result);
}

} // namespace Importers
} // namespace Veriparse