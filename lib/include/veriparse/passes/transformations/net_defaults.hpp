// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_NET_DEFAULTS
#define VERIPARSE_PASSES_TRANSFORMATIONS_NET_DEFAULTS

#include <veriparse/AST/nodes.hpp>

#include <cstdint>
#include <string>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

/**
 * @brief Build the bare net node of a `` `default_nettype `` value (IEEE
 * 1800-2017 §22.8): the 12 net keywords map to their node, NONE maps to
 * null (with `` `default_nettype none `` no net may be created
 * implicitly). Name, type and the other declaration fields are left for
 * the caller to fill.
 */
AST::Net::Ptr make_default_nettype_net(AST::Module::Default_nettypeEnum defnt,
                                       const std::string &filename = "", uint32_t line = 0);

/**
 * @brief Build the net of a `` `default_nettype `` value that REPLACES
 * @p src (an implicit net): the node kind changes, everything else
 * carries over. Null when the value is NONE, like the bare overload.
 * This transfer is the one home of the Declaration+Net field list — a
 * field added to the Net schema is carried over here or nowhere.
 */
AST::Net::Ptr make_default_nettype_net(AST::Module::Default_nettypeEnum defnt,
                                       const AST::Net::Ptr &src);

/**
 * @brief Map an interface's `` `default_nettype `` value onto the
 * identical Module enum (the two enums are generated per-node from the
 * same yaml property).
 */
AST::Module::Default_nettypeEnum to_module_nettype(AST::Interface::Default_nettypeEnum nettype);

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse

#endif
