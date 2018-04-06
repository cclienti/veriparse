%skeleton "lalr1.cc"
%require  "3.0"

%defines
%locations

%define api.namespace {Veriparse::Parser}
%define parser_class_name {VerilogParser}

%define lr.type lalr
%define api.value.type variant
%define parse.assert
%define parse.error verbose

%parse-param {VerilogScanner &scanner}
%parse-param {VerilogDriver &driver}

%code requires {
#include <veriparse/AST/nodes.hpp>
#include <veriparse/misc/string_utils.hpp>
#include <veriparse/misc/math.hpp>
#include <veriparse/logger/logger.hpp>
#include <veriparse/logger/file_report.hpp>
#include <veriparse/parser/verilog_number.hpp>

#include <verilog/location.hh>
#include <sstream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <cstdlib>


namespace Veriparse {
    namespace Parser {
        class VerilogDriver;
        class VerilogScanner;

        enum class net_type_t {
            INTEGER, REAL, REG, TRI, WIRE, SUPPLY0, SUPPLY1, NONE
        };

        enum class direction_t {
            INPUT, INOUT, OUTPUT, NONE
        };

        typedef struct {
            direction_t direction;
            net_type_t net_type;
            bool is_signed;
            AST::Width::ListPtr widths;
            std::string name;
            location loc;
        } port_info_t;

        typedef struct {
            std::string name;
            AST::DelayStatement::Ptr ldelay;
            AST::DelayStatement::Ptr rdelay;
            AST::Length::ListPtr lengths;
            AST::Rvalue::Ptr rvalue;
        } decl_name_t;

        namespace ParserHelpers {
            AST::Variable::Ptr create_net_type(const decl_name_t &decl, net_type_t nt,
                                                       AST::Width::ListPtr widths, bool sign,
                                                       const std::string &filename="", uint32_t line=0);

            AST::Ioport::Ptr create_ioport_decls(direction_t direction, net_type_t net_type, bool is_signed,
                                                       AST::Width::ListPtr widths,  std::string name,
                                                       const std::string &filename="", uint32_t line=0);

            // if return null, the create_ports_decls failed. Error information are set in loc and error_message.
            AST::Node::ListPtr create_ports_decls(const std::list<port_info_t> &port_list,
                                                      const std::string &filename,
                                                      location &loc, std::string &error_message);
        }
    }
}

// The following definitions is missing when %locations isn't used
# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif
}

%code {
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <memory>

/* include for all driver functions */
#include <parser/verilog/verilog_driver.hpp>
#undef yylex
#define yylex scanner.yylex
}

// Token declarations

%token                  TK_MODULE       "'module'"
%token                  TK_ENDMODULE    "'endmodule'"
%token                  TK_BEGIN        "'begin'"
%token                  TK_END          "'end'"
%token                  TK_GENERATE     "'generate'"
%token                  TK_ENDGENERATE  "'endgenerate'"
%token                  TK_GENVAR       "'genvar'"
%token                  TK_FUNCTION     "'function'"
%token                  TK_ENDFUNCTION  "'endfunction'"
%token                  TK_TASK         "'task'"
%token                  TK_ENDTASK      "'endtask'"
%token                  TK_INPUT        "'input'"
%token                  TK_INOUT        "'inout'"
%token                  TK_OUTPUT       "'output'"
%token                  TK_TRI          "'tri'"
%token                  TK_REG          "'reg'"
%token                  TK_WIRE         "'wire'"
%token                  TK_INTEGER      "'integer'"
%token                  TK_REAL         "'real'"
%token                  TK_SIGNED       "'signed'"
%token                  TK_PARAMETER    "'parameter'"
%token                  TK_LOCALPARAM   "'localparam'"
%token                  TK_SUPPLY0      "'supply0'"
%token                  TK_SUPPLY1      "'supply1'"
%token                  TK_AUTOMATIC    "'automatic'"
%token                  TK_ASSIGN       "'assign'"
%token                  TK_ALWAYS       "'always'"
%token                  TK_SENS_OR      "'or'"
%token                  TK_POSEDGE      "'posedge'"
%token                  TK_NEGEDGE      "'negedge'"
%token                  TK_INITIAL      "'initial'"
%token                  TK_IF           "'if'"
%token                  TK_ELSE         "'else'"
%token                  TK_FOR          "'for'"
%token                  TK_WHILE        "'while'"
%token                  TK_CASE         "'case'"
%token                  TK_CASEX        "'casex'"
%token                  TK_CASEZ        "'casez'"
%token                  TK_ENDCASE      "'endcase'"
%token                  TK_DEFAULT      "'default'"
%token                  TK_WAIT         "'wait'"
%token                  TK_FOREVER      "'forever'"
%token                  TK_DISABLE      "'disable'"
%token                  TK_FORK         "'fork'"
%token                  TK_JOIN         "'join'"
%token                  TK_VPP_LINE     "`line'"
%token                  TK_LOR          "'||'"
%token                  TK_LAND         "'&&'"
%token                  TK_NOR          "'~|'"
%token                  TK_NAND         "'~&'"
%token                  TK_XNOR         "'~^'"
%token                  TK_OR           "'|'"
%token                  TK_AND          "'&'"
%token                  TK_XOR          "'^'"
%token                  TK_LNOT         "'!'"
%token                  TK_NOT          "'~'"
%token                  TK_LSHIFTA      "'<<<'"
%token                  TK_RSHIFTA      "'>>>'"
%token                  TK_LSHIFT       "'<<'"
%token                  TK_RSHIFT       "'>>'"
%token                  TK_EQL          "'==='"
%token                  TK_NEL          "'!=='"
%token                  TK_EQ           "'=='"
%token                  TK_NE           "'!='"
%token                  TK_LE           "'<='"
%token                  TK_GE           "'>='"
%token                  TK_LT           "'<'"
%token                  TK_GT           "'>'"
%token                  TK_POWER        "'**'"
%token                  TK_PLUS         "'+'"
%token                  TK_MINUS        "'-'"
%token                  TK_TIMES        "'*'"
%token                  TK_DIVIDE       "'/'"
%token                  TK_MOD          "'%'"
%token                  TK_COND         "'?'"
%token                  TK_EQUALS       "'='"
%token                  TK_PLUSCOLON    "'+:'"
%token                  TK_MINUSCOLON   "'-:'"
%token                  TK_AT           "'@'"
%token                  TK_COMMA        "','"
%token                  TK_SEMICOLON    "';'"
%token                  TK_COLON        "':'"
%token                  TK_DOT          "'.'"
%token                  TK_LPARENTHESIS "'('"
%token                  TK_RPARENTHESIS "')'"
%token                  TK_LBRACKET     "'['"
%token                  TK_RBRACKET     "']'"
%token                  TK_LBRACE       "'{'"
%token                  TK_RBRACE       "'}'"
%token                  TK_DELAY        "'#'"
%token                  TK_DOLLAR       "'$'"
%token  <std::string>   TK_INTNUMBER    "'integer const'"
%token  <std::string>   TK_FLOATNUMBER  "'floating const'"
%token  <std::string>   TK_IDENTIFIER   "identifier"
%token  <std::string>   TK_STRING       "string"


// Token precedences
%left                   TK_RPARENTHESIS
%left                   TK_ELSE

%left                   TK_COLON
%left                   TK_COND
%left                   TK_LOR
%left                   TK_LAND
%left                   TK_OR
%left                   TK_AND TK_XOR TK_XNOR
%left                   TK_EQ TK_NE TK_EQL TK_NEL
%left                   TK_LT TK_GT TK_LE TK_GE
%left                   TK_LSHIFT TK_RSHIFT TK_LSHIFTA TK_RSHIFTA
%left                   TK_PLUS TK_MINUS
%left                   TK_TIMES TK_DIVIDE TK_MOD
%left                   TK_POWER
%right                  TK_UMINUS TK_UPLUS TK_ULNOT TK_UNOT TK_UAND TK_UNAND TK_UOR TK_UNOR TK_UXOR TK_UXNOR
%right                  TK_DOLLAR


// Rule types

%type   <std::string>                        modulename
%type   <std::list<port_info_t>>             portinfo_list ioports
%type   <port_info_t>                        portinfo portname portname_reg_net ioport
%type   <direction_t>                        portdir
%type   <net_type_t>                         net_type reg_net_type

%type   <std::list<decl_name_t>>             net_decl_namelist var_decl_namelist
%type   <decl_name_t>                        net_decl_name var_decl_name

%type   <AST::Source::Ptr>                   source_text
%type   <AST::Description::Ptr>              description
%type   <AST::Node::ListPtr>                 definitions
%type   <AST::Node::Ptr>                     definition
%type   <AST::Module::Ptr>                   moduledef
%type   <AST::Pragma::Ptr>                   pragma
%type   <AST::Node::Ptr>                     expression ternary paren_expression
%type   <AST::Node::ListPtr>                 items item standard_item
%type   <AST::Node::ListPtr>                 ioports_decl
%type   <AST::Node::ListPtr>                 net_decl
%type   <AST::Node::ListPtr>                 reg_decl
%type   <AST::Node::ListPtr>                 integer_decl
%type   <AST::Node::ListPtr>                 real_decl

%type   <AST::GenerateStatement::Ptr>        generate
%type   <AST::Node::ListPtr>                 generate_items
%type   <AST::Node::ListPtr>                 generate_item
%type   <AST::IfStatement::Ptr>              generate_if
%type   <AST::CaseStatement::Ptr>            generate_case
%type   <AST::Case::ListPtr>                 generate_case_statements
%type   <AST::Case::Ptr>                     generate_case_statement
%type   <AST::Block::Ptr>                    generate_block generate_content
%type   <AST::ForStatement::Ptr>             generate_for

%type   <AST::Genvar::ListPtr>               genvar_decl genvarlist
%type   <AST::Genvar::Ptr>                   genvar

%type   <AST::Instancelist::Ptr>             instance
%type   <AST::Instance::Ptr>                 instance_body_noname instance_body
%type   <AST::Instance::ListPtr>             instance_bodylist_noname instance_bodylist
%type   <AST::PortArg::Ptr>                  instance_port_arg instance_port_list
%type   <AST::PortArg::ListPtr>              instance_ports instance_ports_arg instance_ports_list
%type   <AST::ParamArg::Ptr>                 param_arg param_arg_noname
%type   <AST::ParamArg::ListPtr>             parameterlist param_args param_args_noname

%type   <AST::Node::Ptr>                     assignment
%type   <AST::Always::Ptr>                   always
%type   <AST::Initial::Ptr>                  initial
%type   <AST::Node::Ptr>                     initial_statement
%type   <AST::Parameter::ListPtr>            params_block params param_assignment_list parameter_decl
%type   <AST::Parameter::Ptr>                param_assignment param_type param_start
%type   <AST::Localparam::ListPtr>           localparam_assignment_list localparam_decl
%type   <AST::Localparam::Ptr>               localparam_assignment localparam_type
%type   <AST::Width::ListPtr>                widths
%type   <AST::Width::Ptr>                    width
%type   <AST::Length::Ptr>                   length
%type   <AST::Length::ListPtr>               lengths
%type   <AST::Node::ListPtr>                 ports_block ports
%type   <AST::Lvalue::Ptr>                   lvalue
%type   <AST::Rvalue::Ptr>                   rvalue
%type   <AST::DelayStatement::Ptr>           delays
%type   <AST::Node::Ptr>                     const_expression
%type   <AST::Node::Ptr>                     pointer lpointer lpartselect
%type   <AST::Identifier::Ptr>               identifier
%type   <AST::IdentifierScope::Ptr>          scope
%type   <AST::Lconcat::Ptr>                  lconcat
%type   <AST::Node::ListPtr>                 lconcatlist
%type   <AST::Node::Ptr>                     lconcat_one
%type   <AST::Concat::Ptr>                   concat
%type   <AST::Node::ListPtr>                 concatlist
%type   <AST::Repeat::Ptr>                   repeat
%type   <AST::Node::Ptr>                     partselect
%type   <AST::FloatConst::Ptr>               floatnumber
%type   <AST::Node::Ptr>                     intnumber
%type   <AST::StringConst::Ptr>              stringliteral
%type   <AST::SystemCall::Ptr>               systemcall
%type   <AST::Node::ListPtr>                 sysargs
%type   <AST::Node::Ptr>                     sysarg
%type   <AST::Senslist::Ptr>                 senslist
%type   <AST::Sens::ListPtr>                 edgesigs
%type   <AST::Sens::Ptr>                     edgesig
%type   <AST::Sens::Ptr>                     edgesig_base
%type   <AST::Sens::ListPtr>                 levelsigs
%type   <AST::Sens::Ptr>                     levelsig
%type   <AST::Sens::Ptr>                     levelsig_base
%type   <AST::Node::Ptr>                     basic_statement
%type   <AST::IfStatement::Ptr>              if_statement
%type   <AST::Node::Ptr>                     cond true_statement false_statement ifcontent_statement

%type   <AST::CaseStatement::Ptr>            case_statement
%type   <AST::CasexStatement::Ptr>           casex_statement
%type   <AST::CasezStatement::Ptr>           casez_statement
%type   <AST::Node::Ptr>                     case_comp
%type   <AST::Case::ListPtr>                 casecontent_statements
%type   <AST::Case::Ptr>                     casecontent_statement
%type   <AST::Node::ListPtr>                 casecontent_conditions

%type   <AST::ForStatement::Ptr>             for_statement
%type   <AST::BlockingSubstitution::Ptr>     forpre forpost
%type   <AST::Node::Ptr>                     forcond forcontent_statement

