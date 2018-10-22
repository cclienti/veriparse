#ifndef VERIPARSE_GENERATORS_GENERATOR_BASE
#define VERIPARSE_GENERATORS_GENERATOR_BASE

#include <veriparse/AST/nodes.hpp>

namespace Veriparse {
	namespace Generators {

		template <typename T>
		class GeneratorBase {
		public:

			/**
			 * Constructor, can use tabs or spaces to indent.
			 */
			GeneratorBase(bool use_tabs=false, unsigned int indent_length=2) {
				if (use_tabs) {
					m_indent_str.assign(1, '\t');
				}
				else {
					m_indent_str.assign(indent_length, ' ');
				}
			}

			virtual ~GeneratorBase(void) {}

			/**
			 * Return the indent string built using constructor
			 * parameters.
			 */
			const std::string &get_indent_str(void) const {
				return m_indent_str;
			}

			/**
			 * Main rendering method. When the node type is from the base
			 * class, it dispatches the work to the right protected
			 * rendering method depending on the internal node type.
			 */
			T render(const AST::Node::Ptr node) const {
				if (node) {
					switch(node->get_node_type()) {
					case AST::NodeType::Source:
					return render_source(AST::cast_to<AST::Source>(node));
					
					case AST::NodeType::Description:
					return render_description(AST::cast_to<AST::Description>(node));
					
					case AST::NodeType::Pragma:
					return render_pragma(AST::cast_to<AST::Pragma>(node));
					
					case AST::NodeType::Module:
					return render_module(AST::cast_to<AST::Module>(node));
					
					case AST::NodeType::Port:
					return render_port(AST::cast_to<AST::Port>(node));
					
					case AST::NodeType::Width:
					return render_width(AST::cast_to<AST::Width>(node));
					
					case AST::NodeType::Length:
					return render_length(AST::cast_to<AST::Length>(node));
					
					case AST::NodeType::Identifier:
					return render_identifier(AST::cast_to<AST::Identifier>(node));
					
					case AST::NodeType::Constant:
					return render_constant(AST::cast_to<AST::Constant>(node));
					
					case AST::NodeType::StringConst:
					return render_stringconst(AST::cast_to<AST::StringConst>(node));
					
					case AST::NodeType::IntConst:
					return render_intconst(AST::cast_to<AST::IntConst>(node));
					
					case AST::NodeType::IntConstN:
					return render_intconstn(AST::cast_to<AST::IntConstN>(node));
					
					case AST::NodeType::FloatConst:
					return render_floatconst(AST::cast_to<AST::FloatConst>(node));
					
					case AST::NodeType::IODir:
					return render_iodir(AST::cast_to<AST::IODir>(node));
					
					case AST::NodeType::Input:
					return render_input(AST::cast_to<AST::Input>(node));
					
					case AST::NodeType::Output:
					return render_output(AST::cast_to<AST::Output>(node));
					
					case AST::NodeType::Inout:
					return render_inout(AST::cast_to<AST::Inout>(node));
					
					case AST::NodeType::VariableBase:
					return render_variablebase(AST::cast_to<AST::VariableBase>(node));
					
					case AST::NodeType::Genvar:
					return render_genvar(AST::cast_to<AST::Genvar>(node));
					
					case AST::NodeType::Variable:
					return render_variable(AST::cast_to<AST::Variable>(node));
					
					case AST::NodeType::Net:
					return render_net(AST::cast_to<AST::Net>(node));
					
					case AST::NodeType::Integer:
					return render_integer(AST::cast_to<AST::Integer>(node));
					
					case AST::NodeType::Real:
					return render_real(AST::cast_to<AST::Real>(node));
					
					case AST::NodeType::Tri:
					return render_tri(AST::cast_to<AST::Tri>(node));
					
					case AST::NodeType::Wire:
					return render_wire(AST::cast_to<AST::Wire>(node));
					
					case AST::NodeType::Supply0:
					return render_supply0(AST::cast_to<AST::Supply0>(node));
					
					case AST::NodeType::Supply1:
					return render_supply1(AST::cast_to<AST::Supply1>(node));
					
					case AST::NodeType::Reg:
					return render_reg(AST::cast_to<AST::Reg>(node));
					
					case AST::NodeType::Ioport:
					return render_ioport(AST::cast_to<AST::Ioport>(node));
					
					case AST::NodeType::Parameter:
					return render_parameter(AST::cast_to<AST::Parameter>(node));
					
					case AST::NodeType::Localparam:
					return render_localparam(AST::cast_to<AST::Localparam>(node));
					
					case AST::NodeType::Concat:
					return render_concat(AST::cast_to<AST::Concat>(node));
					
					case AST::NodeType::Lconcat:
					return render_lconcat(AST::cast_to<AST::Lconcat>(node));
					
					case AST::NodeType::Repeat:
					return render_repeat(AST::cast_to<AST::Repeat>(node));
					
					case AST::NodeType::Indirect:
					return render_indirect(AST::cast_to<AST::Indirect>(node));
					
					case AST::NodeType::Partselect:
					return render_partselect(AST::cast_to<AST::Partselect>(node));
					
					case AST::NodeType::PartselectIndexed:
					return render_partselectindexed(AST::cast_to<AST::PartselectIndexed>(node));
					
					case AST::NodeType::PartselectPlusIndexed:
					return render_partselectplusindexed(AST::cast_to<AST::PartselectPlusIndexed>(node));
					
					case AST::NodeType::PartselectMinusIndexed:
					return render_partselectminusindexed(AST::cast_to<AST::PartselectMinusIndexed>(node));
					
					case AST::NodeType::Pointer:
					return render_pointer(AST::cast_to<AST::Pointer>(node));
					
					case AST::NodeType::Lvalue:
					return render_lvalue(AST::cast_to<AST::Lvalue>(node));
					
					case AST::NodeType::Rvalue:
					return render_rvalue(AST::cast_to<AST::Rvalue>(node));
					
					case AST::NodeType::UnaryOperator:
					return render_unaryoperator(AST::cast_to<AST::UnaryOperator>(node));
					
					case AST::NodeType::Uplus:
					return render_uplus(AST::cast_to<AST::Uplus>(node));
					
					case AST::NodeType::Uminus:
					return render_uminus(AST::cast_to<AST::Uminus>(node));
					
					case AST::NodeType::Ulnot:
					return render_ulnot(AST::cast_to<AST::Ulnot>(node));
					
					case AST::NodeType::Unot:
					return render_unot(AST::cast_to<AST::Unot>(node));
					
					case AST::NodeType::Uand:
					return render_uand(AST::cast_to<AST::Uand>(node));
					
					case AST::NodeType::Unand:
					return render_unand(AST::cast_to<AST::Unand>(node));
					
					case AST::NodeType::Uor:
					return render_uor(AST::cast_to<AST::Uor>(node));
					
					case AST::NodeType::Unor:
					return render_unor(AST::cast_to<AST::Unor>(node));
					
					case AST::NodeType::Uxor:
					return render_uxor(AST::cast_to<AST::Uxor>(node));
					
					case AST::NodeType::Uxnor:
					return render_uxnor(AST::cast_to<AST::Uxnor>(node));
					
					case AST::NodeType::Operator:
					return render_operator(AST::cast_to<AST::Operator>(node));
					
					case AST::NodeType::Power:
					return render_power(AST::cast_to<AST::Power>(node));
					
					case AST::NodeType::Times:
					return render_times(AST::cast_to<AST::Times>(node));
					
					case AST::NodeType::Divide:
					return render_divide(AST::cast_to<AST::Divide>(node));
					
					case AST::NodeType::Mod:
					return render_mod(AST::cast_to<AST::Mod>(node));
					
					case AST::NodeType::Plus:
					return render_plus(AST::cast_to<AST::Plus>(node));
					
					case AST::NodeType::Minus:
					return render_minus(AST::cast_to<AST::Minus>(node));
					
					case AST::NodeType::Sll:
					return render_sll(AST::cast_to<AST::Sll>(node));
					
					case AST::NodeType::Srl:
					return render_srl(AST::cast_to<AST::Srl>(node));
					
					case AST::NodeType::Sra:
					return render_sra(AST::cast_to<AST::Sra>(node));
					
					case AST::NodeType::LessThan:
					return render_lessthan(AST::cast_to<AST::LessThan>(node));
					
					case AST::NodeType::GreaterThan:
					return render_greaterthan(AST::cast_to<AST::GreaterThan>(node));
					
					case AST::NodeType::LessEq:
					return render_lesseq(AST::cast_to<AST::LessEq>(node));
					
					case AST::NodeType::GreaterEq:
					return render_greatereq(AST::cast_to<AST::GreaterEq>(node));
					
					case AST::NodeType::Eq:
					return render_eq(AST::cast_to<AST::Eq>(node));
					
					case AST::NodeType::NotEq:
					return render_noteq(AST::cast_to<AST::NotEq>(node));
					
					case AST::NodeType::Eql:
					return render_eql(AST::cast_to<AST::Eql>(node));
					
					case AST::NodeType::NotEql:
					return render_noteql(AST::cast_to<AST::NotEql>(node));
					
					case AST::NodeType::And:
					return render_and(AST::cast_to<AST::And>(node));
					
					case AST::NodeType::Xor:
					return render_xor(AST::cast_to<AST::Xor>(node));
					
					case AST::NodeType::Xnor:
					return render_xnor(AST::cast_to<AST::Xnor>(node));
					
					case AST::NodeType::Or:
					return render_or(AST::cast_to<AST::Or>(node));
					
					case AST::NodeType::Land:
					return render_land(AST::cast_to<AST::Land>(node));
					
					case AST::NodeType::Lor:
					return render_lor(AST::cast_to<AST::Lor>(node));
					
					case AST::NodeType::Cond:
					return render_cond(AST::cast_to<AST::Cond>(node));
					
					case AST::NodeType::Always:
					return render_always(AST::cast_to<AST::Always>(node));
					
					case AST::NodeType::Senslist:
					return render_senslist(AST::cast_to<AST::Senslist>(node));
					
					case AST::NodeType::Sens:
					return render_sens(AST::cast_to<AST::Sens>(node));
					
					case AST::NodeType::Defparamlist:
					return render_defparamlist(AST::cast_to<AST::Defparamlist>(node));
					
					case AST::NodeType::Defparam:
					return render_defparam(AST::cast_to<AST::Defparam>(node));
					
					case AST::NodeType::Assign:
					return render_assign(AST::cast_to<AST::Assign>(node));
					
					case AST::NodeType::BlockingSubstitution:
					return render_blockingsubstitution(AST::cast_to<AST::BlockingSubstitution>(node));
					
					case AST::NodeType::NonblockingSubstitution:
					return render_nonblockingsubstitution(AST::cast_to<AST::NonblockingSubstitution>(node));
					
					case AST::NodeType::IfStatement:
					return render_ifstatement(AST::cast_to<AST::IfStatement>(node));
					
					case AST::NodeType::RepeatStatement:
					return render_repeatstatement(AST::cast_to<AST::RepeatStatement>(node));
					
					case AST::NodeType::ForStatement:
					return render_forstatement(AST::cast_to<AST::ForStatement>(node));
					
					case AST::NodeType::WhileStatement:
					return render_whilestatement(AST::cast_to<AST::WhileStatement>(node));
					
					case AST::NodeType::CaseStatement:
					return render_casestatement(AST::cast_to<AST::CaseStatement>(node));
					
					case AST::NodeType::CasexStatement:
					return render_casexstatement(AST::cast_to<AST::CasexStatement>(node));
					
					case AST::NodeType::CasezStatement:
					return render_casezstatement(AST::cast_to<AST::CasezStatement>(node));
					
					case AST::NodeType::Case:
					return render_case(AST::cast_to<AST::Case>(node));
					
					case AST::NodeType::Block:
					return render_block(AST::cast_to<AST::Block>(node));
					
					case AST::NodeType::Initial:
					return render_initial(AST::cast_to<AST::Initial>(node));
					
					case AST::NodeType::EventStatement:
					return render_eventstatement(AST::cast_to<AST::EventStatement>(node));
					
					case AST::NodeType::WaitStatement:
					return render_waitstatement(AST::cast_to<AST::WaitStatement>(node));
					
					case AST::NodeType::ForeverStatement:
					return render_foreverstatement(AST::cast_to<AST::ForeverStatement>(node));
					
					case AST::NodeType::DelayStatement:
					return render_delaystatement(AST::cast_to<AST::DelayStatement>(node));
					
					case AST::NodeType::Instancelist:
					return render_instancelist(AST::cast_to<AST::Instancelist>(node));
					
					case AST::NodeType::Instance:
					return render_instance(AST::cast_to<AST::Instance>(node));
					
					case AST::NodeType::ParamArg:
					return render_paramarg(AST::cast_to<AST::ParamArg>(node));
					
					case AST::NodeType::PortArg:
					return render_portarg(AST::cast_to<AST::PortArg>(node));
					
					case AST::NodeType::Function:
					return render_function(AST::cast_to<AST::Function>(node));
					
					case AST::NodeType::FunctionCall:
					return render_functioncall(AST::cast_to<AST::FunctionCall>(node));
					
					case AST::NodeType::Task:
					return render_task(AST::cast_to<AST::Task>(node));
					
					case AST::NodeType::TaskCall:
					return render_taskcall(AST::cast_to<AST::TaskCall>(node));
					
					case AST::NodeType::GenerateStatement:
					return render_generatestatement(AST::cast_to<AST::GenerateStatement>(node));
					
					case AST::NodeType::SystemCall:
					return render_systemcall(AST::cast_to<AST::SystemCall>(node));
					
					case AST::NodeType::IdentifierScopeLabel:
					return render_identifierscopelabel(AST::cast_to<AST::IdentifierScopeLabel>(node));
					
					case AST::NodeType::IdentifierScope:
					return render_identifierscope(AST::cast_to<AST::IdentifierScope>(node));
					
					case AST::NodeType::Disable:
					return render_disable(AST::cast_to<AST::Disable>(node));
					
					case AST::NodeType::ParallelBlock:
					return render_parallelblock(AST::cast_to<AST::ParallelBlock>(node));
					
					case AST::NodeType::SingleStatement:
					return render_singlestatement(AST::cast_to<AST::SingleStatement>(node));
					
					default: return render_node(node);
					}
				}
				else {
					return T();
				}
			}
			
