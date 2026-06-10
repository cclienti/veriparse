// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/node_ostream.hpp>
#include <veriparse/AST/nodes.hpp>

namespace Veriparse
{
namespace AST
{

std::ostream &operator<<(std::ostream &os, const Node &p)
{
    switch(p.get_node_type()) {
    case NodeType::Source:
        os << static_cast<const AST::Source &>(p);
        break;
    case NodeType::Description:
        os << static_cast<const AST::Description &>(p);
        break;
    case NodeType::Pragmalist:
        os << static_cast<const AST::Pragmalist &>(p);
        break;
    case NodeType::Pragma:
        os << static_cast<const AST::Pragma &>(p);
        break;
    case NodeType::Module:
        os << static_cast<const AST::Module &>(p);
        break;
    case NodeType::Port:
        os << static_cast<const AST::Port &>(p);
        break;
    case NodeType::Package:
        os << static_cast<const AST::Package &>(p);
        break;
    case NodeType::Import:
        os << static_cast<const AST::Import &>(p);
        break;
    case NodeType::Identifier:
        os << static_cast<const AST::Identifier &>(p);
        break;
    case NodeType::Call:
        os << static_cast<const AST::Call &>(p);
        break;
    case NodeType::Constant:
        os << static_cast<const AST::Constant &>(p);
        break;
    case NodeType::StringConst:
        os << static_cast<const AST::StringConst &>(p);
        break;
    case NodeType::IntConst:
        os << static_cast<const AST::IntConst &>(p);
        break;
    case NodeType::IntConstN:
        os << static_cast<const AST::IntConstN &>(p);
        break;
    case NodeType::FloatConst:
        os << static_cast<const AST::FloatConst &>(p);
        break;
    case NodeType::DataType:
        os << static_cast<const AST::DataType &>(p);
        break;
    case NodeType::LogicType:
        os << static_cast<const AST::LogicType &>(p);
        break;
    case NodeType::RegType:
        os << static_cast<const AST::RegType &>(p);
        break;
    case NodeType::BitType:
        os << static_cast<const AST::BitType &>(p);
        break;
    case NodeType::ByteType:
        os << static_cast<const AST::ByteType &>(p);
        break;
    case NodeType::ShortintType:
        os << static_cast<const AST::ShortintType &>(p);
        break;
    case NodeType::IntType:
        os << static_cast<const AST::IntType &>(p);
        break;
    case NodeType::LongintType:
        os << static_cast<const AST::LongintType &>(p);
        break;
    case NodeType::IntegerType:
        os << static_cast<const AST::IntegerType &>(p);
        break;
    case NodeType::TimeType:
        os << static_cast<const AST::TimeType &>(p);
        break;
    case NodeType::RealType:
        os << static_cast<const AST::RealType &>(p);
        break;
    case NodeType::ShortrealType:
        os << static_cast<const AST::ShortrealType &>(p);
        break;
    case NodeType::RealtimeType:
        os << static_cast<const AST::RealtimeType &>(p);
        break;
    case NodeType::StringType:
        os << static_cast<const AST::StringType &>(p);
        break;
    case NodeType::ChandleType:
        os << static_cast<const AST::ChandleType &>(p);
        break;
    case NodeType::EventType:
        os << static_cast<const AST::EventType &>(p);
        break;
    case NodeType::ImplicitType:
        os << static_cast<const AST::ImplicitType &>(p);
        break;
    case NodeType::VoidType:
        os << static_cast<const AST::VoidType &>(p);
        break;
    case NodeType::NamedType:
        os << static_cast<const AST::NamedType &>(p);
        break;
    case NodeType::ScopeName:
        os << static_cast<const AST::ScopeName &>(p);
        break;
    case NodeType::StructType:
        os << static_cast<const AST::StructType &>(p);
        break;
    case NodeType::UnionType:
        os << static_cast<const AST::UnionType &>(p);
        break;
    case NodeType::EnumType:
        os << static_cast<const AST::EnumType &>(p);
        break;
    case NodeType::EnumItem:
        os << static_cast<const AST::EnumItem &>(p);
        break;
    case NodeType::TypeOpExpr:
        os << static_cast<const AST::TypeOpExpr &>(p);
        break;
    case NodeType::TypeOpType:
        os << static_cast<const AST::TypeOpType &>(p);
        break;
    case NodeType::Dimension:
        os << static_cast<const AST::Dimension &>(p);
        break;
    case NodeType::RangeDim:
        os << static_cast<const AST::RangeDim &>(p);
        break;
    case NodeType::SizeDim:
        os << static_cast<const AST::SizeDim &>(p);
        break;
    case NodeType::UnsizedDim:
        os << static_cast<const AST::UnsizedDim &>(p);
        break;
    case NodeType::QueueDim:
        os << static_cast<const AST::QueueDim &>(p);
        break;
    case NodeType::AssocDim:
        os << static_cast<const AST::AssocDim &>(p);
        break;
    case NodeType::Declaration:
        os << static_cast<const AST::Declaration &>(p);
        break;
    case NodeType::Var:
        os << static_cast<const AST::Var &>(p);
        break;
    case NodeType::Net:
        os << static_cast<const AST::Net &>(p);
        break;
    case NodeType::WireNet:
        os << static_cast<const AST::WireNet &>(p);
        break;
    case NodeType::TriNet:
        os << static_cast<const AST::TriNet &>(p);
        break;
    case NodeType::Tri0Net:
        os << static_cast<const AST::Tri0Net &>(p);
        break;
    case NodeType::Tri1Net:
        os << static_cast<const AST::Tri1Net &>(p);
        break;
    case NodeType::TriandNet:
        os << static_cast<const AST::TriandNet &>(p);
        break;
    case NodeType::TriorNet:
        os << static_cast<const AST::TriorNet &>(p);
        break;
    case NodeType::TriregNet:
        os << static_cast<const AST::TriregNet &>(p);
        break;
    case NodeType::WandNet:
        os << static_cast<const AST::WandNet &>(p);
        break;
    case NodeType::WorNet:
        os << static_cast<const AST::WorNet &>(p);
        break;
    case NodeType::UwireNet:
        os << static_cast<const AST::UwireNet &>(p);
        break;
    case NodeType::Supply0Net:
        os << static_cast<const AST::Supply0Net &>(p);
        break;
    case NodeType::Supply1Net:
        os << static_cast<const AST::Supply1Net &>(p);
        break;
    case NodeType::InterconnectNet:
        os << static_cast<const AST::InterconnectNet &>(p);
        break;
    case NodeType::UserNet:
        os << static_cast<const AST::UserNet &>(p);
        break;
    case NodeType::ImplicitNet:
        os << static_cast<const AST::ImplicitNet &>(p);
        break;
    case NodeType::NetTypeDecl:
        os << static_cast<const AST::NetTypeDecl &>(p);
        break;
    case NodeType::Strength:
        os << static_cast<const AST::Strength &>(p);
        break;
    case NodeType::DriveStrength:
        os << static_cast<const AST::DriveStrength &>(p);
        break;
    case NodeType::ChargeStrength:
        os << static_cast<const AST::ChargeStrength &>(p);
        break;
    case NodeType::Param:
        os << static_cast<const AST::Param &>(p);
        break;
    case NodeType::TypeParam:
        os << static_cast<const AST::TypeParam &>(p);
        break;
    case NodeType::Typedef:
        os << static_cast<const AST::Typedef &>(p);
        break;
    case NodeType::Member:
        os << static_cast<const AST::Member &>(p);
        break;
    case NodeType::Arg:
        os << static_cast<const AST::Arg &>(p);
        break;
    case NodeType::Genvar:
        os << static_cast<const AST::Genvar &>(p);
        break;
    case NodeType::Concat:
        os << static_cast<const AST::Concat &>(p);
        break;
    case NodeType::Lconcat:
        os << static_cast<const AST::Lconcat &>(p);
        break;
    case NodeType::Repeat:
        os << static_cast<const AST::Repeat &>(p);
        break;
    case NodeType::AssignmentPattern:
        os << static_cast<const AST::AssignmentPattern &>(p);
        break;
    case NodeType::PatternItem:
        os << static_cast<const AST::PatternItem &>(p);
        break;
    case NodeType::Cast:
        os << static_cast<const AST::Cast &>(p);
        break;
    case NodeType::TypeCast:
        os << static_cast<const AST::TypeCast &>(p);
        break;
    case NodeType::SizeCast:
        os << static_cast<const AST::SizeCast &>(p);
        break;
    case NodeType::SigningCast:
        os << static_cast<const AST::SigningCast &>(p);
        break;
    case NodeType::ConstCast:
        os << static_cast<const AST::ConstCast &>(p);
        break;
    case NodeType::Indirect:
        os << static_cast<const AST::Indirect &>(p);
        break;
    case NodeType::Partselect:
        os << static_cast<const AST::Partselect &>(p);
        break;
    case NodeType::PartselectIndexed:
        os << static_cast<const AST::PartselectIndexed &>(p);
        break;
    case NodeType::PartselectPlusIndexed:
        os << static_cast<const AST::PartselectPlusIndexed &>(p);
        break;
    case NodeType::PartselectMinusIndexed:
        os << static_cast<const AST::PartselectMinusIndexed &>(p);
        break;
    case NodeType::Pointer:
        os << static_cast<const AST::Pointer &>(p);
        break;
    case NodeType::Lvalue:
        os << static_cast<const AST::Lvalue &>(p);
        break;
    case NodeType::Rvalue:
        os << static_cast<const AST::Rvalue &>(p);
        break;
    case NodeType::UnaryOperator:
        os << static_cast<const AST::UnaryOperator &>(p);
        break;
    case NodeType::Uplus:
        os << static_cast<const AST::Uplus &>(p);
        break;
    case NodeType::Uminus:
        os << static_cast<const AST::Uminus &>(p);
        break;
    case NodeType::Ulnot:
        os << static_cast<const AST::Ulnot &>(p);
        break;
    case NodeType::Unot:
        os << static_cast<const AST::Unot &>(p);
        break;
    case NodeType::Uand:
        os << static_cast<const AST::Uand &>(p);
        break;
    case NodeType::Unand:
        os << static_cast<const AST::Unand &>(p);
        break;
    case NodeType::Uor:
        os << static_cast<const AST::Uor &>(p);
        break;
    case NodeType::Unor:
        os << static_cast<const AST::Unor &>(p);
        break;
    case NodeType::Uxor:
        os << static_cast<const AST::Uxor &>(p);
        break;
    case NodeType::Uxnor:
        os << static_cast<const AST::Uxnor &>(p);
        break;
    case NodeType::Operator:
        os << static_cast<const AST::Operator &>(p);
        break;
    case NodeType::Power:
        os << static_cast<const AST::Power &>(p);
        break;
    case NodeType::Times:
        os << static_cast<const AST::Times &>(p);
        break;
    case NodeType::Divide:
        os << static_cast<const AST::Divide &>(p);
        break;
    case NodeType::Mod:
        os << static_cast<const AST::Mod &>(p);
        break;
    case NodeType::Plus:
        os << static_cast<const AST::Plus &>(p);
        break;
    case NodeType::Minus:
        os << static_cast<const AST::Minus &>(p);
        break;
    case NodeType::Sll:
        os << static_cast<const AST::Sll &>(p);
        break;
    case NodeType::Srl:
        os << static_cast<const AST::Srl &>(p);
        break;
    case NodeType::Sra:
        os << static_cast<const AST::Sra &>(p);
        break;
    case NodeType::LessThan:
        os << static_cast<const AST::LessThan &>(p);
        break;
    case NodeType::GreaterThan:
        os << static_cast<const AST::GreaterThan &>(p);
        break;
    case NodeType::LessEq:
        os << static_cast<const AST::LessEq &>(p);
        break;
    case NodeType::GreaterEq:
        os << static_cast<const AST::GreaterEq &>(p);
        break;
    case NodeType::Eq:
        os << static_cast<const AST::Eq &>(p);
        break;
    case NodeType::NotEq:
        os << static_cast<const AST::NotEq &>(p);
        break;
    case NodeType::Eql:
        os << static_cast<const AST::Eql &>(p);
        break;
    case NodeType::NotEql:
        os << static_cast<const AST::NotEql &>(p);
        break;
    case NodeType::And:
        os << static_cast<const AST::And &>(p);
        break;
    case NodeType::Xor:
        os << static_cast<const AST::Xor &>(p);
        break;
    case NodeType::Xnor:
        os << static_cast<const AST::Xnor &>(p);
        break;
    case NodeType::Or:
        os << static_cast<const AST::Or &>(p);
        break;
    case NodeType::Land:
        os << static_cast<const AST::Land &>(p);
        break;
    case NodeType::Lor:
        os << static_cast<const AST::Lor &>(p);
        break;
    case NodeType::Cond:
        os << static_cast<const AST::Cond &>(p);
        break;
    case NodeType::Always:
        os << static_cast<const AST::Always &>(p);
        break;
    case NodeType::AlwaysFF:
        os << static_cast<const AST::AlwaysFF &>(p);
        break;
    case NodeType::AlwaysComb:
        os << static_cast<const AST::AlwaysComb &>(p);
        break;
    case NodeType::AlwaysLatch:
        os << static_cast<const AST::AlwaysLatch &>(p);
        break;
    case NodeType::Senslist:
        os << static_cast<const AST::Senslist &>(p);
        break;
    case NodeType::Sens:
        os << static_cast<const AST::Sens &>(p);
        break;
    case NodeType::Defparamlist:
        os << static_cast<const AST::Defparamlist &>(p);
        break;
    case NodeType::Defparam:
        os << static_cast<const AST::Defparam &>(p);
        break;
    case NodeType::Assign:
        os << static_cast<const AST::Assign &>(p);
        break;
    case NodeType::BlockingSubstitution:
        os << static_cast<const AST::BlockingSubstitution &>(p);
        break;
    case NodeType::NonblockingSubstitution:
        os << static_cast<const AST::NonblockingSubstitution &>(p);
        break;
    case NodeType::IfStatement:
        os << static_cast<const AST::IfStatement &>(p);
        break;
    case NodeType::RepeatStatement:
        os << static_cast<const AST::RepeatStatement &>(p);
        break;
    case NodeType::ForStatement:
        os << static_cast<const AST::ForStatement &>(p);
        break;
    case NodeType::WhileStatement:
        os << static_cast<const AST::WhileStatement &>(p);
        break;
    case NodeType::CaseStatement:
        os << static_cast<const AST::CaseStatement &>(p);
        break;
    case NodeType::CasexStatement:
        os << static_cast<const AST::CasexStatement &>(p);
        break;
    case NodeType::CasezStatement:
        os << static_cast<const AST::CasezStatement &>(p);
        break;
    case NodeType::UniqueCaseStatement:
        os << static_cast<const AST::UniqueCaseStatement &>(p);
        break;
    case NodeType::PriorityCaseStatement:
        os << static_cast<const AST::PriorityCaseStatement &>(p);
        break;
    case NodeType::Case:
        os << static_cast<const AST::Case &>(p);
        break;
    case NodeType::Block:
        os << static_cast<const AST::Block &>(p);
        break;
    case NodeType::Initial:
        os << static_cast<const AST::Initial &>(p);
        break;
    case NodeType::EventStatement:
        os << static_cast<const AST::EventStatement &>(p);
        break;
    case NodeType::WaitStatement:
        os << static_cast<const AST::WaitStatement &>(p);
        break;
    case NodeType::ForeverStatement:
        os << static_cast<const AST::ForeverStatement &>(p);
        break;
    case NodeType::DelayStatement:
        os << static_cast<const AST::DelayStatement &>(p);
        break;
    case NodeType::Instancelist:
        os << static_cast<const AST::Instancelist &>(p);
        break;
    case NodeType::Instance:
        os << static_cast<const AST::Instance &>(p);
        break;
    case NodeType::ParamArg:
        os << static_cast<const AST::ParamArg &>(p);
        break;
    case NodeType::PortArg:
        os << static_cast<const AST::PortArg &>(p);
        break;
    case NodeType::Function:
        os << static_cast<const AST::Function &>(p);
        break;
    case NodeType::FunctionCall:
        os << static_cast<const AST::FunctionCall &>(p);
        break;
    case NodeType::Task:
        os << static_cast<const AST::Task &>(p);
        break;
    case NodeType::TaskCall:
        os << static_cast<const AST::TaskCall &>(p);
        break;
    case NodeType::GenerateStatement:
        os << static_cast<const AST::GenerateStatement &>(p);
        break;
    case NodeType::SystemCall:
        os << static_cast<const AST::SystemCall &>(p);
        break;
    case NodeType::HierLabel:
        os << static_cast<const AST::HierLabel &>(p);
        break;
    case NodeType::HierName:
        os << static_cast<const AST::HierName &>(p);
        break;
    case NodeType::Disable:
        os << static_cast<const AST::Disable &>(p);
        break;
    case NodeType::ParallelBlock:
        os << static_cast<const AST::ParallelBlock &>(p);
        break;
    case NodeType::SingleStatement:
        os << static_cast<const AST::SingleStatement &>(p);
        break;
    default:
        os << "Node: {}";
    }

    return os;
}

std::ostream &operator<<(std::ostream &os, const Node::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "Node: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse