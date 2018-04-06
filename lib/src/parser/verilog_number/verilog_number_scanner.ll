%{

#include <string>

/* Implementation of yyFlexScanner */
#include <parser/verilog_number/verilog_number_scanner.hpp>
#include <veriparse/logger/logger.hpp>

#undef  YY_DECL
#define YY_DECL int Veriparse::Parser::VerilogNumberScanner::yylex \
	(Veriparse::Parser::VerilogNumberParser::semantic_type * const lval, \
	 Veriparse::Parser::VerilogNumberParser::location_type *loc)

/* typedef to make the returns for the tokens shorter */
using token = Veriparse::Parser::VerilogNumberParser::token;

/* msvc2010 requires that we exclude this header file. */
#define YY_NO_UNISTD_H

/* update location on matching */
#define YY_USER_ACTION loc->step(); loc->columns(yyleng);

%}

%option yyclass="Veriparse::Parser::VerilogNumberScanner"
%option noyywrap
%option c++


%%


%{          /** Code executed at the beginning of yylex **/
   yylval = lval;
%}

\'       {return token::TK_SQUOTE;}
[0]      {return token::TK_0;}
[1]      {return token::TK_1;}
[2]      {return token::TK_2;}
[3]      {return token::TK_3;}
[4]      {return token::TK_4;}
[5]      {return token::TK_5;}
[6]      {return token::TK_6;}
[7]      {return token::TK_7;}
[8]      {return token::TK_8;}
[9]      {return token::TK_9;}
[aA]     {return token::TK_A;}
[bB]     {return token::TK_B;}
[cC]     {return token::TK_C;}
[dD]     {return token::TK_D;}
[eE]     {return token::TK_E;}
[fF]     {return token::TK_F;}
[xX]     {return token::TK_X;}
[zZ\?]   {return token::TK_Z;}
[oO]     {return token::TK_O;}
[hH]     {return token::TK_H;}
[sS]     {return token::TK_S;}

[ \t_]

%%