			/**
			 * Main rendering method for the Source node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_source(const AST::Source::Ptr &node)
			 */
			T render(const AST::Source::Ptr node) const {
				return render_source(node);
			}
			
			/**
			 * Main rendering method for the Description node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_description(const AST::Description::Ptr &node)
			 */
			T render(const AST::Description::Ptr node) const {
				return render_description(node);
			}
			
			/**
			 * Main rendering method for the Pragma node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_pragma(const AST::Pragma::Ptr &node)
			 */
			T render(const AST::Pragma::Ptr node) const {
				return render_pragma(node);
			}
			
			/**
			 * Main rendering method for the Module node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_module(const AST::Module::Ptr &node)
			 */
			T render(const AST::Module::Ptr node) const {
				return render_module(node);
			}
			
			/**
			 * Main rendering method for the Port node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_port(const AST::Port::Ptr &node)
			 */
			T render(const AST::Port::Ptr node) const {
				return render_port(node);
			}
			
			/**
			 * Main rendering method for the Width node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_width(const AST::Width::Ptr &node)
			 */
			T render(const AST::Width::Ptr node) const {
				return render_width(node);
			}
			
			/**
			 * Main rendering method for the Length node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_length(const AST::Length::Ptr &node)
			 */
			T render(const AST::Length::Ptr node) const {
				return render_length(node);
			}
			
