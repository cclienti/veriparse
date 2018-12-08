#ifndef VERIPARSE_IMPORTERS_YAML_IMPORTER_HPP
#define VERIPARSE_IMPORTERS_YAML_IMPORTER_HPP


#include <iostream>
#include <string>
#include <yaml-cpp/yaml.h>
#include <veriparse/AST/nodes.hpp>


namespace Veriparse {
namespace Importers {

class YAMLImporter {
public:
	AST::Node::Ptr import(const char *filename) const;
	AST::Node::Ptr import(const std::string &str) const;
	AST::Node::Ptr import(std::istream &iss) const;

private:
	AST::Node::Ptr convert(const YAML::Node node) const;

	AST::Node::Ptr convert_node(const YAML::Node node) const;
	AST::Node::Ptr convert_source(const YAML::Node node) const;
	AST::Node::Ptr convert_description(const YAML::Node node) const;
	AST::Node::Ptr convert_pragma(const YAML::Node node) const;
	AST::Node::Ptr convert_module(const YAML::Node node) const;
	AST::Node::Ptr convert_port(const YAML::Node node) const;
	AST::Node::Ptr convert_width(const YAML::Node node) const;
	AST::Node::Ptr convert_length(const YAML::Node node) const;
	AST::Node::Ptr convert_identifier(const YAML::Node node) const;
	AST::Node::Ptr convert_constant(const YAML::Node node) const;
	AST::Node::Ptr convert_stringconst(const YAML::Node node) const;
	AST::Node::Ptr convert_intconst(const YAML::Node node) const;
	AST::Node::Ptr convert_intconstn(const YAML::Node node) const;
	AST::Node::Ptr convert_floatconst(const YAML::Node node) const;
	AST::Node::Ptr convert_iodir(const YAML::Node node) const;
	AST::Node::Ptr convert_input(const YAML::Node node) const;
	AST::Node::Ptr convert_output(const YAML::Node node) const;
	AST::Node::Ptr convert_inout(const YAML::Node node) const;
	AST::Node::Ptr convert_variablebase(const YAML::Node node) const;
	AST::Node::Ptr convert_genvar(const YAML::Node node) const;
	AST::Node::Ptr convert_variable(const YAML::Node node) const;
	AST::Node::Ptr convert_net(const YAML::Node node) const;
	AST::Node::Ptr convert_integer(const YAML::Node node) const;
	AST::Node::Ptr convert_real(const YAML::Node node) const;
	AST::Node::Ptr convert_tri(const YAML::Node node) const;
	AST::Node::Ptr convert_wire(const YAML::Node node) const;
	AST::Node::Ptr convert_supply0(const YAML::Node node) const;
	AST::Node::Ptr convert_supply1(const YAML::Node node) const;
	AST::Node::Ptr convert_reg(const YAML::Node node) const;
	AST::Node::Ptr convert_ioport(const YAML::Node node) const;
	AST::Node::Ptr convert_parameter(const YAML::Node node) const;
	AST::Node::Ptr convert_localparam(const YAML::Node node) const;
	AST::Node::Ptr convert_concat(const YAML::Node node) const;
	AST::Node::Ptr convert_lconcat(const YAML::Node node) const;
	AST::Node::Ptr convert_repeat(const YAML::Node node) const;
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
	AST::Node::Ptr convert_identifierscopelabel(const YAML::Node node) const;
	AST::Node::Ptr convert_identifierscope(const YAML::Node node) const;
	AST::Node::Ptr convert_disable(const YAML::Node node) const;
	AST::Node::Ptr convert_parallelblock(const YAML::Node node) const;
	AST::Node::Ptr convert_singlestatement(const YAML::Node node) const;
};

}
}


#endif