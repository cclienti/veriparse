#ifndef VERIPARSE_VERILOG_NUMBER_SCANNER_HPP
#define VERIPARSE_VERILOG_NUMBER_SCANNER_HPP

// Avoid multiples re-declarations
#ifndef yyFlexLexerOnce
// This trick allows to use multiple flex in the library. It must be
// used with the -P option of flex. see header of FlexLexer.h
#undef yyFlexLexer
#define yyFlexLexer VerilogNumberFlexLexer
#include <FlexLexer.h>
#endif

/// Generated by bison in the cmake build dir
#include <verilog_number/verilog_number_parser.hpp>
#include <verilog_number/location.hh>

#include <string>

namespace Veriparse {
namespace Parser {

class VerilogNumberScanner : public yyFlexLexer {
public:

	VerilogNumberScanner(std::istream *in) :
		yyFlexLexer(in)
	{
		loc = new Veriparse::Parser::VerilogNumberParser::location_type();
	};

	virtual ~VerilogNumberScanner() {
		delete loc;
	};

	virtual int yylex(Veriparse::Parser::VerilogNumberParser::semantic_type * const lval,
	                  Veriparse::Parser::VerilogNumberParser::location_type * location);
	// YY_DECL defined in scanner.ll
	// Method body created by flex in lex.yy.cc

private:
	/* yyval ptr */
	VerilogNumberParser::semantic_type *yylval = nullptr;
	/* location ptr */
	VerilogNumberParser::location_type *loc    = nullptr;
};

}
}

#endif
