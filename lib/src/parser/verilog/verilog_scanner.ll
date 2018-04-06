%{
#include <string>
#include <cstdlib>

/* Implementation of yyFlexScanner */
#include <parser/verilog/verilog_scanner.hpp>
#include <veriparse/misc/string_utils.hpp>
#include <veriparse/logger/logger.hpp>
#undef  YY_DECL
#define YY_DECL int Veriparse::Parser::VerilogScanner::yylex(Veriparse::Parser::VerilogParser::semantic_type * const lval, \
                                                             Veriparse::Parser::VerilogParser::location_type *loc)

/* typedef to make the returns for the tokens shorter */
using token = Veriparse::Parser::VerilogParser::token;

/* msvc2010 requires that we exclude this header file. */
#define YY_NO_UNISTD_H

/* update location on matching */
#define YY_USER_ACTION loc->step(); loc->columns(yyleng);

%}

%option yyclass="Veriparse::Parser::VerilogScanner"
%option noyywrap
%option c++

%x VPP_LINE
%x VPP_TIMESCALE
%x VPP_DEFAULT_NETTYPE

%x STRING
%x SYNTHESIS_TRANSLATE_OFF1
%x SYNTHESIS_TRANSLATE_OFF2
%x COMMENT

%%

%{          /** Code executed at the beginning of yylex **/
   yylval = lval;
%}

	/**************************************************
	 * Keywords
	 **************************************************/

"module"      {return token::TK_MODULE;}
"endmodule"   {return token::TK_ENDMODULE;}
"begin"       {return token::TK_BEGIN;}
"end"         {return token::TK_END;}
"generate"    {return token::TK_GENERATE;}
"endgenerate" {return token::TK_ENDGENERATE;}
"genvar"      {return token::TK_GENVAR;}
"function"    {return token::TK_FUNCTION;}
"endfunction" {return token::TK_ENDFUNCTION;}
"task"        {return token::TK_TASK;}
"endtask"     {return token::TK_ENDTASK;}
"input"       {return token::TK_INPUT;}
"inout"       {return token::TK_INOUT;}
"output"      {return token::TK_OUTPUT;}
"tri"         {return token::TK_TRI;}
"reg"         {return token::TK_REG;}
"wire"        {return token::TK_WIRE;}
"integer"     {return token::TK_INTEGER;}
"real"        {return token::TK_REAL;}
"signed"      {return token::TK_SIGNED;}
"parameter"   {return token::TK_PARAMETER;}
"localparam"  {return token::TK_LOCALPARAM;}
"supply0"     {return token::TK_SUPPLY0;}
"supply1"     {return token::TK_SUPPLY1;}
"automatic"   {return token::TK_AUTOMATIC;}
"assign"      {return token::TK_ASSIGN;}
"always"      {return token::TK_ALWAYS;}
"or"          {return token::TK_SENS_OR;}
"posedge"     {return token::TK_POSEDGE;}
"negedge"     {return token::TK_NEGEDGE;}
"initial"     {return token::TK_INITIAL;}
"if"          {return token::TK_IF;}
"else"        {return token::TK_ELSE;}
"for"         {return token::TK_FOR;}
"while"       {return token::TK_WHILE;}
"case"        {return token::TK_CASE;}
"casex"       {return token::TK_CASEX;}
"casez"       {return token::TK_CASEZ;}
"endcase"     {return token::TK_ENDCASE;}
"default"     {return token::TK_DEFAULT;}
"wait"        {return token::TK_WAIT;}
"forever"     {return token::TK_FOREVER;}
"disable"     {return token::TK_DISABLE;}
"fork"        {return token::TK_FORK;}
"join"        {return token::TK_JOIN;}

	/**************************************************
	 * Operators
	 **************************************************/

"\|\|" {return token::TK_LOR;}
"\&\&" {return token::TK_LAND;}

"~\|"  {return token::TK_NOR;}
"~\&"  {return token::TK_NAND;}
"~\^"  {return token::TK_XNOR;}
"\|"   {return token::TK_OR;}
"\&"   {return token::TK_AND;}
"\^"   {return token::TK_XOR;}
"!"    {return token::TK_LNOT;}
"~"    {return token::TK_NOT;}