%type   <AST::WhileStatement::Ptr>           while_statement
%type   <AST::Node::Ptr>                     whilecontent_statement
%type   <AST::EventStatement::Ptr>           event_statement
%type   <AST::WaitStatement::Ptr>            wait_statement
%type   <AST::Node::Ptr>                     waitcontent_statement
%type   <AST::ForeverStatement::Ptr>         forever_statement
%type   <AST::Block::Ptr>                    block named_block
%type   <AST::Node::ListPtr>                 block_statements block_statement
%type   <AST::ParallelBlock::Ptr>            parallelblock named_parallelblock

%type   <AST::BlockingSubstitution::Ptr>     blocking_assignment
%type   <AST::BlockingSubstitution::Ptr>     blocking_assignment_base
%type   <AST::NonblockingSubstitution::Ptr>  nonblocking_assignment
%type   <AST::SingleStatement::Ptr>          single_statement

%type   <AST::Node::ListPtr>                 function_ioports_decl
%type   <std::list<port_info_t>>             function_portinfo_list
%type   <port_info_t>                        function_ioport function_portinfo
%type   <direction_t>                        function_portdir
%type   <AST::FunctionCall::Ptr>             function_call
%type   <AST::Node::ListPtr>                 function_args
%type   <AST::Node::ListPtr>                 function_ports_block function_ports
%type   <AST::Function::Ptr>                 function function_rettype
%type   <AST::Node::ListPtr>                 function_statement function_statements
%type   <AST::Node::ListPtr>                 funcvar_decl function_calc

%type   <AST::Task::Ptr>                     task
%type   <AST::Node::ListPtr>                 task_ioports_decl
%type   <std::list<port_info_t>>             task_portinfo_list
%type   <port_info_t>                        task_ioport task_portinfo
%type   <direction_t>                        task_portdir
%type   <AST::TaskCall::Ptr>                 task_call
%type   <AST::Node::ListPtr>                 task_args
%type   <AST::Node::ListPtr>                 task_ports_block task_ports
%type   <AST::Node::ListPtr>                 task_statement task_statements
%type   <AST::Node::ListPtr>                 taskvar_decl task_calc

%type   <AST::Disable::Ptr>                  disable
%type   <bool>                               automatic


%%

// Rules

source_text:    description
                {
                    driver.set_source_node(std::make_shared<AST::Source>($1, scanner.get_filename(), @1.begin.line));
                }
        ;


description:    definitions
                {
                    $$ = std::make_shared<AST::Description>($1, scanner.get_filename(), @1.begin.line);
                }
        ;


definitions:    definition
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back($1);
                }

        |       definitions definition
                {
                    $$ = $1;
                    $$->push_back($2);
                }
        ;


definition:     moduledef
                {
	                $$ = AST::to_node($1);
                }

        |       pragma
                {
	                $$ = AST::to_node($1);
                }
        ;


pragma:         TK_LPARENTHESIS TK_TIMES TK_IDENTIFIER TK_EQUALS expression TK_TIMES TK_RPARENTHESIS
                {
                    $$ = std::make_shared<AST::Pragma>($5, $3, scanner.get_filename(), @1.begin.line);
                }

        |       TK_LPARENTHESIS TK_TIMES TK_IDENTIFIER TK_TIMES TK_RPARENTHESIS
                {
                    $$ = std::make_shared<AST::Pragma>(scanner.get_filename(), @1.begin.line);
                    $$->set_name($3);
                }
        ;


moduledef:      TK_MODULE modulename params_block ports_block items TK_ENDMODULE
                {
                    $$ = std::make_shared<AST::Module>($3, $4, $5, $2, scanner.get_default_nettype(),
                                                       scanner.get_filename(), @1.begin.line);
                }

        |       TK_MODULE modulename ports_block items TK_ENDMODULE
                {
                    $$ = std::make_shared<AST::Module>(scanner.get_filename(), @1.begin.line);
                    $$->set_default_nettype(scanner.get_default_nettype());
                    $$->set_name($2);
                    $$->set_ports($3);
                    $$->set_items($4);
                }

        |       TK_MODULE modulename params_block ports_block TK_ENDMODULE
                {
                    $$ = std::make_shared<AST::Module>(scanner.get_filename(), @1.begin.line);
                    $$->set_default_nettype(scanner.get_default_nettype());
                    $$->set_name($2);
                    $$->set_params($3);
                    $$->set_ports($4);
                }

        |       TK_MODULE modulename ports_block TK_ENDMODULE
                {
                    $$ = std::make_shared<AST::Module>(scanner.get_filename(), @1.begin.line);
                    $$->set_default_nettype(scanner.get_default_nettype());
                    $$->set_name($2);
                    $$->set_ports($3);
                }
        ;


modulename:     TK_IDENTIFIER
                {
                    $$=$1;
                }

        |       TK_SENS_OR { $$ = "or"; }
        ;


params_block:   TK_DELAY TK_LPARENTHESIS params TK_RPARENTHESIS
                {
                    $$ = $3;
                }
        ;


params:         params TK_COMMA param_start
                {
                    $$ = $1;
                    $$->push_back($3);
                }

        |       params TK_COMMA param_assignment
                {
                    $$ = $1;
                    AST::Parameter::Ptr param = AST::cast_to<AST::Parameter>($1->back()->clone());
                    param->set_line($3->get_line());
                    param->set_filename($3->get_filename());
                    param->set_name($3->get_name());
                    param->set_value($3->get_value());
                    param->set_type($3->get_type());
                    $$->push_back(param);
                }

        |       param_start
                {
                    $$ = std::make_shared<AST::Parameter::List>();
                    $$->push_back($1);
                }
        ;


param_start:    TK_PARAMETER param_assignment
                {
                    $$ = $2;
                }

        |       TK_PARAMETER param_type param_assignment
                {
                    $$ = $2;
                    $$->set_name($3->get_name());
                    $$->set_value($3->get_value());
                }
        ;


ports_block:    TK_LPARENTHESIS ports TK_RPARENTHESIS TK_SEMICOLON
                {
                    $$ = $2;
                }

        |       TK_LPARENTHESIS TK_RPARENTHESIS TK_SEMICOLON
                {
                    $$ = nullptr;
                }

        |       TK_SEMICOLON
                {
                    $$ = nullptr;
                }
        ;


ports:          portinfo_list
                {
                    location loc;
                    std::string error_message;
                    $$ = ParserHelpers::create_ports_decls($1, scanner.get_filename(), loc, error_message);
                    if(!$$) error(loc, error_message);
                }
        ;


portinfo_list:  portinfo_list TK_COMMA portinfo
                {
                    $$ = $1;
                    $3.loc = @3;
                    $$.push_back($3);
                }

        |       portinfo
                {
                    $$.clear();
                    $1.loc = @1;
                    $$.push_back($1);
                }
        ;


portinfo:       portname
                {
                    $$ = $1;
                }

        |       portname_reg_net
                {
                    $$ = $1;
                }

        |       ioport
                {
                    $$ = $1;
                }
        ;


portname_reg_net:
                reg_net_type portname
                {
                    $$ = $2;
                    $$.net_type = $1;
                }
        ;


portname:       TK_IDENTIFIER
                {
                    $$.direction = direction_t::NONE;
                    $$.net_type = net_type_t::NONE;
                    $$.is_signed = false;
                    $$.widths = nullptr;
                    $$.name = $1;
                }

        |       widths TK_IDENTIFIER
                {
                    $$.direction = direction_t::NONE;
                    $$.net_type = net_type_t::NONE;
                    $$.is_signed = false;
                    $$.widths = $1;
                    $$.name = $2;
                }

        |       TK_SIGNED widths TK_IDENTIFIER
                {
                    $$.direction = direction_t::NONE;
                    $$.net_type = net_type_t::NONE;
                    $$.is_signed = true;
                    $$.widths = $2;
                    $$.name = $3;
                }

        |       TK_SIGNED TK_IDENTIFIER
                {
                    error(@2, "a range is missing");
                }
        ;


portdir:        TK_INPUT
                {
                    $$ = direction_t::INPUT;
                }

        |       TK_INOUT
                {
                    $$ = direction_t::INOUT;
                }

        |       TK_OUTPUT
                {
                    $$ = direction_t::OUTPUT;
                }
        ;


ioports_decl:   ioports TK_SEMICOLON
                {
                    location loc;
                    std::string error_message;
                    $$ = ParserHelpers::create_ports_decls($1, scanner.get_filename(), loc, error_message);
                    if(!$$) error(loc, error_message);
                }
        ;


ioports:        ioports TK_COMMA portname
                {
                    $$ = $1;
                    $3.loc = @3;
                    $$.push_back($3);
                }

        |       ioports TK_COMMA portname_reg_net
                {
                    $$ = $1;
                    $3.loc = @3;
                    $$.push_back($3);
                }

        |       ioport
                {
                    $$.clear();
                    $1.loc = @1;
                    $$.push_back($1);
                }
        ;


ioport:         portdir portname
                {
                    $$ = $2;
                    $$.direction = $1;
                }

        |       portdir reg_net_type portname
                {
                    $$ = $3;
                    $$.direction = $1;
                    $$.net_type = $2;
                }
        ;


reg_net_type:   TK_REG   {$$ = net_type_t::REG;}
        |       net_type {$$ = $1;}
        ;


net_type:       TK_TRI      {$$ = net_type_t::TRI;}
        |       TK_WIRE     {$$ = net_type_t::WIRE;}
        |       TK_SUPPLY0  {$$ = net_type_t::SUPPLY0;}
        |       TK_SUPPLY1  {$$ = net_type_t::SUPPLY1;}
        ;


widths:         widths width
                {
                    $$ = $1;
                    $$->push_back($2);
                }

        |       width
                {
                    $$ = std::make_shared<AST::Width::List>();
                    $$->push_back($1);
                }
        ;

width:          TK_LBRACKET expression TK_COLON expression TK_RBRACKET
                {
                    $$ = std::make_shared<AST::Width>($2, $4, scanner.get_filename(), @1.begin.line);
                }
        ;


lengths:        lengths length
                {
                    $$=$1;
                    $$->push_back($2);
                }

        |       length
                {
                    $$ = std::make_shared<AST::Length::List>();
                    $$->push_back($1);
                }
        ;


length:         TK_LBRACKET expression TK_COLON expression TK_RBRACKET
                {
                    $$ = std::make_shared<AST::Length>($2, $4, scanner.get_filename(), @1.begin.line);
                }
        ;


items:          items item
                {
                    $$ = $1;
                    $$->splice($1->end(), *$2);
                }

        |       item
                {
                    $$ = $1;
                }
        ;


item:           standard_item
                {
                    $$ = $1;
                }

        |       generate
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back(AST::to_node($1));
                }
        ;


standard_item:  ioports_decl
                {
                    $$ = $1;
                }

        |       net_decl
                {
                    $$ = $1;
                }

        |       reg_decl
                {
                    $$ = $1;
                }

        |       integer_decl
                {
                    $$ = $1;
                }

        |       real_decl
                {
                    $$ = $1;
                }

        |       parameter_decl
                {
                    $$ = AST::cast_list_to<AST::Node>($1);
                }

        |       localparam_decl
                {
                    $$ = AST::cast_list_to<AST::Node>($1);
                }

        |       genvar_decl
                {
                    $$ = AST::cast_list_to<AST::Node>($1);
                }

        |       instance
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back(AST::to_node($1));
                }

        |       assignment
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back($1);
                }

        |       always
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back(AST::to_node($1));
                }

        |       initial
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back(AST::to_node($1));
                }

        |       function
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back(AST::to_node($1));
                }

        |       task
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back(AST::to_node($1));
                }

        |       pragma
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back(AST::to_node($1));
                }
        ;


net_decl:       net_type TK_SIGNED widths net_decl_namelist TK_SEMICOLON
                {
                    $$ = std::make_shared<AST::Node::List>();
                    for(const decl_name_t &decl_name: $4) {
                        AST::Width::ListPtr widths = AST::Width::clone_list($3);
                        AST::Variable::Ptr net = ParserHelpers::create_net_type(decl_name, $1, widths, true);
                        net->set_filename(scanner.get_filename());
                        net->set_line(@1.begin.line);
                        $$->push_back(net);
                    }
                }

        |       net_type widths net_decl_namelist TK_SEMICOLON
                {
                    $$ = std::make_shared<AST::Node::List>();
                    for(const decl_name_t &decl_name: $3) {
                        AST::Width::ListPtr widths = AST::Width::clone_list($2);
                        AST::Variable::Ptr net = ParserHelpers::create_net_type(decl_name, $1, widths, false);
                        net->set_filename(scanner.get_filename());
                        net->set_line(@1.begin.line);
                        $$->push_back(net);
                    }
                }

        |       net_type net_decl_namelist TK_SEMICOLON
                {
                    $$ = std::make_shared<AST::Node::List>();
                    for(const decl_name_t &decl_name: $2) {
                        AST::Variable::Ptr net = ParserHelpers::create_net_type(decl_name, $1,
                                                                          AST::Width::ListPtr(), false);
                        net->set_filename(scanner.get_filename());
                        net->set_line(@1.begin.line);
                        $$->push_back(net);
                    }
                }
        ;


