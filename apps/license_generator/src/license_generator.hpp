#ifndef LICENSE_GENERATOR_HPP
#define LICENSE_GENERATOR_HPP

#include <vector>
#include <string>

int generate_license(std::string const &rsa_private_key, std::string const &rsa_public_key,
                     std::vector<std::string> const &license_files_list);

#endif
