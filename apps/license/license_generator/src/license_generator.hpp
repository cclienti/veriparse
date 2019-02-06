#ifndef LICENSE_LICENSE_GENERATOR_HPP
#define LICENSE_LICENSE_GENERATOR_HPP

#include <vector>
#include <string>

int generate_license(const std::string &rsa_private_key, const std::string &rsa_public_key,
                     const std::string &license_file);

#endif
