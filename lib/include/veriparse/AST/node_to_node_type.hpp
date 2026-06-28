// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_AST_NODE_TO_NODE_TYPE_HPP
#define VERIPARSE_AST_NODE_TO_NODE_TYPE_HPP

#include <veriparse/AST/nodes.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

template <typename NodeT> constexpr NodeType get_node_type() { return NodeType::None; }

template <> constexpr NodeType get_node_type<Node>() { return NodeType::Node; }
template <> constexpr NodeType get_node_type<Source>() { return NodeType::Source; }
template <> constexpr NodeType get_node_type<Description>() { return NodeType::Description; }
template <> constexpr NodeType get_node_type<Pragmalist>() { return NodeType::Pragmalist; }
template <> constexpr NodeType get_node_type<Pragma>() { return NodeType::Pragma; }
template <> constexpr NodeType get_node_type<Module>() { return NodeType::Module; }
template <> constexpr NodeType get_node_type<Port>() { return NodeType::Port; }
template <> constexpr NodeType get_node_type<Package>() { return NodeType::Package; }
template <> constexpr NodeType get_node_type<Import>() { return NodeType::Import; }
template <> constexpr NodeType get_node_type<Export>() { return NodeType::Export; }
template <> constexpr NodeType get_node_type<Identifier>() { return NodeType::Identifier; }
template <> constexpr NodeType get_node_type<Call>() { return NodeType::Call; }
template <> constexpr NodeType get_node_type<Constant>() { return NodeType::Constant; }
template <> constexpr NodeType get_node_type<StringConst>() { return NodeType::StringConst; }
template <> constexpr NodeType get_node_type<IntConst>() { return NodeType::IntConst; }
template <> constexpr NodeType get_node_type<IntConstN>() { return NodeType::IntConstN; }
template <> constexpr NodeType get_node_type<FloatConst>() { return NodeType::FloatConst; }
template <> constexpr NodeType get_node_type<DataType>() { return NodeType::DataType; }
template <> constexpr NodeType get_node_type<LogicType>() { return NodeType::LogicType; }
template <> constexpr NodeType get_node_type<RegType>() { return NodeType::RegType; }
template <> constexpr NodeType get_node_type<BitType>() { return NodeType::BitType; }
template <> constexpr NodeType get_node_type<ByteType>() { return NodeType::ByteType; }
template <> constexpr NodeType get_node_type<ShortintType>() { return NodeType::ShortintType; }
template <> constexpr NodeType get_node_type<IntType>() { return NodeType::IntType; }
template <> constexpr NodeType get_node_type<LongintType>() { return NodeType::LongintType; }
template <> constexpr NodeType get_node_type<IntegerType>() { return NodeType::IntegerType; }
template <> constexpr NodeType get_node_type<TimeType>() { return NodeType::TimeType; }
template <> constexpr NodeType get_node_type<RealType>() { return NodeType::RealType; }
template <> constexpr NodeType get_node_type<ShortrealType>() { return NodeType::ShortrealType; }
template <> constexpr NodeType get_node_type<RealtimeType>() { return NodeType::RealtimeType; }
template <> constexpr NodeType get_node_type<StringType>() { return NodeType::StringType; }
template <> constexpr NodeType get_node_type<ChandleType>() { return NodeType::ChandleType; }
template <> constexpr NodeType get_node_type<EventType>() { return NodeType::EventType; }
template <> constexpr NodeType get_node_type<ImplicitType>() { return NodeType::ImplicitType; }
template <> constexpr NodeType get_node_type<VoidType>() { return NodeType::VoidType; }
template <> constexpr NodeType get_node_type<NamedType>() { return NodeType::NamedType; }
template <> constexpr NodeType get_node_type<ScopeName>() { return NodeType::ScopeName; }
template <> constexpr NodeType get_node_type<StructType>() { return NodeType::StructType; }
template <> constexpr NodeType get_node_type<UnionType>() { return NodeType::UnionType; }
template <> constexpr NodeType get_node_type<EnumType>() { return NodeType::EnumType; }
template <> constexpr NodeType get_node_type<EnumItem>() { return NodeType::EnumItem; }
template <> constexpr NodeType get_node_type<TypeOpExpr>() { return NodeType::TypeOpExpr; }
template <> constexpr NodeType get_node_type<TypeOpType>() { return NodeType::TypeOpType; }
template <> constexpr NodeType get_node_type<Dimension>() { return NodeType::Dimension; }
template <> constexpr NodeType get_node_type<RangeDim>() { return NodeType::RangeDim; }
template <> constexpr NodeType get_node_type<SizeDim>() { return NodeType::SizeDim; }
template <> constexpr NodeType get_node_type<UnsizedDim>() { return NodeType::UnsizedDim; }
template <> constexpr NodeType get_node_type<QueueDim>() { return NodeType::QueueDim; }
template <> constexpr NodeType get_node_type<AssocDim>() { return NodeType::AssocDim; }
template <> constexpr NodeType get_node_type<Declaration>() { return NodeType::Declaration; }
template <> constexpr NodeType get_node_type<Var>() { return NodeType::Var; }
template <> constexpr NodeType get_node_type<Net>() { return NodeType::Net; }
template <> constexpr NodeType get_node_type<WireNet>() { return NodeType::WireNet; }
template <> constexpr NodeType get_node_type<TriNet>() { return NodeType::TriNet; }
template <> constexpr NodeType get_node_type<Tri0Net>() { return NodeType::Tri0Net; }
template <> constexpr NodeType get_node_type<Tri1Net>() { return NodeType::Tri1Net; }
template <> constexpr NodeType get_node_type<TriandNet>() { return NodeType::TriandNet; }
template <> constexpr NodeType get_node_type<TriorNet>() { return NodeType::TriorNet; }
template <> constexpr NodeType get_node_type<TriregNet>() { return NodeType::TriregNet; }
template <> constexpr NodeType get_node_type<WandNet>() { return NodeType::WandNet; }
template <> constexpr NodeType get_node_type<WorNet>() { return NodeType::WorNet; }
template <> constexpr NodeType get_node_type<UwireNet>() { return NodeType::UwireNet; }
template <> constexpr NodeType get_node_type<Supply0Net>() { return NodeType::Supply0Net; }
template <> constexpr NodeType get_node_type<Supply1Net>() { return NodeType::Supply1Net; }
template <> constexpr NodeType get_node_type<InterconnectNet>()
{
    return NodeType::InterconnectNet;
}
template <> constexpr NodeType get_node_type<UserNet>() { return NodeType::UserNet; }
template <> constexpr NodeType get_node_type<ImplicitNet>() { return NodeType::ImplicitNet; }
template <> constexpr NodeType get_node_type<NetTypeDecl>() { return NodeType::NetTypeDecl; }
template <> constexpr NodeType get_node_type<Strength>() { return NodeType::Strength; }
template <> constexpr NodeType get_node_type<DriveStrength>() { return NodeType::DriveStrength; }
template <> constexpr NodeType get_node_type<ChargeStrength>() { return NodeType::ChargeStrength; }
template <> constexpr NodeType get_node_type<Param>() { return NodeType::Param; }
template <> constexpr NodeType get_node_type<TypeParam>() { return NodeType::TypeParam; }
template <> constexpr NodeType get_node_type<Typedef>() { return NodeType::Typedef; }
template <> constexpr NodeType get_node_type<Member>() { return NodeType::Member; }
template <> constexpr NodeType get_node_type<Arg>() { return NodeType::Arg; }
template <> constexpr NodeType get_node_type<Genvar>() { return NodeType::Genvar; }
template <> constexpr NodeType get_node_type<Concat>() { return NodeType::Concat; }
template <> constexpr NodeType get_node_type<Lconcat>() { return NodeType::Lconcat; }
template <> constexpr NodeType get_node_type<Repeat>() { return NodeType::Repeat; }
template <> constexpr NodeType get_node_type<AssignmentPattern>()
{
    return NodeType::AssignmentPattern;
}
template <> constexpr NodeType get_node_type<PatternItem>() { return NodeType::PatternItem; }
template <> constexpr NodeType get_node_type<Cast>() { return NodeType::Cast; }
template <> constexpr NodeType get_node_type<TypeCast>() { return NodeType::TypeCast; }
template <> constexpr NodeType get_node_type<SizeCast>() { return NodeType::SizeCast; }
template <> constexpr NodeType get_node_type<SigningCast>() { return NodeType::SigningCast; }
template <> constexpr NodeType get_node_type<ConstCast>() { return NodeType::ConstCast; }
template <> constexpr NodeType get_node_type<Indirect>() { return NodeType::Indirect; }
template <> constexpr NodeType get_node_type<Partselect>() { return NodeType::Partselect; }
template <> constexpr NodeType get_node_type<PartselectIndexed>()
{
    return NodeType::PartselectIndexed;
}
template <> constexpr NodeType get_node_type<PartselectPlusIndexed>()
{
    return NodeType::PartselectPlusIndexed;
}
template <> constexpr NodeType get_node_type<PartselectMinusIndexed>()
{
    return NodeType::PartselectMinusIndexed;
}
template <> constexpr NodeType get_node_type<Pointer>() { return NodeType::Pointer; }
template <> constexpr NodeType get_node_type<Lvalue>() { return NodeType::Lvalue; }
template <> constexpr NodeType get_node_type<Rvalue>() { return NodeType::Rvalue; }
template <> constexpr NodeType get_node_type<UnaryOperator>() { return NodeType::UnaryOperator; }
template <> constexpr NodeType get_node_type<Uplus>() { return NodeType::Uplus; }
template <> constexpr NodeType get_node_type<Uminus>() { return NodeType::Uminus; }
template <> constexpr NodeType get_node_type<Ulnot>() { return NodeType::Ulnot; }
template <> constexpr NodeType get_node_type<Unot>() { return NodeType::Unot; }
template <> constexpr NodeType get_node_type<Uand>() { return NodeType::Uand; }
template <> constexpr NodeType get_node_type<Unand>() { return NodeType::Unand; }
template <> constexpr NodeType get_node_type<Uor>() { return NodeType::Uor; }
template <> constexpr NodeType get_node_type<Unor>() { return NodeType::Unor; }
template <> constexpr NodeType get_node_type<Uxor>() { return NodeType::Uxor; }
template <> constexpr NodeType get_node_type<Uxnor>() { return NodeType::Uxnor; }
template <> constexpr NodeType get_node_type<Operator>() { return NodeType::Operator; }
template <> constexpr NodeType get_node_type<Power>() { return NodeType::Power; }
template <> constexpr NodeType get_node_type<Times>() { return NodeType::Times; }
template <> constexpr NodeType get_node_type<Divide>() { return NodeType::Divide; }
template <> constexpr NodeType get_node_type<Mod>() { return NodeType::Mod; }
template <> constexpr NodeType get_node_type<Plus>() { return NodeType::Plus; }
template <> constexpr NodeType get_node_type<Minus>() { return NodeType::Minus; }
template <> constexpr NodeType get_node_type<Sll>() { return NodeType::Sll; }
template <> constexpr NodeType get_node_type<Srl>() { return NodeType::Srl; }
template <> constexpr NodeType get_node_type<Sra>() { return NodeType::Sra; }
template <> constexpr NodeType get_node_type<LessThan>() { return NodeType::LessThan; }
template <> constexpr NodeType get_node_type<GreaterThan>() { return NodeType::GreaterThan; }
template <> constexpr NodeType get_node_type<LessEq>() { return NodeType::LessEq; }
template <> constexpr NodeType get_node_type<GreaterEq>() { return NodeType::GreaterEq; }
template <> constexpr NodeType get_node_type<Eq>() { return NodeType::Eq; }
template <> constexpr NodeType get_node_type<NotEq>() { return NodeType::NotEq; }
template <> constexpr NodeType get_node_type<Eql>() { return NodeType::Eql; }
template <> constexpr NodeType get_node_type<NotEql>() { return NodeType::NotEql; }
template <> constexpr NodeType get_node_type<And>() { return NodeType::And; }
template <> constexpr NodeType get_node_type<Xor>() { return NodeType::Xor; }
template <> constexpr NodeType get_node_type<Xnor>() { return NodeType::Xnor; }
template <> constexpr NodeType get_node_type<Or>() { return NodeType::Or; }
template <> constexpr NodeType get_node_type<Land>() { return NodeType::Land; }
template <> constexpr NodeType get_node_type<Lor>() { return NodeType::Lor; }
template <> constexpr NodeType get_node_type<Cond>() { return NodeType::Cond; }
template <> constexpr NodeType get_node_type<Always>() { return NodeType::Always; }
template <> constexpr NodeType get_node_type<AlwaysFF>() { return NodeType::AlwaysFF; }
template <> constexpr NodeType get_node_type<AlwaysComb>() { return NodeType::AlwaysComb; }
template <> constexpr NodeType get_node_type<AlwaysLatch>() { return NodeType::AlwaysLatch; }
template <> constexpr NodeType get_node_type<Senslist>() { return NodeType::Senslist; }
template <> constexpr NodeType get_node_type<Sens>() { return NodeType::Sens; }
template <> constexpr NodeType get_node_type<Defparamlist>() { return NodeType::Defparamlist; }
template <> constexpr NodeType get_node_type<Defparam>() { return NodeType::Defparam; }
template <> constexpr NodeType get_node_type<Assign>() { return NodeType::Assign; }
template <> constexpr NodeType get_node_type<BlockingSubstitution>()
{
    return NodeType::BlockingSubstitution;
}
template <> constexpr NodeType get_node_type<NonblockingSubstitution>()
{
    return NodeType::NonblockingSubstitution;
}
template <> constexpr NodeType get_node_type<IfStatement>() { return NodeType::IfStatement; }
template <> constexpr NodeType get_node_type<RepeatStatement>()
{
    return NodeType::RepeatStatement;
}
template <> constexpr NodeType get_node_type<ForStatement>() { return NodeType::ForStatement; }
template <> constexpr NodeType get_node_type<WhileStatement>() { return NodeType::WhileStatement; }
template <> constexpr NodeType get_node_type<CaseStatement>() { return NodeType::CaseStatement; }
template <> constexpr NodeType get_node_type<CasexStatement>() { return NodeType::CasexStatement; }
template <> constexpr NodeType get_node_type<CasezStatement>() { return NodeType::CasezStatement; }
template <> constexpr NodeType get_node_type<UniqueCaseStatement>()
{
    return NodeType::UniqueCaseStatement;
}
template <> constexpr NodeType get_node_type<PriorityCaseStatement>()
{
    return NodeType::PriorityCaseStatement;
}
template <> constexpr NodeType get_node_type<Case>() { return NodeType::Case; }
template <> constexpr NodeType get_node_type<Block>() { return NodeType::Block; }
template <> constexpr NodeType get_node_type<Initial>() { return NodeType::Initial; }
template <> constexpr NodeType get_node_type<EventStatement>() { return NodeType::EventStatement; }
template <> constexpr NodeType get_node_type<WaitStatement>() { return NodeType::WaitStatement; }
template <> constexpr NodeType get_node_type<ForeverStatement>()
{
    return NodeType::ForeverStatement;
}
template <> constexpr NodeType get_node_type<DelayStatement>() { return NodeType::DelayStatement; }
template <> constexpr NodeType get_node_type<Instancelist>() { return NodeType::Instancelist; }
template <> constexpr NodeType get_node_type<Instance>() { return NodeType::Instance; }
template <> constexpr NodeType get_node_type<ParamArg>() { return NodeType::ParamArg; }
template <> constexpr NodeType get_node_type<PortArg>() { return NodeType::PortArg; }
template <> constexpr NodeType get_node_type<Function>() { return NodeType::Function; }
template <> constexpr NodeType get_node_type<FunctionCall>() { return NodeType::FunctionCall; }
template <> constexpr NodeType get_node_type<Task>() { return NodeType::Task; }
template <> constexpr NodeType get_node_type<TaskCall>() { return NodeType::TaskCall; }
template <> constexpr NodeType get_node_type<GenerateStatement>()
{
    return NodeType::GenerateStatement;
}
template <> constexpr NodeType get_node_type<SystemCall>() { return NodeType::SystemCall; }
template <> constexpr NodeType get_node_type<HierLabel>() { return NodeType::HierLabel; }
template <> constexpr NodeType get_node_type<HierName>() { return NodeType::HierName; }
template <> constexpr NodeType get_node_type<Disable>() { return NodeType::Disable; }
template <> constexpr NodeType get_node_type<ParallelBlock>() { return NodeType::ParallelBlock; }
template <> constexpr NodeType get_node_type<SingleStatement>()
{
    return NodeType::SingleStatement;
}

} // namespace AST
} // namespace Veriparse

#endif