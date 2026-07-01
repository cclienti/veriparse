// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_GENERATORS_VERILOG_GENERATOR
#define VERIPARSE_GENERATORS_VERILOG_GENERATOR

#include <string>
#include <veriparse/generators/generator_base.hpp>

namespace Veriparse
{
namespace Generators
{

class VerilogGenerator : public GeneratorBase<std::string>
{
    virtual std::string render_node(const AST::Node::Ptr node) const;
    virtual std::string render_source(const AST::Source::Ptr node) const;
    virtual std::string render_description(const AST::Description::Ptr node) const;
    virtual std::string render_pragmalist(const AST::Pragmalist::Ptr node) const;
    virtual std::string render_pragma(const AST::Pragma::Ptr node) const;
    virtual std::string render_module(const AST::Module::Ptr node) const;
    virtual std::string render_package(const AST::Package::Ptr node) const;
    virtual std::string render_import(const AST::Import::Ptr node) const;
    virtual std::string render_export(const AST::Export::Ptr node) const;
    virtual std::string render_port(const AST::Port::Ptr node) const;

    // Value name references and the `::` / `.` axes.
    virtual std::string render_identifier(const AST::Identifier::Ptr node) const;
    virtual std::string render_scopename(const AST::ScopeName::Ptr node) const;
    virtual std::string render_hiername(const AST::HierName::Ptr node) const;
    virtual std::string render_hierlabel(const AST::HierLabel::Ptr node) const;

    virtual std::string render_intconst(const AST::IntConst::Ptr node) const;
    virtual std::string render_intconstn(const AST::IntConstN::Ptr node) const;
    virtual std::string render_floatconst(const AST::FloatConst::Ptr node) const;
    virtual std::string render_stringconst(const AST::StringConst::Ptr node) const;

    // DataType hierarchy — every variant renders the bare type (keyword + signing
    // + packed dims) via data_type_to_string().
    virtual std::string render_logictype(const AST::LogicType::Ptr node) const;
    virtual std::string render_regtype(const AST::RegType::Ptr node) const;
    virtual std::string render_bittype(const AST::BitType::Ptr node) const;
    virtual std::string render_bytetype(const AST::ByteType::Ptr node) const;
    virtual std::string render_shortinttype(const AST::ShortintType::Ptr node) const;
    virtual std::string render_inttype(const AST::IntType::Ptr node) const;
    virtual std::string render_longinttype(const AST::LongintType::Ptr node) const;
    virtual std::string render_integertype(const AST::IntegerType::Ptr node) const;
    virtual std::string render_timetype(const AST::TimeType::Ptr node) const;
    virtual std::string render_realtype(const AST::RealType::Ptr node) const;
    virtual std::string render_shortrealtype(const AST::ShortrealType::Ptr node) const;
    virtual std::string render_realtimetype(const AST::RealtimeType::Ptr node) const;
    virtual std::string render_stringtype(const AST::StringType::Ptr node) const;
    virtual std::string render_chandletype(const AST::ChandleType::Ptr node) const;
    virtual std::string render_eventtype(const AST::EventType::Ptr node) const;
    virtual std::string render_voidtype(const AST::VoidType::Ptr node) const;
    virtual std::string render_implicittype(const AST::ImplicitType::Ptr node) const;
    virtual std::string render_namedtype(const AST::NamedType::Ptr node) const;
    virtual std::string render_structtype(const AST::StructType::Ptr node) const;
    virtual std::string render_uniontype(const AST::UnionType::Ptr node) const;
    virtual std::string render_enumtype(const AST::EnumType::Ptr node) const;
    virtual std::string render_typeopexpr(const AST::TypeOpExpr::Ptr node) const;
    virtual std::string render_typeoptype(const AST::TypeOpType::Ptr node) const;