reg_decl:       TK_REG TK_SIGNED widths var_decl_namelist TK_SEMICOLON
                {
                    $$ = std::make_shared<AST::Node::List>();
                    for(const decl_name_t &decl_name: $4) {
                        AST::Width::ListPtr widths = AST::Width::clone_list($3);
                        AST::Variable::Ptr net = ParserHelpers::create_net_type(decl_name, net_type_t::REG, widths, true);
                        net->set_filename(scanner.get_filename());
                        net->set_line(@1.begin.line);
                        $$->push_back(net);
                    }
                }

        |       TK_REG widths var_decl_namelist TK_SEMICOLON
                {
                    $$ = std::make_shared<AST::Node::List>();
                    for(const decl_name_t &decl_name: $3) {
                        AST::Width::ListPtr widths = AST::Width::clone_list($2);
                        AST::Variable::Ptr net = ParserHelpers::create_net_type(decl_name, net_type_t::REG, widths, false);
                        net->set_filename(scanner.get_filename());
                        net->set_line(@1.begin.line);
                        $$->push_back(net);
                    }
                }

        |       TK_REG var_decl_namelist TK_SEMICOLON
                {
                    $$ = std::make_shared<AST::Node::List>();
                    for(const decl_name_t &decl_name: $2) {
                        AST::Variable::Ptr net = ParserHelpers::create_net_type(decl_name, net_type_t::REG,
                                                                          AST::Width::ListPtr(), false);
                        net->set_filename(scanner.get_filename());
                        net->set_line(@1.begin.line);
                        $$->push_back(net);
                    }
                }
        ;


integer_decl:   TK_INTEGER var_decl_namelist TK_SEMICOLON
                {
                    $$ = std::make_shared<AST::Node::List>();
                    for(const decl_name_t &decl_name: $2) {
                        AST::Variable::Ptr net = ParserHelpers::create_net_type(decl_name, net_type_t::INTEGER,
                                                                          AST::Width::ListPtr(), true);
                        net->set_filename(scanner.get_filename());
                        net->set_line(@1.begin.line);
                        $$->push_back(net);
                    }
                }
        ;


real_decl:      TK_REAL var_decl_namelist TK_SEMICOLON
                {
                    $$ = std::make_shared<AST::Node::List>();
                    for(const decl_name_t &decl_name: $2) {
                        AST::Variable::Ptr net = ParserHelpers::create_net_type(decl_name, net_type_t::REAL,
                                                                          AST::Width::ListPtr(), true);
                        net->set_filename(scanner.get_filename());
                        net->set_line(@1.begin.line);
                        $$->push_back(net);
                    }
                }
        ;


net_decl_namelist:
                net_decl_namelist TK_COMMA net_decl_name
                {
                    $$ = $1;
                    $$.push_back($3);
                }

        |       net_decl_name
                {
                    $$ = std::list<decl_name_t>();
                    $$.push_back($1);
                }
        ;


net_decl_name:  TK_IDENTIFIER
                {
                    $$.ldelay = AST::DelayStatement::Ptr();
                    $$.name = $1;
                    $$.lengths = AST::Length::ListPtr();
                    $$.rdelay = AST::DelayStatement::Ptr();
                    $$.rvalue = AST::Rvalue::Ptr();
                }

        |       TK_IDENTIFIER lengths
                {
                    $$.ldelay = AST::DelayStatement::Ptr();
                    $$.name = $1;
                    $$.lengths = $2;
                    $$.rdelay = AST::DelayStatement::Ptr();
                    $$.rvalue = AST::Rvalue::Ptr();
                }

        |       delays TK_IDENTIFIER TK_EQUALS delays rvalue
                {
                    $$.ldelay = $1;
                    $$.name = $2;
                    $$.lengths = AST::Length::ListPtr();
                    $$.rdelay = $4;
                    $$.rvalue = $5;
                }

        |       delays TK_IDENTIFIER TK_EQUALS rvalue
                {
                    $$.ldelay = $1;
                    $$.name = $2;
                    $$.lengths = AST::Length::ListPtr();
                    $$.rdelay = AST::DelayStatement::Ptr();
                    $$.rvalue = $4;
                }

        |       TK_IDENTIFIER TK_EQUALS delays rvalue
                {
                    $$.ldelay = AST::DelayStatement::Ptr();
                    $$.name = $1;
                    $$.lengths = AST::Length::ListPtr();
                    $$.rdelay = $3;
                    $$.rvalue = $4;
                }

        |       TK_IDENTIFIER TK_EQUALS rvalue
                {
                    $$.ldelay = AST::DelayStatement::Ptr();
                    $$.name = $1;
                    $$.lengths = AST::Length::ListPtr();
                    $$.rdelay = AST::DelayStatement::Ptr();
                    $$.rvalue = $3;
                }
        ;


var_decl_namelist:
                var_decl_namelist TK_COMMA var_decl_name
                {
                    $$ = $1;
                    $$.push_back($3);
                }

        |       var_decl_name
                {
                    $$ = std::list<decl_name_t>();
                    $$.push_back($1);
                }
        ;


var_decl_name:  TK_IDENTIFIER
                {
                    $$.ldelay = AST::DelayStatement::Ptr();
                    $$.name = $1;
                    $$.lengths = AST::Length::ListPtr();
                    $$.rdelay = AST::DelayStatement::Ptr();
                    $$.rvalue = AST::Rvalue::Ptr();
                }

        |       TK_IDENTIFIER lengths
                {
                    $$.ldelay = AST::DelayStatement::Ptr();
                    $$.name = $1;
                    $$.lengths = $2;
                    $$.rdelay = AST::DelayStatement::Ptr();
                    $$.rvalue = AST::Rvalue::Ptr();
                }

        |       TK_IDENTIFIER TK_EQUALS const_expression
                {
                    $$.ldelay = AST::DelayStatement::Ptr();
                    $$.name = $1;
                    $$.lengths = AST::Length::ListPtr();
                    $$.rdelay = AST::DelayStatement::Ptr();
                    $$.rvalue = std::make_shared<AST::Rvalue>($3, scanner.get_filename(), @1.begin.line);
                }
        ;


parameter_decl: TK_PARAMETER param_assignment_list TK_SEMICOLON
                {
                    $$ = $2;
                }

        |       TK_PARAMETER param_type param_assignment_list TK_SEMICOLON
                {
                    $$ = $3;
                    for(AST::Parameter::Ptr param: *$$) {
                        AST::Width::ListPtr widths = $2->get_widths();
                        if (widths) {
                            param->set_widths(AST::Width::clone_list(widths));
                        }
                        param->set_sign($2->get_sign());
                        param->set_type($2->get_type());
                    }
                }
        ;


localparam_decl:
                TK_LOCALPARAM localparam_assignment_list TK_SEMICOLON
                {
                    $$ = $2;
                }

        |       TK_LOCALPARAM localparam_type localparam_assignment_list TK_SEMICOLON
                {
                    $$ = $3;
                    for(AST::Localparam::Ptr lparam: *$$) {
                        AST::Width::ListPtr widths = $2->get_widths();
                        if (widths) {
                            lparam->set_widths(AST::Width::clone_list(widths));
                        }
                        lparam->set_sign($2->get_sign());
                        lparam->set_type($2->get_type());
                    }
                }
        ;


param_type:     widths
                {
                    $$ = std::make_shared<AST::Parameter>(scanner.get_filename(), @1.begin.line);
                    $$->set_sign(false);
                    $$->set_widths($1);
                    $$->set_type(AST::Parameter::TypeEnum::NONE);
                }

        |       TK_SIGNED widths
                {
                    $$ = std::make_shared<AST::Parameter>(scanner.get_filename(), @1.begin.line);
                    $$->set_sign(true);
                    $$->set_widths($2);
                    $$->set_type(AST::Parameter::TypeEnum::NONE);
                }

        |       TK_SIGNED
                {
                    $$ = std::make_shared<AST::Parameter>(scanner.get_filename(), @1.begin.line);
                    $$->set_sign(true);
                    $$->set_widths(nullptr);
                    $$->set_type(AST::Parameter::TypeEnum::NONE);
                }

        |       TK_INTEGER
                {
                    $$ = std::make_shared<AST::Parameter>(scanner.get_filename(), @1.begin.line);
                    $$->set_sign(true);
                    $$->set_widths(nullptr);
                    $$->set_type(AST::Parameter::TypeEnum::INTEGER);
                }

        |       TK_REAL
                {
                    $$ = std::make_shared<AST::Parameter>(scanner.get_filename(), @1.begin.line);
                    $$->set_sign(true);
                    $$->set_widths(nullptr);
                    $$->set_type(AST::Parameter::TypeEnum::REAL);
                }
        ;


localparam_type:widths
                {
                    $$ = std::make_shared<AST::Localparam>(scanner.get_filename(), @1.begin.line);
                    $$->set_sign(false);
                    $$->set_widths($1);
                    $$->set_type(AST::Parameter::TypeEnum::NONE);
                }

        |       TK_SIGNED widths
                {
                    $$ = std::make_shared<AST::Localparam>(scanner.get_filename(), @1.begin.line);
                    $$->set_sign(true);
                    $$->set_widths($2);
                    $$->set_type(AST::Parameter::TypeEnum::NONE);
                }

        |       TK_SIGNED
                {
                    $$ = std::make_shared<AST::Localparam>(scanner.get_filename(), @1.begin.line);
                    $$->set_sign(true);
                    $$->set_widths(nullptr);
                    $$->set_type(AST::Parameter::TypeEnum::NONE);
                }

        |       TK_INTEGER
                {
                    $$ = std::make_shared<AST::Localparam>(scanner.get_filename(), @1.begin.line);
                    $$->set_sign(true);
                    $$->set_widths(nullptr);
                    $$->set_type(AST::Parameter::TypeEnum::INTEGER);
                }

        |       TK_REAL
                {
                    $$ = std::make_shared<AST::Localparam>(scanner.get_filename(), @1.begin.line);
                    $$->set_sign(true);
                    $$->set_widths(nullptr);
                    $$->set_type(AST::Parameter::TypeEnum::REAL);
                }
        ;


param_assignment_list:
                param_assignment_list TK_COMMA param_assignment
                {
                    $$ = $1;
                    $1->push_back($3);
                }

        |       param_assignment
                {
                    $$ = std::make_shared<AST::Parameter::List>();
                    $$->push_back($1);
                }
        ;


param_assignment:
                TK_IDENTIFIER TK_EQUALS rvalue
                {
                    $$ = std::make_shared<AST::Parameter>(scanner.get_filename(), @1.begin.line);
                    $$->set_name($1);
                    $$->set_value($3);
                    $$->set_sign(false);
                    $$->set_type(AST::Parameter::TypeEnum::NONE);
                }

        |       TK_IDENTIFIER
                {
                    $$ = std::make_shared<AST::Parameter>(scanner.get_filename(), @1.begin.line);
                    $$->set_name($1);
                    $$->set_sign(false);
                    $$->set_type(AST::Parameter::TypeEnum::NONE);
                }
        ;


localparam_assignment_list:
                localparam_assignment_list TK_COMMA localparam_assignment
                {
                    $$ = $1;
                    $1->push_back($3);
                }

        |       localparam_assignment
                {
                    $$ = std::make_shared<AST::Localparam::List>();
                    $$->push_back($1);
                }
        ;


localparam_assignment:
                TK_IDENTIFIER TK_EQUALS rvalue
                {
                    $$ = std::make_shared<AST::Localparam>(scanner.get_filename(), @1.begin.line);
                    $$->set_name($1);
                    $$->set_value($3);
                    $$->set_sign(false);
                    $$->set_type(AST::Parameter::TypeEnum::NONE);
                }
        ;


assignment:     TK_ASSIGN lvalue TK_EQUALS rvalue TK_SEMICOLON
                {
                    AST::Assign::Ptr assign = std::make_shared<AST::Assign>(scanner.get_filename(), @1.begin.line);
                    assign->set_left($2);
                    assign->set_right($4);
                    $$ = AST::to_node(assign);
                }

        |       TK_ASSIGN lvalue TK_EQUALS delays rvalue TK_SEMICOLON
                {
                    AST::Assign::Ptr assign = std::make_shared<AST::Assign>(scanner.get_filename(), @1.begin.line);
                    assign->set_left($2);
                    assign->set_right($5);
                    assign->set_rdelay($4);
                    $$ = AST::to_node(assign);
                }

        |       TK_ASSIGN delays lvalue TK_EQUALS rvalue TK_SEMICOLON
                {
                    AST::Assign::Ptr assign = std::make_shared<AST::Assign>(scanner.get_filename(), @1.begin.line);
                    assign->set_left($3);
                    assign->set_right($5);
                    assign->set_ldelay($2);
                    $$ = AST::to_node(assign);
                }

        |       TK_ASSIGN delays lvalue TK_EQUALS delays rvalue TK_SEMICOLON
                {
                    AST::Assign::Ptr assign = std::make_shared<AST::Assign>(scanner.get_filename(), @1.begin.line);
                    assign->set_left($3);
                    assign->set_right($6);
                    assign->set_ldelay($2);
                    assign->set_rdelay($5);
                    $$ = AST::to_node(assign);
                }
        ;