			/**
			 * Main rendering method for the Identifier node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_identifier(const AST::Identifier::Ptr &node)
			 */
			T render(const AST::Identifier::Ptr node) const {
				return render_identifier(node);
			}
			
			/**
			 * Main rendering method for the Constant node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_constant(const AST::Constant::Ptr &node)
			 */
			T render(const AST::Constant::Ptr node) const {
				return render_constant(node);
			}
			
			/**
			 * Main rendering method for the StringConst node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_stringconst(const AST::StringConst::Ptr &node)
			 */
			T render(const AST::StringConst::Ptr node) const {
				return render_stringconst(node);
			}
			
			/**
			 * Main rendering method for the IntConst node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_intconst(const AST::IntConst::Ptr &node)
			 */
			T render(const AST::IntConst::Ptr node) const {
				return render_intconst(node);
			}
			
			/**
			 * Main rendering method for the IntConstN node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_intconstn(const AST::IntConstN::Ptr &node)
			 */
			T render(const AST::IntConstN::Ptr node) const {
				return render_intconstn(node);
			}
			
			/**
			 * Main rendering method for the FloatConst node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_floatconst(const AST::FloatConst::Ptr &node)
			 */
			T render(const AST::FloatConst::Ptr node) const {
				return render_floatconst(node);
			}
			
