// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/passes/transformations/scope_table.hpp>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

void ScopeTable::add_local(const std::string &name, AST::Node::Ptr decl)
{
    m_local[name] = Binding{decl, "", Origin::Local};
}

void ScopeTable::add_explicit_import(const std::string &name, const std::string &package,
                                     AST::Node::Ptr decl)
{
    m_explicit[name] = Binding{decl, package, Origin::ExplicitImport};
}

void ScopeTable::add_wildcard_import(const std::string &name, const std::string &package,
                                     AST::Node::Ptr decl)
{
    m_wildcard[name].push_back(Binding{decl, package, Origin::WildcardImport});
}

const ScopeTable::Binding *ScopeTable::lookup(const std::string &name, bool *ambiguous) const
{
    if(ambiguous) {
        *ambiguous = false;
    }

    // §26.4–26.6 precedence: local > explicit import > wildcard import.
    auto local_it = m_local.find(name);
    if(local_it != m_local.end()) {
        return &local_it->second;
    }

    auto explicit_it = m_explicit.find(name);
    if(explicit_it != m_explicit.end()) {
        return &explicit_it->second;
    }

    auto wildcard_it = m_wildcard.find(name);
    if(wildcard_it != m_wildcard.end()) {
        // A single wildcard source resolves; two or more is the §26.6 on-use
        // ambiguity — report it rather than silently picking one.
        if(wildcard_it->second.size() == 1) {
            return &wildcard_it->second.front();
        }
        if(ambiguous) {
            *ambiguous = true;
        }
        return nullptr;
    }

    return nullptr;
}

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse
