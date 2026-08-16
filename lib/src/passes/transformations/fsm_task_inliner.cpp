// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/passes/transformations/fsm_task_inliner.hpp>
#include "implicit_fsm_detail.hpp"

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

using namespace FsmDetail;

namespace
{

/// Rename identifiers everywhere, assignment targets included — the
/// inlined clone renames its writes too (§7.4). The engine is
/// ASTReplace::replace_identifier, which already skips hierarchical
/// leaves and shadowing subroutine scopes; the roots here are blocks,
/// never a bare identifier.
void rename_into(const AST::Node::Ptr &node, const std::map<std::string, AST::Node::Ptr> &map)
{
    ASTReplace::replace_identifier(node, map);
}

std::string upper_of(const std::string &text)
{
    std::string out = text;
    for(auto &c : out) {
        c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    }
    return out;
}

/// Names assigned — with either flavor — on EVERY runtime path through
/// the statement: must_writes' discipline over both substitution kinds,
/// which is what an output formal's copy-out demands (the §6 register
/// analysis keeps its nonblocking-only view).
void all_paths_writes(const AST::Node::Ptr &node, std::set<std::string> &writes)
{
    if(!node) {
        return;
    }
    switch(node->get_node_type()) {
    case AST::NodeType::Block: {
        const auto &stmts = AST::cast_to<AST::Block>(node)->get_statements();
        if(stmts) {
            for(const auto &stmt : *stmts) {
                all_paths_writes(stmt, writes);
            }
        }
        return;
    }
    case AST::NodeType::Pragmalist: {
        const auto &stmts = AST::cast_to<AST::Pragmalist>(node)->get_statements();
        if(stmts) {
            for(const auto &stmt : *stmts) {
                all_paths_writes(stmt, writes);
            }
        }
        return;
    }
    case AST::NodeType::SingleStatement:
        all_paths_writes(AST::cast_to<AST::SingleStatement>(node)->get_statement(), writes);
        return;
    case AST::NodeType::NonblockingSubstitution:
        writes.insert(nba_target(AST::cast_to<AST::NonblockingSubstitution>(node)));
        return;
    case AST::NodeType::BlockingSubstitution:
        writes.insert(lvalue_target(AST::cast_to<AST::BlockingSubstitution>(node)->get_left()));
        return;
    case AST::NodeType::IfStatement: {
        const auto &ifs = AST::cast_to<AST::IfStatement>(node);
        if(!ifs->get_true_statement() || !ifs->get_false_statement()) {
            return; // a missing arm guarantees nothing
        }
        std::set<std::string> t;
        std::set<std::string> e;
        all_paths_writes(ifs->get_true_statement(), t);
        all_paths_writes(ifs->get_false_statement(), e);
        for(const auto &name : t) {
            if(e.count(name)) {
                writes.insert(name);
            }
        }
        return;
    }
    default:
        return; // loops and cases guarantee nothing here
    }
}

AST::BlockingSubstitution::Ptr make_ba(const std::string &target, const AST::Node::Ptr &rhs,
                                       const std::string &fn, int ln)
{
    auto lvalue = std::make_shared<AST::Lvalue>(fn, ln);
    lvalue->set_var(AST::to_node(make_id(target, fn, ln)));
    auto rvalue = std::make_shared<AST::Rvalue>(fn, ln);
    rvalue->set_var(rhs);
    auto ba = std::make_shared<AST::BlockingSubstitution>(fn, ln);
    ba->set_left(lvalue);
    ba->set_right(rvalue);
    return ba;
}
/// Whether a Return sits anywhere below `node`. A task call's own returns
/// are its expansion's business, but a Call child holds only actuals, so
/// the plain walk cannot meet one.
bool contains_return(const AST::Node::Ptr &node)
{
    if(!node) {
        return false;
    }
    if(node->is_node_type(AST::NodeType::Return)) {
        return true;
    }
    const auto &children = node->get_children();
    if(!children) {
        return false;
    }
    for(const auto &child : *children) {
        if(contains_return(child)) {
            return true;
        }
    }
    return false;
}
int lower_return_list(const AST::Node::ListPtr &stmts, const std::string &task_name, bool &always);
/// A branch position has no statement list to thread a continuation into,
/// so a branch holding a return must return on EVERY path through it —
/// then the jump is the branch's end and the structure stays flag-free. A
/// partial return here would need exit state the model does not carry.
int lower_return_branch(AST::Node::Ptr &branch, const std::string &task_name, bool &always)
{
    always = false;
    if(!branch || !contains_return(branch)) {
        return 0;
    }
    if(branch->is_node_type(AST::NodeType::SingleStatement)) {
        // The grammar's single-statement wrapper: the payload is the shape.
        AST::Node::Ptr inner = AST::cast_to<AST::SingleStatement>(branch)->get_statement();
        if(lower_return_branch(inner, task_name, always)) {
            return 1;
        }
        branch = inner;
        return 0;
    }
    if(branch->is_node_type(AST::NodeType::Return)) {
        if(AST::cast_to<AST::Return>(branch)->get_value()) {
            LOG_ERROR_N(branch) << "task '" << task_name
                                << "': a return value is not allowed within a task "
                                << "(IEEE 1800-2017 §13.3)";
            return 1;
        }
        branch = AST::to_node(std::make_shared<AST::Block>(
            std::make_shared<AST::Node::List>(), "", branch->get_filename(), branch->get_line()));
        always = true;
        return 0;
    }
    if(branch->is_node_type(AST::NodeType::Block)) {
        if(lower_return_list(AST::cast_to<AST::Block>(branch)->get_statements(), task_name,
                             always)) {
            return 1;
        }
        if(!always) {
            LOG_ERROR_N(branch)
                << "task '" << task_name << "': a return conditional within its branch has no "
                << "structural exit — make the branch return on every path, or restructure";
            return 1;
        }
        return 0;
    }
    if(branch->is_node_type(AST::NodeType::IfStatement)) {
        const auto &ifs = AST::cast_to<AST::IfStatement>(branch);
        AST::Node::Ptr t = ifs->get_true_statement();
        AST::Node::Ptr e = ifs->get_false_statement();
        bool tret = false;
        bool eret = false;
        if(lower_return_branch(t, task_name, tret) || lower_return_branch(e, task_name, eret)) {
            return 1;
        }
        ifs->set_true_statement(t);
        ifs->set_false_statement(e);
        if(!tret || !eret) {
            LOG_ERROR_N(branch)
                << "task '" << task_name << "': a return conditional within its branch has no "
                << "structural exit — make the branch return on every path, or restructure";
            return 1;
        }
        always = true;
        return 0;
    }
    LOG_ERROR_N(branch) << "task '" << task_name
                        << "': a return inside a loop or case arm has no structural exit — "
                        << "restructure with break or a guard";
    return 1;
}
/// §13.3, measured: a task return jumps to the end of the body, and the
/// copy-out still runs — it sits past the body, where every lowered path
/// falls. The lowering restructures the list so each return path reaches
/// the list's end: what follows an always-returning statement is
/// unreachable and dropped, statements after an if with one
/// always-returning branch move into the other branch, and a return
/// escaping an unnamed block threads the block's continuation inward. On
/// exit `always` says no path falls past the list's own end.
int lower_return_list(const AST::Node::ListPtr &stmts, const std::string &task_name, bool &always)
{
    always = false;
    if(!stmts) {
        return 0;
    }
    for(auto it = stmts->begin(); it != stmts->end(); ++it) {
        AST::Node::Ptr s = *it;
        if(!s || !contains_return(s)) {
            continue;
        }
        if(s->is_node_type(AST::NodeType::SingleStatement)) {
            // The grammar's single-statement wrapper: the payload is the
            // shape, and the rewrite below replaces the whole statement.
            s = AST::cast_to<AST::SingleStatement>(s)->get_statement();
            *it = s;
        }

        if(s->is_node_type(AST::NodeType::Return)) {
            if(AST::cast_to<AST::Return>(s)->get_value()) {
                LOG_ERROR_N(s) << "task '" << task_name
                               << "': a return value is not allowed within a task "
                               << "(IEEE 1800-2017 §13.3)";
                return 1;
            }
            stmts->erase(it, stmts->end()); // the return, and its unreachable tail
            always = true;
            return 0;
        }

        if(s->is_node_type(AST::NodeType::IfStatement)) {
            const auto &ifs = AST::cast_to<AST::IfStatement>(s);
            AST::Node::Ptr t = ifs->get_true_statement();
            AST::Node::Ptr e = ifs->get_false_statement();
            bool tret = false;
            bool eret = false;
            if(lower_return_branch(t, task_name, tret) || lower_return_branch(e, task_name, eret)) {
                return 1;
            }
            ifs->set_true_statement(t);
            ifs->set_false_statement(e);
            if(tret && eret) {
                stmts->erase(std::next(it), stmts->end()); // both paths return
                always = true;
                return 0;
            }
            if(tret || eret) {
                // One branch jumps to the end; the other and the statements
                // after the if are the same fall-through — they become that
                // branch's tail, lowered in turn for any further return.
                AST::Node::Ptr &open = tret ? e : t;
                AST::Block::Ptr host;
                if(open && open->is_node_type(AST::NodeType::Block) &&
                   AST::cast_to<AST::Block>(open)->get_scope().empty()) {
                    host = AST::cast_to<AST::Block>(open);
                } else {
                    auto inner = std::make_shared<AST::Node::List>();
                    if(open) {
                        inner->push_back(open);
                    }
                    host =
                        std::make_shared<AST::Block>(inner, "", s->get_filename(), s->get_line());
                }
                for(auto rit = std::next(it); rit != stmts->end(); ++rit) {
                    host->get_statements()->push_back(*rit);
                }
                stmts->erase(std::next(it), stmts->end());
                open = AST::to_node(host);
                ifs->set_true_statement(t);
                ifs->set_false_statement(e);
                return lower_return_list(host->get_statements(), task_name, always);
            }
            continue; // both branches lowered internally, both fall through
        }

        if(s->is_node_type(AST::NodeType::Block)) {
            const auto &blk = AST::cast_to<AST::Block>(s);
            if(blk->get_scope().empty()) {
                // Unnamed: the continuation threads inward, one motion.
                for(auto rit = std::next(it); rit != stmts->end(); ++rit) {
                    blk->get_statements()->push_back(*rit);
                }
                stmts->erase(std::next(it), stmts->end());
                return lower_return_list(blk->get_statements(), task_name, always);
            }
            // Named: the §10.1 label scopes its own statements — threading
            // the continuation in would rename the caller's states — so
            // only a whole-block return composes.
            bool bret = false;
            if(lower_return_list(blk->get_statements(), task_name, bret)) {
                return 1;
            }
            if(!bret) {
                LOG_ERROR_N(s) << "task '" << task_name
                               << "': a return conditional within named block '" << blk->get_scope()
                               << "' has no structural exit — make the block return on "
                               << "every path, or drop the label";
                return 1;
            }
            stmts->erase(std::next(it), stmts->end());
            always = true;
            return 0;
        }

        LOG_ERROR_N(s) << "task '" << task_name
                       << "': a return inside a loop or case arm has no structural exit — "
                       << "restructure with break or a guard";
        return 1;
    }
    return 0;
}

} // namespace