			/**
			 * Main rendering method for the IODir node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_iodir(const AST::IODir::Ptr &node)
			 */
			T render(const AST::IODir::Ptr node) const {
				return render_iodir(node);
			}
			
			/**
			 * Main rendering method for the Input node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_input(const AST::Input::Ptr &node)
			 */
			T render(const AST::Input::Ptr node) const {
				return render_input(node);
			}
			
			/**
			 * Main rendering method for the Output node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_output(const AST::Output::Ptr &node)
			 */
			T render(const AST::Output::Ptr node) const {
				return render_output(node);
			}
			
			/**
			 * Main rendering method for the Inout node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_inout(const AST::Inout::Ptr &node)
			 */
			T render(const AST::Inout::Ptr node) const {
				return render_inout(node);
			}
			
			/**
			 * Main rendering method for the VariableBase node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_variablebase(const AST::VariableBase::Ptr &node)
			 */
			T render(const AST::VariableBase::Ptr node) const {
				return render_variablebase(node);
			}
			
			/**
			 * Main rendering method for the Genvar node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_genvar(const AST::Genvar::Ptr &node)
			 */
			T render(const AST::Genvar::Ptr node) const {
				return render_genvar(node);
			}
			
			/**
			 * Main rendering method for the Variable node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_variable(const AST::Variable::Ptr &node)
			 */
			T render(const AST::Variable::Ptr node) const {
				return render_variable(node);
			}
			
			/**
			 * Main rendering method for the Net node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_net(const AST::Net::Ptr &node)
			 */
			T render(const AST::Net::Ptr node) const {
				return render_net(node);
			}
			
			/**
			 * Main rendering method for the Integer node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_integer(const AST::Integer::Ptr &node)
			 */
			T render(const AST::Integer::Ptr node) const {
				return render_integer(node);
			}
			
			/**
			 * Main rendering method for the Real node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_real(const AST::Real::Ptr &node)
			 */
			T render(const AST::Real::Ptr node) const {
				return render_real(node);
			}
			
			/**
			 * Main rendering method for the Tri node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_tri(const AST::Tri::Ptr &node)
			 */
			T render(const AST::Tri::Ptr node) const {
				return render_tri(node);
			}
			
			/**
			 * Main rendering method for the Wire node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_wire(const AST::Wire::Ptr &node)
			 */
			T render(const AST::Wire::Ptr node) const {
				return render_wire(node);
			}
			
			/**
			 * Main rendering method for the Supply0 node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_supply0(const AST::Supply0::Ptr &node)
			 */
			T render(const AST::Supply0::Ptr node) const {
				return render_supply0(node);
			}
			
			/**
			 * Main rendering method for the Supply1 node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_supply1(const AST::Supply1::Ptr &node)
			 */
			T render(const AST::Supply1::Ptr node) const {
				return render_supply1(node);
			}
			
			/**
			 * Main rendering method for the Reg node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_reg(const AST::Reg::Ptr &node)
			 */
			T render(const AST::Reg::Ptr node) const {
				return render_reg(node);
			}
			
			/**
			 * Main rendering method for the Ioport node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_ioport(const AST::Ioport::Ptr &node)
			 */
			T render(const AST::Ioport::Ptr node) const {
				return render_ioport(node);
			}
			
			/**
			 * Main rendering method for the Parameter node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_parameter(const AST::Parameter::Ptr &node)
			 */
			T render(const AST::Parameter::Ptr node) const {
				return render_parameter(node);
			}
			
			/**
			 * Main rendering method for the Localparam node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_localparam(const AST::Localparam::Ptr &node)
			 */
			T render(const AST::Localparam::Ptr node) const {
				return render_localparam(node);
			}
			
			/**
			 * Main rendering method for the Concat node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_concat(const AST::Concat::Ptr &node)
			 */
			T render(const AST::Concat::Ptr node) const {
				return render_concat(node);
			}
			
			/**
			 * Main rendering method for the Lconcat node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_lconcat(const AST::Lconcat::Ptr &node)
			 */
			T render(const AST::Lconcat::Ptr node) const {
				return render_lconcat(node);
			}
			
			/**
			 * Main rendering method for the Repeat node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_repeat(const AST::Repeat::Ptr &node)
			 */
			T render(const AST::Repeat::Ptr node) const {
				return render_repeat(node);
			}
			
			/**
			 * Main rendering method for the Indirect node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_indirect(const AST::Indirect::Ptr &node)
			 */
			T render(const AST::Indirect::Ptr node) const {
				return render_indirect(node);
			}
			
			/**
			 * Main rendering method for the Partselect node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_partselect(const AST::Partselect::Ptr &node)
			 */
			T render(const AST::Partselect::Ptr node) const {
				return render_partselect(node);
			}
			
			/**
			 * Main rendering method for the PartselectIndexed node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_partselectindexed(const AST::PartselectIndexed::Ptr &node)
			 */
			T render(const AST::PartselectIndexed::Ptr node) const {
				return render_partselectindexed(node);
			}
			
			/**
			 * Main rendering method for the PartselectPlusIndexed node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_partselectplusindexed(const AST::PartselectPlusIndexed::Ptr &node)
			 */
			T render(const AST::PartselectPlusIndexed::Ptr node) const {
				return render_partselectplusindexed(node);
			}
			
			/**
			 * Main rendering method for the PartselectMinusIndexed node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_partselectminusindexed(const AST::PartselectMinusIndexed::Ptr &node)
			 */
			T render(const AST::PartselectMinusIndexed::Ptr node) const {
				return render_partselectminusindexed(node);
			}
			
			/**
			 * Main rendering method for the Pointer node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_pointer(const AST::Pointer::Ptr &node)
			 */
			T render(const AST::Pointer::Ptr node) const {
				return render_pointer(node);
			}
			
