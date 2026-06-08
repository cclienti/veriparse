// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_AST_NODE_POINTERS_HPP
#define VERIPARSE_AST_NODE_POINTERS_HPP

#include <memory>
#include <list>

namespace Veriparse
{
namespace AST
{

template <class T> struct NodePointers
{
    typedef std::shared_ptr<T> Ptr;
    typedef std::list<Ptr> List;
    typedef std::shared_ptr<List> ListPtr;
};

} // namespace AST
} // namespace Veriparse

#endif