int FsmTaskInliner::process(AST::Node::Ptr node, AST::Node::Ptr parent)
{
    if(!node) {
        return 0;
    }
    if(!node->is_node_type(AST::NodeType::Module)) {
        return recurse_in_childs(node);
    }
    return inline_module(AST::cast_to<AST::Module>(node));
}

int FsmTaskInliner::inline_module(const AST::Module::Ptr &module)
{
    m_tasks.clear();
    m_task_ordinal.clear();
    m_static_hoist.clear();
    m_inlined.clear();
    m_declared.clear();
    m_expanded.clear();
    m_module = module;
    m_iface_ports = collect_iface_ports(module);
    m_pragmalist = nullptr;

    {
        const auto &task_nodes = Analysis::Module::get_task_nodes(AST::to_node(module));
        if(task_nodes) {
            for(const auto &task : *task_nodes) {
                m_tasks[task->get_name()] = task;
            }
        }
    }
    if(m_tasks.empty()) {
        return 0;
    }
    if(Analysis::UniqueDeclaration::analyze(AST::to_node(module), m_declared)) {
        LOG_ERROR_N(module) << "failed to analyze declarations";
        return 1;
    }

    const auto &items = module->get_items();
    if(!items) {
        return 0;
    }
    // The marked initial processes: inlining rewrites their statements
    // in place, never the item list collect_marked iterated.
    for(const auto &pair : collect_marked(module)) {
        if(!pair.second->is_node_type(AST::NodeType::Initial)) {
            continue; // the elaborator diagnoses a mark on anything else
        }
        m_pragmalist = pair.first;
        if(inline_process(AST::cast_to<AST::Initial>(pair.second))) {
            return 1;
        }
    }

    // §7.4: a task definition with no remaining call site is dropped —
    // kept, it would be dead code still writing the machine's registers.
    // One still called (an unmarked process) survives verbatim. To a
    // fixpoint: dropping a task removes the call sites its own body held,
    // which can strand its callees in turn.
    if(!m_inlined.empty()) {
        for(bool removed = true; removed;) {
            removed = false;
            std::set<std::string> called;
            collect_call_names(AST::to_node(module), called);
            const auto &live = module->get_items();
            if(!live) {
                break;
            }
            for(auto it = live->begin(); it != live->end();) {
                if((*it)->is_node_type(AST::NodeType::Task) &&
                   m_inlined.count(AST::cast_to<AST::Task>(*it)->get_name()) &&
                   !called.count(AST::cast_to<AST::Task>(*it)->get_name())) {
                    it = live->erase(it);
                    removed = true;
                    continue;
                }
                ++it;
            }
        }
    }
    return 0;
}