			/**
			 * Main rendering method for the Lvalue node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_lvalue(const AST::Lvalue::Ptr &node)
			 */
			T render(const AST::Lvalue::Ptr node) const {
				return render_lvalue(node);
			}
			
			/**
			 * Main rendering method for the Rvalue node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_rvalue(const AST::Rvalue::Ptr &node)
			 */
			T render(const AST::Rvalue::Ptr node) const {
				return render_rvalue(node);
			}
			
			/**
			 * Main rendering method for the UnaryOperator node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_unaryoperator(const AST::UnaryOperator::Ptr &node)
			 */
			T render(const AST::UnaryOperator::Ptr node) const {
				return render_unaryoperator(node);
			}
			
			/**
			 * Main rendering method for the Uplus node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_uplus(const AST::Uplus::Ptr &node)
			 */
			T render(const AST::Uplus::Ptr node) const {
				return render_uplus(node);
			}
			
			/**
			 * Main rendering method for the Uminus node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_uminus(const AST::Uminus::Ptr &node)
			 */
			T render(const AST::Uminus::Ptr node) const {
				return render_uminus(node);
			}
			
			/**
			 * Main rendering method for the Ulnot node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_ulnot(const AST::Ulnot::Ptr &node)
			 */
			T render(const AST::Ulnot::Ptr node) const {
				return render_ulnot(node);
			}
			
			/**
			 * Main rendering method for the Unot node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_unot(const AST::Unot::Ptr &node)
			 */
			T render(const AST::Unot::Ptr node) const {
				return render_unot(node);
			}
			
			/**
			 * Main rendering method for the Uand node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_uand(const AST::Uand::Ptr &node)
			 */
			T render(const AST::Uand::Ptr node) const {
				return render_uand(node);
			}
			
			/**
			 * Main rendering method for the Unand node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_unand(const AST::Unand::Ptr &node)
			 */
			T render(const AST::Unand::Ptr node) const {
				return render_unand(node);
			}
			
			/**
			 * Main rendering method for the Uor node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_uor(const AST::Uor::Ptr &node)
			 */
			T render(const AST::Uor::Ptr node) const {
				return render_uor(node);
			}
			
			/**
			 * Main rendering method for the Unor node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_unor(const AST::Unor::Ptr &node)
			 */
			T render(const AST::Unor::Ptr node) const {
				return render_unor(node);
			}
			
			/**
			 * Main rendering method for the Uxor node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_uxor(const AST::Uxor::Ptr &node)
			 */
			T render(const AST::Uxor::Ptr node) const {
				return render_uxor(node);
			}
			
			/**
			 * Main rendering method for the Uxnor node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_uxnor(const AST::Uxnor::Ptr &node)
			 */
			T render(const AST::Uxnor::Ptr node) const {
				return render_uxnor(node);
			}
			
			/**
			 * Main rendering method for the Operator node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_operator(const AST::Operator::Ptr &node)
			 */
			T render(const AST::Operator::Ptr node) const {
				return render_operator(node);
			}
			
			/**
			 * Main rendering method for the Power node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_power(const AST::Power::Ptr &node)
			 */
			T render(const AST::Power::Ptr node) const {
				return render_power(node);
			}
			
			/**
			 * Main rendering method for the Times node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_times(const AST::Times::Ptr &node)
			 */
			T render(const AST::Times::Ptr node) const {
				return render_times(node);
			}
			
			/**
			 * Main rendering method for the Divide node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_divide(const AST::Divide::Ptr &node)
			 */
			T render(const AST::Divide::Ptr node) const {
				return render_divide(node);
			}
			
			/**
			 * Main rendering method for the Mod node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_mod(const AST::Mod::Ptr &node)
			 */
			T render(const AST::Mod::Ptr node) const {
				return render_mod(node);
			}
			
			/**
			 * Main rendering method for the Plus node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_plus(const AST::Plus::Ptr &node)
			 */
			T render(const AST::Plus::Ptr node) const {
				return render_plus(node);
			}
			
			/**
			 * Main rendering method for the Minus node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_minus(const AST::Minus::Ptr &node)
			 */
			T render(const AST::Minus::Ptr node) const {
				return render_minus(node);
			}
			
			/**
			 * Main rendering method for the Sll node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_sll(const AST::Sll::Ptr &node)
			 */
			T render(const AST::Sll::Ptr node) const {
				return render_sll(node);
			}
			
			/**
			 * Main rendering method for the Srl node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_srl(const AST::Srl::Ptr &node)
			 */
			T render(const AST::Srl::Ptr node) const {
				return render_srl(node);
			}
			
			/**
			 * Main rendering method for the Sra node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_sra(const AST::Sra::Ptr &node)
			 */
			T render(const AST::Sra::Ptr node) const {
				return render_sra(node);
			}
			
			/**
			 * Main rendering method for the LessThan node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_lessthan(const AST::LessThan::Ptr &node)
			 */
			T render(const AST::LessThan::Ptr node) const {
				return render_lessthan(node);
			}
			
			/**
			 * Main rendering method for the GreaterThan node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_greaterthan(const AST::GreaterThan::Ptr &node)
			 */
			T render(const AST::GreaterThan::Ptr node) const {
				return render_greaterthan(node);
			}
			
			/**
			 * Main rendering method for the LessEq node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_lesseq(const AST::LessEq::Ptr &node)
			 */
			T render(const AST::LessEq::Ptr node) const {
				return render_lesseq(node);
			}
			
			/**
			 * Main rendering method for the GreaterEq node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_greatereq(const AST::GreaterEq::Ptr &node)
			 */
			T render(const AST::GreaterEq::Ptr node) const {
				return render_greatereq(node);
			}
			
