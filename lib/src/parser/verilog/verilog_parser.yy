// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
%skeleton "lalr1.cc"
%require  "3.4"

%defines
%locations

%define api.namespace {Veriparse::Parser}
%define api.parser.class {VerilogParser}

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


namespace Veriparse
{
namespace Parser
{

class VerilogDriver;
class VerilogScanner;

// The real net types parsed today (the data types reg/logic/integer/real are
// NOT nets in the new model — they are variable DataTypes via data_type).
enum class net_type_t {
	 TRI, WIRE, SUPPLY0, SUPPLY1, NONE
};

// SystemVerilog data_type kinds (Annex A). Step 2 starts with the built-in
// scalar types; logic/reg/integer/real, struct/union/enum and named/scoped
// types are folded into the same `data_type` rule in later steps.
enum class data_type_kind_t {
	 BIT, LOGIC, REG, BYTE, SHORTINT, INT, LONGINT, INTEGER,
	 REAL, SHORTREAL, REALTIME, STRUCT, UNION, ENUM, NAMED,
	 TYPEOP, // type(expr) / type(data_type): the prebuilt node lives in inline_def
	 IFACE   // virtual interface type (§25.9): prebuilt InterfaceType in inline_def
};

// Explicit signing of a data_type (`signed`/`unsigned`/absent). Kept tri-state
// because the default differs by type: integer_vector_type is unsigned by
// default, integer_atom_type is signed by default (IEEE 1800-2017 §6.8). Each
// consumer resolves it to data_type_t::is_signed against its own default.
enum class signing_t { NONE, SIGNED, UNSIGNED };

// Semantic carrier filled by the `data_type` grammar rule; build_variable()
// turns it into the matching AST node.
typedef struct {
	 data_type_kind_t kind;
	 signing_t signing;                   // tri-state, preserved (default resolved by a pass)
	 AST::Dimension::ListPtr packed_dims; // packed dimensions
	 AST::NamedType::Ptr named;           // NAMED: the named-type reference (name + :: scope)
	 AST::DataType::Ptr inline_def;       // STRUCT/UNION/ENUM: inline def node
} data_type_t;

// SystemVerilog data_declaration qualifiers ([const] [var] [lifetime]); folded
// into a DataModifier wrapper carried above the variable's data-type node.
struct data_qualifiers_t {
	 bool is_var;
	 bool is_const;
	 AST::Var::LifetimeEnum lifetime;
};

// data_type_or_implicit = implicit ([signing] {packed_dimension}): the type of a
// `var`/`const` declaration with no type keyword. Materialised as an ImplicitType
// node (implied type logic, IEEE 1800-2017 §6.8).
struct implicit_type_t {
	 signing_t signing;          // tri-state, preserved (default resolved by a pass)
	 AST::Dimension::ListPtr widths;
};

// A net (wire/tri/supply) carrying an explicit integer_vector data type must be
// 4-state, and a net keyword may not be directly followed by `reg` (IEEE
// 1800-2017 6.7.1). Of bit/logic/reg only `logic` is valid. Returns an error
// message for an invalid kind, or nullptr.
static inline const char *net_integer_vector_error(data_type_kind_t kind)
{
	 switch(kind) {
	 case data_type_kind_t::REG:
		  return "a net type keyword shall not be directly followed by 'reg' "
				 "(IEEE 1800-2017 6.7.1)";
	 case data_type_kind_t::BIT:
		  return "a net data type shall be 4-state; 'bit' is 2-state "
				 "(IEEE 1800-2017 6.7.1)";
	 default:
		  return nullptr;
	 }
}

// The keyword spelling of a built-in scalar data_type_kind_t (for a cast type,
// rendered as e.g. `int'(x)`). Aggregate/named kinds have no keyword.
static inline const char *data_type_kind_keyword(data_type_kind_t kind)
{
	 switch(kind) {
	 case data_type_kind_t::BIT:       return "bit";
	 case data_type_kind_t::LOGIC:     return "logic";
	 case data_type_kind_t::REG:       return "reg";
	 case data_type_kind_t::BYTE:      return "byte";
	 case data_type_kind_t::SHORTINT:  return "shortint";
	 case data_type_kind_t::INT:       return "int";
	 case data_type_kind_t::LONGINT:   return "longint";
	 case data_type_kind_t::INTEGER:   return "integer";
	 case data_type_kind_t::REAL:      return "real";
	 case data_type_kind_t::SHORTREAL: return "shortreal";
	 case data_type_kind_t::REALTIME:  return "realtime";
	 default:                          return "";
	 }
}

enum class direction_t {
	 INPUT, INOUT, OUTPUT, NONE
};

struct port_info_t {
	 direction_t direction;
	 net_type_t net_type;
	 signing_t signing;          // tri-state, preserved (default resolved by a pass)
	 AST::Dimension::ListPtr widths;
	 std::string name;
	 std::string type_name;    // user-defined type name (typedef); empty if built-in
	 std::string type_package; // package scope of the type (pkg::); empty if local
	 std::string modport;      // modport suffix of an interface port (my_if.mp); empty if none
	 location loc;
	 bool has_data_type = false; // true when the port type is a data_type below
	 data_type_t data_type{};    // bit/atom/non-integer/struct/union/enum port type
};

typedef struct {
	 std::string name;
	 AST::DelayStatement::Ptr ldelay;
	 AST::DelayStatement::Ptr rdelay;
	 AST::Dimension::ListPtr lengths; // unpacked dimensions of this declared name
	 AST::Rvalue::Ptr rvalue;
} decl_name_t;

// What follows `data_qualifiers TK_IDENTIFIER [dims]` once a bracketed dimension
// has been seen: a following identifier means the consumed id was a *type* and
// the dims were its packed dimension (named, `names` are the variables); `;`,
// `=` or `,` mean the id was the *first variable* and the dims were its unpacked
// dimension.
struct id_dims_tail_t {
	 bool is_named;
	 AST::Rvalue::Ptr rvalue;
	 std::list<decl_name_t> names;
};

// What follows `data_qualifiers TK_IDENTIFIER`: either the identifier was a type
// name (is_named, `names` are the declared variables, `dims` its packed dims) or
// it was the first variable of an implicit declaration (its `dims` unpacked +
// `first_rvalue`, plus any further `names`). This factoring keeps the grammar
// LALR(1): `var a` vs `var T x` is decided by the token after the first
// identifier (and, for the bracketed forms, the token after `]`). `dims` is
// captured as a length list and converted to packed widths for the named case.
struct id_decl_tail_t {
	 bool is_named;
	 AST::Dimension::ListPtr dims;
	 AST::Rvalue::Ptr first_rvalue;
	 std::list<decl_name_t> names;
};

// A `const` variable must be initialised in its declaration (IEEE 1800-2017
// 6.8). True if any name in a const declaration lacks an initializer.
static inline bool const_without_init(const data_qualifiers_t &q,
												  const std::list<decl_name_t> &names) {
	 if(!q.is_const) {
		  return false;
	 }
	 for(const decl_name_t &d : names) {
		  if(!d.rvalue) {
				return true;
		  }
	 }
	 return false;
}

// Reinterpret a parsed bracket list as packed dimensions when the leading
// identifier turns out to be a named type (`var my_t [3:0] x`): the same `[..]`
// is captured as `lengths` (which also accepts the single-size `[N]` form) and
// cloned here. The caller rejects an illegal `[N]` packed dim (ADR-0001 §3.8).
static inline AST::Dimension::ListPtr lengths_to_widths(const AST::Dimension::ListPtr &dims) {
	 return dims ? AST::Dimension::clone_list(dims) : nullptr;
}

namespace ParserHelpers
{
// Map the parser's tri-state signing onto the AST enum.
AST::DataType::SigningEnum to_signing(signing_t s);

// Build the DataType node a data_type_t denotes (built-in keyword node, a cloned
// NamedType, or a cloned inline struct/union/enum), with signing + packed dims.
AST::DataType::Ptr make_data_type(const data_type_t &dt,
                                  const std::string &filename="", uint32_t line=0);

// Build a NamedType reference: name + optional single-segment `::` scope.
AST::NamedType::Ptr make_named_type(const std::string &name, const std::string &package,
                                    const std::string &filename="", uint32_t line=0);

// Value-side SV `::` scope list (single package segment) for a name ref.
AST::ScopeName::ListPtr pkg_scope(const std::string &package,
                                  const std::string &filename="", uint32_t line=0);

// task/function argument list (one Arg per port_info).
AST::Arg::ListPtr create_args_decls(const std::list<port_info_t> &port_list,
                                    const std::string &filename);

// Upcast a Param list to a Declaration list (Module.params child).
AST::Declaration::ListPtr to_decl_list(const AST::Param::ListPtr &in);

// Map the scanner's `default_nettype (an AST::Module enum) onto the identical
// AST::Interface enum.
AST::Interface::Default_nettypeEnum to_interface_nettype(AST::Module::Default_nettypeEnum nt);

// Map a parsed port direction onto the ModportPort enum (a modport simple port
// always carries an explicit keyword, so NONE cannot occur).
AST::ModportPort::DirectionEnum to_modport_dir(direction_t d);

// Build a net declaration node (WireNet/TriNet/Supply0Net/Supply1Net) carrying
// the given data type, name, unpacked dims, continuous assign and delays.
AST::Net::Ptr create_net_type(const decl_name_t &decl, net_type_t nt, AST::DataType::Ptr type,
                              const std::string &filename="", uint32_t line=0);

// Build the Var for `data_type name {unpacked} [= init]`.
AST::Var::Ptr build_variable(const data_type_t &dt, const decl_name_t &decl,
                             const std::string &filename="", uint32_t line=0);

// Build the Var for an implicit-type declaration (`var a`, `var [3:0] a`):
// type = ImplicitType(signing, packed dims).
AST::Var::Ptr build_implicit_type(signing_t signing, AST::Dimension::ListPtr widths,
                                  const decl_name_t &decl,
                                  const std::string &filename="", uint32_t line=0);

// Set the [const][var][lifetime] qualifiers on a Var and return it.
AST::Node::Ptr wrap_data_modifier(AST::Var::Ptr inner, const data_qualifiers_t &q);

// Build a named-type declaration (`var my_t x`): one Var per name carrying the
// NamedType (cloned) + packed dims and the qualifiers.
AST::Node::ListPtr build_named_modifier(const AST::NamedType::Ptr &ref,
                                        AST::Dimension::ListPtr widths,
                                        const std::list<decl_name_t> &names,
                                        const data_qualifiers_t &q,
                                        const std::string &filename="", uint32_t line=0);

// Build struct/union members for `data_type list_of_variable_decl_assignments`.
AST::Member::ListPtr build_struct_members(const data_type_t &dt,
                                          const std::list<decl_name_t> &names,
                                          const std::string &filename="", uint32_t line=0);

// Build the bare DataType a data_type denotes (typedef/return/member type).
AST::DataType::Ptr build_data_type_def(const data_type_t &dt,
                                       const std::string &filename="", uint32_t line=0);

// Build a Port wrapping a directed declaration.
AST::Port::Ptr create_ioport_decls(direction_t direction, net_type_t net_type, signing_t signing,
                                   AST::Dimension::ListPtr widths, std::string name,
                                   const std::string &filename="", uint32_t line=0);

AST::Port::Ptr create_typed_ioport_decls(direction_t direction, const std::string &type_name,
                                         const std::string &type_package, const std::string &name,
                                         const std::string &filename="", uint32_t line=0);

// Directionless named-type port (`my_if i`, `my_if.mp i`, ADR-0002): a Port with
// no direction wrapping an Arg. With a modport the type is decisive
// (InterfaceType); bare it stays a NamedType promoted by the resolution pass.
AST::Port::Ptr create_interface_port(const std::string &type_name, const std::string &type_package,
                                     const std::string &modport, const std::string &name,
                                     const std::string &filename="", uint32_t line=0);

AST::Port::Ptr create_data_type_port(direction_t direction, const data_type_t &dt,
                                     const std::string &name,
                                     const std::string &filename="", uint32_t line=0);

AST::Port::Ptr create_net_data_type_port(direction_t direction, net_type_t net_type,
                                         const data_type_t &dt, const std::string &name,
                                         const std::string &filename="", uint32_t line=0);

// if return null, the create_ports_decls failed. Error in loc and error_message.
AST::Port::ListPtr create_ports_decls(const std::list<port_info_t> &port_list,
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
%token                  TK_UNSIGNED     "'unsigned'"
%token                  TK_PARAMETER    "'parameter'"
%token                  TK_LOCALPARAM   "'localparam'"
%token                  TK_SUPPLY0      "'supply0'"
%token                  TK_SUPPLY1      "'supply1'"
%token                  TK_AUTOMATIC    "'automatic'"
%token                  TK_VAR          "'var'"
%token                  TK_CONST        "'const'"
%token                  TK_DEFPARAM     "'defparam'"
%token                  TK_ASSIGN       "'assign'"
%token                  TK_ALWAYS       "'always'"
%token                  TK_SENS_OR      "'or'"
%token                  TK_POSEDGE      "'posedge'"
%token                  TK_NEGEDGE      "'negedge'"
%token                  TK_INITIAL      "'initial'"
%token                  TK_IF           "'if'"
%token                  TK_ELSE         "'else'"
%token                  TK_REPEAT       "'repeat'"
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
%token                  TK_ALWAYS_FF    "'always_ff'"
%token                  TK_ALWAYS_COMB  "'always_comb'"
%token                  TK_ALWAYS_LATCH "'always_latch'"
%token                  TK_LOGIC        "'logic'"
%token                  TK_INT          "'int'"
%token                  TK_BIT          "'bit'"
%token                  TK_BYTE         "'byte'"
%token                  TK_SHORTINT     "'shortint'"
%token                  TK_LONGINT      "'longint'"
%token                  TK_SHORTREAL    "'shortreal'"
%token                  TK_REALTIME     "'realtime'"
%token                  TK_UNIQUE       "'unique'"
%token                  TK_PRIORITY     "'priority'"
%token                  TK_TYPEDEF      "'typedef'"
%token                  TK_TYPE         "'type'"
%token                  TK_VOID         "'void'"
%token                  TK_RETURN       "'return'"
%token                  TK_BREAK        "'break'"
%token                  TK_CONTINUE     "'continue'"
%token                  TK_ENUM         "'enum'"
%token                  TK_STRUCT       "'struct'"
%token                  TK_UNION        "'union'"
%token                  TK_PACKED       "'packed'"
%token                  TK_PACKAGE      "'package'"
%token                  TK_ENDPACKAGE   "'endpackage'"
%token                  TK_IMPORT       "'import'"
%token                  TK_EXPORT       "'export'"
%token                  TK_STATIC       "'static'"
%token                  TK_INTERFACE    "'interface'"
%token                  TK_ENDINTERFACE "'endinterface'"
%token                  TK_MODPORT      "'modport'"
%token                  TK_VIRTUAL      "'virtual'"
%token                  TK_DOLLAR_UNIT  "'$unit'"
%token                  TK_COLONCOLON   "'::'"
%token                  TK_VPP_LINE     "`line'"
%token                  TK_LATTR        "(*"
%token                  TK_RATTR        "*)"
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
%token                  TK_TICK_LBRACE  "''{'"
%token                  TK_TICK_LPAREN  "''('"
%token                  TK_DELAY        "'#'"
%token                  TK_DOLLAR       "'$'"
%token  <std::string>   TK_INTNUMBER    "'integer const'"
%token  <std::string>   TK_FLOATNUMBER  "'floating const'"
%token  <std::string>   TK_IDENTIFIER   "identifier"
%token  <std::string>   TK_STRING       "string"
%token  <std::string>   TK_PRAGMA       "pragma"


// Token precedences
%left                   TK_RPARENTHESIS
%left                   TK_SEMICOLON
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
%type   <port_info_t>                        portinfo portname ioport
%type   <port_info_t>                        port_typed_name implicit_port_type
%type   <direction_t>                        portdir
%type   <net_type_t>                         net_type

%type   <std::list<decl_name_t>>             net_decl_namelist var_decl_namelist
%type   <decl_name_t>                        net_decl_name var_decl_name

%type   <AST::Source::Ptr>                   source_text
%type   <AST::Description::Ptr>              description
%type   <AST::Node::ListPtr>                 definitions
%type   <AST::Node::Ptr>                     definition
%type   <AST::Module::Ptr>                   moduledef
%type   <AST::Interface::Ptr>                interfacedef
%type   <AST::Interface::LifetimeEnum>       interface_lifetime
%type   <AST::Node::ListPtr>                 modport_decl modport_items
%type   <AST::Modport::Ptr>                  modport_item
%type   <AST::ModportPort::ListPtr>          modport_port_list
%type   <std::string>                        virtual_iface_modport
%type   <AST::Package::Ptr>                  packagedef
%type   <AST::Node::ListPtr>                 module_imports
%type   <AST::Node::ListPtr>                 import_decl import_list export_decl
%type   <AST::Import::Ptr>                   import_item
%type   <AST::Identifier::Ptr>                scoped_ref
%type   <std::string>                        package_scope
%type   <AST::Package::LifetimeEnum>         package_lifetime
%type   <data_qualifiers_t>                  data_qualifiers data_qualifier
%type   <implicit_type_t>                    implicit_signing_or_dims
%type   <id_dims_tail_t>                     id_dims_tail
%type   <id_decl_tail_t>                     id_decl_tail
%type   <AST::Pragmalist::Ptr>               pragmalist
%type   <AST::Pragma::ListPtr>               pragma
%type   <AST::Node::Ptr>                     expression ternary paren_expression
%type   <AST::Node::ListPtr>                 items item standard_item_base standard_item
%type   <AST::Node::ListPtr>                 ioports_decl
%type   <AST::Node::ListPtr>                 net_decl
%type   <data_type_t>                        data_type port_data_type
%type   <AST::Node::ListPtr>                 data_declaration
%type   <AST::Node::ListPtr>                 typed_var_decl
%type   <data_type_kind_t>                   integer_vector_type integer_atom_type non_integer_type
%type   <signing_t>                          signing
%type   <AST::Dimension::ListPtr>                packed_dimensions

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
%type   <AST::AlwaysFF::Ptr>                 always_ff
%type   <AST::AlwaysComb::Ptr>               always_comb
%type   <AST::AlwaysLatch::Ptr>              always_latch
%type   <AST::Initial::Ptr>                  initial
%type   <AST::Node::Ptr>                     initial_statement
%type   <AST::Param::ListPtr>            params_block params param_assignment_list parameter_decl
%type   <AST::Param::Ptr>                param_assignment param_start
%type   <AST::DataType::Ptr>             param_type
%type   <AST::Defparamlist::Ptr>             defparam
%type   <AST::Defparam::ListPtr>             defparamlist
%type   <AST::Defparam::Ptr>                 defparam_assignment
%type   <AST::Param::ListPtr>           localparam_assignment_list localparam_decl
%type   <AST::Param::Ptr>               localparam_assignment
%type   <AST::DataType::Ptr>            localparam_type
%type   <AST::Dimension::ListPtr>                widths
%type   <AST::Dimension::Ptr>                    width
%type   <AST::Dimension::Ptr>                   length
%type   <AST::Dimension::ListPtr>               lengths
%type   <AST::Port::ListPtr>                 ports_block ports
%type   <AST::Lvalue::Ptr>                   lvalue
%type   <AST::Rvalue::Ptr>                   rvalue
%type   <AST::DelayStatement::Ptr>           delays
%type   <AST::Node::Ptr>                     const_expression
%type   <AST::Node::Ptr>                     pointer lpointer lpartselect
%type   <AST::Identifier::Ptr>               identifier
%type   <AST::HierName::Ptr>          scope
%type   <AST::Lconcat::Ptr>                  lconcat
%type   <AST::Node::ListPtr>                 lconcatlist
%type   <AST::Node::Ptr>                     lconcat_one
%type   <AST::Concat::Ptr>                   concat
%type   <AST::Node::ListPtr>                 concatlist
%type   <AST::Repeat::Ptr>                   repeat
%type   <AST::AssignmentPattern::Ptr>        assignment_pattern
%type   <AST::Node::ListPtr>                 pattern_items
%type   <AST::Node::Ptr>                     pattern_item
%type   <AST::Cast::Ptr>                     cast
%type   <AST::Cast::Ptr>                     casting_type
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
%type   <AST::Node::Ptr>                     basic_statement_base
%type   <AST::IfStatement::Ptr>              if_statement
%type   <AST::Node::Ptr>                     cond true_statement false_statement ifcontent_statement

%type   <AST::CaseStatement::Ptr>            case_statement
%type   <AST::CasexStatement::Ptr>           casex_statement
%type   <AST::CasezStatement::Ptr>           casez_statement
%type   <AST::UniqueCaseStatement::Ptr>      unique_case_statement
%type   <AST::PriorityCaseStatement::Ptr>    priority_case_statement
%type   <AST::Node::Ptr>                     case_comp
%type   <AST::Case::ListPtr>                 casecontent_statements
%type   <AST::Case::Ptr>                     casecontent_statement
%type   <AST::Node::ListPtr>                 casecontent_conditions

%type   <AST::ForStatement::Ptr>             for_statement
%type   <AST::BlockingSubstitution::Ptr>     forpre forpost
%type   <AST::Node::Ptr>                     forcond forcontent_statement

%type   <AST::RepeatStatement::Ptr>          repeat_statement

%type   <AST::WhileStatement::Ptr>           while_statement
%type   <AST::Node::Ptr>                     whilecontent_statement
%type   <AST::EventStatement::Ptr>           event_statement  // Only used during the parsing to avoid conflict
%type   <AST::WaitStatement::Ptr>            wait_statement
%type   <AST::Node::Ptr>                     waitcontent_statement
%type   <AST::ForeverStatement::Ptr>         forever_statement
%type   <AST::Block::Ptr>                    block named_block
%type   <AST::Node::ListPtr>                 block_statements block_statement
%type   <AST::ParallelBlock::Ptr>            parallelblock named_parallelblock

%type   <AST::BlockingSubstitution::Ptr>     blocking_assignment
%type   <AST::NonblockingSubstitution::Ptr>  nonblocking_assignment
%type   <AST::SingleStatement::Ptr>          single_statement

%type   <AST::Node::ListPtr>                 function_ioports_decl
%type   <std::list<port_info_t>>             function_portinfo_list
%type   <port_info_t>                        function_ioport function_portinfo
%type   <direction_t>                        function_portdir
%type   <AST::FunctionCall::Ptr>             function_call
%type   <AST::Node::ListPtr>                 function_args
%type   <AST::Arg::ListPtr>                  function_ports_block function_ports
%type   <AST::Function::Ptr>                 function function_rettype_name
%type   <AST::Node::ListPtr>                 function_statement function_statements
%type   <AST::Node::ListPtr>                 funcvar_decl function_calc

%type   <AST::Task::Ptr>                     task
%type   <AST::Node::ListPtr>                 task_ioports_decl
%type   <std::list<port_info_t>>             task_portinfo_list
%type   <port_info_t>                        task_ioport task_portinfo
%type   <direction_t>                        task_portdir
%type   <AST::TaskCall::Ptr>                 task_call
%type   <AST::Node::ListPtr>                 task_args
%type   <AST::Arg::ListPtr>                  task_ports_block task_ports
%type   <AST::Node::ListPtr>                 task_statement task_statements
%type   <AST::Node::ListPtr>                 taskvar_decl task_calc

%type   <AST::Disable::Ptr>                  disable
%type   <AST::Node::Ptr>                     jump_statement
%type   <bool>                               automatic
%type   <AST::Typedef::Ptr>                  typedef_decl
%type   <AST::EnumType::Ptr>                  enum_def
%type   <AST::EnumItem::ListPtr>             enum_items
%type   <AST::EnumItem::Ptr>                 enum_item
%type   <AST::StructType::Ptr>                struct_def
%type   <AST::UnionType::Ptr>                    union_def
%type   <AST::Member::ListPtr>         struct_members struct_member


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

        |       import_decl
                {
                    // A package_import_declaration can introduce several Import
                    // nodes (import a::b, c::*;); splice them into the list.
                    $$ = $1;
                }

        |       definitions import_decl
                {
                    $$ = $1;
                    $$->splice($$->end(), *$2);
                }
        ;


definition:     moduledef
                {
                    $$ = AST::to_node($1);
                }

        |       pragmalist moduledef
                {
                    auto stmts = std::make_shared<AST::Node::List>();
                    stmts->push_back($2);
                    $1->set_statements(stmts);
                    $$ = AST::to_node($1);
                }

        |       packagedef
                {
                    $$ = AST::to_node($1);
                }

        |       interfacedef
                {
                    $$ = AST::to_node($1);
                }
        ;


        // The optional `automatic` lifetime is accepted but not modelled (it has
        // no effect on the synthesizable subset). `static` is the default.
packagedef:     TK_PACKAGE package_lifetime TK_IDENTIFIER TK_SEMICOLON items TK_ENDPACKAGE
                {
                    $$ = std::make_shared<AST::Package>(scanner.get_filename(), @1.begin.line);
                    $$->set_name($3);
                    $$->set_lifetime($2);
                    $$->set_items($5);
                }

        |       TK_PACKAGE package_lifetime TK_IDENTIFIER TK_SEMICOLON items TK_ENDPACKAGE TK_COLON TK_IDENTIFIER
                {
                    $$ = std::make_shared<AST::Package>(scanner.get_filename(), @1.begin.line);
                    $$->set_name($3);
                    $$->set_lifetime($2);
                    $$->set_items($5);
                }

        |       TK_PACKAGE package_lifetime TK_IDENTIFIER TK_SEMICOLON TK_ENDPACKAGE
                {
                    $$ = std::make_shared<AST::Package>(scanner.get_filename(), @1.begin.line);
                    $$->set_name($3);
                    $$->set_lifetime($2);
                }

        |       TK_PACKAGE package_lifetime TK_IDENTIFIER TK_SEMICOLON TK_ENDPACKAGE TK_COLON TK_IDENTIFIER
                {
                    $$ = std::make_shared<AST::Package>(scanner.get_filename(), @1.begin.line);
                    $$->set_name($3);
                    $$->set_lifetime($2);
                }
        ;


// Optional package lifetime: NONE (unspecified, i.e. static), AUTOMATIC or
// STATIC. Stored on the Package node so it round-trips.
package_lifetime:
                %empty
                {
                    $$ = AST::Package::LifetimeEnum::NONE;
                }

        |       TK_AUTOMATIC
                {
                    $$ = AST::Package::LifetimeEnum::AUTOMATIC;
                }

        |       TK_STATIC
                {
                    $$ = AST::Package::LifetimeEnum::STATIC;
                }
        ;


import_decl:    TK_IMPORT import_list TK_SEMICOLON
                {
                    $$ = $2;
                }
        ;


// Package re-export (IEEE 1800-2017 §26.6). Reuse import_list's parsing and
// convert each Import into an Export carrying the same package/symbol.
export_decl:    TK_EXPORT import_list TK_SEMICOLON
                {
                    auto list = std::make_shared<AST::Node::List>();
                    for(const auto &n : *$2) {
                        auto imp = AST::cast_to<AST::Import>(n);
                        auto exp = std::make_shared<AST::Export>(scanner.get_filename(),
                                                                 @1.begin.line);
                        exp->set_package(imp->get_package());
                        exp->set_symbol(imp->get_symbol());
                        list->push_back(AST::to_node(exp));
                    }
                    $$ = list;
                }

        |       TK_EXPORT TK_TIMES TK_COLONCOLON TK_TIMES TK_SEMICOLON
                {
                    auto exp = std::make_shared<AST::Export>(scanner.get_filename(), @1.begin.line);
                    exp->set_package("*");
                    exp->set_symbol("*");
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back(AST::to_node(exp));
                }
        ;


import_list:    import_item
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back(AST::to_node($1));
                }