"<<<"  {return token::TK_LSHIFTA;}
">>>"  {return token::TK_RSHIFTA;}
"<<"   {return token::TK_LSHIFT;}
">>"   {return token::TK_RSHIFT;}

"==="  {return token::TK_EQL;}
"!=="  {return token::TK_NEL;}
"=="   {return token::TK_EQ;}
"!="   {return token::TK_NE;}

"<="   {return token::TK_LE;}
">="   {return token::TK_GE;}
"<"    {return token::TK_LT;}
">"    {return token::TK_GT;}

"\*\*" {return token::TK_POWER;}
"\+"   {return token::TK_PLUS;}
"-"    {return token::TK_MINUS;}
"\*"   {return token::TK_TIMES;}
"/"    {return token::TK_DIVIDE;}
"%"    {return token::TK_MOD;}

"\?"   {return token::TK_COND;}
"="    {return token::TK_EQUALS;}

"\+:"  {return token::TK_PLUSCOLON;}
"-:"   {return token::TK_MINUSCOLON;}

"@"    {return token::TK_AT;}
","    {return token::TK_COMMA;}
";"    {return token::TK_SEMICOLON;}
":"    {return token::TK_COLON;}
"\."   {return token::TK_DOT;}

"\("   {return token::TK_LPARENTHESIS;}
"\)"   {return token::TK_RPARENTHESIS;}
"\["   {return token::TK_LBRACKET;}
"\]"   {return token::TK_RBRACKET;}
"\{"   {return token::TK_LBRACE;}
"\}"   {return token::TK_RBRACE;}

"\#"   {return token::TK_DELAY;}
"\$"   {return token::TK_DOLLAR;}


	/**************************************************
	 * Integer number
	 **************************************************/

[0-9][0-9_]* {
   yylval->build<std::string>(yytext);
	return token::TK_INTNUMBER;
}

[0-9]*[ \t]*\'s?[bodhBODH][ \t]*[0-9a-fA-FzxZX?_]+ {
   yylval->build<std::string>(yytext);
	return token::TK_INTNUMBER;
}

	/**************************************************
	 * Float number
	 **************************************************/

[0-9][0-9_]*\.[0-9][0-9_]*([eE][-+]?[0-9_]+)? {
   yylval->build<std::string>(yytext);
	return token::TK_FLOATNUMBER;
}

[0-9][0-9_]*[eE][-+]?[0-9_]+ {
   yylval->build<std::string>(yytext);
	return token::TK_FLOATNUMBER;
}

	/**************************************************
	 * Identifier
	 **************************************************/

[a-zA-Z_][a-zA-Z0-9_$]* {
   yylval->build<std::string>(yytext);
	return token::TK_IDENTIFIER;
}

\\[^ \t\r\n]+ {
   yylval->build<std::string>(yytext);
	return token::TK_IDENTIFIER;
}


	/**************************************************
	 * String
	 **************************************************/

\"            {BEGIN(STRING);}
<STRING>\\\\  {yymore();}
<STRING>\\\"  {yymore();}
<STRING>\n    {LOG_WARNING << "in file \"" << m_filename
                           << "\", line " << loc->begin.line
                           << ": line return in the string";
               loc->lines();
               yymore();}
<STRING>\"    {BEGIN(INITIAL);
               yytext[strlen(yytext)-1] = 0;
               yylval->build<std::string>(yytext);
               return token::TK_STRING;}
<STRING>.     {yymore();}


	/**************************************************
	 * Preprocessor: line
	 **************************************************/

`line                    {BEGIN(VPP_LINE); m_vpp_queue.clear();}

<VPP_LINE>[0-9][0-9_]*   {m_vpp_queue.push_back(yytext);}

<VPP_LINE>[^ \t\r\n]+    {std::string id(yytext);
                          if (id.front() == '"') id.erase(0, 1);
                          if (id.back() == '"') id.pop_back();
                          m_vpp_queue.push_back(id);}

<VPP_LINE>\n             {BEGIN(INITIAL);
                          if (m_vpp_queue.size() >= 2) {
                              int line = atoi(m_vpp_queue[0].c_str());
                              loc->initialize(nullptr, line, 0);
                              m_filename = m_vpp_queue[1];
                          }}

<VPP_LINE>.


	/**************************************************
	 * Preprocessor: timescale
	 **************************************************/

`timescale               {BEGIN(VPP_TIMESCALE);}