lpartselect:    pointer TK_LBRACKET expression TK_COLON expression TK_RBRACKET
                {
	                 $$ = AST::to_node(std::make_shared<AST::Partselect>($3, $5, $1,
	                                                                     scanner.get_filename(), @1.begin.line));
                }

        |       pointer TK_LBRACKET expression TK_PLUSCOLON expression TK_RBRACKET
                {
	                 $$ = AST::to_node(std::make_shared<AST::PartselectPlusIndexed>($3, $5, $1,
                                                                                   scanner.get_filename(),
                                                                                   @1.begin.line));
                }

        |       pointer TK_LBRACKET expression TK_MINUSCOLON expression TK_RBRACKET
                {
                    $$ = AST::to_node(std::make_shared<AST::PartselectMinusIndexed>($3, $5, $1,
                                                                                    scanner.get_filename(),
                                                                                    @1.begin.line));
                }

        |       identifier TK_LBRACKET expression TK_COLON expression TK_RBRACKET
                {
                    AST::Node::Ptr id = AST::to_node($1);
                    $$ = AST::to_node(std::make_shared<AST::Partselect>($3, $5, id,
	                                                                     scanner.get_filename(),
                                                                        @1.begin.line));
                }

        |       identifier TK_LBRACKET expression TK_PLUSCOLON expression TK_RBRACKET
                {
                    AST::Node::Ptr id = AST::to_node($1);
                    $$ = AST::to_node(std::make_shared<AST::PartselectPlusIndexed>($3, $5, id,
                                                                                   scanner.get_filename(),
                                                                                   @1.begin.line));
                }

        |       identifier TK_LBRACKET expression TK_MINUSCOLON expression TK_RBRACKET
                {
                    AST::Node::Ptr id = AST::to_node($1);
                    $$ = AST::to_node(std::make_shared<AST::PartselectMinusIndexed>($3, $5, id,
                                                                                    scanner.get_filename(),
                                                                                    @1.begin.line));
                }
        ;


lpointer:       pointer {$$ = $1;}
        ;


lconcat:        TK_LBRACE lconcatlist TK_RBRACE
                {
                    $$ = std::make_shared<AST::Lconcat>($2, scanner.get_filename(), @1.begin.line);
                }
        ;


lconcatlist:    lconcatlist TK_COMMA lconcat_one
                {
                    $$ = $1;
                    $$->push_back($3);
                }

        |       lconcat_one
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back($1);
                }
        ;


lconcat_one:    identifier
                {
                    $$ = AST::to_node($1);
                }

        |       lpartselect
                {
                    $$ = $1;
                }

        |       lpointer
                {
                    $$ = $1;
                }

        |       lconcat
                {
                    $$ = AST::to_node($1);
                }
        ;


lvalue:         lpartselect
                {
                    $$ = std::make_shared<AST::Lvalue>($1, scanner.get_filename(), @1.begin.line);
                }

        |       lpointer
                {
                    $$ = std::make_shared<AST::Lvalue>($1, scanner.get_filename(), @1.begin.line);
                }

        |       lconcat
                {
                    $$ = std::make_shared<AST::Lvalue>(AST::to_node($1), scanner.get_filename(), @1.begin.line);
                }

        |       identifier
                {
                    $$ = std::make_shared<AST::Lvalue>(AST::to_node($1), scanner.get_filename(), @1.begin.line);
                }
        ;


rvalue:         expression
                {
                    $$ = std::make_shared<AST::Rvalue>($1, scanner.get_filename(), @1.begin.line);
                }
        ;


expression:     TK_MINUS expression %prec TK_UMINUS
                {
                    $$ = AST::to_node(std::make_shared<AST::Uminus>($2, scanner.get_filename(), @1.begin.line));
                }

        |       TK_PLUS expression %prec TK_UPLUS
                {
                    $$ = $2;
                }

        |       TK_LNOT expression %prec TK_ULNOT
                {
                    $$ = AST::to_node(std::make_shared<AST::Ulnot>($2, scanner.get_filename(), @1.begin.line));
                }

        |       TK_NOT expression %prec TK_UNOT
                {
                    $$ = AST::to_node(std::make_shared<AST::Unot>($2, scanner.get_filename(), @1.begin.line));
                }

        |       TK_AND expression %prec TK_UAND
                {
                    $$ = AST::to_node(std::make_shared<AST::Uand>($2, scanner.get_filename(), @1.begin.line));
                }

        |       TK_NAND expression %prec TK_UNAND
                {
                    $$ = AST::to_node(std::make_shared<AST::Unand>($2, scanner.get_filename(), @1.begin.line));
                }

        |       TK_NOR expression %prec TK_UNOR
                {
                    $$ = AST::to_node(std::make_shared<AST::Unor>($2, scanner.get_filename(), @1.begin.line));
                }

        |       TK_OR expression %prec TK_UOR
                {
                    $$ = AST::to_node(std::make_shared<AST::Uor>($2, scanner.get_filename(), @1.begin.line));
                }

        |       TK_XOR expression %prec TK_UXOR
                {
                    $$ = AST::to_node(std::make_shared<AST::Uxor>($2, scanner.get_filename(), @1.begin.line));
                }

        |       TK_XNOR expression %prec TK_UXNOR
                {
                    $$ = AST::to_node(std::make_shared<AST::Uxnor>($2, scanner.get_filename(), @1.begin.line));
                }

        |       expression TK_POWER expression
                {
                    $$ = AST::to_node(std::make_shared<AST::Power>($1, $3, scanner.get_filename(), @1.begin.line));
                }

        |       expression TK_TIMES expression
                {
                    $$ = AST::to_node(std::make_shared<AST::Times>($1, $3, scanner.get_filename(), @1.begin.line));
                }

        |       expression TK_DIVIDE expression
                {
                    $$ = AST::to_node(std::make_shared<AST::Divide>($1, $3, scanner.get_filename(), @1.begin.line));
                }

        |       expression TK_MOD expression
                {
                    $$ = AST::to_node(std::make_shared<AST::Mod>($1, $3, scanner.get_filename(), @1.begin.line));
                }

        |       expression TK_PLUS expression
                {
                    $$ = AST::to_node(std::make_shared<AST::Plus>($1, $3, scanner.get_filename(), @1.begin.line));
                }

        |       expression TK_MINUS expression
                {
                    $$ = AST::to_node(std::make_shared<AST::Minus>($1, $3, scanner.get_filename(), @1.begin.line));
                }

        |       expression TK_LSHIFT expression
                {
                    $$ = AST::to_node(std::make_shared<AST::Sll>($1, $3, scanner.get_filename(), @1.begin.line));
                }

        |       expression TK_RSHIFT expression
                {
                    $$ = AST::to_node(std::make_shared<AST::Srl>($1, $3, scanner.get_filename(), @1.begin.line));
                }

        |       expression TK_LSHIFTA expression
                {
                    $$ = AST::to_node(std::make_shared<AST::Sll>($1, $3, scanner.get_filename(), @1.begin.line));
                }

        |       expression TK_RSHIFTA expression
                {
                    $$ = AST::to_node(std::make_shared<AST::Sra>($1, $3, scanner.get_filename(), @1.begin.line));
                }

        |       expression TK_LT expression
                {
                    $$ = AST::to_node(std::make_shared<AST::LessThan>($1, $3, scanner.get_filename(), @1.begin.line));
                }

        |       expression TK_GT expression
                {
                    $$ = AST::to_node(std::make_shared<AST::GreaterThan>($1, $3, scanner.get_filename(), @1.begin.line));
                }

        |       expression TK_LE expression
                {
                    $$ = AST::to_node(std::make_shared<AST::LessEq>($1, $3, scanner.get_filename(), @1.begin.line));
                }

        |       expression TK_GE expression
                {
                    $$ = AST::to_node(std::make_shared<AST::GreaterEq>($1, $3, scanner.get_filename(), @1.begin.line));
                }

        |       expression TK_EQ expression
                {
                    $$ = AST::to_node(std::make_shared<AST::Eq>($1, $3, scanner.get_filename(), @1.begin.line));
                }

        |       expression TK_NE expression
                {
                    $$ = AST::to_node(std::make_shared<AST::NotEq>($1, $3, scanner.get_filename(), @1.begin.line));
                }

        |       expression TK_EQL expression
                {
                    $$ = AST::to_node(std::make_shared<AST::Eql>($1, $3, scanner.get_filename(), @1.begin.line));
                }

        |       expression TK_NEL expression
                {
                    $$ = AST::to_node(std::make_shared<AST::NotEql>($1, $3, scanner.get_filename(), @1.begin.line));
                }

        |       expression TK_AND expression
                {
                    $$ = AST::to_node(std::make_shared<AST::And>($1, $3, scanner.get_filename(), @1.begin.line));
                }

        |       expression TK_XOR expression
                {
                    $$ = AST::to_node(std::make_shared<AST::Xor>($1, $3, scanner.get_filename(), @1.begin.line));
                }

        |       expression TK_XNOR expression
                {
                    $$ = AST::to_node(std::make_shared<AST::Xnor>($1, $3, scanner.get_filename(), @1.begin.line));
                }

        |       expression TK_OR expression
                {
                    $$ = AST::to_node(std::make_shared<AST::Or>($1, $3, scanner.get_filename(), @1.begin.line));
                }

        |       expression TK_LAND expression
                {
                    $$ = AST::to_node(std::make_shared<AST::Land>($1, $3, scanner.get_filename(), @1.begin.line));
                }

        |       expression TK_LOR expression
                {
                    $$ = AST::to_node(std::make_shared<AST::Lor>($1, $3, scanner.get_filename(), @1.begin.line));
                }

        |       paren_expression
                {
                    $$ = $1;
                }

        |       ternary
                {
                    $$ = $1;
                }

        |       concat
                {
                    $$ = AST::to_node($1);
                }

        |       repeat
                {
                    $$ = AST::to_node($1);
                }

        |       partselect
                {
                    $$ = $1;
                }

        |       pointer
                {
                    $$ = $1;
                }

        |       function_call
                {
                    $$ = AST::to_node($1);
                }

        |       systemcall
                {
                    $$ = AST::to_node($1);
                }

        |       identifier
                {
                    $$ = AST::to_node($1);
                }

        |       const_expression
                {
                    $$ = $1;
                }
        ;


paren_expression:
                TK_LPARENTHESIS expression TK_RPARENTHESIS
                {
                    $$ = $2;
                }
        ;


ternary:        expression TK_COND expression TK_COLON expression
                {
                    $$ = std::make_shared<AST::Cond>($1, $3, $5, scanner.get_filename(), @2.begin.line);
                }
        ;


concat:         TK_LBRACE concatlist TK_RBRACE
                {
                    $$ = std::make_shared<AST::Concat>($2, scanner.get_filename(), @1.begin.line);
                }
        ;


concatlist:     concatlist TK_COMMA expression
                {
                    $$ = $1;
                    $$->push_back($3);
                }

        |       expression
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back($1);
                }
        ;


repeat:         TK_LBRACE expression concat TK_RBRACE
                {
                    $$ = std::make_shared<AST::Repeat>($3, $2, scanner.get_filename(), @1.begin.line);
                }
        ;


partselect:     pointer TK_LBRACKET expression TK_COLON expression TK_RBRACKET
                {
                    $$ = AST::to_node(std::make_shared<AST::Partselect>($3, $5, $1,
                                                                        scanner.get_filename(), @1.begin.line));
                }

        |       pointer TK_LBRACKET expression TK_PLUSCOLON expression TK_RBRACKET
                {
                    $$ = AST::to_node(std::make_shared<AST::PartselectPlusIndexed>($3, $5, $1,
                                                                                   scanner.get_filename(),
                                                                                   @1.begin.line));
                }

        |       pointer TK_LBRACKET expression TK_MINUSCOLON expression TK_RBRACKET
                {
                    $$ = AST::to_node(std::make_shared<AST::PartselectMinusIndexed>($3, $5, $1,
                                                                                    scanner.get_filename(),
                                                                                    @1.begin.line));
                }

        |       identifier TK_LBRACKET expression TK_COLON expression TK_RBRACKET
                {
                    AST::Node::Ptr id = AST::to_node($1);
                    $$ = AST::to_node(std::make_shared<AST::Partselect>($3, $5, id,
                                                                        scanner.get_filename(), @1.begin.line));
                }

        |       identifier TK_LBRACKET expression TK_PLUSCOLON expression TK_RBRACKET
                {
                    AST::Node::Ptr id = AST::to_node($1);
                    $$ = AST::to_node(std::make_shared<AST::PartselectPlusIndexed>($3, $5, id,
                                                                                   scanner.get_filename(),
                                                                                   @1.begin.line));
                }

        |       identifier TK_LBRACKET expression TK_MINUSCOLON expression TK_RBRACKET
                {
                    AST::Node::Ptr id = AST::to_node($1);
                    $$ = AST::to_node(std::make_shared<AST::PartselectMinusIndexed>($3, $5, id,
                                                                                    scanner.get_filename(),
                                                                                    @1.begin.line));
                }
        ;


pointer:        pointer TK_LBRACKET expression TK_RBRACKET
                {
                    $$ = AST::to_node(std::make_shared<AST::Pointer>($3, $1, scanner.get_filename(), @1.begin.line));
                }

        |       identifier TK_LBRACKET expression TK_RBRACKET
                {
                    AST::Node::Ptr id = AST::to_node($1);
                    $$ = AST::to_node(std::make_shared<AST::Pointer>($3, id, scanner.get_filename(), @1.begin.line));
                }
        ;


const_expression:
                intnumber
                {
                    $$ = $1;
                }

        |       floatnumber
                {
                    $$ = AST::to_node($1);
                }

        |       stringliteral
                {
                    $$ = AST::to_node($1);
                }
        ;


floatnumber:    TK_FLOATNUMBER
                {
	                double d;
	                char *end;
	                std::string dblstr = Misc::StringUtils::remove_underscore($1);
	                d = strtod(dblstr.c_str(), &end);
	                if (*end != '\0') error(@1, "malformed real constant");
	                $$ = std::make_shared<AST::FloatConst>(d, scanner.get_filename(), @1.begin.line);
                }
        ;