        |       import_list TK_COMMA import_item
                {
                    $$ = $1;
                    $$->push_back(AST::to_node($3));
                }
        ;


import_item:    package_scope TK_IDENTIFIER
                {
                    $$ = std::make_shared<AST::Import>(scanner.get_filename(), @1.begin.line);
                    $$->set_package($1);
                    $$->set_symbol($2);
                }

        |       package_scope TK_TIMES
                {
                    $$ = std::make_shared<AST::Import>(scanner.get_filename(), @1.begin.line);
                    $$->set_package($1);
                    $$->set_symbol("*");
                }
        ;

pragmalist:     TK_LATTR pragma TK_RATTR
                {
                    $$ = std::make_shared<AST::Pragmalist>(scanner.get_filename(), @1.begin.line);
                    $$->set_pragmas($2);
                }

        |       pragmalist TK_LATTR pragma TK_RATTR
                {
                    $$ = $1;
                    auto existing = $$->get_pragmas();
                    if (!existing) existing = std::make_shared<AST::Pragma::List>();
                    for (const auto &p : *$3) existing->push_back(p);
                    $$->set_pragmas(existing);
                }
        ;
        ;

pragma:         pragma TK_COMMA TK_IDENTIFIER TK_EQUALS expression
                {
                    $$ = $1;
                    $$->push_back(std::make_shared<AST::Pragma>($5, $3,
                                                                scanner.get_filename(), @1.begin.line));
                }

        |       pragma TK_COMMA TK_IDENTIFIER
                {
                    auto pragma = std::make_shared<AST::Pragma>(scanner.get_filename(), @3.begin.line);
                    pragma->set_name($3);
                    $$ = $1;
                    $$->push_back(pragma);
                }

        |       TK_IDENTIFIER TK_EQUALS expression
                {
                    $$ = std::make_shared<AST::Pragma::List>();
                    $$->push_back(std::make_shared<AST::Pragma>($3, $1,
                                                                scanner.get_filename(), @1.begin.line));
                }

        |       TK_IDENTIFIER
                {
                    auto pragma = std::make_shared<AST::Pragma>(scanner.get_filename(), @1.begin.line);
                    pragma->set_name($1);
                    $$ = std::make_shared<AST::Pragma::List>();
                    $$->push_back(pragma);
                }
        ;


moduledef:      TK_MODULE modulename module_imports params_block ports_block items TK_ENDMODULE
                {
                    // SV allows package imports in the module header; they import
                    // into module scope, so prepend them to the items.
                    $6->splice($6->begin(), *$3);
                    $$ = std::make_shared<AST::Module>(scanner.get_filename(), @1.begin.line);
                    $$->set_default_nettype(scanner.get_default_nettype());
                    $$->set_name($2);
                    $$->set_params(ParserHelpers::to_decl_list($4));
                    $$->set_ports($5);
                    $$->set_items($6);
                }

        |       TK_MODULE modulename module_imports ports_block items TK_ENDMODULE
                {
                    $5->splice($5->begin(), *$3);
                    $$ = std::make_shared<AST::Module>(scanner.get_filename(), @1.begin.line);
                    $$->set_default_nettype(scanner.get_default_nettype());
                    $$->set_name($2);
                    $$->set_ports($4);
                    $$->set_items($5);
                }

        |       TK_MODULE modulename module_imports params_block ports_block TK_ENDMODULE
                {
                    $$ = std::make_shared<AST::Module>(scanner.get_filename(), @1.begin.line);
                    $$->set_default_nettype(scanner.get_default_nettype());
                    $$->set_name($2);
                    $$->set_params(ParserHelpers::to_decl_list($4));
                    $$->set_ports($5);
                    if (!$3->empty()) $$->set_items($3);
                }

        |       TK_MODULE modulename module_imports ports_block TK_ENDMODULE
                {
                    $$ = std::make_shared<AST::Module>(scanner.get_filename(), @1.begin.line);
                    $$->set_default_nettype(scanner.get_default_nettype());
                    $$->set_name($2);
                    $$->set_ports($4);
                    if (!$3->empty()) $$->set_items($3);
                }
        ;


// Zero or more package_import_declarations in the module header
// (module_ansi_header, 1800-2017 Annex A). Empty for plain Verilog modules,
// so existing modules parse unchanged.
module_imports: %empty
                {
                    $$ = std::make_shared<AST::Node::List>();
                }

        |       module_imports import_decl
                {
                    $$ = $1;
                    $$->splice($$->end(), *$2);
                }
        ;


modulename:     TK_IDENTIFIER
                {
                    $$=$1;
                }

        |       TK_SENS_OR { $$ = "or"; }
        ;


        // Interface definition (IEEE 1800-2017 §25.3) — module-like: same
        // header shape (imports, #(params), ports) and the module item grammar
        // for the body, so an interface body is traversed exactly like a module
        // body. Modports are body items (§25.5).
interfacedef:   TK_INTERFACE interface_lifetime TK_IDENTIFIER module_imports params_block ports_block items endinterface_tail
                {
                    $7->splice($7->begin(), *$4);
                    $$ = std::make_shared<AST::Interface>(scanner.get_filename(), @1.begin.line);
                    $$->set_default_nettype(ParserHelpers::to_interface_nettype(scanner.get_default_nettype()));
                    $$->set_name($3);
                    $$->set_lifetime($2);
                    $$->set_params(ParserHelpers::to_decl_list($5));
                    $$->set_ports($6);
                    $$->set_items($7);
                }

        |       TK_INTERFACE interface_lifetime TK_IDENTIFIER module_imports ports_block items endinterface_tail
                {
                    $6->splice($6->begin(), *$4);
                    $$ = std::make_shared<AST::Interface>(scanner.get_filename(), @1.begin.line);
                    $$->set_default_nettype(ParserHelpers::to_interface_nettype(scanner.get_default_nettype()));
                    $$->set_name($3);
                    $$->set_lifetime($2);
                    $$->set_ports($5);
                    $$->set_items($6);
                }

        |       TK_INTERFACE interface_lifetime TK_IDENTIFIER module_imports params_block ports_block endinterface_tail
                {
                    $$ = std::make_shared<AST::Interface>(scanner.get_filename(), @1.begin.line);
                    $$->set_default_nettype(ParserHelpers::to_interface_nettype(scanner.get_default_nettype()));
                    $$->set_name($3);
                    $$->set_lifetime($2);
                    $$->set_params(ParserHelpers::to_decl_list($5));
                    $$->set_ports($6);
                    if (!$4->empty()) $$->set_items($4);
                }

        |       TK_INTERFACE interface_lifetime TK_IDENTIFIER module_imports ports_block endinterface_tail
                {
                    $$ = std::make_shared<AST::Interface>(scanner.get_filename(), @1.begin.line);
                    $$->set_default_nettype(ParserHelpers::to_interface_nettype(scanner.get_default_nettype()));
                    $$->set_name($3);
                    $$->set_lifetime($2);
                    $$->set_ports($5);
                    if (!$4->empty()) $$->set_items($4);
                }
        ;


// Optional interface lifetime, same model as package_lifetime: NONE
// (unspecified, i.e. static), AUTOMATIC or STATIC; stored so it round-trips.
interface_lifetime:
                %empty
                {
                    $$ = AST::Interface::LifetimeEnum::NONE;
                }

        |       TK_AUTOMATIC
                {
                    $$ = AST::Interface::LifetimeEnum::AUTOMATIC;
                }

        |       TK_STATIC
                {
                    $$ = AST::Interface::LifetimeEnum::STATIC;
                }
        ;


// The optional `endinterface : name` end label is accepted and dropped (same
// treatment as the package end label).
endinterface_tail:
                TK_ENDINTERFACE
        |       TK_ENDINTERFACE TK_COLON TK_IDENTIFIER
        ;


        // Modport declaration (IEEE 1800-2017 §25.5) — simple direction views
        // only; modport expressions (§25.5.4) and subroutine import/export
        // (§25.5/§25.7) are not parsed. `modport a (...), b (...);` declares
        // one Modport per item.
modport_decl:   TK_MODPORT modport_items TK_SEMICOLON
                {
                    $$ = $2;
                }
        ;


modport_items:  modport_item
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back(AST::to_node($1));
                }