			/**
			 * Main rendering method for the Eq node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_eq(const AST::Eq::Ptr &node)
			 */
			T render(const AST::Eq::Ptr node) const {
				return render_eq(node);
			}
			
			/**
			 * Main rendering method for the NotEq node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_noteq(const AST::NotEq::Ptr &node)
			 */
			T render(const AST::NotEq::Ptr node) const {
				return render_noteq(node);
			}
			
			/**
			 * Main rendering method for the Eql node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_eql(const AST::Eql::Ptr &node)
			 */
			T render(const AST::Eql::Ptr node) const {
				return render_eql(node);
			}
			
			/**
			 * Main rendering method for the NotEql node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_noteql(const AST::NotEql::Ptr &node)
			 */
			T render(const AST::NotEql::Ptr node) const {
				return render_noteql(node);
			}
			
			/**
			 * Main rendering method for the And node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_and(const AST::And::Ptr &node)
			 */
			T render(const AST::And::Ptr node) const {
				return render_and(node);
			}
			
			/**
			 * Main rendering method for the Xor node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_xor(const AST::Xor::Ptr &node)
			 */
			T render(const AST::Xor::Ptr node) const {
				return render_xor(node);
			}
			
			/**
			 * Main rendering method for the Xnor node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_xnor(const AST::Xnor::Ptr &node)
			 */
			T render(const AST::Xnor::Ptr node) const {
				return render_xnor(node);
			}
			
			/**
			 * Main rendering method for the Or node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_or(const AST::Or::Ptr &node)
			 */
			T render(const AST::Or::Ptr node) const {
				return render_or(node);
			}
			
			/**
			 * Main rendering method for the Land node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_land(const AST::Land::Ptr &node)
			 */
			T render(const AST::Land::Ptr node) const {
				return render_land(node);
			}
			
			/**
			 * Main rendering method for the Lor node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_lor(const AST::Lor::Ptr &node)
			 */
			T render(const AST::Lor::Ptr node) const {
				return render_lor(node);
			}
			
			/**
			 * Main rendering method for the Cond node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_cond(const AST::Cond::Ptr &node)
			 */
			T render(const AST::Cond::Ptr node) const {
				return render_cond(node);
			}
			
			/**
			 * Main rendering method for the Always node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_always(const AST::Always::Ptr &node)
			 */
			T render(const AST::Always::Ptr node) const {
				return render_always(node);
			}
			
			/**
			 * Main rendering method for the Senslist node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_senslist(const AST::Senslist::Ptr &node)
			 */
			T render(const AST::Senslist::Ptr node) const {
				return render_senslist(node);
			}
			
			/**
			 * Main rendering method for the Sens node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_sens(const AST::Sens::Ptr &node)
			 */
			T render(const AST::Sens::Ptr node) const {
				return render_sens(node);
			}
			
			/**
			 * Main rendering method for the Defparamlist node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_defparamlist(const AST::Defparamlist::Ptr &node)
			 */
			T render(const AST::Defparamlist::Ptr node) const {
				return render_defparamlist(node);
			}
			
			/**
			 * Main rendering method for the Defparam node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_defparam(const AST::Defparam::Ptr &node)
			 */
			T render(const AST::Defparam::Ptr node) const {
				return render_defparam(node);
			}
			
			/**
			 * Main rendering method for the Assign node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_assign(const AST::Assign::Ptr &node)
			 */
			T render(const AST::Assign::Ptr node) const {
				return render_assign(node);
			}
			
			/**
			 * Main rendering method for the BlockingSubstitution node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_blockingsubstitution(const AST::BlockingSubstitution::Ptr &node)
			 */
			T render(const AST::BlockingSubstitution::Ptr node) const {
				return render_blockingsubstitution(node);
			}
			
			/**
			 * Main rendering method for the NonblockingSubstitution node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_nonblockingsubstitution(const AST::NonblockingSubstitution::Ptr &node)
			 */
			T render(const AST::NonblockingSubstitution::Ptr node) const {
				return render_nonblockingsubstitution(node);
			}
			
			/**
			 * Main rendering method for the IfStatement node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_ifstatement(const AST::IfStatement::Ptr &node)
			 */
			T render(const AST::IfStatement::Ptr node) const {
				return render_ifstatement(node);
			}
			
			/**
			 * Main rendering method for the RepeatStatement node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_repeatstatement(const AST::RepeatStatement::Ptr &node)
			 */
			T render(const AST::RepeatStatement::Ptr node) const {
				return render_repeatstatement(node);
			}
			
			/**
			 * Main rendering method for the ForStatement node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_forstatement(const AST::ForStatement::Ptr &node)
			 */
			T render(const AST::ForStatement::Ptr node) const {
				return render_forstatement(node);
			}
			
			/**
			 * Main rendering method for the WhileStatement node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_whilestatement(const AST::WhileStatement::Ptr &node)
			 */
			T render(const AST::WhileStatement::Ptr node) const {
				return render_whilestatement(node);
			}
			
			/**
			 * Main rendering method for the CaseStatement node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_casestatement(const AST::CaseStatement::Ptr &node)
			 */
			T render(const AST::CaseStatement::Ptr node) const {
				return render_casestatement(node);
			}
			
			/**
			 * Main rendering method for the CasexStatement node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_casexstatement(const AST::CasexStatement::Ptr &node)
			 */
			T render(const AST::CasexStatement::Ptr node) const {
				return render_casexstatement(node);
			}
			
			/**
			 * Main rendering method for the CasezStatement node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_casezstatement(const AST::CasezStatement::Ptr &node)
			 */
			T render(const AST::CasezStatement::Ptr node) const {
				return render_casezstatement(node);
			}
			
