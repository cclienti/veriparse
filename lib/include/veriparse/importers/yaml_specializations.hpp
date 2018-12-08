#ifndef VERIPARSE_GENERATORS_YAML_SPECIALIZATION_HPP
#define VERIPARSE_GENERATORS_YAML_SPECIALIZATION_HPP

#include <gmp.h>
#include <gmpxx.h>
#include <yaml-cpp/yaml.h>
#include <sstream>

namespace YAML {
template<>
struct convert<Veriparse::AST::Module::Default_nettypeEnum> {
	static Node encode(const Veriparse::AST::Module::Default_nettypeEnum &rhs) {
		switch (rhs) {
		case Veriparse::AST::Module::Default_nettypeEnum::INTEGER : return Node("INTEGER");
		case Veriparse::AST::Module::Default_nettypeEnum::REAL : return Node("REAL");
		case Veriparse::AST::Module::Default_nettypeEnum::REG : return Node("REG");
		case Veriparse::AST::Module::Default_nettypeEnum::TRI : return Node("TRI");
		case Veriparse::AST::Module::Default_nettypeEnum::WIRE : return Node("WIRE");
		case Veriparse::AST::Module::Default_nettypeEnum::SUPPLY0 : return Node("SUPPLY0");
		case Veriparse::AST::Module::Default_nettypeEnum::SUPPLY1 : return Node("SUPPLY1");
		default: return Node("NONE");
		}
	}

	static bool decode(const Node& node, Veriparse::AST::Module::Default_nettypeEnum &rhs) {
		if(!node.IsScalar()) {
			return false;
		}
		
		if (node.as<std::string>() == "INTEGER") {
			rhs = Veriparse::AST::Module::Default_nettypeEnum::INTEGER;
			return true;
		}
		
		if (node.as<std::string>() == "REAL") {
			rhs = Veriparse::AST::Module::Default_nettypeEnum::REAL;
			return true;
		}
		
		if (node.as<std::string>() == "REG") {
			rhs = Veriparse::AST::Module::Default_nettypeEnum::REG;
			return true;
		}
		
		if (node.as<std::string>() == "TRI") {
			rhs = Veriparse::AST::Module::Default_nettypeEnum::TRI;
			return true;
		}
		
		if (node.as<std::string>() == "WIRE") {
			rhs = Veriparse::AST::Module::Default_nettypeEnum::WIRE;
			return true;
		}
		
		if (node.as<std::string>() == "SUPPLY0") {
			rhs = Veriparse::AST::Module::Default_nettypeEnum::SUPPLY0;
			return true;
		}
		
		if (node.as<std::string>() == "SUPPLY1") {
			rhs = Veriparse::AST::Module::Default_nettypeEnum::SUPPLY1;
			return true;
		}
		
		if (node.as<std::string>() == "NONE") {
			rhs = Veriparse::AST::Module::Default_nettypeEnum::NONE;
			return true;
		}
		
		return false;
	}
};
template<>
struct convert<Veriparse::AST::Parameter::TypeEnum> {
	static Node encode(const Veriparse::AST::Parameter::TypeEnum &rhs) {
		switch (rhs) {
		case Veriparse::AST::Parameter::TypeEnum::INTEGER : return Node("INTEGER");
		case Veriparse::AST::Parameter::TypeEnum::REAL : return Node("REAL");
		default: return Node("NONE");
		}
	}

	static bool decode(const Node& node, Veriparse::AST::Parameter::TypeEnum &rhs) {
		if(!node.IsScalar()) {
			return false;
		}
		
		if (node.as<std::string>() == "INTEGER") {
			rhs = Veriparse::AST::Parameter::TypeEnum::INTEGER;
			return true;
		}
		
		if (node.as<std::string>() == "REAL") {
			rhs = Veriparse::AST::Parameter::TypeEnum::REAL;
			return true;
		}
		
		if (node.as<std::string>() == "NONE") {
			rhs = Veriparse::AST::Parameter::TypeEnum::NONE;
			return true;
		}
		
		return false;
	}
};
template<>
struct convert<Veriparse::AST::Sens::TypeEnum> {
	static Node encode(const Veriparse::AST::Sens::TypeEnum &rhs) {
		switch (rhs) {
		case Veriparse::AST::Sens::TypeEnum::ALL : return Node("ALL");
		case Veriparse::AST::Sens::TypeEnum::POSEDGE : return Node("POSEDGE");
		case Veriparse::AST::Sens::TypeEnum::NEGEDGE : return Node("NEGEDGE");
		default: return Node("NONE");
		}
	}

	static bool decode(const Node& node, Veriparse::AST::Sens::TypeEnum &rhs) {
		if(!node.IsScalar()) {
			return false;
		}
		
		if (node.as<std::string>() == "ALL") {
			rhs = Veriparse::AST::Sens::TypeEnum::ALL;
			return true;
		}
		
		if (node.as<std::string>() == "POSEDGE") {
			rhs = Veriparse::AST::Sens::TypeEnum::POSEDGE;
			return true;
		}
		
		if (node.as<std::string>() == "NEGEDGE") {
			rhs = Veriparse::AST::Sens::TypeEnum::NEGEDGE;
			return true;
		}
		
		if (node.as<std::string>() == "NONE") {
			rhs = Veriparse::AST::Sens::TypeEnum::NONE;
			return true;
		}
		
		return false;
	}
};
template<>
struct convert<Veriparse::AST::Function::RettypeEnum> {
	static Node encode(const Veriparse::AST::Function::RettypeEnum &rhs) {
		switch (rhs) {
		case Veriparse::AST::Function::RettypeEnum::INTEGER : return Node("INTEGER");
		case Veriparse::AST::Function::RettypeEnum::REAL : return Node("REAL");
		default: return Node("NONE");
		}
	}

	static bool decode(const Node& node, Veriparse::AST::Function::RettypeEnum &rhs) {
		if(!node.IsScalar()) {
			return false;
		}
		
		if (node.as<std::string>() == "INTEGER") {
			rhs = Veriparse::AST::Function::RettypeEnum::INTEGER;
			return true;
		}
		
		if (node.as<std::string>() == "REAL") {
			rhs = Veriparse::AST::Function::RettypeEnum::REAL;
			return true;
		}
		
		if (node.as<std::string>() == "NONE") {
			rhs = Veriparse::AST::Function::RettypeEnum::NONE;
			return true;
		}
		
		return false;
	}
};

template<>
struct convert<mpz_class> {
	static Node encode(const mpz_class &rhs) {
		return Node(rhs.get_str(10));
	}

	static bool decode(const Node& node, mpz_class &rhs) {
		if(!node.IsScalar()) {
			return false;
		}

		bool result = true;

		try {
			mpz_class num(node.as<std::string>());
			rhs = num;
		}
		catch (const std::invalid_argument &arg) {
			result = false;
		}

		return result;
	}
};

}


#endif