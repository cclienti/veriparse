#include "license_generator.hpp"

#include <license/buffer_encryption.hpp>
#include <license/buffer_decryption.hpp>
#include <license/public_key.hpp>
#include <license/hide_string.hpp>

#include <veriparse/logger/logger.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <fstream>


static inline int validate_license(const std::string &license_file)
{
	boost::property_tree::ptree tree;
	boost::property_tree::read_ini(license_file, tree);

	std::string info_company, info_experition;
	std::string info_hostname, info_interface, info_address;
	std::string encrypted_key, encrypted_license;

	try {
		info_company = tree.get<std::string>("information.company");
		info_experition = tree.get<std::string>("information.expiration");
		info_hostname = tree.get<std::string>("information.hostname");
		info_interface = tree.get<std::string>("information.interface");
		info_address = tree.get<std::string>("information.address");
		encrypted_key = tree.get<std::string>("key.value");
		encrypted_license = tree.get<std::string>("license.value");
	}
	catch (const boost::property_tree::ptree_error &e) {
		LOG_ERROR << "could not read information in " << license_file
		          << " (" << e.what() << ")" << std::endl;
	}

	std::string decrypted_license;

	char *c_public_key = license::create_public_key();
	int rc = license::decrypt_buffer(c_public_key, encrypted_key, encrypted_license, decrypted_license);
	license::shred_string(c_public_key);

	if (rc != 0) {
		return -1;
	}

	return 0;
}

int generate_license(const std::string &rsa_private_key, const std::string &rsa_public_key,
                     const std::string &license_file)
{
	size_t lastdot;

	// Check extension of file to encrypt.
	lastdot = license_file.find_last_of(".");
	if(license_file.substr(lastdot + 1) != "ini") {
		LOG_ERROR << license_file << " is not a valid licence file";
		return 1;
	}

	// Build the output filename.
	lastdot = license_file.find_last_of(".");
	std::string output_filename = license_file.substr(0, lastdot);
	output_filename.append(".dat");

	// Read the license file
	boost::property_tree::ptree tree;
	boost::property_tree::read_ini(license_file, tree);

	std::string info_company, info_expiration;
	std::string info_hostname, info_interface, info_address;

	try {
		info_company    = tree.get<std::string>("information.company");
		info_expiration = tree.get<std::string>("information.expiration");
		info_hostname   = tree.get<std::string>("information.hostname");
		info_interface  = tree.get<std::string>("information.interface");
		info_address    = tree.get<std::string>("information.address");
	}
	catch (const boost::property_tree::ptree_error &e) {
		LOG_ERROR << "could not read information in " << license_file
		          << " (" << e.what() << ")" << std::endl;
		return 1;
	}

	std::string license_buffer;
	license_buffer += info_company + '\n';
	license_buffer += info_expiration + '\n';
	license_buffer += info_hostname + '\n';
	license_buffer += info_interface + '\n';
	license_buffer += info_address + '\n';

	LOG_INFO << "Encrypting " << license_file << " to " << output_filename;

	std::string encrypted_license_buffer;
	std::string encrypted_key_buffer;
	if (license::encrypt_buffer(rsa_private_key, rsa_public_key,
	                            license_buffer, encrypted_license_buffer,
	                            encrypted_key_buffer) != 0) return 1;

	std::string decrypted_license_buffer;
	if (license::decrypt_buffer(rsa_public_key, encrypted_key_buffer,
	                            encrypted_license_buffer, decrypted_license_buffer) != 0) return -1;

	if (license_buffer.compare(decrypted_license_buffer) != 0) {
		LOG_ERROR << "Encryption failed!";
		return 1;
	}

	// Write encrypted license file to disk
	std::ofstream out(output_filename, std::ios::out | std::ios::binary);
	out << "[information]"
	    << '\n' << "company = " << info_company
	    << '\n' << "expiration = " << info_expiration
	    << '\n' << "hostname = " << info_hostname
	    << '\n' << "interface = " << info_interface
	    << '\n' << "address = " << info_address
	    << '\n'
	    << '\n' << "[key]"
	    << '\n' << "value = " << encrypted_key_buffer
	    << '\n'
	    << '\n' << "[license]"
	    << '\n' << "value = " << encrypted_license_buffer
	    << std::endl;

	if (validate_license(output_filename) == -1) return 1;

	return 0;
}