			/**
			 * Main rendering method for the Case node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_case(const AST::Case::Ptr &node)
			 */
			T render(const AST::Case::Ptr node) const {
				return render_case(node);
			}
			
			/**
			 * Main rendering method for the Block node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_block(const AST::Block::Ptr &node)
			 */
			T render(const AST::Block::Ptr node) const {
				return render_block(node);
			}
			
			/**
			 * Main rendering method for the Initial node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_initial(const AST::Initial::Ptr &node)
			 */
			T render(const AST::Initial::Ptr node) const {
				return render_initial(node);
			}
			
			/**
			 * Main rendering method for the EventStatement node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_eventstatement(const AST::EventStatement::Ptr &node)
			 */
			T render(const AST::EventStatement::Ptr node) const {
				return render_eventstatement(node);
			}
			
			/**
			 * Main rendering method for the WaitStatement node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_waitstatement(const AST::WaitStatement::Ptr &node)
			 */
			T render(const AST::WaitStatement::Ptr node) const {
				return render_waitstatement(node);
			}
			
			/**
			 * Main rendering method for the ForeverStatement node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_foreverstatement(const AST::ForeverStatement::Ptr &node)
			 */
			T render(const AST::ForeverStatement::Ptr node) const {
				return render_foreverstatement(node);
			}
			
			/**
			 * Main rendering method for the DelayStatement node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_delaystatement(const AST::DelayStatement::Ptr &node)
			 */
			T render(const AST::DelayStatement::Ptr node) const {
				return render_delaystatement(node);
			}
			
			/**
			 * Main rendering method for the Instancelist node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_instancelist(const AST::Instancelist::Ptr &node)
			 */
			T render(const AST::Instancelist::Ptr node) const {
				return render_instancelist(node);
			}
			
			/**
			 * Main rendering method for the Instance node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_instance(const AST::Instance::Ptr &node)
			 */
			T render(const AST::Instance::Ptr node) const {
				return render_instance(node);
			}
			
			/**
			 * Main rendering method for the ParamArg node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_paramarg(const AST::ParamArg::Ptr &node)
			 */
			T render(const AST::ParamArg::Ptr node) const {
				return render_paramarg(node);
			}
			
			/**
			 * Main rendering method for the PortArg node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_portarg(const AST::PortArg::Ptr &node)
			 */
			T render(const AST::PortArg::Ptr node) const {
				return render_portarg(node);
			}
			
			/**
			 * Main rendering method for the Function node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_function(const AST::Function::Ptr &node)
			 */
			T render(const AST::Function::Ptr node) const {
				return render_function(node);
			}
			
			/**
			 * Main rendering method for the FunctionCall node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_functioncall(const AST::FunctionCall::Ptr &node)
			 */
			T render(const AST::FunctionCall::Ptr node) const {
				return render_functioncall(node);
			}
			
			/**
			 * Main rendering method for the Task node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_task(const AST::Task::Ptr &node)
			 */
			T render(const AST::Task::Ptr node) const {
				return render_task(node);
			}
			
			/**
			 * Main rendering method for the TaskCall node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_taskcall(const AST::TaskCall::Ptr &node)
			 */
			T render(const AST::TaskCall::Ptr node) const {
				return render_taskcall(node);
			}
			
			/**
			 * Main rendering method for the GenerateStatement node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_generatestatement(const AST::GenerateStatement::Ptr &node)
			 */
			T render(const AST::GenerateStatement::Ptr node) const {
				return render_generatestatement(node);
			}
			
			/**
			 * Main rendering method for the SystemCall node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_systemcall(const AST::SystemCall::Ptr &node)
			 */
			T render(const AST::SystemCall::Ptr node) const {
				return render_systemcall(node);
			}
			
			/**
			 * Main rendering method for the IdentifierScopeLabel node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_identifierscopelabel(const AST::IdentifierScopeLabel::Ptr &node)
			 */
			T render(const AST::IdentifierScopeLabel::Ptr node) const {
				return render_identifierscopelabel(node);
			}
			
			/**
			 * Main rendering method for the IdentifierScope node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_identifierscope(const AST::IdentifierScope::Ptr &node)
			 */
			T render(const AST::IdentifierScope::Ptr node) const {
				return render_identifierscope(node);
			}
			
			/**
			 * Main rendering method for the Disable node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_disable(const AST::Disable::Ptr &node)
			 */
			T render(const AST::Disable::Ptr node) const {
				return render_disable(node);
			}
			
			/**
			 * Main rendering method for the ParallelBlock node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_parallelblock(const AST::ParallelBlock::Ptr &node)
			 */
			T render(const AST::ParallelBlock::Ptr node) const {
				return render_parallelblock(node);
			}
			
			/**
			 * Main rendering method for the SingleStatement node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_singlestatement(const AST::SingleStatement::Ptr &node)
			 */
			T render(const AST::SingleStatement::Ptr node) const {
				return render_singlestatement(node);
			}
			

