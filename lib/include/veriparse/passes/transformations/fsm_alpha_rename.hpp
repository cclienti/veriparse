// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_FSM_ALPHA_RENAME
#define VERIPARSE_PASSES_TRANSFORMATIONS_FSM_ALPHA_RENAME

#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>
#include <veriparse/passes/analysis/unique_declaration.hpp>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

/**
 * @brief Uniquify the shadowed block-scoped declarations of the
 * (* veriparse_fsm *) processes and the tasks they call (ADR-0014 §6.1)
 * — the first stage of the structural pre-lowering.
 *
 * The lowering's substitutions bind by name, so a temporary shadowing a
 * module-level declaration or an enclosing temporary — legal
 * SystemVerilog — used to be refused. This pass renames the inner
 * declaration instead (`x` becomes `x_0`, first free ordinal) and
 * rewrites its references within its lexical scope, order-aware: a
 * reference before the declaration keeps the outer name. In a called
 * task the locals uniquify against the formals and each other, so two
 * sibling cut-spanning statics keep two storages and a local can no
 * longer capture a formal's binding. Renaming happens only on conflict:
 * a conflict-free process is untouched. Alpha-conversion preserves
 * semantics by construction; unmarked processes and uncalled tasks are
 * left alone.
 */
class FsmAlphaRename : public TransformationBase
{
public:
    FsmAlphaRename() = default;
    virtual ~FsmAlphaRename() = default;

private:
    virtual int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

    int rename_module(const AST::Module::Ptr &module);
    int rename_task(const AST::Task::Ptr &task);

    /// One lexical frame: original name -> the name now in force.
    using Frame = std::map<std::string, std::string>;

    int rename_list(const AST::Node::ListPtr &stmts, std::vector<Frame> &frames);
    int rename_refs(const AST::Node::Ptr &node, std::vector<Frame> &frames);
    std::string fresh_name(const std::string &base);
    bool bound_in_frames(const std::string &name, const std::vector<Frame> &frames) const;

    /// Two name sets: the module's own scope — ports and item-level
    /// declarations, the outermost frame for a process's conflicts — and
    /// the deep declaration set plus every name this pass minted, for
    /// freshness. `m_module_wide` is false inside a task, whose locals
    /// conflict only with its formals and each other (the inliner's site
    /// prefixes keep them clear of the module).
    std::set<std::string> m_module_names;
    Analysis::UniqueDeclaration::IdentifierSet m_declared;
    std::set<std::string> m_used;
    /// Every declaration of the current process or task, siblings
    /// included: lexically distinct same-named siblings are legal
    /// SystemVerilog, but the lowering's flat substitutions need them
    /// distinct too.
    std::set<std::string> m_seen;
    bool m_module_wide = true;
};

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse

#endif