        |       modport_items TK_COMMA modport_item
                {
                    $$ = $1;
                    $$->push_back(AST::to_node($3));
                }
        ;


modport_item:   TK_IDENTIFIER TK_LPARENTHESIS modport_port_list TK_RPARENTHESIS
                {
                    $$ = std::make_shared<AST::Modport>(scanner.get_filename(), @1.begin.line);
                    $$->set_name($1);
                    $$->set_ports($3);
                }
        ;


        // A direction distributes over the names that follow it
        // (`input a, b, output c` -> a,b INPUT; c OUTPUT).
modport_port_list:
                portdir TK_IDENTIFIER
                {
                    auto port = std::make_shared<AST::ModportPort>(scanner.get_filename(), @2.begin.line);
                    port->set_name($2);
                    port->set_direction(ParserHelpers::to_modport_dir($1));
                    $$ = std::make_shared<AST::ModportPort::List>();
                    $$->push_back(port);
                }

        |       modport_port_list TK_COMMA portdir TK_IDENTIFIER
                {
                    auto port = std::make_shared<AST::ModportPort>(scanner.get_filename(), @4.begin.line);
                    port->set_name($4);
                    port->set_direction(ParserHelpers::to_modport_dir($3));
                    $$ = $1;
                    $$->push_back(port);
                }

        |       modport_port_list TK_COMMA TK_IDENTIFIER
                {
                    auto port = std::make_shared<AST::ModportPort>(scanner.get_filename(), @3.begin.line);
                    port->set_name($3);
                    port->set_direction($1->back()->get_direction());
                    $$ = $1;
                    $$->push_back(port);
                }
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
                    // inherit the previous param's type by cloning it, override name/value
                    AST::Param::Ptr param = AST::cast_to<AST::Param>($1->back()->clone());
                    param->set_line($3->get_line());
                    param->set_filename($3->get_filename());
                    param->set_name($3->get_name());
                    param->set_value($3->get_value());
                    $$->push_back(param);
                }

        |       param_start
                {
                    $$ = std::make_shared<AST::Param::List>();
                    $$->push_back($1);
                }
        ;


param_start:    TK_PARAMETER param_assignment
                {
                    $$ = $2;
                    // No type keyword written -> ImplicitType, never a null type
                    // (ADR-0001 §5.6), to match the module-body parameter_decl path.
                    if(!$$->get_type()) {
                        $$->set_type(std::make_shared<AST::ImplicitType>(scanner.get_filename(), @1.begin.line));
                    }
                }

        |       TK_PARAMETER param_type param_assignment
                {
                    $$ = $3;
                    $$->set_type(AST::cast_to<AST::DataType>($2->clone()));
                }

        |       TK_LOCALPARAM localparam_assignment
                {
                    $$ = $2;
                    $$->set_is_local(true);
                    if(!$$->get_type()) {
                        $$->set_type(std::make_shared<AST::ImplicitType>(scanner.get_filename(), @1.begin.line));
                    }
                }

        |       TK_LOCALPARAM localparam_type localparam_assignment
                {
                    $$ = $3;
                    $$->set_is_local(true);
                    $$->set_type(AST::cast_to<AST::DataType>($2->clone()));
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

        |       port_typed_name
                {
                    $$ = $1;
                }

        |       ioport
                {
                    $$ = $1;
                }
        ;


portname:       TK_IDENTIFIER
                {
                    $$.direction = direction_t::NONE;
                    $$.net_type = net_type_t::NONE;
                    $$.signing = signing_t::NONE;
                    $$.widths = nullptr;
                    $$.name = $1;
                }

        |       TK_IDENTIFIER TK_IDENTIFIER
                {
                    // user-defined type port: `my_t name`
                    $$.direction = direction_t::NONE;
                    $$.net_type = net_type_t::NONE;
                    $$.signing = signing_t::NONE;
                    $$.widths = nullptr;
                    $$.name = $2;
                    $$.type_name = $1;
                }

        |       package_scope TK_IDENTIFIER TK_IDENTIFIER
                {
                    // package-scoped type port: `pkg::T name`
                    $$.direction = direction_t::NONE;
                    $$.net_type = net_type_t::NONE;
                    $$.signing = signing_t::NONE;
                    $$.widths = nullptr;
                    $$.name = $3;
                    $$.type_name = $2;
                    $$.type_package = $1;
                }

        |       TK_IDENTIFIER TK_DOT TK_IDENTIFIER TK_IDENTIFIER
                {
                    // modport-qualified interface port: `my_if.mp name`. The
                    // `.modport` suffix is interface-only syntax, so this form is
                    // decisive at parse time (ADR-0002) and yields an
                    // InterfaceType directly.
                    $$.direction = direction_t::NONE;
                    $$.net_type = net_type_t::NONE;
                    $$.signing = signing_t::NONE;
                    $$.widths = nullptr;
                    $$.name = $4;
                    $$.type_name = $1;
                    $$.modport = $3;
                }
        ;


// Implicit data type on a port (no type keyword): a range, optionally signed or
// unsigned — `[3:0] x`, `signed [3:0] x`, `unsigned [3:0] x` (implicit_data_type
// ::= [ signing ] { packed_dimension }). Signing is tri-state and preserved.
// Lives on its own path so it never competes with `integer_vector_type signing`
// inside port_data_type (the old type-vs-name wall).
implicit_port_type:
                widths
                {
                    $$.direction = direction_t::NONE;
                    $$.net_type = net_type_t::NONE;
                    $$.signing = signing_t::NONE;
                    $$.widths = $1;
                }

        |       TK_SIGNED widths
                {
                    $$.direction = direction_t::NONE;
                    $$.net_type = net_type_t::NONE;
                    $$.signing = signing_t::SIGNED;
                    $$.widths = $2;
                }

        |       TK_UNSIGNED widths
                {
                    $$.direction = direction_t::NONE;
                    $$.net_type = net_type_t::NONE;
                    $$.signing = signing_t::UNSIGNED;
                    $$.widths = $2;
                }
        ;


// A port carrying a type, without direction (shared by ANSI and non-ANSI):
// a built-in data_type, an implicit range, or a net_type (optionally with an
// implicit range). IEEE: [net_type] data_type_or_implicit.
port_typed_name:
                port_data_type TK_IDENTIFIER
                {
                    // bit/logic/reg/atom/struct/enum port: `int d`, `reg [3:0] q`
                    $$.direction = direction_t::NONE;
                    $$.net_type = net_type_t::NONE;
                    $$.signing = signing_t::NONE;
                    $$.widths = nullptr;
                    $$.name = $2;
                    $$.has_data_type = true;
                    $$.data_type = $1;
                }

        |       implicit_port_type TK_IDENTIFIER
                {
                    $$ = $1;
                    $$.name = $2;
                }

        |       net_type TK_IDENTIFIER
                {
                    $$.direction = direction_t::NONE;
                    $$.net_type = $1;
                    $$.signing = signing_t::NONE;
                    $$.widths = nullptr;
                    $$.name = $2;
                }

        |       net_type implicit_port_type TK_IDENTIFIER
                {
                    $$ = $2;
                    $$.net_type = $1;
                    $$.name = $3;
                }

        |       net_type integer_vector_type signing packed_dimensions TK_IDENTIFIER
                {
                    // net type + explicit integer_vector data type:
                    // `input wire logic [3:0] x` (IEEE net_port_type ::= net_type
                    // data_type). The net carries the data type keyword in its
                    // `type` child, like net_declaration. Only `logic` is valid:
                    // `reg` may not directly follow a net keyword and `bit` is
                    // 2-state (6.7.1).
                    if(const char *err = net_integer_vector_error($2)) {
                        error(@2, err);
                    }
                    $$.direction = direction_t::NONE;
                    $$.net_type = $1;
                    $$.signing = signing_t::NONE;
                    $$.widths = nullptr;
                    $$.name = $5;
                    $$.has_data_type = true;
                    $$.data_type = data_type_t{$2, $3, $4, nullptr, nullptr};
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
                    auto _ports = ParserHelpers::create_ports_decls($1, scanner.get_filename(), loc, error_message);
                    if(!_ports) error(loc, error_message);
                    $$ = std::make_shared<AST::Node::List>();
                    for(const AST::Port::Ptr &_p : *_ports) $$->push_back(_p);
                }
        ;


ioports:        ioports TK_COMMA portname
                {
                    $$ = $1;
                    $3.loc = @3;
                    $$.push_back($3);
                }

        |       ioports TK_COMMA port_typed_name
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

        |       portdir port_typed_name
                {
                    // ANSI port with a type: `input int d`, `output bit [3:0] q`,
                    // `input reg [3:0] r`, `input wire [3:0] w`, `input [3:0] x`
                    $$ = $2;
                    $$.direction = $1;
                }


// data_type usable as a port type: bit/logic/reg (integer_vector_type, owning
// their signing + packed dims), the integer atom types, the non-integer types,
// and inline aggregates. wire/tri/supply are net_type, kept separate.
port_data_type:
                integer_vector_type signing packed_dimensions
                {
                    $$ = data_type_t{$1, $2, $3, nullptr, nullptr};
                }
        |       integer_atom_type signing
                {
                    $$ = data_type_t{$1, $2, nullptr, nullptr, nullptr};
                }
        |       non_integer_type
                {
                    $$ = data_type_t{$1, signing_t::NONE, nullptr, nullptr, nullptr};
                }
        |       struct_def packed_dimensions
                {
                    $$ = data_type_t{data_type_kind_t::STRUCT, signing_t::NONE, $2, nullptr, $1};
                }
        |       union_def packed_dimensions
                {
                    $$ = data_type_t{data_type_kind_t::UNION, signing_t::NONE, $2, nullptr, $1};
                }
        |       enum_def packed_dimensions
                {
                    $$ = data_type_t{data_type_kind_t::ENUM, signing_t::NONE, $2, nullptr, $1};
                }
        ;


// True net types only (Annex A): wire/tri/supply. `logic`/`reg` are data types,
// not nets — they reach every position (items, ports, returns, members) through
// integer_vector_type inside data_type / port_data_type.
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
                    $$ = std::make_shared<AST::Dimension::List>();
                    $$->push_back($1);
                }
        ;

width:          TK_LBRACKET expression TK_COLON expression TK_RBRACKET
                {
                    $$ = std::make_shared<AST::RangeDim>($2, $4, scanner.get_filename(), @1.begin.line);
                }
        ;


lengths:        lengths length
                {
                    $$=$1;
                    $$->push_back($2);
                }

        |       length
                {
                    $$ = std::make_shared<AST::Dimension::List>();
                    $$->push_back($1);
                }
        ;


length:         TK_LBRACKET expression TK_COLON expression TK_RBRACKET
                {
                    $$ = std::make_shared<AST::RangeDim>($2, $4, scanner.get_filename(), @1.begin.line);
                }
        |       TK_LBRACKET expression TK_RBRACKET
                {
                    // single-size unpacked dimension `[N]` (IEEE unpacked_dimension
                    // ::= [ constant_expression ], ≡ [0:N-1]): the size is the msb,
                    // lsb stays null to mark the short form and round-trip as `[N]`.
                    $$ = std::make_shared<AST::SizeDim>($2, scanner.get_filename(),
                                                       @1.begin.line);
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

standard_item:  standard_item_base
                {
                    $$ = $1;
                }

        |       pragmalist standard_item_base
                {
                    $1->set_statements($2);
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back(AST::to_node($1));
                }
        ;

standard_item_base:
                ioports_decl
                {
                    $$ = $1;
                }

        |       net_decl
                {
                    $$ = $1;
                }

        |       data_declaration
                {
                    $$ = $1;
                }

        |       typed_var_decl
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

        |       defparam
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back(AST::to_node($1));
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

        |       always_ff
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back(AST::to_node($1));
                }

        |       always_comb
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back(AST::to_node($1));
                }

        |       always_latch
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

        |       typedef_decl
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back(AST::to_node($1));
                }

        |       import_decl
                {
                    $$ = $1;
                }

        |       export_decl
                {
                    $$ = $1;
                }

        |       modport_decl
                {
                    // Only meaningful inside an interface body (§25.5); the
                    // shared items grammar stays permissive and a modport in a
                    // module body is a semantic check, not a parse error.
                    $$ = $1;
                }
        ;




// typedef of any data_type (Annex A: `typedef data_type type_identifier ;`).
// Built-ins, struct/union/enum (incl. logic/reg now in data_type) come through
// `data_type`; net types (wire/tri/supply) and named/scoped types — which can't
// fold into data_type — keep dedicated forms. build_data_type_def() yields the
// bare type node so the Typedef def stays identical to before.
typedef_decl:
                TK_TYPEDEF data_type TK_IDENTIFIER TK_SEMICOLON
                {
                    $$ = std::make_shared<AST::Typedef>();
                    $$->set_type(ParserHelpers::build_data_type_def($2, scanner.get_filename(),
                                                                   @1.begin.line));
                    $$->set_name($3);
                    $$->set_filename(scanner.get_filename());
                    $$->set_line(@1.begin.line);
                }

        |       TK_TYPEDEF net_type widths TK_IDENTIFIER TK_SEMICOLON
                {
                    // typedef of a net type: the alias is the net's data type
                    // (ImplicitType carrying the packed dims); the net keyword is
                    // not a data_type and is dropped.
                    auto def = std::make_shared<AST::ImplicitType>(scanner.get_filename(), @1.begin.line);
                    def->set_packed_dims($3);
                    $$ = std::make_shared<AST::Typedef>();
                    $$->set_type(def);
                    $$->set_name($4);
                    $$->set_filename(scanner.get_filename());
                    $$->set_line(@1.begin.line);
                }

        |       TK_TYPEDEF net_type TK_IDENTIFIER TK_SEMICOLON
                {
                    auto def = std::make_shared<AST::ImplicitType>(scanner.get_filename(), @1.begin.line);
                    $$ = std::make_shared<AST::Typedef>();
                    $$->set_type(def);
                    $$->set_name($3);
                    $$->set_filename(scanner.get_filename());
                    $$->set_line(@1.begin.line);
                }

        |       TK_TYPEDEF TK_IDENTIFIER TK_IDENTIFIER TK_SEMICOLON
                {
                    // named type alias: `typedef my_t other_t;`
                    auto ref = ParserHelpers::make_named_type($2, "", scanner.get_filename(), @1.begin.line);
                    data_type_t dt{data_type_kind_t::NAMED, signing_t::NONE, nullptr, ref, nullptr};
                    $$ = std::make_shared<AST::Typedef>();
                    $$->set_type(ParserHelpers::build_data_type_def(dt, scanner.get_filename(),
                                                                   @1.begin.line));
                    $$->set_name($3);
                    $$->set_filename(scanner.get_filename());
                    $$->set_line(@1.begin.line);
                }

        |       TK_TYPEDEF TK_IDENTIFIER widths TK_IDENTIFIER TK_SEMICOLON
                {
                    // named type alias with packed dims: `typedef my_t [3:0] arr_t;`
                    auto ref = ParserHelpers::make_named_type($2, "", scanner.get_filename(), @1.begin.line);
                    data_type_t dt{data_type_kind_t::NAMED, signing_t::NONE, $3, ref, nullptr};
                    $$ = std::make_shared<AST::Typedef>();
                    $$->set_type(ParserHelpers::build_data_type_def(dt, scanner.get_filename(),
                                                                   @1.begin.line));
                    $$->set_name($4);
                    $$->set_filename(scanner.get_filename());
                    $$->set_line(@1.begin.line);
                }

        |       TK_TYPEDEF package_scope TK_IDENTIFIER TK_IDENTIFIER TK_SEMICOLON
                {
                    // package-scoped type alias: `typedef pkg::T other_t;`
                    auto ref = ParserHelpers::make_named_type($3, $2, scanner.get_filename(), @1.begin.line);
                    data_type_t dt{data_type_kind_t::NAMED, signing_t::NONE, nullptr, ref, nullptr};
                    $$ = std::make_shared<AST::Typedef>();
                    $$->set_type(ParserHelpers::build_data_type_def(dt, scanner.get_filename(),
                                                                   @1.begin.line));
                    $$->set_name($4);
                    $$->set_filename(scanner.get_filename());
                    $$->set_line(@1.begin.line);
                }

        |       TK_TYPEDEF package_scope TK_IDENTIFIER widths TK_IDENTIFIER TK_SEMICOLON
                {
                    // package-scoped alias with packed dims: `typedef pkg::T [3:0] arr_t;`
                    auto ref = ParserHelpers::make_named_type($3, $2, scanner.get_filename(), @1.begin.line);
                    data_type_t dt{data_type_kind_t::NAMED, signing_t::NONE, $4, ref, nullptr};
                    $$ = std::make_shared<AST::Typedef>();
                    $$->set_type(ParserHelpers::build_data_type_def(dt, scanner.get_filename(),
                                                                   @1.begin.line));
                    $$->set_name($5);
                    $$->set_filename(scanner.get_filename());
                    $$->set_line(@1.begin.line);
                }
        ;

