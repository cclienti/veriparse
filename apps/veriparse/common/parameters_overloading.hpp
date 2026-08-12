// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_APPS_PARAMETERS_OVERLOADING_HPP
#define VERIPARSE_APPS_PARAMETERS_OVERLOADING_HPP

#include <veriparse/AST/nodes.hpp>

/**
 * @brief Parse the string to extract the list of overloaded parameters.
 *
 * Success is set to false if something failed during string parsing.
 *
 * @return nullptr if the string is empty, else the list of ParamArg to apply.
 */
Veriparse::AST::ParamArg::ListPtr overload_parameters(const std::string &parameters_string,
                                                      bool &success);

/**
 * @brief Check that every overloaded/kept parameter names one the top
 * module declares — a typo'd name must not compile a silently
 * default-parameterized design.
 *
 * @return true when all names resolve.
 */
bool check_parameter_names(const Veriparse::AST::ParamArg::ListPtr &param_args,
                           const Veriparse::AST::Node::Ptr &module);

#endif
