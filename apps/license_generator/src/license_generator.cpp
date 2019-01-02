#include "license_generator.hpp"

#include <license/buffer_encryption.hpp>
#include <license/buffer_decryption.hpp>
#include <license/public_key.hpp>
#include <license/hide_string.hpp>

#include <veriparse/logger/logger.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <fstream>


static inline int validate_license(std::string const &license_file)
{
	boost::property_tree::ptree tree;
	boost::property_tree::read_ini(license_file, tree);

	std::string encrypted_key = tree.get<std::string>("key.value");
	std::string encrypted_license = tree.get<std::string>("license.value");
	std::string decrypted_license;

	char *c_public_key = license::create_public_key();
	int rc = license::decrypt_buffer(c_public_key, encrypted_key, encrypted_license, decrypted_license);
	license::shred_string(c_public_key);

	if (rc != 0) {
		return -1;
	}

	return 0;
}

int generate_license(std::string const &rsa_private_key, std::string const &rsa_public_key,
                     std::vector<std::string> const & license_files_list)
{
	// Check extension of all licence files to be crypted
	for(std::string const & license_file : license_files_list) {
		size_t lastdot = license_file.find_last_of(".");

		if(license_file.substr(lastdot + 1) != "txt") {
			LOG_ERROR << license_file << " is not a valid licence file";
			return -1;
		}
	}

	// Encrypt licence files
	for(std::string const & license_file : license_files_list) {
		// Build the output filename
		size_t lastdot = license_file.find_last_of(".");
		std::string output_filename = license_file.substr(0, lastdot);
		output_filename.append(".dat");

		// Read the license file
		std::string license_buffer;
		std::ifstream in(license_file, std::ios::in | std::ios::binary);
		if (in) {
			in.seekg(0, std::ios::end);
			license_buffer.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&license_buffer[0], license_buffer.size());
			in.close();
		} else {
			LOG_ERROR << "Could not read the file to encrypt";
			return -1;
		}

		LOG_INFO << "Encrypting " << license_file << " to " << output_filename;

		std::string encrypted_license_buffer;
		std::string encrypted_key_buffer;
		if (license::encrypt_buffer(rsa_private_key, rsa_public_key,
		                            license_buffer, encrypted_license_buffer,
		                            encrypted_key_buffer) != 0) return -1;

		std::string decrypted_license_buffer;
		if (license::decrypt_buffer(rsa_public_key, encrypted_key_buffer,
		                            encrypted_license_buffer, decrypted_license_buffer) != 0) return -1;

		if (license_buffer.compare(decrypted_license_buffer) != 0) {
			LOG_ERROR << "Encryption failed!";
			return -1;
		}

		// Write encrypted license file to disk
		std::ofstream out(output_filename, std::ios::out | std::ios::binary);
		out << "[key]" << std::endl
		    << "value = " << encrypted_key_buffer << std::endl
		    << std::endl
		    << "[license]" << std::endl
		    << "value = " << encrypted_license_buffer << std::endl;

		if (validate_license(output_filename) == -1) return -1;
	}



	return 0;
}