int FsmTaskInliner::hoist_declaration(const std::string &name, const AST::Node::Ptr &type,
                                      const std::string &fn, int ln)
{
    if(m_declared.count(name)) {
        // Re-hoist of a static local from a later call site: the single
        // shared register already exists.
        if(m_static_hoist.count(name)) {
            return 0;
        }
        LOG_ERROR << "generated declaration '" << name
                  << "' collides with an existing one; rename the task, argument or local";
        return 1;
    }
    auto reg = std::make_shared<AST::Var>(fn, ln);
    reg->set_name(name);
    if(type) {
        reg->set_type(AST::cast_to<AST::DataType>(type->clone()));
    }
    const auto &items = m_module->get_items();
    for(auto it = items->begin(); it != items->end(); ++it) {
        if(it->get() == static_cast<AST::Node *>(m_pragmalist.get())) {
            items->insert(it, AST::to_node(reg));
            m_declared.insert(name);
            return 0;
        }
    }
    items->push_back(AST::to_node(reg));
    m_declared.insert(name);
    return 0;
}

/// An input formal (§13.3 copy-in): a '='-written one is a §6.1
/// temporary seeded from the actual; a constant actual substitutes
/// unless anything — the body or a nested copy-out — writes the formal,
/// which then takes the capture register like any other.
int FsmTaskInliner::lower_input_formal(FormalContext &ctx)
{
    const std::string &fn = ctx.call->get_filename();
    const int ln = ctx.call->get_line();
    if(ctx.ba_written) {
        // The local-copy idiom: a '='-written formal is a §6.1 temporary
        // declared in the task block — legal only where no cut spans,
        // which §6 checks on the declaration — seeded from the actual.
        auto tmp = std::make_shared<AST::Var>(fn, ln);
        tmp->set_name(ctx.rname);
        if(ctx.formal->get_type()) {
            tmp->set_type(AST::cast_to<AST::DataType>(ctx.formal->get_type()->clone()));
        }
        ctx.head->push_back(AST::to_node(tmp));
        ctx.head->push_back(AST::to_node(make_ba(ctx.rname, ctx.actual, fn, ln)));
        (*ctx.subst)[ctx.fname] = AST::to_node(make_id(ctx.rname, fn, ln));
        return 0;
    }
    if(ctx.actual->is_node_category(AST::NodeType::Constant) && !ctx.nba_written &&
       !ctx.induced_written) {
        // A written formal is storage, never a substitutable constant —
        // the capture register below takes it.
        (*ctx.subst)[ctx.fname] = ctx.actual;
        return 0;
    }
    if(hoist_declaration(ctx.rname, AST::to_node(ctx.formal->get_type()), fn, ln)) {
        return 1;
    }
    ctx.head->push_back(make_induced_marker(make_nba(ctx.rname, ctx.actual, fn, ln), true, fn, ln));
    (*ctx.subst)[ctx.fname] = AST::to_node(make_id(ctx.rname, fn, ln));
    return 0;
}