		protected:
			/**
			 * Render the Node base class. This method must be overloaded
			 * in a derived class to your needs.
			 */
			virtual T render_node(const AST::Node::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Source node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_source(const AST::Source::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Description node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_description(const AST::Description::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Pragma node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_pragma(const AST::Pragma::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Module node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_module(const AST::Module::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Port node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_port(const AST::Port::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Width node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_width(const AST::Width::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Length node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_length(const AST::Length::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Identifier node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_identifier(const AST::Identifier::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Constant node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_constant(const AST::Constant::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the StringConst node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_stringconst(const AST::StringConst::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the IntConst node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_intconst(const AST::IntConst::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the IntConstN node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_intconstn(const AST::IntConstN::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the FloatConst node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_floatconst(const AST::FloatConst::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the IODir node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_iodir(const AST::IODir::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Input node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_input(const AST::Input::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Output node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_output(const AST::Output::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Inout node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_inout(const AST::Inout::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the VariableBase node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_variablebase(const AST::VariableBase::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Genvar node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_genvar(const AST::Genvar::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Variable node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_variable(const AST::Variable::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Net node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_net(const AST::Net::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Integer node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_integer(const AST::Integer::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Real node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_real(const AST::Real::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Tri node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_tri(const AST::Tri::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Wire node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_wire(const AST::Wire::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Supply0 node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_supply0(const AST::Supply0::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Supply1 node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_supply1(const AST::Supply1::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Reg node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_reg(const AST::Reg::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Ioport node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_ioport(const AST::Ioport::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Parameter node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_parameter(const AST::Parameter::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Localparam node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_localparam(const AST::Localparam::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Concat node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_concat(const AST::Concat::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Lconcat node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_lconcat(const AST::Lconcat::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Repeat node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_repeat(const AST::Repeat::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Indirect node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_indirect(const AST::Indirect::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Partselect node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_partselect(const AST::Partselect::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the PartselectIndexed node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_partselectindexed(const AST::PartselectIndexed::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the PartselectPlusIndexed node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_partselectplusindexed(const AST::PartselectPlusIndexed::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the PartselectMinusIndexed node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_partselectminusindexed(const AST::PartselectMinusIndexed::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Pointer node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_pointer(const AST::Pointer::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Lvalue node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_lvalue(const AST::Lvalue::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Rvalue node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_rvalue(const AST::Rvalue::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the UnaryOperator node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_unaryoperator(const AST::UnaryOperator::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Uplus node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_uplus(const AST::Uplus::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Uminus node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_uminus(const AST::Uminus::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Ulnot node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_ulnot(const AST::Ulnot::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Unot node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_unot(const AST::Unot::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Uand node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_uand(const AST::Uand::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Unand node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_unand(const AST::Unand::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Uor node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_uor(const AST::Uor::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Unor node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_unor(const AST::Unor::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Uxor node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_uxor(const AST::Uxor::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Uxnor node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_uxnor(const AST::Uxnor::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Operator node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_operator(const AST::Operator::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Power node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_power(const AST::Power::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Times node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_times(const AST::Times::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Divide node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_divide(const AST::Divide::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Mod node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_mod(const AST::Mod::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Plus node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_plus(const AST::Plus::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Minus node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_minus(const AST::Minus::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Sll node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_sll(const AST::Sll::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Srl node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_srl(const AST::Srl::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Sra node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_sra(const AST::Sra::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the LessThan node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_lessthan(const AST::LessThan::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the GreaterThan node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_greaterthan(const AST::GreaterThan::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the LessEq node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_lesseq(const AST::LessEq::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the GreaterEq node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_greatereq(const AST::GreaterEq::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Eq node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_eq(const AST::Eq::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the NotEq node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_noteq(const AST::NotEq::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Eql node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_eql(const AST::Eql::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the NotEql node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_noteql(const AST::NotEql::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the And node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_and(const AST::And::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Xor node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_xor(const AST::Xor::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Xnor node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_xnor(const AST::Xnor::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Or node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_or(const AST::Or::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Land node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_land(const AST::Land::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Lor node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_lor(const AST::Lor::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Cond node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_cond(const AST::Cond::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Always node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_always(const AST::Always::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Senslist node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_senslist(const AST::Senslist::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Sens node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_sens(const AST::Sens::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Defparamlist node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_defparamlist(const AST::Defparamlist::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Defparam node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_defparam(const AST::Defparam::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Assign node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_assign(const AST::Assign::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the BlockingSubstitution node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_blockingsubstitution(const AST::BlockingSubstitution::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the NonblockingSubstitution node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_nonblockingsubstitution(const AST::NonblockingSubstitution::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the IfStatement node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_ifstatement(const AST::IfStatement::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the RepeatStatement node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_repeatstatement(const AST::RepeatStatement::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the ForStatement node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_forstatement(const AST::ForStatement::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the WhileStatement node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_whilestatement(const AST::WhileStatement::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the CaseStatement node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_casestatement(const AST::CaseStatement::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the CasexStatement node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_casexstatement(const AST::CasexStatement::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the CasezStatement node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_casezstatement(const AST::CasezStatement::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Case node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_case(const AST::Case::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Block node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_block(const AST::Block::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Initial node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_initial(const AST::Initial::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the EventStatement node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_eventstatement(const AST::EventStatement::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the WaitStatement node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_waitstatement(const AST::WaitStatement::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the ForeverStatement node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_foreverstatement(const AST::ForeverStatement::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the DelayStatement node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_delaystatement(const AST::DelayStatement::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Instancelist node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_instancelist(const AST::Instancelist::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Instance node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_instance(const AST::Instance::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the ParamArg node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_paramarg(const AST::ParamArg::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the PortArg node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_portarg(const AST::PortArg::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Function node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_function(const AST::Function::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the FunctionCall node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_functioncall(const AST::FunctionCall::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Task node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_task(const AST::Task::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the TaskCall node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_taskcall(const AST::TaskCall::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the GenerateStatement node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_generatestatement(const AST::GenerateStatement::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the SystemCall node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_systemcall(const AST::SystemCall::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the IdentifierScopeLabel node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_identifierscopelabel(const AST::IdentifierScopeLabel::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the IdentifierScope node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_identifierscope(const AST::IdentifierScope::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the Disable node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_disable(const AST::Disable::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the ParallelBlock node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_parallelblock(const AST::ParallelBlock::Ptr node) const {
				return T();
			}
			
			/**
			 * Render the SingleStatement node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_singlestatement(const AST::SingleStatement::Ptr node) const {
				return T();
			}
			


		private:
			std::string m_indent_str;

		};

	}
}

#endif