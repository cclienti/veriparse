%skeleton "lalr1.cc"
%require  "3.4"

%defines
%locations

%define api.namespace {Veriparse::Parser}
%define api.parser.class {VerilogNumberParser}

%define lr.type lalr
%define api.value.type variant
%define parse.assert
%define parse.error verbose

%parse-param {VerilogNumberScanner &scanner}
%parse-param {VerilogNumberDriver &driver}
%parse-param {const std::string str_to_parse}
%parse-param {mpz_class &mpz}
%parse-param {int &size}
%parse-param {int &base}
%parse-param {bool &is_signed}
%parse-param {int &rc}
%parse-param {const std::string filename}
%parse-param {const int line}


%code requires {
#include <verilog_number/location.hh>
#include <cstdint>
#include <vector>
#include <string>
#include <gmpxx.h>


namespace Veriparse {
    namespace Parser {
        class VerilogNumberDriver;
        class VerilogNumberScanner;

        typedef std::vector<int> digit_list_t;

        typedef struct {
            int base;
            bool is_signed;
        } base_t;

        namespace ParserHelpers {
            int list_to_number(const std::string &filename, const int line, const std::string &str_to_parse,
                               const digit_list_t &list, base_t base, int size, mpz_class &number);

            void warning(const std::string &filename, const int line, const std::string &str_to_parse,
                         const std::string &message);

            void error(const std::string &filename, const int line, const std::string &str_to_parse,
                       const std::string &message);
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
#include <veriparse/logger/logger.hpp>
#include <veriparse/logger/string_report.hpp>

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <memory>

    /* include for all driver functions */
#include <parser/verilog_number/verilog_number_driver.hpp>
#undef yylex
#define yylex scanner.yylex
}

// Token declarations
%token TK_0      "0"
%token TK_1      "1"
%token TK_2      "2"
%token TK_3      "3"
%token TK_4      "4"
%token TK_5      "5"
%token TK_6      "6"
%token TK_7      "7"
%token TK_8      "8"
%token TK_9      "9"

%token TK_A      "A"
%token TK_B      "B"
%token TK_C      "C"
%token TK_D      "D"
%token TK_E      "E"
%token TK_F      "F"

%token TK_X      "X"
%token TK_Z      "Z"
%token TK_O      "O"
%token TK_H      "H"
%token TK_S      "S"

%token TK_SQUOTE "'"


//  Rules types
%type   <int>                digit
%type   <digit_list_t>       digits
%type   <int>                size
%type   <base_t>             base


//  Grammar
%%

number:         without_base_number
        |       with_base_number
        ;

without_base_number:
                digits
                {
                    base_t b;
                    mpz = 0;
                    size = -1;
                    b.base = base = 10;
                    b.is_signed = is_signed = false;
                    rc = ParserHelpers::list_to_number(filename, line, str_to_parse, $1, b, -1, mpz);
                }
        ;

with_base_number:
                size base digits
                {
                    mpz = 0;
                    size = $1;
                    base = $2.base;
                    is_signed = $2.is_signed;
                    rc = ParserHelpers::list_to_number(filename, line, str_to_parse, $3, $2, $1, mpz);
                }

        |       base digits
                {
                    mpz = 0;
                    size = -1;
                    base = $1.base;
                    is_signed = $1.is_signed;
                    rc = ParserHelpers::list_to_number(filename, line, str_to_parse, $2, $1, -1, mpz);
                }
        ;


base:           TK_SQUOTE TK_D       {$$.base = 10; $$.is_signed = false;}
        |       TK_SQUOTE TK_B       {$$.base = 2;  $$.is_signed = false;}
        |       TK_SQUOTE TK_O       {$$.base = 8;  $$.is_signed = false;}
        |       TK_SQUOTE TK_H       {$$.base = 16; $$.is_signed = false;}
        |       TK_SQUOTE TK_S TK_D  {$$.base = 10; $$.is_signed = true;}
        |       TK_SQUOTE TK_S TK_B  {$$.base = 2;  $$.is_signed = true;}
        |       TK_SQUOTE TK_S TK_O  {$$.base = 8;  $$.is_signed = true;}
        |       TK_SQUOTE TK_S TK_H  {$$.base = 16; $$.is_signed = true;}
        ;


size:           digits
                {
                    $$ = 0;
                    if ($1.front() == 0) {
                        error(@1, "leading zero in the size field");
                    }

                    for (int digit: $1) {
                        if ((digit >=0) && (digit <=9)) {
                            $$ = $$*10 + digit;
                        }
                        else {
                            error(@1, "unauthorized digit in the size field");
                        }
                    }
                }
        ;


digits:         digits digit
                {
                    $$ = $1;
                    if ($2 > -3) $$.push_back($2);
                }

        |       digit
                {
                    $$.clear();
                    $$.push_back($1);
                }
        ;


digit:          TK_0     {$$ = 0;}
        |       TK_1     {$$ = 1;}
        |       TK_2     {$$ = 2;}
        |       TK_3     {$$ = 3;}
        |       TK_4     {$$ = 4;}
        |       TK_5     {$$ = 5;}
        |       TK_6     {$$ = 6;}
        |       TK_7     {$$ = 7;}
        |       TK_8     {$$ = 8;}
        |       TK_9     {$$ = 9;}
        |       TK_A     {$$ = 10;}
        |       TK_B     {$$ = 11;}
        |       TK_C     {$$ = 12;}
        |       TK_D     {$$ = 13;}
        |       TK_E     {$$ = 14;}
        |       TK_F     {$$ = 15;}
        |       TK_X     {$$ = -1;}
        |       TK_Z     {$$ = -2;}
        ;


%%

namespace Veriparse {
    namespace Parser {

        void VerilogNumberParser::error(const location_type &l,
                                        const std::string &err_message) {
            std::string s = Logger::string_report(err_message, str_to_parse, l.begin.column, l.end.column);
            if (filename != "") {
                LOG_ERROR << "in file \"" << filename << "\", line " << line << ": " << s;
            }
            else {
                LOG_ERROR << s;
            }
        }

        namespace ParserHelpers {
            int list_to_number(const std::string &filename, const int line, const std::string &str_to_parse,
                               const digit_list_t &list, base_t base, int size, mpz_class &number) {

                for (int digit: list) {
                    if ((digit >= 0) && (digit < base.base)) {
                        number = number*base.base + digit;
                    }
                    /* else if (base.base == 10) { */
                    /*     std::stringstream ss; */
                    /*     ss << "unauthorized digit in a base " << base.base << " number"; */
                    /*     ParserHelpers::error(filename, line, str_to_parse, ss.str()); */
                    /*     return 2; */
                    /* } */
                    else {
                        return 1;
                    }
                }

                LOG_TRACE << "number: " << number
                          << ", size: " << size
                          << ", base: " << base.base
                          << ", is_signed: " << (base.is_signed ? "true" : "false");

                if (size != -1) {
                    mpz_class mpz_msb = mpz_class(1) << (size-1);
                    mpz_class mpz_max = (mpz_msb << 1) - 1;

                    if (number > mpz_max) {
                        std::stringstream ss;
                        ss << "is truncated to fit " << size << " bit";
                        if (size > 1) ss << "s";
                        ParserHelpers::warning(filename, line, str_to_parse, ss.str());
                        number = number & mpz_max;
                    }

                    if (base.is_signed) {
                        mpz_msb = mpz_msb & number;

                        if (mpz_msb != 0) {
                            number = ~number + 1;
                            number = number & mpz_max;
                            number = -number;
                        }
                    }
                }

                return 0;
            }

            void warning(const std::string &filename, const int line, const std::string &str_to_parse,
                         const std::string &message) {
                if (filename == "") {
                    LOG_WARNING << str_to_parse << " " << message;
                }
                else {
                    LOG_WARNING << "in file \"" << filename << "\" at line " << line << ": "
                                << str_to_parse << " " << message;
                }
            }

            void error(const std::string &filename, const int line, const std::string &str_to_parse,
                       const std::string &message) {
                if (filename == "") {
                    LOG_ERROR << str_to_parse << " " << message;
                }
                else {
                    LOG_ERROR << "in file \"" << filename << "\" at line " << line << ": "
                              << str_to_parse << " " << message;
                }
            }
        }

    }
}