intnumber:      TK_INTNUMBER
                {
                    mpz_class mpz;
                    int size;
                    int base;
                    bool is_signed;
                    int rc;

                    if(VerilogNumber().parse($1, mpz, size, base, is_signed, rc)) {
                       error(@1, "malformed integer constant");
                    }

                    if (rc == 0) {
                        $$ = std::make_shared<AST::IntConstN>(base, size, is_signed, mpz,
                                                              scanner.get_filename(), @1.begin.line);
                    }
                    else {
                        $$ = std::make_shared<AST::IntConst>($1, scanner.get_filename(), @1.begin.line);
                    }
                }
        ;


stringliteral:  TK_STRING
                {
                    $$ = std::make_shared<AST::StringConst>($1, scanner.get_filename(), @1.begin.line);
                }
        ;


always:         TK_ALWAYS basic_statement
                {
                    $$ = std::make_shared<AST::Always>(scanner.get_filename(), @1.begin.line);

                    /**
                     * To avoid reduce/reduce conflict with event_statement, we do not add a rule:
                     *
                     *    always: TK_ALWAYS senslist basic_statement
                     *
                     * So we need to check if the basic_statement is a just a block or an event
                     */
                    if ($2->get_node_type() == AST::NodeType::EventStatement) {
                        $$->set_senslist(AST::cast_to<AST::EventStatement>($2)->get_senslist());
                        $$->set_statement(AST::cast_to<AST::EventStatement>($2)->get_statement());
                    }
                    else {
                        $$->set_statement($2);
                    }
                }
        ;


senslist:       TK_AT TK_LPARENTHESIS edgesigs TK_RPARENTHESIS
                {
                    $$ = std::make_shared<AST::Senslist>($3, scanner.get_filename(), @1.begin.line);
                }

        |       TK_AT levelsig
                {
                    $$ = std::make_shared<AST::Senslist>(scanner.get_filename(), @1.begin.line);
                    AST::Sens::ListPtr l = std::make_shared<AST::Sens::List>();
                    l->push_back($2);
                    $$->set_list(l);
                }

        |       TK_AT TK_LPARENTHESIS levelsigs TK_RPARENTHESIS
                {
                    $$ = std::make_shared<AST::Senslist>($3, scanner.get_filename(), @1.begin.line);
                }

        |       TK_AT TK_TIMES
                {
                    $$ = std::make_shared<AST::Senslist>(scanner.get_filename(), @1.begin.line);
                    AST::Sens::Ptr s = std::make_shared<AST::Sens>(scanner.get_filename(), @1.begin.line);
                    AST::Sens::ListPtr l = std::make_shared<AST::Sens::List>();
                    s->set_type(AST::Sens::TypeEnum::ALL);
                    l->push_back(s);
                    $$->set_list(l);
                }

        |       TK_AT TK_LPARENTHESIS TK_TIMES TK_RPARENTHESIS
                {
                    $$ = std::make_shared<AST::Senslist>(scanner.get_filename(), @1.begin.line);
                    AST::Sens::Ptr s = std::make_shared<AST::Sens>(scanner.get_filename(), @1.begin.line);
                    AST::Sens::ListPtr l = std::make_shared<AST::Sens::List>();
                    s->set_type(AST::Sens::TypeEnum::ALL);
                    l->push_back(s);
                    $$->set_list(l);
                }
        ;


edgesigs:       edgesigs TK_SENS_OR edgesig
                {
                    $$ = $1;
                    $$->push_back($3);
                }

        |       edgesigs TK_COMMA edgesig
                {
                    $$ = $1;
                    $$->push_back($3);
                }

        |       edgesig
                {
                    $$ = std::make_shared<AST::Sens::List>();
                    $$->push_back($1);
                }
        ;


edgesig:        TK_POSEDGE edgesig_base
                {
                    $$ = $2;
                    $$->set_type(AST::Sens::TypeEnum::POSEDGE);
                }

        |       TK_NEGEDGE edgesig_base
                {
                    $$ = $2;
                    $$->set_type(AST::Sens::TypeEnum::NEGEDGE);
                }
        ;


edgesig_base:   identifier
                {
                    $$ = std::make_shared<AST::Sens>(scanner.get_filename(), @1.begin.line);
                    $$->set_sig(AST::to_node($1));
                    $$->set_type(AST::Sens::TypeEnum::NONE);
                }

        |       pointer
                {
                    $$ = std::make_shared<AST::Sens>(scanner.get_filename(), @1.begin.line);
                    $$->set_sig($1);
                    $$->set_type(AST::Sens::TypeEnum::NONE);
                }
        ;


levelsigs:      levelsigs TK_SENS_OR levelsig
                {
                    $$ = $1;
                    $$->push_back($3);
                }

        |       levelsigs TK_COMMA levelsig
                {
                    $$ = $1;
                    $$->push_back($3);
                }

        |       levelsig
                {
                    $$ = std::make_shared<AST::Sens::List>();
                    $$->push_back($1);
                }
        ;


levelsig:       levelsig_base {$$ = $1;}
        ;


levelsig_base:  identifier
                {
                    $$ = std::make_shared<AST::Sens>(scanner.get_filename(), @1.begin.line);
                    $$->set_sig(AST::to_node($1));
                    $$->set_type(AST::Sens::TypeEnum::NONE);
                }

        |       pointer
                {
                    $$ = std::make_shared<AST::Sens>(scanner.get_filename(), @1.begin.line);
                    $$->set_sig($1);
                    $$->set_type(AST::Sens::TypeEnum::NONE);
                }

        |       partselect
                {
                    $$ = std::make_shared<AST::Sens>(scanner.get_filename(), @1.begin.line);
                    $$->set_sig($1);
                    $$->set_type(AST::Sens::TypeEnum::NONE);
                }
        ;


basic_statement:if_statement
                {
                    $$ = AST::to_node($1);
                }

        |       case_statement
                {
                    $$ = AST::to_node($1);
                }

        |       casex_statement
                {
                    $$ = AST::to_node($1);
                }

        |       casez_statement
                {
                    $$ = AST::to_node($1);
                }

        |       for_statement
                {
                    $$ = AST::to_node($1);
                }

        |       while_statement
                {
                    $$ = AST::to_node($1);
                }

        |       event_statement
                {
                    $$ = AST::to_node($1);
                }

        |       wait_statement
                {
                    $$ = AST::to_node($1);
                }

        |       forever_statement
                {
                    $$ = AST::to_node($1);
                }

        |       block
                {
                    $$ = AST::to_node($1);
                }

        |       named_block
                {
                    $$ = AST::to_node($1);
                }

        |       parallelblock
                {
                    $$ = AST::to_node($1);
                }

        |       named_parallelblock
                {
                    $$ = AST::to_node($1);
                }

        |       blocking_assignment
                {
                    $$ = AST::to_node($1);
                }

        |       nonblocking_assignment
                {
                    $$ = AST::to_node($1);
                }

        |       single_statement
                {
                    $$ = AST::to_node($1);
                }
        ;


blocking_assignment:
                blocking_assignment_base TK_SEMICOLON {$$ = $1;}
        ;


blocking_assignment_base:
                delays lvalue TK_EQUALS delays rvalue
                {
                    $$ = std::make_shared<AST::BlockingSubstitution>(scanner.get_filename(), @1.begin.line);
                    $$->set_left($2);
                    $$->set_right($5);
                    $$->set_ldelay($1);
                    $$->set_rdelay($4);
                }

        |       delays lvalue TK_EQUALS rvalue
                {
                    $$ = std::make_shared<AST::BlockingSubstitution>(scanner.get_filename(), @1.begin.line);
                    $$->set_left($2);
                    $$->set_right($4);
                    $$->set_ldelay($1);
                }

        |       lvalue TK_EQUALS delays rvalue
                {
                    $$ = std::make_shared<AST::BlockingSubstitution>(scanner.get_filename(), @1.begin.line);
                    $$->set_left($1);
                    $$->set_right($4);
                    $$->set_rdelay($3);
                }

        |       lvalue TK_EQUALS rvalue
                {
                    $$ = std::make_shared<AST::BlockingSubstitution>(scanner.get_filename(), @1.begin.line);
                    $$->set_left($1);
                    $$->set_right($3);
                }
        ;

nonblocking_assignment:
                delays lvalue TK_LE delays rvalue TK_SEMICOLON
                {
                    $$ = std::make_shared<AST::NonblockingSubstitution>(scanner.get_filename(), @1.begin.line);
                    $$->set_left($2);
                    $$->set_right($5);
                    $$->set_ldelay($1);
                    $$->set_rdelay($4);
                }

        |       delays lvalue TK_LE rvalue TK_SEMICOLON
                {
                    $$ = std::make_shared<AST::NonblockingSubstitution>(scanner.get_filename(), @1.begin.line);
                    $$->set_left($2);
                    $$->set_right($4);
                    $$->set_ldelay($1);
                }

        |       lvalue TK_LE delays rvalue TK_SEMICOLON
                {
                    $$ = std::make_shared<AST::NonblockingSubstitution>(scanner.get_filename(), @1.begin.line);
                    $$->set_left($1);
                    $$->set_right($4);
                    $$->set_rdelay($3);
                }

        |       lvalue TK_LE rvalue TK_SEMICOLON
                {
                    $$ = std::make_shared<AST::NonblockingSubstitution>(scanner.get_filename(), @1.begin.line);
                    $$->set_left($1);
                    $$->set_right($3);
                }
        ;


delays:         TK_DELAY paren_expression
                {
                    $$ = std::make_shared<AST::DelayStatement>($2, scanner.get_filename(), @1.begin.line);
                }

        |       TK_DELAY identifier
                {
                    $$ = std::make_shared<AST::DelayStatement>(AST::to_node($2), scanner.get_filename(), @1.begin.line);
                }

        |       TK_DELAY intnumber
                {
                    $$ = std::make_shared<AST::DelayStatement>($2, scanner.get_filename(), @1.begin.line);
                }

        |       TK_DELAY floatnumber
                {
                    $$ = std::make_shared<AST::DelayStatement>(AST::to_node($2), scanner.get_filename(), @1.begin.line);
                }
        ;


block:          TK_BEGIN block_statements TK_END
                {
                    $$ = std::make_shared<AST::Block>(scanner.get_filename(), @1.begin.line);
                    $$->set_statements($2);
                }

        |       TK_BEGIN TK_END
                {
                    $$ = std::make_shared<AST::Block>(scanner.get_filename(), @1.begin.line);
                }
        ;


named_block:    TK_BEGIN TK_COLON TK_IDENTIFIER block_statements TK_END
                {
                    $$ = std::make_shared<AST::Block>(scanner.get_filename(), @1.begin.line);
                    $$->set_scope($3);
                    $$->set_statements($4);
                }

        |       TK_BEGIN TK_COLON TK_IDENTIFIER TK_END
                {
                    $$ = std::make_shared<AST::Block>(scanner.get_filename(), @1.begin.line);
                    $$->set_scope($3);
                }
        ;


block_statements:
                block_statements block_statement
                {
                    $$ = $1;
                    $$->splice($$->end(), *$2);
                }

        |       block_statement
                {
                    $$ = $1;
                }
        ;

block_statement:
                basic_statement
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back($1);
                }

        |       net_decl
                {
                    $$ = $1;
                }

        |       reg_decl
                {
                    $$ = $1;
                }

        |       integer_decl
                {
                    $$ = $1;
                }

        |       real_decl
                {
                    $$ = $1;
                }

        |       parameter_decl
                {
                    $$ = AST::cast_list_to<AST::Node>($1);
                }

        |       localparam_decl
                {
                    $$ = AST::cast_list_to<AST::Node>($1);
                }
        ;


parallelblock:
                TK_FORK block_statements TK_JOIN
                {
                    $$ = std::make_shared<AST::ParallelBlock>(scanner.get_filename(), @1.begin.line);
                    $$->set_statements($2);
                }

        |       TK_FORK TK_JOIN
                {
                    $$ = std::make_shared<AST::ParallelBlock>(scanner.get_filename(), @1.begin.line);
                }
        ;

named_parallelblock:
                TK_FORK TK_COLON TK_IDENTIFIER block_statements TK_JOIN
                {
                    $$ = std::make_shared<AST::ParallelBlock>(scanner.get_filename(), @1.begin.line);
                    $$->set_scope($3);
                    $$->set_statements($4);
                }

        |       TK_FORK TK_COLON TK_IDENTIFIER TK_JOIN
                {
                    $$ = std::make_shared<AST::ParallelBlock>(scanner.get_filename(), @1.begin.line);
                    $$->set_scope($3);
                }
        ;

if_statement:   TK_IF TK_LPARENTHESIS cond TK_RPARENTHESIS true_statement TK_ELSE false_statement
                {
                    $$ = std::make_shared<AST::IfStatement>(scanner.get_filename(), @1.begin.line);
                    $$->set_cond($3);
                    $$->set_true_statement($5);
                    $$->set_false_statement($7);
                }

        |       TK_IF TK_LPARENTHESIS cond TK_RPARENTHESIS true_statement
                {
                    $$ = std::make_shared<AST::IfStatement>(scanner.get_filename(), @1.begin.line);
                    $$->set_cond($3);
                    $$->set_true_statement($5);
                }

        |       delays TK_IF TK_LPARENTHESIS cond TK_RPARENTHESIS true_statement TK_ELSE false_statement
                {
                    $$ = std::make_shared<AST::IfStatement>(scanner.get_filename(), @1.begin.line);
                    $$->set_cond($4);
                    $$->set_true_statement($6);
                    $$->set_false_statement($8);
                }

        |       delays TK_IF TK_LPARENTHESIS cond TK_RPARENTHESIS true_statement
                {
                    $$ = std::make_shared<AST::IfStatement>(scanner.get_filename(), @1.begin.line);
                    $$->set_cond($4);
                    $$->set_true_statement($6);
                }
        ;


