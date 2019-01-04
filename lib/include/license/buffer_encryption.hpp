#ifndef LICENSE_BUFFER_ENCRYPTION_HPP
#define LICENSE_BUFFER_ENCRYPTION_HPP

#include <veriparse/logger/logger.hpp>

#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/aes.h>

#include <sstream>
#include <iomanip>
#include <random>
#include <string>
#include <cstring>
#include <cstdint>


namespace license
{

/// Encrypt a file, returns -1 on error.
static inline int encrypt_buffer(std::string const &rsa_private_key_buffer,
                                 std::string const &rsa_public_key_buffer,
                                 std::string const &input_buffer, std::string &output_buffer,
                                 std::string &rsa_encrypted_aes_key_buffer)
{

	//-------------------------
	// Generate the AES-256 key
	//-------------------------

	uint8_t aes_key[32]; // 256-bit AES Key
	uint32_t aes_key_size = sizeof(aes_key);

	std::random_device rd;
	std::uniform_int_distribution<uint8_t> distribution(0, 255);
	for (size_t i=0; i<aes_key_size; i++) aes_key[i] = distribution(rd);

	std::stringstream debug_aes;
	debug_aes << std::hex << std::setfill('0');
	for(size_t i=0; i<aes_key_size; i++) {
		debug_aes <<  std::setw(2) << static_cast<uint32_t>(aes_key[i]);
	}

	//------------------------------
	// Encrypt the AES key using RSA
	//------------------------------

	std::string rsa_private_key_buffer_copy = rsa_private_key_buffer;
	BIO *private_keybio = BIO_new_mem_buf(&rsa_private_key_buffer_copy[0], -1);
	if (private_keybio == NULL) {
		LOG_ERROR << "Could not create the private encryption key bio";
		return 1;
	}

	std::string rsa_public_key_buffer_copy = rsa_public_key_buffer;
	BIO *public_keybio = BIO_new_mem_buf(&rsa_public_key_buffer_copy[0], -1);
	if (public_keybio == NULL) {
		LOG_ERROR << "Could not create the public encryption key bio";
		return 1;
	}

	RSA *rsa_private_key = NULL;
	rsa_private_key = PEM_read_bio_RSAPrivateKey(private_keybio, &rsa_private_key, NULL, NULL);
	if (rsa_private_key == NULL) {
		LOG_ERROR << "Could not create the private encryption key";
		return 1;
	}

	RSA *rsa_public_key = NULL;
	rsa_public_key = PEM_read_bio_RSA_PUBKEY(public_keybio, &rsa_public_key, NULL, NULL);
	if (rsa_public_key == NULL) {
		LOG_ERROR << "Could not create the public encryption key";
		return 1;
	}

	uint8_t *rsa_encrypted_aes_key = new uint8_t[RSA_size(rsa_private_key)];

	int rsa_size = RSA_private_encrypt(aes_key_size, aes_key, rsa_encrypted_aes_key,
	                                   rsa_private_key, RSA_PKCS1_PADDING);

	if (rsa_size < 0) {
		LOG_ERROR << "Could not private encrypt the aes key";
		delete[] rsa_encrypted_aes_key;
		return 1;
	}


	//-----------------------------
	// Return the AES encrypted key
	//-----------------------------

	std::stringstream ss;
	ss << std::hex << std::setfill('0');
	for (int i=0; i<rsa_size; i++) {
		// The byte is xored, then nibbles are reversed to harden
		// the aes key discovery.
		uint8_t nibble_reverse = rsa_encrypted_aes_key[i] ^ 42;
		nibble_reverse = (((nibble_reverse & 0x0f) << 4) | ((nibble_reverse & 0xf0) >> 4));
		ss << std::setw(2) << static_cast<int>(nibble_reverse);
	}
	rsa_encrypted_aes_key_buffer = ss.str();

	delete[] rsa_encrypted_aes_key;


	//-------------------------------------------------
	// Encrypt the input buffer using the plain AES key
	//-------------------------------------------------

	char const * input_buffer_str = input_buffer.c_str();

	AES_KEY aes_enc_key;
	AES_set_encrypt_key(aes_key, aes_key_size*8, &aes_enc_key);

	// AES function works only on 128-bit buffer even if the key is
	// larger.
	//
	// But due to gcc-8 checking of bad truncation
	// (-Werror=stringop-truncation), the compilation fails. The
	// attribute '__attribute__ ((nonstring))' can be added to the
	// following declarations. It specifies that the array can be not
	// null-terminated and the strncpy is safe.
	//
	// As this attribute is not standard, we choose to add an extra
	// byte in the array and to null-terminate the array after each
	// call to strncpy.
	uint8_t aes_encrypt_input[16+1]; // __attribute__ ((nonstring));
	uint8_t aes_encrypt_output[16+1]; // __attribute__ ((nonstring));

	ss.str(std::string());
	ss.clear();
	ss << std::hex << std::setfill('0');

	for(uint32_t block=0; block<strlen(input_buffer_str)+1; block+=16) {
		strncpy(reinterpret_cast<char*>(aes_encrypt_input), input_buffer_str+block, 16);
		aes_encrypt_input[sizeof(aes_encrypt_input)-1] = '\0';
		AES_encrypt(aes_encrypt_input, aes_encrypt_output, &aes_enc_key);

		for(uint32_t i=0; i<16; i++) {
			// The byte is xored, then nibbles are reversed to harden
			// the aes key discovery.
			uint8_t nibble_reverse = aes_encrypt_output[i] ^ 17;
			nibble_reverse = (((nibble_reverse & 0x0f) << 4) | ((nibble_reverse & 0xf0) >> 4));
			ss << std::setw(2) << static_cast<uint32_t>(nibble_reverse);
		}
	}

	output_buffer = ss.str();

	return 0;
}

}

#endif
