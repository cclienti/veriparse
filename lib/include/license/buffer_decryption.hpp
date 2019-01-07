#ifndef LICENSE_BUFFER_DECRYPTION_HPP
#define LICENSE_BUFFER_DECRYPTION_HPP

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
#include <cstdlib>

namespace license
{

static inline int decrypt_buffer(std::string const &rsa_public_key_buffer,
                                 std::string const rsa_encrypted_aes_key_buffer,
                                 std::string const &input_buffer, std::string &output_buffer)
{

	//------------------------------
	// Decrypt the AES key using RSA
	//------------------------------

	std::string rsa_public_key_buffer_copy = rsa_public_key_buffer;
	BIO *keybio = BIO_new_mem_buf(&rsa_public_key_buffer_copy[0], -1);
	if (keybio == NULL) {
		LOG_DEBUG << "Could not create the public decryption key bio";
		return 1;
	}

	RSA *rsa_public_key = NULL;
	rsa_public_key = PEM_read_bio_RSA_PUBKEY(keybio, &rsa_public_key, NULL, NULL);
	if (rsa_public_key == NULL) {
		LOG_DEBUG << "Could not create the public decryption key";
		return 1;
	}

	if((static_cast<unsigned>(RSA_size(rsa_public_key) * 2)) != rsa_encrypted_aes_key_buffer.size()) {
		LOG_DEBUG << "The rsa encrypted aes key buffer is corrupted";
		return 1;
	}

	std::uint32_t rsa_encrypted_aes_key_size = rsa_encrypted_aes_key_buffer.size()/2;

	std::uint8_t *rsa_encrypted_aes_key = new std::uint8_t[rsa_encrypted_aes_key_size];
	for(std::uint32_t i=0; i<rsa_encrypted_aes_key_buffer.size(); i+=2) {
		char to_int[3] = {rsa_encrypted_aes_key_buffer[i+1], rsa_encrypted_aes_key_buffer[i], 0};
		rsa_encrypted_aes_key[i>>1] = strtol(to_int, NULL, 16) ^ 42;
	}

	std::uint8_t *aes_key = new std::uint8_t[RSA_size(rsa_public_key)];

	int aes_key_size = RSA_public_decrypt(rsa_encrypted_aes_key_size,
	                                      rsa_encrypted_aes_key, aes_key, rsa_public_key, RSA_PKCS1_PADDING);

	delete [] rsa_encrypted_aes_key;

	if (aes_key_size == -1) {
		ERR_load_crypto_strings();
		std::uint64_t e = ERR_get_error();
		LOG_DEBUG << "RSA decryption failed, " << ERR_error_string(e, NULL);
		ERR_free_strings();
		return 1;
	}

	std::stringstream debug_aes;
	debug_aes << std::hex << std::setfill('0');
	for(int i=0; i<aes_key_size; i++) {
		debug_aes <<  std::setw(2) << static_cast<std::uint32_t>(aes_key[i]);
	}


	//-------------------------------------------
	// Decrypt the input buffer using the AES key
	//-------------------------------------------

	AES_KEY aes_dec_key;
	AES_set_decrypt_key(aes_key, aes_key_size*8, &aes_dec_key);

	std::uint8_t aes_decrypt_input[16];
	std::uint8_t aes_decrypt_output[16+1];

	output_buffer.clear();

	for(std::uint32_t block=0; block<input_buffer.size(); block+=32) {
		for(int i=0; i<32; i+=2) {
			char to_int[3] = {input_buffer[block+i+1], input_buffer[block+i], 0};
			aes_decrypt_input[i>>1] = strtol(to_int, NULL, 16) ^ 17;
		}

		AES_decrypt(aes_decrypt_input, aes_decrypt_output, &aes_dec_key);

		aes_decrypt_output[16] = 0;
		output_buffer.append(reinterpret_cast<char*>(aes_decrypt_output));
	}

	return 0;
}

}

#endif