/// An output or inout formal: private storage with one copy-out
/// committed at the return (§13.3, measured) — a register when the body
/// writes '<=', a §6.1 temporary when it writes '='; inout adds the
/// copy-in, output the assign-before-read obligation.
int FsmTaskInliner::lower_copyout_formal(FormalContext &ctx)
{
    const std::string &fn = ctx.call->get_filename();
    const int ln = ctx.call->get_line();
    const bool is_inout = ctx.formal->get_direction() == AST::Arg::DirectionEnum::INOUT;
    if(!ctx.actual->is_node_type(AST::NodeType::Identifier)) {
        LOG_ERROR_N(ctx.call) << "task '" << ctx.task_name << "': the actual for " << "'"
                              << ctx.fname
                              << "' must be a plain register of the process — copy-out has "
                              << "one storage to write";
        return 1;
    }
    const auto &actual_id = AST::cast_to<AST::Identifier>(ctx.actual);
    if(actual_id->get_hier() && !hier_is_iface_member(actual_id, m_iface_ports)) {
        LOG_ERROR_N(ctx.call) << "task '" << ctx.task_name << "': the actual for '" << ctx.fname
                              << "' is '" << Analysis::Statement::identifier_key(actual_id)
                              << "' — a hierarchical actual must be a member of an interface "
                              << "port of this module";
        return 1;
    }
    if(ctx.ba_written) {
        // A '='-written output/inout formal is the same §6.1 temporary,
        // copied out to the actual at return — the no-wait helper shape a
        // package task takes when its result must land the same cycle.
        auto tmp = std::make_shared<AST::Var>(fn, ln);
        tmp->set_name(ctx.rname);
        if(ctx.formal->get_type()) {
            tmp->set_type(AST::cast_to<AST::DataType>(ctx.formal->get_type()->clone()));
        }
        ctx.head->push_back(AST::to_node(tmp));
        if(is_inout) {
            ctx.head->push_back(AST::to_node(make_ba(ctx.rname, ctx.actual, fn, ln)));
        } else {
            ctx.check_read_first->push_back(ctx.rname);
        }
    } else {
        if(hoist_declaration(ctx.rname, AST::to_node(ctx.formal->get_type()), fn, ln)) {
            return 1;
        }
        if(is_inout) {
            ctx.head->push_back(
                make_induced_marker(make_nba(ctx.rname, ctx.actual, fn, ln), true, fn, ln));
        } else {
            ctx.check_read_first->push_back(ctx.rname);
        }
    }
    ctx.tail->push_back(make_induced_marker(
        make_nba_to(ctx.actual->clone(), AST::to_node(make_id(ctx.rname, fn, ln)), fn, ln), false,
        fn, ln));
    (*ctx.subst)[ctx.fname] = AST::to_node(make_id(ctx.rname, fn, ln));
    return 0;
}