enum_def:
                TK_ENUM TK_LBRACE enum_items TK_RBRACE
                {
                    $$ = std::make_shared<AST::EnumType>(scanner.get_filename(), @1.begin.line);
                    $$->set_items($3);
                }

        |       TK_ENUM TK_LOGIC widths TK_LBRACE enum_items TK_RBRACE
                {
                    $$ = std::make_shared<AST::EnumType>(scanner.get_filename(), @1.begin.line);
                    auto base = std::make_shared<AST::LogicType>(scanner.get_filename(), @1.begin.line);
                    base->set_packed_dims($3);
                    $$->set_base(base);
                    $$->set_items($5);
                }

        |       TK_ENUM TK_LOGIC TK_LBRACE enum_items TK_RBRACE
                {
                    $$ = std::make_shared<AST::EnumType>(scanner.get_filename(), @1.begin.line);
                    $$->set_base(std::make_shared<AST::LogicType>(scanner.get_filename(), @1.begin.line));
                    $$->set_items($4);
                }

        |       TK_ENUM TK_BIT widths TK_LBRACE enum_items TK_RBRACE
                {
                    $$ = std::make_shared<AST::EnumType>(scanner.get_filename(), @1.begin.line);
                    auto base = std::make_shared<AST::BitType>(scanner.get_filename(), @1.begin.line);
                    base->set_packed_dims($3);
                    $$->set_base(base);
                    $$->set_items($5);
                }

        |       TK_ENUM TK_BIT TK_LBRACE enum_items TK_RBRACE
                {
                    $$ = std::make_shared<AST::EnumType>(scanner.get_filename(), @1.begin.line);
                    $$->set_base(std::make_shared<AST::BitType>(scanner.get_filename(), @1.begin.line));
                    $$->set_items($4);
                }

        |       TK_ENUM TK_INT TK_LBRACE enum_items TK_RBRACE
                {
                    $$ = std::make_shared<AST::EnumType>(scanner.get_filename(), @1.begin.line);
                    $$->set_base(std::make_shared<AST::IntType>(scanner.get_filename(), @1.begin.line));
                    $$->set_items($4);
                }
        ;

enum_items:
                enum_items TK_COMMA enum_item
                {
                    $$ = $1;
                    $$->push_back($3);
                }

        |       enum_item
                {
                    $$ = std::make_shared<AST::EnumItem::List>();
                    $$->push_back($1);
                }
        ;

enum_item:
                TK_IDENTIFIER TK_EQUALS expression
                {
                    $$ = std::make_shared<AST::EnumItem>();
                    $$->set_name($1);
                    $$->set_value(AST::to_node($3));
                    $$->set_filename(scanner.get_filename());
                    $$->set_line(@1.begin.line);
                }

        |       TK_IDENTIFIER length
                {
                    // member-range form `enum { A[N] }` / `enum { A[N:M] }`
                    // (enum_name_declaration). `length`: `[N]`->SizeDim, `[N:M]`->RangeDim.
                    $$ = std::make_shared<AST::EnumItem>();
                    $$->set_name($1);
                    $$->set_range($2);
                    $$->set_filename(scanner.get_filename());
                    $$->set_line(@1.begin.line);
                }

        |       TK_IDENTIFIER length TK_EQUALS expression
                {
                    // `enum { A[N] = value }`
                    $$ = std::make_shared<AST::EnumItem>();
                    $$->set_name($1);
                    $$->set_range($2);
                    $$->set_value(AST::to_node($4));
                    $$->set_filename(scanner.get_filename());
                    $$->set_line(@1.begin.line);
                }

        |       TK_IDENTIFIER
                {
                    $$ = std::make_shared<AST::EnumItem>();
                    $$->set_name($1);
                    $$->set_filename(scanner.get_filename());
                    $$->set_line(@1.begin.line);
                }
        ;

struct_def:
                TK_STRUCT TK_PACKED TK_SIGNED TK_LBRACE struct_members TK_RBRACE
                {
                    $$ = std::make_shared<AST::StructType>(scanner.get_filename(), @1.begin.line);
                    $$->set_is_packed(true);
                    $$->set_signing(AST::DataType::SigningEnum::SIGNED);
                    $$->set_members($5);
                }

        |       TK_STRUCT TK_PACKED TK_LBRACE struct_members TK_RBRACE
                {
                    $$ = std::make_shared<AST::StructType>(scanner.get_filename(), @1.begin.line);
                    $$->set_is_packed(true);
                    $$->set_members($4);
                }

        |       TK_STRUCT TK_LBRACE struct_members TK_RBRACE
                {
                    $$ = std::make_shared<AST::StructType>(scanner.get_filename(), @1.begin.line);
                    $$->set_members($3);
                }
        ;

union_def:      TK_UNION TK_PACKED TK_SIGNED TK_LBRACE struct_members TK_RBRACE
                {
                    $$ = std::make_shared<AST::UnionType>(scanner.get_filename(), @1.begin.line);
                    $$->set_is_packed(true);
                    $$->set_signing(AST::DataType::SigningEnum::SIGNED);
                    $$->set_members($5);
                }

        |       TK_UNION TK_PACKED TK_LBRACE struct_members TK_RBRACE
                {
                    $$ = std::make_shared<AST::UnionType>(scanner.get_filename(), @1.begin.line);
                    $$->set_is_packed(true);
                    $$->set_members($4);
                }

        |       TK_UNION TK_LBRACE struct_members TK_RBRACE
                {
                    $$ = std::make_shared<AST::UnionType>(scanner.get_filename(), @1.begin.line);
                    $$->set_members($3);
                }
        ;

struct_members:
                struct_members struct_member
                {
                    // one struct_member declaration may yield several members
                    // (a comma list: `bit a, b;`), so splice the whole list
                    $$ = $1;
                    $$->insert($$->end(), $2->begin(), $2->end());
                }

        |       struct_member
                {
                    $$ = $1;
                }
        ;

// struct/union member: a data_type plus a member name. Built-in and inline
// aggregate types come through `data_type`; `logic` (still in net_type until
// step 4) and named/scoped types use dedicated forms whose data_type_t is built
// by hand. build_struct_member() keeps the StructMember shape unified.
struct_member:  data_type var_decl_namelist TK_SEMICOLON
                {
                    $$ = ParserHelpers::build_struct_members($1, $2, scanner.get_filename(),
                                                             @1.begin.line);
                }
        |       TK_IDENTIFIER var_decl_namelist TK_SEMICOLON
                {
                    auto ref = ParserHelpers::make_named_type($1, "", scanner.get_filename(), @1.begin.line);
                    data_type_t dt{data_type_kind_t::NAMED, signing_t::NONE, nullptr, ref, nullptr};
                    $$ = ParserHelpers::build_struct_members(dt, $2, scanner.get_filename(),
                                                             @1.begin.line);
                }
        |       TK_IDENTIFIER widths var_decl_namelist TK_SEMICOLON
                {
                    auto ref = ParserHelpers::make_named_type($1, "", scanner.get_filename(), @1.begin.line);
                    data_type_t dt{data_type_kind_t::NAMED, signing_t::NONE, $2, ref, nullptr};
                    $$ = ParserHelpers::build_struct_members(dt, $3, scanner.get_filename(),
                                                             @1.begin.line);
                }
        |       package_scope TK_IDENTIFIER var_decl_namelist TK_SEMICOLON
                {
                    auto ref = ParserHelpers::make_named_type($2, $1, scanner.get_filename(), @1.begin.line);
                    data_type_t dt{data_type_kind_t::NAMED, signing_t::NONE, nullptr, ref, nullptr};
                    $$ = ParserHelpers::build_struct_members(dt, $3, scanner.get_filename(),
                                                             @1.begin.line);
                }
        |       package_scope TK_IDENTIFIER widths var_decl_namelist TK_SEMICOLON
                {
                    auto ref = ParserHelpers::make_named_type($2, $1, scanner.get_filename(), @1.begin.line);
                    data_type_t dt{data_type_kind_t::NAMED, signing_t::NONE, $3, ref, nullptr};
                    $$ = ParserHelpers::build_struct_members(dt, $4, scanner.get_filename(),
                                                             @1.begin.line);
                }
        ;

net_decl:       net_type TK_SIGNED widths net_decl_namelist TK_SEMICOLON
                {
                    $$ = std::make_shared<AST::Node::List>();
                    for(const decl_name_t &decl_name: $4) {
                        auto type = std::make_shared<AST::ImplicitType>(scanner.get_filename(), @1.begin.line);
                        type->set_signing(AST::DataType::SigningEnum::SIGNED);
                        type->set_packed_dims(AST::Dimension::clone_list($3));
                        $$->push_back(ParserHelpers::create_net_type(decl_name, $1, type,
                                          scanner.get_filename(), @1.begin.line));
                    }
                }

        |       net_type widths net_decl_namelist TK_SEMICOLON
                {
                    $$ = std::make_shared<AST::Node::List>();
                    for(const decl_name_t &decl_name: $3) {
                        auto type = std::make_shared<AST::ImplicitType>(scanner.get_filename(), @1.begin.line);
                        type->set_packed_dims(AST::Dimension::clone_list($2));
                        $$->push_back(ParserHelpers::create_net_type(decl_name, $1, type,
                                          scanner.get_filename(), @1.begin.line));
                    }
                }

        |       net_type net_decl_namelist TK_SEMICOLON
                {
                    $$ = std::make_shared<AST::Node::List>();
                    for(const decl_name_t &decl_name: $2) {
                        $$->push_back(ParserHelpers::create_net_type(decl_name, $1, nullptr,
                                          scanner.get_filename(), @1.begin.line));
                    }
                }

        |       net_type integer_vector_type signing packed_dimensions net_decl_namelist TK_SEMICOLON
                {
                    // `wire logic [3:0] x`: explicit integer_vector net data type.
                    // Only `logic` is valid (reg may not directly follow a net
                    // keyword, bit is 2-state — IEEE 1800-2017 6.7.1).
                    if(const char *err = net_integer_vector_error($2)) {
                        error(@2, err);
                    }
                    $$ = std::make_shared<AST::Node::List>();
                    for(const decl_name_t &decl_name: $5) {
                        data_type_t dt{$2, $3, $4, nullptr, nullptr};
                        AST::DataType::Ptr type =
                            ParserHelpers::make_data_type(dt, scanner.get_filename(), @2.begin.line);
                        $$->push_back(ParserHelpers::create_net_type(decl_name, $1, type,
                                          scanner.get_filename(), @1.begin.line));
                    }
                }
        ;


// SystemVerilog data_type (Annex A), mirroring the standard's nonterminal
// hierarchy. Step 2: built-in scalar types. logic/reg/integer/real,
// struct/union/enum and named/scoped types fold into the same rule in later
// steps (logic/reg join integer_vector_type, integer joins integer_atom_type,
// real joins non_integer_type).
integer_vector_type:                          // [signing] {packed_dimension}, unsigned by default
                TK_BIT       { $$ = data_type_kind_t::BIT; }
        |       TK_LOGIC     { $$ = data_type_kind_t::LOGIC; }
        |       TK_REG       { $$ = data_type_kind_t::REG; }
        ;

integer_atom_type:                            // [signing], signed by default
                TK_BYTE      { $$ = data_type_kind_t::BYTE; }
        |       TK_SHORTINT  { $$ = data_type_kind_t::SHORTINT; }
        |       TK_INT       { $$ = data_type_kind_t::INT; }
        |       TK_LONGINT   { $$ = data_type_kind_t::LONGINT; }
        |       TK_INTEGER   { $$ = data_type_kind_t::INTEGER; }
        ;

non_integer_type:                             // no signing, no packed dimension
                TK_SHORTREAL { $$ = data_type_kind_t::SHORTREAL; }
        |       TK_REALTIME  { $$ = data_type_kind_t::REALTIME; }
        |       TK_REAL      { $$ = data_type_kind_t::REAL; }
        ;

signing:        %empty       { $$ = signing_t::NONE; }
        |       TK_SIGNED    { $$ = signing_t::SIGNED; }
        |       TK_UNSIGNED  { $$ = signing_t::UNSIGNED; }
        ;

packed_dimensions:
                %empty       { $$ = nullptr; }
        |       widths       { $$ = $1; }
        ;

data_type:      integer_vector_type signing packed_dimensions
                {
                    // vector types are unsigned by default; `signed` flips it
                    $$ = data_type_t{$1, $2, $3, nullptr, nullptr};
                }
        |       integer_atom_type signing
                {
                    // atom types are signed by default; `unsigned` flips it
                    $$ = data_type_t{$1, $2, nullptr, nullptr, nullptr};
                }
        |       non_integer_type
                {
                    $$ = data_type_t{$1, signing_t::NONE, nullptr, nullptr, nullptr};
                }
        |       struct_def packed_dimensions
                {
                    $$ = data_type_t{data_type_kind_t::STRUCT, signing_t::NONE, $2, nullptr, $1};
                }
        |       union_def packed_dimensions
                {
                    $$ = data_type_t{data_type_kind_t::UNION, signing_t::NONE, $2, nullptr, $1};
                }
        |       enum_def packed_dimensions
                {
                    $$ = data_type_t{data_type_kind_t::ENUM, signing_t::NONE, $2, nullptr, $1};
                }

        |       TK_TYPE TK_LPARENTHESIS expression TK_RPARENTHESIS
                {
                    // type(expr): the type of an expression (SV §6.23). A bare
                    // `type(my_t)` reads as an expression too -> TypeOpExpr (the
                    // named-type case is resolved later, like Call/NamedType).
                    auto t = std::make_shared<AST::TypeOpExpr>(scanner.get_filename(), @1.begin.line);
                    t->set_expr($3);
                    $$ = data_type_t{data_type_kind_t::TYPEOP, signing_t::NONE, nullptr, nullptr, t};
                }

        |       TK_TYPE TK_LPARENTHESIS data_type TK_RPARENTHESIS
                {
                    // type(data_type): a keyword-led data type (logic/int/struct/...).
                    auto t = std::make_shared<AST::TypeOpType>(scanner.get_filename(), @1.begin.line);
                    t->set_type(ParserHelpers::make_data_type($3, scanner.get_filename(), @1.begin.line));
                    $$ = data_type_t{data_type_kind_t::TYPEOP, signing_t::NONE, nullptr, nullptr, t};
                }

        |       TK_VIRTUAL virtual_iface_kw TK_IDENTIFIER virtual_iface_modport
                {
                    // virtual interface type (§25.9): `virtual` is interface-only
                    // syntax, so the InterfaceType is decisive at parse time
                    // (ADR-0002).
                    auto t = std::make_shared<AST::InterfaceType>(scanner.get_filename(), @1.begin.line);
                    t->set_name($3);
                    t->set_is_virtual(true);
                    t->set_modport($4);
                    $$ = data_type_t{data_type_kind_t::IFACE, signing_t::NONE, nullptr, nullptr, t};
                }

        |       TK_VIRTUAL virtual_iface_kw TK_IDENTIFIER parameterlist virtual_iface_modport
                {
                    // parameterized virtual interface: `virtual my_if#(8) v;`.
                    auto t = std::make_shared<AST::InterfaceType>(scanner.get_filename(), @1.begin.line);
                    t->set_name($3);
                    t->set_is_virtual(true);
                    t->set_params($4);
                    t->set_modport($5);
                    $$ = data_type_t{data_type_kind_t::IFACE, signing_t::NONE, nullptr, nullptr, t};
                }
        ;


// The optional `interface` keyword of a virtual interface type
// (`virtual [ interface ] interface_identifier`, §25.9). Accepted and dropped.
virtual_iface_kw:
                %empty
        |       TK_INTERFACE
        ;


// Optional restricting modport of a virtual interface type ("" if none).
virtual_iface_modport:
                %empty
                {
                    $$ = std::string();
                }

        |       TK_DOT TK_IDENTIFIER
                {
                    $$ = $2;
                }
        ;

// Named/scoped type declaration (`my_t x;`, `pkg::T [3:0] y;`). It cannot fold
// into `data_type`: a leading `type_identifier` would have to reduce before the
// second identifier, clashing with a module instance (`m i(...);`) — 2 s/r
// conflicts. So it stays a dedicated rule with the `type_id var_list` shape that
// defers the instance-vs-declaration choice to the token after the second id;
// build_variable(NAMED) keeps node construction unified.
typed_var_decl: TK_IDENTIFIER var_decl_namelist TK_SEMICOLON
                {
                    auto ref = ParserHelpers::make_named_type($1, "", scanner.get_filename(), @1.begin.line);
                    data_type_t dt{data_type_kind_t::NAMED, signing_t::NONE, nullptr, ref, nullptr};
                    $$ = std::make_shared<AST::Node::List>();
                    for(const decl_name_t &d: $2) {
                        $$->push_back(ParserHelpers::build_variable(dt, d, scanner.get_filename(),
                                                                    @1.begin.line));
                    }
                }
        |       TK_IDENTIFIER widths var_decl_namelist TK_SEMICOLON
                {
                    auto ref = ParserHelpers::make_named_type($1, "", scanner.get_filename(), @1.begin.line);
                    data_type_t dt{data_type_kind_t::NAMED, signing_t::NONE, $2, ref, nullptr};
                    $$ = std::make_shared<AST::Node::List>();
                    for(const decl_name_t &d: $3) {
                        $$->push_back(ParserHelpers::build_variable(dt, d, scanner.get_filename(),
                                                                    @1.begin.line));
                    }
                }
        |       package_scope TK_IDENTIFIER var_decl_namelist TK_SEMICOLON
                {
                    auto ref = ParserHelpers::make_named_type($2, $1, scanner.get_filename(), @1.begin.line);
                    data_type_t dt{data_type_kind_t::NAMED, signing_t::NONE, nullptr, ref, nullptr};
                    $$ = std::make_shared<AST::Node::List>();
                    for(const decl_name_t &d: $3) {
                        $$->push_back(ParserHelpers::build_variable(dt, d, scanner.get_filename(),
                                                                    @1.begin.line));
                    }
                }
        |       package_scope TK_IDENTIFIER widths var_decl_namelist TK_SEMICOLON
                {
                    auto ref = ParserHelpers::make_named_type($2, $1, scanner.get_filename(), @1.begin.line);
                    data_type_t dt{data_type_kind_t::NAMED, signing_t::NONE, $3, ref, nullptr};
                    $$ = std::make_shared<AST::Node::List>();
                    for(const decl_name_t &d: $4) {
                        $$->push_back(ParserHelpers::build_variable(dt, d, scanner.get_filename(),
                                                                    @1.begin.line));
                    }
                }
        ;