cond:           expression
                {
                    $$ = $1;
                }
        ;


true_statement: ifcontent_statement
                {
                    $$ = $1;
                }
        ;


false_statement:
                ifcontent_statement
                {
                    $$ = $1;
                }
        ;


ifcontent_statement:
                basic_statement
                {
                    $$ = $1;
                }
        ;


for_statement:  TK_FOR TK_LPARENTHESIS forpre forcond forpost TK_RPARENTHESIS forcontent_statement
                {
                    $$ = std::make_shared<AST::ForStatement>(scanner.get_filename(), @1.begin.line);
                    $$->set_pre($3);
                    $$->set_cond($4);
                    $$->set_post($5);
                    $$->set_statement($7);
                }

        |       TK_FOR TK_LPARENTHESIS forpre forcond TK_RPARENTHESIS forcontent_statement
                {
                    $$ = std::make_shared<AST::ForStatement>(scanner.get_filename(), @1.begin.line);
                    $$->set_pre($3);
                    $$->set_cond($4);
                    $$->set_statement($6);
                }
        ;

forpre:         blocking_assignment
                {
                    $$ = $1;
                }

        |       TK_SEMICOLON
                {
                    $$ = std::make_shared<AST::BlockingSubstitution>(scanner.get_filename(), @1.begin.line);
                }
        ;


forcond:        cond TK_SEMICOLON
                {
                    $$ = $1;
                }

        |       TK_SEMICOLON
                {
                    $$ = std::make_shared<AST::Node>(scanner.get_filename(), @1.begin.line);
                }
        ;


forpost:        blocking_assignment_base
                {
                    $$ = $1;
                }
        ;


forcontent_statement:
                basic_statement
                {
                    $$ = $1;
                }
        ;


while_statement:TK_WHILE TK_LPARENTHESIS cond TK_RPARENTHESIS whilecontent_statement
                {
                    $$ = std::make_shared<AST::WhileStatement>($3, $5, scanner.get_filename(), @1.begin.line);
                }
        ;


whilecontent_statement:
                basic_statement
                {
                    $$ = $1;
                }
        ;


case_statement: TK_CASE TK_LPARENTHESIS case_comp TK_RPARENTHESIS casecontent_statements TK_ENDCASE
                {
                    $$ = std::make_shared<AST::CaseStatement>($3, $5, scanner.get_filename(), @1.begin.line);
                }
        ;


casex_statement:
                TK_CASEX TK_LPARENTHESIS case_comp TK_RPARENTHESIS casecontent_statements TK_ENDCASE
                {
                    $$ = std::make_shared<AST::CasexStatement>($3, $5, scanner.get_filename(), @1.begin.line);
                }
        ;


casez_statement:
                TK_CASEZ TK_LPARENTHESIS case_comp TK_RPARENTHESIS casecontent_statements TK_ENDCASE
                {
                    $$ = std::make_shared<AST::CasezStatement>($3, $5, scanner.get_filename(), @1.begin.line);
                }
        ;


case_comp:      expression
                {
                    $$ = $1;
                }
        ;


casecontent_statements:
                casecontent_statements casecontent_statement
                {
                    $$ = $1;
                    $$->push_back($2);
                }

        |       casecontent_statement
                {
                    $$ = std::make_shared<AST::Case::List>();
                    $$->push_back($1);
                }
        ;


casecontent_statement:
                casecontent_conditions TK_COLON basic_statement
                {
                    $$ = std::make_shared<AST::Case>(scanner.get_filename(), @1.begin.line);
                    $$->set_cond($1);
                    $$->set_statement($3);
                }

        |       TK_DEFAULT TK_COLON basic_statement
                {
                    $$ = std::make_shared<AST::Case>(scanner.get_filename(), @1.begin.line);
                    $$->set_statement($3);
                }
        ;


casecontent_conditions:
                casecontent_conditions TK_COMMA expression
                {
                    $$ = $1;
                    $$->push_back($3);
                }

        |       expression
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back($1);
                }
        ;


initial:        TK_INITIAL initial_statement
                {
                    $$ = std::make_shared<AST::Initial>($2, scanner.get_filename(), @1.begin.line);
                }
        ;


initial_statement:
                basic_statement
                {
                    $$ = $1;
                }
        ;


event_statement:
                senslist basic_statement
                {
                    $$ = std::make_shared<AST::EventStatement>(scanner.get_filename(), @1.begin.line);
                    $$->set_senslist($1);
                    $$->set_statement($2);
                }

        |       senslist TK_SEMICOLON
                {
                    $$ = std::make_shared<AST::EventStatement>(scanner.get_filename(), @1.begin.line);
                    $$->set_senslist($1);
                }
        ;


wait_statement: TK_WAIT TK_LPARENTHESIS cond TK_RPARENTHESIS waitcontent_statement
                {
                    $$ = std::make_shared<AST::WaitStatement>($3, $5, scanner.get_filename(), @1.begin.line);
                }

        |       TK_WAIT TK_LPARENTHESIS cond TK_RPARENTHESIS TK_SEMICOLON
                {
                    $$ = std::make_shared<AST::WaitStatement>(scanner.get_filename(), @1.begin.line);
                    $$->set_cond($3);
                }
        ;


waitcontent_statement:
                basic_statement
                {
                    $$ = $1;
                }
        ;


forever_statement:
                TK_FOREVER basic_statement
                {
                    $$ = std::make_shared<AST::ForeverStatement>($2);
                }
        ;


instance:       TK_IDENTIFIER parameterlist instance_bodylist TK_SEMICOLON
                {
                    $$ = std::make_shared<AST::Instancelist>(scanner.get_filename(), @1.begin.line);
                    $$->set_parameterlist($2);
                    $$->set_instances($3);
                    $$->set_module($1);
                    for(AST::Instance::Ptr &i : *$3) {
                        i->set_module($1);
                        i->set_parameterlist(AST::ParamArg::clone_list($2));
                    }
                }

        |       TK_IDENTIFIER instance_bodylist TK_SEMICOLON
                {
                    $$ = std::make_shared<AST::Instancelist>(scanner.get_filename(), @1.begin.line);
                    $$->set_instances($2);
                    $$->set_module($1);
                    for(AST::Instance::Ptr &i : *$2) {
                        i->set_module($1);
                    }
                }

        |       TK_IDENTIFIER instance_bodylist_noname TK_SEMICOLON
                {
                    $$ = std::make_shared<AST::Instancelist>(scanner.get_filename(), @1.begin.line);
                    $$->set_instances($2);
                    $$->set_module($1);
                    for(AST::Instance::Ptr &i : *$2) {
                        i->set_module($1);
                    }
                }

        |       TK_SENS_OR parameterlist instance_bodylist TK_SEMICOLON
                {
                    $$ = std::make_shared<AST::Instancelist>(scanner.get_filename(), @1.begin.line);
                    $$->set_parameterlist($2);
                    $$->set_instances($3);
                    $$->set_module("or");
                    for(AST::Instance::Ptr &i : *$3) {
                        i->set_module("or");
                        i->set_parameterlist(AST::ParamArg::clone_list($2));
                    }
                }

        |       TK_SENS_OR instance_bodylist TK_SEMICOLON
                {
                    $$ = std::make_shared<AST::Instancelist>(scanner.get_filename(), @1.begin.line);
                    $$->set_instances($2);
                    $$->set_module("or");
                    for(AST::Instance::Ptr &i : *$2) {
                        i->set_module("or");
                    }
                }

        |       TK_SENS_OR instance_bodylist_noname TK_SEMICOLON
                {
                    $$ = std::make_shared<AST::Instancelist>(scanner.get_filename(), @1.begin.line);
                    $$->set_instances($2);
                    $$->set_module("or");
                    for(AST::Instance::Ptr &i : *$2) {
                        i->set_module("or");
                    }
                }
        ;


instance_bodylist:
                instance_bodylist TK_COMMA instance_body
                {
                    $$ = $1;
                    $$->push_back($3);
                }

        |       instance_body
                {
                    $$ = std::make_shared<AST::Instance::List>();
                    $$->push_back($1);
                }
        ;


instance_body:  TK_IDENTIFIER TK_LPARENTHESIS instance_ports TK_RPARENTHESIS
                {
                    $$ = std::make_shared<AST::Instance>(scanner.get_filename(), @1.begin.line);
                    $$->set_portlist($3);
                    $$->set_name($1);
                }

        |       TK_IDENTIFIER length TK_LPARENTHESIS instance_ports TK_RPARENTHESIS
                {
                    $$ = std::make_shared<AST::Instance>(scanner.get_filename(), @1.begin.line);
                    $$->set_array($2);
                    $$->set_portlist($4);
                    $$->set_name($1);
                }
        ;


instance_bodylist_noname:
                instance_bodylist_noname TK_COMMA instance_body_noname
                {
                    $$ = $1;
                    $$->push_back($3);
                }

        |       instance_body_noname
                {
                    $$ = std::make_shared<AST::Instance::List>();
                    $$->push_back($1);
                }
        ;

instance_body_noname:
                TK_LPARENTHESIS instance_ports TK_RPARENTHESIS
                {
                    $$ = std::make_shared<AST::Instance>(scanner.get_filename(), @1.begin.line);
                    $$->set_portlist($2);
                }
        ;


parameterlist:  TK_DELAY TK_LPARENTHESIS param_args TK_RPARENTHESIS
                {
                    $$ = $3;
                }

        |       TK_DELAY TK_LPARENTHESIS param_args_noname TK_RPARENTHESIS
                {
                    $$ = $3;
                }
        ;


param_args_noname:
                param_args_noname TK_COMMA param_arg_noname
                {
                    $$ = $1;
                    $$->push_back($3);
                }

        |       param_arg_noname
                {
                    $$ = std::make_shared<AST::ParamArg::List>();
                    $$->push_back($1);
                }
        ;


param_args:     param_args TK_COMMA param_arg
                {
                    $$ = $1;
                    $$->push_back($3);
                }

        |       param_arg
                {
                    $$ = std::make_shared<AST::ParamArg::List>();
                    $$->push_back($1);
                }
        ;


param_arg_noname:
                expression
                {
                    $$ = std::make_shared<AST::ParamArg>(scanner.get_filename(), @1.begin.line);
                    $$->set_value($1);
                }


param_arg:      TK_DOT TK_IDENTIFIER paren_expression
                {
                    $$ = std::make_shared<AST::ParamArg>(scanner.get_filename(), @1.begin.line);
                    $$->set_value($3);
                    $$->set_name($2);
                }
        ;


instance_ports: instance_ports_list
                {
                    $$ = $1;
                }

        |       instance_ports_arg
                {
                    $$ = $1;
                }
        ;


instance_ports_list:
                instance_ports_list TK_COMMA instance_port_list
                {
                    $$ = $1;
                    $$->push_back($3);
                }

        |       instance_port_list
                {
                    $$ = std::make_shared<AST::PortArg::List>();
                    $$->push_back($1);
                }
        ;


instance_port_list:
                expression
                {
                    $$ = std::make_shared<AST::PortArg>();
                    $$->set_value($1);
                }
        ;


instance_ports_arg:
                instance_ports_arg TK_COMMA instance_port_arg
                {
                    $$ = $1;
                    $$->push_back($3);
                }

        |       instance_port_arg
                {
                    $$ = std::make_shared<AST::PortArg::List>();
                    $$->push_back($1);
                }
        ;


instance_port_arg:
                TK_DOT TK_IDENTIFIER paren_expression
                {
                    $$ = std::make_shared<AST::PortArg>(scanner.get_filename(), @1.begin.line);
                    $$->set_value($3);
                    $$->set_name($2);
                }

        |       TK_DOT TK_IDENTIFIER TK_LPARENTHESIS TK_RPARENTHESIS
                {
                    $$ = std::make_shared<AST::PortArg>(scanner.get_filename(), @1.begin.line);
                    $$->set_name($2);
                }
        ;


genvar_decl:    TK_GENVAR genvarlist TK_SEMICOLON
                {
                    $$ = $2;
                }
        ;


genvarlist:     genvarlist TK_COMMA genvar
                {
                    $$ = $1;
                    $$->push_back($3);
                }

        |       genvar
                {
                    $$ = std::make_shared<AST::Genvar::List>();
                    $$->push_back($1);
                }
        ;


genvar:         TK_IDENTIFIER
                {
                    $$ = std::make_shared<AST::Genvar>($1, scanner.get_filename(), @1.begin.line);
                }
        ;


generate:       TK_GENERATE generate_items TK_ENDGENERATE
                {
                    $$ = std::make_shared<AST::GenerateStatement>(scanner.get_filename(), @1.begin.line);
                    $$->set_items($2);
                }

        |       TK_GENERATE TK_ENDGENERATE
                {
                    $$ = std::make_shared<AST::GenerateStatement>(scanner.get_filename(), @1.begin.line);
                }
        ;


generate_items: generate_items generate_item
                {
                    $$ = $1;
                    $$->splice($$->end(), *$2);
                }

        |       generate_item
                {
                    $$ = $1;
                }


generate_item:  standard_item
                {
                    $$ = $1;
                }

        |       generate_if
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back(AST::to_node($1));
                }

        |       generate_case
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back(AST::to_node($1));
                }

        |       generate_for
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back(AST::to_node($1));
                }
        ;


