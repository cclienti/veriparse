// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
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

} // namespace

int ImplicitFsmElaboration::hoist_declaration(const std::string &name, const AST::Node::Ptr &type,
                                              const std::string &fn, int ln)
{
    if(m_module_state.declared.count(name)) {
        // Re-hoist of a static local from a later call site: the single
        // shared register already exists.
        if(m_module_state.static_hoist.count(name)) {
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
    const auto &items = m_proc.module->get_items();
    for(auto it = items->begin(); it != items->end(); ++it) {
        if(it->get() == static_cast<AST::Node *>(m_proc.pragmalist.get())) {
            items->insert(it, AST::to_node(reg));
            m_module_state.declared.insert(name);
            return 0;
        }
    }
    items->push_back(AST::to_node(reg));
    m_module_state.declared.insert(name);
    return 0;
}

AST::Node::Ptr ImplicitFsmElaboration::expand_call(const AST::Call::Ptr &call,
                                                   std::set<std::string> &visiting)
{
    const std::string &name = call->get_name();
    const auto &task = m_module_state.tasks.at(name);
    const std::string &fn = call->get_filename();
    const int ln = call->get_line();

    if(visiting.count(name)) {
        LOG_ERROR_N(call) << "recursive task call: '" << name << "' reaches itself — "
                          << "inlining is the model, and there is no stack to give "
                          << "recursion meaning";
        return nullptr;
    }

    const unsigned int ordinal = m_module_state.task_ordinal[name]++;
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
    {
        std::function<void(const AST::Node::Ptr &)> scan = [&](const AST::Node::Ptr &n) {
            if(!n) {
                return;
            }
            if(induced_marker_kind(n)) {
                return; // a nested expansion's induced commit, not the body's
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
            switch(formal->get_direction()) {
            case AST::Arg::DirectionEnum::INPUT:
                if(ba_written) {
                    // The local-copy idiom: a '='-written formal is a §6.1
                    // temporary declared in the task block — legal only where
                    // no cut spans, which §6 checks on the declaration —
                    // seeded from the actual.
                    auto tmp = std::make_shared<AST::Var>(fn, ln);
                    tmp->set_name(rname);
                    if(formal->get_type()) {
                        tmp->set_type(AST::cast_to<AST::DataType>(formal->get_type()->clone()));
                    }
                    head.push_back(AST::to_node(tmp));
                    head.push_back(AST::to_node(make_ba(rname, actual, fn, ln)));
                    subst[fname] = AST::to_node(make_id(rname, fn, ln));
                    break;
                }
                if(actual->is_node_category(AST::NodeType::Constant) &&
                   !body_nba_targets.count(fname)) {
                    // A written formal is storage, never a substitutable
                    // constant — the capture register below takes it.
                    subst[fname] = actual;
                    break;
                }
                if(hoist_declaration(rname, AST::to_node(formal->get_type()), fn, ln)) {
                    return nullptr;
                }
                head.push_back(make_induced_marker(make_nba(rname, actual, fn, ln), true, fn, ln));
                subst[fname] = AST::to_node(make_id(rname, fn, ln));
                break;
            case AST::Arg::DirectionEnum::OUTPUT:
            case AST::Arg::DirectionEnum::INOUT: {
                if(!actual->is_node_type(AST::NodeType::Identifier)) {
                    LOG_ERROR_N(call) << "task '" << name << "': the actual for " << "'" << fname
                                      << "' must be a plain register of the process — copy-out has "
                                      << "one storage to write";
                    return nullptr;
                }
                if(ba_written) {
                    // A '='-written output/inout formal is the same §6.1
                    // temporary, copied out to the actual at return — the
                    // no-wait helper shape a package task takes when its
                    // result must land the same cycle.
                    auto tmp = std::make_shared<AST::Var>(fn, ln);
                    tmp->set_name(rname);
                    if(formal->get_type()) {
                        tmp->set_type(AST::cast_to<AST::DataType>(formal->get_type()->clone()));
                    }
                    head.push_back(AST::to_node(tmp));
                    if(formal->get_direction() == AST::Arg::DirectionEnum::INOUT) {
                        head.push_back(AST::to_node(make_ba(rname, actual, fn, ln)));
                    } else {
                        check_read_first.push_back(rname);
                    }
                    {
                        const std::string aname = AST::cast_to<AST::Identifier>(actual)->get_name();
                        tail.push_back(make_induced_marker(
                            make_nba(aname, AST::to_node(make_id(rname, fn, ln)), fn, ln), false,
                            fn, ln));
                    }
                    subst[fname] = AST::to_node(make_id(rname, fn, ln));
                    break;
                }
                if(hoist_declaration(rname, AST::to_node(formal->get_type()), fn, ln)) {
                    return nullptr;
                }
                if(formal->get_direction() == AST::Arg::DirectionEnum::INOUT) {
                    head.push_back(
                        make_induced_marker(make_nba(rname, actual, fn, ln), true, fn, ln));
                } else {
                    check_read_first.push_back(rname);
                }
                {
                    const std::string aname = AST::cast_to<AST::Identifier>(actual)->get_name();
                    tail.push_back(make_induced_marker(
                        make_nba(aname, AST::to_node(make_id(rname, fn, ln)), fn, ln), false, fn,
                        ln));
                }
                subst[fname] = AST::to_node(make_id(rname, fn, ln));
                break;
            }
            case AST::Arg::DirectionEnum::CONST_REF:
            case AST::Arg::DirectionEnum::REF: {
                const bool read_only =
                    formal->get_direction() == AST::Arg::DirectionEnum::CONST_REF;
                // §7.4, measured: a ref is a true alias for blocking
                // writes on an automatic task — pure substitution, no
                // local, no copy anything. '<=' through it is illegal
                // IEEE (no nonblocking assignment to an automatic), and
                // §13.5.2 ties ref to automatic lifetime.
                if(task->get_lifetime() != AST::Task::LifetimeEnum::AUTOMATIC) {
                    LOG_ERROR_N(call)
                        << "task '" << name << "': a ref argument needs 'task automatic' "
                        << "(IEEE 1800-2017 §13.5.2)";
                    return nullptr;
                }
                if(!actual->is_node_type(AST::NodeType::Identifier)) {
                    LOG_ERROR_N(call) << "task '" << name << "': the actual for ref '" << fname
                                      << "' must be a plain signal of the process";
                    return nullptr;
                }
                {
                    const std::string &aname = AST::cast_to<AST::Identifier>(actual)->get_name();
                    if(is_net_signal(m_proc.module, aname)) {
                        LOG_ERROR_N(call)
                            << "task '" << name << "': actual '" << aname << "' for ref '" << fname
                            << "' is a net — nets shall not be passed by reference (IEEE "
                            << "1800-2017 §13.5.2); make it a variable ('input var logic " << aname
                            << "')";
                        return nullptr;
                    }
                }
                if(body_nba_targets.count(fname)) {
                    LOG_ERROR_N(call) << "task '" << name << "': '<=' through ref '" << fname
                                      << "' — no nonblocking assignment to an automatic (IEEE "
                                      << "1800-2017 §10.4.2, §13.5.2); alias with '=', or capture "
                                      << "with 'input' and commit registers directly";
                    return nullptr;
                }
                if(read_only && ba_written) {
                    LOG_ERROR_N(call) << "task '" << name << "': write through const "
                                      << "ref '" << fname << "' (IEEE 1800-2017 §13.5.2)";
                    return nullptr;
                }
                subst[fname] = actual;
                break;
            }
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
            if(m_inline.expanded.count(node.get())) {
                return 0; // a nested expansion owns its renames
            }
            if(node->is_node_type(AST::NodeType::Block)) {
                const auto &inner = AST::cast_to<AST::Block>(node)->get_statements();
                if(inner) {
                    const bool spans = contains_event_statement(node);
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
                        std::set<std::string> blocking;
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
                        const std::string hname = name + "_" + lname;
                        // Register the shared hoist only once the declaration
                        // lands: registering first would make an author's
                        // same-named signal look like a benign re-hoist and
                        // silently alias it.
                        if(hoist_declaration(hname, AST::to_node(var->get_type()), fn, ln)) {
                            return 1;
                        }
                        m_module_state.static_hoist[hname] = name;
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
    for(const auto &checked : check_read_first) {
        if(Analysis::Statement::first_reference(block_node, checked) < 0) {
            LOG_ERROR_N(call)
                << "'" << checked << "' is read before it is assigned in this activation: "
                << "a register persists where the task's storage starts fresh — assign it "
                << "first";
            return nullptr;
        }
    }

    m_module_state.inlined_tasks.insert(name);
    m_inline.expanded.insert(block_node.get());
    return block_node;
}

int ImplicitFsmElaboration::inline_calls_in(const AST::Node::Ptr &node,
                                            std::set<std::string> &visiting)
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
            if(m_module_state.tasks.count(call->get_name())) {
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

int ImplicitFsmElaboration::inline_tasks(const AST::Initial::Ptr &initial)
{
    if(m_module_state.tasks.empty()) {
        return 0;
    }
    // §7.4: user loops with constant bounds unroll BEFORE inlining, so a
    // call inside an unrolled body becomes its own call site with its own
    // copy-in; a task-body loop whose bound substitutes constant unrolls
    // in the second, post-inline run — after hoisting, seeded module-wide
    // both times — and the induced commits that second run's cloning
    // duplicated re-adopt by their generated shapes.
    if(LoopUnrolling(m_module_state.declared).run(AST::to_node(initial))) {
        return 1;
    }
    std::set<std::string> visiting;
    if(inline_calls_in(AST::to_node(initial), visiting)) {
        return 1;
    }
    if(m_module_state.inlined_tasks.empty()) {
        return 0;
    }
    if(LoopUnrolling(m_module_state.declared).run(AST::to_node(initial))) {
        return 1;
    }
    adopt_markers(AST::to_node(initial));
    return 0;
}

/// The induced commits travelled as pragma markers, which any cloning
/// copies along; with the process's text final, each marker unwraps to
/// its nonblocking assignment and the walk's induced index takes the
/// pointer — stable from here, nothing clones again.
void ImplicitFsmElaboration::adopt_markers(const AST::Node::Ptr &node)
{
    if(!node) {
        return;
    }
    const auto &children = node->get_children();
    if(!children) {
        return;
    }
    for(const auto &child : *children) {
        const int kind = induced_marker_kind(child);
        if(kind == 0) {
            adopt_markers(child);
            continue;
        }
        const auto &nba = induced_marker_nba(child);
        if(!nba) {
            continue;
        }
        node->replace(child, AST::to_node(nba));
        if(kind == 1) {
            m_inline.captures.insert(nba.get());
        } else {
            m_inline.copyouts.insert(nba.get());
        }
    }
}

bool ImplicitFsmElaboration::contains_induced(const AST::Node::Ptr &node) const
{
    if(!node) {
        return false;
    }
    if(m_inline.captures.count(node.get()) || m_inline.copyouts.count(node.get())) {
        return true;
    }
    const auto &children = node->get_children();
    if(!children) {
        return false;
    }
    for(const auto &child : *children) {
        if(contains_induced(child)) {
            return true;
        }
    }
    return false;
}

/// A cut-point-free subtree holding induced commits, emitted into an
/// action: reads substitute against a branch-local environment that the
/// captures and copy-outs update in program order — §13.3's immediate
/// visibility inside the branch — while each nested arm diverges on a
/// copy. The caller marks the committed targets branch-dependent in the
/// segment's own environment.
AST::Node::Ptr ImplicitFsmElaboration::emit_verbatim(const AST::Node::Ptr &stmt, Env &env,
                                                     std::set<std::string> &committed)
{
    const std::string &fn = stmt->get_filename();
    const int ln = stmt->get_line();

    if(stmt->is_node_type(AST::NodeType::NonblockingSubstitution) &&
       (m_inline.captures.count(stmt.get()) || m_inline.copyouts.count(stmt.get()))) {
        const auto &nba = AST::cast_to<AST::NonblockingSubstitution>(stmt);
        const auto &value = clone_subst(AST::to_node(nba->get_right()->get_var()), env);
        if(check_temp_reads(value, env)) {
            return nullptr;
        }
        env[nba_target(nba)] = value;
        committed.insert(nba_target(nba));
        return AST::to_node(make_nba(nba_target(nba), value, fn, ln));
    }

    if(stmt->is_node_type(AST::NodeType::Block)) {
        const auto &blk = AST::cast_to<AST::Block>(stmt);
        auto out = std::make_shared<AST::Node::List>();
        if(blk->get_statements()) {
            for(const auto &child : *blk->get_statements()) {
                if(!child) {
                    continue;
                }
                if(child->is_node_type(AST::NodeType::Var)) {
                    out->push_back(child->clone());
                    continue;
                }
                const auto &emitted = emit_verbatim(child, env, committed);
                if(!emitted) {
                    return nullptr;
                }
                out->push_back(emitted);
            }
        }
        return AST::to_node(std::make_shared<AST::Block>(out, blk->get_scope(), fn, ln));
    }

    if(stmt->is_node_type(AST::NodeType::SingleStatement)) {
        const auto &single = AST::cast_to<AST::SingleStatement>(stmt);
        if(single->get_statement() && contains_induced(single->get_statement())) {
            const auto &inner = emit_verbatim(single->get_statement(), env, committed);
            if(!inner) {
                return nullptr;
            }
            const auto &out = AST::cast_to<AST::SingleStatement>(stmt->clone());
            out->set_statement(inner);
            return AST::to_node(out);
        }
    }

    if(stmt->is_node_type(AST::NodeType::IfStatement) && contains_induced(stmt)) {
        const auto &ifs = AST::cast_to<AST::IfStatement>(stmt);
        const auto &cond = clone_subst(ifs->get_cond(), env);
        if(check_temp_reads(cond, env)) {
            return nullptr;
        }
        AST::Node::Ptr t;
        AST::Node::Ptr e;
        std::set<std::string> arm_committed;
        if(ifs->get_true_statement()) {
            Env arm_env = env;
            t = emit_verbatim(ifs->get_true_statement(), arm_env, arm_committed);
            if(!t) {
                return nullptr;
            }
        }
        if(ifs->get_false_statement()) {
            Env arm_env = env;
            e = emit_verbatim(ifs->get_false_statement(), arm_env, arm_committed);
            if(!e) {
                return nullptr;
            }
        }
        for(const auto &target : arm_committed) {
            env[target] = nullptr; // arm-dependent past this if
            committed.insert(target);
        }
        auto out = std::make_shared<AST::IfStatement>(fn, ln);
        out->set_cond(cond);
        out->set_true_statement(t);
        out->set_false_statement(e);
        return AST::to_node(out);
    }

    if(contains_induced(stmt)) {
        LOG_ERROR_N(stmt) << "a task call under this construct, inside a branch no cut point "
                          << "spans, is not lowered — hoist the call out of the branch, or put "
                          << "a wait inside it";
        return nullptr;
    }

    const auto &placed = clone_subst(stmt, env);
    if(check_temp_reads(placed, env)) {
        return nullptr;
    }
    return placed;
}

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse
