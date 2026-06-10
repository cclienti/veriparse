// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_GENERATORS_YAML_GENERATOR_HPP
#define VERIPARSE_GENERATORS_YAML_GENERATOR_HPP

#include <yaml-cpp/yaml.h>
#include <veriparse/generators/generator_base.hpp>

namespace Veriparse
{
namespace Generators
{

class YAMLGenerator : public GeneratorBase<YAML::Node>
{
    virtual YAML::Node render_node(const AST::Node::Ptr node) const;
    virtual YAML::Node render_source(const AST::Source::Ptr node) const;
    virtual YAML::Node render_description(const AST::Description::Ptr node) const;
    virtual YAML::Node render_pragmalist(const AST::Pragmalist::Ptr node) const;
    virtual YAML::Node render_pragma(const AST::Pragma::Ptr node) const;
    virtual YAML::Node render_module(const AST::Module::Ptr node) const;
    virtual YAML::Node render_port(const AST::Port::Ptr node) const;
    virtual YAML::Node render_package(const AST::Package::Ptr node) const;
    virtual YAML::Node render_import(const AST::Import::Ptr node) const;
    virtual YAML::Node render_identifier(const AST::Identifier::Ptr node) const;
    virtual YAML::Node render_call(const AST::Call::Ptr node) const;
    virtual YAML::Node render_constant(const AST::Constant::Ptr node) const;
    virtual YAML::Node render_stringconst(const AST::StringConst::Ptr node) const;
    virtual YAML::Node render_intconst(const AST::IntConst::Ptr node) const;
    virtual YAML::Node render_intconstn(const AST::IntConstN::Ptr node) const;
    virtual YAML::Node render_floatconst(const AST::FloatConst::Ptr node) const;
    virtual YAML::Node render_datatype(const AST::DataType::Ptr node) const;
    virtual YAML::Node render_logictype(const AST::LogicType::Ptr node) const;
    virtual YAML::Node render_regtype(const AST::RegType::Ptr node) const;
    virtual YAML::Node render_bittype(const AST::BitType::Ptr node) const;
    virtual YAML::Node render_bytetype(const AST::ByteType::Ptr node) const;
    virtual YAML::Node render_shortinttype(const AST::ShortintType::Ptr node) const;
    virtual YAML::Node render_inttype(const AST::IntType::Ptr node) const;
    virtual YAML::Node render_longinttype(const AST::LongintType::Ptr node) const;
    virtual YAML::Node render_integertype(const AST::IntegerType::Ptr node) const;
    virtual YAML::Node render_timetype(const AST::TimeType::Ptr node) const;
    virtual YAML::Node render_realtype(const AST::RealType::Ptr node) const;
    virtual YAML::Node render_shortrealtype(const AST::ShortrealType::Ptr node) const;
    virtual YAML::Node render_realtimetype(const AST::RealtimeType::Ptr node) const;
    virtual YAML::Node render_stringtype(const AST::StringType::Ptr node) const;
    virtual YAML::Node render_chandletype(const AST::ChandleType::Ptr node) const;
    virtual YAML::Node render_eventtype(const AST::EventType::Ptr node) const;
    virtual YAML::Node render_implicittype(const AST::ImplicitType::Ptr node) const;
    virtual YAML::Node render_voidtype(const AST::VoidType::Ptr node) const;
    virtual YAML::Node render_namedtype(const AST::NamedType::Ptr node) const;
    virtual YAML::Node render_scopename(const AST::ScopeName::Ptr node) const;
    virtual YAML::Node render_structtype(const AST::StructType::Ptr node) const;
    virtual YAML::Node render_uniontype(const AST::UnionType::Ptr node) const;
    virtual YAML::Node render_enumtype(const AST::EnumType::Ptr node) const;
    virtual YAML::Node render_enumitem(const AST::EnumItem::Ptr node) const;
    virtual YAML::Node render_typeopexpr(const AST::TypeOpExpr::Ptr node) const;
    virtual YAML::Node render_typeoptype(const AST::TypeOpType::Ptr node) const;
    virtual YAML::Node render_dimension(const AST::Dimension::Ptr node) const;
    virtual YAML::Node render_rangedim(const AST::RangeDim::Ptr node) const;
    virtual YAML::Node render_sizedim(const AST::SizeDim::Ptr node) const;
    virtual YAML::Node render_unsizeddim(const AST::UnsizedDim::Ptr node) const;
    virtual YAML::Node render_queuedim(const AST::QueueDim::Ptr node) const;
    virtual YAML::Node render_assocdim(const AST::AssocDim::Ptr node) const;
    virtual YAML::Node render_declaration(const AST::Declaration::Ptr node) const;
    virtual YAML::Node render_var(const AST::Var::Ptr node) const;
    virtual YAML::Node render_net(const AST::Net::Ptr node) const;
    virtual YAML::Node render_wirenet(const AST::WireNet::Ptr node) const;
    virtual YAML::Node render_trinet(const AST::TriNet::Ptr node) const;
    virtual YAML::Node render_tri0net(const AST::Tri0Net::Ptr node) const;
    virtual YAML::Node render_tri1net(const AST::Tri1Net::Ptr node) const;
    virtual YAML::Node render_triandnet(const AST::TriandNet::Ptr node) const;
    virtual YAML::Node render_triornet(const AST::TriorNet::Ptr node) const;
    virtual YAML::Node render_triregnet(const AST::TriregNet::Ptr node) const;
    virtual YAML::Node render_wandnet(const AST::WandNet::Ptr node) const;
    virtual YAML::Node render_wornet(const AST::WorNet::Ptr node) const;
    virtual YAML::Node render_uwirenet(const AST::UwireNet::Ptr node) const;
    virtual YAML::Node render_supply0net(const AST::Supply0Net::Ptr node) const;
    virtual YAML::Node render_supply1net(const AST::Supply1Net::Ptr node) const;
    virtual YAML::Node render_interconnectnet(const AST::InterconnectNet::Ptr node) const;
    virtual YAML::Node render_usernet(const AST::UserNet::Ptr node) const;
    virtual YAML::Node render_implicitnet(const AST::ImplicitNet::Ptr node) const;
    virtual YAML::Node render_nettypedecl(const AST::NetTypeDecl::Ptr node) const;
    virtual YAML::Node render_strength(const AST::Strength::Ptr node) const;
    virtual YAML::Node render_drivestrength(const AST::DriveStrength::Ptr node) const;
    virtual YAML::Node render_chargestrength(const AST::ChargeStrength::Ptr node) const;
    virtual YAML::Node render_param(const AST::Param::Ptr node) const;
    virtual YAML::Node render_typeparam(const AST::TypeParam::Ptr node) const;
    virtual YAML::Node render_typedef(const AST::Typedef::Ptr node) const;
    virtual YAML::Node render_member(const AST::Member::Ptr node) const;
    virtual YAML::Node render_arg(const AST::Arg::Ptr node) const;
    virtual YAML::Node render_genvar(const AST::Genvar::Ptr node) const;
    virtual YAML::Node render_concat(const AST::Concat::Ptr node) const;
    virtual YAML::Node render_lconcat(const AST::Lconcat::Ptr node) const;
    virtual YAML::Node render_repeat(const AST::Repeat::Ptr node) const;
    virtual YAML::Node render_assignmentpattern(const AST::AssignmentPattern::Ptr node) const;
    virtual YAML::Node render_patternitem(const AST::PatternItem::Ptr node) const;
    virtual YAML::Node render_cast(const AST::Cast::Ptr node) const;
    virtual YAML::Node render_typecast(const AST::TypeCast::Ptr node) const;
    virtual YAML::Node render_sizecast(const AST::SizeCast::Ptr node) const;
    virtual YAML::Node render_signingcast(const AST::SigningCast::Ptr node) const;
    virtual YAML::Node render_constcast(const AST::ConstCast::Ptr node) const;
    virtual YAML::Node render_indirect(const AST::Indirect::Ptr node) const;
    virtual YAML::Node render_partselect(const AST::Partselect::Ptr node) const;
    virtual YAML::Node render_partselectindexed(const AST::PartselectIndexed::Ptr node) const;
    virtual YAML::Node
    render_partselectplusindexed(const AST::PartselectPlusIndexed::Ptr node) const;
    virtual YAML::Node
    render_partselectminusindexed(const AST::PartselectMinusIndexed::Ptr node) const;
    virtual YAML::Node render_pointer(const AST::Pointer::Ptr node) const;
    virtual YAML::Node render_lvalue(const AST::Lvalue::Ptr node) const;
    virtual YAML::Node render_rvalue(const AST::Rvalue::Ptr node) const;
    virtual YAML::Node render_unaryoperator(const AST::UnaryOperator::Ptr node) const;
    virtual YAML::Node render_uplus(const AST::Uplus::Ptr node) const;
    virtual YAML::Node render_uminus(const AST::Uminus::Ptr node) const;
    virtual YAML::Node render_ulnot(const AST::Ulnot::Ptr node) const;
    virtual YAML::Node render_unot(const AST::Unot::Ptr node) const;
    virtual YAML::Node render_uand(const AST::Uand::Ptr node) const;
    virtual YAML::Node render_unand(const AST::Unand::Ptr node) const;
    virtual YAML::Node render_uor(const AST::Uor::Ptr node) const;
    virtual YAML::Node render_unor(const AST::Unor::Ptr node) const;
    virtual YAML::Node render_uxor(const AST::Uxor::Ptr node) const;
    virtual YAML::Node render_uxnor(const AST::Uxnor::Ptr node) const;
    virtual YAML::Node render_operator(const AST::Operator::Ptr node) const;
    virtual YAML::Node render_power(const AST::Power::Ptr node) const;
    virtual YAML::Node render_times(const AST::Times::Ptr node) const;
    virtual YAML::Node render_divide(const AST::Divide::Ptr node) const;
    virtual YAML::Node render_mod(const AST::Mod::Ptr node) const;
    virtual YAML::Node render_plus(const AST::Plus::Ptr node) const;
    virtual YAML::Node render_minus(const AST::Minus::Ptr node) const;
    virtual YAML::Node render_sll(const AST::Sll::Ptr node) const;
    virtual YAML::Node render_srl(const AST::Srl::Ptr node) const;
    virtual YAML::Node render_sra(const AST::Sra::Ptr node) const;
    virtual YAML::Node render_lessthan(const AST::LessThan::Ptr node) const;
    virtual YAML::Node render_greaterthan(const AST::GreaterThan::Ptr node) const;
    virtual YAML::Node render_lesseq(const AST::LessEq::Ptr node) const;
    virtual YAML::Node render_greatereq(const AST::GreaterEq::Ptr node) const;
    virtual YAML::Node render_eq(const AST::Eq::Ptr node) const;
    virtual YAML::Node render_noteq(const AST::NotEq::Ptr node) const;
    virtual YAML::Node render_eql(const AST::Eql::Ptr node) const;
    virtual YAML::Node render_noteql(const AST::NotEql::Ptr node) const;
    virtual YAML::Node render_and(const AST::And::Ptr node) const;
    virtual YAML::Node render_xor(const AST::Xor::Ptr node) const;
    virtual YAML::Node render_xnor(const AST::Xnor::Ptr node) const;
    virtual YAML::Node render_or(const AST::Or::Ptr node) const;
    virtual YAML::Node render_land(const AST::Land::Ptr node) const;
    virtual YAML::Node render_lor(const AST::Lor::Ptr node) const;
    virtual YAML::Node render_cond(const AST::Cond::Ptr node) const;
    virtual YAML::Node render_always(const AST::Always::Ptr node) const;
    virtual YAML::Node render_alwaysff(const AST::AlwaysFF::Ptr node) const;
    virtual YAML::Node render_alwayscomb(const AST::AlwaysComb::Ptr node) const;
    virtual YAML::Node render_alwayslatch(const AST::AlwaysLatch::Ptr node) const;
    virtual YAML::Node render_senslist(const AST::Senslist::Ptr node) const;
    virtual YAML::Node render_sens(const AST::Sens::Ptr node) const;
    virtual YAML::Node render_defparamlist(const AST::Defparamlist::Ptr node) const;
    virtual YAML::Node render_defparam(const AST::Defparam::Ptr node) const;
    virtual YAML::Node render_assign(const AST::Assign::Ptr node) const;
    virtual YAML::Node render_blockingsubstitution(const AST::BlockingSubstitution::Ptr node) const;
    virtual YAML::Node
    render_nonblockingsubstitution(const AST::NonblockingSubstitution::Ptr node) const;
    virtual YAML::Node render_ifstatement(const AST::IfStatement::Ptr node) const;
    virtual YAML::Node render_repeatstatement(const AST::RepeatStatement::Ptr node) const;
    virtual YAML::Node render_forstatement(const AST::ForStatement::Ptr node) const;
    virtual YAML::Node render_whilestatement(const AST::WhileStatement::Ptr node) const;
    virtual YAML::Node render_casestatement(const AST::CaseStatement::Ptr node) const;
    virtual YAML::Node render_casexstatement(const AST::CasexStatement::Ptr node) const;
    virtual YAML::Node render_casezstatement(const AST::CasezStatement::Ptr node) const;
    virtual YAML::Node render_uniquecasestatement(const AST::UniqueCaseStatement::Ptr node) const;
    virtual YAML::Node
    render_prioritycasestatement(const AST::PriorityCaseStatement::Ptr node) const;
    virtual YAML::Node render_case(const AST::Case::Ptr node) const;
    virtual YAML::Node render_block(const AST::Block::Ptr node) const;
    virtual YAML::Node render_initial(const AST::Initial::Ptr node) const;
    virtual YAML::Node render_eventstatement(const AST::EventStatement::Ptr node) const;
    virtual YAML::Node render_waitstatement(const AST::WaitStatement::Ptr node) const;
    virtual YAML::Node render_foreverstatement(const AST::ForeverStatement::Ptr node) const;
    virtual YAML::Node render_delaystatement(const AST::DelayStatement::Ptr node) const;
    virtual YAML::Node render_instancelist(const AST::Instancelist::Ptr node) const;
    virtual YAML::Node render_instance(const AST::Instance::Ptr node) const;
    virtual YAML::Node render_paramarg(const AST::ParamArg::Ptr node) const;
    virtual YAML::Node render_portarg(const AST::PortArg::Ptr node) const;
    virtual YAML::Node render_function(const AST::Function::Ptr node) const;
    virtual YAML::Node render_functioncall(const AST::FunctionCall::Ptr node) const;
    virtual YAML::Node render_task(const AST::Task::Ptr node) const;
    virtual YAML::Node render_taskcall(const AST::TaskCall::Ptr node) const;
    virtual YAML::Node render_generatestatement(const AST::GenerateStatement::Ptr node) const;
    virtual YAML::Node render_systemcall(const AST::SystemCall::Ptr node) const;
    virtual YAML::Node render_hierlabel(const AST::HierLabel::Ptr node) const;
    virtual YAML::Node render_hiername(const AST::HierName::Ptr node) const;
    virtual YAML::Node render_disable(const AST::Disable::Ptr node) const;
    virtual YAML::Node render_parallelblock(const AST::ParallelBlock::Ptr node) const;
    virtual YAML::Node render_singlestatement(const AST::SingleStatement::Ptr node) const;
};

} // namespace Generators
} // namespace Veriparse

#endif