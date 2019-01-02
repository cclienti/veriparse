#ifndef LICENSE_LICENSE_CHECKER_HPP
#define LICENSE_LICENSE_CHECKER_HPP

#include <private_lib/buffer_decryption.hpp>
#include <private_lib/board_spec.hpp>
#include <private_lib/public_key.hpp>
#include <private_lib/hide_string.hpp>

#include <veriparse/logger/logger.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/foreach.hpp>

#include <set>
#include <exception>
#include <iostream>
#include <algorithm>
#include <string>

#include <cstdint>
#include <cstdio>
#include <cstdlib>


namespace license
{
DEFINE_HIDDEN_STRING(network_interface_string, 7, ('e')('t')('h')('0'))

/**
 * Return 1 if the license buffer cannot be decrypted.
 * Return > 1 if the decrypted license content is not valid
 * Return 0 is the license is valid
 */
static inline int check_license(std::string const &license_file)
{
	boost::property_tree::ptree tree;
	boost::property_tree::read_ini(license_file, tree);

	std::string encrypted_key = tree.get<std::string>("key.value");
	std::string encrypted_license = tree.get<std::string>("license.value");
	std::string decrypted_license;

	char *c_public_key = create_public_key();
	std::string public_key(c_public_key);
	shred_string(c_public_key);

	int rc = decrypt_buffer(public_key, encrypted_key, encrypted_license, decrypted_license);
	public_key.erase(public_key.begin(), public_key.end());
	if (rc != 0) {
		return 1;
	}

	std::stringstream ss(decrypted_license);

	uint64_t ref_serial_number;
	ss >> std::hex >> ref_serial_number;

	std::string ref_mac_address;
	ss >> ref_mac_address;

	std::string ref_ip_address;
	ss >> ref_ip_address;

	uint64_t serial_number;
	get_serial_number(serial_number);

	char *c_network_interface_string = create_network_interface_string();

	char mac_address_c[18];
	get_mac_address(mac_address_c, c_network_interface_string);
	std::string mac_address = mac_address_c;

	char ip_address_c[16];
	get_ip_address(ip_address_c, c_network_interface_string);
	std::string ip_address = ip_address_c;

	shred_string(c_network_interface_string);

	if (ref_ip_address != ip_address) return 4;
	if (ref_mac_address != mac_address) return 3;
	if (ref_serial_number != serial_number) return 2;

	return 0;
}


}

#endif