// SystemVerilog data_declaration: a data_type followed by a list of declared
// names, each with optional unpacked dimensions and initializer.
data_declaration:
                data_type var_decl_namelist TK_SEMICOLON
                {
                    $$ = std::make_shared<AST::Node::List>();
                    for(const decl_name_t &decl_name: $2) {
                        AST::Var::Ptr var = ParserHelpers::build_variable(
                            $1, decl_name, scanner.get_filename(), @1.begin.line);
                        $$->push_back(var);
                    }
                }

                // [const] [var] with an explicit data type: each variable is
                // wrapped in a DataModifier carrying the qualifiers.
        |       data_qualifiers data_type var_decl_namelist TK_SEMICOLON
                {
                    $$ = std::make_shared<AST::Node::List>();
                    for(const decl_name_t &decl_name: $3) {
                        if($1.is_const && !decl_name.rvalue) {
                            error(@1, "a 'const' variable shall have an initializer "
                                      "(IEEE 1800-2017 6.8)");
                        }
                        AST::Var::Ptr var = ParserHelpers::build_variable(
                            $2, decl_name, scanner.get_filename(), @1.begin.line);
                        $$->push_back(ParserHelpers::wrap_data_modifier(
                            var, $1));
                    }
                }

                // [const] [var] with an implicit type that carries signing
                // and/or packed dims (`var [3:0] a`, `var signed a`). The bare
                // implicit form (`var a`) shares the rule below to stay LALR(1).
        |       data_qualifiers implicit_signing_or_dims var_decl_namelist TK_SEMICOLON
                {
                    if(!$1.is_var) {
                        error(@1, "an implicit data type (no type keyword) requires the "
                                  "'var' keyword (IEEE 1800-2017 6.8)");
                    }
                    $$ = std::make_shared<AST::Node::List>();
                    for(const decl_name_t &decl_name: $3) {
                        if($1.is_const && !decl_name.rvalue) {
                            error(@1, "a 'const' variable shall have an initializer "
                                      "(IEEE 1800-2017 6.8)");
                        }
                        AST::Dimension::ListPtr widths =
                            $2.widths ? AST::Dimension::clone_list($2.widths) : nullptr;
                        AST::Var::Ptr var = ParserHelpers::build_implicit_type(
                            $2.signing, widths, decl_name, scanner.get_filename(),
                            @1.begin.line);
                        $$->push_back(ParserHelpers::wrap_data_modifier(
                            var, $1));
                    }
                }

                // [const] [var] then a single identifier: either a named type
                // (`var my_t x`, `var my_t [3:0] x` — the brackets are the type's
                // packed dim) or the first variable of an implicit declaration
                // (`var a`, `var a = 2`, `var a, b`, `var a [3:0]` — unpacked).
                // id_decl_tail resolves which from the token after the identifier
                // (and, for the bracketed form, after `]`). LALR(1), no conflict.
        |       data_qualifiers TK_IDENTIFIER id_decl_tail TK_SEMICOLON
                {
                    if($3.is_named) {
                        if(const_without_init($1, $3.names)) {
                            error(@1, "a 'const' variable shall have an initializer "
                                      "(IEEE 1800-2017 6.8)");
                        }
                        auto ref = ParserHelpers::make_named_type($2, "", scanner.get_filename(),
                                                                     @2.begin.line);
                        // The brackets after the type name are a packed dimension.
                        // They were captured via `lengths`, which also accepts the
                        // single-size `[N]` form, but a packed dimension admits only
                        // a range `[msb:lsb]` (or unsized `[]`), never `[N]`
                        // (IEEE 1800-2017 7.4.1, ADR-0001 §3.8).
                        if($3.dims) {
                            for(const AST::Dimension::Ptr &dim: *$3.dims) {
                                if(dim->is_node_type(AST::NodeType::SizeDim)) {
                                    error(@2, "a packed dimension shall be a range "
                                              "'[msb:lsb]', not a size '[N]' "
                                              "(IEEE 1800-2017 7.4.1)");
                                }
                            }
                        }
                        $$ = ParserHelpers::build_named_modifier(
                            ref, lengths_to_widths($3.dims), $3.names, $1,
                            scanner.get_filename(), @1.begin.line);
                    } else {
                        if(!$1.is_var) {
                            error(@1, "an implicit data type (no type keyword) requires the "
                                      "'var' keyword (IEEE 1800-2017 6.8)");
                        }
                        decl_name_t first{};
                        first.name = $2;
                        first.lengths = $3.dims; // unpacked dimension of the first variable
                        first.rvalue = $3.first_rvalue;
                        std::list<decl_name_t> all;
                        all.push_back(first);
                        all.insert(all.end(), $3.names.begin(), $3.names.end());
                        $$ = std::make_shared<AST::Node::List>();
                        for(const decl_name_t &decl_name: all) {
                            if($1.is_const && !decl_name.rvalue) {
                                error(@1, "a 'const' variable shall have an initializer "
                                          "(IEEE 1800-2017 6.8)");
                            }
                            AST::Var::Ptr var = ParserHelpers::build_implicit_type(
                                signing_t::NONE, nullptr, decl_name, scanner.get_filename(), @1.begin.line);
                            $$->push_back(ParserHelpers::wrap_data_modifier(
                                var, $1));
                        }
                    }
                }

                // [const] [var] with a scoped named type (`var pkg::T x`).
        |       data_qualifiers package_scope TK_IDENTIFIER var_decl_namelist TK_SEMICOLON
                {
                    if(const_without_init($1, $4)) {
                        error(@1, "a 'const' variable shall have an initializer "
                                  "(IEEE 1800-2017 6.8)");
                    }
                    auto ref = ParserHelpers::make_named_type($3, $2, scanner.get_filename(), @3.begin.line);
                    $$ = ParserHelpers::build_named_modifier(ref, nullptr, $4, $1,
                                                             scanner.get_filename(), @1.begin.line);
                }
        |       data_qualifiers package_scope TK_IDENTIFIER widths var_decl_namelist TK_SEMICOLON
                {
                    if(const_without_init($1, $5)) {
                        error(@1, "a 'const' variable shall have an initializer "
                                  "(IEEE 1800-2017 6.8)");
                    }
                    auto ref = ParserHelpers::make_named_type($3, $2, scanner.get_filename(), @3.begin.line);
                    $$ = ParserHelpers::build_named_modifier(ref, $4, $5, $1,
                                                             scanner.get_filename(), @1.begin.line);
                }
        ;

data_qualifiers:
                data_qualifier                  { $$ = $1; }
        |       data_qualifiers data_qualifier
                {
                    $$.is_var = $1.is_var || $2.is_var;
                    $$.is_const = $1.is_const || $2.is_const;
                    $$.lifetime = ($2.lifetime != AST::Var::LifetimeEnum::NONE)
                                      ? $2.lifetime : $1.lifetime;
                }
        ;

data_qualifier: TK_VAR
                {
                    $$ = data_qualifiers_t{true, false, AST::Var::LifetimeEnum::NONE};
                }
        |       TK_CONST
                {
                    $$ = data_qualifiers_t{false, true, AST::Var::LifetimeEnum::NONE};
                }
        |       TK_STATIC
                {
                    $$ = data_qualifiers_t{false, false, AST::Var::LifetimeEnum::STATIC};
                }
        |       TK_AUTOMATIC
                {
                    $$ = data_qualifiers_t{false, false,
                                           AST::Var::LifetimeEnum::AUTOMATIC};
                }
        ;

// data_type_or_implicit = implicit, carrying signing and/or packed dims (so it
// does NOT start with an identifier — the bare `var a` form is handled by the
// id_decl_tail rule). Non-empty: at least a signing keyword or a packed dim.
implicit_signing_or_dims:
                TK_SIGNED packed_dimensions   { $$ = implicit_type_t{signing_t::SIGNED, $2}; }
        |       TK_UNSIGNED packed_dimensions { $$ = implicit_type_t{signing_t::UNSIGNED, $2}; }
        |       widths                        { $$ = implicit_type_t{signing_t::NONE, $1}; }
        ;

// What follows `data_qualifiers TK_IDENTIFIER`: see id_decl_tail_t. A leading
// identifier (var_decl_namelist) means the consumed id was a *type* name; any
// other start means it was the *first variable* of an implicit declaration.
id_decl_tail:
                var_decl_namelist                       // `var my_t x` (named, no packed dim)
                {
                    $$ = id_decl_tail_t{};
                    $$.is_named = true;
                    $$.names = $1;
                }
        |       %empty                                  // `var a`
                {
                    $$ = id_decl_tail_t{};
                    $$.is_named = false;
                }
        |       TK_EQUALS rvalue                        // `var a = 2`
                {
                    $$ = id_decl_tail_t{};
                    $$.is_named = false;
                    $$.first_rvalue = $2;
                }
        |       TK_COMMA var_decl_namelist              // `var a, b`
                {
                    $$ = id_decl_tail_t{};
                    $$.is_named = false;
                    $$.names = $2;
                }
        |       lengths id_dims_tail                    // `var a [3:0]...` OR `var my_t [3:0] x`
                {
                    $$ = id_decl_tail_t{};
                    $$.is_named = $2.is_named;
                    $$.dims = $1;
                    $$.first_rvalue = $2.rvalue;
                    $$.names = $2.names;
                }
        ;

