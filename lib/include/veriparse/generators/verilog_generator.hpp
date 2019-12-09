#ifndef VERIPARSE_GENERATORS_VERILOG_GENERATOR
#define VERIPARSE_GENERATORS_VERILOG_GENERATOR

#include <string>
#include <veriparse/generators/generator_base.hpp>


namespace Veriparse {
namespace Generators {

class VerilogGenerator : public GeneratorBase<std::string> {
	virtual std::string render_node(const AST::Node::Ptr node) const;
	virtual std::string render_source(const AST::Source::Ptr node) const;
	virtual std::string render_description(const AST::Description::Ptr node) const;
	virtual std::string render_pragmalist(const AST::Pragmalist::Ptr node) const;
	virtual std::string render_pragma(const AST::Pragma::Ptr node) const;
	virtual std::string render_module(const AST::Module::Ptr node) const;
	virtual std::string render_port(const AST::Port::Ptr node) const;
	virtual std::string render_width(const AST::Width::Ptr node) const;
	virtual std::string render_length(const AST::Length::Ptr node) const;
	virtual std::string render_identifier(const AST::Identifier::Ptr node) const;
	virtual std::string render_intconst(const AST::IntConst::Ptr node) const;
	virtual std::string render_intconstn(const AST::IntConstN::Ptr node) const;
	virtual std::string render_floatconst(const AST::FloatConst::Ptr node) const;
	virtual std::string render_stringconst(const AST::StringConst::Ptr node) const;
	virtual std::string render_input(const AST::Input::Ptr node) const;
	virtual std::string render_output(const AST::Output::Ptr node) const;
	virtual std::string render_inout(const AST::Inout::Ptr node) const;
	virtual std::string render_tri(const AST::Tri::Ptr node) const;
	virtual std::string render_wire(const AST::Wire::Ptr node) const;
	virtual std::string render_reg(const AST::Reg::Ptr node) const;
	virtual std::string render_supply0(const AST::Supply0::Ptr node) const;
	virtual std::string render_supply1(const AST::Supply1::Ptr node) const;
	virtual std::string render_integer(const AST::Integer::Ptr node) const;
	virtual std::string render_real(const AST::Real::Ptr node) const;
	virtual std::string render_genvar(const AST::Genvar::Ptr node) const;
	virtual std::string render_ioport(const AST::Ioport::Ptr node) const;
	virtual std::string render_parameter(const AST::Parameter::Ptr node) const;
	virtual std::string render_localparam(const AST::Localparam::Ptr node) const;
	virtual std::string render_concat(const AST::Concat::Ptr node) const;
	virtual std::string render_lconcat(const AST::Lconcat::Ptr node) const;
	virtual std::string render_repeat(const AST::Repeat::Ptr node) const;
	virtual std::string render_partselect(const AST::Partselect::Ptr node) const;
	virtual std::string render_partselectplusindexed(const AST::PartselectPlusIndexed::Ptr node) const;
	virtual std::string render_partselectminusindexed(const AST::PartselectMinusIndexed::Ptr node) const;
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
	virtual std::string render_always(const AST::Always::Ptr node) const;
	virtual std::string render_senslist(const AST::Senslist::Ptr node) const;
	virtual std::string render_sens(const AST::Sens::Ptr node) const;
	virtual std::string render_defparamlist(const AST::Defparamlist::Ptr node) const;
	virtual std::string render_defparam(const AST::Defparam::Ptr node) const;
	virtual std::string render_assign(const AST::Assign::Ptr node) const;
	virtual std::string render_blockingsubstitution(const AST::BlockingSubstitution::Ptr node) const;
	virtual std::string render_nonblockingsubstitution(const AST::NonblockingSubstitution::Ptr node) const;
	virtual std::string render_ifstatement(const AST::IfStatement::Ptr node) const;
	virtual std::string render_repeatstatement(const AST::RepeatStatement::Ptr node) const;
	virtual std::string render_forstatement(const AST::ForStatement::Ptr node) const;
	virtual std::string render_whilestatement(const AST::WhileStatement::Ptr node) const;
	virtual std::string render_casestatement(const AST::CaseStatement::Ptr node) const;
	virtual std::string render_casexstatement(const AST::CasexStatement::Ptr node) const;
	virtual std::string render_casezstatement(const AST::CasezStatement::Ptr node) const;
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
	virtual std::string render_functioncall(const AST::FunctionCall::Ptr node) const;
	virtual std::string render_task(const AST::Task::Ptr node) const;
	virtual std::string render_taskcall(const AST::TaskCall::Ptr node) const;
	virtual std::string render_generatestatement(const AST::GenerateStatement::Ptr node) const;
	virtual std::string render_systemcall(const AST::SystemCall::Ptr node) const;
	virtual std::string render_identifierscopelabel(const AST::IdentifierScopeLabel::Ptr node) const;
	virtual std::string render_identifierscope(const AST::IdentifierScope::Ptr node) const;
	virtual std::string render_disable(const AST::Disable::Ptr node) const;
	virtual std::string render_parallelblock(const AST::ParallelBlock::Ptr node) const;
	virtual std::string render_singlestatement(const AST::SingleStatement::Ptr node) const;

	virtual std::string indent(const std::string str) const;

	virtual std::string unary_operators_to_string(AST::NodeType node_type,
	                                              const AST::Node::Ptr right) const;

	virtual std::string operators_to_string(AST::NodeType node_type, const AST::Node::Ptr left,
	                                        const AST::Node::Ptr right) const;

	virtual std::string variable_to_string(const char *variable, bool is_signed,
	                                       const AST::Width::ListPtr widths,
	                                       const AST::Length::ListPtr lengths,
	                                       const AST::Rvalue::Ptr rvalue,
	                                       const std::string &name) const;

	virtual std::string parameter_to_string(const char *parameter, std::string variable,
	                                        bool is_signed, const AST::Width::ListPtr widths,
	                                        const std::string &name, const std::string value) const;

	virtual std::string block_or_single_statement_to_string(AST::Node::Ptr stmt) const;

	virtual std::string widths_list_to_string(const AST::Width::ListPtr widths) const;

	virtual std::string ports_list_to_string(const AST::Node::ListPtr ports, int length=0) const;

	virtual std::string parameters_list_to_string(const AST::Parameter::ListPtr parameters,
	                                              int length=0) const;


};

}
}

#endif
