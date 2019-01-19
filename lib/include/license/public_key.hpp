// File generated the 2019-01-19 19:30:01, do not edit!
// Command: ./encode_key.py ../../../apps/license_generator/resources/public.pem public_key.hpp.template public_key

#ifndef LICENSE_PUBLIC_KEY_HPP
#define LICENSE_PUBLIC_KEY_HPP

#include <license/hide_string.hpp>
#include <cstring>

namespace license
{

DEFINE_HIDDEN_STRING(public_key_0, 72, ('-')('-')('-')('-')('-')('B')('E')('G')('I')('N')(' ')('P')('U')('B')('L')('I')('C')(' ')('K')('E')('Y')('-')('-')('-')('-')('-')('\n'))

DEFINE_HIDDEN_STRING(public_key_1, 46, ('M')('I')('I')('B')('I')('j')('A')('N')('B')('g')('k')('q')('h')('k')('i')('G')('9')('w')('0')('B')('A')('Q')('E')('F')('A')('A')('O')('C')('A')('Q')('8')('A')('M')('I')('I')('B')('C')('g')('K')('C')('A')('Q')('E')('A')('+')('m')('K')('T')('0')('S')('7')('9')('8')('5')('u')('3')('i')('P')('F')('M')('m')('1')('E')('0')('\n'))

DEFINE_HIDDEN_STRING(public_key_2, 100, ('2')('k')('K')('m')('i')('v')('Z')('t')('Q')('4')('D')('A')('O')('p')('8')('v')('P')('Q')('J')('Q')('7')('+')('3')('h')('x')('Z')('8')('l')('j')('9')('3')('b')('K')('h')('c')('g')('N')('R')('h')('N')('3')('E')('E')('b')('4')('W')('v')('O')('T')('l')('h')('n')('T')('9')('a')('h')('g')('g')('f')('W')('V')('y')('w')('h')('\n'))

DEFINE_HIDDEN_STRING(public_key_3, 193, ('R')('d')('p')('z')('1')('P')('R')('o')('v')('o')('t')('I')('W')('V')('J')('l')('i')('V')('0')('X')('F')('v')('c')('q')('9')('G')('w')('9')('5')('g')('g')('U')('M')('f')('t')('G')('p')('P')('T')('b')('3')('1')('w')('r')('X')('N')('t')('x')('P')('h')('C')('U')('Z')('O')('U')('4')('w')('q')('V')('T')('H')('3')('2')('T')('\n'))

DEFINE_HIDDEN_STRING(public_key_4, 7, ('0')('X')('W')('Q')('4')('e')('D')('8')('O')('x')('I')('G')('O')('0')('R')('l')('r')('5')('i')('E')('0')('2')('x')('K')('P')('V')('4')('+')('5')('5')('x')('a')('P')('o')('v')('x')('F')('h')('H')('q')('2')('Y')('I')('8')('J')('d')('D')('r')('c')('H')('W')('N')('l')('V')('v')('J')('C')('V')('O')('E')('3')('B')('9')('e')('\n'))

DEFINE_HIDDEN_STRING(public_key_5, 93, ('M')('5')('4')('9')('A')('r')('V')('f')('Q')('u')('K')('N')('1')('k')('w')('m')('C')('B')('M')('Y')('o')('t')('L')('B')('y')('V')('n')('G')('c')('j')('B')('z')('S')('E')('o')('j')('N')('2')('N')('d')('U')('F')('6')('p')('o')('x')('A')('O')('I')('t')('S')('7')('A')('m')('C')('2')('L')('u')('m')('+')('G')('o')('v')('N')('\n'))

DEFINE_HIDDEN_STRING(public_key_6, 236, ('r')('s')('Y')('V')('O')('J')('5')('C')('b')('t')('2')('l')('+')('E')('c')('P')('o')('J')('Q')('7')('J')('Z')('i')('h')('A')('2')('Z')('b')('k')('I')('E')('W')('K')('K')('+')('N')('J')('u')('w')('h')('k')('1')('y')('K')('i')('P')('Q')('s')('n')('1')('8')('h')('y')('Q')('W')('C')('E')('Q')('L')('c')('x')('7')('i')('v')('\n'))

DEFINE_HIDDEN_STRING(public_key_7, 39, ('z')('Q')('I')('D')('A')('Q')('A')('B')('\n'))

DEFINE_HIDDEN_STRING(public_key_8, 134, ('-')('-')('-')('-')('-')('E')('N')('D')(' ')('P')('U')('B')('L')('I')('C')(' ')('K')('E')('Y')('-')('-')('-')('-')('-')('\n'))



static inline char *create_public_key() {
    char *s[] = {
			create_public_key_0(),
			create_public_key_1(),
			create_public_key_2(),
			create_public_key_3(),
			create_public_key_4(),
			create_public_key_5(),
			create_public_key_6(),
			create_public_key_7(),
			create_public_key_8()
		};

    int len = 0;

    for(size_t i=0; i<(sizeof(s)/sizeof(char*)); ++i)
        len += strlen(s[i]);

    char *data_out = new char[len];

    strcpy(data_out, s[0]);
    shred_string(s[0]);

    for(size_t i=1; i<(sizeof(s)/sizeof(char*)); ++i) {
        strcat(data_out, s[i]);
        shred_string(s[i]);
    }

    return data_out;
}

}

#endif