// What follows the bracket list in `data_qualifiers TK_IDENTIFIER lengths ...`: a
// variable name list means the identifier was a named type and the brackets were
// its *packed* dimension (`var my_t [3:0] x`); `;`/`=`/`,` mean the identifier
// was the first variable and the brackets were its *unpacked* dimension
// (`var a [3:0]`). Per-name unpacked dims are still carried by var_decl_name.
id_dims_tail:
                var_decl_namelist            { $$ = id_dims_tail_t{}; $$.is_named = true; $$.names = $1; }
        |       %empty                       { $$ = id_dims_tail_t{}; $$.is_named = false; }
        |       TK_EQUALS rvalue             { $$ = id_dims_tail_t{}; $$.is_named = false; $$.rvalue = $2; }
        |       TK_COMMA var_decl_namelist   { $$ = id_dims_tail_t{}; $$.is_named = false; $$.names = $2; }
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
                    $$.lengths = AST::Dimension::ListPtr();
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
                    $$.lengths = AST::Dimension::ListPtr();
                    $$.rdelay = $4;
                    $$.rvalue = $5;
                }

        |       delays TK_IDENTIFIER TK_EQUALS rvalue
                {
                    $$.ldelay = $1;
                    $$.name = $2;
                    $$.lengths = AST::Dimension::ListPtr();
                    $$.rdelay = AST::DelayStatement::Ptr();
                    $$.rvalue = $4;
                }

        |       TK_IDENTIFIER TK_EQUALS delays rvalue
                {
                    $$.ldelay = AST::DelayStatement::Ptr();
                    $$.name = $1;
                    $$.lengths = AST::Dimension::ListPtr();
                    $$.rdelay = $3;
                    $$.rvalue = $4;
                }

        |       TK_IDENTIFIER TK_EQUALS rvalue
                {
                    $$.ldelay = AST::DelayStatement::Ptr();
                    $$.name = $1;
                    $$.lengths = AST::Dimension::ListPtr();
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
                    $$.lengths = AST::Dimension::ListPtr();
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

        |       TK_IDENTIFIER TK_EQUALS rvalue
                {
                    $$.ldelay = AST::DelayStatement::Ptr();
                    $$.name = $1;
                    $$.lengths = AST::Dimension::ListPtr();
                    $$.rdelay = AST::DelayStatement::Ptr();
                    $$.rvalue = $3;
                }
        ;


parameter_decl: TK_PARAMETER param_assignment_list TK_SEMICOLON
                {
                    $$ = $2;
                    for(AST::Param::Ptr param: *$$) {
                        if(!param->get_type()) {
                            param->set_type(std::make_shared<AST::ImplicitType>(scanner.get_filename(), @1.begin.line));
                        }
                    }
                }

        |       TK_PARAMETER param_type param_assignment_list TK_SEMICOLON
                {
                    $$ = $3;
                    for(AST::Param::Ptr param: *$$) {
                        param->set_type(AST::cast_to<AST::DataType>($2->clone()));
                    }
                }
        ;


localparam_decl:
                TK_LOCALPARAM localparam_assignment_list TK_SEMICOLON
                {
                    $$ = $2;
                    for(AST::Param::Ptr param: *$$) {
                        param->set_is_local(true);
                        if(!param->get_type()) {
                            param->set_type(std::make_shared<AST::ImplicitType>(scanner.get_filename(), @1.begin.line));
                        }
                    }
                }

        |       TK_LOCALPARAM localparam_type localparam_assignment_list TK_SEMICOLON
                {
                    $$ = $3;
                    for(AST::Param::Ptr param: *$$) {
                        param->set_is_local(true);
                        param->set_type(AST::cast_to<AST::DataType>($2->clone()));
                    }
                }
        ;


param_type:     widths
                {
                    auto t = std::make_shared<AST::ImplicitType>(scanner.get_filename(), @1.begin.line);
                    t->set_packed_dims($1);
                    $$ = t;
                }
        |       TK_SIGNED widths
                {
                    auto t = std::make_shared<AST::ImplicitType>(scanner.get_filename(), @1.begin.line);
                    t->set_signing(AST::DataType::SigningEnum::SIGNED);
                    t->set_packed_dims($2);
                    $$ = t;
                }
        |       TK_SIGNED
                {
                    auto t = std::make_shared<AST::ImplicitType>(scanner.get_filename(), @1.begin.line);
                    t->set_signing(AST::DataType::SigningEnum::SIGNED);
                    $$ = t;
                }
        |       TK_INTEGER  { $$ = std::make_shared<AST::IntegerType>(scanner.get_filename(), @1.begin.line); }
        |       TK_REAL     { $$ = std::make_shared<AST::RealType>(scanner.get_filename(), @1.begin.line); }
        |       TK_LOGIC widths
                {
                    auto t = std::make_shared<AST::LogicType>(scanner.get_filename(), @1.begin.line);
                    t->set_packed_dims($2);
                    $$ = t;
                }
        |       TK_LOGIC    { $$ = std::make_shared<AST::LogicType>(scanner.get_filename(), @1.begin.line); }
        |       TK_INT      { $$ = std::make_shared<AST::IntType>(scanner.get_filename(), @1.begin.line); }
        |       TK_BIT widths
                {
                    auto t = std::make_shared<AST::BitType>(scanner.get_filename(), @1.begin.line);
                    t->set_packed_dims($2);
                    $$ = t;
                }
        |       TK_BIT      { $$ = std::make_shared<AST::BitType>(scanner.get_filename(), @1.begin.line); }
        |       TK_BYTE     { $$ = std::make_shared<AST::ByteType>(scanner.get_filename(), @1.begin.line); }
        |       TK_SHORTINT { $$ = std::make_shared<AST::ShortintType>(scanner.get_filename(), @1.begin.line); }
        |       TK_LONGINT  { $$ = std::make_shared<AST::LongintType>(scanner.get_filename(), @1.begin.line); }
        ;


localparam_type:widths
                {
                    auto t = std::make_shared<AST::ImplicitType>(scanner.get_filename(), @1.begin.line);
                    t->set_packed_dims($1);
                    $$ = t;
                }
        |       TK_SIGNED widths
                {
                    auto t = std::make_shared<AST::ImplicitType>(scanner.get_filename(), @1.begin.line);
                    t->set_signing(AST::DataType::SigningEnum::SIGNED);
                    t->set_packed_dims($2);
                    $$ = t;
                }
        |       TK_SIGNED
                {
                    auto t = std::make_shared<AST::ImplicitType>(scanner.get_filename(), @1.begin.line);
                    t->set_signing(AST::DataType::SigningEnum::SIGNED);
                    $$ = t;
                }
        |       TK_INTEGER  { $$ = std::make_shared<AST::IntegerType>(scanner.get_filename(), @1.begin.line); }
        |       TK_REAL     { $$ = std::make_shared<AST::RealType>(scanner.get_filename(), @1.begin.line); }
        |       TK_LOGIC widths
                {
                    auto t = std::make_shared<AST::LogicType>(scanner.get_filename(), @1.begin.line);
                    t->set_packed_dims($2);
                    $$ = t;
                }
        |       TK_LOGIC    { $$ = std::make_shared<AST::LogicType>(scanner.get_filename(), @1.begin.line); }
        |       TK_INT      { $$ = std::make_shared<AST::IntType>(scanner.get_filename(), @1.begin.line); }
        |       TK_BIT widths
                {
                    auto t = std::make_shared<AST::BitType>(scanner.get_filename(), @1.begin.line);
                    t->set_packed_dims($2);
                    $$ = t;
                }
        |       TK_BIT      { $$ = std::make_shared<AST::BitType>(scanner.get_filename(), @1.begin.line); }
        |       TK_BYTE     { $$ = std::make_shared<AST::ByteType>(scanner.get_filename(), @1.begin.line); }
        |       TK_SHORTINT { $$ = std::make_shared<AST::ShortintType>(scanner.get_filename(), @1.begin.line); }
        |       TK_LONGINT  { $$ = std::make_shared<AST::LongintType>(scanner.get_filename(), @1.begin.line); }
        ;


param_assignment_list:
                param_assignment_list TK_COMMA param_assignment
                {
                    $$ = $1;
                    $1->push_back($3);
                }
        |       param_assignment
                {
                    $$ = std::make_shared<AST::Param::List>();
                    $$->push_back($1);
                }
        ;


param_assignment:
                TK_IDENTIFIER TK_EQUALS rvalue
                {
                    $$ = std::make_shared<AST::Param>(scanner.get_filename(), @1.begin.line);
                    $$->set_name($1);
                    $$->set_value($3);
                }
        |       TK_IDENTIFIER
                {
                    $$ = std::make_shared<AST::Param>(scanner.get_filename(), @1.begin.line);
                    $$->set_name($1);
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
                    $$ = std::make_shared<AST::Param::List>();
                    $$->push_back($1);
                }
        ;


localparam_assignment:
                TK_IDENTIFIER TK_EQUALS rvalue
                {
                    $$ = std::make_shared<AST::Param>(scanner.get_filename(), @1.begin.line);
                    $$->set_name($1);
                    $$->set_value($3);
                }
        ;
        ;


defparam:       TK_DEFPARAM defparamlist TK_SEMICOLON
                {
                    $$ = std::make_shared<AST::Defparamlist>(scanner.get_filename(), @1.begin.line);
                    $$->set_list($2);
                }
        ;

defparamlist:   defparamlist TK_COMMA defparam_assignment
                {
                    $$ = $1;
                    $1->push_back($3);
                }

        |       defparam_assignment
                {
                    $$ = std::make_shared<AST::Defparam::List>();
                    $$->push_back($1);
                }
        ;

defparam_assignment:
                identifier TK_EQUALS rvalue
                {
                    $$ = std::make_shared<AST::Defparam>(scanner.get_filename(), @1.begin.line);
                    $$->set_identifier($1);
                    $$->set_right($3);
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

        |       assignment_pattern
                {
                    $$ = AST::to_node($1);
                }

        |       cast
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

        |       scoped_ref
                {
                    $$ = AST::to_node($1);
                }

        |       const_expression
                {
                    $$ = $1;
                }
        ;


// SystemVerilog package-scoped reference, e.g. pkg::WIDTH. Modelled as an
// Identifier carrying a non-empty `package`; PackageInliner resolves it.
scoped_ref:     package_scope TK_IDENTIFIER
                {
                    $$ = std::make_shared<AST::Identifier>(scanner.get_filename(), @1.begin.line);
                    $$->set_scope(ParserHelpers::pkg_scope($1, scanner.get_filename(), @1.begin.line));
                    $$->set_name($2);
                }
        ;


// Scope-resolution prefix `<scope> ::`, shared by every scoped construct so
// `$unit::` works wherever `pkg::` does. Produces the scope name (the package,
// or "$unit" for the compilation-unit scope).
package_scope:  TK_IDENTIFIER TK_COLONCOLON
                {
                    $$ = $1;
                }

        |       TK_DOLLAR_UNIT TK_COLONCOLON
                {
                    $$ = "$unit";
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


// SystemVerilog assignment pattern: '{ ... } — a struct/array literal. Items are
// positional expressions or keyed `key : value` / `default : value` entries.
assignment_pattern:
                TK_TICK_LBRACE pattern_items TK_RBRACE
                {
                    $$ = std::make_shared<AST::AssignmentPattern>(scanner.get_filename(),
                                                                  @1.begin.line);
                    $$->set_items($2);
                }

        |       TK_TICK_LBRACE expression concat TK_RBRACE
                {
                    // replicated form `'{N{a, b, ...}}`: N copies of the inner list
                    $$ = std::make_shared<AST::AssignmentPattern>(scanner.get_filename(),
                                                                  @1.begin.line);
                    $$->set_items($3->get_list());
                    $$->set_times($2);
                }
        ;


pattern_items:  pattern_items TK_COMMA pattern_item
                {
                    $$ = $1;
                    $$->push_back($3);
                }

        |       pattern_item
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back($1);
                }
        ;


pattern_item:   expression
                {
                    $$ = $1;
                }

        |       expression TK_COLON expression
                {
                    // keyed entry `key : value` (member name, index, or type)
                    auto pi = std::make_shared<AST::PatternItem>(scanner.get_filename(),
                                                                 @1.begin.line);
                    pi->set_is_default(false);
                    pi->set_key($1);
                    pi->set_value($3);
                    $$ = AST::to_node(pi);
                }

        |       TK_DEFAULT TK_COLON expression
                {
                    // `default : value`
                    auto pi = std::make_shared<AST::PatternItem>(scanner.get_filename(),
                                                                 @1.begin.line);
                    pi->set_is_default(true);
                    pi->set_value($3);
                    $$ = AST::to_node(pi);
                }
        ;


// SystemVerilog cast: type'(expr). The casting type is a built-in keyword, a
// signing keyword, a named type, or a size (constant expression).
cast:           casting_type TK_TICK_LPAREN expression TK_RPARENTHESIS
                {
                    $$ = $1;
                    $$->set_expr($3);
                }
        ;


casting_type:   integer_vector_type
                {
                    data_type_t dt{$1, signing_t::NONE, nullptr, nullptr, nullptr};
                    auto c = std::make_shared<AST::TypeCast>(scanner.get_filename(), @1.begin.line);
                    c->set_target(ParserHelpers::make_data_type(dt, scanner.get_filename(), @1.begin.line));
                    $$ = c;
                }

        |       integer_atom_type
                {
                    data_type_t dt{$1, signing_t::NONE, nullptr, nullptr, nullptr};
                    auto c = std::make_shared<AST::TypeCast>(scanner.get_filename(), @1.begin.line);
                    c->set_target(ParserHelpers::make_data_type(dt, scanner.get_filename(), @1.begin.line));
                    $$ = c;
                }

        |       non_integer_type
                {
                    data_type_t dt{$1, signing_t::NONE, nullptr, nullptr, nullptr};
                    auto c = std::make_shared<AST::TypeCast>(scanner.get_filename(), @1.begin.line);
                    c->set_target(ParserHelpers::make_data_type(dt, scanner.get_filename(), @1.begin.line));
                    $$ = c;
                }

        |       TK_SIGNED
                {
                    auto c = std::make_shared<AST::SigningCast>(scanner.get_filename(), @1.begin.line);
                    c->set_signing(AST::SigningCast::SigningEnum::SIGNED);
                    $$ = c;
                }

        |       TK_UNSIGNED
                {
                    auto c = std::make_shared<AST::SigningCast>(scanner.get_filename(), @1.begin.line);
                    c->set_signing(AST::SigningCast::SigningEnum::UNSIGNED);
                    $$ = c;
                }

        |       TK_IDENTIFIER
                {
                    // named type cast: `my_t'(x)`
                    auto c = std::make_shared<AST::TypeCast>(scanner.get_filename(), @1.begin.line);
                    c->set_target(ParserHelpers::make_named_type($1, "", scanner.get_filename(), @1.begin.line));
                    $$ = c;
                }

        |       intnumber
                {
                    // size cast: `8'(x)` — a constant size before the tick.
                    // (`WIDTH'(x)` with an identifier is indistinguishable from a
                    // named-type cast and stays a TypeCast, resolved later.)
                    auto c = std::make_shared<AST::SizeCast>(scanner.get_filename(), @1.begin.line);
                    c->set_size($1);
                    $$ = c;
                }
        ;


partselect:     partselect TK_LBRACKET expression TK_COLON expression TK_RBRACKET
                {
                    $$ = AST::to_node(std::make_shared<AST::Partselect>($3, $5, $1,
                                                                        scanner.get_filename(), @1.begin.line));
                }

        |       partselect TK_LBRACKET expression TK_PLUSCOLON expression TK_RBRACKET
                {
                    $$ = AST::to_node(std::make_shared<AST::PartselectPlusIndexed>($3, $5, $1,
                                                                                   scanner.get_filename(),
                                                                                   @1.begin.line));
                }

        |       partselect TK_LBRACKET expression TK_MINUSCOLON expression TK_RBRACKET
                {
                    $$ = AST::to_node(std::make_shared<AST::PartselectMinusIndexed>($3, $5, $1,
                                                                                    scanner.get_filename(),
                                                                                    @1.begin.line));
                }

        |       pointer TK_LBRACKET expression TK_COLON expression TK_RBRACKET
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


pointer:        partselect TK_LBRACKET expression TK_RBRACKET
                {
                    $$ = AST::to_node(std::make_shared<AST::Pointer>($3, $1, scanner.get_filename(), @1.begin.line));
                }

        |       pointer TK_LBRACKET expression TK_RBRACKET
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

always_ff:      TK_ALWAYS_FF basic_statement
                {
                    $$ = std::make_shared<AST::AlwaysFF>(scanner.get_filename(), @1.begin.line);
                    if ($2->get_node_type() == AST::NodeType::EventStatement) {
                        $$->set_senslist(AST::cast_to<AST::EventStatement>($2)->get_senslist());
                        $$->set_statement(AST::cast_to<AST::EventStatement>($2)->get_statement());
                    }
                    else {
                        $$->set_statement($2);
                    }
                }
        ;

always_comb:    TK_ALWAYS_COMB basic_statement
                {
                    $$ = std::make_shared<AST::AlwaysComb>(scanner.get_filename(), @1.begin.line);
                    AST::Sens::Ptr s = std::make_shared<AST::Sens>(scanner.get_filename(), @1.begin.line);
                    AST::Sens::ListPtr l = std::make_shared<AST::Sens::List>();
                    s->set_type(AST::Sens::TypeEnum::ALL);
                    l->push_back(s);
                    AST::Senslist::Ptr sl = std::make_shared<AST::Senslist>(scanner.get_filename(), @1.begin.line);
                    sl->set_list(l);
                    $$->set_senslist(sl);
                    $$->set_statement($2);
                }
        ;

always_latch:   TK_ALWAYS_LATCH basic_statement
                {
                    $$ = std::make_shared<AST::AlwaysLatch>(scanner.get_filename(), @1.begin.line);
                    AST::Sens::Ptr s = std::make_shared<AST::Sens>(scanner.get_filename(), @1.begin.line);
                    AST::Sens::ListPtr l = std::make_shared<AST::Sens::List>();
                    s->set_type(AST::Sens::TypeEnum::ALL);
                    l->push_back(s);
                    AST::Senslist::Ptr sl = std::make_shared<AST::Senslist>(scanner.get_filename(), @1.begin.line);
                    sl->set_list(l);
                    $$->set_senslist(sl);
                    $$->set_statement($2);
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

        |       TK_AT TK_LPARENTHESIS TK_RATTR
                {
                    $$ = std::make_shared<AST::Senslist>(scanner.get_filename(), @1.begin.line);
                    AST::Sens::Ptr s = std::make_shared<AST::Sens>(scanner.get_filename(), @1.begin.line);
                    AST::Sens::ListPtr l = std::make_shared<AST::Sens::List>();
                    s->set_type(AST::Sens::TypeEnum::ALL);
                    l->push_back(s);
                    $$->set_list(l);
                }

        |       TK_AT TK_LATTR TK_RPARENTHESIS
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

basic_statement:
                basic_statement_base
                {
                    $$ = $1;
                }

        |       pragmalist basic_statement_base
                {
                    auto stmts = std::make_shared<AST::Node::List>();
                    stmts->push_back($2);
                    $1->set_statements(stmts);
                    $$ = AST::to_node($1);
                }
        ;

basic_statement_base:
                if_statement
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

        |       unique_case_statement
                {
                    $$ = AST::to_node($1);
                }

        |       priority_case_statement
                {
                    $$ = AST::to_node($1);
                }

        |       repeat_statement
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
                delays lvalue TK_EQUALS delays rvalue TK_SEMICOLON
                {
                    $$ = std::make_shared<AST::BlockingSubstitution>(scanner.get_filename(), @1.begin.line);
                    $$->set_left($2);
                    $$->set_right($5);
                    $$->set_ldelay($1);
                    $$->set_rdelay($4);
                }

        |       delays lvalue TK_EQUALS rvalue TK_SEMICOLON
                {
                    $$ = std::make_shared<AST::BlockingSubstitution>(scanner.get_filename(), @1.begin.line);
                    $$->set_left($2);
                    $$->set_right($4);
                    $$->set_ldelay($1);
                }

        |       lvalue TK_EQUALS delays rvalue TK_SEMICOLON
                {
                    $$ = std::make_shared<AST::BlockingSubstitution>(scanner.get_filename(), @1.begin.line);
                    $$->set_left($1);
                    $$->set_right($4);
                    $$->set_rdelay($3);
                }

        |       lvalue TK_EQUALS rvalue TK_SEMICOLON
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

        |       data_declaration
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

repeat_statement:
                TK_REPEAT TK_LPARENTHESIS expression TK_RPARENTHESIS basic_statement
                {
                   $$ = std::make_shared<AST::RepeatStatement>(scanner.get_filename(), @1.begin.line);
                   $$->set_times($3);
                   $$->set_statement($5);
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

        |       TK_SEMICOLON
                {
                    $$ = nullptr;
                }
        ;


false_statement:
                ifcontent_statement
                {
                    $$ = $1;
                }

        |       TK_SEMICOLON
                {
                    $$ = nullptr;
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

        |       TK_GENVAR TK_IDENTIFIER TK_EQUALS rvalue TK_SEMICOLON
                {
                    $$ = std::make_shared<AST::BlockingSubstitution>(scanner.get_filename(), @1.begin.line);
                    AST::Identifier::Ptr id = std::make_shared<AST::Identifier>(scanner.get_filename(), @2.begin.line);
                    id->set_name($2);
                    AST::Lvalue::Ptr lv = std::make_shared<AST::Lvalue>(scanner.get_filename(), @2.begin.line);
                    lv->set_var(AST::to_node(id));
                    $$->set_left(lv);
                    $$->set_right($4);
                }

        |       TK_INT TK_IDENTIFIER TK_EQUALS rvalue TK_SEMICOLON
                {
                    $$ = std::make_shared<AST::BlockingSubstitution>(scanner.get_filename(), @1.begin.line);
                    AST::Identifier::Ptr id = std::make_shared<AST::Identifier>(scanner.get_filename(), @2.begin.line);
                    id->set_name($2);
                    AST::Lvalue::Ptr lv = std::make_shared<AST::Lvalue>(scanner.get_filename(), @2.begin.line);
                    lv->set_var(AST::to_node(id));
                    $$->set_left(lv);
                    $$->set_right($4);
                }


        |       TK_INTEGER TK_IDENTIFIER TK_EQUALS rvalue TK_SEMICOLON
                {
                    $$ = std::make_shared<AST::BlockingSubstitution>(scanner.get_filename(), @1.begin.line);
                    AST::Identifier::Ptr id = std::make_shared<AST::Identifier>(scanner.get_filename(), @2.begin.line);
                    id->set_name($2);
                    AST::Lvalue::Ptr lv = std::make_shared<AST::Lvalue>(scanner.get_filename(), @2.begin.line);
                    lv->set_var(AST::to_node(id));
                    $$->set_left(lv);
                    $$->set_right($4);
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


forpost:        lvalue TK_EQUALS rvalue
                {
                    $$ = std::make_shared<AST::BlockingSubstitution>(scanner.get_filename(), @1.begin.line);
                    $$->set_left($1);
                    $$->set_right($3);
                }

        |       lvalue TK_PLUS TK_PLUS
                {
                    $$ = std::make_shared<AST::BlockingSubstitution>(scanner.get_filename(), @1.begin.line);
                    AST::Rvalue::Ptr rv = std::make_shared<AST::Rvalue>(scanner.get_filename(), @1.begin.line);
                    AST::Plus::Ptr plus = std::make_shared<AST::Plus>(scanner.get_filename(), @1.begin.line);
                    AST::IntConstN::Ptr one = std::make_shared<AST::IntConstN>(scanner.get_filename(), @1.begin.line);
                    one->set_base(10); one->set_size(-1); one->set_sign(false); one->set_value(1);
                    plus->set_left($1->get_var()->clone());
                    plus->set_right(AST::to_node(one));
                    rv->set_var(AST::to_node(plus));
                    $$->set_left($1);
                    $$->set_right(rv);
                }

        |       lvalue TK_MINUS TK_MINUS
                {
                    $$ = std::make_shared<AST::BlockingSubstitution>(scanner.get_filename(), @1.begin.line);
                    AST::Rvalue::Ptr rv = std::make_shared<AST::Rvalue>(scanner.get_filename(), @1.begin.line);
                    AST::Minus::Ptr minus = std::make_shared<AST::Minus>(scanner.get_filename(), @1.begin.line);
                    AST::IntConstN::Ptr one = std::make_shared<AST::IntConstN>(scanner.get_filename(), @1.begin.line);
                    one->set_base(10); one->set_size(-1); one->set_sign(false); one->set_value(1);
                    minus->set_left($1->get_var()->clone());
                    minus->set_right(AST::to_node(one));
                    rv->set_var(AST::to_node(minus));
                    $$->set_left($1);
                    $$->set_right(rv);
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

unique_case_statement:
                TK_UNIQUE TK_CASE TK_LPARENTHESIS case_comp TK_RPARENTHESIS casecontent_statements TK_ENDCASE
                {
                    $$ = std::make_shared<AST::UniqueCaseStatement>($4, $6, scanner.get_filename(), @1.begin.line);
                }
        ;

priority_case_statement:
                TK_PRIORITY TK_CASE TK_LPARENTHESIS case_comp TK_RPARENTHESIS casecontent_statements TK_ENDCASE
                {
                    $$ = std::make_shared<AST::PriorityCaseStatement>($4, $6, scanner.get_filename(), @1.begin.line);
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


instance_body:  TK_IDENTIFIER TK_LPARENTHESIS TK_RPARENTHESIS
                {
                    $$ = std::make_shared<AST::Instance>(scanner.get_filename(), @1.begin.line);
                    $$->set_name($1);
                }

        |       TK_IDENTIFIER TK_LPARENTHESIS instance_ports TK_RPARENTHESIS
                {
                    $$ = std::make_shared<AST::Instance>(scanner.get_filename(), @1.begin.line);
                    $$->set_portlist($3);
                    $$->set_name($1);
                }

        |       TK_IDENTIFIER length TK_LPARENTHESIS TK_RPARENTHESIS
                {
                    $$ = std::make_shared<AST::Instance>(scanner.get_filename(), @1.begin.line);
                    $$->set_array($2);
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
                TK_LPARENTHESIS TK_RPARENTHESIS
                {
                    $$ = std::make_shared<AST::Instance>(scanner.get_filename(), @1.begin.line);
                }

        |       TK_LPARENTHESIS instance_ports TK_RPARENTHESIS
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

        |       param_args_noname TK_COMMA
                {
                    $$ = $1;
                    $$->push_back(std::make_shared<AST::ParamArg>());
                }

        |       TK_COMMA param_arg_noname
                {
                    $$ = std::make_shared<AST::ParamArg::List>();
                    $$->push_back(std::make_shared<AST::ParamArg>());
                    $$->push_back($2);
                }

        |       TK_COMMA
                {
                    $$ = std::make_shared<AST::ParamArg::List>();
                    $$->push_back(std::make_shared<AST::ParamArg>());
                    $$->push_back(std::make_shared<AST::ParamArg>());
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


param_arg:      TK_DOT TK_IDENTIFIER TK_LPARENTHESIS expression TK_RPARENTHESIS
                {
                    $$ = std::make_shared<AST::ParamArg>(scanner.get_filename(), @1.begin.line);
                    $$->set_value($4);
                    $$->set_name($2);
                }

        |       TK_DOT TK_IDENTIFIER TK_LPARENTHESIS TK_RPARENTHESIS
                {
                    $$ = std::make_shared<AST::ParamArg>(scanner.get_filename(), @1.begin.line);
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

        |       instance_ports_list TK_COMMA
                {
                    $$ = $1;
                    $$->push_back(std::make_shared<AST::PortArg>());
                }

        |       TK_COMMA instance_port_list
                {
                    $$ = std::make_shared<AST::PortArg::List>();
                    $$->push_back(std::make_shared<AST::PortArg>());
                    $$->push_back($2);
                }

        |       TK_COMMA
                {
                    $$ = std::make_shared<AST::PortArg::List>();
                    $$->push_back(std::make_shared<AST::PortArg>());
                    $$->push_back(std::make_shared<AST::PortArg>());
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
                TK_DOT TK_IDENTIFIER TK_LPARENTHESIS expression TK_RPARENTHESIS
                {
                    $$ = std::make_shared<AST::PortArg>(scanner.get_filename(), @1.begin.line);
                    $$->set_value($4);
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

        |       generate_block
                {
                    $$ = std::make_shared<AST::Node::List>();
                    $$->push_back(AST::to_node($1));
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

        |       TK_BEGIN TK_END
                {
                    $$ = std::make_shared<AST::Block>(scanner.get_filename(), @1.begin.line);
                }

        |       TK_BEGIN TK_COLON TK_IDENTIFIER TK_END
                {
                    $$ = std::make_shared<AST::Block>(scanner.get_filename(), @1.begin.line);
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
                    if ($1->size() != 1) {
                        error(@1, "a begin end block is missing");
                    }
                    const auto &elt = $1->front();
                    if (elt->is_node_type(AST::NodeType::Block)) {
                        $$ = AST::cast_to<AST::Block>(elt);
                    }
                    else {
                        $$ = std::make_shared<AST::Block>(scanner.get_filename(), @1.begin.line);
                        $$->set_statements($1);
                    }
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

                // $unsigned system task (`unsigned` is now a keyword, like signed)
        |       TK_DOLLAR TK_UNSIGNED
                {
                    $$ = std::make_shared<AST::SystemCall>(scanner.get_filename(), @1.begin.line);
                    $$->set_syscall("unsigned");
                }

        |       TK_DOLLAR TK_UNSIGNED TK_LPARENTHESIS TK_RPARENTHESIS
                {
                    $$ = std::make_shared<AST::SystemCall>(scanner.get_filename(), @1.begin.line);
                    $$->set_syscall("unsigned");
                }

        |       TK_DOLLAR TK_UNSIGNED TK_LPARENTHESIS sysargs TK_RPARENTHESIS
                {
                    $$ = std::make_shared<AST::SystemCall>($4, "unsigned", scanner.get_filename(), @1.begin.line);
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


function:       TK_FUNCTION automatic function_rettype_name function_ports_block function_statements TK_ENDFUNCTION
                {
                    // function_rettype_name already carries the return type AND name
                    // (factored so a user-type return is LR(1) without a type table).
                    $$ = $3;
                    $$->set_filename(scanner.get_filename());
                    $$->set_line(@1.begin.line);
                    $$->set_lifetime($2 ? AST::Function::LifetimeEnum::AUTOMATIC
                                        : AST::Function::LifetimeEnum::NONE);
                    $$->set_args($4);
                    $$->set_statements($5);
                }
        ;


        // Return type and function name are factored into one rule: we shift the
        // first identifier, then decide on the next token (another ID => the first
        // was a user-defined return type; `(`/`;` => it was the name). Pure LR(1),
        // no type table.
function_rettype_name:
                TK_IDENTIFIER
                {
                    // no return type, $1 is the function name (implicit 1-bit return)
                    $$ = std::make_shared<AST::Function>();
                    $$->set_return_type(std::make_shared<AST::ImplicitType>(scanner.get_filename(), @1.begin.line));
                    $$->set_name($1);
                }

        |       TK_VOID TK_IDENTIFIER
                {
                    // `function void f(...)` — no return value (SV)
                    $$ = std::make_shared<AST::Function>();
                    $$->set_return_type(std::make_shared<AST::VoidType>(scanner.get_filename(), @1.begin.line));
                    $$->set_name($2);
                }

        |       TK_IDENTIFIER TK_IDENTIFIER
                {
                    // user-defined return type $1, name $2
                    $$ = std::make_shared<AST::Function>();
                    $$->set_return_type(ParserHelpers::make_named_type($1, "", scanner.get_filename(), @1.begin.line));
                    $$->set_name($2);
                }

        |       package_scope TK_IDENTIFIER TK_IDENTIFIER
                {
                    // package-scoped return type $1::$2, name $3
                    $$ = std::make_shared<AST::Function>();
                    $$->set_return_type(ParserHelpers::make_named_type($2, $1, scanner.get_filename(), @1.begin.line));
                    $$->set_name($3);
                }

        |       widths TK_IDENTIFIER
                {
                    $$ = std::make_shared<AST::Function>();
                    {
                        auto rt = std::make_shared<AST::ImplicitType>(scanner.get_filename(), @1.begin.line);
                        rt->set_packed_dims($1);
                        $$->set_return_type(rt);
                    }
                    $$->set_name($2);
                }

        |       TK_SIGNED widths TK_IDENTIFIER
                {
                    $$ = std::make_shared<AST::Function>();
                    {
                        auto rt = std::make_shared<AST::ImplicitType>(scanner.get_filename(), @1.begin.line);
                        rt->set_signing(AST::DataType::SigningEnum::SIGNED);
                        rt->set_packed_dims($2);
                        $$->set_return_type(rt);
                    }
                    $$->set_name($3);
                }

        |       data_type TK_IDENTIFIER
                {
                    // explicit data_type return (integer/real/int/bit/logic/reg/
                    // byte/.../struct/union/enum). rettype_ref holds the type as
                    // its own node: the dedicated built-in scalar node (carrying
                    // signing and packed dims), the inline struct/union/enum def,
                    // or the named-type reference — the same node a declaration
                    // would use. Function variable analysis (get_variable_nodes)
                    // skips rettype_ref, so a built-in scalar Variable node here
                    // is not mistaken for a declared variable.
                    $$ = std::make_shared<AST::Function>();
                    $$->set_return_type(ParserHelpers::build_data_type_def($1, scanner.get_filename(), @1.begin.line));
                    $$->set_name($2);
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
                    $$ = ParserHelpers::create_args_decls($1, scanner.get_filename());
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

        |       port_data_type TK_IDENTIFIER
                {
                    // built-in data_type function/task port: `input int a`,
                    // `input bit [3:0] b`, `input reg/logic [3:0] r`, `input real r`
                    $$.direction = direction_t::NONE;
                    $$.net_type = net_type_t::NONE;
                    $$.signing = signing_t::NONE;
                    $$.widths = nullptr;
                    $$.name = $2;
                    $$.has_data_type = true;
                    $$.data_type = $1;
                }

        |       implicit_port_type TK_IDENTIFIER
                {
                    // implicit range port: `input [3:0] x`, `input signed [3:0] x`
                    $$ = $1;
                    $$.name = $2;
                }
        ;


function_statements:
                %empty
                {
                    $$ = std::make_shared<AST::Node::List>();
                }

        |       function_statements function_statement
                {
                    $$ = $1;
                    $$->splice($$->end(), *$2);
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

        |       data_declaration
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
                    auto _ports = ParserHelpers::create_ports_decls($2, scanner.get_filename(), loc, error_message);
                    if(!_ports) error(loc, error_message);
                    $$ = std::make_shared<AST::Node::List>();
                    for(const AST::Port::Ptr &_p : *_ports) $$->push_back(_p);
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
                    $$ = std::make_shared<AST::FunctionCall>(scanner.get_filename(), @1.begin.line);
                    $$->set_name($1);
                    $$->set_args($3);
                }

        |       TK_IDENTIFIER TK_LPARENTHESIS TK_RPARENTHESIS
                {
                    $$ = std::make_shared<AST::FunctionCall>(scanner.get_filename(), @1.begin.line);
                    $$->set_name($1);
                }

        |       package_scope TK_IDENTIFIER TK_LPARENTHESIS function_args TK_RPARENTHESIS
                {
                    $$ = std::make_shared<AST::FunctionCall>(scanner.get_filename(), @1.begin.line);
                    $$->set_name($2);
                    $$->set_scope(ParserHelpers::pkg_scope($1, scanner.get_filename(), @1.begin.line));
                    $$->set_args($4);
                }

        |       package_scope TK_IDENTIFIER TK_LPARENTHESIS TK_RPARENTHESIS
                {
                    $$ = std::make_shared<AST::FunctionCall>(scanner.get_filename(), @1.begin.line);
                    $$->set_name($2);
                    $$->set_scope(ParserHelpers::pkg_scope($1, scanner.get_filename(), @1.begin.line));
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
                    $$->set_lifetime($2 ? AST::Task::LifetimeEnum::AUTOMATIC
                                        : AST::Task::LifetimeEnum::NONE);
                    $$->set_args($4);
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
                    $$ = ParserHelpers::create_args_decls($1, scanner.get_filename());
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

        |       port_data_type TK_IDENTIFIER
                {
                    // built-in data_type function/task port: `input int a`,
                    // `input bit [3:0] b`, `input reg/logic [3:0] r`, `input real r`
                    $$.direction = direction_t::NONE;
                    $$.net_type = net_type_t::NONE;
                    $$.signing = signing_t::NONE;
                    $$.widths = nullptr;
                    $$.name = $2;
                    $$.has_data_type = true;
                    $$.data_type = $1;
                }

        |       implicit_port_type TK_IDENTIFIER
                {
                    // implicit range port: `input [3:0] x`, `input signed [3:0] x`
                    $$ = $1;
                    $$.name = $2;
                }
        ;


task_statements:
                %empty
                {
                    $$ = std::make_shared<AST::Node::List>();
                }

        |       task_statements task_statement
                {
                    $$ = $1;
                    $$->splice($$->end(), *$2);
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

        |       data_declaration
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
                    auto _ports = ParserHelpers::create_ports_decls($2, scanner.get_filename(), loc, error_message);
                    if(!_ports) error(loc, error_message);
                    $$ = std::make_shared<AST::Node::List>();
                    for(const AST::Port::Ptr &_p : *_ports) $$->push_back(_p);
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
                    $$ = std::make_shared<AST::TaskCall>(scanner.get_filename(), @1.begin.line);
                    $$->set_name($1);
                    $$->set_args($3);
                }

        |       TK_IDENTIFIER TK_LPARENTHESIS TK_RPARENTHESIS
                {
                    $$ = std::make_shared<AST::TaskCall>(scanner.get_filename(), @1.begin.line);
                    $$->set_name($1);
                }

        |       TK_IDENTIFIER
                {
                    $$ = std::make_shared<AST::TaskCall>(scanner.get_filename(), @1.begin.line);
                    $$->set_name($1);
                }

        |       package_scope TK_IDENTIFIER TK_LPARENTHESIS task_args TK_RPARENTHESIS
                {
                    $$ = std::make_shared<AST::TaskCall>(scanner.get_filename(), @1.begin.line);
                    $$->set_name($2);
                    $$->set_scope(ParserHelpers::pkg_scope($1, scanner.get_filename(), @1.begin.line));
                    $$->set_args($4);
                }

        |       package_scope TK_IDENTIFIER TK_LPARENTHESIS TK_RPARENTHESIS
                {
                    $$ = std::make_shared<AST::TaskCall>(scanner.get_filename(), @1.begin.line);
                    $$->set_name($2);
                    $$->set_scope(ParserHelpers::pkg_scope($1, scanner.get_filename(), @1.begin.line));
                }

        |       package_scope TK_IDENTIFIER
                {
                    $$ = std::make_shared<AST::TaskCall>(scanner.get_filename(), @1.begin.line);
                    $$->set_name($2);
                    $$->set_scope(ParserHelpers::pkg_scope($1, scanner.get_filename(), @1.begin.line));
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
                    $$->set_hier($1);
                }
        ;


scope:          identifier TK_DOT
                {
                    $$ = $1->get_hier();
                    if (!$$) {
                        $$ = std::make_shared<AST::HierName>(scanner.get_filename(), @1.begin.line);
                        $$->set_labellist(std::make_shared<AST::HierLabel::List>());
                    }

                    AST::HierLabel::Ptr lbl =
                        std::make_shared<AST::HierLabel>(scanner.get_filename(), @1.begin.line);
                    lbl->set_name($1->get_name());
                    $$->get_labellist()->push_back(lbl);
                }

        |       pointer TK_DOT
                {
                    AST::Pointer::Ptr ptr = AST::cast_to<AST::Pointer>($1);
                    AST::Identifier::Ptr var = AST::cast_to<AST::Identifier>(ptr->get_var());
                    $$ = var->get_hier();
                    if(!$$) {
                        $$ = std::make_shared<AST::HierName>(scanner.get_filename(), @1.begin.line);
                        $$->set_labellist(std::make_shared<AST::HierLabel::List>());
                    }

                    AST::HierLabel::Ptr lbl =
                        std::make_shared<AST::HierLabel>(scanner.get_filename(), @1.begin.line);
                    lbl->set_name(var->get_name());
                    lbl->set_loop(ptr->get_ptr());
                    $$->get_labellist()->push_back(lbl);
                }
        ;


disable:        TK_DISABLE TK_IDENTIFIER
                {
                    $$ = std::make_shared<AST::Disable>(scanner.get_filename(), @1.begin.line);
                    auto dest = std::make_shared<AST::Identifier>(scanner.get_filename(), @2.begin.line);
                    dest->set_name($2);
                    $$->set_dest(dest);
                }
        ;


// Jump statements (IEEE 1800-2017 §12.8). The trailing `;` is added by the
// single_statement wrapper, like the other statement forms.
jump_statement: TK_RETURN expression
                {
                    auto r = std::make_shared<AST::Return>(scanner.get_filename(), @1.begin.line);
                    r->set_value($2);
                    $$ = AST::to_node(r);
                }

        |       TK_RETURN
                {
                    $$ = AST::to_node(std::make_shared<AST::Return>(scanner.get_filename(),
                                                                    @1.begin.line));
                }

        |       TK_BREAK
                {
                    $$ = AST::to_node(std::make_shared<AST::Break>(scanner.get_filename(),
                                                                   @1.begin.line));
                }

        |       TK_CONTINUE
                {
                    $$ = AST::to_node(std::make_shared<AST::Continue>(scanner.get_filename(),
                                                                      @1.begin.line));
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

        |       jump_statement TK_SEMICOLON
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
            exit(1);
        }

        namespace ParserHelpers {

            AST::DataType::SigningEnum to_signing(signing_t s) {
                switch(s) {
                case signing_t::SIGNED:   return AST::DataType::SigningEnum::SIGNED;
                case signing_t::UNSIGNED: return AST::DataType::SigningEnum::UNSIGNED;
                default:                  return AST::DataType::SigningEnum::NONE;
                }
            }

            static AST::Port::DirectionEnum to_port_dir(direction_t d) {
                switch(d) {
                case direction_t::INPUT:  return AST::Port::DirectionEnum::INPUT;
                case direction_t::OUTPUT: return AST::Port::DirectionEnum::OUTPUT;
                case direction_t::INOUT:  return AST::Port::DirectionEnum::INOUT;
                default:                  return AST::Port::DirectionEnum::NONE;
                }
            }

            static AST::Arg::DirectionEnum to_arg_dir(direction_t d) {
                switch(d) {
                case direction_t::OUTPUT: return AST::Arg::DirectionEnum::OUTPUT;
                case direction_t::INOUT:  return AST::Arg::DirectionEnum::INOUT;
                default:                  return AST::Arg::DirectionEnum::INPUT;
                }
            }

            // Value-side SV `::` scope (single package segment) for a name ref.
            AST::ScopeName::ListPtr pkg_scope(const std::string &package,
                                              const std::string &filename, uint32_t line) {
                if(package.empty()) {
                    return nullptr;
                }
                auto scope = std::make_shared<AST::ScopeName::List>();
                auto seg = std::make_shared<AST::ScopeName>(filename, line);
                seg->set_name(package);
                scope->push_back(seg);
                return scope;
            }

            AST::Declaration::ListPtr to_decl_list(const AST::Param::ListPtr &in) {
                if(!in) return nullptr;
                auto out = std::make_shared<AST::Declaration::List>();
                for(const AST::Param::Ptr &p : *in) out->push_back(p);
                return out;
            }

            AST::ModportPort::DirectionEnum to_modport_dir(direction_t d) {
                switch(d) {
                case direction_t::OUTPUT: return AST::ModportPort::DirectionEnum::OUTPUT;
                case direction_t::INOUT:  return AST::ModportPort::DirectionEnum::INOUT;
                default:                  return AST::ModportPort::DirectionEnum::INPUT;
                }
            }

            AST::Interface::Default_nettypeEnum
            to_interface_nettype(AST::Module::Default_nettypeEnum nt) {
                using M = AST::Module::Default_nettypeEnum;
                using I = AST::Interface::Default_nettypeEnum;
                switch(nt) {
                case M::WIRE:    return I::WIRE;
                case M::TRI:     return I::TRI;
                case M::TRI0:    return I::TRI0;
                case M::TRI1:    return I::TRI1;
                case M::TRIAND:  return I::TRIAND;
                case M::TRIOR:   return I::TRIOR;
                case M::TRIREG:  return I::TRIREG;
                case M::WAND:    return I::WAND;
                case M::WOR:     return I::WOR;
                case M::UWIRE:   return I::UWIRE;
                case M::SUPPLY0: return I::SUPPLY0;
                case M::SUPPLY1: return I::SUPPLY1;
                case M::NONE:    return I::NONE;
                }
                return I::NONE;
            }

            AST::NamedType::Ptr make_named_type(const std::string &name, const std::string &package,
                                                const std::string &filename, uint32_t line) {
                auto nt = std::make_shared<AST::NamedType>(filename, line);
                nt->set_name(name);
                if(!package.empty()) {
                    auto scope = std::make_shared<AST::ScopeName::List>();
                    auto seg = std::make_shared<AST::ScopeName>(filename, line);
                    seg->set_name(package);
                    scope->push_back(seg);
                    nt->set_scope(scope);
                }
                return nt;
            }

            AST::DataType::Ptr make_data_type(const data_type_t &dt,
                                              const std::string &filename, uint32_t line) {
                // Shared subtrees (named ref, inline def, packed dims) are cloned so
                // the same data_type_t can feed every name of a comma list / port
                // group without two nodes sharing a child (tree invariant).
                const AST::Dimension::ListPtr packed =
                    dt.packed_dims ? AST::Dimension::clone_list(dt.packed_dims) : nullptr;

                AST::DataType::Ptr type;
                bool set_signing = false;

                switch(dt.kind) {
                case data_type_kind_t::BIT:       type = std::make_shared<AST::BitType>(filename, line); set_signing = true; break;
                case data_type_kind_t::LOGIC:     type = std::make_shared<AST::LogicType>(filename, line); set_signing = true; break;
                case data_type_kind_t::REG:       type = std::make_shared<AST::RegType>(filename, line); set_signing = true; break;
                case data_type_kind_t::BYTE:      type = std::make_shared<AST::ByteType>(filename, line); set_signing = true; break;
                case data_type_kind_t::SHORTINT:  type = std::make_shared<AST::ShortintType>(filename, line); set_signing = true; break;
                case data_type_kind_t::INT:       type = std::make_shared<AST::IntType>(filename, line); set_signing = true; break;
                case data_type_kind_t::LONGINT:   type = std::make_shared<AST::LongintType>(filename, line); set_signing = true; break;
                case data_type_kind_t::INTEGER:   type = std::make_shared<AST::IntegerType>(filename, line); set_signing = true; break;
                case data_type_kind_t::REAL:      type = std::make_shared<AST::RealType>(filename, line); break;
                case data_type_kind_t::SHORTREAL: type = std::make_shared<AST::ShortrealType>(filename, line); break;
                case data_type_kind_t::REALTIME:  type = std::make_shared<AST::RealtimeType>(filename, line); break;
                case data_type_kind_t::NAMED:
                    type = dt.named ? AST::cast_to<AST::DataType>(dt.named->clone()) : nullptr;
                    break;
                case data_type_kind_t::STRUCT:
                case data_type_kind_t::UNION:
                case data_type_kind_t::ENUM:
                case data_type_kind_t::TYPEOP:
                case data_type_kind_t::IFACE:
                    type = dt.inline_def ? AST::cast_to<AST::DataType>(dt.inline_def->clone()) : nullptr;
                    break;
                }

                if(type) {
                    if(set_signing) {
                        type->set_signing(to_signing(dt.signing));
                    }
                    if(packed) {
                        type->set_packed_dims(packed);
                    }
                }
                return type;
            }

            AST::Net::Ptr create_net_type(const decl_name_t &decl, net_type_t nt,
                                          AST::DataType::Ptr type,
                                          const std::string &filename, uint32_t line) {
                AST::Net::Ptr net;
                switch(nt) {
                case net_type_t::WIRE:    net = std::make_shared<AST::WireNet>(filename, line); break;
                case net_type_t::TRI:     net = std::make_shared<AST::TriNet>(filename, line); break;
                case net_type_t::SUPPLY0: net = std::make_shared<AST::Supply0Net>(filename, line); break;
                case net_type_t::SUPPLY1: net = std::make_shared<AST::Supply1Net>(filename, line); break;
                default: break;
                }
                if(net) {
                    net->set_type(type ? type : std::make_shared<AST::ImplicitType>(filename, line));
                    net->set_name(decl.name);
                    net->set_unpacked_dims(decl.lengths);
                    net->set_cont_assign(decl.rvalue);
                    net->set_ldelay(decl.ldelay);
                    net->set_rdelay(decl.rdelay);
                }
                return net;
            }

            AST::Var::Ptr build_variable(const data_type_t &dt, const decl_name_t &decl,
                                         const std::string &filename, uint32_t line) {
                auto var = std::make_shared<AST::Var>(filename, line);
                var->set_type(make_data_type(dt, filename, line));
                var->set_name(decl.name);
                var->set_unpacked_dims(decl.lengths);
                var->set_init(decl.rvalue);
                return var;
            }

            AST::Var::Ptr build_implicit_type(signing_t signing, AST::Dimension::ListPtr widths,
                                              const decl_name_t &decl,
                                              const std::string &filename, uint32_t line) {
                auto type = std::make_shared<AST::ImplicitType>(filename, line);
                if(signing != signing_t::NONE) {
                    type->set_signing(to_signing(signing));
                }
                if(widths) {
                    type->set_packed_dims(widths);
                }
                auto var = std::make_shared<AST::Var>(filename, line);
                var->set_type(type);
                var->set_name(decl.name);
                var->set_unpacked_dims(decl.lengths);
                var->set_init(decl.rvalue);
                return var;
            }

            AST::Node::Ptr wrap_data_modifier(AST::Var::Ptr inner, const data_qualifiers_t &q) {
                if(inner) {
                    inner->set_is_var(q.is_var);
                    inner->set_is_const(q.is_const);
                    inner->set_lifetime(q.lifetime);
                }
                return AST::to_node(inner);
            }

            AST::Node::ListPtr build_named_modifier(const AST::NamedType::Ptr &ref,
                                                    AST::Dimension::ListPtr widths,
                                                    const std::list<decl_name_t> &names,
                                                    const data_qualifiers_t &q,
                                                    const std::string &filename, uint32_t line) {
                // make_data_type clones the named ref + packed dims per name.
                data_type_t dt{data_type_kind_t::NAMED, signing_t::NONE, widths, ref, nullptr};
                auto list = std::make_shared<AST::Node::List>();
                for(const decl_name_t &d : names) {
                    AST::Var::Ptr var = build_variable(dt, d, filename, line);
                    list->push_back(wrap_data_modifier(var, q));
                }
                return list;
            }

            AST::Member::ListPtr build_struct_members(const data_type_t &dt,
                                                      const std::list<decl_name_t> &names,
                                                      const std::string &filename, uint32_t line) {
                // One Member per name: the type is the bare data_type (cloned per
                // member), unpacked dims + init come from the variable_decl_assignment.
                auto list = std::make_shared<AST::Member::List>();
                for(const decl_name_t &decl : names) {
                    auto member = std::make_shared<AST::Member>(filename, line);
                    member->set_name(decl.name);
                    member->set_type(make_data_type(dt, filename, line));
                    member->set_unpacked_dims(decl.lengths);
                    member->set_init(decl.rvalue);
                    list->push_back(member);
                }
                return list;
            }

            AST::DataType::Ptr build_data_type_def(const data_type_t &dt,
                                                   const std::string &filename, uint32_t line) {
                // The bare type a data_type denotes (typedef/return/member type):
                // make_data_type already carries signing + packed dims and clones
                // the named ref / inline def.
                return make_data_type(dt, filename, line);
            }

            AST::Port::Ptr create_ioport_decls(direction_t direction, net_type_t net_type, signing_t signing,
                                               AST::Dimension::ListPtr widths, std::string name,
                                               const std::string &filename, uint32_t line) {
                auto port = std::make_shared<AST::Port>(filename, line);
                port->set_name(name);
                port->set_direction(to_port_dir(direction));

                auto type = std::make_shared<AST::ImplicitType>(filename, line);
                if(signing != signing_t::NONE) type->set_signing(to_signing(signing));
                if(widths) type->set_packed_dims(AST::Dimension::clone_list(widths));

                AST::Net::Ptr net;
                if(net_type == net_type_t::NONE) {
                    net = std::make_shared<AST::ImplicitNet>(filename, line);
                    net->set_type(type);
                    net->set_name(name);
                } else {
                    decl_name_t decl;
                    decl.name = name;
                    net = create_net_type(decl, net_type, type, filename, line);
                }
                port->set_decl(net);
                return port;
            }

            AST::Port::Ptr create_typed_ioport_decls(direction_t direction,
                                                     const std::string &type_name,
                                                     const std::string &type_package,
                                                     const std::string &name,
                                                     const std::string &filename, uint32_t line) {
                auto port = std::make_shared<AST::Port>(filename, line);
                port->set_name(name);
                port->set_direction(to_port_dir(direction));
                auto net = std::make_shared<AST::ImplicitNet>(filename, line);
                net->set_type(make_named_type(type_name, type_package, filename, line));
                net->set_name(name);
                port->set_decl(net);
                return port;
            }

            AST::Port::Ptr create_interface_port(const std::string &type_name,
                                                 const std::string &type_package,
                                                 const std::string &modport,
                                                 const std::string &name,
                                                 const std::string &filename, uint32_t line) {
                auto port = std::make_shared<AST::Port>(filename, line);
                port->set_name(name);
                port->set_direction(AST::Port::DirectionEnum::NONE);
                auto arg = std::make_shared<AST::Arg>(filename, line);
                arg->set_name(name);
                arg->set_direction(AST::Arg::DirectionEnum::NONE);
                if(!modport.empty()) {
                    auto type = std::make_shared<AST::InterfaceType>(filename, line);
                    type->set_name(type_name);
                    type->set_modport(modport);
                    arg->set_type(type);
                } else {
                    arg->set_type(make_named_type(type_name, type_package, filename, line));
                }
                port->set_decl(arg);
                return port;
            }

            AST::Port::Ptr create_data_type_port(direction_t direction, const data_type_t &dt,
                                                 const std::string &name,
                                                 const std::string &filename, uint32_t line) {
                auto port = std::make_shared<AST::Port>(filename, line);
                port->set_name(name);
                port->set_direction(to_port_dir(direction));
                auto net = std::make_shared<AST::ImplicitNet>(filename, line);
                net->set_type(make_data_type(dt, filename, line));
                net->set_name(name);
                port->set_decl(net);
                return port;
            }

            AST::Port::Ptr create_net_data_type_port(direction_t direction, net_type_t net_type,
                                                     const data_type_t &dt, const std::string &name,
                                                     const std::string &filename, uint32_t line) {
                auto port = std::make_shared<AST::Port>(filename, line);
                port->set_name(name);
                port->set_direction(to_port_dir(direction));
                decl_name_t decl;
                decl.name = name;
                AST::Net::Ptr net =
                    create_net_type(decl, net_type, make_data_type(dt, filename, line), filename, line);
                port->set_decl(net);
                return port;
            }

            // task/function argument list: one Arg per port_info (direction
            // inherited from the previous arg when absent). The arg's type comes
            // from the data_type / named type / implicit signing+dims of the info.
            AST::Arg::ListPtr create_args_decls(const std::list<port_info_t> &port_list,
                                                const std::string &filename) {
                auto list = std::make_shared<AST::Arg::List>();
                direction_t last_dir = direction_t::INPUT;
                for(const port_info_t &pinfo : port_list) {
                    direction_t dir = (pinfo.direction != direction_t::NONE) ? pinfo.direction
                                                                            : last_dir;
                    uint32_t line = pinfo.loc.begin.line;
                    auto arg = std::make_shared<AST::Arg>(filename, line);
                    arg->set_name(pinfo.name);
                    arg->set_direction(to_arg_dir(dir));

                    if(pinfo.has_data_type) {
                        arg->set_type(make_data_type(pinfo.data_type, filename, line));
                    } else if(!pinfo.type_name.empty()) {
                        arg->set_type(make_named_type(pinfo.type_name, pinfo.type_package, filename, line));
                    } else {
                        auto t = std::make_shared<AST::ImplicitType>(filename, line);
                        if(pinfo.signing != signing_t::NONE) t->set_signing(to_signing(pinfo.signing));
                        if(pinfo.widths) t->set_packed_dims(AST::Dimension::clone_list(pinfo.widths));
                        arg->set_type(t);
                    }
                    list->push_back(arg);
                    last_dir = dir;
                }
                return list;
            }

            AST::Port::ListPtr create_ports_decls(const std::list<port_info_t> &port_list,
                                                  const std::string &filename,
                                                  location &loc, std::string &error_message) {
                direction_t last_dir = direction_t::NONE;
                net_type_t last_net = net_type_t::NONE;
                signing_t last_signing = signing_t::NONE;
                AST::Dimension::ListPtr last_widths(nullptr);
                std::string last_type_name;
                std::string last_type_package;
                std::string last_modport;
                bool last_has_data_type = false;
                data_type_t last_data_type{};

                AST::Port::ListPtr node_list = std::make_shared<AST::Port::List>();

                if (port_list.empty()) {
                    return node_list;
                }

                if (port_list.front().direction == direction_t::NONE &&
                    port_list.front().type_name.empty()) {
                    // name-only ports (non-ANSI header references)
                    for(const port_info_t &pinfo: port_list) {
                        if ((pinfo.direction != direction_t::NONE) || (pinfo.net_type != net_type_t::NONE) ||
                            (pinfo.signing != signing_t::NONE) || (pinfo.widths) || (!pinfo.type_name.empty()) ||
                            (pinfo.has_data_type)) {
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
                    for(const port_info_t &pinfo: port_list) {
                        direction_t dir;
                        net_type_t net_type;
                        AST::Dimension::ListPtr widths;
                        signing_t signing;
                        std::string type_name;
                        std::string type_package;
                        std::string modport;
                        bool has_data_type;
                        data_type_t data_type;

                        if (pinfo.direction == direction_t::NONE && !pinfo.type_name.empty() &&
                            (!pinfo.modport.empty() || last_dir == direction_t::NONE)) {
                            // Directionless named-type port that cannot be a
                            // direction-inheriting typed port: either the
                            // `.modport` suffix makes it a decisive interface
                            // port, or there is no preceding direction to
                            // inherit (first port / after another such port).
                            // Bare -> NamedType left for the resolution pass
                            // (ADR-0002 / ADR-0003 §4.4). Starts a fresh
                            // inheritance group.
                            dir = direction_t::NONE;
                            net_type = net_type_t::NONE;
                            widths = nullptr;
                            signing = signing_t::NONE;
                            type_name = pinfo.type_name;
                            type_package = pinfo.type_package;
                            modport = pinfo.modport;
                            has_data_type = false;
                            data_type = data_type_t{};
                        }
                        else if (pinfo.direction == direction_t::NONE && !pinfo.type_name.empty()) {
                            // Bare named-type port after a directional port: the
                            // direction is omitted but a data type is present,
                            // so ONLY the direction is inherited (IEEE 1800-2017
                            // §23.2.2.3) — a normal named-type port. If the name
                            // proves to be an interface, the resolution pass
                            // rewrites it (the NamedType is preserved).
                            dir = last_dir;
                            net_type = net_type_t::NONE;
                            widths = nullptr;
                            signing = signing_t::NONE;
                            type_name = pinfo.type_name;
                            type_package = pinfo.type_package;
                            modport.clear();
                            has_data_type = false;
                            data_type = data_type_t{};
                        }
                        else if (pinfo.direction == direction_t::NONE) {
                            if ((pinfo.net_type != net_type_t::NONE) || (pinfo.signing != signing_t::NONE) ||
                                (pinfo.widths) || (pinfo.has_data_type)) {
                                loc = pinfo.loc;
                                error_message = std::string("missing port direction qualifier");
                                return nullptr;
                            }
                            dir = last_dir;
                            net_type = last_net;
                            widths = last_widths;
                            signing = last_signing;
                            type_name = last_type_name;
                            type_package = last_type_package;
                            modport = last_modport;
                            has_data_type = last_has_data_type;
                            data_type = last_data_type;
                        }
                        else {
                            dir = pinfo.direction;
                            net_type = pinfo.net_type;
                            widths = pinfo.widths;
                            signing = pinfo.signing;
                            type_name = pinfo.type_name;
                            type_package = pinfo.type_package;
                            modport = pinfo.modport;
                            has_data_type = pinfo.has_data_type;
                            data_type = pinfo.data_type;
                        }

                        if (widths) widths = AST::Dimension::clone_list(widths);

                        AST::Port::Ptr iop;
                        if (dir == direction_t::NONE && !type_name.empty()) {
                            iop = create_interface_port(type_name, type_package, modport, pinfo.name,
                                                        filename, pinfo.loc.begin.line);
                        }
                        else if (has_data_type && net_type != net_type_t::NONE) {
                            iop = create_net_data_type_port(dir, net_type, data_type, pinfo.name,
                                                            filename, pinfo.loc.begin.line);
                        }
                        else if (has_data_type) {
                            iop = create_data_type_port(dir, data_type, pinfo.name,
                                                        filename, pinfo.loc.begin.line);
                        }
                        else if (!type_name.empty()) {
                            iop = create_typed_ioport_decls(dir, type_name, type_package, pinfo.name,
                                                            filename, pinfo.loc.begin.line);
                        }
                        else {
                            iop = create_ioport_decls(dir, net_type, signing, widths, pinfo.name,
                                                      filename, pinfo.loc.begin.line);
                        }
                        node_list->push_back(iop);
                        last_dir = dir;
                        last_net = net_type;
                        last_widths = widths;
                        last_signing = signing;
                        last_type_name = type_name;
                        last_type_package = type_package;
                        last_modport = modport;
                        last_has_data_type = has_data_type;
                        last_data_type = data_type;
                    }
                }

                return node_list;
            }

        }
    }
}