/// A ref or const ref formal: pure substitution to a variable actual
/// (§13.5.2, measured aliasing) — no local, no copy anything — with the
/// refusals that boundary carries: automatic lifetime only, no '<='
/// through the alias, no write of any kind through const ref.
int FsmTaskInliner::lower_ref_formal(FormalContext &ctx)
{
    const bool read_only = ctx.formal->get_direction() == AST::Arg::DirectionEnum::CONST_REF;
    // §7.4, measured: a ref is a true alias for blocking writes on an
    // automatic task — '<=' through it is illegal IEEE (no nonblocking
    // assignment to an automatic), and §13.5.2 ties ref to automatic
    // lifetime.
    if(!ctx.task_automatic) {
        LOG_ERROR_N(ctx.call) << "task '" << ctx.task_name
                              << "': a ref argument needs 'task automatic' "
                              << "(IEEE 1800-2017 §13.5.2)";
        return 1;
    }
    if(!ctx.actual->is_node_type(AST::NodeType::Identifier)) {
        LOG_ERROR_N(ctx.call) << "task '" << ctx.task_name << "': the actual for ref '" << ctx.fname
                              << "' must be a plain signal of the process";
        return 1;
    }
    const auto &actual_id = AST::cast_to<AST::Identifier>(ctx.actual);
    const std::string aname = Analysis::Statement::identifier_key(actual_id);
    if(actual_id->get_hier()) {
        // An interface member is a variable (§25.3) — the net refusal
        // below cannot apply, and any other scope is unreachable.
        if(!hier_is_iface_member(actual_id, m_iface_ports)) {
            LOG_ERROR_N(ctx.call) << "task '" << ctx.task_name << "': the actual for ref '"
                                  << ctx.fname << "' is '" << aname << "' — a hierarchical "
                                  << "actual must be a member of an interface port of this "
                                  << "module";
            return 1;
        }
    } else if(is_net_signal(m_module, aname)) {
        log_net_actual(AST::to_node(ctx.call), "task", ctx.task_name, ctx.fname, aname);
        return 1;
    }
    if(ctx.nba_written) {
        LOG_ERROR_N(ctx.call) << "task '" << ctx.task_name << "': '<=' through ref '" << ctx.fname
                              << "' — no nonblocking assignment to an automatic (IEEE "
                              << "1800-2017 §10.4.2, §13.5.2); alias with '=', or capture "
                              << "with 'input' and commit registers directly";
        return 1;
    }
    if(read_only && (ctx.ba_written || ctx.induced_written)) {
        LOG_ERROR_N(ctx.call) << "task '" << ctx.task_name << "': write through const "
                              << "ref '" << ctx.fname << "' — a nested call's copy-out "
                              << "included (IEEE 1800-2017 §13.5.2)";
        return 1;
    }
    (*ctx.subst)[ctx.fname] = ctx.actual;
    return 0;
}

