// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#pragma once

#include <veriparse/passes/transformations/implicit_fsm_elaboration.hpp>
#include <veriparse/passes/transformations/loop_unrolling.hpp>
#include <veriparse/passes/transformations/expression_evaluation.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/passes/analysis/statement.hpp>
#include <veriparse/passes/analysis/storage_kind.hpp>
#include <veriparse/passes/transformations/ast_replace.hpp>
#include <veriparse/generators/verilog_generator.hpp>
#include <veriparse/misc/string_utils.hpp>
#include <veriparse/logger/logger.hpp>

#include <algorithm>
#include <cctype>
#include <functional>
#include <map>
#include <set>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

/// Shared free helpers of the ImplicitFsmElaboration translation units —
/// the pass is one class split across implicit_fsm_elaboration.cpp (walk
/// and emission), implicit_fsm_checks.cpp (§2/§5/§9 checks) and
/// implicit_fsm_task_inliner.cpp (§7.4). Internal header: not installed.
namespace FsmDetail
{

inline bool has_pragma(const AST::Pragmalist::Ptr &pragmalist, const std::string &name)
{
    const auto &pragmas = pragmalist->get_pragmas();
    if(!pragmas) {
        return false;
    }
    for(const auto &pragma : *pragmas) {
        if(pragma && pragma->get_name() == name) {
            return true;
        }
    }
    return false;
}

inline AST::Pragma::Ptr get_pragma(const AST::Pragmalist::Ptr &pragmalist, const std::string &name)
{
    const auto &pragmas = pragmalist->get_pragmas();
    if(!pragmas) {
        return nullptr;
    }
    for(const auto &pragma : *pragmas) {
        if(pragma && pragma->get_name() == name) {
            return pragma;
        }
    }
    return nullptr;
}

/// The module's (* veriparse_fsm *)-marked statements, in source order:
/// each marked pragmalist paired with every statement under it. The FSM
/// passes filter the Initial ones; the elaborator diagnoses the rest.
inline std::vector<std::pair<AST::Pragmalist::Ptr, AST::Node::Ptr>>
collect_marked(const AST::Module::Ptr &module)
{
    std::vector<std::pair<AST::Pragmalist::Ptr, AST::Node::Ptr>> marked;
    const auto &items = module->get_items();
    if(!items) {
        return marked;
    }
    for(const auto &item : *items) {
        if(!item || !item->is_node_type(AST::NodeType::Pragmalist)) {
            continue;
        }
        const auto &pragmalist = AST::cast_to<AST::Pragmalist>(item);
        if(!has_pragma(pragmalist, "veriparse_fsm")) {
            continue;
        }
        const auto &statements = pragmalist->get_statements();
        if(!statements) {
            continue;
        }
        for(const auto &stmt : *statements) {
            marked.emplace_back(pragmalist, stmt);
        }
    }
    return marked;
}

/// §13.5.2's net-actual refusal, worded once for task and function calls.
inline void log_net_actual(const AST::Node::Ptr &at, const char *kind, const std::string &callee,
                           const std::string &formal, const std::string &actual)
{
    LOG_ERROR_N(at) << kind << " '" << callee << "': actual '" << actual << "' for ref '" << formal
                    << "' is a net — nets and selects into nets shall not be passed by "
                    << "reference (IEEE 1800-2017 §13.5.2); make it a variable "
                    << "('input var logic " << actual << "')";
}

/// The declaration prefix of the index-th marked process (of `total`):
/// the veriparse_prefix hint's text when one is written — validated by
/// the elaborator, which owns the diagnostics — else __fsm, ordinal-
/// suffixed when several processes share the module (§3, §10.1).
inline std::string marked_prefix(const AST::Pragmalist::Ptr &pragmalist, std::size_t index,
                                 std::size_t total)
{
    const auto &hint = get_pragma(pragmalist, "veriparse_prefix");
    if(hint) {
        const auto &expr = hint->get_expression();
        std::string wanted;
        if(expr && expr->is_node_type(AST::NodeType::StringConst)) {
            wanted = AST::cast_to<AST::StringConst>(expr)->get_value();
        } else if(expr && expr->is_node_type(AST::NodeType::Identifier)) {
            wanted = AST::cast_to<AST::Identifier>(expr)->get_name();
        }
        if(!wanted.empty()) {
            return wanted;
        }
    }
    return (total > 1) ? ("__fsm" + std::to_string(index)) : "__fsm";
}

/// §7.4 induced-commit markers: the inliner's copy-in captures and
/// copy-outs travel as pragma-wrapped nonblocking assignments —
/// `(* veriparse_fsm_capture *)` / `(* veriparse_fsm_copyout *)` — so the
/// identity survives any cloning (an unrolled clone keeps its pragma where
/// a pointer would dangle). The walk adopts and unwraps them before path
/// enumeration; the markers never reach the output.
constexpr const char *k_capture_marker = "veriparse_fsm_capture";
constexpr const char *k_copyout_marker = "veriparse_fsm_copyout";

/// 0 = not a marker; 1 = capture; 2 = copy-out. A marker is a Pragmalist
/// carrying exactly the internal pragma around one nonblocking assignment.
inline int induced_marker_kind(const AST::Node::Ptr &node)
{
    if(!node || !node->is_node_type(AST::NodeType::Pragmalist)) {
        return 0;
    }
    const auto &pragmas = AST::cast_to<AST::Pragmalist>(node)->get_pragmas();
    if(!pragmas) {
        return 0;
    }
    for(const auto &pragma : *pragmas) {
        if(!pragma) {
            continue;
        }
        if(pragma->get_name() == k_capture_marker) {
            return 1;
        }
        if(pragma->get_name() == k_copyout_marker) {
            return 2;
        }
    }
    return 0;
}

/// The marked nonblocking assignment inside an induced-commit marker.
inline AST::NonblockingSubstitution::Ptr induced_marker_nba(const AST::Node::Ptr &node)
{
    const auto &stmts = AST::cast_to<AST::Pragmalist>(node)->get_statements();
    if(!stmts || stmts->size() != 1 ||
       !stmts->front()->is_node_type(AST::NodeType::NonblockingSubstitution)) {
        return nullptr;
    }
    return AST::cast_to<AST::NonblockingSubstitution>(stmts->front());
}

/// Wrap an induced commit in its marker.
inline AST::Node::Ptr make_induced_marker(const AST::NonblockingSubstitution::Ptr &nba,
                                          bool capture, const std::string &fn, int ln)
{
    auto pragma = std::make_shared<AST::Pragma>(fn, ln);
    pragma->set_name(capture ? k_capture_marker : k_copyout_marker);
    auto pragmas = std::make_shared<AST::Pragma::List>();
    pragmas->push_back(pragma);
    auto stmts = std::make_shared<AST::Node::List>();
    stmts->push_back(AST::to_node(nba));
    auto wrapper = std::make_shared<AST::Pragmalist>(fn, ln);
    wrapper->set_pragmas(pragmas);
    wrapper->set_statements(stmts);
    return AST::to_node(wrapper);
}

inline bool contains_event_statement(const AST::Node::Ptr &node)
{
    if(!node) {
        return false;
    }
    if(node->is_node_type(AST::NodeType::EventStatement)) {
        return true;
    }
    const auto &children = node->get_children();
    for(const auto &child : *children) {
        if(contains_event_statement(child)) {
            return true;
        }
    }
    return false;
}

/// Statement-tree queries live in Analysis::Statement; these forwarders
/// keep the pass's unqualified idiom.
inline void collect_identifier_names(const AST::Node::Ptr &node, std::set<std::string> &names)
{
    Analysis::Statement::collect_identifier_names(node, names);
}

inline std::string lvalue_target(const AST::Lvalue::Ptr &lvalue)
{
    return Analysis::Statement::lvalue_target(lvalue);
}

inline std::string nba_target(const AST::NonblockingSubstitution::Ptr &nba)
{
    return Analysis::Statement::nba_target(nba);
}

/// The target of a for's blocking init or step (§7.2).
inline std::string nba_like_target(const AST::BlockingSubstitution::Ptr &assign)
{
    return lvalue_target(assign->get_left());
}

inline std::string to_lower(const std::string &str)
{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

inline unsigned int clog2(unsigned int value)
{
    unsigned int width = 0;
    for(unsigned int remaining = value - 1; remaining != 0; remaining >>= 1) {
        ++width;
    }
    return width == 0 ? 1 : width;
}

inline AST::IntConstN::Ptr make_const(unsigned int value, int size, const std::string &fn, int ln)
{
    auto node = std::make_shared<AST::IntConstN>(fn, ln);
    node->set_base(10);
    node->set_size(size);
    node->set_sign(false);
    node->set_value(value);
    return node;
}

inline AST::Identifier::Ptr make_id(const std::string &name, const std::string &fn, int ln)
{
    auto node = std::make_shared<AST::Identifier>(fn, ln);
    node->set_name(name);
    return node;
}

inline AST::Dimension::ListPtr make_packed_range(unsigned int msb, const std::string &fn, int ln)
{
    auto range = std::make_shared<AST::RangeDim>(fn, ln);
    range->set_left(AST::to_node(make_const(msb, -1, fn, ln)));
    range->set_right(AST::to_node(make_const(0, -1, fn, ln)));
    auto dims = std::make_shared<AST::Dimension::List>();
    dims->push_back(range);
    return dims;
}

inline AST::NonblockingSubstitution::Ptr make_state_assign(const std::string &state_reg,
                                                           const std::string &state_name,
                                                           const std::string &fn, int ln)
{
    auto lvalue = std::make_shared<AST::Lvalue>(fn, ln);
    lvalue->set_var(AST::to_node(make_id(state_reg, fn, ln)));
    auto rvalue = std::make_shared<AST::Rvalue>(fn, ln);
    rvalue->set_var(AST::to_node(make_id(state_name, fn, ln)));
    auto nba = std::make_shared<AST::NonblockingSubstitution>(fn, ln);
    nba->set_left(lvalue);
    nba->set_right(rvalue);
    return nba;
}

/// The entry pseudo-state of the path enumeration: its single transition is
/// the reset branch (§5.1).
inline constexpr std::size_t k_entry = static_cast<std::size_t>(-1);

/// Every identifier read in a subtree, by name.
/// §6.2: whether a subtree holds a '=' whose target no declaration in
/// scope covers — a write to an enclosing scope or to module level,
/// either of which forces the branch onto the path cover. Scope-aware:
/// a declaration hides the name only for its own block's statements, so
/// a sibling branch's local never masks a module-level write.
inline bool contains_outer_blocking_scan(const AST::Node::Ptr &node,
                                         const std::set<std::string> &locals)
{
    if(!node) {
        return false;
    }
    if(node->is_node_type(AST::NodeType::Block)) {
        const auto &stmts = AST::cast_to<AST::Block>(node)->get_statements();
        if(stmts) {
            // The one copy, at the one scope boundary.
            std::set<std::string> scope = locals;
            for(const auto &stmt : *stmts) {
                if(stmt->is_node_type(AST::NodeType::Var)) {
                    scope.insert(AST::cast_to<AST::Var>(stmt)->get_name());
                    continue;
                }
                if(contains_outer_blocking_scan(stmt, scope)) {
                    return true;
                }
            }
        }
        return false;
    }
    if(node->is_node_type(AST::NodeType::BlockingSubstitution)) {
        const auto &target =
            lvalue_target(AST::cast_to<AST::BlockingSubstitution>(node)->get_left());
        return !target.empty() && !locals.count(target);
    }
    const AST::Node::ListPtr children = node->get_children();
    if(children) {
        for(const AST::Node::Ptr &child : *children) {
            if(contains_outer_blocking_scan(child, locals)) {
                return true;
            }
        }
    }
    return false;
}

inline bool contains_outer_blocking(const AST::Node::Ptr &node)
{
    return contains_outer_blocking_scan(node, {});
}

inline AST::BlockingSubstitution::Ptr
make_blocking(const std::string &name, const AST::Node::Ptr &value, const std::string &fn, int ln)
{
    auto lvalue = std::make_shared<AST::Lvalue>(fn, ln);
    lvalue->set_var(AST::to_node(make_id(name, fn, ln)));
    auto rvalue = std::make_shared<AST::Rvalue>(fn, ln);
    rvalue->set_var(value->clone());
    auto assign = std::make_shared<AST::BlockingSubstitution>(fn, ln);
    assign->set_left(lvalue);
    assign->set_right(rvalue);
    return assign;
}

inline AST::Node::Ptr make_ulnot(const AST::Node::Ptr &expr, const std::string &fn, int ln)
{
    auto node = std::make_shared<AST::Ulnot>(fn, ln);
    node->set_right(expr);
    return AST::to_node(node);
}

inline AST::Node::Ptr make_land(const AST::Node::Ptr &left, const AST::Node::Ptr &right,
                                const std::string &fn, int ln)
{
    auto node = std::make_shared<AST::Land>(fn, ln);
    node->set_left(left);
    node->set_right(right);
    return AST::to_node(node);
}

inline AST::Node::Ptr make_lor(const AST::Node::Ptr &left, const AST::Node::Ptr &right,
                               const std::string &fn, int ln)
{
    auto node = std::make_shared<AST::Lor>(fn, ln);
    node->set_left(left);
    node->set_right(right);
    return AST::to_node(node);
}

inline AST::Node::Ptr make_eq(const AST::Node::Ptr &left, const AST::Node::Ptr &right,
                              const std::string &fn, int ln)
{
    auto node = std::make_shared<AST::Eq>(fn, ln);
    node->set_left(left);
    node->set_right(right);
    return AST::to_node(node);
}

inline AST::Node::Ptr make_noteq(const AST::Node::Ptr &left, const AST::Node::Ptr &right,
                                 const std::string &fn, int ln)
{
    auto node = std::make_shared<AST::NotEq>(fn, ln);
    node->set_left(left);
    node->set_right(right);
    return AST::to_node(node);
}

inline AST::Node::Ptr make_minus(const AST::Node::Ptr &left, const AST::Node::Ptr &right,
                                 const std::string &fn, int ln)
{
    auto node = std::make_shared<AST::Minus>(fn, ln);
    node->set_left(left);
    node->set_right(right);
    return AST::to_node(node);
}

inline AST::NonblockingSubstitution::Ptr
make_nba(const std::string &target, const AST::Node::Ptr &rhs, const std::string &fn, int ln)
{
    auto lvalue = std::make_shared<AST::Lvalue>(fn, ln);
    lvalue->set_var(AST::to_node(make_id(target, fn, ln)));
    auto rvalue = std::make_shared<AST::Rvalue>(fn, ln);
    rvalue->set_var(rhs);
    auto nba = std::make_shared<AST::NonblockingSubstitution>(fn, ln);
    nba->set_left(lvalue);
    nba->set_right(rvalue);
    return nba;
}

/// A nonblocking assignment whose target is a ready-made lvalue
/// expression — a hierarchical interface member the string form of
/// make_nba could not name.
inline AST::NonblockingSubstitution::Ptr
make_nba_to(const AST::Node::Ptr &target, const AST::Node::Ptr &rhs, const std::string &fn, int ln)
{
    auto lvalue = std::make_shared<AST::Lvalue>(fn, ln);
    lvalue->set_var(target);
    auto rvalue = std::make_shared<AST::Rvalue>(fn, ln);
    rvalue->set_var(rhs);
    auto nba = std::make_shared<AST::NonblockingSubstitution>(fn, ln);
    nba->set_left(lvalue);
    nba->set_right(rvalue);
    return nba;
}

/// The commit an induced assignment becomes: the source lvalue is kept as
/// it stands, never rebuilt from its key — a key identifies storage and
/// cannot name it again (a hierarchical path would come back as one
/// escaped identifier, and every consumer would read it as a local).
inline AST::NonblockingSubstitution::Ptr commit_like(const AST::NonblockingSubstitution::Ptr &src,
                                                     const AST::Node::Ptr &value,
                                                     const std::string &fn, int ln)
{
    return make_nba_to(src->get_left()->get_var()->clone(), value, fn, ln);
}

/// Whether a key names a member of one of @p iface_ports: its root label,
/// index dropped, is one of them.
inline bool key_is_iface_member(const std::string &key, const std::set<std::string> &iface_ports)
{
    const std::size_t dot = key.find('.');
    if(dot == std::string::npos) {
        return false;
    }
    std::string root = key.substr(0, dot);
    const std::size_t bracket = root.find('[');
    if(bracket != std::string::npos) {
        root = root.substr(0, bracket);
    }
    return iface_ports.count(root) != 0;
}

/// Refuse a reference whose hierarchical label is indexed by a variable:
/// it names a different storage per evaluation, so the analyses that rest
/// on identity — definedness, commit ordering, forwarding — would all
/// decide on a name two references share.
inline int check_static_hier(const AST::Node::Ptr &node)
{
    if(!node) {
        return 0;
    }
    if(node->is_node_type(AST::NodeType::Identifier)) {
        const auto &id = AST::cast_to<AST::Identifier>(node);
        if(id->get_hier() && Analysis::Statement::identifier_key(id).empty()) {
            LOG_ERROR_N(node) << "hierarchical name indexed by a variable: it names a "
                              << "different signal per evaluation, which the state model "
                              << "cannot tell apart — index it with a constant, or select "
                              << "outside the process";
            return 1;
        }
    }
    const auto &children = node->get_children();
    for(const auto &child : *children) {
        if(check_static_hier(child)) {
            return 1;
        }
    }
    return 0;
}

/// Names of this module's interface ports (`bus_if.dev bus` — non-virtual):
/// their members are signals of the machine, every other hierarchical name
/// belongs to another scope.
inline std::set<std::string> collect_iface_ports(const AST::Module::Ptr &module)
{
    std::set<std::string> names;
    const auto &ports = module->get_ports();
    if(!ports) {
        return names;
    }
    for(const auto &port_node : *ports) {
        if(!port_node->is_node_type(AST::NodeType::Port)) {
            continue;
        }
        const auto &decl = AST::cast_to<AST::Port>(port_node)->get_decl();
        if(decl && decl->is_node_type(AST::NodeType::Arg)) {
            const auto &arg = AST::cast_to<AST::Arg>(decl);
            if(arg->get_type() && arg->get_type()->is_node_type(AST::NodeType::InterfaceType) &&
               !AST::cast_to<AST::InterfaceType>(arg->get_type())->get_is_virtual()) {
                names.insert(arg->get_name());
            }
        }
    }
    return names;
}

/// Whether a hierarchical identifier reaches into one of this module's
/// interface ports: its outermost label names one.
inline bool hier_is_iface_member(const AST::Identifier::Ptr &id,
                                 const std::set<std::string> &iface_ports)
{
    const auto &hier = id->get_hier();
    if(!hier || !hier->get_labellist() || hier->get_labellist()->empty()) {
        return false;
    }
    return iface_ports.count(hier->get_labellist()->front()->get_name()) != 0;
}

/// The conjuncts of a guard, flattening the && tree.
inline void flatten_land(const AST::Node::Ptr &node, std::vector<AST::Node::Ptr> &conjuncts)
{
    if(!node) {
        return;
    }
    if(node->is_node_type(AST::NodeType::Land)) {
        const auto &land = AST::cast_to<AST::Land>(node);
        flatten_land(land->get_left(), conjuncts);
        flatten_land(land->get_right(), conjuncts);
        return;
    }
    conjuncts.push_back(node);
}

/// §9 on system functions: outside the constant/query subset every
/// synthesis flow accepts, a call has no stable value — and the walk
/// forks on conditions, reuses them across guards and prunes their
/// contradictions, all assuming an expression reads stably within its
/// zero-time segment. $random breaks every one of those moves, so it is
/// rejected wherever it appears, not mis-walked.
inline int check_impure_calls(const AST::Node::Ptr &expr)
{
    if(!expr) {
        return 0;
    }
    if(expr->is_node_type(AST::NodeType::SystemCall)) {
        // §20.5-§20.8 queries and casts, plus the §20.9 bit-vector
        // functions — every one stable and synthesizable.
        static const std::set<std::string> pure = {"clog2",     "bits",   "size",     "left",
                                                   "right",     "signed", "unsigned", "countones",
                                                   "countbits", "onehot", "onehot0",  "isunknown"};
        const auto &name = AST::cast_to<AST::SystemCall>(expr)->get_syscall();
        if(!pure.count(name)) {
            LOG_ERROR_N(expr) << "system function '$" << name << "' in a marked process: "
                              << "outside the constant/query subset it has no stable value, "
                              << "and the walk assumes expressions read stably within their "
                              << "zero-time segment";
            return 1;
        }
    }
    const auto &children = expr->get_children();
    for(const auto &child : *children) {
        if(check_impure_calls(child)) {
            return 1;
        }
    }
    return 0;
}

/// §9: a function called in a marked process must not write non-local
/// state — an output argument included: expression position is no place
/// for a side effect, and the (R_p, s_p) model would miss the write
/// silently. Pure functions pass through as the ordinary combinational
/// calls they are.
int check_called_functions(const AST::Module::Ptr &module, const AST::Initial::Ptr &initial);

inline void collect_lvalue_bases(const AST::Node::Ptr &node, std::set<std::string> &names)
{
    Analysis::Statement::collect_lvalue_bases(node, names);
}

/// The name a header port answers to: the inner declaration's for an ANSI
/// port, the Port's own for a non-ANSI name-only reference.
inline std::string header_port_name(const AST::Port::Ptr &port)
{
    const auto &decl = port->get_decl();
    return decl ? decl->get_name() : port->get_name();
}

/// A child module port's direction, ANSI or non-ANSI: the header's when it
/// carries one, else the body direction declaration of that name — which
/// is where a non-ANSI module states it.
inline AST::Port::DirectionEnum child_port_direction(const AST::Module::Ptr &definition,
                                                     const std::string &name)
{
    const auto &ports = definition->get_ports();
    if(ports) {
        for(const auto &port : *ports) {
            if(header_port_name(port) == name &&
               port->get_direction() != AST::Port::DirectionEnum::NONE) {
                return port->get_direction();
            }
        }
    }
    const auto &items = definition->get_items();
    if(items) {
        for(const auto &item : *items) {
            if(!item->is_node_type(AST::NodeType::Port)) {
                continue;
            }
            const auto &port = AST::cast_to<AST::Port>(item);
            if(header_port_name(port) == name &&
               port->get_direction() != AST::Port::DirectionEnum::NONE) {
                return port->get_direction();
            }
        }
    }
    return AST::Port::DirectionEnum::NONE;
}

inline void collect_call_names(const AST::Node::Ptr &node, std::set<std::string> &names)
{
    Analysis::Statement::collect_call_names(node, names);
}

inline void collect_driven(const AST::Node::Ptr &node, std::set<std::string> &names)
{
    Analysis::Statement::collect_driven(node, names);
}

inline void collect_declaration_names(const AST::Node::Ptr &node, std::set<std::string> &names)
{
    Analysis::Statement::collect_declaration_names(node, names);
}

/// §13.5.2 classification lives in Analysis::StorageKind.
inline bool is_net_signal(const AST::Module::Ptr &module, const std::string &name)
{
    return Analysis::StorageKind::is_net(module, name);
}
inline AST::Identifier::Ptr select_base(const AST::Node::Ptr &node)
{
    return Analysis::StorageKind::select_base(node);
}

inline bool complements(const AST::Node::Ptr &a, const AST::Node::Ptr &b)
{
    if(a->is_node_type(AST::NodeType::Ulnot)) {
        const auto &operand = AST::cast_to<AST::Ulnot>(a)->get_right();
        if(operand && operand->is_equal(b, false)) {
            return true;
        }
    }
    if(b->is_node_type(AST::NodeType::Ulnot)) {
        const auto &operand = AST::cast_to<AST::Ulnot>(b)->get_right();
        if(operand && operand->is_equal(a, false)) {
            return true;
        }
    }
    const bool a_eq = a->is_node_type(AST::NodeType::Eq);
    const bool b_eq = b->is_node_type(AST::NodeType::Eq);
    const bool a_neq = a->is_node_type(AST::NodeType::NotEq);
    const bool b_neq = b->is_node_type(AST::NodeType::NotEq);
    if((a_eq && b_neq) || (a_neq && b_eq)) {
        const auto &lhs = AST::cast_to<AST::Operator>(a);
        const auto &rhs = AST::cast_to<AST::Operator>(b);
        if(lhs->get_left() && rhs->get_left() && lhs->get_right() && rhs->get_right() &&
           lhs->get_left()->is_equal(rhs->get_left(), false) &&
           lhs->get_right()->is_equal(rhs->get_right(), false)) {
            return true;
        }
    }
    if(a_eq && b_eq) {
        const auto &lhs = AST::cast_to<AST::Eq>(a);
        const auto &rhs = AST::cast_to<AST::Eq>(b);
        mpz_class lval, rval;
        if(lhs->get_left() && rhs->get_left() &&
           lhs->get_left()->is_equal(rhs->get_left(), false) &&
           ExpressionEvaluation().evaluate_node(lhs->get_right(), lval) &&
           ExpressionEvaluation().evaluate_node(rhs->get_right(), rval) && lval != rval) {
            return true;
        }
    }
    return false;
}

/// Whether adding @p extra to feasible @p conjuncts creates a
/// contradiction: a complementary pair involving the newcomer, or a
/// negated conjunction — !(a && b) — whose flattened parts are now all
/// individually present. Guards only ever grow through conjoin, so
/// checking against the newcomer keeps the whole set screened.
inline bool contradicts(const std::vector<AST::Node::Ptr> &conjuncts, const AST::Node::Ptr &extra)
{
    for(const auto &conjunct : conjuncts) {
        if(complements(conjunct, extra)) {
            return true;
        }
    }
    std::vector<AST::Node::Ptr> all = conjuncts;
    all.push_back(extra);
    for(const auto &negation : all) {
        if(!negation->is_node_type(AST::NodeType::Ulnot)) {
            continue;
        }
        const auto &negated = AST::cast_to<AST::Ulnot>(negation)->get_right();
        if(!negated || !negated->is_node_type(AST::NodeType::Land)) {
            continue;
        }
        std::vector<AST::Node::Ptr> parts;
        flatten_land(negated, parts);
        bool covered = true;
        for(const auto &part : parts) {
            bool present = false;
            for(const auto &conjunct : all) {
                if(conjunct != negation && conjunct->is_equal(part, false)) {
                    present = true;
                    break;
                }
            }
            covered &= present;
        }
        if(covered) {
            return true;
        }
    }
    return false;
}

/// Conjoin a path condition onto a guard; a null side passes the other
/// through, so unconditional composes as identity. A condition the guard
/// already carries — the same test forked twice along one path — adds
/// nothing and is dropped. With @p dead supplied, a condition that
/// contradicts the guard marks the leg as the empty path (§C.4): no
/// execution takes it, so the caller drops it before §6 or the emission
/// ever see it — soundly, since removing an empty piece keeps the
/// remaining legs a partition.
inline AST::Node::Ptr conjoin(const AST::Node::Ptr &guard, const AST::Node::Ptr &extra,
                              const std::string &fn, int ln, bool *dead = nullptr)
{
    if(!extra) {
        return guard;
    }
    // The condition joins part by part, so a compound test dedups and
    // screens against the guard's conjuncts exactly like a simple one.
    std::vector<AST::Node::Ptr> parts;
    flatten_land(extra, parts);
    AST::Node::Ptr result = guard;
    for(const auto &part : parts) {
        if(!result) {
            result = part;
            continue;
        }
        std::vector<AST::Node::Ptr> conjuncts;
        flatten_land(result, conjuncts);
        bool present = false;
        for(const auto &conjunct : conjuncts) {
            if(conjunct->is_equal(part, false)) {
                present = true;
                break;
            }
        }
        if(present) {
            continue;
        }
        if(dead && contradicts(conjuncts, part)) {
            *dead = true;
            return nullptr;
        }
        result = make_land(result, part, fn, ln);
    }
    return result;
}

/// §6.1 substitution: read positions take their environment value; write
/// positions (Lvalue subtrees) are left alone; a null value marks a
/// branch-dependent name the reads check refuses, left in place. The
/// engine is ASTReplace::substitute_values; a root identifier substitutes
/// directly, there being no parent to relink.
inline AST::Node::Ptr subst_into(AST::Node::Ptr node,
                                 const std::map<std::string, AST::Node::Ptr> &env)
{
    if(!node) {
        return node;
    }
    if(node->is_node_type(AST::NodeType::Identifier)) {
        const auto &id = AST::cast_to<AST::Identifier>(node);
        if(!id->get_hier()) {
            const auto &found = env.find(id->get_name());
            if(found != env.end() && found->second) {
                return found->second->clone();
            }
            return node;
        }
    }
    ASTReplace::substitute_values(node, env);
    return node;
}

/// A fresh expression for a guard or an action: cloned, with the segment's
/// induced-register values substituted in (§6.1).
inline AST::Node::Ptr clone_subst(const AST::Node::Ptr &node,
                                  const std::map<std::string, AST::Node::Ptr> &env)
{
    return env.empty() ? node->clone() : subst_into(node->clone(), env);
}

/// The module-level declaration behind a name — a body item or an ANSI
/// port's inner declaration — or null. @p is_input reports whether the
/// name is an input or inout port, which no process may drive.
inline AST::Declaration::Ptr find_declaration(const AST::Module::Ptr &module,
                                              const std::string &name, bool *is_input = nullptr,
                                              bool *is_port = nullptr)
{
    const auto &ports = module->get_ports();
    if(ports) {
        for(const auto &port : *ports) {
            const auto &decl = port->get_decl();
            if(decl && decl->get_name() == name) {
                if(is_input) {
                    *is_input = port->get_direction() == AST::Port::DirectionEnum::INPUT ||
                                port->get_direction() == AST::Port::DirectionEnum::INOUT;
                }
                if(is_port) {
                    *is_port = true;
                }
                return decl;
            }
        }
    }
    const auto &items = module->get_items();
    if(items) {
        for(const auto &item : *items) {
            const auto &decl = std::dynamic_pointer_cast<AST::Declaration>(item);
            if(decl && decl->get_name() == name) {
                if(is_input) {
                    *is_input = false;
                }
                return decl;
            }
        }
    }
    return nullptr;
}

/// The declared packed width of a declaration: the packed dimension when
/// it folds to constants, the keyword width for the integer atom types
/// (IEEE 1800-2017 §6.11), one bit for a scalar vector type.
inline int declared_width(const AST::Declaration::Ptr &decl, unsigned int &width)
{
    const auto &type = decl->get_type();
    if(!type) {
        return 1;
    }
    const auto &dims = type->get_packed_dims();
    if(!dims || dims->empty()) {
        switch(type->get_node_type()) {
        case AST::NodeType::ByteType:
            width = 8;
            return 0;
        case AST::NodeType::ShortintType:
            width = 16;
            return 0;
        case AST::NodeType::IntType:
        case AST::NodeType::IntegerType:
            width = 32;
            return 0;
        case AST::NodeType::LongintType:
        case AST::NodeType::TimeType:
            width = 64;
            return 0;
        case AST::NodeType::LogicType:
        case AST::NodeType::RegType:
        case AST::NodeType::BitType:
            width = 1;
            return 0;
        default:
            return 1;
        }
    }
    if(dims->size() != 1 || !dims->front()->is_node_type(AST::NodeType::RangeDim)) {
        return 1;
    }
    const auto &range = AST::cast_to<AST::RangeDim>(dims->front());
    mpz_class left, right;
    if(!ExpressionEvaluation().evaluate_node(range->get_left(), left) ||
       !ExpressionEvaluation().evaluate_node(range->get_right(), right)) {
        return 1;
    }
    const mpz_class span = left >= right ? left - right : right - left;
    width = span.convert_to<unsigned int>() + 1;
    return 0;
}

inline AST::Node::ListPtr copy_list(const AST::Node::ListPtr &list)
{
    auto result = std::make_shared<AST::Node::List>();
    for(const auto &stmt : *list) {
        result->push_back(stmt);
    }
    return result;
}

inline void collect_reads(const AST::Node::Ptr &node, std::set<std::string> &reads)
{
    Analysis::Statement::collect_reads(node, reads);
}

/// Register targets of a statement run, recursing through the branches an
/// action may keep verbatim (§4).
inline int collect_targets(const AST::Node::Ptr &node, std::set<std::string> &targets)
{
    if(!node) {
        return 0;
    }
    switch(node->get_node_type()) {
    case AST::NodeType::Block: {
        const auto &stmts = AST::cast_to<AST::Block>(node)->get_statements();
        if(stmts) {
            for(const auto &stmt : *stmts) {
                if(collect_targets(stmt, targets)) {
                    return 1;
                }
            }
        }
        return 0;
    }
    case AST::NodeType::NonblockingSubstitution: {
        const auto &target = nba_target(AST::cast_to<AST::NonblockingSubstitution>(node));
        if(target.empty()) {
            LOG_ERROR_N(node) << "nonblocking target is not a plain register "
                              << "identifier: not handled by the lowering yet";
            return 1;
        }
        targets.insert(target);
        return 0;
    }
    case AST::NodeType::IfStatement: {
        const auto &ifs = AST::cast_to<AST::IfStatement>(node);
        if(collect_targets(ifs->get_true_statement(), targets)) {
            return 1;
        }
        return collect_targets(ifs->get_false_statement(), targets);
    }
    case AST::NodeType::CaseStatement: {
        const auto &caselist = AST::cast_to<AST::CaseStatement>(node)->get_caselist();
        if(caselist) {
            for(const auto &arm : *caselist) {
                if(collect_targets(arm->get_statement(), targets)) {
                    return 1;
                }
            }
        }
        return 0;
    }
    default:
        return 0;
    }
}

inline int collect_targets_list(const AST::Node::ListPtr &stmts, std::set<std::string> &targets)
{
    for(const auto &stmt : *stmts) {
        if(collect_targets(stmt, targets)) {
            return 1;
        }
    }
    return 0;
}

/// Per register, the largest number of commits on any runtime path through
/// the statement run: sequence adds, a branch contributes its worst arm.
inline void max_commits(const AST::Node::Ptr &node, std::map<std::string, int> &counts);

inline void max_commits_list(const AST::Node::ListPtr &stmts, std::map<std::string, int> &counts)
{
    for(const auto &stmt : *stmts) {
        max_commits(stmt, counts);
    }
}

inline void max_commits(const AST::Node::Ptr &node, std::map<std::string, int> &counts)
{
    if(!node) {
        return;
    }
    switch(node->get_node_type()) {
    case AST::NodeType::Block: {
        const auto &stmts = AST::cast_to<AST::Block>(node)->get_statements();
        if(stmts) {
            for(const auto &stmt : *stmts) {
                max_commits(stmt, counts);
            }
        }
        break;
    }
    case AST::NodeType::NonblockingSubstitution:
        counts[nba_target(AST::cast_to<AST::NonblockingSubstitution>(node))] += 1;
        break;
    case AST::NodeType::IfStatement: {
        const auto &ifs = AST::cast_to<AST::IfStatement>(node);
        std::map<std::string, int> true_counts, false_counts;
        max_commits(ifs->get_true_statement(), true_counts);
        max_commits(ifs->get_false_statement(), false_counts);
        for(const auto &elt : false_counts) {
            auto &count = true_counts[elt.first];
            count = std::max(count, elt.second);
        }
        for(const auto &elt : true_counts) {
            counts[elt.first] += elt.second;
        }
        break;
    }
    case AST::NodeType::CaseStatement: {
        const auto &caselist = AST::cast_to<AST::CaseStatement>(node)->get_caselist();
        std::map<std::string, int> worst;
        if(caselist) {
            for(const auto &arm : *caselist) {
                std::map<std::string, int> arm_counts;
                max_commits(arm->get_statement(), arm_counts);
                for(const auto &elt : arm_counts) {
                    auto &count = worst[elt.first];
                    count = std::max(count, elt.second);
                }
            }
        }
        for(const auto &elt : worst) {
            counts[elt.first] += elt.second;
        }
        break;
    }
    default:
        break;
    }
}

} // namespace FsmDetail

/// §6.2 stability support, defined in implicit_fsm_checks.cpp.
void collect_foreign_drivers(const AST::Module::Ptr &module, const AST::Pragmalist::Ptr &pragmalist,
                             const Analysis::Module::ModulesMap *modules,
                             std::set<std::string> &others);

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse
