#include <veriparse/importers/yaml_importer.hpp>
#include <veriparse/importers/yaml_specializations.hpp>
#include <veriparse/AST/node_cast.hpp>

#include <memory>
#include <iostream>
#include <exception>


namespace Veriparse {
namespace Importers {

AST::Node::Ptr YAMLImporter::import(const char *filename) const {
	YAML::Node node;

	try {
		node = YAML::LoadFile(filename);
	}
	catch ( const std::exception & e ) {
		std::cerr << e.what() << std::endl;
	}

	return convert(node);
}

AST::Node::Ptr YAMLImporter::import(std::istream &iss) const {
	YAML::Node node;

	try {
		node = YAML::Load(iss);
	}
	catch ( const std::exception & e ) {
		std::cerr << e.what() << std::endl;
	}

	return convert(node);
}

AST::Node::Ptr YAMLImporter::import(const std::string &str) const {
	YAML::Node node;

	try {
		node = YAML::Load(str);
	}
	catch ( const std::exception & e ) {
		std::cerr << e.what() << std::endl;
	}

	return convert(node);
}

AST::Node::Ptr YAMLImporter::convert(const YAML::Node node) const {
	if (node.IsMap()) {
		if (node["Source"]) {
			return convert_source(node["Source"]);
		}
		if (node["Description"]) {
			return convert_description(node["Description"]);
		}
		if (node["Pragma"]) {
			return convert_pragma(node["Pragma"]);
		}
		if (node["Module"]) {
			return convert_module(node["Module"]);
		}
		if (node["Port"]) {
			return convert_port(node["Port"]);
		}
		if (node["Width"]) {
			return convert_width(node["Width"]);
		}
		if (node["Length"]) {
			return convert_length(node["Length"]);
		}
		if (node["Identifier"]) {
			return convert_identifier(node["Identifier"]);
		}
		if (node["Constant"]) {
			return convert_constant(node["Constant"]);
		}
		if (node["StringConst"]) {
			return convert_stringconst(node["StringConst"]);
		}
		if (node["IntConst"]) {
			return convert_intconst(node["IntConst"]);
		}
		if (node["IntConstN"]) {
			return convert_intconstn(node["IntConstN"]);
		}
		if (node["FloatConst"]) {
			return convert_floatconst(node["FloatConst"]);
		}
		if (node["IODir"]) {
			return convert_iodir(node["IODir"]);
		}
		if (node["Input"]) {
			return convert_input(node["Input"]);
		}
		if (node["Output"]) {
			return convert_output(node["Output"]);
		}
		if (node["Inout"]) {
			return convert_inout(node["Inout"]);
		}
		if (node["VariableBase"]) {
			return convert_variablebase(node["VariableBase"]);
		}
		if (node["Genvar"]) {
			return convert_genvar(node["Genvar"]);
		}
		if (node["Variable"]) {
			return convert_variable(node["Variable"]);
		}
		if (node["Net"]) {
			return convert_net(node["Net"]);
		}
		if (node["Integer"]) {
			return convert_integer(node["Integer"]);
		}
		if (node["Real"]) {
			return convert_real(node["Real"]);
		}
		if (node["Tri"]) {
			return convert_tri(node["Tri"]);
		}
		if (node["Wire"]) {
			return convert_wire(node["Wire"]);
		}
		if (node["Supply0"]) {
			return convert_supply0(node["Supply0"]);
		}
		if (node["Supply1"]) {
			return convert_supply1(node["Supply1"]);
		}
		if (node["Reg"]) {
			return convert_reg(node["Reg"]);
		}
		if (node["Ioport"]) {
			return convert_ioport(node["Ioport"]);
		}
		if (node["Parameter"]) {
			return convert_parameter(node["Parameter"]);
		}
		if (node["Localparam"]) {
			return convert_localparam(node["Localparam"]);
		}
		if (node["Concat"]) {
			return convert_concat(node["Concat"]);
		}
		if (node["Lconcat"]) {
			return convert_lconcat(node["Lconcat"]);
		}
		if (node["Repeat"]) {
			return convert_repeat(node["Repeat"]);
		}
		if (node["Indirect"]) {
			return convert_indirect(node["Indirect"]);
		}
		if (node["Partselect"]) {
			return convert_partselect(node["Partselect"]);
		}
		if (node["PartselectIndexed"]) {
			return convert_partselectindexed(node["PartselectIndexed"]);
		}
		if (node["PartselectPlusIndexed"]) {
			return convert_partselectplusindexed(node["PartselectPlusIndexed"]);
		}
		if (node["PartselectMinusIndexed"]) {
			return convert_partselectminusindexed(node["PartselectMinusIndexed"]);
		}
		if (node["Pointer"]) {
			return convert_pointer(node["Pointer"]);
		}
		if (node["Lvalue"]) {
			return convert_lvalue(node["Lvalue"]);
		}
		if (node["Rvalue"]) {
			return convert_rvalue(node["Rvalue"]);
		}
		if (node["UnaryOperator"]) {
			return convert_unaryoperator(node["UnaryOperator"]);
		}
		if (node["Uplus"]) {
			return convert_uplus(node["Uplus"]);
		}
		if (node["Uminus"]) {
			return convert_uminus(node["Uminus"]);
		}
		if (node["Ulnot"]) {
			return convert_ulnot(node["Ulnot"]);
		}
		if (node["Unot"]) {
			return convert_unot(node["Unot"]);
		}
		if (node["Uand"]) {
			return convert_uand(node["Uand"]);
		}
		if (node["Unand"]) {
			return convert_unand(node["Unand"]);
		}
		if (node["Uor"]) {
			return convert_uor(node["Uor"]);
		}
		if (node["Unor"]) {
			return convert_unor(node["Unor"]);
		}
		if (node["Uxor"]) {
			return convert_uxor(node["Uxor"]);
		}
		if (node["Uxnor"]) {
			return convert_uxnor(node["Uxnor"]);
		}
		if (node["Operator"]) {
			return convert_operator(node["Operator"]);
		}
		if (node["Power"]) {
			return convert_power(node["Power"]);
		}
		if (node["Times"]) {
			return convert_times(node["Times"]);
		}
		if (node["Divide"]) {
			return convert_divide(node["Divide"]);
		}
		if (node["Mod"]) {
			return convert_mod(node["Mod"]);
		}
		if (node["Plus"]) {
			return convert_plus(node["Plus"]);
		}
		if (node["Minus"]) {
			return convert_minus(node["Minus"]);
		}
		if (node["Sll"]) {
			return convert_sll(node["Sll"]);
		}
		if (node["Srl"]) {
			return convert_srl(node["Srl"]);
		}
		if (node["Sra"]) {
			return convert_sra(node["Sra"]);
		}
		if (node["LessThan"]) {
			return convert_lessthan(node["LessThan"]);
		}
		if (node["GreaterThan"]) {
			return convert_greaterthan(node["GreaterThan"]);
		}
		if (node["LessEq"]) {
			return convert_lesseq(node["LessEq"]);
		}
		if (node["GreaterEq"]) {
			return convert_greatereq(node["GreaterEq"]);
		}
		if (node["Eq"]) {
			return convert_eq(node["Eq"]);
		}
		if (node["NotEq"]) {
			return convert_noteq(node["NotEq"]);
		}
		if (node["Eql"]) {
			return convert_eql(node["Eql"]);
		}
		if (node["NotEql"]) {
			return convert_noteql(node["NotEql"]);
		}
		if (node["And"]) {
			return convert_and(node["And"]);
		}
		if (node["Xor"]) {
			return convert_xor(node["Xor"]);
		}
		if (node["Xnor"]) {
			return convert_xnor(node["Xnor"]);
		}
		if (node["Or"]) {
			return convert_or(node["Or"]);
		}
		if (node["Land"]) {
			return convert_land(node["Land"]);
		}
		if (node["Lor"]) {
			return convert_lor(node["Lor"]);
		}
		if (node["Cond"]) {
			return convert_cond(node["Cond"]);
		}
		if (node["Always"]) {
			return convert_always(node["Always"]);
		}
		if (node["Senslist"]) {
			return convert_senslist(node["Senslist"]);
		}
		if (node["Sens"]) {
			return convert_sens(node["Sens"]);
		}
		if (node["Defparamlist"]) {
			return convert_defparamlist(node["Defparamlist"]);
		}
		if (node["Defparam"]) {
			return convert_defparam(node["Defparam"]);
		}
		if (node["Assign"]) {
			return convert_assign(node["Assign"]);
		}
		if (node["BlockingSubstitution"]) {
			return convert_blockingsubstitution(node["BlockingSubstitution"]);
		}
		if (node["NonblockingSubstitution"]) {
			return convert_nonblockingsubstitution(node["NonblockingSubstitution"]);
		}
		if (node["IfStatement"]) {
			return convert_ifstatement(node["IfStatement"]);
		}
		if (node["RepeatStatement"]) {
			return convert_repeatstatement(node["RepeatStatement"]);
		}
		if (node["ForStatement"]) {
			return convert_forstatement(node["ForStatement"]);
		}
		if (node["WhileStatement"]) {
			return convert_whilestatement(node["WhileStatement"]);
		}
		if (node["CaseStatement"]) {
			return convert_casestatement(node["CaseStatement"]);
		}
		if (node["CasexStatement"]) {
			return convert_casexstatement(node["CasexStatement"]);
		}
		if (node["CasezStatement"]) {
			return convert_casezstatement(node["CasezStatement"]);
		}
		if (node["Case"]) {
			return convert_case(node["Case"]);
		}
		if (node["Block"]) {
			return convert_block(node["Block"]);
		}
		if (node["Initial"]) {
			return convert_initial(node["Initial"]);
		}
		if (node["EventStatement"]) {
			return convert_eventstatement(node["EventStatement"]);
		}
		if (node["WaitStatement"]) {
			return convert_waitstatement(node["WaitStatement"]);
		}
		if (node["ForeverStatement"]) {
			return convert_foreverstatement(node["ForeverStatement"]);
		}
		if (node["DelayStatement"]) {
			return convert_delaystatement(node["DelayStatement"]);
		}
		if (node["Instancelist"]) {
			return convert_instancelist(node["Instancelist"]);
		}
		if (node["Instance"]) {
			return convert_instance(node["Instance"]);
		}
		if (node["ParamArg"]) {
			return convert_paramarg(node["ParamArg"]);
		}
		if (node["PortArg"]) {
			return convert_portarg(node["PortArg"]);
		}
		if (node["Function"]) {
			return convert_function(node["Function"]);
		}
		if (node["FunctionCall"]) {
			return convert_functioncall(node["FunctionCall"]);
		}
		if (node["Task"]) {
			return convert_task(node["Task"]);
		}
		if (node["TaskCall"]) {
			return convert_taskcall(node["TaskCall"]);
		}
		if (node["GenerateStatement"]) {
			return convert_generatestatement(node["GenerateStatement"]);
		}
		if (node["SystemCall"]) {
			return convert_systemcall(node["SystemCall"]);
		}
		if (node["IdentifierScopeLabel"]) {
			return convert_identifierscopelabel(node["IdentifierScopeLabel"]);
		}
		if (node["IdentifierScope"]) {
			return convert_identifierscope(node["IdentifierScope"]);
		}
		if (node["Disable"]) {
			return convert_disable(node["Disable"]);
		}
		if (node["ParallelBlock"]) {
			return convert_parallelblock(node["ParallelBlock"]);
		}
		if (node["SingleStatement"]) {
			return convert_singlestatement(node["SingleStatement"]);
		}
	}

	return AST::Node::Ptr(nullptr);
}

AST::Node::Ptr YAMLImporter::convert_node(const YAML::Node node) const {
	AST::Node::Ptr result(new AST::Node);
	return result;
}



AST::Node::Ptr YAMLImporter::convert_source(const YAML::Node node) const {
	AST::Source::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Source>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Source>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child description
		if (node["description"]) {
			const YAML::Node node_description = node["description"];
			// Set the child
			AST::Node::Ptr child = convert(node_description);
			if (child) {
				AST::Description::Ptr child_cast = AST::cast_to<AST::Description>(child);
				if(!result) result = std::make_shared<AST::Source>();
				result->set_description(child_cast);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Source>(result);
}


AST::Node::Ptr YAMLImporter::convert_description(const YAML::Node node) const {
	AST::Description::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Description>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Description>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child definitions
		if (node["definitions"]) {
			const YAML::Node node_definitions = node["definitions"];
			// Fill the list of children
			AST::Node::ListPtr definitions_list (new AST::Node::List);
			if (node_definitions.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_definitions.begin(); it !=  node_definitions.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						definitions_list->push_back(child);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_definitions);
				if (child) {
					definitions_list->push_back(child);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::Description>();
			result->set_definitions(definitions_list);
		}
	}

	// Return the result
	return AST::cast_to<AST::Description>(result);
}


AST::Node::Ptr YAMLImporter::convert_pragma(const YAML::Node node) const {
	AST::Pragma::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Pragma>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Pragma>();
				result->set_line(node["line"].as<int>());
			}
		}
		// Manage property name
		if (node["name"]) {
			if (node["name"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Pragma>();
				result->set_name(node["name"].as<std::string>());
			}
		}

		// Manage Child expression
		if (node["expression"]) {
			const YAML::Node node_expression = node["expression"];
			// Set the child
			AST::Node::Ptr child = convert(node_expression);
			if (child) {
				if(!result) result = std::make_shared<AST::Pragma>();
				result->set_expression(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Pragma>(result);
}


AST::Node::Ptr YAMLImporter::convert_module(const YAML::Node node) const {
	AST::Module::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Module>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Module>();
				result->set_line(node["line"].as<int>());
			}
		}
		// Manage property name
		if (node["name"]) {
			if (node["name"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Module>();
				result->set_name(node["name"].as<std::string>());
			}
		}
		// Manage property default_nettype
		if (node["default_nettype"]) {
			if (node["default_nettype"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Module>();
				result->set_default_nettype(node["default_nettype"].as<AST::Module::Default_nettypeEnum>());
			}
		}

		// Manage Child params
		if (node["params"]) {
			const YAML::Node node_params = node["params"];
			// Fill the list of children
			AST::Parameter::ListPtr params_list (new AST::Parameter::List);
			if (node_params.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_params.begin(); it !=  node_params.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						AST::Parameter::Ptr child_cast = AST::cast_to<AST::Parameter>(child);
						params_list->push_back(child_cast);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_params);
				if (child) {
					AST::Parameter::Ptr child_cast = AST::cast_to<AST::Parameter>(child);
					params_list->push_back(child_cast);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::Module>();
			result->set_params(params_list);
		}

		// Manage Child ports
		if (node["ports"]) {
			const YAML::Node node_ports = node["ports"];
			// Fill the list of children
			AST::Node::ListPtr ports_list (new AST::Node::List);
			if (node_ports.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_ports.begin(); it !=  node_ports.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						ports_list->push_back(child);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_ports);
				if (child) {
					ports_list->push_back(child);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::Module>();
			result->set_ports(ports_list);
		}

		// Manage Child items
		if (node["items"]) {
			const YAML::Node node_items = node["items"];
			// Fill the list of children
			AST::Node::ListPtr items_list (new AST::Node::List);
			if (node_items.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_items.begin(); it !=  node_items.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						items_list->push_back(child);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_items);
				if (child) {
					items_list->push_back(child);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::Module>();
			result->set_items(items_list);
		}
	}

	// Return the result
	return AST::cast_to<AST::Module>(result);
}


AST::Node::Ptr YAMLImporter::convert_port(const YAML::Node node) const {
	AST::Port::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Port>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Port>();
				result->set_line(node["line"].as<int>());
			}
		}
		// Manage property name
		if (node["name"]) {
			if (node["name"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Port>();
				result->set_name(node["name"].as<std::string>());
			}
		}

		// Manage Child widths
		if (node["widths"]) {
			const YAML::Node node_widths = node["widths"];
			// Fill the list of children
			AST::Width::ListPtr widths_list (new AST::Width::List);
			if (node_widths.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_widths.begin(); it !=  node_widths.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						AST::Width::Ptr child_cast = AST::cast_to<AST::Width>(child);
						widths_list->push_back(child_cast);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_widths);
				if (child) {
					AST::Width::Ptr child_cast = AST::cast_to<AST::Width>(child);
					widths_list->push_back(child_cast);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::Port>();
			result->set_widths(widths_list);
		}
	}

	// Return the result
	return AST::cast_to<AST::Port>(result);
}


AST::Node::Ptr YAMLImporter::convert_width(const YAML::Node node) const {
	AST::Width::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Width>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Width>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child msb
		if (node["msb"]) {
			const YAML::Node node_msb = node["msb"];
			// Set the child
			AST::Node::Ptr child = convert(node_msb);
			if (child) {
				if(!result) result = std::make_shared<AST::Width>();
				result->set_msb(child);
			}
		}

		// Manage Child lsb
		if (node["lsb"]) {
			const YAML::Node node_lsb = node["lsb"];
			// Set the child
			AST::Node::Ptr child = convert(node_lsb);
			if (child) {
				if(!result) result = std::make_shared<AST::Width>();
				result->set_lsb(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Width>(result);
}


AST::Node::Ptr YAMLImporter::convert_length(const YAML::Node node) const {
	AST::Length::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Length>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Length>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child msb
		if (node["msb"]) {
			const YAML::Node node_msb = node["msb"];
			// Set the child
			AST::Node::Ptr child = convert(node_msb);
			if (child) {
				if(!result) result = std::make_shared<AST::Length>();
				result->set_msb(child);
			}
		}

		// Manage Child lsb
		if (node["lsb"]) {
			const YAML::Node node_lsb = node["lsb"];
			// Set the child
			AST::Node::Ptr child = convert(node_lsb);
			if (child) {
				if(!result) result = std::make_shared<AST::Length>();
				result->set_lsb(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Length>(result);
}


AST::Node::Ptr YAMLImporter::convert_identifier(const YAML::Node node) const {
	AST::Identifier::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Identifier>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Identifier>();
				result->set_line(node["line"].as<int>());
			}
		}
		// Manage property name
		if (node["name"]) {
			if (node["name"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Identifier>();
				result->set_name(node["name"].as<std::string>());
			}
		}

		// Manage Child scope
		if (node["scope"]) {
			const YAML::Node node_scope = node["scope"];
			// Set the child
			AST::Node::Ptr child = convert(node_scope);
			if (child) {
				AST::IdentifierScope::Ptr child_cast = AST::cast_to<AST::IdentifierScope>(child);
				if(!result) result = std::make_shared<AST::Identifier>();
				result->set_scope(child_cast);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Identifier>(result);
}


AST::Node::Ptr YAMLImporter::convert_constant(const YAML::Node node) const {
	AST::Constant::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Constant>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Constant>();
				result->set_line(node["line"].as<int>());
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Constant>(result);
}


AST::Node::Ptr YAMLImporter::convert_stringconst(const YAML::Node node) const {
	AST::StringConst::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::StringConst>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::StringConst>();
				result->set_line(node["line"].as<int>());
			}
		}
		// Manage property value
		if (node["value"]) {
			if (node["value"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::StringConst>();
				result->set_value(node["value"].as<std::string>());
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::StringConst>(result);
}


AST::Node::Ptr YAMLImporter::convert_intconst(const YAML::Node node) const {
	AST::IntConst::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::IntConst>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::IntConst>();
				result->set_line(node["line"].as<int>());
			}
		}
		// Manage property value
		if (node["value"]) {
			if (node["value"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::IntConst>();
				result->set_value(node["value"].as<std::string>());
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::IntConst>(result);
}


AST::Node::Ptr YAMLImporter::convert_intconstn(const YAML::Node node) const {
	AST::IntConstN::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::IntConstN>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::IntConstN>();
				result->set_line(node["line"].as<int>());
			}
		}
		// Manage property base
		if (node["base"]) {
			if (node["base"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::IntConstN>();
				result->set_base(node["base"].as<int>());
			}
		}
		// Manage property size
		if (node["size"]) {
			if (node["size"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::IntConstN>();
				result->set_size(node["size"].as<int>());
			}
		}
		// Manage property sign
		if (node["sign"]) {
			if (node["sign"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::IntConstN>();
				result->set_sign(node["sign"].as<bool>());
			}
		}
		// Manage property value
		if (node["value"]) {
			if (node["value"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::IntConstN>();
				result->set_value(node["value"].as<mpz_class>());
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::IntConstN>(result);
}


AST::Node::Ptr YAMLImporter::convert_floatconst(const YAML::Node node) const {
	AST::FloatConst::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::FloatConst>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::FloatConst>();
				result->set_line(node["line"].as<int>());
			}
		}
		// Manage property value
		if (node["value"]) {
			if (node["value"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::FloatConst>();
				result->set_value(node["value"].as<double>());
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::FloatConst>(result);
}


AST::Node::Ptr YAMLImporter::convert_iodir(const YAML::Node node) const {
	AST::IODir::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::IODir>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::IODir>();
				result->set_line(node["line"].as<int>());
			}
		}
		// Manage property name
		if (node["name"]) {
			if (node["name"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::IODir>();
				result->set_name(node["name"].as<std::string>());
			}
		}
		// Manage property sign
		if (node["sign"]) {
			if (node["sign"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::IODir>();
				result->set_sign(node["sign"].as<bool>());
			}
		}

		// Manage Child widths
		if (node["widths"]) {
			const YAML::Node node_widths = node["widths"];
			// Fill the list of children
			AST::Width::ListPtr widths_list (new AST::Width::List);
			if (node_widths.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_widths.begin(); it !=  node_widths.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						AST::Width::Ptr child_cast = AST::cast_to<AST::Width>(child);
						widths_list->push_back(child_cast);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_widths);
				if (child) {
					AST::Width::Ptr child_cast = AST::cast_to<AST::Width>(child);
					widths_list->push_back(child_cast);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::IODir>();
			result->set_widths(widths_list);
		}
	}

	// Return the result
	return AST::cast_to<AST::IODir>(result);
}


AST::Node::Ptr YAMLImporter::convert_input(const YAML::Node node) const {
	AST::Input::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Input>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Input>();
				result->set_line(node["line"].as<int>());
			}
		}
		// Manage property name
		if (node["name"]) {
			if (node["name"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Input>();
				result->set_name(node["name"].as<std::string>());
			}
		}
		// Manage property sign
		if (node["sign"]) {
			if (node["sign"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Input>();
				result->set_sign(node["sign"].as<bool>());
			}
		}

		// Manage Child widths
		if (node["widths"]) {
			const YAML::Node node_widths = node["widths"];
			// Fill the list of children
			AST::Width::ListPtr widths_list (new AST::Width::List);
			if (node_widths.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_widths.begin(); it !=  node_widths.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						AST::Width::Ptr child_cast = AST::cast_to<AST::Width>(child);
						widths_list->push_back(child_cast);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_widths);
				if (child) {
					AST::Width::Ptr child_cast = AST::cast_to<AST::Width>(child);
					widths_list->push_back(child_cast);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::Input>();
			result->set_widths(widths_list);
		}
	}

	// Return the result
	return AST::cast_to<AST::Input>(result);
}


AST::Node::Ptr YAMLImporter::convert_output(const YAML::Node node) const {
	AST::Output::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Output>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Output>();
				result->set_line(node["line"].as<int>());
			}
		}
		// Manage property name
		if (node["name"]) {
			if (node["name"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Output>();
				result->set_name(node["name"].as<std::string>());
			}
		}
		// Manage property sign
		if (node["sign"]) {
			if (node["sign"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Output>();
				result->set_sign(node["sign"].as<bool>());
			}
		}

		// Manage Child widths
		if (node["widths"]) {
			const YAML::Node node_widths = node["widths"];
			// Fill the list of children
			AST::Width::ListPtr widths_list (new AST::Width::List);
			if (node_widths.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_widths.begin(); it !=  node_widths.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						AST::Width::Ptr child_cast = AST::cast_to<AST::Width>(child);
						widths_list->push_back(child_cast);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_widths);
				if (child) {
					AST::Width::Ptr child_cast = AST::cast_to<AST::Width>(child);
					widths_list->push_back(child_cast);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::Output>();
			result->set_widths(widths_list);
		}
	}

	// Return the result
	return AST::cast_to<AST::Output>(result);
}


AST::Node::Ptr YAMLImporter::convert_inout(const YAML::Node node) const {
	AST::Inout::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Inout>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Inout>();
				result->set_line(node["line"].as<int>());
			}
		}
		// Manage property name
		if (node["name"]) {
			if (node["name"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Inout>();
				result->set_name(node["name"].as<std::string>());
			}
		}
		// Manage property sign
		if (node["sign"]) {
			if (node["sign"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Inout>();
				result->set_sign(node["sign"].as<bool>());
			}
		}

		// Manage Child widths
		if (node["widths"]) {
			const YAML::Node node_widths = node["widths"];
			// Fill the list of children
			AST::Width::ListPtr widths_list (new AST::Width::List);
			if (node_widths.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_widths.begin(); it !=  node_widths.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						AST::Width::Ptr child_cast = AST::cast_to<AST::Width>(child);
						widths_list->push_back(child_cast);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_widths);
				if (child) {
					AST::Width::Ptr child_cast = AST::cast_to<AST::Width>(child);
					widths_list->push_back(child_cast);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::Inout>();
			result->set_widths(widths_list);
		}
	}

	// Return the result
	return AST::cast_to<AST::Inout>(result);
}


AST::Node::Ptr YAMLImporter::convert_variablebase(const YAML::Node node) const {
	AST::VariableBase::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::VariableBase>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::VariableBase>();
				result->set_line(node["line"].as<int>());
			}
		}
		// Manage property name
		if (node["name"]) {
			if (node["name"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::VariableBase>();
				result->set_name(node["name"].as<std::string>());
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::VariableBase>(result);
}


AST::Node::Ptr YAMLImporter::convert_genvar(const YAML::Node node) const {
	AST::Genvar::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Genvar>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Genvar>();
				result->set_line(node["line"].as<int>());
			}
		}
		// Manage property name
		if (node["name"]) {
			if (node["name"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Genvar>();
				result->set_name(node["name"].as<std::string>());
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Genvar>(result);
}


AST::Node::Ptr YAMLImporter::convert_variable(const YAML::Node node) const {
	AST::Variable::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Variable>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Variable>();
				result->set_line(node["line"].as<int>());
			}
		}
		// Manage property name
		if (node["name"]) {
			if (node["name"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Variable>();
				result->set_name(node["name"].as<std::string>());
			}
		}

		// Manage Child lengths
		if (node["lengths"]) {
			const YAML::Node node_lengths = node["lengths"];
			// Fill the list of children
			AST::Length::ListPtr lengths_list (new AST::Length::List);
			if (node_lengths.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_lengths.begin(); it !=  node_lengths.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						AST::Length::Ptr child_cast = AST::cast_to<AST::Length>(child);
						lengths_list->push_back(child_cast);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_lengths);
				if (child) {
					AST::Length::Ptr child_cast = AST::cast_to<AST::Length>(child);
					lengths_list->push_back(child_cast);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::Variable>();
			result->set_lengths(lengths_list);
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				AST::Rvalue::Ptr child_cast = AST::cast_to<AST::Rvalue>(child);
				if(!result) result = std::make_shared<AST::Variable>();
				result->set_right(child_cast);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Variable>(result);
}


AST::Node::Ptr YAMLImporter::convert_net(const YAML::Node node) const {
	AST::Net::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Net>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Net>();
				result->set_line(node["line"].as<int>());
			}
		}
		// Manage property sign
		if (node["sign"]) {
			if (node["sign"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Net>();
				result->set_sign(node["sign"].as<bool>());
			}
		}
		// Manage property name
		if (node["name"]) {
			if (node["name"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Net>();
				result->set_name(node["name"].as<std::string>());
			}
		}

		// Manage Child widths
		if (node["widths"]) {
			const YAML::Node node_widths = node["widths"];
			// Fill the list of children
			AST::Width::ListPtr widths_list (new AST::Width::List);
			if (node_widths.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_widths.begin(); it !=  node_widths.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						AST::Width::Ptr child_cast = AST::cast_to<AST::Width>(child);
						widths_list->push_back(child_cast);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_widths);
				if (child) {
					AST::Width::Ptr child_cast = AST::cast_to<AST::Width>(child);
					widths_list->push_back(child_cast);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::Net>();
			result->set_widths(widths_list);
		}

		// Manage Child ldelay
		if (node["ldelay"]) {
			const YAML::Node node_ldelay = node["ldelay"];
			// Set the child
			AST::Node::Ptr child = convert(node_ldelay);
			if (child) {
				AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
				if(!result) result = std::make_shared<AST::Net>();
				result->set_ldelay(child_cast);
			}
		}

		// Manage Child rdelay
		if (node["rdelay"]) {
			const YAML::Node node_rdelay = node["rdelay"];
			// Set the child
			AST::Node::Ptr child = convert(node_rdelay);
			if (child) {
				AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
				if(!result) result = std::make_shared<AST::Net>();
				result->set_rdelay(child_cast);
			}
		}

		// Manage Child lengths
		if (node["lengths"]) {
			const YAML::Node node_lengths = node["lengths"];
			// Fill the list of children
			AST::Length::ListPtr lengths_list (new AST::Length::List);
			if (node_lengths.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_lengths.begin(); it !=  node_lengths.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						AST::Length::Ptr child_cast = AST::cast_to<AST::Length>(child);
						lengths_list->push_back(child_cast);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_lengths);
				if (child) {
					AST::Length::Ptr child_cast = AST::cast_to<AST::Length>(child);
					lengths_list->push_back(child_cast);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::Net>();
			result->set_lengths(lengths_list);
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				AST::Rvalue::Ptr child_cast = AST::cast_to<AST::Rvalue>(child);
				if(!result) result = std::make_shared<AST::Net>();
				result->set_right(child_cast);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Net>(result);
}


AST::Node::Ptr YAMLImporter::convert_integer(const YAML::Node node) const {
	AST::Integer::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Integer>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Integer>();
				result->set_line(node["line"].as<int>());
			}
		}
		// Manage property name
		if (node["name"]) {
			if (node["name"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Integer>();
				result->set_name(node["name"].as<std::string>());
			}
		}

		// Manage Child lengths
		if (node["lengths"]) {
			const YAML::Node node_lengths = node["lengths"];
			// Fill the list of children
			AST::Length::ListPtr lengths_list (new AST::Length::List);
			if (node_lengths.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_lengths.begin(); it !=  node_lengths.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						AST::Length::Ptr child_cast = AST::cast_to<AST::Length>(child);
						lengths_list->push_back(child_cast);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_lengths);
				if (child) {
					AST::Length::Ptr child_cast = AST::cast_to<AST::Length>(child);
					lengths_list->push_back(child_cast);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::Integer>();
			result->set_lengths(lengths_list);
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				AST::Rvalue::Ptr child_cast = AST::cast_to<AST::Rvalue>(child);
				if(!result) result = std::make_shared<AST::Integer>();
				result->set_right(child_cast);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Integer>(result);
}


AST::Node::Ptr YAMLImporter::convert_real(const YAML::Node node) const {
	AST::Real::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Real>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Real>();
				result->set_line(node["line"].as<int>());
			}
		}
		// Manage property name
		if (node["name"]) {
			if (node["name"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Real>();
				result->set_name(node["name"].as<std::string>());
			}
		}

		// Manage Child lengths
		if (node["lengths"]) {
			const YAML::Node node_lengths = node["lengths"];
			// Fill the list of children
			AST::Length::ListPtr lengths_list (new AST::Length::List);
			if (node_lengths.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_lengths.begin(); it !=  node_lengths.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						AST::Length::Ptr child_cast = AST::cast_to<AST::Length>(child);
						lengths_list->push_back(child_cast);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_lengths);
				if (child) {
					AST::Length::Ptr child_cast = AST::cast_to<AST::Length>(child);
					lengths_list->push_back(child_cast);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::Real>();
			result->set_lengths(lengths_list);
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				AST::Rvalue::Ptr child_cast = AST::cast_to<AST::Rvalue>(child);
				if(!result) result = std::make_shared<AST::Real>();
				result->set_right(child_cast);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Real>(result);
}


AST::Node::Ptr YAMLImporter::convert_tri(const YAML::Node node) const {
	AST::Tri::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Tri>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Tri>();
				result->set_line(node["line"].as<int>());
			}
		}
		// Manage property sign
		if (node["sign"]) {
			if (node["sign"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Tri>();
				result->set_sign(node["sign"].as<bool>());
			}
		}
		// Manage property name
		if (node["name"]) {
			if (node["name"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Tri>();
				result->set_name(node["name"].as<std::string>());
			}
		}

		// Manage Child widths
		if (node["widths"]) {
			const YAML::Node node_widths = node["widths"];
			// Fill the list of children
			AST::Width::ListPtr widths_list (new AST::Width::List);
			if (node_widths.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_widths.begin(); it !=  node_widths.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						AST::Width::Ptr child_cast = AST::cast_to<AST::Width>(child);
						widths_list->push_back(child_cast);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_widths);
				if (child) {
					AST::Width::Ptr child_cast = AST::cast_to<AST::Width>(child);
					widths_list->push_back(child_cast);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::Tri>();
			result->set_widths(widths_list);
		}

		// Manage Child ldelay
		if (node["ldelay"]) {
			const YAML::Node node_ldelay = node["ldelay"];
			// Set the child
			AST::Node::Ptr child = convert(node_ldelay);
			if (child) {
				AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
				if(!result) result = std::make_shared<AST::Tri>();
				result->set_ldelay(child_cast);
			}
		}

		// Manage Child rdelay
		if (node["rdelay"]) {
			const YAML::Node node_rdelay = node["rdelay"];
			// Set the child
			AST::Node::Ptr child = convert(node_rdelay);
			if (child) {
				AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
				if(!result) result = std::make_shared<AST::Tri>();
				result->set_rdelay(child_cast);
			}
		}

		// Manage Child lengths
		if (node["lengths"]) {
			const YAML::Node node_lengths = node["lengths"];
			// Fill the list of children
			AST::Length::ListPtr lengths_list (new AST::Length::List);
			if (node_lengths.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_lengths.begin(); it !=  node_lengths.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						AST::Length::Ptr child_cast = AST::cast_to<AST::Length>(child);
						lengths_list->push_back(child_cast);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_lengths);
				if (child) {
					AST::Length::Ptr child_cast = AST::cast_to<AST::Length>(child);
					lengths_list->push_back(child_cast);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::Tri>();
			result->set_lengths(lengths_list);
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				AST::Rvalue::Ptr child_cast = AST::cast_to<AST::Rvalue>(child);
				if(!result) result = std::make_shared<AST::Tri>();
				result->set_right(child_cast);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Tri>(result);
}


AST::Node::Ptr YAMLImporter::convert_wire(const YAML::Node node) const {
	AST::Wire::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Wire>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Wire>();
				result->set_line(node["line"].as<int>());
			}
		}
		// Manage property sign
		if (node["sign"]) {
			if (node["sign"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Wire>();
				result->set_sign(node["sign"].as<bool>());
			}
		}
		// Manage property name
		if (node["name"]) {
			if (node["name"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Wire>();
				result->set_name(node["name"].as<std::string>());
			}
		}

		// Manage Child widths
		if (node["widths"]) {
			const YAML::Node node_widths = node["widths"];
			// Fill the list of children
			AST::Width::ListPtr widths_list (new AST::Width::List);
			if (node_widths.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_widths.begin(); it !=  node_widths.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						AST::Width::Ptr child_cast = AST::cast_to<AST::Width>(child);
						widths_list->push_back(child_cast);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_widths);
				if (child) {
					AST::Width::Ptr child_cast = AST::cast_to<AST::Width>(child);
					widths_list->push_back(child_cast);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::Wire>();
			result->set_widths(widths_list);
		}

		// Manage Child ldelay
		if (node["ldelay"]) {
			const YAML::Node node_ldelay = node["ldelay"];
			// Set the child
			AST::Node::Ptr child = convert(node_ldelay);
			if (child) {
				AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
				if(!result) result = std::make_shared<AST::Wire>();
				result->set_ldelay(child_cast);
			}
		}

		// Manage Child rdelay
		if (node["rdelay"]) {
			const YAML::Node node_rdelay = node["rdelay"];
			// Set the child
			AST::Node::Ptr child = convert(node_rdelay);
			if (child) {
				AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
				if(!result) result = std::make_shared<AST::Wire>();
				result->set_rdelay(child_cast);
			}
		}

		// Manage Child lengths
		if (node["lengths"]) {
			const YAML::Node node_lengths = node["lengths"];
			// Fill the list of children
			AST::Length::ListPtr lengths_list (new AST::Length::List);
			if (node_lengths.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_lengths.begin(); it !=  node_lengths.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						AST::Length::Ptr child_cast = AST::cast_to<AST::Length>(child);
						lengths_list->push_back(child_cast);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_lengths);
				if (child) {
					AST::Length::Ptr child_cast = AST::cast_to<AST::Length>(child);
					lengths_list->push_back(child_cast);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::Wire>();
			result->set_lengths(lengths_list);
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				AST::Rvalue::Ptr child_cast = AST::cast_to<AST::Rvalue>(child);
				if(!result) result = std::make_shared<AST::Wire>();
				result->set_right(child_cast);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Wire>(result);
}


AST::Node::Ptr YAMLImporter::convert_supply0(const YAML::Node node) const {
	AST::Supply0::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Supply0>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Supply0>();
				result->set_line(node["line"].as<int>());
			}
		}
		// Manage property sign
		if (node["sign"]) {
			if (node["sign"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Supply0>();
				result->set_sign(node["sign"].as<bool>());
			}
		}
		// Manage property name
		if (node["name"]) {
			if (node["name"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Supply0>();
				result->set_name(node["name"].as<std::string>());
			}
		}

		// Manage Child widths
		if (node["widths"]) {
			const YAML::Node node_widths = node["widths"];
			// Fill the list of children
			AST::Width::ListPtr widths_list (new AST::Width::List);
			if (node_widths.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_widths.begin(); it !=  node_widths.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						AST::Width::Ptr child_cast = AST::cast_to<AST::Width>(child);
						widths_list->push_back(child_cast);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_widths);
				if (child) {
					AST::Width::Ptr child_cast = AST::cast_to<AST::Width>(child);
					widths_list->push_back(child_cast);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::Supply0>();
			result->set_widths(widths_list);
		}

		// Manage Child ldelay
		if (node["ldelay"]) {
			const YAML::Node node_ldelay = node["ldelay"];
			// Set the child
			AST::Node::Ptr child = convert(node_ldelay);
			if (child) {
				AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
				if(!result) result = std::make_shared<AST::Supply0>();
				result->set_ldelay(child_cast);
			}
		}

		// Manage Child rdelay
		if (node["rdelay"]) {
			const YAML::Node node_rdelay = node["rdelay"];
			// Set the child
			AST::Node::Ptr child = convert(node_rdelay);
			if (child) {
				AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
				if(!result) result = std::make_shared<AST::Supply0>();
				result->set_rdelay(child_cast);
			}
		}

		// Manage Child lengths
		if (node["lengths"]) {
			const YAML::Node node_lengths = node["lengths"];
			// Fill the list of children
			AST::Length::ListPtr lengths_list (new AST::Length::List);
			if (node_lengths.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_lengths.begin(); it !=  node_lengths.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						AST::Length::Ptr child_cast = AST::cast_to<AST::Length>(child);
						lengths_list->push_back(child_cast);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_lengths);
				if (child) {
					AST::Length::Ptr child_cast = AST::cast_to<AST::Length>(child);
					lengths_list->push_back(child_cast);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::Supply0>();
			result->set_lengths(lengths_list);
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				AST::Rvalue::Ptr child_cast = AST::cast_to<AST::Rvalue>(child);
				if(!result) result = std::make_shared<AST::Supply0>();
				result->set_right(child_cast);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Supply0>(result);
}


AST::Node::Ptr YAMLImporter::convert_supply1(const YAML::Node node) const {
	AST::Supply1::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Supply1>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Supply1>();
				result->set_line(node["line"].as<int>());
			}
		}
		// Manage property sign
		if (node["sign"]) {
			if (node["sign"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Supply1>();
				result->set_sign(node["sign"].as<bool>());
			}
		}
		// Manage property name
		if (node["name"]) {
			if (node["name"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Supply1>();
				result->set_name(node["name"].as<std::string>());
			}
		}

		// Manage Child widths
		if (node["widths"]) {
			const YAML::Node node_widths = node["widths"];
			// Fill the list of children
			AST::Width::ListPtr widths_list (new AST::Width::List);
			if (node_widths.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_widths.begin(); it !=  node_widths.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						AST::Width::Ptr child_cast = AST::cast_to<AST::Width>(child);
						widths_list->push_back(child_cast);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_widths);
				if (child) {
					AST::Width::Ptr child_cast = AST::cast_to<AST::Width>(child);
					widths_list->push_back(child_cast);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::Supply1>();
			result->set_widths(widths_list);
		}

		// Manage Child ldelay
		if (node["ldelay"]) {
			const YAML::Node node_ldelay = node["ldelay"];
			// Set the child
			AST::Node::Ptr child = convert(node_ldelay);
			if (child) {
				AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
				if(!result) result = std::make_shared<AST::Supply1>();
				result->set_ldelay(child_cast);
			}
		}

		// Manage Child rdelay
		if (node["rdelay"]) {
			const YAML::Node node_rdelay = node["rdelay"];
			// Set the child
			AST::Node::Ptr child = convert(node_rdelay);
			if (child) {
				AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
				if(!result) result = std::make_shared<AST::Supply1>();
				result->set_rdelay(child_cast);
			}
		}

		// Manage Child lengths
		if (node["lengths"]) {
			const YAML::Node node_lengths = node["lengths"];
			// Fill the list of children
			AST::Length::ListPtr lengths_list (new AST::Length::List);
			if (node_lengths.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_lengths.begin(); it !=  node_lengths.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						AST::Length::Ptr child_cast = AST::cast_to<AST::Length>(child);
						lengths_list->push_back(child_cast);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_lengths);
				if (child) {
					AST::Length::Ptr child_cast = AST::cast_to<AST::Length>(child);
					lengths_list->push_back(child_cast);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::Supply1>();
			result->set_lengths(lengths_list);
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				AST::Rvalue::Ptr child_cast = AST::cast_to<AST::Rvalue>(child);
				if(!result) result = std::make_shared<AST::Supply1>();
				result->set_right(child_cast);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Supply1>(result);
}


AST::Node::Ptr YAMLImporter::convert_reg(const YAML::Node node) const {
	AST::Reg::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Reg>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Reg>();
				result->set_line(node["line"].as<int>());
			}
		}
		// Manage property sign
		if (node["sign"]) {
			if (node["sign"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Reg>();
				result->set_sign(node["sign"].as<bool>());
			}
		}
		// Manage property name
		if (node["name"]) {
			if (node["name"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Reg>();
				result->set_name(node["name"].as<std::string>());
			}
		}

		// Manage Child widths
		if (node["widths"]) {
			const YAML::Node node_widths = node["widths"];
			// Fill the list of children
			AST::Width::ListPtr widths_list (new AST::Width::List);
			if (node_widths.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_widths.begin(); it !=  node_widths.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						AST::Width::Ptr child_cast = AST::cast_to<AST::Width>(child);
						widths_list->push_back(child_cast);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_widths);
				if (child) {
					AST::Width::Ptr child_cast = AST::cast_to<AST::Width>(child);
					widths_list->push_back(child_cast);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::Reg>();
			result->set_widths(widths_list);
		}

		// Manage Child lengths
		if (node["lengths"]) {
			const YAML::Node node_lengths = node["lengths"];
			// Fill the list of children
			AST::Length::ListPtr lengths_list (new AST::Length::List);
			if (node_lengths.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_lengths.begin(); it !=  node_lengths.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						AST::Length::Ptr child_cast = AST::cast_to<AST::Length>(child);
						lengths_list->push_back(child_cast);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_lengths);
				if (child) {
					AST::Length::Ptr child_cast = AST::cast_to<AST::Length>(child);
					lengths_list->push_back(child_cast);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::Reg>();
			result->set_lengths(lengths_list);
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				AST::Rvalue::Ptr child_cast = AST::cast_to<AST::Rvalue>(child);
				if(!result) result = std::make_shared<AST::Reg>();
				result->set_right(child_cast);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Reg>(result);
}


AST::Node::Ptr YAMLImporter::convert_ioport(const YAML::Node node) const {
	AST::Ioport::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Ioport>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Ioport>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child first
		if (node["first"]) {
			const YAML::Node node_first = node["first"];
			// Set the child
			AST::Node::Ptr child = convert(node_first);
			if (child) {
				AST::IODir::Ptr child_cast = AST::cast_to<AST::IODir>(child);
				if(!result) result = std::make_shared<AST::Ioport>();
				result->set_first(child_cast);
			}
		}

		// Manage Child second
		if (node["second"]) {
			const YAML::Node node_second = node["second"];
			// Set the child
			AST::Node::Ptr child = convert(node_second);
			if (child) {
				AST::Variable::Ptr child_cast = AST::cast_to<AST::Variable>(child);
				if(!result) result = std::make_shared<AST::Ioport>();
				result->set_second(child_cast);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Ioport>(result);
}


AST::Node::Ptr YAMLImporter::convert_parameter(const YAML::Node node) const {
	AST::Parameter::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Parameter>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Parameter>();
				result->set_line(node["line"].as<int>());
			}
		}
		// Manage property name
		if (node["name"]) {
			if (node["name"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Parameter>();
				result->set_name(node["name"].as<std::string>());
			}
		}
		// Manage property sign
		if (node["sign"]) {
			if (node["sign"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Parameter>();
				result->set_sign(node["sign"].as<bool>());
			}
		}
		// Manage property type
		if (node["type"]) {
			if (node["type"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Parameter>();
				result->set_type(node["type"].as<AST::Parameter::TypeEnum>());
			}
		}

		// Manage Child value
		if (node["value"]) {
			const YAML::Node node_value = node["value"];
			// Set the child
			AST::Node::Ptr child = convert(node_value);
			if (child) {
				if(!result) result = std::make_shared<AST::Parameter>();
				result->set_value(child);
			}
		}

		// Manage Child widths
		if (node["widths"]) {
			const YAML::Node node_widths = node["widths"];
			// Fill the list of children
			AST::Width::ListPtr widths_list (new AST::Width::List);
			if (node_widths.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_widths.begin(); it !=  node_widths.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						AST::Width::Ptr child_cast = AST::cast_to<AST::Width>(child);
						widths_list->push_back(child_cast);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_widths);
				if (child) {
					AST::Width::Ptr child_cast = AST::cast_to<AST::Width>(child);
					widths_list->push_back(child_cast);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::Parameter>();
			result->set_widths(widths_list);
		}
	}

	// Return the result
	return AST::cast_to<AST::Parameter>(result);
}


AST::Node::Ptr YAMLImporter::convert_localparam(const YAML::Node node) const {
	AST::Localparam::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Localparam>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Localparam>();
				result->set_line(node["line"].as<int>());
			}
		}
		// Manage property name
		if (node["name"]) {
			if (node["name"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Localparam>();
				result->set_name(node["name"].as<std::string>());
			}
		}
		// Manage property sign
		if (node["sign"]) {
			if (node["sign"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Localparam>();
				result->set_sign(node["sign"].as<bool>());
			}
		}
		// Manage property type
		if (node["type"]) {
			if (node["type"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Localparam>();
				result->set_type(node["type"].as<AST::Parameter::TypeEnum>());
			}
		}

		// Manage Child value
		if (node["value"]) {
			const YAML::Node node_value = node["value"];
			// Set the child
			AST::Node::Ptr child = convert(node_value);
			if (child) {
				if(!result) result = std::make_shared<AST::Localparam>();
				result->set_value(child);
			}
		}

		// Manage Child widths
		if (node["widths"]) {
			const YAML::Node node_widths = node["widths"];
			// Fill the list of children
			AST::Width::ListPtr widths_list (new AST::Width::List);
			if (node_widths.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_widths.begin(); it !=  node_widths.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						AST::Width::Ptr child_cast = AST::cast_to<AST::Width>(child);
						widths_list->push_back(child_cast);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_widths);
				if (child) {
					AST::Width::Ptr child_cast = AST::cast_to<AST::Width>(child);
					widths_list->push_back(child_cast);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::Localparam>();
			result->set_widths(widths_list);
		}
	}

	// Return the result
	return AST::cast_to<AST::Localparam>(result);
}


AST::Node::Ptr YAMLImporter::convert_concat(const YAML::Node node) const {
	AST::Concat::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Concat>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Concat>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child list
		if (node["list"]) {
			const YAML::Node node_list = node["list"];
			// Fill the list of children
			AST::Node::ListPtr list_list (new AST::Node::List);
			if (node_list.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_list.begin(); it !=  node_list.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						list_list->push_back(child);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_list);
				if (child) {
					list_list->push_back(child);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::Concat>();
			result->set_list(list_list);
		}
	}

	// Return the result
	return AST::cast_to<AST::Concat>(result);
}


AST::Node::Ptr YAMLImporter::convert_lconcat(const YAML::Node node) const {
	AST::Lconcat::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Lconcat>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Lconcat>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child list
		if (node["list"]) {
			const YAML::Node node_list = node["list"];
			// Fill the list of children
			AST::Node::ListPtr list_list (new AST::Node::List);
			if (node_list.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_list.begin(); it !=  node_list.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						list_list->push_back(child);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_list);
				if (child) {
					list_list->push_back(child);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::Lconcat>();
			result->set_list(list_list);
		}
	}

	// Return the result
	return AST::cast_to<AST::Lconcat>(result);
}


AST::Node::Ptr YAMLImporter::convert_repeat(const YAML::Node node) const {
	AST::Repeat::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Repeat>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Repeat>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child value
		if (node["value"]) {
			const YAML::Node node_value = node["value"];
			// Set the child
			AST::Node::Ptr child = convert(node_value);
			if (child) {
				if(!result) result = std::make_shared<AST::Repeat>();
				result->set_value(child);
			}
		}

		// Manage Child times
		if (node["times"]) {
			const YAML::Node node_times = node["times"];
			// Set the child
			AST::Node::Ptr child = convert(node_times);
			if (child) {
				if(!result) result = std::make_shared<AST::Repeat>();
				result->set_times(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Repeat>(result);
}


AST::Node::Ptr YAMLImporter::convert_indirect(const YAML::Node node) const {
	AST::Indirect::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Indirect>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Indirect>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child var
		if (node["var"]) {
			const YAML::Node node_var = node["var"];
			// Set the child
			AST::Node::Ptr child = convert(node_var);
			if (child) {
				if(!result) result = std::make_shared<AST::Indirect>();
				result->set_var(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Indirect>(result);
}


AST::Node::Ptr YAMLImporter::convert_partselect(const YAML::Node node) const {
	AST::Partselect::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Partselect>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Partselect>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child msb
		if (node["msb"]) {
			const YAML::Node node_msb = node["msb"];
			// Set the child
			AST::Node::Ptr child = convert(node_msb);
			if (child) {
				if(!result) result = std::make_shared<AST::Partselect>();
				result->set_msb(child);
			}
		}

		// Manage Child lsb
		if (node["lsb"]) {
			const YAML::Node node_lsb = node["lsb"];
			// Set the child
			AST::Node::Ptr child = convert(node_lsb);
			if (child) {
				if(!result) result = std::make_shared<AST::Partselect>();
				result->set_lsb(child);
			}
		}

		// Manage Child var
		if (node["var"]) {
			const YAML::Node node_var = node["var"];
			// Set the child
			AST::Node::Ptr child = convert(node_var);
			if (child) {
				if(!result) result = std::make_shared<AST::Partselect>();
				result->set_var(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Partselect>(result);
}


AST::Node::Ptr YAMLImporter::convert_partselectindexed(const YAML::Node node) const {
	AST::PartselectIndexed::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::PartselectIndexed>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::PartselectIndexed>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child index
		if (node["index"]) {
			const YAML::Node node_index = node["index"];
			// Set the child
			AST::Node::Ptr child = convert(node_index);
			if (child) {
				if(!result) result = std::make_shared<AST::PartselectIndexed>();
				result->set_index(child);
			}
		}

		// Manage Child size
		if (node["size"]) {
			const YAML::Node node_size = node["size"];
			// Set the child
			AST::Node::Ptr child = convert(node_size);
			if (child) {
				if(!result) result = std::make_shared<AST::PartselectIndexed>();
				result->set_size(child);
			}
		}

		// Manage Child var
		if (node["var"]) {
			const YAML::Node node_var = node["var"];
			// Set the child
			AST::Node::Ptr child = convert(node_var);
			if (child) {
				if(!result) result = std::make_shared<AST::PartselectIndexed>();
				result->set_var(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::PartselectIndexed>(result);
}


AST::Node::Ptr YAMLImporter::convert_partselectplusindexed(const YAML::Node node) const {
	AST::PartselectPlusIndexed::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::PartselectPlusIndexed>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::PartselectPlusIndexed>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child index
		if (node["index"]) {
			const YAML::Node node_index = node["index"];
			// Set the child
			AST::Node::Ptr child = convert(node_index);
			if (child) {
				if(!result) result = std::make_shared<AST::PartselectPlusIndexed>();
				result->set_index(child);
			}
		}

		// Manage Child size
		if (node["size"]) {
			const YAML::Node node_size = node["size"];
			// Set the child
			AST::Node::Ptr child = convert(node_size);
			if (child) {
				if(!result) result = std::make_shared<AST::PartselectPlusIndexed>();
				result->set_size(child);
			}
		}

		// Manage Child var
		if (node["var"]) {
			const YAML::Node node_var = node["var"];
			// Set the child
			AST::Node::Ptr child = convert(node_var);
			if (child) {
				if(!result) result = std::make_shared<AST::PartselectPlusIndexed>();
				result->set_var(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::PartselectPlusIndexed>(result);
}


AST::Node::Ptr YAMLImporter::convert_partselectminusindexed(const YAML::Node node) const {
	AST::PartselectMinusIndexed::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::PartselectMinusIndexed>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::PartselectMinusIndexed>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child index
		if (node["index"]) {
			const YAML::Node node_index = node["index"];
			// Set the child
			AST::Node::Ptr child = convert(node_index);
			if (child) {
				if(!result) result = std::make_shared<AST::PartselectMinusIndexed>();
				result->set_index(child);
			}
		}

		// Manage Child size
		if (node["size"]) {
			const YAML::Node node_size = node["size"];
			// Set the child
			AST::Node::Ptr child = convert(node_size);
			if (child) {
				if(!result) result = std::make_shared<AST::PartselectMinusIndexed>();
				result->set_size(child);
			}
		}

		// Manage Child var
		if (node["var"]) {
			const YAML::Node node_var = node["var"];
			// Set the child
			AST::Node::Ptr child = convert(node_var);
			if (child) {
				if(!result) result = std::make_shared<AST::PartselectMinusIndexed>();
				result->set_var(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::PartselectMinusIndexed>(result);
}


AST::Node::Ptr YAMLImporter::convert_pointer(const YAML::Node node) const {
	AST::Pointer::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Pointer>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Pointer>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child ptr
		if (node["ptr"]) {
			const YAML::Node node_ptr = node["ptr"];
			// Set the child
			AST::Node::Ptr child = convert(node_ptr);
			if (child) {
				if(!result) result = std::make_shared<AST::Pointer>();
				result->set_ptr(child);
			}
		}

		// Manage Child var
		if (node["var"]) {
			const YAML::Node node_var = node["var"];
			// Set the child
			AST::Node::Ptr child = convert(node_var);
			if (child) {
				if(!result) result = std::make_shared<AST::Pointer>();
				result->set_var(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Pointer>(result);
}


AST::Node::Ptr YAMLImporter::convert_lvalue(const YAML::Node node) const {
	AST::Lvalue::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Lvalue>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Lvalue>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child var
		if (node["var"]) {
			const YAML::Node node_var = node["var"];
			// Set the child
			AST::Node::Ptr child = convert(node_var);
			if (child) {
				if(!result) result = std::make_shared<AST::Lvalue>();
				result->set_var(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Lvalue>(result);
}


AST::Node::Ptr YAMLImporter::convert_rvalue(const YAML::Node node) const {
	AST::Rvalue::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Rvalue>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Rvalue>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child var
		if (node["var"]) {
			const YAML::Node node_var = node["var"];
			// Set the child
			AST::Node::Ptr child = convert(node_var);
			if (child) {
				if(!result) result = std::make_shared<AST::Rvalue>();
				result->set_var(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Rvalue>(result);
}


AST::Node::Ptr YAMLImporter::convert_unaryoperator(const YAML::Node node) const {
	AST::UnaryOperator::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::UnaryOperator>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::UnaryOperator>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				if(!result) result = std::make_shared<AST::UnaryOperator>();
				result->set_right(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::UnaryOperator>(result);
}


AST::Node::Ptr YAMLImporter::convert_uplus(const YAML::Node node) const {
	AST::Uplus::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Uplus>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Uplus>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				if(!result) result = std::make_shared<AST::Uplus>();
				result->set_right(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Uplus>(result);
}


AST::Node::Ptr YAMLImporter::convert_uminus(const YAML::Node node) const {
	AST::Uminus::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Uminus>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Uminus>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				if(!result) result = std::make_shared<AST::Uminus>();
				result->set_right(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Uminus>(result);
}


AST::Node::Ptr YAMLImporter::convert_ulnot(const YAML::Node node) const {
	AST::Ulnot::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Ulnot>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Ulnot>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				if(!result) result = std::make_shared<AST::Ulnot>();
				result->set_right(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Ulnot>(result);
}


AST::Node::Ptr YAMLImporter::convert_unot(const YAML::Node node) const {
	AST::Unot::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Unot>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Unot>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				if(!result) result = std::make_shared<AST::Unot>();
				result->set_right(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Unot>(result);
}


AST::Node::Ptr YAMLImporter::convert_uand(const YAML::Node node) const {
	AST::Uand::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Uand>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Uand>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				if(!result) result = std::make_shared<AST::Uand>();
				result->set_right(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Uand>(result);
}


AST::Node::Ptr YAMLImporter::convert_unand(const YAML::Node node) const {
	AST::Unand::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Unand>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Unand>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				if(!result) result = std::make_shared<AST::Unand>();
				result->set_right(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Unand>(result);
}


AST::Node::Ptr YAMLImporter::convert_uor(const YAML::Node node) const {
	AST::Uor::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Uor>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Uor>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				if(!result) result = std::make_shared<AST::Uor>();
				result->set_right(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Uor>(result);
}


AST::Node::Ptr YAMLImporter::convert_unor(const YAML::Node node) const {
	AST::Unor::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Unor>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Unor>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				if(!result) result = std::make_shared<AST::Unor>();
				result->set_right(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Unor>(result);
}


AST::Node::Ptr YAMLImporter::convert_uxor(const YAML::Node node) const {
	AST::Uxor::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Uxor>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Uxor>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				if(!result) result = std::make_shared<AST::Uxor>();
				result->set_right(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Uxor>(result);
}


AST::Node::Ptr YAMLImporter::convert_uxnor(const YAML::Node node) const {
	AST::Uxnor::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Uxnor>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Uxnor>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				if(!result) result = std::make_shared<AST::Uxnor>();
				result->set_right(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Uxnor>(result);
}


AST::Node::Ptr YAMLImporter::convert_operator(const YAML::Node node) const {
	AST::Operator::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Operator>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Operator>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child left
		if (node["left"]) {
			const YAML::Node node_left = node["left"];
			// Set the child
			AST::Node::Ptr child = convert(node_left);
			if (child) {
				if(!result) result = std::make_shared<AST::Operator>();
				result->set_left(child);
			}
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				if(!result) result = std::make_shared<AST::Operator>();
				result->set_right(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Operator>(result);
}


AST::Node::Ptr YAMLImporter::convert_power(const YAML::Node node) const {
	AST::Power::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Power>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Power>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child left
		if (node["left"]) {
			const YAML::Node node_left = node["left"];
			// Set the child
			AST::Node::Ptr child = convert(node_left);
			if (child) {
				if(!result) result = std::make_shared<AST::Power>();
				result->set_left(child);
			}
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				if(!result) result = std::make_shared<AST::Power>();
				result->set_right(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Power>(result);
}


AST::Node::Ptr YAMLImporter::convert_times(const YAML::Node node) const {
	AST::Times::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Times>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Times>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child left
		if (node["left"]) {
			const YAML::Node node_left = node["left"];
			// Set the child
			AST::Node::Ptr child = convert(node_left);
			if (child) {
				if(!result) result = std::make_shared<AST::Times>();
				result->set_left(child);
			}
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				if(!result) result = std::make_shared<AST::Times>();
				result->set_right(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Times>(result);
}


AST::Node::Ptr YAMLImporter::convert_divide(const YAML::Node node) const {
	AST::Divide::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Divide>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Divide>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child left
		if (node["left"]) {
			const YAML::Node node_left = node["left"];
			// Set the child
			AST::Node::Ptr child = convert(node_left);
			if (child) {
				if(!result) result = std::make_shared<AST::Divide>();
				result->set_left(child);
			}
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				if(!result) result = std::make_shared<AST::Divide>();
				result->set_right(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Divide>(result);
}


AST::Node::Ptr YAMLImporter::convert_mod(const YAML::Node node) const {
	AST::Mod::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Mod>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Mod>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child left
		if (node["left"]) {
			const YAML::Node node_left = node["left"];
			// Set the child
			AST::Node::Ptr child = convert(node_left);
			if (child) {
				if(!result) result = std::make_shared<AST::Mod>();
				result->set_left(child);
			}
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				if(!result) result = std::make_shared<AST::Mod>();
				result->set_right(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Mod>(result);
}


AST::Node::Ptr YAMLImporter::convert_plus(const YAML::Node node) const {
	AST::Plus::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Plus>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Plus>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child left
		if (node["left"]) {
			const YAML::Node node_left = node["left"];
			// Set the child
			AST::Node::Ptr child = convert(node_left);
			if (child) {
				if(!result) result = std::make_shared<AST::Plus>();
				result->set_left(child);
			}
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				if(!result) result = std::make_shared<AST::Plus>();
				result->set_right(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Plus>(result);
}


AST::Node::Ptr YAMLImporter::convert_minus(const YAML::Node node) const {
	AST::Minus::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Minus>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Minus>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child left
		if (node["left"]) {
			const YAML::Node node_left = node["left"];
			// Set the child
			AST::Node::Ptr child = convert(node_left);
			if (child) {
				if(!result) result = std::make_shared<AST::Minus>();
				result->set_left(child);
			}
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				if(!result) result = std::make_shared<AST::Minus>();
				result->set_right(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Minus>(result);
}


AST::Node::Ptr YAMLImporter::convert_sll(const YAML::Node node) const {
	AST::Sll::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Sll>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Sll>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child left
		if (node["left"]) {
			const YAML::Node node_left = node["left"];
			// Set the child
			AST::Node::Ptr child = convert(node_left);
			if (child) {
				if(!result) result = std::make_shared<AST::Sll>();
				result->set_left(child);
			}
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				if(!result) result = std::make_shared<AST::Sll>();
				result->set_right(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Sll>(result);
}


AST::Node::Ptr YAMLImporter::convert_srl(const YAML::Node node) const {
	AST::Srl::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Srl>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Srl>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child left
		if (node["left"]) {
			const YAML::Node node_left = node["left"];
			// Set the child
			AST::Node::Ptr child = convert(node_left);
			if (child) {
				if(!result) result = std::make_shared<AST::Srl>();
				result->set_left(child);
			}
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				if(!result) result = std::make_shared<AST::Srl>();
				result->set_right(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Srl>(result);
}


AST::Node::Ptr YAMLImporter::convert_sra(const YAML::Node node) const {
	AST::Sra::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Sra>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Sra>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child left
		if (node["left"]) {
			const YAML::Node node_left = node["left"];
			// Set the child
			AST::Node::Ptr child = convert(node_left);
			if (child) {
				if(!result) result = std::make_shared<AST::Sra>();
				result->set_left(child);
			}
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				if(!result) result = std::make_shared<AST::Sra>();
				result->set_right(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Sra>(result);
}


AST::Node::Ptr YAMLImporter::convert_lessthan(const YAML::Node node) const {
	AST::LessThan::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::LessThan>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::LessThan>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child left
		if (node["left"]) {
			const YAML::Node node_left = node["left"];
			// Set the child
			AST::Node::Ptr child = convert(node_left);
			if (child) {
				if(!result) result = std::make_shared<AST::LessThan>();
				result->set_left(child);
			}
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				if(!result) result = std::make_shared<AST::LessThan>();
				result->set_right(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::LessThan>(result);
}


AST::Node::Ptr YAMLImporter::convert_greaterthan(const YAML::Node node) const {
	AST::GreaterThan::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::GreaterThan>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::GreaterThan>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child left
		if (node["left"]) {
			const YAML::Node node_left = node["left"];
			// Set the child
			AST::Node::Ptr child = convert(node_left);
			if (child) {
				if(!result) result = std::make_shared<AST::GreaterThan>();
				result->set_left(child);
			}
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				if(!result) result = std::make_shared<AST::GreaterThan>();
				result->set_right(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::GreaterThan>(result);
}


AST::Node::Ptr YAMLImporter::convert_lesseq(const YAML::Node node) const {
	AST::LessEq::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::LessEq>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::LessEq>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child left
		if (node["left"]) {
			const YAML::Node node_left = node["left"];
			// Set the child
			AST::Node::Ptr child = convert(node_left);
			if (child) {
				if(!result) result = std::make_shared<AST::LessEq>();
				result->set_left(child);
			}
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				if(!result) result = std::make_shared<AST::LessEq>();
				result->set_right(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::LessEq>(result);
}


AST::Node::Ptr YAMLImporter::convert_greatereq(const YAML::Node node) const {
	AST::GreaterEq::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::GreaterEq>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::GreaterEq>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child left
		if (node["left"]) {
			const YAML::Node node_left = node["left"];
			// Set the child
			AST::Node::Ptr child = convert(node_left);
			if (child) {
				if(!result) result = std::make_shared<AST::GreaterEq>();
				result->set_left(child);
			}
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				if(!result) result = std::make_shared<AST::GreaterEq>();
				result->set_right(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::GreaterEq>(result);
}


AST::Node::Ptr YAMLImporter::convert_eq(const YAML::Node node) const {
	AST::Eq::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Eq>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Eq>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child left
		if (node["left"]) {
			const YAML::Node node_left = node["left"];
			// Set the child
			AST::Node::Ptr child = convert(node_left);
			if (child) {
				if(!result) result = std::make_shared<AST::Eq>();
				result->set_left(child);
			}
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				if(!result) result = std::make_shared<AST::Eq>();
				result->set_right(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Eq>(result);
}


AST::Node::Ptr YAMLImporter::convert_noteq(const YAML::Node node) const {
	AST::NotEq::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::NotEq>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::NotEq>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child left
		if (node["left"]) {
			const YAML::Node node_left = node["left"];
			// Set the child
			AST::Node::Ptr child = convert(node_left);
			if (child) {
				if(!result) result = std::make_shared<AST::NotEq>();
				result->set_left(child);
			}
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				if(!result) result = std::make_shared<AST::NotEq>();
				result->set_right(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::NotEq>(result);
}


AST::Node::Ptr YAMLImporter::convert_eql(const YAML::Node node) const {
	AST::Eql::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Eql>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Eql>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child left
		if (node["left"]) {
			const YAML::Node node_left = node["left"];
			// Set the child
			AST::Node::Ptr child = convert(node_left);
			if (child) {
				if(!result) result = std::make_shared<AST::Eql>();
				result->set_left(child);
			}
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				if(!result) result = std::make_shared<AST::Eql>();
				result->set_right(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Eql>(result);
}


AST::Node::Ptr YAMLImporter::convert_noteql(const YAML::Node node) const {
	AST::NotEql::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::NotEql>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::NotEql>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child left
		if (node["left"]) {
			const YAML::Node node_left = node["left"];
			// Set the child
			AST::Node::Ptr child = convert(node_left);
			if (child) {
				if(!result) result = std::make_shared<AST::NotEql>();
				result->set_left(child);
			}
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				if(!result) result = std::make_shared<AST::NotEql>();
				result->set_right(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::NotEql>(result);
}


AST::Node::Ptr YAMLImporter::convert_and(const YAML::Node node) const {
	AST::And::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::And>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::And>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child left
		if (node["left"]) {
			const YAML::Node node_left = node["left"];
			// Set the child
			AST::Node::Ptr child = convert(node_left);
			if (child) {
				if(!result) result = std::make_shared<AST::And>();
				result->set_left(child);
			}
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				if(!result) result = std::make_shared<AST::And>();
				result->set_right(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::And>(result);
}


AST::Node::Ptr YAMLImporter::convert_xor(const YAML::Node node) const {
	AST::Xor::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Xor>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Xor>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child left
		if (node["left"]) {
			const YAML::Node node_left = node["left"];
			// Set the child
			AST::Node::Ptr child = convert(node_left);
			if (child) {
				if(!result) result = std::make_shared<AST::Xor>();
				result->set_left(child);
			}
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				if(!result) result = std::make_shared<AST::Xor>();
				result->set_right(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Xor>(result);
}


AST::Node::Ptr YAMLImporter::convert_xnor(const YAML::Node node) const {
	AST::Xnor::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Xnor>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Xnor>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child left
		if (node["left"]) {
			const YAML::Node node_left = node["left"];
			// Set the child
			AST::Node::Ptr child = convert(node_left);
			if (child) {
				if(!result) result = std::make_shared<AST::Xnor>();
				result->set_left(child);
			}
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				if(!result) result = std::make_shared<AST::Xnor>();
				result->set_right(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Xnor>(result);
}


AST::Node::Ptr YAMLImporter::convert_or(const YAML::Node node) const {
	AST::Or::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Or>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Or>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child left
		if (node["left"]) {
			const YAML::Node node_left = node["left"];
			// Set the child
			AST::Node::Ptr child = convert(node_left);
			if (child) {
				if(!result) result = std::make_shared<AST::Or>();
				result->set_left(child);
			}
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				if(!result) result = std::make_shared<AST::Or>();
				result->set_right(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Or>(result);
}


AST::Node::Ptr YAMLImporter::convert_land(const YAML::Node node) const {
	AST::Land::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Land>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Land>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child left
		if (node["left"]) {
			const YAML::Node node_left = node["left"];
			// Set the child
			AST::Node::Ptr child = convert(node_left);
			if (child) {
				if(!result) result = std::make_shared<AST::Land>();
				result->set_left(child);
			}
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				if(!result) result = std::make_shared<AST::Land>();
				result->set_right(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Land>(result);
}


AST::Node::Ptr YAMLImporter::convert_lor(const YAML::Node node) const {
	AST::Lor::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Lor>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Lor>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child left
		if (node["left"]) {
			const YAML::Node node_left = node["left"];
			// Set the child
			AST::Node::Ptr child = convert(node_left);
			if (child) {
				if(!result) result = std::make_shared<AST::Lor>();
				result->set_left(child);
			}
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				if(!result) result = std::make_shared<AST::Lor>();
				result->set_right(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Lor>(result);
}


AST::Node::Ptr YAMLImporter::convert_cond(const YAML::Node node) const {
	AST::Cond::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Cond>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Cond>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child cond
		if (node["cond"]) {
			const YAML::Node node_cond = node["cond"];
			// Set the child
			AST::Node::Ptr child = convert(node_cond);
			if (child) {
				if(!result) result = std::make_shared<AST::Cond>();
				result->set_cond(child);
			}
		}

		// Manage Child left
		if (node["left"]) {
			const YAML::Node node_left = node["left"];
			// Set the child
			AST::Node::Ptr child = convert(node_left);
			if (child) {
				if(!result) result = std::make_shared<AST::Cond>();
				result->set_left(child);
			}
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				if(!result) result = std::make_shared<AST::Cond>();
				result->set_right(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Cond>(result);
}


AST::Node::Ptr YAMLImporter::convert_always(const YAML::Node node) const {
	AST::Always::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Always>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Always>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child senslist
		if (node["senslist"]) {
			const YAML::Node node_senslist = node["senslist"];
			// Set the child
			AST::Node::Ptr child = convert(node_senslist);
			if (child) {
				AST::Senslist::Ptr child_cast = AST::cast_to<AST::Senslist>(child);
				if(!result) result = std::make_shared<AST::Always>();
				result->set_senslist(child_cast);
			}
		}

		// Manage Child statement
		if (node["statement"]) {
			const YAML::Node node_statement = node["statement"];
			// Set the child
			AST::Node::Ptr child = convert(node_statement);
			if (child) {
				if(!result) result = std::make_shared<AST::Always>();
				result->set_statement(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Always>(result);
}


AST::Node::Ptr YAMLImporter::convert_senslist(const YAML::Node node) const {
	AST::Senslist::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Senslist>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Senslist>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child list
		if (node["list"]) {
			const YAML::Node node_list = node["list"];
			// Fill the list of children
			AST::Sens::ListPtr list_list (new AST::Sens::List);
			if (node_list.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_list.begin(); it !=  node_list.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						AST::Sens::Ptr child_cast = AST::cast_to<AST::Sens>(child);
						list_list->push_back(child_cast);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_list);
				if (child) {
					AST::Sens::Ptr child_cast = AST::cast_to<AST::Sens>(child);
					list_list->push_back(child_cast);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::Senslist>();
			result->set_list(list_list);
		}
	}

	// Return the result
	return AST::cast_to<AST::Senslist>(result);
}


AST::Node::Ptr YAMLImporter::convert_sens(const YAML::Node node) const {
	AST::Sens::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Sens>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Sens>();
				result->set_line(node["line"].as<int>());
			}
		}
		// Manage property type
		if (node["type"]) {
			if (node["type"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Sens>();
				result->set_type(node["type"].as<AST::Sens::TypeEnum>());
			}
		}

		// Manage Child sig
		if (node["sig"]) {
			const YAML::Node node_sig = node["sig"];
			// Set the child
			AST::Node::Ptr child = convert(node_sig);
			if (child) {
				if(!result) result = std::make_shared<AST::Sens>();
				result->set_sig(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Sens>(result);
}


AST::Node::Ptr YAMLImporter::convert_defparamlist(const YAML::Node node) const {
	AST::Defparamlist::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Defparamlist>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Defparamlist>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child list
		if (node["list"]) {
			const YAML::Node node_list = node["list"];
			// Fill the list of children
			AST::Defparam::ListPtr list_list (new AST::Defparam::List);
			if (node_list.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_list.begin(); it !=  node_list.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						AST::Defparam::Ptr child_cast = AST::cast_to<AST::Defparam>(child);
						list_list->push_back(child_cast);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_list);
				if (child) {
					AST::Defparam::Ptr child_cast = AST::cast_to<AST::Defparam>(child);
					list_list->push_back(child_cast);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::Defparamlist>();
			result->set_list(list_list);
		}
	}

	// Return the result
	return AST::cast_to<AST::Defparamlist>(result);
}


AST::Node::Ptr YAMLImporter::convert_defparam(const YAML::Node node) const {
	AST::Defparam::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Defparam>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Defparam>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child identifier
		if (node["identifier"]) {
			const YAML::Node node_identifier = node["identifier"];
			// Set the child
			AST::Node::Ptr child = convert(node_identifier);
			if (child) {
				AST::Identifier::Ptr child_cast = AST::cast_to<AST::Identifier>(child);
				if(!result) result = std::make_shared<AST::Defparam>();
				result->set_identifier(child_cast);
			}
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				AST::Rvalue::Ptr child_cast = AST::cast_to<AST::Rvalue>(child);
				if(!result) result = std::make_shared<AST::Defparam>();
				result->set_right(child_cast);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Defparam>(result);
}


AST::Node::Ptr YAMLImporter::convert_assign(const YAML::Node node) const {
	AST::Assign::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Assign>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Assign>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child left
		if (node["left"]) {
			const YAML::Node node_left = node["left"];
			// Set the child
			AST::Node::Ptr child = convert(node_left);
			if (child) {
				AST::Lvalue::Ptr child_cast = AST::cast_to<AST::Lvalue>(child);
				if(!result) result = std::make_shared<AST::Assign>();
				result->set_left(child_cast);
			}
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				AST::Rvalue::Ptr child_cast = AST::cast_to<AST::Rvalue>(child);
				if(!result) result = std::make_shared<AST::Assign>();
				result->set_right(child_cast);
			}
		}

		// Manage Child ldelay
		if (node["ldelay"]) {
			const YAML::Node node_ldelay = node["ldelay"];
			// Set the child
			AST::Node::Ptr child = convert(node_ldelay);
			if (child) {
				AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
				if(!result) result = std::make_shared<AST::Assign>();
				result->set_ldelay(child_cast);
			}
		}

		// Manage Child rdelay
		if (node["rdelay"]) {
			const YAML::Node node_rdelay = node["rdelay"];
			// Set the child
			AST::Node::Ptr child = convert(node_rdelay);
			if (child) {
				AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
				if(!result) result = std::make_shared<AST::Assign>();
				result->set_rdelay(child_cast);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Assign>(result);
}


AST::Node::Ptr YAMLImporter::convert_blockingsubstitution(const YAML::Node node) const {
	AST::BlockingSubstitution::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::BlockingSubstitution>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::BlockingSubstitution>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child left
		if (node["left"]) {
			const YAML::Node node_left = node["left"];
			// Set the child
			AST::Node::Ptr child = convert(node_left);
			if (child) {
				AST::Lvalue::Ptr child_cast = AST::cast_to<AST::Lvalue>(child);
				if(!result) result = std::make_shared<AST::BlockingSubstitution>();
				result->set_left(child_cast);
			}
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				AST::Rvalue::Ptr child_cast = AST::cast_to<AST::Rvalue>(child);
				if(!result) result = std::make_shared<AST::BlockingSubstitution>();
				result->set_right(child_cast);
			}
		}

		// Manage Child ldelay
		if (node["ldelay"]) {
			const YAML::Node node_ldelay = node["ldelay"];
			// Set the child
			AST::Node::Ptr child = convert(node_ldelay);
			if (child) {
				AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
				if(!result) result = std::make_shared<AST::BlockingSubstitution>();
				result->set_ldelay(child_cast);
			}
		}

		// Manage Child rdelay
		if (node["rdelay"]) {
			const YAML::Node node_rdelay = node["rdelay"];
			// Set the child
			AST::Node::Ptr child = convert(node_rdelay);
			if (child) {
				AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
				if(!result) result = std::make_shared<AST::BlockingSubstitution>();
				result->set_rdelay(child_cast);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::BlockingSubstitution>(result);
}


AST::Node::Ptr YAMLImporter::convert_nonblockingsubstitution(const YAML::Node node) const {
	AST::NonblockingSubstitution::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::NonblockingSubstitution>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::NonblockingSubstitution>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child left
		if (node["left"]) {
			const YAML::Node node_left = node["left"];
			// Set the child
			AST::Node::Ptr child = convert(node_left);
			if (child) {
				AST::Lvalue::Ptr child_cast = AST::cast_to<AST::Lvalue>(child);
				if(!result) result = std::make_shared<AST::NonblockingSubstitution>();
				result->set_left(child_cast);
			}
		}

		// Manage Child right
		if (node["right"]) {
			const YAML::Node node_right = node["right"];
			// Set the child
			AST::Node::Ptr child = convert(node_right);
			if (child) {
				AST::Rvalue::Ptr child_cast = AST::cast_to<AST::Rvalue>(child);
				if(!result) result = std::make_shared<AST::NonblockingSubstitution>();
				result->set_right(child_cast);
			}
		}

		// Manage Child ldelay
		if (node["ldelay"]) {
			const YAML::Node node_ldelay = node["ldelay"];
			// Set the child
			AST::Node::Ptr child = convert(node_ldelay);
			if (child) {
				AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
				if(!result) result = std::make_shared<AST::NonblockingSubstitution>();
				result->set_ldelay(child_cast);
			}
		}

		// Manage Child rdelay
		if (node["rdelay"]) {
			const YAML::Node node_rdelay = node["rdelay"];
			// Set the child
			AST::Node::Ptr child = convert(node_rdelay);
			if (child) {
				AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
				if(!result) result = std::make_shared<AST::NonblockingSubstitution>();
				result->set_rdelay(child_cast);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::NonblockingSubstitution>(result);
}


AST::Node::Ptr YAMLImporter::convert_ifstatement(const YAML::Node node) const {
	AST::IfStatement::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::IfStatement>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::IfStatement>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child cond
		if (node["cond"]) {
			const YAML::Node node_cond = node["cond"];
			// Set the child
			AST::Node::Ptr child = convert(node_cond);
			if (child) {
				if(!result) result = std::make_shared<AST::IfStatement>();
				result->set_cond(child);
			}
		}

		// Manage Child true_statement
		if (node["true_statement"]) {
			const YAML::Node node_true_statement = node["true_statement"];
			// Set the child
			AST::Node::Ptr child = convert(node_true_statement);
			if (child) {
				if(!result) result = std::make_shared<AST::IfStatement>();
				result->set_true_statement(child);
			}
		}

		// Manage Child false_statement
		if (node["false_statement"]) {
			const YAML::Node node_false_statement = node["false_statement"];
			// Set the child
			AST::Node::Ptr child = convert(node_false_statement);
			if (child) {
				if(!result) result = std::make_shared<AST::IfStatement>();
				result->set_false_statement(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::IfStatement>(result);
}


AST::Node::Ptr YAMLImporter::convert_repeatstatement(const YAML::Node node) const {
	AST::RepeatStatement::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::RepeatStatement>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::RepeatStatement>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child times
		if (node["times"]) {
			const YAML::Node node_times = node["times"];
			// Set the child
			AST::Node::Ptr child = convert(node_times);
			if (child) {
				if(!result) result = std::make_shared<AST::RepeatStatement>();
				result->set_times(child);
			}
		}

		// Manage Child statement
		if (node["statement"]) {
			const YAML::Node node_statement = node["statement"];
			// Set the child
			AST::Node::Ptr child = convert(node_statement);
			if (child) {
				if(!result) result = std::make_shared<AST::RepeatStatement>();
				result->set_statement(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::RepeatStatement>(result);
}


AST::Node::Ptr YAMLImporter::convert_forstatement(const YAML::Node node) const {
	AST::ForStatement::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::ForStatement>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::ForStatement>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child pre
		if (node["pre"]) {
			const YAML::Node node_pre = node["pre"];
			// Set the child
			AST::Node::Ptr child = convert(node_pre);
			if (child) {
				AST::BlockingSubstitution::Ptr child_cast = AST::cast_to<AST::BlockingSubstitution>(child);
				if(!result) result = std::make_shared<AST::ForStatement>();
				result->set_pre(child_cast);
			}
		}

		// Manage Child cond
		if (node["cond"]) {
			const YAML::Node node_cond = node["cond"];
			// Set the child
			AST::Node::Ptr child = convert(node_cond);
			if (child) {
				if(!result) result = std::make_shared<AST::ForStatement>();
				result->set_cond(child);
			}
		}

		// Manage Child post
		if (node["post"]) {
			const YAML::Node node_post = node["post"];
			// Set the child
			AST::Node::Ptr child = convert(node_post);
			if (child) {
				AST::BlockingSubstitution::Ptr child_cast = AST::cast_to<AST::BlockingSubstitution>(child);
				if(!result) result = std::make_shared<AST::ForStatement>();
				result->set_post(child_cast);
			}
		}

		// Manage Child statement
		if (node["statement"]) {
			const YAML::Node node_statement = node["statement"];
			// Set the child
			AST::Node::Ptr child = convert(node_statement);
			if (child) {
				if(!result) result = std::make_shared<AST::ForStatement>();
				result->set_statement(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::ForStatement>(result);
}


AST::Node::Ptr YAMLImporter::convert_whilestatement(const YAML::Node node) const {
	AST::WhileStatement::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::WhileStatement>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::WhileStatement>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child cond
		if (node["cond"]) {
			const YAML::Node node_cond = node["cond"];
			// Set the child
			AST::Node::Ptr child = convert(node_cond);
			if (child) {
				if(!result) result = std::make_shared<AST::WhileStatement>();
				result->set_cond(child);
			}
		}

		// Manage Child statement
		if (node["statement"]) {
			const YAML::Node node_statement = node["statement"];
			// Set the child
			AST::Node::Ptr child = convert(node_statement);
			if (child) {
				if(!result) result = std::make_shared<AST::WhileStatement>();
				result->set_statement(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::WhileStatement>(result);
}


AST::Node::Ptr YAMLImporter::convert_casestatement(const YAML::Node node) const {
	AST::CaseStatement::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::CaseStatement>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::CaseStatement>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child comp
		if (node["comp"]) {
			const YAML::Node node_comp = node["comp"];
			// Set the child
			AST::Node::Ptr child = convert(node_comp);
			if (child) {
				if(!result) result = std::make_shared<AST::CaseStatement>();
				result->set_comp(child);
			}
		}

		// Manage Child caselist
		if (node["caselist"]) {
			const YAML::Node node_caselist = node["caselist"];
			// Fill the list of children
			AST::Case::ListPtr caselist_list (new AST::Case::List);
			if (node_caselist.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_caselist.begin(); it !=  node_caselist.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						AST::Case::Ptr child_cast = AST::cast_to<AST::Case>(child);
						caselist_list->push_back(child_cast);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_caselist);
				if (child) {
					AST::Case::Ptr child_cast = AST::cast_to<AST::Case>(child);
					caselist_list->push_back(child_cast);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::CaseStatement>();
			result->set_caselist(caselist_list);
		}
	}

	// Return the result
	return AST::cast_to<AST::CaseStatement>(result);
}


AST::Node::Ptr YAMLImporter::convert_casexstatement(const YAML::Node node) const {
	AST::CasexStatement::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::CasexStatement>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::CasexStatement>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child comp
		if (node["comp"]) {
			const YAML::Node node_comp = node["comp"];
			// Set the child
			AST::Node::Ptr child = convert(node_comp);
			if (child) {
				if(!result) result = std::make_shared<AST::CasexStatement>();
				result->set_comp(child);
			}
		}

		// Manage Child caselist
		if (node["caselist"]) {
			const YAML::Node node_caselist = node["caselist"];
			// Fill the list of children
			AST::Case::ListPtr caselist_list (new AST::Case::List);
			if (node_caselist.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_caselist.begin(); it !=  node_caselist.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						AST::Case::Ptr child_cast = AST::cast_to<AST::Case>(child);
						caselist_list->push_back(child_cast);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_caselist);
				if (child) {
					AST::Case::Ptr child_cast = AST::cast_to<AST::Case>(child);
					caselist_list->push_back(child_cast);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::CasexStatement>();
			result->set_caselist(caselist_list);
		}
	}

	// Return the result
	return AST::cast_to<AST::CasexStatement>(result);
}


AST::Node::Ptr YAMLImporter::convert_casezstatement(const YAML::Node node) const {
	AST::CasezStatement::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::CasezStatement>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::CasezStatement>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child comp
		if (node["comp"]) {
			const YAML::Node node_comp = node["comp"];
			// Set the child
			AST::Node::Ptr child = convert(node_comp);
			if (child) {
				if(!result) result = std::make_shared<AST::CasezStatement>();
				result->set_comp(child);
			}
		}

		// Manage Child caselist
		if (node["caselist"]) {
			const YAML::Node node_caselist = node["caselist"];
			// Fill the list of children
			AST::Case::ListPtr caselist_list (new AST::Case::List);
			if (node_caselist.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_caselist.begin(); it !=  node_caselist.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						AST::Case::Ptr child_cast = AST::cast_to<AST::Case>(child);
						caselist_list->push_back(child_cast);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_caselist);
				if (child) {
					AST::Case::Ptr child_cast = AST::cast_to<AST::Case>(child);
					caselist_list->push_back(child_cast);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::CasezStatement>();
			result->set_caselist(caselist_list);
		}
	}

	// Return the result
	return AST::cast_to<AST::CasezStatement>(result);
}


AST::Node::Ptr YAMLImporter::convert_case(const YAML::Node node) const {
	AST::Case::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Case>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Case>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child cond
		if (node["cond"]) {
			const YAML::Node node_cond = node["cond"];
			// Fill the list of children
			AST::Node::ListPtr cond_list (new AST::Node::List);
			if (node_cond.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_cond.begin(); it !=  node_cond.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						cond_list->push_back(child);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_cond);
				if (child) {
					cond_list->push_back(child);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::Case>();
			result->set_cond(cond_list);
		}

		// Manage Child statement
		if (node["statement"]) {
			const YAML::Node node_statement = node["statement"];
			// Set the child
			AST::Node::Ptr child = convert(node_statement);
			if (child) {
				if(!result) result = std::make_shared<AST::Case>();
				result->set_statement(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Case>(result);
}


AST::Node::Ptr YAMLImporter::convert_block(const YAML::Node node) const {
	AST::Block::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Block>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Block>();
				result->set_line(node["line"].as<int>());
			}
		}
		// Manage property scope
		if (node["scope"]) {
			if (node["scope"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Block>();
				result->set_scope(node["scope"].as<std::string>());
			}
		}

		// Manage Child statements
		if (node["statements"]) {
			const YAML::Node node_statements = node["statements"];
			// Fill the list of children
			AST::Node::ListPtr statements_list (new AST::Node::List);
			if (node_statements.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_statements.begin(); it !=  node_statements.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						statements_list->push_back(child);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_statements);
				if (child) {
					statements_list->push_back(child);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::Block>();
			result->set_statements(statements_list);
		}
	}

	// Return the result
	return AST::cast_to<AST::Block>(result);
}


AST::Node::Ptr YAMLImporter::convert_initial(const YAML::Node node) const {
	AST::Initial::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Initial>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Initial>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child statement
		if (node["statement"]) {
			const YAML::Node node_statement = node["statement"];
			// Set the child
			AST::Node::Ptr child = convert(node_statement);
			if (child) {
				if(!result) result = std::make_shared<AST::Initial>();
				result->set_statement(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Initial>(result);
}


AST::Node::Ptr YAMLImporter::convert_eventstatement(const YAML::Node node) const {
	AST::EventStatement::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::EventStatement>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::EventStatement>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child senslist
		if (node["senslist"]) {
			const YAML::Node node_senslist = node["senslist"];
			// Set the child
			AST::Node::Ptr child = convert(node_senslist);
			if (child) {
				AST::Senslist::Ptr child_cast = AST::cast_to<AST::Senslist>(child);
				if(!result) result = std::make_shared<AST::EventStatement>();
				result->set_senslist(child_cast);
			}
		}

		// Manage Child statement
		if (node["statement"]) {
			const YAML::Node node_statement = node["statement"];
			// Set the child
			AST::Node::Ptr child = convert(node_statement);
			if (child) {
				if(!result) result = std::make_shared<AST::EventStatement>();
				result->set_statement(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::EventStatement>(result);
}


AST::Node::Ptr YAMLImporter::convert_waitstatement(const YAML::Node node) const {
	AST::WaitStatement::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::WaitStatement>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::WaitStatement>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child cond
		if (node["cond"]) {
			const YAML::Node node_cond = node["cond"];
			// Set the child
			AST::Node::Ptr child = convert(node_cond);
			if (child) {
				if(!result) result = std::make_shared<AST::WaitStatement>();
				result->set_cond(child);
			}
		}

		// Manage Child statement
		if (node["statement"]) {
			const YAML::Node node_statement = node["statement"];
			// Set the child
			AST::Node::Ptr child = convert(node_statement);
			if (child) {
				if(!result) result = std::make_shared<AST::WaitStatement>();
				result->set_statement(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::WaitStatement>(result);
}


AST::Node::Ptr YAMLImporter::convert_foreverstatement(const YAML::Node node) const {
	AST::ForeverStatement::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::ForeverStatement>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::ForeverStatement>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child statement
		if (node["statement"]) {
			const YAML::Node node_statement = node["statement"];
			// Set the child
			AST::Node::Ptr child = convert(node_statement);
			if (child) {
				if(!result) result = std::make_shared<AST::ForeverStatement>();
				result->set_statement(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::ForeverStatement>(result);
}


AST::Node::Ptr YAMLImporter::convert_delaystatement(const YAML::Node node) const {
	AST::DelayStatement::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::DelayStatement>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::DelayStatement>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child delay
		if (node["delay"]) {
			const YAML::Node node_delay = node["delay"];
			// Set the child
			AST::Node::Ptr child = convert(node_delay);
			if (child) {
				if(!result) result = std::make_shared<AST::DelayStatement>();
				result->set_delay(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::DelayStatement>(result);
}


AST::Node::Ptr YAMLImporter::convert_instancelist(const YAML::Node node) const {
	AST::Instancelist::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Instancelist>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Instancelist>();
				result->set_line(node["line"].as<int>());
			}
		}
		// Manage property module
		if (node["module"]) {
			if (node["module"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Instancelist>();
				result->set_module(node["module"].as<std::string>());
			}
		}

		// Manage Child parameterlist
		if (node["parameterlist"]) {
			const YAML::Node node_parameterlist = node["parameterlist"];
			// Fill the list of children
			AST::ParamArg::ListPtr parameterlist_list (new AST::ParamArg::List);
			if (node_parameterlist.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_parameterlist.begin(); it !=  node_parameterlist.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						AST::ParamArg::Ptr child_cast = AST::cast_to<AST::ParamArg>(child);
						parameterlist_list->push_back(child_cast);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_parameterlist);
				if (child) {
					AST::ParamArg::Ptr child_cast = AST::cast_to<AST::ParamArg>(child);
					parameterlist_list->push_back(child_cast);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::Instancelist>();
			result->set_parameterlist(parameterlist_list);
		}

		// Manage Child instances
		if (node["instances"]) {
			const YAML::Node node_instances = node["instances"];
			// Fill the list of children
			AST::Instance::ListPtr instances_list (new AST::Instance::List);
			if (node_instances.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_instances.begin(); it !=  node_instances.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						AST::Instance::Ptr child_cast = AST::cast_to<AST::Instance>(child);
						instances_list->push_back(child_cast);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_instances);
				if (child) {
					AST::Instance::Ptr child_cast = AST::cast_to<AST::Instance>(child);
					instances_list->push_back(child_cast);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::Instancelist>();
			result->set_instances(instances_list);
		}
	}

	// Return the result
	return AST::cast_to<AST::Instancelist>(result);
}


AST::Node::Ptr YAMLImporter::convert_instance(const YAML::Node node) const {
	AST::Instance::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Instance>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Instance>();
				result->set_line(node["line"].as<int>());
			}
		}
		// Manage property module
		if (node["module"]) {
			if (node["module"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Instance>();
				result->set_module(node["module"].as<std::string>());
			}
		}
		// Manage property name
		if (node["name"]) {
			if (node["name"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Instance>();
				result->set_name(node["name"].as<std::string>());
			}
		}

		// Manage Child array
		if (node["array"]) {
			const YAML::Node node_array = node["array"];
			// Set the child
			AST::Node::Ptr child = convert(node_array);
			if (child) {
				AST::Length::Ptr child_cast = AST::cast_to<AST::Length>(child);
				if(!result) result = std::make_shared<AST::Instance>();
				result->set_array(child_cast);
			}
		}

		// Manage Child parameterlist
		if (node["parameterlist"]) {
			const YAML::Node node_parameterlist = node["parameterlist"];
			// Fill the list of children
			AST::ParamArg::ListPtr parameterlist_list (new AST::ParamArg::List);
			if (node_parameterlist.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_parameterlist.begin(); it !=  node_parameterlist.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						AST::ParamArg::Ptr child_cast = AST::cast_to<AST::ParamArg>(child);
						parameterlist_list->push_back(child_cast);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_parameterlist);
				if (child) {
					AST::ParamArg::Ptr child_cast = AST::cast_to<AST::ParamArg>(child);
					parameterlist_list->push_back(child_cast);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::Instance>();
			result->set_parameterlist(parameterlist_list);
		}

		// Manage Child portlist
		if (node["portlist"]) {
			const YAML::Node node_portlist = node["portlist"];
			// Fill the list of children
			AST::PortArg::ListPtr portlist_list (new AST::PortArg::List);
			if (node_portlist.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_portlist.begin(); it !=  node_portlist.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						AST::PortArg::Ptr child_cast = AST::cast_to<AST::PortArg>(child);
						portlist_list->push_back(child_cast);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_portlist);
				if (child) {
					AST::PortArg::Ptr child_cast = AST::cast_to<AST::PortArg>(child);
					portlist_list->push_back(child_cast);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::Instance>();
			result->set_portlist(portlist_list);
		}
	}

	// Return the result
	return AST::cast_to<AST::Instance>(result);
}


AST::Node::Ptr YAMLImporter::convert_paramarg(const YAML::Node node) const {
	AST::ParamArg::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::ParamArg>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::ParamArg>();
				result->set_line(node["line"].as<int>());
			}
		}
		// Manage property name
		if (node["name"]) {
			if (node["name"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::ParamArg>();
				result->set_name(node["name"].as<std::string>());
			}
		}

		// Manage Child value
		if (node["value"]) {
			const YAML::Node node_value = node["value"];
			// Set the child
			AST::Node::Ptr child = convert(node_value);
			if (child) {
				if(!result) result = std::make_shared<AST::ParamArg>();
				result->set_value(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::ParamArg>(result);
}


AST::Node::Ptr YAMLImporter::convert_portarg(const YAML::Node node) const {
	AST::PortArg::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::PortArg>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::PortArg>();
				result->set_line(node["line"].as<int>());
			}
		}
		// Manage property name
		if (node["name"]) {
			if (node["name"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::PortArg>();
				result->set_name(node["name"].as<std::string>());
			}
		}

		// Manage Child value
		if (node["value"]) {
			const YAML::Node node_value = node["value"];
			// Set the child
			AST::Node::Ptr child = convert(node_value);
			if (child) {
				if(!result) result = std::make_shared<AST::PortArg>();
				result->set_value(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::PortArg>(result);
}


AST::Node::Ptr YAMLImporter::convert_function(const YAML::Node node) const {
	AST::Function::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Function>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Function>();
				result->set_line(node["line"].as<int>());
			}
		}
		// Manage property name
		if (node["name"]) {
			if (node["name"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Function>();
				result->set_name(node["name"].as<std::string>());
			}
		}
		// Manage property automatic
		if (node["automatic"]) {
			if (node["automatic"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Function>();
				result->set_automatic(node["automatic"].as<bool>());
			}
		}
		// Manage property rettype
		if (node["rettype"]) {
			if (node["rettype"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Function>();
				result->set_rettype(node["rettype"].as<AST::Function::RettypeEnum>());
			}
		}
		// Manage property retsign
		if (node["retsign"]) {
			if (node["retsign"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Function>();
				result->set_retsign(node["retsign"].as<bool>());
			}
		}

		// Manage Child retwidths
		if (node["retwidths"]) {
			const YAML::Node node_retwidths = node["retwidths"];
			// Fill the list of children
			AST::Width::ListPtr retwidths_list (new AST::Width::List);
			if (node_retwidths.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_retwidths.begin(); it !=  node_retwidths.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						AST::Width::Ptr child_cast = AST::cast_to<AST::Width>(child);
						retwidths_list->push_back(child_cast);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_retwidths);
				if (child) {
					AST::Width::Ptr child_cast = AST::cast_to<AST::Width>(child);
					retwidths_list->push_back(child_cast);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::Function>();
			result->set_retwidths(retwidths_list);
		}

		// Manage Child ports
		if (node["ports"]) {
			const YAML::Node node_ports = node["ports"];
			// Fill the list of children
			AST::Node::ListPtr ports_list (new AST::Node::List);
			if (node_ports.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_ports.begin(); it !=  node_ports.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						ports_list->push_back(child);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_ports);
				if (child) {
					ports_list->push_back(child);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::Function>();
			result->set_ports(ports_list);
		}

		// Manage Child statements
		if (node["statements"]) {
			const YAML::Node node_statements = node["statements"];
			// Fill the list of children
			AST::Node::ListPtr statements_list (new AST::Node::List);
			if (node_statements.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_statements.begin(); it !=  node_statements.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						statements_list->push_back(child);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_statements);
				if (child) {
					statements_list->push_back(child);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::Function>();
			result->set_statements(statements_list);
		}
	}

	// Return the result
	return AST::cast_to<AST::Function>(result);
}


AST::Node::Ptr YAMLImporter::convert_functioncall(const YAML::Node node) const {
	AST::FunctionCall::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::FunctionCall>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::FunctionCall>();
				result->set_line(node["line"].as<int>());
			}
		}
		// Manage property name
		if (node["name"]) {
			if (node["name"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::FunctionCall>();
				result->set_name(node["name"].as<std::string>());
			}
		}

		// Manage Child args
		if (node["args"]) {
			const YAML::Node node_args = node["args"];
			// Fill the list of children
			AST::Node::ListPtr args_list (new AST::Node::List);
			if (node_args.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_args.begin(); it !=  node_args.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						args_list->push_back(child);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_args);
				if (child) {
					args_list->push_back(child);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::FunctionCall>();
			result->set_args(args_list);
		}
	}

	// Return the result
	return AST::cast_to<AST::FunctionCall>(result);
}


AST::Node::Ptr YAMLImporter::convert_task(const YAML::Node node) const {
	AST::Task::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Task>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Task>();
				result->set_line(node["line"].as<int>());
			}
		}
		// Manage property name
		if (node["name"]) {
			if (node["name"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Task>();
				result->set_name(node["name"].as<std::string>());
			}
		}
		// Manage property automatic
		if (node["automatic"]) {
			if (node["automatic"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Task>();
				result->set_automatic(node["automatic"].as<bool>());
			}
		}

		// Manage Child ports
		if (node["ports"]) {
			const YAML::Node node_ports = node["ports"];
			// Fill the list of children
			AST::Node::ListPtr ports_list (new AST::Node::List);
			if (node_ports.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_ports.begin(); it !=  node_ports.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						ports_list->push_back(child);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_ports);
				if (child) {
					ports_list->push_back(child);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::Task>();
			result->set_ports(ports_list);
		}

		// Manage Child statements
		if (node["statements"]) {
			const YAML::Node node_statements = node["statements"];
			// Fill the list of children
			AST::Node::ListPtr statements_list (new AST::Node::List);
			if (node_statements.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_statements.begin(); it !=  node_statements.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						statements_list->push_back(child);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_statements);
				if (child) {
					statements_list->push_back(child);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::Task>();
			result->set_statements(statements_list);
		}
	}

	// Return the result
	return AST::cast_to<AST::Task>(result);
}


AST::Node::Ptr YAMLImporter::convert_taskcall(const YAML::Node node) const {
	AST::TaskCall::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::TaskCall>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::TaskCall>();
				result->set_line(node["line"].as<int>());
			}
		}
		// Manage property name
		if (node["name"]) {
			if (node["name"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::TaskCall>();
				result->set_name(node["name"].as<std::string>());
			}
		}

		// Manage Child args
		if (node["args"]) {
			const YAML::Node node_args = node["args"];
			// Fill the list of children
			AST::Node::ListPtr args_list (new AST::Node::List);
			if (node_args.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_args.begin(); it !=  node_args.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						args_list->push_back(child);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_args);
				if (child) {
					args_list->push_back(child);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::TaskCall>();
			result->set_args(args_list);
		}
	}

	// Return the result
	return AST::cast_to<AST::TaskCall>(result);
}


AST::Node::Ptr YAMLImporter::convert_generatestatement(const YAML::Node node) const {
	AST::GenerateStatement::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::GenerateStatement>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::GenerateStatement>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child items
		if (node["items"]) {
			const YAML::Node node_items = node["items"];
			// Fill the list of children
			AST::Node::ListPtr items_list (new AST::Node::List);
			if (node_items.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_items.begin(); it !=  node_items.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						items_list->push_back(child);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_items);
				if (child) {
					items_list->push_back(child);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::GenerateStatement>();
			result->set_items(items_list);
		}
	}

	// Return the result
	return AST::cast_to<AST::GenerateStatement>(result);
}


AST::Node::Ptr YAMLImporter::convert_systemcall(const YAML::Node node) const {
	AST::SystemCall::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::SystemCall>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::SystemCall>();
				result->set_line(node["line"].as<int>());
			}
		}
		// Manage property syscall
		if (node["syscall"]) {
			if (node["syscall"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::SystemCall>();
				result->set_syscall(node["syscall"].as<std::string>());
			}
		}

		// Manage Child args
		if (node["args"]) {
			const YAML::Node node_args = node["args"];
			// Fill the list of children
			AST::Node::ListPtr args_list (new AST::Node::List);
			if (node_args.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_args.begin(); it !=  node_args.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						args_list->push_back(child);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_args);
				if (child) {
					args_list->push_back(child);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::SystemCall>();
			result->set_args(args_list);
		}
	}

	// Return the result
	return AST::cast_to<AST::SystemCall>(result);
}


AST::Node::Ptr YAMLImporter::convert_identifierscopelabel(const YAML::Node node) const {
	AST::IdentifierScopeLabel::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::IdentifierScopeLabel>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::IdentifierScopeLabel>();
				result->set_line(node["line"].as<int>());
			}
		}
		// Manage property name
		if (node["name"]) {
			if (node["name"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::IdentifierScopeLabel>();
				result->set_name(node["name"].as<std::string>());
			}
		}

		// Manage Child loop
		if (node["loop"]) {
			const YAML::Node node_loop = node["loop"];
			// Set the child
			AST::Node::Ptr child = convert(node_loop);
			if (child) {
				if(!result) result = std::make_shared<AST::IdentifierScopeLabel>();
				result->set_loop(child);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::IdentifierScopeLabel>(result);
}


AST::Node::Ptr YAMLImporter::convert_identifierscope(const YAML::Node node) const {
	AST::IdentifierScope::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::IdentifierScope>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::IdentifierScope>();
				result->set_line(node["line"].as<int>());
			}
		}

		// Manage Child labellist
		if (node["labellist"]) {
			const YAML::Node node_labellist = node["labellist"];
			// Fill the list of children
			AST::IdentifierScopeLabel::ListPtr labellist_list (new AST::IdentifierScopeLabel::List);
			if (node_labellist.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_labellist.begin(); it !=  node_labellist.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						AST::IdentifierScopeLabel::Ptr child_cast = AST::cast_to<AST::IdentifierScopeLabel>(child);
						labellist_list->push_back(child_cast);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_labellist);
				if (child) {
					AST::IdentifierScopeLabel::Ptr child_cast = AST::cast_to<AST::IdentifierScopeLabel>(child);
					labellist_list->push_back(child_cast);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::IdentifierScope>();
			result->set_labellist(labellist_list);
		}
	}

	// Return the result
	return AST::cast_to<AST::IdentifierScope>(result);
}


AST::Node::Ptr YAMLImporter::convert_disable(const YAML::Node node) const {
	AST::Disable::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Disable>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::Disable>();
				result->set_line(node["line"].as<int>());
			}
		}
		// Manage property dest
		if (node["dest"]) {
			if (node["dest"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::Disable>();
				result->set_dest(node["dest"].as<std::string>());
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::Disable>(result);
}


AST::Node::Ptr YAMLImporter::convert_parallelblock(const YAML::Node node) const {
	AST::ParallelBlock::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::ParallelBlock>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::ParallelBlock>();
				result->set_line(node["line"].as<int>());
			}
		}
		// Manage property scope
		if (node["scope"]) {
			if (node["scope"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::ParallelBlock>();
				result->set_scope(node["scope"].as<std::string>());
			}
		}

		// Manage Child statements
		if (node["statements"]) {
			const YAML::Node node_statements = node["statements"];
			// Fill the list of children
			AST::Node::ListPtr statements_list (new AST::Node::List);
			if (node_statements.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_statements.begin(); it !=  node_statements.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						statements_list->push_back(child);
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_statements);
				if (child) {
					statements_list->push_back(child);
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::ParallelBlock>();
			result->set_statements(statements_list);
		}
	}

	// Return the result
	return AST::cast_to<AST::ParallelBlock>(result);
}


AST::Node::Ptr YAMLImporter::convert_singlestatement(const YAML::Node node) const {
	AST::SingleStatement::Ptr result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::SingleStatement>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::SingleStatement>();
				result->set_line(node["line"].as<int>());
			}
		}
		// Manage property scope
		if (node["scope"]) {
			if (node["scope"].IsScalar()) {
				
				if(!result) result = std::make_shared<AST::SingleStatement>();
				result->set_scope(node["scope"].as<std::string>());
			}
		}

		// Manage Child statement
		if (node["statement"]) {
			const YAML::Node node_statement = node["statement"];
			// Set the child
			AST::Node::Ptr child = convert(node_statement);
			if (child) {
				if(!result) result = std::make_shared<AST::SingleStatement>();
				result->set_statement(child);
			}
		}

		// Manage Child delay
		if (node["delay"]) {
			const YAML::Node node_delay = node["delay"];
			// Set the child
			AST::Node::Ptr child = convert(node_delay);
			if (child) {
				AST::DelayStatement::Ptr child_cast = AST::cast_to<AST::DelayStatement>(child);
				if(!result) result = std::make_shared<AST::SingleStatement>();
				result->set_delay(child_cast);
			}
		}
	}

	// Return the result
	return AST::cast_to<AST::SingleStatement>(result);
}


}
}