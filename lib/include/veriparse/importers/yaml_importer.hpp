// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_IMPORTERS_YAML_IMPORTER_HPP
#define VERIPARSE_IMPORTERS_YAML_IMPORTER_HPP

#include <iostream>
#include <string>
#include <yaml-cpp/yaml.h>
#include <veriparse/AST/nodes.hpp>

namespace Veriparse
{
namespace Importers
{

class YAMLImporter
{
public:
    AST::Node::Ptr import(const char *filename) const;
    AST::Node::Ptr import(const std::string &str) const;
    AST::Node::Ptr import(std::istream &iss) const;

private:
    AST::Node::Ptr convert(const YAML::Node node) const;

    AST::Node::Ptr convert_node(const YAML::Node node) const;
    AST::Node::Ptr convert_source(const YAML::Node node) const;
    AST::Node::Ptr convert_description(const YAML::Node node) const;
    AST::Node::Ptr convert_pragmalist(const YAML::Node node) const;
    AST::Node::Ptr convert_pragma(const YAML::Node node) const;
    AST::Node::Ptr convert_module(const YAML::Node node) const;
    AST::Node::Ptr convert_port(const YAML::Node node) const;
    AST::Node::Ptr convert_package(const YAML::Node node) const;
    AST::Node::Ptr convert_import(const YAML::Node node) const;
    AST::Node::Ptr convert_export(const YAML::Node node) const;
    AST::Node::Ptr convert_identifier(const YAML::Node node) const;
    AST::Node::Ptr convert_call(const YAML::Node node) const;
    AST::Node::Ptr convert_constant(const YAML::Node node) const;
    AST::Node::Ptr convert_stringconst(const YAML::Node node) const;
    AST::Node::Ptr convert_intconst(const YAML::Node node) const;
    AST::Node::Ptr convert_intconstn(const YAML::Node node) const;
    AST::Node::Ptr convert_floatconst(const YAML::Node node) const;
    AST::Node::Ptr convert_datatype(const YAML::Node node) const;
    AST::Node::Ptr convert_logictype(const YAML::Node node) const;
    AST::Node::Ptr convert_regtype(const YAML::Node node) const;
    AST::Node::Ptr convert_bittype(const YAML::Node node) const;
    AST::Node::Ptr convert_bytetype(const YAML::Node node) const;
    AST::Node::Ptr convert_shortinttype(const YAML::Node node) const;
    AST::Node::Ptr convert_inttype(const YAML::Node node) const;
    AST::Node::Ptr convert_longinttype(const YAML::Node node) const;
    AST::Node::Ptr convert_integertype(const YAML::Node node) const;
    AST::Node::Ptr convert_timetype(const YAML::Node node) const;
    AST::Node::Ptr convert_realtype(const YAML::Node node) const;
    AST::Node::Ptr convert_shortrealtype(const YAML::Node node) const;
    AST::Node::Ptr convert_realtimetype(const YAML::Node node) const;
    AST::Node::Ptr convert_stringtype(const YAML::Node node) const;
    AST::Node::Ptr convert_chandletype(const YAML::Node node) const;
    AST::Node::Ptr convert_eventtype(const YAML::Node node) const;
    AST::Node::Ptr convert_implicittype(const YAML::Node node) const;
    AST::Node::Ptr convert_voidtype(const YAML::Node node) const;
    AST::Node::Ptr convert_namedtype(const YAML::Node node) const;
    AST::Node::Ptr convert_scopename(const YAML::Node node) const;
    AST::Node::Ptr convert_structtype(const YAML::Node node) const;
    AST::Node::Ptr convert_uniontype(const YAML::Node node) const;
    AST::Node::Ptr convert_enumtype(const YAML::Node node) const;
    AST::Node::Ptr convert_enumitem(const YAML::Node node) const;
    AST::Node::Ptr convert_typeopexpr(const YAML::Node node) const;
    AST::Node::Ptr convert_typeoptype(const YAML::Node node) const;
    AST::Node::Ptr convert_dimension(const YAML::Node node) const;
    AST::Node::Ptr convert_rangedim(const YAML::Node node) const;
    AST::Node::Ptr convert_sizedim(const YAML::Node node) const;
    AST::Node::Ptr convert_unsizeddim(const YAML::Node node) const;
    AST::Node::Ptr convert_queuedim(const YAML::Node node) const;
    AST::Node::Ptr convert_assocdim(const YAML::Node node) const;
    AST::Node::Ptr convert_declaration(const YAML::Node node) const;
    AST::Node::Ptr convert_var(const YAML::Node node) const;
    AST::Node::Ptr convert_net(const YAML::Node node) const;
    AST::Node::Ptr convert_wirenet(const YAML::Node node) const;
    AST::Node::Ptr convert_trinet(const YAML::Node node) const;
    AST::Node::Ptr convert_tri0net(const YAML::Node node) const;
    AST::Node::Ptr convert_tri1net(const YAML::Node node) const;
    AST::Node::Ptr convert_triandnet(const YAML::Node node) const;
    AST::Node::Ptr convert_triornet(const YAML::Node node) const;
    AST::Node::Ptr convert_triregnet(const YAML::Node node) const;
    AST::Node::Ptr convert_wandnet(const YAML::Node node) const;
    AST::Node::Ptr convert_wornet(const YAML::Node node) const;
    AST::Node::Ptr convert_uwirenet(const YAML::Node node) const;
    AST::Node::Ptr convert_supply0net(const YAML::Node node) const;
    AST::Node::Ptr convert_supply1net(const YAML::Node node) const;
    AST::Node::Ptr convert_interconnectnet(const YAML::Node node) const;
    AST::Node::Ptr convert_usernet(const YAML::Node node) const;
    AST::Node::Ptr convert_implicitnet(const YAML::Node node) const;
    AST::Node::Ptr convert_nettypedecl(const YAML::Node node) const;
    AST::Node::Ptr convert_strength(const YAML::Node node) const;
    AST::Node::Ptr convert_drivestrength(const YAML::Node node) const;
    AST::Node::Ptr convert_chargestrength(const YAML::Node node) const;
    AST::Node::Ptr convert_param(const YAML::Node node) const;
    AST::Node::Ptr convert_typeparam(const YAML::Node node) const;
    AST::Node::Ptr convert_typedef(const YAML::Node node) const;
    AST::Node::Ptr convert_member(const YAML::Node node) const;
    AST::Node::Ptr convert_arg(const YAML::Node node) const;
    AST::Node::Ptr convert_genvar(const YAML::Node node) const;
    AST::Node::Ptr convert_concat(const YAML::Node node) const;
    AST::Node::Ptr convert_lconcat(const YAML::Node node) const;
    AST::Node::Ptr convert_repeat(const YAML::Node node) const;
    AST::Node::Ptr convert_assignmentpattern(const YAML::Node node) const;
    AST::Node::Ptr convert_patternitem(const YAML::Node node) const;
    AST::Node::Ptr convert_cast(const YAML::Node node) const;
    AST::Node::Ptr convert_typecast(const YAML::Node node) const;
    AST::Node::Ptr convert_sizecast(const YAML::Node node) const;
    AST::Node::Ptr convert_signingcast(const YAML::Node node) const;
    AST::Node::Ptr convert_constcast(const YAML::Node node) const;
    AST::Node::Ptr convert_indirect(const YAML::Node node) const;
    AST::Node::Ptr convert_partselect(const YAML::Node node) const;
    AST::Node::Ptr convert_partselectindexed(const YAML::Node node) const;
    AST::Node::Ptr convert_partselectplusindexed(const YAML::Node node) const;
    AST::Node::Ptr convert_partselectminusindexed(const YAML::Node node) const;
    AST::Node::Ptr convert_pointer(const YAML::Node node) const;
    AST::Node::Ptr convert_lvalue(const YAML::Node node) const;
    AST::Node::Ptr convert_rvalue(const YAML::Node node) const;
    AST::Node::Ptr convert_unaryoperator(const YAML::Node node) const;
    AST::Node::Ptr convert_uplus(const YAML::Node node) const;
    AST::Node::Ptr convert_uminus(const YAML::Node node) const;
    AST::Node::Ptr convert_ulnot(const YAML::Node node) const;
    AST::Node::Ptr convert_unot(const YAML::Node node) const;
    AST::Node::Ptr convert_uand(const YAML::Node node) const;
    AST::Node::Ptr convert_unand(const YAML::Node node) const;
    AST::Node::Ptr convert_uor(const YAML::Node node) const;
    AST::Node::Ptr convert_unor(const YAML::Node node) const;
    AST::Node::Ptr convert_uxor(const YAML::Node node) const;
    AST::Node::Ptr convert_uxnor(const YAML::Node node) const;
    AST::Node::Ptr convert_operator(const YAML::Node node) const;
    AST::Node::Ptr convert_power(const YAML::Node node) const;
    AST::Node::Ptr convert_times(const YAML::Node node) const;
    AST::Node::Ptr convert_divide(const YAML::Node node) const;
    AST::Node::Ptr convert_mod(const YAML::Node node) const;
    AST::Node::Ptr convert_plus(const YAML::Node node) const;
    AST::Node::Ptr convert_minus(const YAML::Node node) const;
    AST::Node::Ptr convert_sll(const YAML::Node node) const;
    AST::Node::Ptr convert_srl(const YAML::Node node) const;
    AST::Node::Ptr convert_sra(const YAML::Node node) const;
    AST::Node::Ptr convert_lessthan(const YAML::Node node) const;
    AST::Node::Ptr convert_greaterthan(const YAML::Node node) const;
    AST::Node::Ptr convert_lesseq(const YAML::Node node) const;
    AST::Node::Ptr convert_greatereq(const YAML::Node node) const;
    AST::Node::Ptr convert_eq(const YAML::Node node) const;
    AST::Node::Ptr convert_noteq(const YAML::Node node) const;
    AST::Node::Ptr convert_eql(const YAML::Node node) const;
    AST::Node::Ptr convert_noteql(const YAML::Node node) const;
    AST::Node::Ptr convert_and(const YAML::Node node) const;
    AST::Node::Ptr convert_xor(const YAML::Node node) const;
    AST::Node::Ptr convert_xnor(const YAML::Node node) const;
    AST::Node::Ptr convert_or(const YAML::Node node) const;
    AST::Node::Ptr convert_land(const YAML::Node node) const;
    AST::Node::Ptr convert_lor(const YAML::Node node) const;
    AST::Node::Ptr convert_cond(const YAML::Node node) const;
    AST::Node::Ptr convert_always(const YAML::Node node) const;
    AST::Node::Ptr convert_alwaysff(const YAML::Node node) const;
    AST::Node::Ptr convert_alwayscomb(const YAML::Node node) const;
    AST::Node::Ptr convert_alwayslatch(const YAML::Node node) const;
    AST::Node::Ptr convert_senslist(const YAML::Node node) const;
    AST::Node::Ptr convert_sens(const YAML::Node node) const;
    AST::Node::Ptr convert_defparamlist(const YAML::Node node) const;
    AST::Node::Ptr convert_defparam(const YAML::Node node) const;
    AST::Node::Ptr convert_assign(const YAML::Node node) const;
    AST::Node::Ptr convert_blockingsubstitution(const YAML::Node node) const;
    AST::Node::Ptr convert_nonblockingsubstitution(const YAML::Node node) const;
    AST::Node::Ptr convert_ifstatement(const YAML::Node node) const;
    AST::Node::Ptr convert_repeatstatement(const YAML::Node node) const;
    AST::Node::Ptr convert_forstatement(const YAML::Node node) const;
    AST::Node::Ptr convert_whilestatement(const YAML::Node node) const;
    AST::Node::Ptr convert_casestatement(const YAML::Node node) const;
    AST::Node::Ptr convert_casexstatement(const YAML::Node node) const;
    AST::Node::Ptr convert_casezstatement(const YAML::Node node) const;
    AST::Node::Ptr convert_uniquecasestatement(const YAML::Node node) const;
    AST::Node::Ptr convert_prioritycasestatement(const YAML::Node node) const;
    AST::Node::Ptr convert_case(const YAML::Node node) const;
    AST::Node::Ptr convert_block(const YAML::Node node) const;
    AST::Node::Ptr convert_initial(const YAML::Node node) const;
    AST::Node::Ptr convert_eventstatement(const YAML::Node node) const;
    AST::Node::Ptr convert_waitstatement(const YAML::Node node) const;
    AST::Node::Ptr convert_foreverstatement(const YAML::Node node) const;
    AST::Node::Ptr convert_delaystatement(const YAML::Node node) const;
    AST::Node::Ptr convert_instancelist(const YAML::Node node) const;
    AST::Node::Ptr convert_instance(const YAML::Node node) const;
    AST::Node::Ptr convert_paramarg(const YAML::Node node) const;
    AST::Node::Ptr convert_portarg(const YAML::Node node) const;
    AST::Node::Ptr convert_function(const YAML::Node node) const;
    AST::Node::Ptr convert_functioncall(const YAML::Node node) const;
    AST::Node::Ptr convert_task(const YAML::Node node) const;
    AST::Node::Ptr convert_taskcall(const YAML::Node node) const;
    AST::Node::Ptr convert_generatestatement(const YAML::Node node) const;
    AST::Node::Ptr convert_systemcall(const YAML::Node node) const;
    AST::Node::Ptr convert_hierlabel(const YAML::Node node) const;
    AST::Node::Ptr convert_hiername(const YAML::Node node) const;
    AST::Node::Ptr convert_disable(const YAML::Node node) const;
    AST::Node::Ptr convert_parallelblock(const YAML::Node node) const;
    AST::Node::Ptr convert_singlestatement(const YAML::Node node) const;
};

} // namespace Importers
} // namespace Veriparse

#endif