    // Dimension hierarchy (packed and unpacked).
    virtual std::string render_rangedim(const AST::RangeDim::Ptr node) const;
    virtual std::string render_sizedim(const AST::SizeDim::Ptr node) const;
    virtual std::string render_unsizeddim(const AST::UnsizedDim::Ptr node) const;
    virtual std::string render_queuedim(const AST::QueueDim::Ptr node) const;
    virtual std::string render_assocdim(const AST::AssocDim::Ptr node) const;

    virtual std::string render_enumitem(const AST::EnumItem::Ptr node) const;
    virtual std::string render_member(const AST::Member::Ptr node) const;

    // Declaration hierarchy.
    virtual std::string render_var(const AST::Var::Ptr node) const;
    virtual std::string render_wirenet(const AST::WireNet::Ptr node) const;
    virtual std::string render_trinet(const AST::TriNet::Ptr node) const;
    virtual std::string render_tri0net(const AST::Tri0Net::Ptr node) const;
    virtual std::string render_tri1net(const AST::Tri1Net::Ptr node) const;
    virtual std::string render_triandnet(const AST::TriandNet::Ptr node) const;
    virtual std::string render_triornet(const AST::TriorNet::Ptr node) const;
    virtual std::string render_triregnet(const AST::TriregNet::Ptr node) const;
    virtual std::string render_wandnet(const AST::WandNet::Ptr node) const;
    virtual std::string render_wornet(const AST::WorNet::Ptr node) const;
    virtual std::string render_uwirenet(const AST::UwireNet::Ptr node) const;
    virtual std::string render_supply0net(const AST::Supply0Net::Ptr node) const;
    virtual std::string render_supply1net(const AST::Supply1Net::Ptr node) const;
    virtual std::string render_interconnectnet(const AST::InterconnectNet::Ptr node) const;
    virtual std::string render_usernet(const AST::UserNet::Ptr node) const;
    virtual std::string render_implicitnet(const AST::ImplicitNet::Ptr node) const;
    virtual std::string render_drivestrength(const AST::DriveStrength::Ptr node) const;
    virtual std::string render_chargestrength(const AST::ChargeStrength::Ptr node) const;
    virtual std::string render_param(const AST::Param::Ptr node) const;
    virtual std::string render_typeparam(const AST::TypeParam::Ptr node) const;
    virtual std::string render_typedef(const AST::Typedef::Ptr node) const;
    virtual std::string render_arg(const AST::Arg::Ptr node) const;
    virtual std::string render_genvar(const AST::Genvar::Ptr node) const;
    virtual std::string render_nettypedecl(const AST::NetTypeDecl::Ptr node) const;

    virtual std::string render_concat(const AST::Concat::Ptr node) const;
    virtual std::string render_lconcat(const AST::Lconcat::Ptr node) const;
    virtual std::string render_repeat(const AST::Repeat::Ptr node) const;
    virtual std::string render_assignmentpattern(const AST::AssignmentPattern::Ptr node) const;
    virtual std::string render_patternitem(const AST::PatternItem::Ptr node) const;

    // Cast hierarchy (node per form).
    virtual std::string render_typecast(const AST::TypeCast::Ptr node) const;
    virtual std::string render_sizecast(const AST::SizeCast::Ptr node) const;
    virtual std::string render_signingcast(const AST::SigningCast::Ptr node) const;
    virtual std::string render_constcast(const AST::ConstCast::Ptr node) const;

