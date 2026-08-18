// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_SPLICE_UTILS
#define VERIPARSE_PASSES_TRANSFORMATIONS_SPLICE_UTILS

#include <veriparse/AST/nodes.hpp>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{
namespace SpliceUtils
{

/**
 * @brief Freeze a subroutine's effective lifetime before it leaves its
 * declaring scope: spliced into a module it would inherit that module's
 * default instead (IEEE 1800-2017 13.3.1, 13.4.2). `automatic_scope` says
 * whether the declaring scope (package, interface) is declared automatic.
 * One rule for every splice, wherever the subroutine comes from.
 */
void stamp_subroutine_lifetime(const AST::Node::Ptr &item, bool automatic_scope);

} // namespace SpliceUtils
} // namespace Transformations
} // namespace Passes
} // namespace Veriparse

#endif