generate_block: TK_BEGIN generate_items TK_END
                {
                    $$ = std::make_shared<AST::Block>(scanner.get_filename(), @1.begin.line);
                    $$->set_statements($2);
                }

        |       TK_BEGIN TK_COLON TK_IDENTIFIER generate_items TK_END
                {
                    $$ = std::make_shared<AST::Block>(scanner.get_filename(), @1.begin.line);
                    $$->set_statements($4);
                    $$->set_scope($3);
                }
        ;


generate_case:  TK_CASE TK_LPARENTHESIS case_comp TK_RPARENTHESIS generate_case_statements TK_ENDCASE
                {
                    $$ = std::make_shared<AST::CaseStatement>($3, $5, scanner.get_filename(), @1.begin.line);
                }
        ;


generate_case_statements:
                generate_case_statements generate_case_statement
                {
                    $$ = $1;
                    $$->push_back($2);
                }

        |       generate_case_statement
                {
                    $$ = std::make_shared<AST::Case::List>();
                    $$->push_back($1);
                }
        ;


generate_case_statement:
                casecontent_conditions TK_COLON generate_content
                {
                    $$ = std::make_shared<AST::Case>(scanner.get_filename(), @1.begin.line);
                    $$->set_cond($1);
                    $$->set_statement($3);
                }

        |       TK_DEFAULT TK_COLON generate_content
                {
                    $$ = std::make_shared<AST::Case>(scanner.get_filename(), @1.begin.line);
                    $$->set_statement($3);
                }
        ;


generate_if:    TK_IF TK_LPARENTHESIS cond TK_RPARENTHESIS generate_content TK_ELSE generate_content
                {
                    $$ = std::make_shared<AST::IfStatement>(scanner.get_filename(), @1.begin.line);
                    $$->set_cond($3);
                    $$->set_true_statement($5);
                    $$->set_false_statement($7);
                }

        |       TK_IF TK_LPARENTHESIS cond TK_RPARENTHESIS generate_content
                {
                    $$ = std::make_shared<AST::IfStatement>(scanner.get_filename(), @1.begin.line);
                    $$->set_cond($3);
                    $$->set_true_statement($5);
                }
        ;


generate_for:   TK_FOR TK_LPARENTHESIS forpre forcond forpost TK_RPARENTHESIS generate_content
                {
                     $$ = std::make_shared<AST::ForStatement>(scanner.get_filename(), @1.begin.line);
                     $$->set_pre($3);
                     $$->set_cond($4);
                     $$->set_post($5);
                     $$->set_statement($7);
                }
        ;


generate_content:
                generate_item
                {
                    $$ = std::make_shared<AST::Block>(scanner.get_filename(), @1.begin.line);
                    $$->set_statements($1);
                }

        |       generate_block
                {
                    $$ = $1;
                }
        ;


systemcall:     TK_DOLLAR TK_IDENTIFIER
                {
                    $$ = std::make_shared<AST::SystemCall>(scanner.get_filename(), @1.begin.line);
                    $$->set_syscall($2);
                }

        |       TK_DOLLAR TK_IDENTIFIER TK_LPARENTHESIS TK_RPARENTHESIS
                {
                    $$ = std::make_shared<AST::SystemCall>(scanner.get_filename(), @1.begin.line);
                    $$->set_syscall($2);
                }

        |       TK_DOLLAR TK_IDENTIFIER TK_LPARENTHESIS sysargs TK_RPARENTHESIS
                {
                    $$ = std::make_shared<AST::SystemCall>($4, $2, scanner.get_filename(), @1.begin.line);
                }

                // $signed system task
        |       TK_DOLLAR TK_SIGNED
                {
                    $$ = std::make_shared<AST::SystemCall>(scanner.get_filename(), @1.begin.line);
                    $$->set_syscall("signed");
                }

        |       TK_DOLLAR TK_SIGNED TK_LPARENTHESIS TK_RPARENTHESIS
                {
                    $$ = std::make_shared<AST::SystemCall>(scanner.get_filename(), @1.begin.line);
                    $$->set_syscall("signed");
                }

        |       TK_DOLLAR TK_SIGNED TK_LPARENTHESIS sysargs TK_RPARENTHESIS
                {
                    $$ = std::make_shared<AST::SystemCall>($4, "signed", scanner.get_filename(), @1.begin.line);
                }
        ;


sysargs:        sysargs TK_COMMA sysarg
                {
                    $$ = $1;
                    $$->push_back($3);
                }

        |       sysarg
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back($1);
                }
        ;

sysarg:         expression {$$ = $1;}
        ;


function:       TK_FUNCTION automatic function_rettype TK_IDENTIFIER function_ports_block function_statements TK_ENDFUNCTION
                {
                    $$ = $3;
                    $$->set_filename(scanner.get_filename());
                    $$->set_line(@1.begin.line);
                    $$->set_name($4);
                    $$->set_automatic($2);
                    $$->set_ports($5);
                    $$->set_statements($6);
                }
        ;


function_rettype:
                %empty
                {
                    $$ = std::make_shared<AST::Function>();
                    $$->set_retsign(false);
                    $$->set_rettype(AST::Function::RettypeEnum::NONE);
                }

        |       widths
                {
                    $$ = std::make_shared<AST::Function>();
                    $$->set_retsign(false);
                    $$->set_retwidths($1);
                    $$->set_rettype(AST::Function::RettypeEnum::NONE);
                }

        |       TK_SIGNED widths
                {
                    $$ = std::make_shared<AST::Function>();
                    $$->set_retsign(false);
                    $$->set_retwidths($2);
                    $$->set_rettype(AST::Function::RettypeEnum::NONE);
                }

        |       TK_INTEGER
                {
                    $$ = std::make_shared<AST::Function>();
                    $$->set_retsign(true);
                    $$->set_rettype(AST::Function::RettypeEnum::INTEGER);
                }

        |       TK_REAL
                {
                    $$ = std::make_shared<AST::Function>();
                    $$->set_retsign(true);
                    $$->set_rettype(AST::Function::RettypeEnum::REAL);
                }
        ;


function_ports_block:
                TK_LPARENTHESIS function_ports TK_RPARENTHESIS TK_SEMICOLON
                {
                    $$ = $2;
                }

        |       TK_LPARENTHESIS TK_RPARENTHESIS TK_SEMICOLON
                {
                    $$ = nullptr;
                }

        |       TK_SEMICOLON
                {
                    $$ = nullptr;
                }
        ;


function_ports: function_portinfo_list
                {
                    location loc;
                    std::string error_message;
                    $$ = ParserHelpers::create_ports_decls($1, scanner.get_filename(), loc, error_message);
                    if(!$$) error(loc, error_message);
                }
        ;


function_portinfo_list:
                function_portinfo_list TK_COMMA function_portinfo
                {
                    $$ = $1;
                    $3.loc = @3;
                    $$.push_back($3);
                }

        |       function_portinfo
                {
                    $$.clear();
                    $1.loc = @1;
                    $$.push_back($1);
                }
        ;


function_portinfo:
                function_ioport
                {
                    $$ = $1;
                    $$.direction = direction_t::NONE;
                }

        |       function_portdir function_ioport
                {
                    $$ = $2;
                    $$.direction = $1;
                }
        ;


function_portdir:
                TK_INPUT
                {
                    $$ = direction_t::INPUT;
                }
        ;


function_ioport:portname
                {
                    $$ = $1;
                }

        |       TK_REG portname
                {
                    $$ = $2;
                    $$.net_type = net_type_t::REG;
                }

        |       TK_INTEGER TK_IDENTIFIER
                {
                    $$.direction = direction_t::NONE;
                    $$.net_type = net_type_t::INTEGER;
                    $$.is_signed = true;
                    $$.widths = nullptr;
                    $$.name = $2;
                }

        |       TK_REAL TK_IDENTIFIER
                {
                    $$.direction = direction_t::NONE;
                    $$.net_type = net_type_t::REAL;
                    $$.is_signed = true;
                    $$.widths = nullptr;
                    $$.name = $2;
                }
        ;


function_statements:
                function_statements function_statement
                {
                    $$ = $1;
                    $$->splice($$->end(), *$2);
                }

        |       function_statement
                {
                    $$ = $1;
                }
        ;


function_statement:
                funcvar_decl
                {
                    $$ = $1;
                }

        |       function_calc
                {
                    $$ = $1;
                }
        ;


funcvar_decl:   parameter_decl
                {
                    $$ = AST::cast_list_to<AST::Node>($1);
                }

        |       localparam_decl
                {
                    $$ = AST::cast_list_to<AST::Node>($1);
                }

        |       function_ioports_decl
                {
                    $$ = $1;
                }

        |       reg_decl
                {
                    $$ = $1;
                }

        |       integer_decl
                {
                    $$ = $1;
                }

        |       real_decl
                {
                    $$ = $1;
                }
        ;


function_ioports_decl:
                function_portdir function_portinfo_list TK_SEMICOLON
                {
                    location loc;
                    std::string error_message;
                    for(port_info_t &p: $2) p.direction = $1;
                    $$ = ParserHelpers::create_ports_decls($2, scanner.get_filename(), loc, error_message);
                    if(!$$) error(loc, error_message);
                }
        ;


function_calc:  blocking_assignment
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back(AST::to_node($1));
                }

        |       if_statement
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back(AST::to_node($1));
                }

        |       for_statement
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back(AST::to_node($1));
                }

        |       while_statement
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back(AST::to_node($1));
                }

        |       case_statement
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back(AST::to_node($1));
                }

        |       casex_statement
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back(AST::to_node($1));
                }

        |       casez_statement
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back(AST::to_node($1));
                }

        |       block
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back(AST::to_node($1));
                }

        |       named_block
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back(AST::to_node($1));
                }

        |       single_statement
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back(AST::to_node($1));
                }
        ;


function_call:  TK_IDENTIFIER TK_LPARENTHESIS function_args TK_RPARENTHESIS
                {
                    $$ = std::make_shared<AST::FunctionCall>($3, $1, scanner.get_filename(), @1.begin.line);
                }

        |       TK_IDENTIFIER TK_LPARENTHESIS TK_RPARENTHESIS
                {
                    $$ = std::make_shared<AST::FunctionCall>(scanner.get_filename(), @1.begin.line);
                    $$->set_name($1);
                }
        ;


function_args:  function_args TK_COMMA expression
                {
                    $$ = $1;
                    $$->push_back($3);
                }

        |       expression
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back($1);
                }
        ;


task:           TK_TASK automatic TK_IDENTIFIER task_ports_block task_statements TK_ENDTASK
                {
                    $$ = std::make_shared<AST::Task>(scanner.get_filename(), @1.begin.line);
                    $$->set_name($3);
                    $$->set_automatic($2);
                    $$->set_ports($4);
                    $$->set_statements($5);
                }
        ;


task_ports_block:
                TK_LPARENTHESIS task_ports TK_RPARENTHESIS TK_SEMICOLON
                {
                    $$ = $2;
                }

        |       TK_LPARENTHESIS TK_RPARENTHESIS TK_SEMICOLON
                {
                    $$ = nullptr;
                }

        |       TK_SEMICOLON
                {
                    $$ = nullptr;
                }
        ;


task_ports:     task_portinfo_list
                {
                    location loc;
                    std::string error_message;
                    $$ = ParserHelpers::create_ports_decls($1, scanner.get_filename(), loc, error_message);
                    if(!$$) error(loc, error_message);
                }
        ;


task_portinfo_list:
                task_portinfo_list TK_COMMA task_portinfo
                {
                    $$ = $1;
                    $3.loc = @3;
                    $$.push_back($3);
                }

        |       task_portinfo
                {
                    $$.clear();
                    $1.loc = @1;
                    $$.push_back($1);
                }
        ;


task_portinfo:
                task_ioport
                {
                    $$ = $1;
                    $$.direction = direction_t::NONE;
                }

        |       task_portdir task_ioport
                {
                    $$ = $2;
                    $$.direction = $1;
                }
        ;


task_portdir:   TK_INPUT
                {
                    $$ = direction_t::INPUT;
                }

        |       TK_OUTPUT
                {
                    $$ = direction_t::OUTPUT;
                }

        |       TK_INOUT
                {
                    $$ = direction_t::INOUT;
                }
        ;


task_ioport:    portname
                {
                    $$ = $1;
                }

        |       TK_REG portname
                {
                    $$ = $2;
                    $$.net_type = net_type_t::REG;
                }

        |       TK_INTEGER TK_IDENTIFIER
                {
                    $$.direction = direction_t::NONE;
                    $$.net_type = net_type_t::INTEGER;
                    $$.is_signed = true;
                    $$.widths = nullptr;
                    $$.name = $2;
                }

        |       TK_REAL TK_IDENTIFIER
                {
                    $$.direction = direction_t::NONE;
                    $$.net_type = net_type_t::REAL;
                    $$.is_signed = true;
                    $$.widths = nullptr;
                    $$.name = $2;
                }
        ;


task_statements:
                task_statements task_statement
                {
                    $$ = $1;
                    $$->splice($$->end(), *$2);
                }

        |       task_statement
                {
                    $$ = $1;
                }
        ;


task_statement: taskvar_decl
                {
                    $$ = $1;
                }

        |       task_calc
                {
                    $$ = $1;
                }
        ;