    virtual std::string render_partselect(const AST::Partselect::Ptr node) const;
    virtual std::string
    render_partselectplusindexed(const AST::PartselectPlusIndexed::Ptr node) const;
    virtual std::string
    render_partselectminusindexed(const AST::PartselectMinusIndexed::Ptr node) const;
    virtual std::string render_pointer(const AST::Pointer::Ptr node) const;
    virtual std::string render_lvalue(const AST::Lvalue::Ptr node) const;
    virtual std::string render_rvalue(const AST::Rvalue::Ptr node) const;
    virtual std::string render_uplus(const AST::Uplus::Ptr node) const;
    virtual std::string render_uminus(const AST::Uminus::Ptr node) const;
    virtual std::string render_ulnot(const AST::Ulnot::Ptr node) const;
    virtual std::string render_unot(const AST::Unot::Ptr node) const;
    virtual std::string render_uand(const AST::Uand::Ptr node) const;
    virtual std::string render_unand(const AST::Unand::Ptr node) const;
    virtual std::string render_uor(const AST::Uor::Ptr node) const;
    virtual std::string render_unor(const AST::Unor::Ptr node) const;
    virtual std::string render_uxor(const AST::Uxor::Ptr node) const;
    virtual std::string render_uxnor(const AST::Uxnor::Ptr node) const;
    virtual std::string render_power(const AST::Power::Ptr node) const;
    virtual std::string render_times(const AST::Times::Ptr node) const;
    virtual std::string render_divide(const AST::Divide::Ptr node) const;
    virtual std::string render_mod(const AST::Mod::Ptr node) const;
    virtual std::string render_plus(const AST::Plus::Ptr node) const;
    virtual std::string render_minus(const AST::Minus::Ptr node) const;
    virtual std::string render_sll(const AST::Sll::Ptr node) const;
    virtual std::string render_srl(const AST::Srl::Ptr node) const;
    virtual std::string render_sra(const AST::Sra::Ptr node) const;
    virtual std::string render_lessthan(const AST::LessThan::Ptr node) const;
    virtual std::string render_greaterthan(const AST::GreaterThan::Ptr node) const;
    virtual std::string render_lesseq(const AST::LessEq::Ptr node) const;
    virtual std::string render_greatereq(const AST::GreaterEq::Ptr node) const;
    virtual std::string render_eq(const AST::Eq::Ptr node) const;
    virtual std::string render_noteq(const AST::NotEq::Ptr node) const;
    virtual std::string render_eql(const AST::Eql::Ptr node) const;
    virtual std::string render_noteql(const AST::NotEql::Ptr node) const;
    virtual std::string render_and(const AST::And::Ptr node) const;
    virtual std::string render_xor(const AST::Xor::Ptr node) const;
    virtual std::string render_xnor(const AST::Xnor::Ptr node) const;
    virtual std::string render_or(const AST::Or::Ptr node) const;
    virtual std::string render_land(const AST::Land::Ptr node) const;
    virtual std::string render_lor(const AST::Lor::Ptr node) const;
    virtual std::string render_cond(const AST::Cond::Ptr node) const;
    virtual std::string render_alwaysff(const AST::AlwaysFF::Ptr node) const;
    virtual std::string render_alwayscomb(const AST::AlwaysComb::Ptr node) const;
    virtual std::string render_alwayslatch(const AST::AlwaysLatch::Ptr node) const;
    virtual std::string render_always(const AST::Always::Ptr node) const;
    virtual std::string render_senslist(const AST::Senslist::Ptr node) const;
    virtual std::string render_sens(const AST::Sens::Ptr node) const;
    virtual std::string render_defparamlist(const AST::Defparamlist::Ptr node) const;
    virtual std::string render_defparam(const AST::Defparam::Ptr node) const;
    virtual std::string render_assign(const AST::Assign::Ptr node) const;
    virtual std::string
    render_blockingsubstitution(const AST::BlockingSubstitution::Ptr node) const;
    virtual std::string
    render_nonblockingsubstitution(const AST::NonblockingSubstitution::Ptr node) const;
    virtual std::string render_ifstatement(const AST::IfStatement::Ptr node) const;
    virtual std::string render_repeatstatement(const AST::RepeatStatement::Ptr node) const;
    virtual std::string render_forstatement(const AST::ForStatement::Ptr node) const;
    virtual std::string render_whilestatement(const AST::WhileStatement::Ptr node) const;
    virtual std::string render_casestatement(const AST::CaseStatement::Ptr node) const;
    virtual std::string render_casexstatement(const AST::CasexStatement::Ptr node) const;
    virtual std::string render_casezstatement(const AST::CasezStatement::Ptr node) const;
    virtual std::string render_uniquecasestatement(const AST::UniqueCaseStatement::Ptr node) const;
    virtual std::string
    render_prioritycasestatement(const AST::PriorityCaseStatement::Ptr node) const;
    virtual std::string render_case(const AST::Case::Ptr node) const;
    virtual std::string render_block(const AST::Block::Ptr node) const;
    virtual std::string render_initial(const AST::Initial::Ptr node) const;
    virtual std::string render_eventstatement(const AST::EventStatement::Ptr node) const;
    virtual std::string render_waitstatement(const AST::WaitStatement::Ptr node) const;
    virtual std::string render_foreverstatement(const AST::ForeverStatement::Ptr node) const;
    virtual std::string render_delaystatement(const AST::DelayStatement::Ptr node) const;
    virtual std::string render_instancelist(const AST::Instancelist::Ptr node) const;
    virtual std::string render_instance(const AST::Instance::Ptr node) const;
    virtual std::string render_paramarg(const AST::ParamArg::Ptr node) const;
    virtual std::string render_portarg(const AST::PortArg::Ptr node) const;
    virtual std::string render_function(const AST::Function::Ptr node) const;
    virtual std::string render_call(const AST::Call::Ptr node) const;
    virtual std::string render_functioncall(const AST::FunctionCall::Ptr node) const;
    virtual std::string render_task(const AST::Task::Ptr node) const;
    virtual std::string render_taskcall(const AST::TaskCall::Ptr node) const;
    virtual std::string render_generatestatement(const AST::GenerateStatement::Ptr node) const;
    virtual std::string render_systemcall(const AST::SystemCall::Ptr node) const;
    virtual std::string render_disable(const AST::Disable::Ptr node) const;
    virtual std::string render_return(const AST::Return::Ptr node) const;
    virtual std::string render_break(const AST::Break::Ptr node) const;
    virtual std::string render_continue(const AST::Continue::Ptr node) const;
    virtual std::string render_parallelblock(const AST::ParallelBlock::Ptr node) const;
    virtual std::string render_singlestatement(const AST::SingleStatement::Ptr node) const;

