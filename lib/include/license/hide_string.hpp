#ifndef LICENSE_HIDE_STRING_HPP
#define LICENSE_HIDE_STRING_HPP

#include <boost/preprocessor.hpp>

#define CRYPT_MACRO(r, d, i, elem) ((char) (elem ^ ( d - i )))

#define DEFINE_HIDDEN_STRING(NAME, SEED, SEQ)                                      \
	static inline char* BOOST_PP_CAT(create_, NAME)() {                             \
		const char data[] = {                                                        \
			BOOST_PP_SEQ_ENUM(BOOST_PP_SEQ_FOR_EACH_I(CRYPT_MACRO, SEED, SEQ)), '\0'  \
		};                                                                           \
                                                                                   \
		char *data_out = new char [sizeof(data)/sizeof(data[0])];                    \
                                                                                   \
		for (unsigned i=0; i<(sizeof(data)/sizeof(data[0]))-1; ++i) {                \
			data_out[i] = CRYPT_MACRO(_, SEED, i, data[i]);                           \
		}                                                                            \
                                                                                   \
		data_out[(sizeof(data)/sizeof(data[0]))-1]='\0';                             \
		return data_out;                                                             \
	}

namespace license
{
inline void shred_string(char *str) {
	char *p = str;
	while (*p != '\0') {
		*p++ = '\0';
	}
	delete [] str;
}
}

#endif
