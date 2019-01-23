#ifndef LICENSE_LICENSE_CHECKER_HPP
#define LICENSE_LICENSE_CHECKER_HPP

#include <license/buffer_decryption.hpp>
#include <license/platform_id.hpp>
#include <license/public_key.hpp>
#include <license/hide_string.hpp>

#include <veriparse/logger/logger.hpp>

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/asio.hpp>

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

/**
 * @brief Check if the license file is valid regarding hardware
 * resources.
 *
 * @param license_file
 *
 * @return 1 if the license buffer cannot be decrypted or hardware
 * information cannot be retrieved, > 1 if the decrypted license
 * content is not valid and 0 when the license is valid.
 */
static inline int check_license(const std::string &license_file)
{
	static bool silent = false;

	if (!silent) {
		LOG_INFO << "Using license file '" << license_file << "'";
		silent = true;
	}

	boost::property_tree::ptree tree;
	boost::property_tree::read_ini(license_file, tree);

	std::string info_company, info_expiration;
	std::string info_hostname, info_interface, info_address;
	std::string encrypted_key, encrypted_license;

	try {
		info_company = tree.get<std::string>("information.company");
		info_expiration = tree.get<std::string>("information.expiration");
		info_hostname = tree.get<std::string>("information.hostname");
		info_interface = tree.get<std::string>("information.interface");
		info_address = tree.get<std::string>("information.address");
		encrypted_key = tree.get<std::string>("key.value");
		encrypted_license = tree.get<std::string>("license.value");
	}
	catch (const boost::property_tree::ptree_error &e) {
		LOG_ERROR << "could not read information in " << license_file
		          << " (" << e.what() << ")" << std::endl;
		return 1;
	}

	char *c_public_key = create_public_key();
	std::string public_key(c_public_key);
	shred_string(c_public_key);

	std::string decrypted_license;
	int rc = decrypt_buffer(public_key, encrypted_key, encrypted_license, decrypted_license);
	public_key.erase(public_key.begin(), public_key.end());
	if (rc != 0) {
		return 1;
	}
	std::stringstream ss(decrypted_license);

	//----------------------------------
	// Check Company
	//----------------------------------
	std::string ref_company;
	ss >> ref_company;

	if (ref_company != info_company) {
		LOG_ERROR << "Company has been modified in the license file " << license_file;
		return 2;
	}

	//----------------------------------
	// Check Expiration
	//----------------------------------
	std::string ref_expiration;
	ss >> ref_expiration;

	if (ref_expiration != info_expiration) {
		LOG_ERROR << "Expiration has been modified in the license file " << license_file;
		return 3;
	}

	auto info_expiration_date = boost::gregorian::from_simple_string(ref_expiration);
	boost::gregorian::date today(boost::gregorian::day_clock::local_day());
	auto diff_expiration = (info_expiration_date - today).days();
	if (diff_expiration < 0) {
		LOG_ERROR << "License expired " << -1 * diff_expiration << " days ago";
		return 3;
	}
	else if (diff_expiration < 32) {
		static bool warn = true;
		if (warn == true) {
			warn = false;
			LOG_WARNING << "License will expire in " << diff_expiration << " day(s)";
		}
	}

	//----------------------------------
	// Check Hostname
	//----------------------------------
	std::string ref_hostname;
	ss >> ref_hostname;

	if (ref_hostname != info_hostname ) {
		LOG_ERROR << "Hostname has been modified in the license file " << license_file;
		return 4;
	}

	std::string hostname = boost::asio::ip::host_name();
	if (ref_hostname != hostname) {
		LOG_ERROR << "Hostname does not correspond to that declared in the license file " << license_file;
		return 4;
	}

	//----------------------------------
	// Check Interface
	//----------------------------------
	std::string ref_interface;
	ss >> ref_interface;

	if (ref_interface != info_interface ) {
		LOG_ERROR << "Interface has been modified in the license file " << license_file;
		return 5;
	}

	//----------------------------------
	// Check MAC Address
	//----------------------------------
	std::string ref_address;
	ss >> ref_address;

	if (ref_address != info_address) {
		LOG_ERROR << "MAC address has been modified in the license file " << license_file;
		return 6;
	}

	char mac_address_c[18];
	if(get_mac_address(mac_address_c, info_interface)) {
		LOG_ERROR << "Cannot read mac address of " << info_interface;
		return 1;
	}

	if (ref_address != mac_address_c) {
		LOG_ERROR << "MAC address of " << ref_interface << " "
		          << "does not correspond to that declared in the license file " << license_file;
		return 6;
	}


	return 0;
}


/**
 * @brief Check if the license file is valid regarding hardware
 * resources.
 *
 * The license file is read from the environment variable
 * VERIPARSE_LICENSE_FILE.
 *
 * @return 1 if the license buffer cannot be decrypted or hardware
 * information cannot be retrieved, > 1 if the decrypted license
 * content is not valid and 0 when the license is valid.
 */
static inline int check_license()
{
#ifndef VERIPARSE_NO_LICENSE
	const char *license_file = std::getenv("VERIPARSE_LICENSE_FILE");
	if (license_file == nullptr) {
		LOG_ERROR << "VERIPARSE_LICENSE_FILE environment variable not set or empty";
		return 1;
	}

	return check_license(license_file);
#endif

	return 0;
}


}

#endif