    virtual std::string indent(const std::string str) const;

    virtual std::string unary_operators_to_string(AST::NodeType node_type,
                                                  const AST::Node::Ptr right) const;

    virtual std::string operators_to_string(AST::NodeType node_type, const AST::Node::Ptr left,
                                            const AST::Node::Ptr right) const;

    // --- shared helpers (new model) ---

    // The bare type: keyword + signing + packed dims (e.g. "logic signed [3:0]",
    // "int unsigned", "my_t [3:0]"). Empty for a null/keyword-less ImplicitType
    // with no dims. Never a trailing name or ';'.
    virtual std::string data_type_to_string(const AST::DataType::Ptr type) const;

    // Concatenated dimension list, no separators: "[3:0][7:0]" / "[4]" / "[]".
    virtual std::string dims_to_string(const AST::Dimension::ListPtr dims) const;

    // A value name reference: `::` scope + `.` hierarchy + name.
    virtual std::string identifier_to_string(const AST::Identifier::Ptr node) const;

    // `[const] [var] [lifetime] ` qualifier prefix of a Var.
    virtual std::string var_qualifier_prefix(const AST::Var::Ptr node) const;

    // Shared net renderer: keyword + strength + type + name + unpacked dims +
    // continuous assignment.
    virtual std::string net_to_string(const char *keyword, const AST::Net::Ptr node) const;

    // `name [unpacked dims] [= init]` declaration tail shared by Var/Member.
    virtual std::string decl_tail_to_string(const std::string &name,
                                            const AST::Dimension::ListPtr unpacked_dims,
                                            const AST::Node::Ptr init) const;

    virtual std::string block_or_single_statement_to_string(AST::Node::Ptr stmt) const;

    virtual std::string ports_list_to_string(const AST::Port::ListPtr ports, int length = 0) const;

    virtual std::string parameters_list_to_string(const AST::Declaration::ListPtr parameters,
                                                  int length = 0) const;
};

} // namespace Generators
} // namespace Veriparse

#endif