taskvar_decl:
                parameter_decl
                {
                    $$ = AST::cast_list_to<AST::Node>($1);
                }

        |       localparam_decl
                {
                    $$ = AST::cast_list_to<AST::Node>($1);
                }

        |       task_ioports_decl
                {
                    $$ = $1;
                }

        |       reg_decl
                {
                    $$ = $1;
                }

        |       integer_decl
                {
                    $$ = $1;
                }

        |       real_decl
                {
                    $$ = $1;
                }
        ;


task_ioports_decl:
                task_portdir task_portinfo_list TK_SEMICOLON
                {
                    location loc;
                    std::string error_message;
                    for(port_info_t &p: $2) p.direction = $1;
                    $$ = ParserHelpers::create_ports_decls($2, scanner.get_filename(), loc, error_message);
                    if(!$$) error(loc, error_message);
                }
        ;


task_calc:      blocking_assignment
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back(AST::to_node($1));
                }

        |       if_statement
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back(AST::to_node($1));
                }

        |       for_statement
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back(AST::to_node($1));
                }

        |       while_statement
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back(AST::to_node($1));
                }

        |       case_statement
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back(AST::to_node($1));
                }

        |       casex_statement
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back(AST::to_node($1));
                }

        |       casez_statement
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back(AST::to_node($1));
                }

        |       block
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back(AST::to_node($1));
                }

        |       named_block
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back(AST::to_node($1));
                }

        |       single_statement
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back(AST::to_node($1));
                }
        ;


task_call:      TK_IDENTIFIER TK_LPARENTHESIS task_args TK_RPARENTHESIS
                {
                    $$ = std::make_shared<AST::TaskCall>($3, $1, scanner.get_filename(), @1.begin.line);
                }

        |       TK_IDENTIFIER TK_LPARENTHESIS TK_RPARENTHESIS
                {
                    $$ = std::make_shared<AST::TaskCall>(scanner.get_filename(), @1.begin.line);
                    $$->set_name($1);
                }
        ;


task_args:      task_args TK_COMMA expression
                {
                    $$ = $1;
                    $$->push_back($3);
                }

        |       expression
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back($1);
                }
        ;


identifier:     TK_IDENTIFIER
                {
                    $$ = std::make_shared<AST::Identifier>(scanner.get_filename(), @1.begin.line);
                    $$->set_name($1);
                }

        |       scope TK_IDENTIFIER
                {
                    $$ = std::make_shared<AST::Identifier>(scanner.get_filename(), @1.begin.line);
                    $$->set_name($2);
                    $$->set_scope($1);
                }
        ;


scope:          identifier TK_DOT
                {
                    $$ = $1->get_scope();
                    if (!$$) {
                        $$ = std::make_shared<AST::IdentifierScope>(scanner.get_filename(), @1.begin.line);
                        $$->set_labellist(std::make_shared<AST::IdentifierScopeLabel::List>());
                    }

                    AST::IdentifierScopeLabel::Ptr lbl =
                        std::make_shared<AST::IdentifierScopeLabel>(scanner.get_filename(), @1.begin.line);
                    lbl->set_name($1->get_name());
                    $$->get_labellist()->push_back(lbl);
                }

        |       pointer TK_DOT
                {
                    AST::Pointer::Ptr ptr = AST::cast_to<AST::Pointer>($1);
                    AST::Identifier::Ptr var = AST::cast_to<AST::Identifier>(ptr->get_var());
                    $$ = var->get_scope();
                    if(!$$) {
                        $$ = std::make_shared<AST::IdentifierScope>(scanner.get_filename(), @1.begin.line);
                        $$->set_labellist(std::make_shared<AST::IdentifierScopeLabel::List>());
                    }

                    AST::IdentifierScopeLabel::Ptr lbl =
                        std::make_shared<AST::IdentifierScopeLabel>(scanner.get_filename(), @1.begin.line);
                    lbl->set_name(var->get_name());
                    lbl->set_loop(ptr->get_ptr());
                    $$->get_labellist()->push_back(lbl);
                }
        ;


disable:        TK_DISABLE TK_IDENTIFIER
                {
                    $$ = std::make_shared<AST::Disable>(scanner.get_filename(), @1.begin.line);
                    $$->set_dest($2);
                }
        ;


single_statement:
                delays TK_SEMICOLON
                {
                    $$ = std::make_shared<AST::SingleStatement>(scanner.get_filename(), @1.begin.line);
                    $$->set_delay($1);
                }

        |       delays systemcall TK_SEMICOLON
                {
                    $$ = std::make_shared<AST::SingleStatement>(scanner.get_filename(), @1.begin.line);
                    $$->set_statement($2);
                    $$->set_delay($1);
                }

        |       delays task_call TK_SEMICOLON
                {
                    $$ = std::make_shared<AST::SingleStatement>(scanner.get_filename(), @1.begin.line);
                    $$->set_delay($1);
                    $$->set_statement($2);
                }

        |       systemcall TK_SEMICOLON
                {
                    $$ = std::make_shared<AST::SingleStatement>(scanner.get_filename(), @1.begin.line);
                    $$->set_statement($1);
                }

        |       task_call TK_SEMICOLON
                {
                    $$ = std::make_shared<AST::SingleStatement>(scanner.get_filename(), @1.begin.line);
                    $$->set_statement($1);
                }

        |       disable TK_SEMICOLON
                {
                    $$ = std::make_shared<AST::SingleStatement>(scanner.get_filename(), @1.begin.line);
                    $$->set_statement($1);
                }
        ;


automatic:      %empty
                {
                    $$ = false;
                }

        |       TK_AUTOMATIC
                {
                    $$ = true;
                }
        ;


// Preprocessor

/* vpp_line:       TK_VPP_LINE TK_INTNUMBER TK_STRING TK_INTNUMBER */
/*                 { */
/*                     unsigned int line = static_cast<unsigned int>(atoi($2)); */
/*                     unsigned int level = static_cast<unsigned int>(atoi($4)); */
/*                     (void) level; */
/*                     driver.set_line(line); */
/*                     driver.set_filename($3); */
/*                 } */
/*         ; */


%%

namespace Veriparse {
    namespace Parser {

        void VerilogParser::error(const location_type &l,
                                  const std::string &err_message) {
            std::string s = Logger::file_report(err_message, scanner.get_filename(),
                                                l.begin.line, l.end.line, l.begin.column, l.end.column);
            LOG_ERROR << s << std::endl;
        }

        namespace ParserHelpers {
            AST::Variable::Ptr create_net_type(const decl_name_t &decl, net_type_t nt,
                                                       AST::Width::ListPtr widths, bool sign,
                                                       const std::string &filename, uint32_t line) {
                AST::Variable::Ptr node;

                switch(nt) {
                case net_type_t::INTEGER: {
                    AST::Integer::Ptr integer = std::make_shared<AST::Integer>(filename, line);
                    integer->set_name(decl.name);
                    integer->set_lengths(decl.lengths);
                    integer->set_right(decl.rvalue);
                    node = std::static_pointer_cast<AST::Variable>(integer);
                } break;

                case net_type_t::REAL: {
                    AST::Real::Ptr real = std::make_shared<AST::Real>(filename, line);
                    real->set_name(decl.name);
                    real->set_lengths(decl.lengths);
                    real->set_right(decl.rvalue);
                    node = std::static_pointer_cast<AST::Variable>(real);
                } break;

                case net_type_t::REG: {
                    AST::Reg::Ptr reg = std::make_shared<AST::Reg>(filename, line);
                    reg->set_widths(widths);
                    reg->set_name(decl.name);
                    reg->set_lengths(decl.lengths);
                    reg->set_sign(sign);
                    reg->set_right(decl.rvalue);
                    node = std::static_pointer_cast<AST::Variable>(reg);
                } break;

                case net_type_t::TRI: {
                    AST::Tri::Ptr tri = std::make_shared<AST::Tri>(filename, line);
                    tri->set_widths(widths);
                    tri->set_name(decl.name);
                    tri->set_lengths(decl.lengths);
                    tri->set_sign(sign);
                    tri->set_right(decl.rvalue);
                    tri->set_ldelay(decl.ldelay);
                    tri->set_rdelay(decl.rdelay);
                    node = std::static_pointer_cast<AST::Variable>(tri);
                } break;

                case net_type_t::WIRE: {
                    AST::Wire::Ptr wire = std::make_shared<AST::Wire>(filename, line);
                    wire->set_widths(widths);
                    wire->set_name(decl.name);
                    wire->set_lengths(decl.lengths);
                    wire->set_sign(sign);
                    wire->set_right(decl.rvalue);
                    wire->set_ldelay(decl.ldelay);
                    wire->set_rdelay(decl.rdelay);
                    node = std::static_pointer_cast<AST::Variable>(wire);
                } break;

                case net_type_t::SUPPLY0: {
                    AST::Supply0::Ptr supply0 = std::make_shared<AST::Supply0>(filename, line);
                    supply0->set_widths(widths);
                    supply0->set_name(decl.name);
                    supply0->set_lengths(decl.lengths);
                    supply0->set_sign(sign);
                    supply0->set_right(decl.rvalue);
                    supply0->set_right(decl.rvalue);
                    supply0->set_ldelay(decl.ldelay);
                    supply0->set_rdelay(decl.rdelay);
                    node = std::static_pointer_cast<AST::Variable>(supply0);
                } break;

                case net_type_t::SUPPLY1: {
                    AST::Supply1::Ptr supply1 = std::make_shared<AST::Supply1>(filename, line);
                    supply1->set_widths(widths);
                    supply1->set_name(decl.name);
                    supply1->set_lengths(decl.lengths);
                    supply1->set_sign(sign);
                    supply1->set_right(decl.rvalue);
                    supply1->set_ldelay(decl.ldelay);
                    supply1->set_rdelay(decl.rdelay);
                    node = std::static_pointer_cast<AST::Variable>(supply1);
                } break;

                default: break;
                }

                return node;
            }

            AST::Ioport::Ptr create_ioport_decls(direction_t direction, net_type_t net_type, bool is_signed,
                                                       AST::Width::ListPtr widths, std::string name,
                                                       const std::string &filename, uint32_t line) {

                AST::Ioport::Ptr ioport = std::make_shared<AST::Ioport>(filename, line);

                decl_name_t decl;
                decl.name = name;
                decl.lengths = nullptr;

                switch(direction) {
                case direction_t::INPUT:
                    ioport->set_first(std::static_pointer_cast<AST::IODir>
                                      (std::make_shared<AST::Input>(widths, name, is_signed, filename, line)));
                    break;

                case direction_t::INOUT:
                    ioport->set_first(std::static_pointer_cast<AST::IODir>
                                      (std::make_shared<AST::Inout>(widths, name, is_signed, filename, line)));
                    break;

                case direction_t::OUTPUT:
                    ioport->set_first(std::static_pointer_cast<AST::IODir>
                                      (std::make_shared<AST::Output>(widths, name, is_signed, filename, line)));
                    break;

                default:
                    break;
                }

                if (widths) {
                    ioport->set_second(ParserHelpers::create_net_type(decl, net_type, AST::Width::clone_list(widths),
                                                                is_signed, filename, line));
                }
                else {
                    ioport->set_second(ParserHelpers::create_net_type(decl, net_type, nullptr,
                                                                is_signed, filename, line));
                }

                return ioport;
            }


            AST::Node::ListPtr create_ports_decls(const std::list<port_info_t> &port_list,
                                                  const std::string &filename,
                                                  location &loc, std::string &error_message) {
                direction_t last_dir = direction_t::NONE;
                net_type_t last_net = net_type_t::NONE;
                bool last_is_signed = false;
                AST::Width::ListPtr last_widths(nullptr);

                AST::Node::ListPtr node_list = std::make_shared<AST::Node::List>();

                if (port_list.empty()) {
                    return node_list;
                }

                if (port_list.front().direction == direction_t::NONE) {
                    // Check for name only ports (without qualifiers)
                    for(const port_info_t &pinfo: port_list) {
                        if ((pinfo.direction != direction_t::NONE) || (pinfo.net_type != net_type_t::NONE) ||
                            (pinfo.is_signed) || (pinfo.widths)) {
                            loc = pinfo.loc;
                            error_message = std::string("missing port direction qualifier");
                            return nullptr;
                        }
                        AST::Port::Ptr p = std::make_shared<AST::Port>(filename, pinfo.loc.begin.line);
                        p->set_name(pinfo.name);
                        node_list->push_back(p);
                    }
                }

                else {
                    // Check for qualified ports
                    for(const port_info_t &pinfo: port_list) {
                        direction_t dir;
                        net_type_t net_type;
                        AST::Width::ListPtr widths;
                        bool is_signed;

                        if (pinfo.direction == direction_t::NONE) {
                            if ((pinfo.net_type != net_type_t::NONE) || (pinfo.is_signed) || (pinfo.widths)) {
                                loc = pinfo.loc;
                                error_message = std::string("missing port direction qualifier");
                                return nullptr;
                            }
                            dir = last_dir;
                            net_type = last_net;
                            widths = last_widths;
                            is_signed = last_is_signed;
                        }
                        else {
                            dir = pinfo.direction;
                            net_type = pinfo.net_type;
                            widths = pinfo.widths;
                            is_signed = pinfo.is_signed;
                        }

                        if (widths) widths = AST::Width::clone_list(widths);

                        AST::Ioport::Ptr iop = ParserHelpers::create_ioport_decls(dir, net_type, is_signed,
                                                                                  widths, pinfo.name,
                                                                                  filename,
                                                                                  pinfo.loc.begin.line);
                        node_list->push_back(iop);
                        last_dir = dir;
                        last_net = net_type;
                        last_widths = widths;
                        last_is_signed = is_signed;
                    }
                }

                return node_list;
            }

        }
    }
}