AST::Node::Ptr FsmTaskInliner::expand_call(const AST::Call::Ptr &call,
                                           std::set<std::string> &visiting)
{
    const std::string &name = call->get_name();
    const auto &task = m_tasks.at(name);
    const std::string &fn = call->get_filename();
    const int ln = call->get_line();

    if(visiting.count(name)) {
        LOG_ERROR_N(call) << "recursive task call: '" << name << "' reaches itself — "
                          << "inlining is the model, and there is no stack to give "
                          << "recursion meaning";
        return nullptr;
    }

    const unsigned int ordinal = m_task_ordinal[name]++;
    const std::string site = name + "_" + std::to_string(ordinal);

    // Clone the body.
    const auto &stmts = std::make_shared<AST::Node::List>();
    if(task->get_statements()) {
        for(const auto &stmt : *task->get_statements()) {
            stmts->push_back(stmt->clone());
        }
    }
    auto block = std::make_shared<AST::Block>(stmts, upper_of(site), fn, ln);
    const auto &block_node = AST::to_node(block);

    // §13.3: a return jumps to the body's end — restructure before anything
    // else reads the shape, so the copy-out spliced past the body catches
    // every early path (measured: an early return still copies out).
    {
        bool returns_always = false;
        if(lower_return_list(stmts, name, returns_always)) {
            return nullptr;
        }
    }

    // ---- Depth-first: tasks calling tasks, expanded BEFORE this call's
    // own analysis, so a nested expansion's writes into this task's
    // formals are visible to the classification below — its induced
    // commits excluded, its user writes counted. Recursion is a cycle.
    visiting.insert(name);
    if(inline_calls_in(block_node, visiting)) {
        visiting.erase(name);
        return nullptr;
    }
    visiting.erase(name);

    // The body's written names by flavor, computed once. '<=' matters to
    // §6.21 — no nonblocking assignment to automatic storage, so an
    // automatic task's formals and locals may not take it (vsim enforces
    // it, and a package task is automatic whenever it waits, since its
    // clock arrives by ref); '=' picks the temporary lowering for a formal.
    std::set<std::string> body_nba_targets;
    std::set<std::string> body_ba_targets;
    std::set<std::string> body_induced_targets;
    {
        std::function<void(const AST::Node::Ptr &)> scan = [&](const AST::Node::Ptr &n) {
            if(!n) {
                return;
            }
            if(induced_marker_kind(n)) {
                // A nested expansion's induced commit: not the body's own
                // write — §6.21 and the flavor rules don't see it — but a
                // copy-out INTO one of this task's formals still makes that
                // formal storage.
                const auto &nba = induced_marker_nba(n);
                if(nba) {
                    body_induced_targets.insert(nba_target(nba));
                }
                return;
            }
            if(n->is_node_type(AST::NodeType::NonblockingSubstitution)) {
                body_nba_targets.insert(nba_target(AST::cast_to<AST::NonblockingSubstitution>(n)));
            } else if(n->is_node_type(AST::NodeType::BlockingSubstitution)) {
                body_ba_targets.insert(
                    lvalue_target(AST::cast_to<AST::BlockingSubstitution>(n)->get_left()));
            }
            const auto &kids = n->get_children();
            if(kids) {
                for(const auto &c : *kids) {
                    scan(c);
                }
            }
        };
        scan(block_node);
    }
    const bool task_automatic = task->get_lifetime() == AST::Task::LifetimeEnum::AUTOMATIC;

    // ---- Formals (§7.4, §13.3): arity, then per-direction lowering.
    std::map<std::string, AST::Node::Ptr> subst;
    std::vector<AST::Node::Ptr> head, tail;
    std::vector<std::string> check_read_first;
    const auto &formals = task->get_args();
    const std::size_t formal_count = formals ? formals->size() : 0;
    std::vector<AST::Node::Ptr> given;
    if(call->get_args()) {
        for(const auto &a : *call->get_args()) {
            given.push_back(a);
        }
    }
    if(given.size() > formal_count) {
        LOG_ERROR_N(call) << "task '" << name << "' takes " << formal_count << " argument"
                          << (formal_count == 1 ? "" : "s") << ", called with " << given.size();
        return nullptr;
    }
    if(formals) {
        std::size_t position = 0;
        for(const auto &formal : *formals) {
            // §13.5.3: an omitted trailing actual takes the formal's default,
            // evaluated in the declaring scope — which inlining preserves,
            // the task's names being the module's after the splice. (A blank
            // mid-list placeholder is call-site grammar, not admitted yet.)
            AST::Node::Ptr actual;
            if(position < given.size()) {
                actual = given[position]->clone();
            } else if(formal->get_default_value()) {
                actual = formal->get_default_value()->clone();
            } else {
                LOG_ERROR_N(call) << "task '" << name << "': no actual for argument '"
                                  << formal->get_name() << "' and no default (IEEE 1800-2017 "
                                  << "§13.5.3)";
                return nullptr;
            }
            ++position;
            const std::string &fname = formal->get_name();
            const std::string rname = site + "_" + fname;
            const bool is_ref = formal->get_direction() == AST::Arg::DirectionEnum::REF ||
                                formal->get_direction() == AST::Arg::DirectionEnum::CONST_REF;
            const bool ba_written = body_ba_targets.count(fname) != 0;
            if(!is_ref && task_automatic && body_nba_targets.count(fname)) {
                LOG_ERROR_N(call) << "task '" << name << "': '<=' to '" << fname
                                  << "' — no nonblocking assignment to automatic storage (IEEE "
                                  << "1800-2017 §6.21); a task that commits through its formals "
                                  << "must be static";
                return nullptr;
            }
            if(!is_ref && ba_written && body_nba_targets.count(fname)) {
                LOG_ERROR_N(call) << "task '" << name << "': '" << fname
                                  << "' is written with both '=' and '<=' — a formal is one "
                                  << "storage, pick one flavor";
                return nullptr;
            }
            FormalContext ctx;
            ctx.call = call;
            ctx.task_name = name;
            ctx.formal = formal;
            ctx.actual = actual;
            ctx.fname = fname;
            ctx.rname = rname;
            ctx.task_automatic = task_automatic;
            ctx.ba_written = ba_written;
            ctx.nba_written = body_nba_targets.count(fname) != 0;
            ctx.induced_written = body_induced_targets.count(fname) != 0;
            ctx.subst = &subst;
            ctx.head = &head;
            ctx.tail = &tail;
            ctx.check_read_first = &check_read_first;
            switch(formal->get_direction()) {
            case AST::Arg::DirectionEnum::INPUT:
                if(lower_input_formal(ctx)) {
                    return nullptr;
                }
                break;
            case AST::Arg::DirectionEnum::OUTPUT:
            case AST::Arg::DirectionEnum::INOUT:
                if(lower_copyout_formal(ctx)) {
                    return nullptr;
                }
                break;
            case AST::Arg::DirectionEnum::CONST_REF:
            case AST::Arg::DirectionEnum::REF:
                if(lower_ref_formal(ctx)) {
                    return nullptr;
                }
                break;
            default:
                LOG_ERROR_N(call) << "task '" << name << "': argument '" << fname
                                  << "' has a direction the inliner does not carry";
                return nullptr;
            }
        }
    }

    // ---- Locals: rename per site; a cut-spanning one hoists to a shared
    // register on a static task; a =-written cut-spanning one is refused
    // (§6, §7.4, IEEE §6.21). Substitution binds by name across the whole
    // body, so a local sharing a formal's or another local's name would
    // silently hijack the earlier binding — refused, like §6.1's shadows.
    {
        std::set<std::string> taken;
        if(formals) {
            for(const auto &formal : *formals) {
                taken.insert(formal->get_name());
            }
        }
        std::function<int(const AST::Node::Ptr &)> visit = [&](const AST::Node::Ptr &node) -> int {
            if(!node) {
                return 0;
            }
            if(m_expanded.count(node.get())) {
                return 0; // a nested expansion owns its renames
            }
            if(node->is_node_type(AST::NodeType::Block)) {
                const auto &inner = AST::cast_to<AST::Block>(node)->get_statements();
                if(inner) {
                    const bool spans = contains_event_statement(node);
                    // The '='-written names of a spanning block, once per
                    // block: every cut-spanning local checks against it.
                    std::set<std::string> blocking;
                    if(spans) {
                        std::function<void(const AST::Node::Ptr &)> scan_ba =
                            [&](const AST::Node::Ptr &n) {
                                if(!n) {
                                    return;
                                }
                                if(n->is_node_type(AST::NodeType::BlockingSubstitution)) {
                                    blocking.insert(lvalue_target(
                                        AST::cast_to<AST::BlockingSubstitution>(n)->get_left()));
                                }
                                const auto &kids = n->get_children();
                                if(kids) {
                                    for(const auto &c : *kids) {
                                        scan_ba(c);
                                    }
                                }
                            };
                        scan_ba(node);
                    }
                    for(auto it = inner->begin(); it != inner->end();) {
                        if(!(*it)->is_node_type(AST::NodeType::Var)) {
                            ++it;
                            continue;
                        }
                        const auto &var = AST::cast_to<AST::Var>(*it);
                        // By value: set_name below mutates the member a
                        // reference would alias.
                        const std::string lname = var->get_name();
                        if(!taken.insert(lname).second) {
                            LOG_ERROR_N(var)
                                << "task '" << name << "': local '" << lname << "' shares its "
                                << "name with a formal or another local — substitution binds "
                                << "by name (§6.1) — rename it";
                            return 1;
                        }
                        if(!spans) {
                            var->set_name(site + "_" + lname);
                            subst[lname] = AST::to_node(make_id(site + "_" + lname, fn, ln));
                            ++it;
                            continue;
                        }
                        if(blocking.count(lname)) {
                            LOG_ERROR_N(var)
                                << "task local '" << lname << "' is written with '=' in a "
                                << "scope a cut point spans: its intermediate values would "
                                << "need storage — write it with '<=', or keep it inside a "
                                << "scope with no wait";
                            return 1;
                        }
                        // A local that lives across a wait is a register, and
                        // only a static task's locals are static storage —
                        // automatic ones cannot take '<=' (IEEE 1800-2017
                        // §6.21), and unwritten ones would read garbage.
                        if(task->get_lifetime() == AST::Task::LifetimeEnum::AUTOMATIC) {
                            LOG_ERROR_N(var)
                                << "task local '" << lname << "' lives across a wait in an "
                                << "automatic task: automatic storage cannot be written with "
                                << "'<=' (IEEE 1800-2017 §6.21) — make the task static, or "
                                << "keep the local within one segment";
                            return 1;
                        }
                        // The hoist carries the declared type only: an
                        // array register is not hoisted yet, and would
                        // otherwise truncate to a scalar in silence.
                        if(var->get_unpacked_dims() && !var->get_unpacked_dims()->empty()) {
                            LOG_ERROR_N(var)
                                << "task local '" << lname << "' carries unpacked dimensions "
                                << "across a wait: an array register is not hoisted yet — "
                                << "keep it within one segment, or use a module-level "
                                << "declaration";
                            return 1;
                        }
                        // A static local initializes once (IEEE 1800-2017
                        // §6.21); the machine's reset re-runs — the hoist
                        // would silently drop the initializer either way.
                        if(var->get_init()) {
                            LOG_ERROR_N(var)
                                << "task local '" << lname << "' carries an initializer "
                                << "across a wait: a static local initializes once, which "
                                << "the machine's reset cannot express — drop the "
                                << "initializer and assign it explicitly";
                            return 1;
                        }
                        const std::string hname = name + "_" + lname;
                        // Register the shared hoist only once the declaration
                        // lands: registering first would make an author's
                        // same-named signal look like a benign re-hoist and
                        // silently alias it.
                        if(hoist_declaration(hname, AST::to_node(var->get_type()), fn, ln)) {
                            return 1;
                        }
                        m_static_hoist[hname] = name;
                        subst[lname] = AST::to_node(make_id(hname, fn, ln));
                        it = inner->erase(it);
                    }
                }
            }
            const auto &children = node->get_children();
            if(children) {
                for(const auto &child : *children) {
                    if(visit(child)) {
                        return 1;
                    }
                }
            }
            return 0;
        };
        if(visit(block_node)) {
            return nullptr;
        }
    }

    // §7.4: a clock or enable cannot arrive through copy-in — the §13.3
    // copy is captured at the call and never toggles again, so the source
    // would hang on it where a lowered copy-register would advance. The
    // reference kinds substitute to the live signal and are the spelling.
    {
        std::set<std::string> captured;
        if(formals) {
            for(const auto &formal : *formals) {
                if(formal->get_direction() != AST::Arg::DirectionEnum::REF &&
                   formal->get_direction() != AST::Arg::DirectionEnum::CONST_REF) {
                    captured.insert(formal->get_name());
                }
            }
        }
        if(!captured.empty()) {
            std::function<int(const AST::Node::Ptr &)> scan_waits =
                [&](const AST::Node::Ptr &n) -> int {
                if(!n) {
                    return 0;
                }
                if(n->is_node_type(AST::NodeType::EventStatement)) {
                    std::set<std::string> names;
                    collect_identifier_names(
                        AST::to_node(AST::cast_to<AST::EventStatement>(n)->get_senslist()), names);
                    for(const auto &formal_name : captured) {
                        if(names.count(formal_name)) {
                            LOG_ERROR_N(n) << "task '" << name << "': the wait reads formal '"
                                           << formal_name << "', which is copied in at the call — "
                                           << "a §13.3 copy never toggles again, so the source "
                                           << "would hang on it; pass a clock or enable by "
                                           << "'const ref'";
                            return 1;
                        }
                    }
                }
                const auto &kids = n->get_children();
                if(kids) {
                    for(const auto &c : *kids) {
                        if(scan_waits(c)) {
                            return 1;
                        }
                    }
                }
                return 0;
            };
            if(scan_waits(block_node)) {
                return nullptr;
            }
        }
    }

    // ---- Apply the renames, then splice copy-in/copy-out.
    if(!subst.empty()) {
        rename_into(block_node, subst);
    }
    for(auto it = head.rbegin(); it != head.rend(); ++it) {
        stmts->push_front(*it);
    }
    for(const auto &stmt : tail) {
        stmts->push_back(stmt);
    }

    // §7.4: an automatic hoisted local and an output formal must be
    // assigned before read — a register persists where the fresh local
    // (or the not-yet-copied-out formal) holds nothing readable.
    std::set<std::string> always_written;
    if(!check_read_first.empty()) {
        all_paths_writes(block_node, always_written);
        for(const auto &w : always_written) {
            LOG_INFO << "APW " << name << ": " << w;
        }
        for(const auto &c : check_read_first) {
            LOG_INFO << "CRF " << name << ": " << c;
        }
    }
    for(const auto &checked : check_read_first) {
        // The copy-out reads an output formal on every path, so a write
        // on only some paths leaves the read undefined on the others —
        // the all-paths set is the property, program order the tiebreak.
        if(Analysis::Statement::first_reference(block_node, checked) < 0 ||
           !always_written.count(checked)) {
            LOG_ERROR_N(call)
                << "'" << checked << "' is read before it is assigned in this activation: "
                << "a register persists where the task's storage starts fresh — assign it "
                << "first";
            return nullptr;
        }
    }

    m_inlined.insert(name);
    m_expanded.insert(block_node.get());
    return block_node;
}

