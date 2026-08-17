// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_HIER_CALL_RESOLUTION
#define VERIPARSE_PASSES_TRANSFORMATIONS_HIER_CALL_RESOLUTION

#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/passes/analysis/unique_declaration.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>

#include <list>
#include <map>
#include <set>
#include <string>
#include <utility>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

/**
 * @brief Resolve hierarchical subroutine calls (IEEE 1800-2017 A.8.2,
 * `hierarchical_tf_identifier`).
 *
 * A call whose root names a non-virtual interface port of the module is
 * resolved by splicing the subroutine into the module: the definition is
 * cloned, its free references to interface members become hierarchical
 * references rooted at the port (`req` -> `bus.req`), and the call site
 * becomes an ordinary local call, re-tagged to TaskCall/FunctionCall from
 * the declared kind of the callee. Every other hierarchical call (a module
 * or local interface instance, an indexed or multi-level path) is a hard
 * error.
 */
class HierCallResolution : public TransformationBase
{
public:
    explicit HierCallResolution(const Analysis::Module::InterfacesMap &interfaces_map);

private:
    /**
     * @return zero on success
     */
    virtual int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

    int process_module(const AST::Module::Ptr &module);

    /// Interface name of each root usable in a hierarchical call: the
    /// module's non-virtual interface ports.
    int collect_roots(const AST::Module::Ptr &module);

    /// Depth-first collection of the hierarchical call sites, with parents.
    void collect_calls(const AST::Node::Ptr &node, const AST::Node::Ptr &parent,
                       std::list<std::pair<AST::Call::Ptr, AST::Node::Ptr>> &calls);

    /// Resolve one call site: validate the path, splice the callee, rewrite
    /// and re-tag the call.
    int resolve_call(const AST::Module::Ptr &module, const AST::Call::Ptr &call,
                     const AST::Node::Ptr &parent);

    /// Splice `subroutine` (a Task or Function of `iface`) into the module
    /// for the given root, memoized per (root, name); returns the spliced
    /// declaration through `decl`.
    int splice(const AST::Module::Ptr &module, const std::string &root,
               const AST::Interface::Ptr &iface, const AST::Node::Ptr &subroutine,
               AST::Node::Ptr &decl);

    /// Rewrite the cloned body: free member references gain the root prefix;
    /// anything outside the v1 closure (parameters, typedefs, enum items,
    /// nested calls) is a hard error.
    int rewrite_body(const AST::Node::Ptr &node, const std::string &root,
                     const std::string &iface_name, const std::string &sub_name,
                     const std::set<std::string> &members, const std::set<std::string> &bound,
                     const std::map<std::string, std::string> &kinds);

private:
    const Analysis::Module::InterfacesMap &m_interfaces_map;

    /// root name -> interface name, for the module being processed.
    std::map<std::string, std::string> m_roots;

    /// "<root>.<subroutine>" -> spliced declaration (Task or Function).
    std::map<std::string, AST::Node::Ptr> m_splices;

    /// Declared identifiers of the module being processed, for uniquifying
    /// spliced names.
    Analysis::UniqueDeclaration::IdentifierSet m_declared;
};

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse

#endif
