#ifndef VERIFLAT_PARAMETERS_OVERLOADING_HPP
#define VERIFLAT_PARAMETERS_OVERLOADING_HPP

#include <veriparse/AST/nodes.hpp>


/**
 * @brief Parse the string to extract the list of overloaded parameters.
 *
 * Success is set to false if something failed during string parsing.
 *
 * @return nullptr if the string is empty, else the list of ParamArg to apply.
 */
Veriparse::AST::ParamArg::ListPtr overload_parameters(const std::string &parameters_string, bool &success);


#endif