int FsmTaskInliner::inline_calls_in(const AST::Node::Ptr &node, std::set<std::string> &visiting)
{
    if(!node) {
        return 0;
    }
    const auto &children = node->get_children();
    if(!children) {
        return 0;
    }
    for(const auto &child : *children) {
        const bool statement_call = child && (child->is_node_type(AST::NodeType::TaskCall) ||
                                              child->is_node_type(AST::NodeType::Call));
        if(statement_call) {
            const auto &call = AST::cast_to<AST::Call>(child);
            if(m_tasks.count(call->get_name())) {
                const auto &expansion = expand_call(call, visiting);
                if(!expansion) {
                    return 1;
                }
                node->replace(child, expansion);
                continue;
            }
        }
        if(inline_calls_in(child, visiting)) {
            return 1;
        }
    }
    return 0;
}

int FsmTaskInliner::inline_process(const AST::Initial::Ptr &initial)
{
    if(m_tasks.empty()) {
        return 0;
    }
    // §7.4: user loops with constant bounds unroll BEFORE inlining, so a
    // call inside an unrolled body becomes its own call site with its own
    // copy-in; a task-body loop whose bound substitutes constant unrolls
    // in the second, post-inline run — after hoisting, seeded module-wide
    // both times — and the induced commits that second run's cloning
    // duplicated re-adopt by their generated shapes.
    if(LoopUnrolling(m_declared).run(AST::to_node(initial))) {
        return 1;
    }
    std::set<std::string> visiting;
    if(inline_calls_in(AST::to_node(initial), visiting)) {
        return 1;
    }
    if(m_inlined.empty()) {
        return 0;
    }
    if(LoopUnrolling(m_declared).run(AST::to_node(initial))) {
        return 1;
    }
    return 0;
}

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse
