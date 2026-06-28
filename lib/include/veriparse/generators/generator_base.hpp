// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_GENERATORS_GENERATOR_BASE
#define VERIPARSE_GENERATORS_GENERATOR_BASE

#include <veriparse/AST/nodes.hpp>

namespace Veriparse
{
namespace Generators
{

template <typename T> class GeneratorBase
{
public:
    /**
     * Constructor, can use tabs or spaces to indent.
     */
    GeneratorBase(bool use_tabs = false, unsigned int indent_length = 2)
    {
        if(use_tabs) {
            m_indent_str.assign(1, '\t');
        } else {
            m_indent_str.assign(indent_length, ' ');
        }
    }

    virtual ~GeneratorBase(void) {}

    /**
     * Return the indent string built using constructor
     * parameters.
     */
    const std::string &get_indent_str(void) const { return m_indent_str; }

    /**
     * Main rendering method. When the node type is from the base
     * class, it dispatches the work to the right protected
     * rendering method depending on the internal node type.
     */
    T render(const AST::Node::Ptr node) const
    {
        if(node) {
            switch(node->get_node_type()) {
            case AST::NodeType::Source:
                return render_source(AST::cast_to<AST::Source>(node));

            case AST::NodeType::Description:
                return render_description(AST::cast_to<AST::Description>(node));

            case AST::NodeType::Pragmalist:
                return render_pragmalist(AST::cast_to<AST::Pragmalist>(node));

            case AST::NodeType::Pragma:
                return render_pragma(AST::cast_to<AST::Pragma>(node));

            case AST::NodeType::Module:
                return render_module(AST::cast_to<AST::Module>(node));

            case AST::NodeType::Port:
                return render_port(AST::cast_to<AST::Port>(node));

            case AST::NodeType::Package:
                return render_package(AST::cast_to<AST::Package>(node));

            case AST::NodeType::Import:
                return render_import(AST::cast_to<AST::Import>(node));

            case AST::NodeType::Export:
                return render_export(AST::cast_to<AST::Export>(node));

            case AST::NodeType::Identifier:
                return render_identifier(AST::cast_to<AST::Identifier>(node));

            case AST::NodeType::Call:
                return render_call(AST::cast_to<AST::Call>(node));

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

            case AST::NodeType::DataType:
                return render_datatype(AST::cast_to<AST::DataType>(node));

            case AST::NodeType::LogicType:
                return render_logictype(AST::cast_to<AST::LogicType>(node));

            case AST::NodeType::RegType:
                return render_regtype(AST::cast_to<AST::RegType>(node));

            case AST::NodeType::BitType:
                return render_bittype(AST::cast_to<AST::BitType>(node));

            case AST::NodeType::ByteType:
                return render_bytetype(AST::cast_to<AST::ByteType>(node));

            case AST::NodeType::ShortintType:
                return render_shortinttype(AST::cast_to<AST::ShortintType>(node));

            case AST::NodeType::IntType:
                return render_inttype(AST::cast_to<AST::IntType>(node));

            case AST::NodeType::LongintType:
                return render_longinttype(AST::cast_to<AST::LongintType>(node));

            case AST::NodeType::IntegerType:
                return render_integertype(AST::cast_to<AST::IntegerType>(node));

            case AST::NodeType::TimeType:
                return render_timetype(AST::cast_to<AST::TimeType>(node));

            case AST::NodeType::RealType:
                return render_realtype(AST::cast_to<AST::RealType>(node));

            case AST::NodeType::ShortrealType:
                return render_shortrealtype(AST::cast_to<AST::ShortrealType>(node));

            case AST::NodeType::RealtimeType:
                return render_realtimetype(AST::cast_to<AST::RealtimeType>(node));

            case AST::NodeType::StringType:
                return render_stringtype(AST::cast_to<AST::StringType>(node));

            case AST::NodeType::ChandleType:
                return render_chandletype(AST::cast_to<AST::ChandleType>(node));

            case AST::NodeType::EventType:
                return render_eventtype(AST::cast_to<AST::EventType>(node));

            case AST::NodeType::ImplicitType:
                return render_implicittype(AST::cast_to<AST::ImplicitType>(node));

            case AST::NodeType::VoidType:
                return render_voidtype(AST::cast_to<AST::VoidType>(node));

            case AST::NodeType::NamedType:
                return render_namedtype(AST::cast_to<AST::NamedType>(node));

            case AST::NodeType::ScopeName:
                return render_scopename(AST::cast_to<AST::ScopeName>(node));

            case AST::NodeType::StructType:
                return render_structtype(AST::cast_to<AST::StructType>(node));

            case AST::NodeType::UnionType:
                return render_uniontype(AST::cast_to<AST::UnionType>(node));

            case AST::NodeType::EnumType:
                return render_enumtype(AST::cast_to<AST::EnumType>(node));

            case AST::NodeType::EnumItem:
                return render_enumitem(AST::cast_to<AST::EnumItem>(node));

            case AST::NodeType::TypeOpExpr:
                return render_typeopexpr(AST::cast_to<AST::TypeOpExpr>(node));

            case AST::NodeType::TypeOpType:
                return render_typeoptype(AST::cast_to<AST::TypeOpType>(node));

            case AST::NodeType::Dimension:
                return render_dimension(AST::cast_to<AST::Dimension>(node));

            case AST::NodeType::RangeDim:
                return render_rangedim(AST::cast_to<AST::RangeDim>(node));

            case AST::NodeType::SizeDim:
                return render_sizedim(AST::cast_to<AST::SizeDim>(node));

            case AST::NodeType::UnsizedDim:
                return render_unsizeddim(AST::cast_to<AST::UnsizedDim>(node));

            case AST::NodeType::QueueDim:
                return render_queuedim(AST::cast_to<AST::QueueDim>(node));

            case AST::NodeType::AssocDim:
                return render_assocdim(AST::cast_to<AST::AssocDim>(node));

            case AST::NodeType::Declaration:
                return render_declaration(AST::cast_to<AST::Declaration>(node));

            case AST::NodeType::Var:
                return render_var(AST::cast_to<AST::Var>(node));

            case AST::NodeType::Net:
                return render_net(AST::cast_to<AST::Net>(node));

            case AST::NodeType::WireNet:
                return render_wirenet(AST::cast_to<AST::WireNet>(node));

            case AST::NodeType::TriNet:
                return render_trinet(AST::cast_to<AST::TriNet>(node));

            case AST::NodeType::Tri0Net:
                return render_tri0net(AST::cast_to<AST::Tri0Net>(node));

            case AST::NodeType::Tri1Net:
                return render_tri1net(AST::cast_to<AST::Tri1Net>(node));

            case AST::NodeType::TriandNet:
                return render_triandnet(AST::cast_to<AST::TriandNet>(node));

            case AST::NodeType::TriorNet:
                return render_triornet(AST::cast_to<AST::TriorNet>(node));

            case AST::NodeType::TriregNet:
                return render_triregnet(AST::cast_to<AST::TriregNet>(node));

            case AST::NodeType::WandNet:
                return render_wandnet(AST::cast_to<AST::WandNet>(node));

            case AST::NodeType::WorNet:
                return render_wornet(AST::cast_to<AST::WorNet>(node));

            case AST::NodeType::UwireNet:
                return render_uwirenet(AST::cast_to<AST::UwireNet>(node));

            case AST::NodeType::Supply0Net:
                return render_supply0net(AST::cast_to<AST::Supply0Net>(node));

            case AST::NodeType::Supply1Net:
                return render_supply1net(AST::cast_to<AST::Supply1Net>(node));

            case AST::NodeType::InterconnectNet:
                return render_interconnectnet(AST::cast_to<AST::InterconnectNet>(node));

            case AST::NodeType::UserNet:
                return render_usernet(AST::cast_to<AST::UserNet>(node));

            case AST::NodeType::ImplicitNet:
                return render_implicitnet(AST::cast_to<AST::ImplicitNet>(node));

            case AST::NodeType::NetTypeDecl:
                return render_nettypedecl(AST::cast_to<AST::NetTypeDecl>(node));

            case AST::NodeType::Strength:
                return render_strength(AST::cast_to<AST::Strength>(node));

            case AST::NodeType::DriveStrength:
                return render_drivestrength(AST::cast_to<AST::DriveStrength>(node));

            case AST::NodeType::ChargeStrength:
                return render_chargestrength(AST::cast_to<AST::ChargeStrength>(node));

            case AST::NodeType::Param:
                return render_param(AST::cast_to<AST::Param>(node));

            case AST::NodeType::TypeParam:
                return render_typeparam(AST::cast_to<AST::TypeParam>(node));

            case AST::NodeType::Typedef:
                return render_typedef(AST::cast_to<AST::Typedef>(node));

            case AST::NodeType::Member:
                return render_member(AST::cast_to<AST::Member>(node));

            case AST::NodeType::Arg:
                return render_arg(AST::cast_to<AST::Arg>(node));

            case AST::NodeType::Genvar:
                return render_genvar(AST::cast_to<AST::Genvar>(node));

            case AST::NodeType::Concat:
                return render_concat(AST::cast_to<AST::Concat>(node));

            case AST::NodeType::Lconcat:
                return render_lconcat(AST::cast_to<AST::Lconcat>(node));

            case AST::NodeType::Repeat:
                return render_repeat(AST::cast_to<AST::Repeat>(node));

            case AST::NodeType::AssignmentPattern:
                return render_assignmentpattern(AST::cast_to<AST::AssignmentPattern>(node));

            case AST::NodeType::PatternItem:
                return render_patternitem(AST::cast_to<AST::PatternItem>(node));

            case AST::NodeType::Cast:
                return render_cast(AST::cast_to<AST::Cast>(node));

            case AST::NodeType::TypeCast:
                return render_typecast(AST::cast_to<AST::TypeCast>(node));

            case AST::NodeType::SizeCast:
                return render_sizecast(AST::cast_to<AST::SizeCast>(node));

            case AST::NodeType::SigningCast:
                return render_signingcast(AST::cast_to<AST::SigningCast>(node));

            case AST::NodeType::ConstCast:
                return render_constcast(AST::cast_to<AST::ConstCast>(node));

            case AST::NodeType::Indirect:
                return render_indirect(AST::cast_to<AST::Indirect>(node));

            case AST::NodeType::Partselect:
                return render_partselect(AST::cast_to<AST::Partselect>(node));

            case AST::NodeType::PartselectIndexed:
                return render_partselectindexed(AST::cast_to<AST::PartselectIndexed>(node));

            case AST::NodeType::PartselectPlusIndexed:
                return render_partselectplusindexed(AST::cast_to<AST::PartselectPlusIndexed>(node));

            case AST::NodeType::PartselectMinusIndexed:
                return render_partselectminusindexed(
                    AST::cast_to<AST::PartselectMinusIndexed>(node));

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

            case AST::NodeType::AlwaysFF:
                return render_alwaysff(AST::cast_to<AST::AlwaysFF>(node));

            case AST::NodeType::AlwaysComb:
                return render_alwayscomb(AST::cast_to<AST::AlwaysComb>(node));

            case AST::NodeType::AlwaysLatch:
                return render_alwayslatch(AST::cast_to<AST::AlwaysLatch>(node));

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
                return render_nonblockingsubstitution(
                    AST::cast_to<AST::NonblockingSubstitution>(node));

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

            case AST::NodeType::UniqueCaseStatement:
                return render_uniquecasestatement(AST::cast_to<AST::UniqueCaseStatement>(node));

            case AST::NodeType::PriorityCaseStatement:
                return render_prioritycasestatement(AST::cast_to<AST::PriorityCaseStatement>(node));

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

            case AST::NodeType::HierLabel:
                return render_hierlabel(AST::cast_to<AST::HierLabel>(node));

            case AST::NodeType::HierName:
                return render_hiername(AST::cast_to<AST::HierName>(node));

            case AST::NodeType::Disable:
                return render_disable(AST::cast_to<AST::Disable>(node));

            case AST::NodeType::ParallelBlock:
                return render_parallelblock(AST::cast_to<AST::ParallelBlock>(node));

            case AST::NodeType::SingleStatement:
                return render_singlestatement(AST::cast_to<AST::SingleStatement>(node));

            default:
                return render_node(node);
            }
        } else {
            return T();
        }
    }

    /**
     * Main rendering method for the Source node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_source(const AST::Source::Ptr &node)
     */
    T render(const AST::Source::Ptr node) const { return render_source(node); }

    /**
     * Main rendering method for the Description node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_description(const AST::Description::Ptr &node)
     */
    T render(const AST::Description::Ptr node) const { return render_description(node); }

    /**
     * Main rendering method for the Pragmalist node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_pragmalist(const AST::Pragmalist::Ptr &node)
     */
    T render(const AST::Pragmalist::Ptr node) const { return render_pragmalist(node); }

    /**
     * Main rendering method for the Pragma node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_pragma(const AST::Pragma::Ptr &node)
     */
    T render(const AST::Pragma::Ptr node) const { return render_pragma(node); }

    /**
     * Main rendering method for the Module node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_module(const AST::Module::Ptr &node)
     */
    T render(const AST::Module::Ptr node) const { return render_module(node); }

    /**
     * Main rendering method for the Port node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_port(const AST::Port::Ptr &node)
     */
    T render(const AST::Port::Ptr node) const { return render_port(node); }

    /**
     * Main rendering method for the Package node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_package(const AST::Package::Ptr &node)
     */
    T render(const AST::Package::Ptr node) const { return render_package(node); }

    /**
     * Main rendering method for the Import node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_import(const AST::Import::Ptr &node)
     */
    T render(const AST::Import::Ptr node) const { return render_import(node); }

    /**
     * Main rendering method for the Export node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_export(const AST::Export::Ptr &node)
     */
    T render(const AST::Export::Ptr node) const { return render_export(node); }

    /**
     * Main rendering method for the Identifier node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_identifier(const AST::Identifier::Ptr &node)
     */
    T render(const AST::Identifier::Ptr node) const { return render_identifier(node); }

    /**
     * Main rendering method for the Call node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_call(const AST::Call::Ptr &node)
     */
    T render(const AST::Call::Ptr node) const { return render_call(node); }

    /**
     * Main rendering method for the Constant node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_constant(const AST::Constant::Ptr &node)
     */
    T render(const AST::Constant::Ptr node) const { return render_constant(node); }

    /**
     * Main rendering method for the StringConst node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_stringconst(const AST::StringConst::Ptr &node)
     */
    T render(const AST::StringConst::Ptr node) const { return render_stringconst(node); }

    /**
     * Main rendering method for the IntConst node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_intconst(const AST::IntConst::Ptr &node)
     */
    T render(const AST::IntConst::Ptr node) const { return render_intconst(node); }

    /**
     * Main rendering method for the IntConstN node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_intconstn(const AST::IntConstN::Ptr &node)
     */
    T render(const AST::IntConstN::Ptr node) const { return render_intconstn(node); }

    /**
     * Main rendering method for the FloatConst node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_floatconst(const AST::FloatConst::Ptr &node)
     */
    T render(const AST::FloatConst::Ptr node) const { return render_floatconst(node); }

    /**
     * Main rendering method for the DataType node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_datatype(const AST::DataType::Ptr &node)
     */
    T render(const AST::DataType::Ptr node) const { return render_datatype(node); }

    /**
     * Main rendering method for the LogicType node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_logictype(const AST::LogicType::Ptr &node)
     */
    T render(const AST::LogicType::Ptr node) const { return render_logictype(node); }

    /**
     * Main rendering method for the RegType node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_regtype(const AST::RegType::Ptr &node)
     */
    T render(const AST::RegType::Ptr node) const { return render_regtype(node); }

    /**
     * Main rendering method for the BitType node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_bittype(const AST::BitType::Ptr &node)
     */
    T render(const AST::BitType::Ptr node) const { return render_bittype(node); }

    /**
     * Main rendering method for the ByteType node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_bytetype(const AST::ByteType::Ptr &node)
     */
    T render(const AST::ByteType::Ptr node) const { return render_bytetype(node); }

    /**
     * Main rendering method for the ShortintType node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_shortinttype(const AST::ShortintType::Ptr &node)
     */
    T render(const AST::ShortintType::Ptr node) const { return render_shortinttype(node); }

    /**
     * Main rendering method for the IntType node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_inttype(const AST::IntType::Ptr &node)
     */
    T render(const AST::IntType::Ptr node) const { return render_inttype(node); }

    /**
     * Main rendering method for the LongintType node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_longinttype(const AST::LongintType::Ptr &node)
     */
    T render(const AST::LongintType::Ptr node) const { return render_longinttype(node); }

    /**
     * Main rendering method for the IntegerType node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_integertype(const AST::IntegerType::Ptr &node)
     */
    T render(const AST::IntegerType::Ptr node) const { return render_integertype(node); }

    /**
     * Main rendering method for the TimeType node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_timetype(const AST::TimeType::Ptr &node)
     */
    T render(const AST::TimeType::Ptr node) const { return render_timetype(node); }

    /**
     * Main rendering method for the RealType node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_realtype(const AST::RealType::Ptr &node)
     */
    T render(const AST::RealType::Ptr node) const { return render_realtype(node); }

    /**
     * Main rendering method for the ShortrealType node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_shortrealtype(const AST::ShortrealType::Ptr &node)
     */
    T render(const AST::ShortrealType::Ptr node) const { return render_shortrealtype(node); }

    /**
     * Main rendering method for the RealtimeType node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_realtimetype(const AST::RealtimeType::Ptr &node)
     */
    T render(const AST::RealtimeType::Ptr node) const { return render_realtimetype(node); }

    /**
     * Main rendering method for the StringType node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_stringtype(const AST::StringType::Ptr &node)
     */
    T render(const AST::StringType::Ptr node) const { return render_stringtype(node); }

    /**
     * Main rendering method for the ChandleType node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_chandletype(const AST::ChandleType::Ptr &node)
     */
    T render(const AST::ChandleType::Ptr node) const { return render_chandletype(node); }

    /**
     * Main rendering method for the EventType node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_eventtype(const AST::EventType::Ptr &node)
     */
    T render(const AST::EventType::Ptr node) const { return render_eventtype(node); }

    /**
     * Main rendering method for the ImplicitType node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_implicittype(const AST::ImplicitType::Ptr &node)
     */
    T render(const AST::ImplicitType::Ptr node) const { return render_implicittype(node); }

    /**
     * Main rendering method for the VoidType node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_voidtype(const AST::VoidType::Ptr &node)
     */
    T render(const AST::VoidType::Ptr node) const { return render_voidtype(node); }

    /**
     * Main rendering method for the NamedType node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_namedtype(const AST::NamedType::Ptr &node)
     */
    T render(const AST::NamedType::Ptr node) const { return render_namedtype(node); }

    /**
     * Main rendering method for the ScopeName node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_scopename(const AST::ScopeName::Ptr &node)
     */
    T render(const AST::ScopeName::Ptr node) const { return render_scopename(node); }

    /**
     * Main rendering method for the StructType node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_structtype(const AST::StructType::Ptr &node)
     */
    T render(const AST::StructType::Ptr node) const { return render_structtype(node); }

    /**
     * Main rendering method for the UnionType node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_uniontype(const AST::UnionType::Ptr &node)
     */
    T render(const AST::UnionType::Ptr node) const { return render_uniontype(node); }

    /**
     * Main rendering method for the EnumType node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_enumtype(const AST::EnumType::Ptr &node)
     */
    T render(const AST::EnumType::Ptr node) const { return render_enumtype(node); }

    /**
     * Main rendering method for the EnumItem node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_enumitem(const AST::EnumItem::Ptr &node)
     */
    T render(const AST::EnumItem::Ptr node) const { return render_enumitem(node); }

    /**
     * Main rendering method for the TypeOpExpr node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_typeopexpr(const AST::TypeOpExpr::Ptr &node)
     */
    T render(const AST::TypeOpExpr::Ptr node) const { return render_typeopexpr(node); }

    /**
     * Main rendering method for the TypeOpType node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_typeoptype(const AST::TypeOpType::Ptr &node)
     */
    T render(const AST::TypeOpType::Ptr node) const { return render_typeoptype(node); }

    /**
     * Main rendering method for the Dimension node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_dimension(const AST::Dimension::Ptr &node)
     */
    T render(const AST::Dimension::Ptr node) const { return render_dimension(node); }

    /**
     * Main rendering method for the RangeDim node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_rangedim(const AST::RangeDim::Ptr &node)
     */
    T render(const AST::RangeDim::Ptr node) const { return render_rangedim(node); }

    /**
     * Main rendering method for the SizeDim node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_sizedim(const AST::SizeDim::Ptr &node)
     */
    T render(const AST::SizeDim::Ptr node) const { return render_sizedim(node); }

    /**
     * Main rendering method for the UnsizedDim node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_unsizeddim(const AST::UnsizedDim::Ptr &node)
     */
    T render(const AST::UnsizedDim::Ptr node) const { return render_unsizeddim(node); }

    /**
     * Main rendering method for the QueueDim node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_queuedim(const AST::QueueDim::Ptr &node)
     */
    T render(const AST::QueueDim::Ptr node) const { return render_queuedim(node); }

    /**
     * Main rendering method for the AssocDim node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_assocdim(const AST::AssocDim::Ptr &node)
     */
    T render(const AST::AssocDim::Ptr node) const { return render_assocdim(node); }

    /**
     * Main rendering method for the Declaration node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_declaration(const AST::Declaration::Ptr &node)
     */
    T render(const AST::Declaration::Ptr node) const { return render_declaration(node); }

    /**
     * Main rendering method for the Var node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_var(const AST::Var::Ptr &node)
     */
    T render(const AST::Var::Ptr node) const { return render_var(node); }

    /**
     * Main rendering method for the Net node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_net(const AST::Net::Ptr &node)
     */
    T render(const AST::Net::Ptr node) const { return render_net(node); }

    /**
     * Main rendering method for the WireNet node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_wirenet(const AST::WireNet::Ptr &node)
     */
    T render(const AST::WireNet::Ptr node) const { return render_wirenet(node); }

    /**
     * Main rendering method for the TriNet node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_trinet(const AST::TriNet::Ptr &node)
     */
    T render(const AST::TriNet::Ptr node) const { return render_trinet(node); }

    /**
     * Main rendering method for the Tri0Net node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_tri0net(const AST::Tri0Net::Ptr &node)
     */
    T render(const AST::Tri0Net::Ptr node) const { return render_tri0net(node); }

    /**
     * Main rendering method for the Tri1Net node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_tri1net(const AST::Tri1Net::Ptr &node)
     */
    T render(const AST::Tri1Net::Ptr node) const { return render_tri1net(node); }

    /**
     * Main rendering method for the TriandNet node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_triandnet(const AST::TriandNet::Ptr &node)
     */
    T render(const AST::TriandNet::Ptr node) const { return render_triandnet(node); }

    /**
     * Main rendering method for the TriorNet node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_triornet(const AST::TriorNet::Ptr &node)
     */
    T render(const AST::TriorNet::Ptr node) const { return render_triornet(node); }

    /**
     * Main rendering method for the TriregNet node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_triregnet(const AST::TriregNet::Ptr &node)
     */
    T render(const AST::TriregNet::Ptr node) const { return render_triregnet(node); }

    /**
     * Main rendering method for the WandNet node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_wandnet(const AST::WandNet::Ptr &node)
     */
    T render(const AST::WandNet::Ptr node) const { return render_wandnet(node); }

    /**
     * Main rendering method for the WorNet node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_wornet(const AST::WorNet::Ptr &node)
     */
    T render(const AST::WorNet::Ptr node) const { return render_wornet(node); }

    /**
     * Main rendering method for the UwireNet node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_uwirenet(const AST::UwireNet::Ptr &node)
     */
    T render(const AST::UwireNet::Ptr node) const { return render_uwirenet(node); }

    /**
     * Main rendering method for the Supply0Net node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_supply0net(const AST::Supply0Net::Ptr &node)
     */
    T render(const AST::Supply0Net::Ptr node) const { return render_supply0net(node); }

    /**
     * Main rendering method for the Supply1Net node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_supply1net(const AST::Supply1Net::Ptr &node)
     */
    T render(const AST::Supply1Net::Ptr node) const { return render_supply1net(node); }

    /**
     * Main rendering method for the InterconnectNet node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_interconnectnet(const AST::InterconnectNet::Ptr &node)
     */
    T render(const AST::InterconnectNet::Ptr node) const { return render_interconnectnet(node); }

    /**
     * Main rendering method for the UserNet node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_usernet(const AST::UserNet::Ptr &node)
     */
    T render(const AST::UserNet::Ptr node) const { return render_usernet(node); }

    /**
     * Main rendering method for the ImplicitNet node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_implicitnet(const AST::ImplicitNet::Ptr &node)
     */
    T render(const AST::ImplicitNet::Ptr node) const { return render_implicitnet(node); }

    /**
     * Main rendering method for the NetTypeDecl node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_nettypedecl(const AST::NetTypeDecl::Ptr &node)
     */
    T render(const AST::NetTypeDecl::Ptr node) const { return render_nettypedecl(node); }

    /**
     * Main rendering method for the Strength node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_strength(const AST::Strength::Ptr &node)
     */
    T render(const AST::Strength::Ptr node) const { return render_strength(node); }

    /**
     * Main rendering method for the DriveStrength node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_drivestrength(const AST::DriveStrength::Ptr &node)
     */
    T render(const AST::DriveStrength::Ptr node) const { return render_drivestrength(node); }

    /**
     * Main rendering method for the ChargeStrength node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_chargestrength(const AST::ChargeStrength::Ptr &node)
     */
    T render(const AST::ChargeStrength::Ptr node) const { return render_chargestrength(node); }

    /**
     * Main rendering method for the Param node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_param(const AST::Param::Ptr &node)
     */
    T render(const AST::Param::Ptr node) const { return render_param(node); }

    /**
     * Main rendering method for the TypeParam node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_typeparam(const AST::TypeParam::Ptr &node)
     */
    T render(const AST::TypeParam::Ptr node) const { return render_typeparam(node); }

    /**
     * Main rendering method for the Typedef node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_typedef(const AST::Typedef::Ptr &node)
     */
    T render(const AST::Typedef::Ptr node) const { return render_typedef(node); }

    /**
     * Main rendering method for the Member node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_member(const AST::Member::Ptr &node)
     */
    T render(const AST::Member::Ptr node) const { return render_member(node); }

    /**
     * Main rendering method for the Arg node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_arg(const AST::Arg::Ptr &node)
     */
    T render(const AST::Arg::Ptr node) const { return render_arg(node); }

    /**
     * Main rendering method for the Genvar node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_genvar(const AST::Genvar::Ptr &node)
     */
    T render(const AST::Genvar::Ptr node) const { return render_genvar(node); }

    /**
     * Main rendering method for the Concat node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_concat(const AST::Concat::Ptr &node)
     */
    T render(const AST::Concat::Ptr node) const { return render_concat(node); }

    /**
     * Main rendering method for the Lconcat node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_lconcat(const AST::Lconcat::Ptr &node)
     */
    T render(const AST::Lconcat::Ptr node) const { return render_lconcat(node); }

    /**
     * Main rendering method for the Repeat node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_repeat(const AST::Repeat::Ptr &node)
     */
    T render(const AST::Repeat::Ptr node) const { return render_repeat(node); }

    /**
     * Main rendering method for the AssignmentPattern node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_assignmentpattern(const AST::AssignmentPattern::Ptr &node)
     */
    T render(const AST::AssignmentPattern::Ptr node) const
    {
        return render_assignmentpattern(node);
    }

    /**
     * Main rendering method for the PatternItem node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_patternitem(const AST::PatternItem::Ptr &node)
     */
    T render(const AST::PatternItem::Ptr node) const { return render_patternitem(node); }

    /**
     * Main rendering method for the Cast node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_cast(const AST::Cast::Ptr &node)
     */
    T render(const AST::Cast::Ptr node) const { return render_cast(node); }

    /**
     * Main rendering method for the TypeCast node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_typecast(const AST::TypeCast::Ptr &node)
     */
    T render(const AST::TypeCast::Ptr node) const { return render_typecast(node); }

    /**
     * Main rendering method for the SizeCast node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_sizecast(const AST::SizeCast::Ptr &node)
     */
    T render(const AST::SizeCast::Ptr node) const { return render_sizecast(node); }

    /**
     * Main rendering method for the SigningCast node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_signingcast(const AST::SigningCast::Ptr &node)
     */
    T render(const AST::SigningCast::Ptr node) const { return render_signingcast(node); }

    /**
     * Main rendering method for the ConstCast node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_constcast(const AST::ConstCast::Ptr &node)
     */
    T render(const AST::ConstCast::Ptr node) const { return render_constcast(node); }

    /**
     * Main rendering method for the Indirect node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_indirect(const AST::Indirect::Ptr &node)
     */
    T render(const AST::Indirect::Ptr node) const { return render_indirect(node); }

    /**
     * Main rendering method for the Partselect node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_partselect(const AST::Partselect::Ptr &node)
     */
    T render(const AST::Partselect::Ptr node) const { return render_partselect(node); }

    /**
     * Main rendering method for the PartselectIndexed node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_partselectindexed(const AST::PartselectIndexed::Ptr &node)
     */
    T render(const AST::PartselectIndexed::Ptr node) const
    {
        return render_partselectindexed(node);
    }

    /**
     * Main rendering method for the PartselectPlusIndexed node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_partselectplusindexed(const AST::PartselectPlusIndexed::Ptr &node)
     */
    T render(const AST::PartselectPlusIndexed::Ptr node) const
    {
        return render_partselectplusindexed(node);
    }

    /**
     * Main rendering method for the PartselectMinusIndexed node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_partselectminusindexed(const AST::PartselectMinusIndexed::Ptr
     * &node)
     */
    T render(const AST::PartselectMinusIndexed::Ptr node) const
    {
        return render_partselectminusindexed(node);
    }

    /**
     * Main rendering method for the Pointer node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_pointer(const AST::Pointer::Ptr &node)
     */
    T render(const AST::Pointer::Ptr node) const { return render_pointer(node); }

    /**
     * Main rendering method for the Lvalue node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_lvalue(const AST::Lvalue::Ptr &node)
     */
    T render(const AST::Lvalue::Ptr node) const { return render_lvalue(node); }

    /**
     * Main rendering method for the Rvalue node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_rvalue(const AST::Rvalue::Ptr &node)
     */
    T render(const AST::Rvalue::Ptr node) const { return render_rvalue(node); }

    /**
     * Main rendering method for the UnaryOperator node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_unaryoperator(const AST::UnaryOperator::Ptr &node)
     */
    T render(const AST::UnaryOperator::Ptr node) const { return render_unaryoperator(node); }

    /**
     * Main rendering method for the Uplus node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_uplus(const AST::Uplus::Ptr &node)
     */
    T render(const AST::Uplus::Ptr node) const { return render_uplus(node); }

    /**
     * Main rendering method for the Uminus node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_uminus(const AST::Uminus::Ptr &node)
     */
    T render(const AST::Uminus::Ptr node) const { return render_uminus(node); }

    /**
     * Main rendering method for the Ulnot node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_ulnot(const AST::Ulnot::Ptr &node)
     */
    T render(const AST::Ulnot::Ptr node) const { return render_ulnot(node); }

    /**
     * Main rendering method for the Unot node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_unot(const AST::Unot::Ptr &node)
     */
    T render(const AST::Unot::Ptr node) const { return render_unot(node); }

    /**
     * Main rendering method for the Uand node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_uand(const AST::Uand::Ptr &node)
     */
    T render(const AST::Uand::Ptr node) const { return render_uand(node); }

    /**
     * Main rendering method for the Unand node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_unand(const AST::Unand::Ptr &node)
     */
    T render(const AST::Unand::Ptr node) const { return render_unand(node); }

    /**
     * Main rendering method for the Uor node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_uor(const AST::Uor::Ptr &node)
     */
    T render(const AST::Uor::Ptr node) const { return render_uor(node); }

    /**
     * Main rendering method for the Unor node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_unor(const AST::Unor::Ptr &node)
     */
    T render(const AST::Unor::Ptr node) const { return render_unor(node); }

    /**
     * Main rendering method for the Uxor node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_uxor(const AST::Uxor::Ptr &node)
     */
    T render(const AST::Uxor::Ptr node) const { return render_uxor(node); }

    /**
     * Main rendering method for the Uxnor node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_uxnor(const AST::Uxnor::Ptr &node)
     */
    T render(const AST::Uxnor::Ptr node) const { return render_uxnor(node); }

    /**
     * Main rendering method for the Operator node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_operator(const AST::Operator::Ptr &node)
     */
    T render(const AST::Operator::Ptr node) const { return render_operator(node); }

    /**
     * Main rendering method for the Power node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_power(const AST::Power::Ptr &node)
     */
    T render(const AST::Power::Ptr node) const { return render_power(node); }

    /**
     * Main rendering method for the Times node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_times(const AST::Times::Ptr &node)
     */
    T render(const AST::Times::Ptr node) const { return render_times(node); }

    /**
     * Main rendering method for the Divide node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_divide(const AST::Divide::Ptr &node)
     */
    T render(const AST::Divide::Ptr node) const { return render_divide(node); }

    /**
     * Main rendering method for the Mod node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_mod(const AST::Mod::Ptr &node)
     */
    T render(const AST::Mod::Ptr node) const { return render_mod(node); }

    /**
     * Main rendering method for the Plus node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_plus(const AST::Plus::Ptr &node)
     */
    T render(const AST::Plus::Ptr node) const { return render_plus(node); }

    /**
     * Main rendering method for the Minus node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_minus(const AST::Minus::Ptr &node)
     */
    T render(const AST::Minus::Ptr node) const { return render_minus(node); }

    /**
     * Main rendering method for the Sll node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_sll(const AST::Sll::Ptr &node)
     */
    T render(const AST::Sll::Ptr node) const { return render_sll(node); }

    /**
     * Main rendering method for the Srl node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_srl(const AST::Srl::Ptr &node)
     */
    T render(const AST::Srl::Ptr node) const { return render_srl(node); }

    /**
     * Main rendering method for the Sra node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_sra(const AST::Sra::Ptr &node)
     */
    T render(const AST::Sra::Ptr node) const { return render_sra(node); }

    /**
     * Main rendering method for the LessThan node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_lessthan(const AST::LessThan::Ptr &node)
     */
    T render(const AST::LessThan::Ptr node) const { return render_lessthan(node); }

    /**
     * Main rendering method for the GreaterThan node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_greaterthan(const AST::GreaterThan::Ptr &node)
     */
    T render(const AST::GreaterThan::Ptr node) const { return render_greaterthan(node); }

    /**
     * Main rendering method for the LessEq node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_lesseq(const AST::LessEq::Ptr &node)
     */
    T render(const AST::LessEq::Ptr node) const { return render_lesseq(node); }

    /**
     * Main rendering method for the GreaterEq node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_greatereq(const AST::GreaterEq::Ptr &node)
     */
    T render(const AST::GreaterEq::Ptr node) const { return render_greatereq(node); }

    /**
     * Main rendering method for the Eq node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_eq(const AST::Eq::Ptr &node)
     */
    T render(const AST::Eq::Ptr node) const { return render_eq(node); }

    /**
     * Main rendering method for the NotEq node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_noteq(const AST::NotEq::Ptr &node)
     */
    T render(const AST::NotEq::Ptr node) const { return render_noteq(node); }

    /**
     * Main rendering method for the Eql node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_eql(const AST::Eql::Ptr &node)
     */
    T render(const AST::Eql::Ptr node) const { return render_eql(node); }

    /**
     * Main rendering method for the NotEql node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_noteql(const AST::NotEql::Ptr &node)
     */
    T render(const AST::NotEql::Ptr node) const { return render_noteql(node); }

    /**
     * Main rendering method for the And node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_and(const AST::And::Ptr &node)
     */
    T render(const AST::And::Ptr node) const { return render_and(node); }

    /**
     * Main rendering method for the Xor node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_xor(const AST::Xor::Ptr &node)
     */
    T render(const AST::Xor::Ptr node) const { return render_xor(node); }

    /**
     * Main rendering method for the Xnor node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_xnor(const AST::Xnor::Ptr &node)
     */
    T render(const AST::Xnor::Ptr node) const { return render_xnor(node); }

    /**
     * Main rendering method for the Or node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_or(const AST::Or::Ptr &node)
     */
    T render(const AST::Or::Ptr node) const { return render_or(node); }

    /**
     * Main rendering method for the Land node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_land(const AST::Land::Ptr &node)
     */
    T render(const AST::Land::Ptr node) const { return render_land(node); }

    /**
     * Main rendering method for the Lor node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_lor(const AST::Lor::Ptr &node)
     */
    T render(const AST::Lor::Ptr node) const { return render_lor(node); }

    /**
     * Main rendering method for the Cond node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_cond(const AST::Cond::Ptr &node)
     */
    T render(const AST::Cond::Ptr node) const { return render_cond(node); }

    /**
     * Main rendering method for the Always node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_always(const AST::Always::Ptr &node)
     */
    T render(const AST::Always::Ptr node) const { return render_always(node); }

    /**
     * Main rendering method for the AlwaysFF node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_alwaysff(const AST::AlwaysFF::Ptr &node)
     */
    T render(const AST::AlwaysFF::Ptr node) const { return render_alwaysff(node); }

    /**
     * Main rendering method for the AlwaysComb node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_alwayscomb(const AST::AlwaysComb::Ptr &node)
     */
    T render(const AST::AlwaysComb::Ptr node) const { return render_alwayscomb(node); }

    /**
     * Main rendering method for the AlwaysLatch node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_alwayslatch(const AST::AlwaysLatch::Ptr &node)
     */
    T render(const AST::AlwaysLatch::Ptr node) const { return render_alwayslatch(node); }

    /**
     * Main rendering method for the Senslist node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_senslist(const AST::Senslist::Ptr &node)
     */
    T render(const AST::Senslist::Ptr node) const { return render_senslist(node); }

    /**
     * Main rendering method for the Sens node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_sens(const AST::Sens::Ptr &node)
     */
    T render(const AST::Sens::Ptr node) const { return render_sens(node); }

    /**
     * Main rendering method for the Defparamlist node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_defparamlist(const AST::Defparamlist::Ptr &node)
     */
    T render(const AST::Defparamlist::Ptr node) const { return render_defparamlist(node); }

    /**
     * Main rendering method for the Defparam node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_defparam(const AST::Defparam::Ptr &node)
     */
    T render(const AST::Defparam::Ptr node) const { return render_defparam(node); }

    /**
     * Main rendering method for the Assign node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_assign(const AST::Assign::Ptr &node)
     */
    T render(const AST::Assign::Ptr node) const { return render_assign(node); }

    /**
     * Main rendering method for the BlockingSubstitution node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_blockingsubstitution(const AST::BlockingSubstitution::Ptr &node)
     */
    T render(const AST::BlockingSubstitution::Ptr node) const
    {
        return render_blockingsubstitution(node);
    }

    /**
     * Main rendering method for the NonblockingSubstitution node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_nonblockingsubstitution(const AST::NonblockingSubstitution::Ptr
     * &node)
     */
    T render(const AST::NonblockingSubstitution::Ptr node) const
    {
        return render_nonblockingsubstitution(node);
    }

    /**
     * Main rendering method for the IfStatement node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_ifstatement(const AST::IfStatement::Ptr &node)
     */
    T render(const AST::IfStatement::Ptr node) const { return render_ifstatement(node); }

    /**
     * Main rendering method for the RepeatStatement node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_repeatstatement(const AST::RepeatStatement::Ptr &node)
     */
    T render(const AST::RepeatStatement::Ptr node) const { return render_repeatstatement(node); }

    /**
     * Main rendering method for the ForStatement node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_forstatement(const AST::ForStatement::Ptr &node)
     */
    T render(const AST::ForStatement::Ptr node) const { return render_forstatement(node); }

    /**
     * Main rendering method for the WhileStatement node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_whilestatement(const AST::WhileStatement::Ptr &node)
     */
    T render(const AST::WhileStatement::Ptr node) const { return render_whilestatement(node); }

    /**
     * Main rendering method for the CaseStatement node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_casestatement(const AST::CaseStatement::Ptr &node)
     */
    T render(const AST::CaseStatement::Ptr node) const { return render_casestatement(node); }

    /**
     * Main rendering method for the CasexStatement node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_casexstatement(const AST::CasexStatement::Ptr &node)
     */
    T render(const AST::CasexStatement::Ptr node) const { return render_casexstatement(node); }

    /**
     * Main rendering method for the CasezStatement node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_casezstatement(const AST::CasezStatement::Ptr &node)
     */
    T render(const AST::CasezStatement::Ptr node) const { return render_casezstatement(node); }

    /**
     * Main rendering method for the UniqueCaseStatement node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_uniquecasestatement(const AST::UniqueCaseStatement::Ptr &node)
     */
    T render(const AST::UniqueCaseStatement::Ptr node) const
    {
        return render_uniquecasestatement(node);
    }

    /**
     * Main rendering method for the PriorityCaseStatement node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_prioritycasestatement(const AST::PriorityCaseStatement::Ptr &node)
     */
    T render(const AST::PriorityCaseStatement::Ptr node) const
    {
        return render_prioritycasestatement(node);
    }

    /**
     * Main rendering method for the Case node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_case(const AST::Case::Ptr &node)
     */
    T render(const AST::Case::Ptr node) const { return render_case(node); }

    /**
     * Main rendering method for the Block node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_block(const AST::Block::Ptr &node)
     */
    T render(const AST::Block::Ptr node) const { return render_block(node); }

    /**
     * Main rendering method for the Initial node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_initial(const AST::Initial::Ptr &node)
     */
    T render(const AST::Initial::Ptr node) const { return render_initial(node); }

    /**
     * Main rendering method for the EventStatement node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_eventstatement(const AST::EventStatement::Ptr &node)
     */
    T render(const AST::EventStatement::Ptr node) const { return render_eventstatement(node); }

    /**
     * Main rendering method for the WaitStatement node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_waitstatement(const AST::WaitStatement::Ptr &node)
     */
    T render(const AST::WaitStatement::Ptr node) const { return render_waitstatement(node); }

    /**
     * Main rendering method for the ForeverStatement node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_foreverstatement(const AST::ForeverStatement::Ptr &node)
     */
    T render(const AST::ForeverStatement::Ptr node) const { return render_foreverstatement(node); }

    /**
     * Main rendering method for the DelayStatement node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_delaystatement(const AST::DelayStatement::Ptr &node)
     */
    T render(const AST::DelayStatement::Ptr node) const { return render_delaystatement(node); }

    /**
     * Main rendering method for the Instancelist node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_instancelist(const AST::Instancelist::Ptr &node)
     */
    T render(const AST::Instancelist::Ptr node) const { return render_instancelist(node); }

    /**
     * Main rendering method for the Instance node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_instance(const AST::Instance::Ptr &node)
     */
    T render(const AST::Instance::Ptr node) const { return render_instance(node); }

    /**
     * Main rendering method for the ParamArg node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_paramarg(const AST::ParamArg::Ptr &node)
     */
    T render(const AST::ParamArg::Ptr node) const { return render_paramarg(node); }

    /**
     * Main rendering method for the PortArg node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_portarg(const AST::PortArg::Ptr &node)
     */
    T render(const AST::PortArg::Ptr node) const { return render_portarg(node); }

    /**
     * Main rendering method for the Function node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_function(const AST::Function::Ptr &node)
     */
    T render(const AST::Function::Ptr node) const { return render_function(node); }

    /**
     * Main rendering method for the FunctionCall node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_functioncall(const AST::FunctionCall::Ptr &node)
     */
    T render(const AST::FunctionCall::Ptr node) const { return render_functioncall(node); }

    /**
     * Main rendering method for the Task node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_task(const AST::Task::Ptr &node)
     */
    T render(const AST::Task::Ptr node) const { return render_task(node); }

    /**
     * Main rendering method for the TaskCall node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_taskcall(const AST::TaskCall::Ptr &node)
     */
    T render(const AST::TaskCall::Ptr node) const { return render_taskcall(node); }

    /**
     * Main rendering method for the GenerateStatement node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_generatestatement(const AST::GenerateStatement::Ptr &node)
     */
    T render(const AST::GenerateStatement::Ptr node) const
    {
        return render_generatestatement(node);
    }

    /**
     * Main rendering method for the SystemCall node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_systemcall(const AST::SystemCall::Ptr &node)
     */
    T render(const AST::SystemCall::Ptr node) const { return render_systemcall(node); }

    /**
     * Main rendering method for the HierLabel node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_hierlabel(const AST::HierLabel::Ptr &node)
     */
    T render(const AST::HierLabel::Ptr node) const { return render_hierlabel(node); }

    /**
     * Main rendering method for the HierName node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_hiername(const AST::HierName::Ptr &node)
     */
    T render(const AST::HierName::Ptr node) const { return render_hiername(node); }

    /**
     * Main rendering method for the Disable node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_disable(const AST::Disable::Ptr &node)
     */
    T render(const AST::Disable::Ptr node) const { return render_disable(node); }

    /**
     * Main rendering method for the ParallelBlock node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_parallelblock(const AST::ParallelBlock::Ptr &node)
     */
    T render(const AST::ParallelBlock::Ptr node) const { return render_parallelblock(node); }

    /**
     * Main rendering method for the SingleStatement node, we
     * dispatch directly to the right rendering method.
     *
     * @see GeneratorBase::render_singlestatement(const AST::SingleStatement::Ptr &node)
     */
    T render(const AST::SingleStatement::Ptr node) const { return render_singlestatement(node); }

protected:
    /**
     * Render the Node base class. This method must be overloaded
     * in a derived class to your needs.
     */
    virtual T render_node(const AST::Node::Ptr node) const { return T(); }

    /**
     * Render the Source node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_source(const AST::Source::Ptr node) const { return T(); }

    /**
     * Render the Description node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_description(const AST::Description::Ptr node) const { return T(); }

    /**
     * Render the Pragmalist node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_pragmalist(const AST::Pragmalist::Ptr node) const { return T(); }

    /**
     * Render the Pragma node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_pragma(const AST::Pragma::Ptr node) const { return T(); }

    /**
     * Render the Module node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_module(const AST::Module::Ptr node) const { return T(); }

    /**
     * Render the Port node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_port(const AST::Port::Ptr node) const { return T(); }

    /**
     * Render the Package node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_package(const AST::Package::Ptr node) const { return T(); }

    /**
     * Render the Import node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_import(const AST::Import::Ptr node) const { return T(); }

    /**
     * Render the Export node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_export(const AST::Export::Ptr node) const { return T(); }

    /**
     * Render the Identifier node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_identifier(const AST::Identifier::Ptr node) const { return T(); }

    /**
     * Render the Call node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_call(const AST::Call::Ptr node) const { return T(); }

    /**
     * Render the Constant node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_constant(const AST::Constant::Ptr node) const { return T(); }

    /**
     * Render the StringConst node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_stringconst(const AST::StringConst::Ptr node) const { return T(); }

    /**
     * Render the IntConst node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_intconst(const AST::IntConst::Ptr node) const { return T(); }

    /**
     * Render the IntConstN node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_intconstn(const AST::IntConstN::Ptr node) const { return T(); }

    /**
     * Render the FloatConst node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_floatconst(const AST::FloatConst::Ptr node) const { return T(); }

    /**
     * Render the DataType node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_datatype(const AST::DataType::Ptr node) const { return T(); }

    /**
     * Render the LogicType node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_logictype(const AST::LogicType::Ptr node) const { return T(); }

    /**
     * Render the RegType node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_regtype(const AST::RegType::Ptr node) const { return T(); }

    /**
     * Render the BitType node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_bittype(const AST::BitType::Ptr node) const { return T(); }

    /**
     * Render the ByteType node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_bytetype(const AST::ByteType::Ptr node) const { return T(); }

    /**
     * Render the ShortintType node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_shortinttype(const AST::ShortintType::Ptr node) const { return T(); }

    /**
     * Render the IntType node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_inttype(const AST::IntType::Ptr node) const { return T(); }

    /**
     * Render the LongintType node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_longinttype(const AST::LongintType::Ptr node) const { return T(); }

    /**
     * Render the IntegerType node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_integertype(const AST::IntegerType::Ptr node) const { return T(); }

    /**
     * Render the TimeType node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_timetype(const AST::TimeType::Ptr node) const { return T(); }

    /**
     * Render the RealType node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_realtype(const AST::RealType::Ptr node) const { return T(); }

    /**
     * Render the ShortrealType node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_shortrealtype(const AST::ShortrealType::Ptr node) const { return T(); }

    /**
     * Render the RealtimeType node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_realtimetype(const AST::RealtimeType::Ptr node) const { return T(); }

    /**
     * Render the StringType node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_stringtype(const AST::StringType::Ptr node) const { return T(); }

    /**
     * Render the ChandleType node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_chandletype(const AST::ChandleType::Ptr node) const { return T(); }

    /**
     * Render the EventType node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_eventtype(const AST::EventType::Ptr node) const { return T(); }

    /**
     * Render the ImplicitType node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_implicittype(const AST::ImplicitType::Ptr node) const { return T(); }

    /**
     * Render the VoidType node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_voidtype(const AST::VoidType::Ptr node) const { return T(); }

    /**
     * Render the NamedType node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_namedtype(const AST::NamedType::Ptr node) const { return T(); }

    /**
     * Render the ScopeName node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_scopename(const AST::ScopeName::Ptr node) const { return T(); }

    /**
     * Render the StructType node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_structtype(const AST::StructType::Ptr node) const { return T(); }

    /**
     * Render the UnionType node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_uniontype(const AST::UnionType::Ptr node) const { return T(); }

    /**
     * Render the EnumType node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_enumtype(const AST::EnumType::Ptr node) const { return T(); }

    /**
     * Render the EnumItem node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_enumitem(const AST::EnumItem::Ptr node) const { return T(); }

    /**
     * Render the TypeOpExpr node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_typeopexpr(const AST::TypeOpExpr::Ptr node) const { return T(); }

    /**
     * Render the TypeOpType node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_typeoptype(const AST::TypeOpType::Ptr node) const { return T(); }

    /**
     * Render the Dimension node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_dimension(const AST::Dimension::Ptr node) const { return T(); }

    /**
     * Render the RangeDim node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_rangedim(const AST::RangeDim::Ptr node) const { return T(); }

    /**
     * Render the SizeDim node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_sizedim(const AST::SizeDim::Ptr node) const { return T(); }

    /**
     * Render the UnsizedDim node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_unsizeddim(const AST::UnsizedDim::Ptr node) const { return T(); }

    /**
     * Render the QueueDim node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_queuedim(const AST::QueueDim::Ptr node) const { return T(); }

    /**
     * Render the AssocDim node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_assocdim(const AST::AssocDim::Ptr node) const { return T(); }

    /**
     * Render the Declaration node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_declaration(const AST::Declaration::Ptr node) const { return T(); }

    /**
     * Render the Var node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_var(const AST::Var::Ptr node) const { return T(); }

    /**
     * Render the Net node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_net(const AST::Net::Ptr node) const { return T(); }

    /**
     * Render the WireNet node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_wirenet(const AST::WireNet::Ptr node) const { return T(); }

    /**
     * Render the TriNet node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_trinet(const AST::TriNet::Ptr node) const { return T(); }

    /**
     * Render the Tri0Net node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_tri0net(const AST::Tri0Net::Ptr node) const { return T(); }

    /**
     * Render the Tri1Net node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_tri1net(const AST::Tri1Net::Ptr node) const { return T(); }

    /**
     * Render the TriandNet node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_triandnet(const AST::TriandNet::Ptr node) const { return T(); }

    /**
     * Render the TriorNet node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_triornet(const AST::TriorNet::Ptr node) const { return T(); }

    /**
     * Render the TriregNet node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_triregnet(const AST::TriregNet::Ptr node) const { return T(); }

    /**
     * Render the WandNet node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_wandnet(const AST::WandNet::Ptr node) const { return T(); }

    /**
     * Render the WorNet node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_wornet(const AST::WorNet::Ptr node) const { return T(); }

    /**
     * Render the UwireNet node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_uwirenet(const AST::UwireNet::Ptr node) const { return T(); }

    /**
     * Render the Supply0Net node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_supply0net(const AST::Supply0Net::Ptr node) const { return T(); }

    /**
     * Render the Supply1Net node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_supply1net(const AST::Supply1Net::Ptr node) const { return T(); }

    /**
     * Render the InterconnectNet node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_interconnectnet(const AST::InterconnectNet::Ptr node) const { return T(); }

    /**
     * Render the UserNet node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_usernet(const AST::UserNet::Ptr node) const { return T(); }

    /**
     * Render the ImplicitNet node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_implicitnet(const AST::ImplicitNet::Ptr node) const { return T(); }

    /**
     * Render the NetTypeDecl node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_nettypedecl(const AST::NetTypeDecl::Ptr node) const { return T(); }

    /**
     * Render the Strength node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_strength(const AST::Strength::Ptr node) const { return T(); }

    /**
     * Render the DriveStrength node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_drivestrength(const AST::DriveStrength::Ptr node) const { return T(); }

    /**
     * Render the ChargeStrength node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_chargestrength(const AST::ChargeStrength::Ptr node) const { return T(); }

    /**
     * Render the Param node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_param(const AST::Param::Ptr node) const { return T(); }

    /**
     * Render the TypeParam node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_typeparam(const AST::TypeParam::Ptr node) const { return T(); }

    /**
     * Render the Typedef node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_typedef(const AST::Typedef::Ptr node) const { return T(); }

    /**
     * Render the Member node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_member(const AST::Member::Ptr node) const { return T(); }

    /**
     * Render the Arg node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_arg(const AST::Arg::Ptr node) const { return T(); }

    /**
     * Render the Genvar node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_genvar(const AST::Genvar::Ptr node) const { return T(); }

    /**
     * Render the Concat node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_concat(const AST::Concat::Ptr node) const { return T(); }

    /**
     * Render the Lconcat node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_lconcat(const AST::Lconcat::Ptr node) const { return T(); }

    /**
     * Render the Repeat node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_repeat(const AST::Repeat::Ptr node) const { return T(); }

    /**
     * Render the AssignmentPattern node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_assignmentpattern(const AST::AssignmentPattern::Ptr node) const { return T(); }

    /**
     * Render the PatternItem node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_patternitem(const AST::PatternItem::Ptr node) const { return T(); }

    /**
     * Render the Cast node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_cast(const AST::Cast::Ptr node) const { return T(); }

    /**
     * Render the TypeCast node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_typecast(const AST::TypeCast::Ptr node) const { return T(); }

    /**
     * Render the SizeCast node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_sizecast(const AST::SizeCast::Ptr node) const { return T(); }

    /**
     * Render the SigningCast node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_signingcast(const AST::SigningCast::Ptr node) const { return T(); }

    /**
     * Render the ConstCast node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_constcast(const AST::ConstCast::Ptr node) const { return T(); }

    /**
     * Render the Indirect node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_indirect(const AST::Indirect::Ptr node) const { return T(); }

    /**
     * Render the Partselect node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_partselect(const AST::Partselect::Ptr node) const { return T(); }

    /**
     * Render the PartselectIndexed node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_partselectindexed(const AST::PartselectIndexed::Ptr node) const { return T(); }

    /**
     * Render the PartselectPlusIndexed node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_partselectplusindexed(const AST::PartselectPlusIndexed::Ptr node) const
    {
        return T();
    }

    /**
     * Render the PartselectMinusIndexed node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_partselectminusindexed(const AST::PartselectMinusIndexed::Ptr node) const
    {
        return T();
    }

    /**
     * Render the Pointer node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_pointer(const AST::Pointer::Ptr node) const { return T(); }

    /**
     * Render the Lvalue node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_lvalue(const AST::Lvalue::Ptr node) const { return T(); }

    /**
     * Render the Rvalue node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_rvalue(const AST::Rvalue::Ptr node) const { return T(); }

    /**
     * Render the UnaryOperator node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_unaryoperator(const AST::UnaryOperator::Ptr node) const { return T(); }

    /**
     * Render the Uplus node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_uplus(const AST::Uplus::Ptr node) const { return T(); }

    /**
     * Render the Uminus node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_uminus(const AST::Uminus::Ptr node) const { return T(); }

    /**
     * Render the Ulnot node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_ulnot(const AST::Ulnot::Ptr node) const { return T(); }

    /**
     * Render the Unot node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_unot(const AST::Unot::Ptr node) const { return T(); }

    /**
     * Render the Uand node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_uand(const AST::Uand::Ptr node) const { return T(); }

    /**
     * Render the Unand node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_unand(const AST::Unand::Ptr node) const { return T(); }

    /**
     * Render the Uor node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_uor(const AST::Uor::Ptr node) const { return T(); }

    /**
     * Render the Unor node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_unor(const AST::Unor::Ptr node) const { return T(); }

    /**
     * Render the Uxor node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_uxor(const AST::Uxor::Ptr node) const { return T(); }

    /**
     * Render the Uxnor node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_uxnor(const AST::Uxnor::Ptr node) const { return T(); }

    /**
     * Render the Operator node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_operator(const AST::Operator::Ptr node) const { return T(); }

    /**
     * Render the Power node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_power(const AST::Power::Ptr node) const { return T(); }

    /**
     * Render the Times node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_times(const AST::Times::Ptr node) const { return T(); }

    /**
     * Render the Divide node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_divide(const AST::Divide::Ptr node) const { return T(); }

    /**
     * Render the Mod node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_mod(const AST::Mod::Ptr node) const { return T(); }

    /**
     * Render the Plus node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_plus(const AST::Plus::Ptr node) const { return T(); }

    /**
     * Render the Minus node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_minus(const AST::Minus::Ptr node) const { return T(); }

    /**
     * Render the Sll node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_sll(const AST::Sll::Ptr node) const { return T(); }

    /**
     * Render the Srl node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_srl(const AST::Srl::Ptr node) const { return T(); }

    /**
     * Render the Sra node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_sra(const AST::Sra::Ptr node) const { return T(); }

    /**
     * Render the LessThan node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_lessthan(const AST::LessThan::Ptr node) const { return T(); }

    /**
     * Render the GreaterThan node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_greaterthan(const AST::GreaterThan::Ptr node) const { return T(); }

    /**
     * Render the LessEq node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_lesseq(const AST::LessEq::Ptr node) const { return T(); }

    /**
     * Render the GreaterEq node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_greatereq(const AST::GreaterEq::Ptr node) const { return T(); }

    /**
     * Render the Eq node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_eq(const AST::Eq::Ptr node) const { return T(); }

    /**
     * Render the NotEq node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_noteq(const AST::NotEq::Ptr node) const { return T(); }

    /**
     * Render the Eql node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_eql(const AST::Eql::Ptr node) const { return T(); }

    /**
     * Render the NotEql node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_noteql(const AST::NotEql::Ptr node) const { return T(); }

    /**
     * Render the And node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_and(const AST::And::Ptr node) const { return T(); }

    /**
     * Render the Xor node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_xor(const AST::Xor::Ptr node) const { return T(); }

    /**
     * Render the Xnor node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_xnor(const AST::Xnor::Ptr node) const { return T(); }

    /**
     * Render the Or node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_or(const AST::Or::Ptr node) const { return T(); }

    /**
     * Render the Land node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_land(const AST::Land::Ptr node) const { return T(); }

    /**
     * Render the Lor node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_lor(const AST::Lor::Ptr node) const { return T(); }

    /**
     * Render the Cond node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_cond(const AST::Cond::Ptr node) const { return T(); }

    /**
     * Render the Always node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_always(const AST::Always::Ptr node) const { return T(); }

    /**
     * Render the AlwaysFF node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_alwaysff(const AST::AlwaysFF::Ptr node) const { return T(); }

    /**
     * Render the AlwaysComb node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_alwayscomb(const AST::AlwaysComb::Ptr node) const { return T(); }

    /**
     * Render the AlwaysLatch node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_alwayslatch(const AST::AlwaysLatch::Ptr node) const { return T(); }

    /**
     * Render the Senslist node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_senslist(const AST::Senslist::Ptr node) const { return T(); }

    /**
     * Render the Sens node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_sens(const AST::Sens::Ptr node) const { return T(); }

    /**
     * Render the Defparamlist node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_defparamlist(const AST::Defparamlist::Ptr node) const { return T(); }

    /**
     * Render the Defparam node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_defparam(const AST::Defparam::Ptr node) const { return T(); }

    /**
     * Render the Assign node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_assign(const AST::Assign::Ptr node) const { return T(); }

    /**
     * Render the BlockingSubstitution node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_blockingsubstitution(const AST::BlockingSubstitution::Ptr node) const
    {
        return T();
    }

    /**
     * Render the NonblockingSubstitution node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_nonblockingsubstitution(const AST::NonblockingSubstitution::Ptr node) const
    {
        return T();
    }

    /**
     * Render the IfStatement node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_ifstatement(const AST::IfStatement::Ptr node) const { return T(); }

    /**
     * Render the RepeatStatement node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_repeatstatement(const AST::RepeatStatement::Ptr node) const { return T(); }

    /**
     * Render the ForStatement node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_forstatement(const AST::ForStatement::Ptr node) const { return T(); }

    /**
     * Render the WhileStatement node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_whilestatement(const AST::WhileStatement::Ptr node) const { return T(); }

    /**
     * Render the CaseStatement node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_casestatement(const AST::CaseStatement::Ptr node) const { return T(); }

    /**
     * Render the CasexStatement node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_casexstatement(const AST::CasexStatement::Ptr node) const { return T(); }

    /**
     * Render the CasezStatement node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_casezstatement(const AST::CasezStatement::Ptr node) const { return T(); }

    /**
     * Render the UniqueCaseStatement node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_uniquecasestatement(const AST::UniqueCaseStatement::Ptr node) const
    {
        return T();
    }

    /**
     * Render the PriorityCaseStatement node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_prioritycasestatement(const AST::PriorityCaseStatement::Ptr node) const
    {
        return T();
    }

    /**
     * Render the Case node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_case(const AST::Case::Ptr node) const { return T(); }

    /**
     * Render the Block node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_block(const AST::Block::Ptr node) const { return T(); }

    /**
     * Render the Initial node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_initial(const AST::Initial::Ptr node) const { return T(); }

    /**
     * Render the EventStatement node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_eventstatement(const AST::EventStatement::Ptr node) const { return T(); }

    /**
     * Render the WaitStatement node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_waitstatement(const AST::WaitStatement::Ptr node) const { return T(); }

    /**
     * Render the ForeverStatement node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_foreverstatement(const AST::ForeverStatement::Ptr node) const { return T(); }

    /**
     * Render the DelayStatement node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_delaystatement(const AST::DelayStatement::Ptr node) const { return T(); }

    /**
     * Render the Instancelist node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_instancelist(const AST::Instancelist::Ptr node) const { return T(); }

    /**
     * Render the Instance node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_instance(const AST::Instance::Ptr node) const { return T(); }

    /**
     * Render the ParamArg node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_paramarg(const AST::ParamArg::Ptr node) const { return T(); }

    /**
     * Render the PortArg node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_portarg(const AST::PortArg::Ptr node) const { return T(); }

    /**
     * Render the Function node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_function(const AST::Function::Ptr node) const { return T(); }

    /**
     * Render the FunctionCall node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_functioncall(const AST::FunctionCall::Ptr node) const { return T(); }

    /**
     * Render the Task node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_task(const AST::Task::Ptr node) const { return T(); }

    /**
     * Render the TaskCall node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_taskcall(const AST::TaskCall::Ptr node) const { return T(); }

    /**
     * Render the GenerateStatement node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_generatestatement(const AST::GenerateStatement::Ptr node) const { return T(); }

    /**
     * Render the SystemCall node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_systemcall(const AST::SystemCall::Ptr node) const { return T(); }

    /**
     * Render the HierLabel node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_hierlabel(const AST::HierLabel::Ptr node) const { return T(); }

    /**
     * Render the HierName node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_hiername(const AST::HierName::Ptr node) const { return T(); }

    /**
     * Render the Disable node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_disable(const AST::Disable::Ptr node) const { return T(); }

    /**
     * Render the ParallelBlock node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_parallelblock(const AST::ParallelBlock::Ptr node) const { return T(); }

    /**
     * Render the SingleStatement node. This method must be
     * overloaded in a derived class to your needs.
     */
    virtual T render_singlestatement(const AST::SingleStatement::Ptr node) const { return T(); }

private:
    std::string m_indent_str;
};

} // namespace Generators
} // namespace Veriparse

#endif