<VPP_TIMESCALE>\n        {BEGIN(INITIAL); loc->lines();}

<VPP_TIMESCALE>.


	/**************************************************
	 * Preprocessor: default_nettype
	 **************************************************/

`default_nettype                    {BEGIN(VPP_DEFAULT_NETTYPE); m_vpp_queue.clear();}

<VPP_DEFAULT_NETTYPE>[^ \t\r\n]+    {std::string nt(yytext);
                                     m_vpp_queue.push_back(nt);
                                     if (m_vpp_queue.size() > 1) {
                                         LOG_ERROR << "file \"" << m_filename << "\", line " << loc->begin.line
                                                   << ", cannot define multiple default_nettype in the same directive";
                                         return -1;
                                     }
                                     if (nt == "integer")      m_default_nettype = AST::Module::Default_nettypeEnum::INTEGER;
                                     else if (nt == "real")    m_default_nettype = AST::Module::Default_nettypeEnum::REAL;
                                     else if (nt == "reg")     m_default_nettype = AST::Module::Default_nettypeEnum::REG;
                                     else if (nt == "tri")     m_default_nettype = AST::Module::Default_nettypeEnum::TRI;
                                     else if (nt == "wire")    m_default_nettype = AST::Module::Default_nettypeEnum::WIRE;
                                     else if (nt == "supply0") m_default_nettype = AST::Module::Default_nettypeEnum::SUPPLY0;
                                     else if (nt == "supply1") m_default_nettype = AST::Module::Default_nettypeEnum::SUPPLY1;
                                     else if (nt == "none")    m_default_nettype = AST::Module::Default_nettypeEnum::NONE;
                                     else {
                                         LOG_ERROR << "file \"" << m_filename << "\", line " << loc->begin.line
                                                   << ", unknown default_nettype value: " << nt;
                                         return -1;
                                     }}

<VPP_DEFAULT_NETTYPE>\n             {BEGIN(INITIAL);
                                     if (m_vpp_queue.size() >= 2) {
                                         int line = atoi(m_vpp_queue[0].c_str());
                                         loc->initialize(nullptr, line, 0);
                                         m_filename = m_vpp_queue[1];
                                     }
                                     loc->lines();}

<VPP_DEFAULT_NETTYPE>.


	/**************************************************
	 * Interpreted comments
	 **************************************************/

"/*"[ \t]*(synopsys|synthesis)[ \t]*translate_off[ \t]*"*/"                            {BEGIN(SYNTHESIS_TRANSLATE_OFF1);}
<SYNTHESIS_TRANSLATE_OFF1>\n                                                           {loc->lines();}
<SYNTHESIS_TRANSLATE_OFF1>.
<SYNTHESIS_TRANSLATE_OFF1>"/*"[ \t]*(synopsys|synthesis)[ \t]*"translate_on"[ \t]*"*/" {BEGIN(INITIAL);}


"//"[ \t]*(synopsys|synthesis)[ \t]*translate_off                                      {BEGIN(SYNTHESIS_TRANSLATE_OFF2);}
<SYNTHESIS_TRANSLATE_OFF2>\n                                                           {loc->lines();}
<SYNTHESIS_TRANSLATE_OFF2>.
<SYNTHESIS_TRANSLATE_OFF2>"//"[ \t]*(synopsys|synthesis)[ \t]*"translate_on"           {BEGIN(INITIAL);}


	/**************************************************
	 * Comments
	 **************************************************/

"//"[^\r\n]*

"/*"          {BEGIN(COMMENT);}
<COMMENT>\n   {loc->lines();}
<COMMENT>.
<COMMENT>"*/" {BEGIN(INITIAL);}


	/**************************************************
	 * Whitespaces
	 **************************************************/

(\\)?[ \t\r]  /* remove whitespaces */

(\\)?\n       {loc->lines();}


%%
