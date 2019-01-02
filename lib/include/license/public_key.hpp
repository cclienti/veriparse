// File generated the 2016-01-24 20:39:51, do not edit!
// Command: ./encode_key.py ../keys/licenses/public.pem public_key.hpp.template public_key

#ifndef LICENSE_PUBLIC_KEY_HPP
#define LICENSE_PUBLIC_KEY_HPP

#include <license/hide_string.hpp>
#include <cstring>

namespace license
{

DEFINE_HIDDEN_STRING(public_key_0, 32, ('-')('-')('-')('-')('-')('B')('E')('G')('I')('N')(' ')('P')('U')('B')('L')('I')('C')(' ')('K')('E')('Y')('-')('-')('-')('-')('-')('\n'))

DEFINE_HIDDEN_STRING(public_key_1, 252, ('M')('I')('I')('B')('I')('j')('A')('N')('B')('g')('k')('q')('h')('k')('i')('G')('9')('w')('0')('B')('A')('Q')('E')('F')('A')('A')('O')('C')('A')('Q')('8')('A')('M')('I')('I')('B')('C')('g')('K')('C')('A')('Q')('E')('A')('x')('d')('S')('v')('2')('M')('h')('v')('J')('d')('E')('I')('X')('i')('d')('n')('n')('j')('+')('K')('\n'))

DEFINE_HIDDEN_STRING(public_key_2, 137, ('3')('f')('M')('2')('+')('K')('x')('z')('3')('X')('f')('b')('N')('5')('z')('N')('R')('V')('R')('v')('H')('1')('F')('8')('n')('f')('W')('g')('E')('G')('k')('R')('w')('t')('c')('1')('W')('/')('u')('k')('/')('i')('l')('J')('x')('T')('i')('M')('I')('7')('m')('5')('O')('o')('k')('d')('i')('D')('U')('z')('f')('0')('Z')('D')('\n'))

DEFINE_HIDDEN_STRING(public_key_3, 181, ('0')('X')('G')('b')('g')('M')('x')('4')('k')('w')('S')('v')('9')('S')('9')('a')('y')('O')('0')('a')('4')('c')('3')('p')('D')('C')('g')('+')('Y')('z')('t')('b')('S')('d')('u')('x')('R')('E')('t')('m')('z')('F')('T')('Q')('V')('E')('i')('s')('j')('G')('D')('M')('Q')('E')('g')('4')('q')('T')('b')('r')('+')('o')('w')('n')('\n'))

DEFINE_HIDDEN_STRING(public_key_4, 54, ('m')('j')('2')('N')('U')('2')('Z')('4')('a')('p')('p')('W')('A')('4')('Z')('z')('E')('S')('Y')('L')('R')('9')('Z')('f')('P')('W')('t')('T')('3')('o')('x')('r')('m')('w')('9')('E')('e')('L')('8')('Z')('H')('x')('Q')('N')('A')('i')('U')('D')('C')('x')('u')('P')('0')('t')('A')('i')('B')('y')('O')('F')('y')('V')('l')('U')('\n'))

DEFINE_HIDDEN_STRING(public_key_5, 173, ('/')('+')('7')('O')('2')('O')('s')('H')('B')('A')('c')('g')('V')('i')('J')('P')('s')('W')('G')('I')('G')('O')('S')('w')('n')('q')('M')('Y')('y')('Y')('O')('4')('d')('t')('d')('u')('4')('B')('A')('X')('i')('2')('9')('I')('C')('L')('s')('R')('b')('V')('N')('+')('n')('F')('q')('/')('+')('Q')('7')('m')('2')('r')('9')('n')('\n'))

DEFINE_HIDDEN_STRING(public_key_6, 18, ('+')('e')('w')('t')('+')('z')('Z')('C')('B')('P')('4')('T')('h')('K')('N')('a')('/')('7')('k')('q')('7')('8')('Z')('M')('H')('r')('l')('M')('6')('9')('S')('3')('Q')('J')('e')('Y')('7')('d')('X')('I')('j')('X')('c')('H')('8')('8')('R')('J')('Z')('O')('L')('A')('j')('u')('z')('Z')('e')('n')('G')('H')('4')('I')('9')('4')('\n'))

DEFINE_HIDDEN_STRING(public_key_7, 252, ('t')('Q')('I')('D')('A')('Q')('A')('B')('\n'))

DEFINE_HIDDEN_STRING(public_key_8, 107, ('-')('-')('-')('-')('-')('E')('N')('D')(' ')('P')('U')('B')('L')('I')('C')(' ')('K')('E')('Y')('-')('-')('-')('-')('-')('\n'))


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
