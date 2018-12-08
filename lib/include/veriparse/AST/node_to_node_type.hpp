#ifndef VERIPARSE_AST_NODE_TO_NODE_TYPE_HPP
#define VERIPARSE_AST_NODE_TO_NODE_TYPE_HPP

#include <veriparse/AST/nodes.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

template<typename NodeT> constexpr NodeType get_node_type() {
	return NodeType::None;
}

template<> constexpr NodeType get_node_type<Node>() {return NodeType::Node;}
template<> constexpr NodeType get_node_type<Source>() {return NodeType::Source;}
template<> constexpr NodeType get_node_type<Description>() {return NodeType::Description;}
template<> constexpr NodeType get_node_type<Pragma>() {return NodeType::Pragma;}
template<> constexpr NodeType get_node_type<Module>() {return NodeType::Module;}
template<> constexpr NodeType get_node_type<Port>() {return NodeType::Port;}
template<> constexpr NodeType get_node_type<Width>() {return NodeType::Width;}
template<> constexpr NodeType get_node_type<Length>() {return NodeType::Length;}
template<> constexpr NodeType get_node_type<Identifier>() {return NodeType::Identifier;}
template<> constexpr NodeType get_node_type<Constant>() {return NodeType::Constant;}
template<> constexpr NodeType get_node_type<StringConst>() {return NodeType::StringConst;}
template<> constexpr NodeType get_node_type<IntConst>() {return NodeType::IntConst;}
template<> constexpr NodeType get_node_type<IntConstN>() {return NodeType::IntConstN;}
template<> constexpr NodeType get_node_type<FloatConst>() {return NodeType::FloatConst;}
template<> constexpr NodeType get_node_type<IODir>() {return NodeType::IODir;}
template<> constexpr NodeType get_node_type<Input>() {return NodeType::Input;}
template<> constexpr NodeType get_node_type<Output>() {return NodeType::Output;}
template<> constexpr NodeType get_node_type<Inout>() {return NodeType::Inout;}
template<> constexpr NodeType get_node_type<VariableBase>() {return NodeType::VariableBase;}
template<> constexpr NodeType get_node_type<Genvar>() {return NodeType::Genvar;}
template<> constexpr NodeType get_node_type<Variable>() {return NodeType::Variable;}
template<> constexpr NodeType get_node_type<Net>() {return NodeType::Net;}
template<> constexpr NodeType get_node_type<Integer>() {return NodeType::Integer;}
template<> constexpr NodeType get_node_type<Real>() {return NodeType::Real;}
template<> constexpr NodeType get_node_type<Tri>() {return NodeType::Tri;}
template<> constexpr NodeType get_node_type<Wire>() {return NodeType::Wire;}
template<> constexpr NodeType get_node_type<Supply0>() {return NodeType::Supply0;}
template<> constexpr NodeType get_node_type<Supply1>() {return NodeType::Supply1;}
template<> constexpr NodeType get_node_type<Reg>() {return NodeType::Reg;}
template<> constexpr NodeType get_node_type<Ioport>() {return NodeType::Ioport;}
template<> constexpr NodeType get_node_type<Parameter>() {return NodeType::Parameter;}
template<> constexpr NodeType get_node_type<Localparam>() {return NodeType::Localparam;}
template<> constexpr NodeType get_node_type<Concat>() {return NodeType::Concat;}
template<> constexpr NodeType get_node_type<Lconcat>() {return NodeType::Lconcat;}
template<> constexpr NodeType get_node_type<Repeat>() {return NodeType::Repeat;}
template<> constexpr NodeType get_node_type<Indirect>() {return NodeType::Indirect;}
template<> constexpr NodeType get_node_type<Partselect>() {return NodeType::Partselect;}
template<> constexpr NodeType get_node_type<PartselectIndexed>() {return NodeType::PartselectIndexed;}
template<> constexpr NodeType get_node_type<PartselectPlusIndexed>() {return NodeType::PartselectPlusIndexed;}
template<> constexpr NodeType get_node_type<PartselectMinusIndexed>() {return NodeType::PartselectMinusIndexed;}
template<> constexpr NodeType get_node_type<Pointer>() {return NodeType::Pointer;}
template<> constexpr NodeType get_node_type<Lvalue>() {return NodeType::Lvalue;}
template<> constexpr NodeType get_node_type<Rvalue>() {return NodeType::Rvalue;}
template<> constexpr NodeType get_node_type<UnaryOperator>() {return NodeType::UnaryOperator;}
template<> constexpr NodeType get_node_type<Uplus>() {return NodeType::Uplus;}
template<> constexpr NodeType get_node_type<Uminus>() {return NodeType::Uminus;}
template<> constexpr NodeType get_node_type<Ulnot>() {return NodeType::Ulnot;}
template<> constexpr NodeType get_node_type<Unot>() {return NodeType::Unot;}
template<> constexpr NodeType get_node_type<Uand>() {return NodeType::Uand;}
template<> constexpr NodeType get_node_type<Unand>() {return NodeType::Unand;}
template<> constexpr NodeType get_node_type<Uor>() {return NodeType::Uor;}
template<> constexpr NodeType get_node_type<Unor>() {return NodeType::Unor;}
template<> constexpr NodeType get_node_type<Uxor>() {return NodeType::Uxor;}
template<> constexpr NodeType get_node_type<Uxnor>() {return NodeType::Uxnor;}
template<> constexpr NodeType get_node_type<Operator>() {return NodeType::Operator;}
template<> constexpr NodeType get_node_type<Power>() {return NodeType::Power;}
template<> constexpr NodeType get_node_type<Times>() {return NodeType::Times;}
template<> constexpr NodeType get_node_type<Divide>() {return NodeType::Divide;}
template<> constexpr NodeType get_node_type<Mod>() {return NodeType::Mod;}
template<> constexpr NodeType get_node_type<Plus>() {return NodeType::Plus;}
template<> constexpr NodeType get_node_type<Minus>() {return NodeType::Minus;}
template<> constexpr NodeType get_node_type<Sll>() {return NodeType::Sll;}
template<> constexpr NodeType get_node_type<Srl>() {return NodeType::Srl;}
template<> constexpr NodeType get_node_type<Sra>() {return NodeType::Sra;}
template<> constexpr NodeType get_node_type<LessThan>() {return NodeType::LessThan;}
template<> constexpr NodeType get_node_type<GreaterThan>() {return NodeType::GreaterThan;}
template<> constexpr NodeType get_node_type<LessEq>() {return NodeType::LessEq;}
template<> constexpr NodeType get_node_type<GreaterEq>() {return NodeType::GreaterEq;}
template<> constexpr NodeType get_node_type<Eq>() {return NodeType::Eq;}
template<> constexpr NodeType get_node_type<NotEq>() {return NodeType::NotEq;}
template<> constexpr NodeType get_node_type<Eql>() {return NodeType::Eql;}
template<> constexpr NodeType get_node_type<NotEql>() {return NodeType::NotEql;}
template<> constexpr NodeType get_node_type<And>() {return NodeType::And;}
template<> constexpr NodeType get_node_type<Xor>() {return NodeType::Xor;}
template<> constexpr NodeType get_node_type<Xnor>() {return NodeType::Xnor;}
template<> constexpr NodeType get_node_type<Or>() {return NodeType::Or;}
template<> constexpr NodeType get_node_type<Land>() {return NodeType::Land;}
template<> constexpr NodeType get_node_type<Lor>() {return NodeType::Lor;}
template<> constexpr NodeType get_node_type<Cond>() {return NodeType::Cond;}
template<> constexpr NodeType get_node_type<Always>() {return NodeType::Always;}
template<> constexpr NodeType get_node_type<Senslist>() {return NodeType::Senslist;}
template<> constexpr NodeType get_node_type<Sens>() {return NodeType::Sens;}
template<> constexpr NodeType get_node_type<Defparamlist>() {return NodeType::Defparamlist;}
template<> constexpr NodeType get_node_type<Defparam>() {return NodeType::Defparam;}
template<> constexpr NodeType get_node_type<Assign>() {return NodeType::Assign;}
template<> constexpr NodeType get_node_type<BlockingSubstitution>() {return NodeType::BlockingSubstitution;}
template<> constexpr NodeType get_node_type<NonblockingSubstitution>() {return NodeType::NonblockingSubstitution;}
template<> constexpr NodeType get_node_type<IfStatement>() {return NodeType::IfStatement;}
template<> constexpr NodeType get_node_type<RepeatStatement>() {return NodeType::RepeatStatement;}
template<> constexpr NodeType get_node_type<ForStatement>() {return NodeType::ForStatement;}
template<> constexpr NodeType get_node_type<WhileStatement>() {return NodeType::WhileStatement;}
template<> constexpr NodeType get_node_type<CaseStatement>() {return NodeType::CaseStatement;}
template<> constexpr NodeType get_node_type<CasexStatement>() {return NodeType::CasexStatement;}
template<> constexpr NodeType get_node_type<CasezStatement>() {return NodeType::CasezStatement;}
template<> constexpr NodeType get_node_type<Case>() {return NodeType::Case;}
template<> constexpr NodeType get_node_type<Block>() {return NodeType::Block;}
template<> constexpr NodeType get_node_type<Initial>() {return NodeType::Initial;}
template<> constexpr NodeType get_node_type<EventStatement>() {return NodeType::EventStatement;}
template<> constexpr NodeType get_node_type<WaitStatement>() {return NodeType::WaitStatement;}
template<> constexpr NodeType get_node_type<ForeverStatement>() {return NodeType::ForeverStatement;}
template<> constexpr NodeType get_node_type<DelayStatement>() {return NodeType::DelayStatement;}
template<> constexpr NodeType get_node_type<Instancelist>() {return NodeType::Instancelist;}
template<> constexpr NodeType get_node_type<Instance>() {return NodeType::Instance;}
template<> constexpr NodeType get_node_type<ParamArg>() {return NodeType::ParamArg;}
template<> constexpr NodeType get_node_type<PortArg>() {return NodeType::PortArg;}
template<> constexpr NodeType get_node_type<Function>() {return NodeType::Function;}
template<> constexpr NodeType get_node_type<FunctionCall>() {return NodeType::FunctionCall;}
template<> constexpr NodeType get_node_type<Task>() {return NodeType::Task;}
template<> constexpr NodeType get_node_type<TaskCall>() {return NodeType::TaskCall;}
template<> constexpr NodeType get_node_type<GenerateStatement>() {return NodeType::GenerateStatement;}
template<> constexpr NodeType get_node_type<SystemCall>() {return NodeType::SystemCall;}
template<> constexpr NodeType get_node_type<IdentifierScopeLabel>() {return NodeType::IdentifierScopeLabel;}
template<> constexpr NodeType get_node_type<IdentifierScope>() {return NodeType::IdentifierScope;}
template<> constexpr NodeType get_node_type<Disable>() {return NodeType::Disable;}
template<> constexpr NodeType get_node_type<ParallelBlock>() {return NodeType::ParallelBlock;}
template<> constexpr NodeType get_node_type<SingleStatement>() {return NodeType::SingleStatement;}

}
}

#endif