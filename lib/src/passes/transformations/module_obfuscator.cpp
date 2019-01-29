#include <veriparse/passes/transformations/module_obfuscator.hpp>
#include <veriparse/passes/transformations/ast_replace.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/passes/analysis/unique_declaration.hpp>
#include <veriparse/misc/string_utils.hpp>
#include <veriparse/logger/logger.hpp>

#include <functional>
#include <string>
#include <random>
#include <set>


namespace Veriparse {
namespace Passes {
namespace Transformations {


int ModuleObfuscator::process(AST::Node::Ptr node, AST::Node::Ptr parent)
{
	auto nets = Analysis::Module::get_variable_nodes_within_module(node);

	Analysis::UniqueDeclaration::IdentifierSet unique_id;
	ASTReplace::ReplaceMap replace_map;
	std::hash<std::string> hash;

	for (const auto &net: *nets) {
		const auto &net_name = net->get_name();
		auto id = std::make_shared<AST::Identifier>();

		if (m_use_hash) {
			auto obf_name = std::to_string(hash(net_name));
			obf_name.insert(0, 1, 'h');

			if (unique_id.count(obf_name)) {
				LOG_WARNING_N(net) << "Hash collision for net '" << net_name << "'";
				obf_name = Analysis::UniqueDeclaration::get_unique_identifier("h", unique_id,
				                                                              m_identifier_length);
			}
			else {
				unique_id.insert(obf_name);
			}

			id->set_name(obf_name);
		}
		else {
			auto obf_name = Analysis::UniqueDeclaration::get_unique_identifier("h", unique_id,
			                                                                   m_identifier_length);
			unique_id.insert(obf_name);
		}

		LOG_DEBUG_N(net) << "Obfuscating '" << net_name << "' with '" << id->get_name() << "'";

		replace_map.emplace(net_name, id);
	}

	if (ASTReplace::replace_identifier(node, replace_map)) {
		LOG_ERROR_N(node) << "cannot obfuscate the module";
		return 1;
	}

	// ADD REPLACE OF DECLARATION

	return 0;
}


}
}
}
