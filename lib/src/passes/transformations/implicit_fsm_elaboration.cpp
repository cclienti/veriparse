// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/passes/transformations/implicit_fsm_elaboration.hpp>
#include <veriparse/passes/transformations/expression_evaluation.hpp>
#include <veriparse/passes/analysis/module.hpp>
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

namespace
{

bool has_pragma(const AST::Pragmalist::Ptr &pragmalist, const std::string &name)
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

AST::Pragma::Ptr get_pragma(const AST::Pragmalist::Ptr &pragmalist, const std::string &name)
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

bool contains_event_statement(const AST::Node::Ptr &node)
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

/// Whether the subtree holds a break or continue: a jump transfers control,
/// so a branch carrying one forks the path walk even with no cut point in
/// its arms (§8). A jump bound to a loop nested inside the branch would be
/// a false positive, but such a loop holds cut points of its own, so the
/// branch forks regardless.
bool contains_jump(const AST::Node::Ptr &node)
{
    if(!node) {
        return false;
    }
    if(node->is_node_type(AST::NodeType::Break) || node->is_node_type(AST::NodeType::Continue)) {
        return true;
    }
    const auto &children = node->get_children();
    for(const auto &child : *children) {
        if(contains_jump(child)) {
            return true;
        }
    }
    return false;
}

void collect_identifier_names(const AST::Node::Ptr &node, std::set<std::string> &names)
{
    if(!node) {
        return;
    }
    if(node->is_node_type(AST::NodeType::Identifier)) {
        names.insert(AST::cast_to<AST::Identifier>(node)->get_name());
        return;
    }
    const auto &children = node->get_children();
    for(const auto &child : *children) {
        collect_identifier_names(child, names);
    }
}

/// The plain-identifier target of an assignment's left-hand side, or empty
/// when the shape is outside the subset.
std::string lvalue_target(const AST::Lvalue::Ptr &lvalue)
{
    if(!lvalue || !lvalue->get_var()) {
        return "";
    }
    if(!lvalue->get_var()->is_node_type(AST::NodeType::Identifier)) {
        return "";
    }
    return AST::cast_to<AST::Identifier>(lvalue->get_var())->get_name();
}

/// The target register of a straight-line nonblocking assignment.
std::string nba_target(const AST::NonblockingSubstitution::Ptr &nba)
{
    return lvalue_target(nba->get_left());
}

/// The target of a for's blocking init or step (§7.2).
std::string nba_like_target(const AST::BlockingSubstitution::Ptr &assign)
{
    return lvalue_target(assign->get_left());
}

std::string to_lower(const std::string &str)
{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

unsigned int clog2(unsigned int value)
{
    unsigned int width = 0;
    for(unsigned int remaining = value - 1; remaining != 0; remaining >>= 1) {
        ++width;
    }
    return width == 0 ? 1 : width;
}

AST::IntConstN::Ptr make_const(unsigned int value, int size, const std::string &fn, int ln)
{
    auto node = std::make_shared<AST::IntConstN>(fn, ln);
    node->set_base(10);
    node->set_size(size);
    node->set_sign(false);
    node->set_value(value);
    return node;
}

AST::Identifier::Ptr make_id(const std::string &name, const std::string &fn, int ln)
{
    auto node = std::make_shared<AST::Identifier>(fn, ln);
    node->set_name(name);
    return node;
}

AST::Dimension::ListPtr make_packed_range(unsigned int msb, const std::string &fn, int ln)
{
    auto range = std::make_shared<AST::RangeDim>(fn, ln);
    range->set_left(AST::to_node(make_const(msb, -1, fn, ln)));
    range->set_right(AST::to_node(make_const(0, -1, fn, ln)));
    auto dims = std::make_shared<AST::Dimension::List>();
    dims->push_back(range);
    return dims;
}

AST::NonblockingSubstitution::Ptr make_state_assign(const std::string &state_reg,
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
constexpr std::size_t k_entry = static_cast<std::size_t>(-1);

AST::Node::Ptr make_ulnot(const AST::Node::Ptr &expr, const std::string &fn, int ln)
{
    auto node = std::make_shared<AST::Ulnot>(fn, ln);
    node->set_right(expr);
    return AST::to_node(node);
}

AST::Node::Ptr make_land(const AST::Node::Ptr &left, const AST::Node::Ptr &right,
                         const std::string &fn, int ln)
{
    auto node = std::make_shared<AST::Land>(fn, ln);
    node->set_left(left);
    node->set_right(right);
    return AST::to_node(node);
}

AST::Node::Ptr make_lor(const AST::Node::Ptr &left, const AST::Node::Ptr &right,
                        const std::string &fn, int ln)
{
    auto node = std::make_shared<AST::Lor>(fn, ln);
    node->set_left(left);
    node->set_right(right);
    return AST::to_node(node);
}

AST::Node::Ptr make_eq(const AST::Node::Ptr &left, const AST::Node::Ptr &right,
                       const std::string &fn, int ln)
{
    auto node = std::make_shared<AST::Eq>(fn, ln);
    node->set_left(left);
    node->set_right(right);
    return AST::to_node(node);
}

AST::Node::Ptr make_noteq(const AST::Node::Ptr &left, const AST::Node::Ptr &right,
                          const std::string &fn, int ln)
{
    auto node = std::make_shared<AST::NotEq>(fn, ln);
    node->set_left(left);
    node->set_right(right);
    return AST::to_node(node);
}

AST::Node::Ptr make_minus(const AST::Node::Ptr &left, const AST::Node::Ptr &right,
                          const std::string &fn, int ln)
{
    auto node = std::make_shared<AST::Minus>(fn, ln);
    node->set_left(left);
    node->set_right(right);
    return AST::to_node(node);
}

AST::NonblockingSubstitution::Ptr make_nba(const std::string &target, const AST::Node::Ptr &rhs,
                                           const std::string &fn, int ln)
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

/// The conjuncts of a guard, flattening the && tree.
void flatten_land(const AST::Node::Ptr &node, std::vector<AST::Node::Ptr> &conjuncts)
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

/// Conjoin a path condition onto a guard; a null side passes the other
/// through, so unconditional composes as identity. A condition the guard
/// already carries — the same test forked twice along one path — adds
/// nothing and is dropped.
AST::Node::Ptr conjoin(const AST::Node::Ptr &guard, const AST::Node::Ptr &extra,
                       const std::string &fn, int ln)
{
    if(!extra) {
        return guard;
    }
    if(!guard) {
        return extra;
    }
    std::vector<AST::Node::Ptr> conjuncts;
    flatten_land(guard, conjuncts);
    for(const auto &conjunct : conjuncts) {
        if(conjunct->is_equal(extra, false)) {
            return guard;
        }
    }
    return make_land(guard, extra, fn, ln);
}

/// Whether a guard is a structural contradiction — some conjunct is the
/// negation of another. Such a path is the empty set: the walk enumerated
/// it syntactically, but no execution takes it, so §6 must not judge it
/// and the emission must not print it. Dropping it keeps the remaining
/// legs a partition — the removed piece was empty.
bool is_infeasible(const AST::Node::Ptr &guard)
{
    if(!guard) {
        return false;
    }
    std::vector<AST::Node::Ptr> conjuncts;
    flatten_land(guard, conjuncts);
    for(std::size_t i = 0; i < conjuncts.size(); ++i) {
        for(std::size_t j = 0; j < conjuncts.size(); ++j) {
            if(i == j || !conjuncts[i]->is_node_type(AST::NodeType::Ulnot)) {
                continue;
            }
            const auto &negated = AST::cast_to<AST::Ulnot>(conjuncts[i])->get_right();
            if(negated && negated->is_equal(conjuncts[j], false)) {
                return true;
            }
        }
    }
    return false;
}

/// §6.1 substitution, in place on a tree the caller owns: identifiers in
/// read position take their environment value; write positions — Lvalue
/// subtrees — are left alone.
AST::Node::Ptr subst_into(AST::Node::Ptr node, const std::map<std::string, AST::Node::Ptr> &env)
{
    if(!node) {
        return node;
    }
    if(node->is_node_type(AST::NodeType::Identifier)) {
        const auto &found = env.find(AST::cast_to<AST::Identifier>(node)->get_name());
        return found != env.end() ? found->second->clone() : node;
    }
    if(node->is_node_type(AST::NodeType::Lvalue)) {
        return node;
    }
    const auto &children = node->get_children();
    for(const auto &child : *children) {
        const auto &replacement = subst_into(child, env);
        if(replacement != child) {
            node->replace(child, replacement);
        }
    }
    return node;
}

/// A fresh expression for a guard or an action: cloned, with the segment's
/// induced-register values substituted in (§6.1).
AST::Node::Ptr clone_subst(const AST::Node::Ptr &node,
                           const std::map<std::string, AST::Node::Ptr> &env)
{
    return env.empty() ? node->clone() : subst_into(node->clone(), env);
}

/// The module-level declaration behind a name — a body item or an ANSI
/// port's inner declaration — or null. @p is_input reports whether the
/// name is an input or inout port, which no process may drive.
AST::Declaration::Ptr find_declaration(const AST::Module::Ptr &module, const std::string &name,
                                       bool *is_input = nullptr)
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
int declared_width(const AST::Declaration::Ptr &decl, unsigned int &width)
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

AST::Node::ListPtr copy_list(const AST::Node::ListPtr &list)
{
    auto result = std::make_shared<AST::Node::List>();
    for(const auto &stmt : *list) {
        result->push_back(stmt);
    }
    return result;
}

/// Reads of a statement run: identifiers in expression position —
/// right-hand sides and branch conditions — never assignment targets.
void collect_reads(const AST::Node::Ptr &node, std::set<std::string> &reads)
{
    if(!node) {
        return;
    }
    switch(node->get_node_type()) {
    case AST::NodeType::Block: {
        const auto &stmts = AST::cast_to<AST::Block>(node)->get_statements();
        if(stmts) {
            for(const auto &stmt : *stmts) {
                collect_reads(stmt, reads);
            }
        }
        break;
    }
    case AST::NodeType::NonblockingSubstitution:
        collect_identifier_names(
            AST::to_node(AST::cast_to<AST::NonblockingSubstitution>(node)->get_right()), reads);
        break;
    case AST::NodeType::IfStatement: {
        const auto &ifs = AST::cast_to<AST::IfStatement>(node);
        collect_identifier_names(ifs->get_cond(), reads);
        collect_reads(ifs->get_true_statement(), reads);
        collect_reads(ifs->get_false_statement(), reads);
        break;
    }
    case AST::NodeType::CaseStatement:
    case AST::NodeType::CasexStatement:
    case AST::NodeType::CasezStatement: {
        const auto &cs = AST::cast_to<AST::CaseStatement>(node);
        collect_identifier_names(cs->get_comp(), reads);
        const auto &caselist = cs->get_caselist();
        if(caselist) {
            for(const auto &arm : *caselist) {
                const auto &conds = arm->get_cond();
                if(conds) {
                    for(const auto &value : *conds) {
                        collect_identifier_names(value, reads);
                    }
                }
                collect_reads(arm->get_statement(), reads);
            }
        }
        break;
    }
    default:
        break;
    }
}

/// Register targets of a statement run, recursing through the branches an
/// action may keep verbatim (§4).
int collect_targets(const AST::Node::Ptr &node, std::set<std::string> &targets)
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

int collect_targets_list(const AST::Node::ListPtr &stmts, std::set<std::string> &targets)
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
void max_commits(const AST::Node::Ptr &node, std::map<std::string, int> &counts);

void max_commits_list(const AST::Node::ListPtr &stmts, std::map<std::string, int> &counts)
{
    for(const auto &stmt : *stmts) {
        max_commits(stmt, counts);
    }
}

void max_commits(const AST::Node::Ptr &node, std::map<std::string, int> &counts)
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

/// Registers assigned on EVERY runtime path through the statement run: a
/// sequence unions, a branch keeps only what all its arms agree on — an
/// `if` with no `else`, or a `case` with no `default`, guarantees nothing.
void must_writes(const AST::Node::Ptr &node, std::set<std::string> &writes);

void must_writes_list(const AST::Node::ListPtr &stmts, std::set<std::string> &writes)
{
    for(const auto &stmt : *stmts) {
        must_writes(stmt, writes);
    }
}

void must_writes(const AST::Node::Ptr &node, std::set<std::string> &writes)
{
    if(!node) {
        return;
    }
    switch(node->get_node_type()) {
    case AST::NodeType::Block: {
        const auto &stmts = AST::cast_to<AST::Block>(node)->get_statements();
        if(stmts) {
            for(const auto &stmt : *stmts) {
                must_writes(stmt, writes);
            }
        }
        break;
    }
    case AST::NodeType::NonblockingSubstitution:
        writes.insert(nba_target(AST::cast_to<AST::NonblockingSubstitution>(node)));
        break;
    case AST::NodeType::IfStatement: {
        const auto &ifs = AST::cast_to<AST::IfStatement>(node);
        if(!ifs->get_false_statement()) {
            break;
        }
        std::set<std::string> true_writes, false_writes;
        must_writes(ifs->get_true_statement(), true_writes);
        must_writes(ifs->get_false_statement(), false_writes);
        for(const auto &name : true_writes) {
            if(false_writes.count(name)) {
                writes.insert(name);
            }
        }
        break;
    }
    case AST::NodeType::CaseStatement: {
        const auto &caselist = AST::cast_to<AST::CaseStatement>(node)->get_caselist();
        if(!caselist || caselist->empty()) {
            break;
        }
        bool has_default = false;
        bool first = true;
        std::set<std::string> agreed;
        for(const auto &arm : *caselist) {
            const auto &conds = arm->get_cond();
            if(!conds || conds->empty()) {
                has_default = true;
            }
            std::set<std::string> arm_writes;
            must_writes(arm->get_statement(), arm_writes);
            if(first) {
                agreed = arm_writes;
                first = false;
            } else {
                std::set<std::string> kept;
                for(const auto &name : agreed) {
                    if(arm_writes.count(name)) {
                        kept.insert(name);
                    }
                }
                agreed = kept;
            }
        }
        if(has_default) {
            writes.insert(agreed.begin(), agreed.end());
        }
        break;
    }
    default:
        break;
    }
}

} // namespace

int ImplicitFsmElaboration::process(AST::Node::Ptr node, AST::Node::Ptr parent)
{
    if(!node) {
        return 0;
    }

    if(!node->is_node_type(AST::NodeType::Module)) {
        return recurse_in_childs(node);
    }

    const auto &module = AST::cast_to<AST::Module>(node);
    const auto &items = module->get_items();
    if(!items) {
        return 0;
    }

    // Marked processes are collected first: replacing an item invalidates
    // iteration, and the count decides the prefix — several processes get an
    // ordinal each, one shared prefix would collide by construction (§10).
    std::vector<std::pair<AST::Pragmalist::Ptr, AST::Initial::Ptr>> marked;
    int ret = 0;

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
            if(stmt->is_node_type(AST::NodeType::Initial)) {
                marked.emplace_back(pragmalist, AST::cast_to<AST::Initial>(stmt));
            } else if(stmt->is_node_category(AST::NodeType::Always)) {
                if(contains_event_statement(AST::cast_to<AST::Always>(stmt)->get_statement())) {
                    LOG_ERROR_N(stmt) << "(* veriparse_fsm *) on an always process holding "
                                      << "event controls: not compiled — rewrite the body as "
                                      << "'initial forever begin ... end' and drop the senslist";
                    ret += 1;
                } else {
                    LOG_WARNING_N(stmt)
                        << "(* veriparse_fsm *) has no effect: this process is already RTL";
                }
            } else {
                LOG_ERROR_N(stmt) << "(* veriparse_fsm *) on an item that is not a "
                                  << "process: there is nothing to compile";
                ret += 1;
            }
        }
    }

    if(ret) {
        return ret;
    }

    std::size_t ordinal = 0;
    for(const auto &elt : marked) {
        const std::string prefix =
            (marked.size() > 1) ? ("__fsm" + std::to_string(ordinal)) : "__fsm";
        ret += compile_process(module, node, elt.first, elt.second, prefix);
        ++ordinal;
    }

    return ret;
}

int ImplicitFsmElaboration::collect_body(const AST::Node::Ptr &node,
                                         std::vector<AST::EventStatement::Ptr> &waits,
                                         AST::Sens::Ptr &clock, bool &has_wait)
{
    if(!node) {
        return 0;
    }

    switch(node->get_node_type()) {
    case AST::NodeType::Block: {
        const auto &statements = AST::cast_to<AST::Block>(node)->get_statements();
        if(statements) {
            for(const auto &stmt : *statements) {
                if(collect_body(stmt, waits, clock, has_wait)) {
                    return 1;
                }
            }
        }
        return 0;
    }

    case AST::NodeType::EventStatement: {
        const auto &event = AST::cast_to<AST::EventStatement>(node);
        if(check_wait(event, clock)) {
            return 1;
        }
        m_wait_index[event.get()] = waits.size();
        waits.push_back(event);
        has_wait = true;
        // `@(posedge clk) stmt;` attaches the statement to the wait: it runs
        // after the edge, so it belongs to the following segment.
        return collect_body(event->get_statement(), waits, clock, has_wait);
    }

    case AST::NodeType::NonblockingSubstitution:
        return 0;

    case AST::NodeType::IfStatement: {
        const auto &ifs = AST::cast_to<AST::IfStatement>(node);
        bool arms_wait = false;
        if(collect_body(ifs->get_true_statement(), waits, clock, arms_wait)) {
            return 1;
        }
        if(collect_body(ifs->get_false_statement(), waits, clock, arms_wait)) {
            return 1;
        }
        if(arms_wait || contains_jump(node)) {
            m_forking.insert(node.get());
        }
        has_wait |= arms_wait;
        return 0;
    }

    case AST::NodeType::CaseStatement: {
        const auto &caselist = AST::cast_to<AST::CaseStatement>(node)->get_caselist();
        bool arms_wait = false;
        std::size_t defaults = 0;
        if(caselist) {
            for(const auto &arm : *caselist) {
                const auto &conds = arm->get_cond();
                if(!conds || conds->empty()) {
                    ++defaults;
                }
                if(collect_body(arm->get_statement(), waits, clock, arms_wait)) {
                    return 1;
                }
            }
        }
        // The grammar admits several default arms; IEEE 1800-2017 §12.5
        // allows at most one, and the guard construction has no condition
        // to give a second one.
        if(defaults > 1) {
            LOG_ERROR_N(node) << "case with " << defaults << " default arms in a marked "
                              << "process: at most one (IEEE 1800-2017 §12.5, ADR-0014 §9)";
            return 1;
        }
        // A forking case is if-converted with `==`, but item matching is
        // case equality (IEEE 1800-2017 §12.5): an item with x/z bits can
        // never satisfy the guard, so it is rejected, not silently dropped.
        // A cut-point-free case stays verbatim and keeps its semantics.
        const bool forking = arms_wait || contains_jump(node);
        if(forking && caselist) {
            for(const auto &arm : *caselist) {
                const auto &conds = arm->get_cond();
                if(!conds) {
                    continue;
                }
                for(const auto &value : *conds) {
                    if(!value->is_node_type(AST::NodeType::IntConst)) {
                        continue;
                    }
                    const auto &text = AST::cast_to<AST::IntConst>(value)->get_value();
                    if(text.find_first_of("xXzZ?") != std::string::npos) {
                        LOG_ERROR_N(value) << "case item '" << text << "' with x/z bits in a case "
                                           << "holding cut points: the fork guard uses logical "
                                           << "equality, which such an item never satisfies "
                                           << "(IEEE 1800-2017 §12.5, ADR-0014 §9)";
                        return 1;
                    }
                }
            }
        }
        if(forking) {
            m_forking.insert(node.get());
        }
        has_wait |= arms_wait;
        return 0;
    }

    case AST::NodeType::CasexStatement:
    case AST::NodeType::CasezStatement:
        LOG_ERROR_N(node) << "casex/casez in a marked process: wildcard matching "
                          << "is not handled by the lowering yet (ADR-0014 §9)";
        return 1;

    case AST::NodeType::Pragmalist: {
        // (* veriparse_no_unroll *) directly on a loop keeps it rolled
        // (§7.2); any other statement pragma is transparent.
        const auto &pragmalist = AST::cast_to<AST::Pragmalist>(node);
        const bool no_unroll = has_pragma(pragmalist, "veriparse_no_unroll");
        const auto &statements = pragmalist->get_statements();
        if(statements) {
            for(const auto &stmt : *statements) {
                const bool loop = stmt->is_node_type(AST::NodeType::RepeatStatement) ||
                                  stmt->is_node_type(AST::NodeType::ForStatement) ||
                                  stmt->is_node_type(AST::NodeType::WhileStatement);
                if(no_unroll && loop) {
                    if(collect_loop(stmt, true, waits, clock, has_wait)) {
                        return 1;
                    }
                } else if(collect_body(stmt, waits, clock, has_wait)) {
                    return 1;
                }
            }
        }
        return 0;
    }

    case AST::NodeType::WhileStatement:
        return collect_loop(node, false, waits, clock, has_wait);

    case AST::NodeType::RepeatStatement:
    case AST::NodeType::ForStatement:
        return collect_loop(node, false, waits, clock, has_wait);

    case AST::NodeType::ForeverStatement:
        // §2: the perpetual form — the §7.3 back-edge with no exit test.
        return collect_loop(node, false, waits, clock, has_wait);

    case AST::NodeType::Break:
    case AST::NodeType::Continue:
        // §8: a CFG edge — resolved during the path walk, where the
        // innermost enclosing loop is known.
        return 0;

    case AST::NodeType::SingleStatement: {
        const auto &single = AST::cast_to<AST::SingleStatement>(node);
        if(single->get_delay()) {
            LOG_ERROR_N(node) << "'#' delay in a marked process: simulation timing "
                              << "with no hardware meaning (ADR-0014 §9)";
            return 1;
        }
        return collect_body(single->get_statement(), waits, clock, has_wait);
    }

    case AST::NodeType::BlockingSubstitution:
        LOG_ERROR_N(node) << "blocking assignment in a marked process: '=' names a "
                          << "combinational value the lowering does not handle yet "
                          << "— a register takes '<=' (ADR-0014 §6)";
        return 1;

    case AST::NodeType::DelayStatement:
        LOG_ERROR_N(node) << "'#' delay in a marked process: simulation timing "
                          << "with no hardware meaning (ADR-0014 §9)";
        return 1;

    case AST::NodeType::SystemCall:
        LOG_ERROR_N(node) << "system task in a marked process: no hardware "
                          << "meaning — the mark landed on testbench code? (ADR-0014 §9)";
        return 1;

    case AST::NodeType::WaitStatement:
        LOG_ERROR_N(node) << "level-sensitive wait in a marked process: not an "
                          << "edge, no boundary to cut at (ADR-0014 §9)";
        return 1;

    default:
        LOG_ERROR_N(node) << "construct not handled by the FSM lowering: " << node->get_node_type();
        return 1;
    }
}

int ImplicitFsmElaboration::collect_loop(const AST::Node::Ptr &node, bool kept_rolled,
                                         std::vector<AST::EventStatement::Ptr> &waits,
                                         AST::Sens::Ptr &clock, bool &has_wait)
{
    LoopInfo info;
    switch(node->get_node_type()) {
    case AST::NodeType::WhileStatement: {
        const auto &loop = AST::cast_to<AST::WhileStatement>(node);
        info.kind = LoopInfo::Kind::WHILE;
        info.cond = loop->get_cond();
        info.body = loop->get_statement();
        break;
    }
    case AST::NodeType::ForeverStatement: {
        info.kind = LoopInfo::Kind::FOREVER;
        info.body = AST::cast_to<AST::ForeverStatement>(node)->get_statement();
        break;
    }
    case AST::NodeType::RepeatStatement: {
        const auto &loop = AST::cast_to<AST::RepeatStatement>(node);
        info.kind = LoopInfo::Kind::REPEAT;
        info.cond = loop->get_times();
        info.body = loop->get_statement();
        mpz_class value;
        if(ExpressionEvaluation().evaluate_node(info.cond, value)) {
            // §12.7.2 gives x/z a meaning (zero) but a negative count none,
            // and tools disagree on one: almost always a parameterization
            // off-by-N, so it is rejected rather than silently guessed.
            if(value < 0) {
                LOG_ERROR_N(info.cond)
                    << "repeat count folds to " << value << ": a loop cannot execute "
                    << "a negative number of times — a parameterization off-by-N? "
                    << "(IEEE 1800-2017 §12.7.2, ADR-0014 §7.2, §9)";
                return 1;
            }
            if(value > mpz_class(0xFFFFFFFFUL)) {
                LOG_ERROR_N(info.cond)
                    << "repeat count folds to " << value << ": beyond any countdown "
                    << "the lowering will size (ADR-0014 §7.2, §9)";
                return 1;
            }
            info.count_known = true;
            info.count_value = value.convert_to<unsigned long>();
        }
        break;
    }
    case AST::NodeType::ForStatement: {
        const auto &loop = AST::cast_to<AST::ForStatement>(node);
        info.kind = LoopInfo::Kind::FOR;
        info.cond = loop->get_cond();
        info.body = loop->get_statement();
        const auto &pre = loop->get_pre();
        const auto &post = loop->get_post();
        if(!pre || !post || !info.cond) {
            LOG_ERROR_N(node) << "a rolled for honours the construct's full contract and "
                              << "needs all three of init, test and step (ADR-0014 §7.2)";
            return 1;
        }
        const auto &pre_target = nba_like_target(pre);
        const auto &post_target = nba_like_target(post);
        if(pre_target.empty() || pre_target != post_target) {
            LOG_ERROR_N(node) << "a rolled for's init and step assign the same plain index "
                              << "register (ADR-0014 §7.2)";
            return 1;
        }
        info.index = pre_target;
        info.init_rhs = pre->get_right() ? pre->get_right()->get_var() : nullptr;
        info.step_rhs = post->get_right() ? post->get_right()->get_var() : nullptr;
        if(!info.init_rhs || !info.step_rhs) {
            LOG_ERROR_N(node) << "a rolled for's init and step carry plain expressions "
                              << "(ADR-0014 §7.2)";
            return 1;
        }
        break;
    }
    default:
        return 1;
    }

    // §15 gives the process one shared countdown; nesting counting repeats
    // would have the inner reload clobber the outer's remaining count.
    // Sequential repeats re-initialise on entry and share it soundly.
    const bool counting_repeat =
        info.kind == LoopInfo::Kind::REPEAT && (!info.count_known || info.count_value >= 2);
    if(counting_repeat) {
        if(m_repeat_depth > 0) {
            LOG_ERROR_N(node) << "nested rolled repeats: both would drive the one shared "
                              << "countdown, the inner reload clobbering the outer count — "
                              << "unroll one of them (ADR-0014 §7.2, §9, §15)";
            return 1;
        }
        ++m_repeat_depth;
    }
    bool body_wait = false;
    const int body_rc = collect_body(info.body, waits, clock, body_wait);
    if(counting_repeat) {
        --m_repeat_depth;
    }
    if(body_rc) {
        return 1;
    }

    if(!body_wait) {
        if(kept_rolled) {
            LOG_ERROR_N(node) << "(* veriparse_no_unroll *) on a loop without a cut point: "
                              << "the loop runs in zero time and there is no state to save — "
                              << "drop the hint and let it unroll (ADR-0014 §7.2)";
        } else {
            LOG_ERROR_N(node) << "loop with no cut point survived to the FSM lowering: no "
                              << "static exit, or the unroller refused it — a zero-delay "
                              << "loop has no hardware meaning (IEEE 1800-2017 §9.2.2.1, "
                              << "ADR-0014 §9)";
        }
        return 1;
    }

    // §7.2: the rolled lowering is forced on a bounded loop the unroller
    // left behind — a non-constant bound is no longer an error — but the
    // author should know the state count changed hands.
    if(!kept_rolled && info.kind != LoopInfo::Kind::WHILE && info.kind != LoopInfo::Kind::FOREVER) {
        LOG_WARNING_N(node) << "bounded loop with a cut point was not unrolled upstream: "
                            << "compiled rolled — mark it (* veriparse_no_unroll *) to make "
                            << "that explicit (ADR-0014 §7.2, §8)";
    }

    m_loops[node.get()] = info;
    has_wait = true;
    return 0;
}

void ImplicitFsmElaboration::push_frame(std::vector<Frame> &frames, const AST::Node::Ptr &node,
                                        const AST::Node *loop)
{
    if(!node) {
        return;
    }
    if(node->is_node_type(AST::NodeType::Block)) {
        const auto &stmts = AST::cast_to<AST::Block>(node)->get_statements();
        if(stmts) {
            frames.push_back(Frame{stmts, stmts->begin(), loop});
        }
        return;
    }
    if(node->is_node_type(AST::NodeType::Pragmalist)) {
        const auto &stmts = AST::cast_to<AST::Pragmalist>(node)->get_statements();
        if(stmts) {
            frames.push_back(Frame{stmts, stmts->begin(), loop});
        }
        return;
    }
    auto single = std::make_shared<AST::Node::List>();
    single->push_back(node);
    frames.push_back(Frame{single, single->begin(), loop});
}

int ImplicitFsmElaboration::walk_paths(std::size_t from, const AST::Node::Ptr &guard,
                                       AST::Node::ListPtr action, std::vector<Frame> frames,
                                       Env env, std::set<const AST::Node *> lapped,
                                       std::vector<State> &states, std::vector<Transition> &entry)
{
    // A structurally contradictory guard names the empty path — the same
    // condition forked both ways along one walk, like taking an
    // `if (!send)` and then skipping its `while (!send)` in the same zero
    // time. No execution takes it; it must neither trip §6 nor be emitted.
    if(is_infeasible(guard)) {
        return 0;
    }

    const auto record = [&](std::size_t next) {
        if(from == k_entry) {
            entry.push_back(Transition{guard, action, next});
        } else {
            states[from].out.push_back(Transition{guard, action, next});
        }
    };

    while(!frames.empty()) {
        Frame &top = frames.back();
        if(top.it == top.stmts->end()) {
            // The end of a loop's body is its back-edge (§7.3): fork at the
            // loop head again instead of popping through.
            if(top.loop) {
                const AST::Node *loop = top.loop;
                frames.pop_back();
                return loop_fork(loop, false, from, guard, action, frames, env, lapped, states,
                                 entry);
            }
            frames.pop_back();
            continue;
        }
        const AST::Node::Ptr stmt = *top.it;
        ++top.it;

        const std::string &fn = stmt->get_filename();
        const int ln = stmt->get_line();

        switch(stmt->get_node_type()) {
        case AST::NodeType::Block:
        case AST::NodeType::Pragmalist:
            push_frame(frames, stmt);
            break;

        case AST::NodeType::SingleStatement:
            // Validated delay-free by the collector; the wrapper is
            // transparent.
            push_frame(frames, AST::cast_to<AST::SingleStatement>(stmt)->get_statement());
            break;

        case AST::NodeType::NonblockingSubstitution:
            action->push_back(env.empty() ? stmt : subst_into(stmt->clone(), env));
            break;

        case AST::NodeType::IfStatement: {
            // Cut-point-free: a plain conditional inside the action, no
            // state spent on it (§4).
            if(!m_forking.count(stmt.get())) {
                action->push_back(env.empty() ? stmt : subst_into(stmt->clone(), env));
                break;
            }
            const auto &ifs = AST::cast_to<AST::IfStatement>(stmt);
            {
                std::vector<Frame> leg = frames;
                push_frame(leg, ifs->get_true_statement());
                if(walk_paths(from, conjoin(guard, clone_subst(ifs->get_cond(), env), fn, ln),
                              copy_list(action), leg, env, lapped, states, entry)) {
                    return 1;
                }
            }
            {
                // A missing else is the fall-through path: the machine takes
                // it in zero extra statements, not zero probability.
                std::vector<Frame> leg = frames;
                push_frame(leg, ifs->get_false_statement());
                if(walk_paths(from,
                              conjoin(guard, make_ulnot(clone_subst(ifs->get_cond(), env), fn, ln),
                                      fn, ln),
                              copy_list(action), leg, env, lapped, states, entry)) {
                    return 1;
                }
            }
            return 0;
        }

        case AST::NodeType::CaseStatement: {
            if(!m_forking.count(stmt.get())) {
                action->push_back(env.empty() ? stmt : subst_into(stmt->clone(), env));
                break;
            }
            const auto &cs = AST::cast_to<AST::CaseStatement>(stmt);
            // One leg per arm, guarded by the disjunction of its matches;
            // the default arm — or the fall-through when there is none —
            // takes the conjunction of every match negated.
            std::vector<std::pair<AST::Node::Ptr, AST::Node::Ptr>> legs;
            AST::Node::Ptr not_any;
            bool has_default = false;
            const auto &caselist = cs->get_caselist();
            if(caselist) {
                for(const auto &arm : *caselist) {
                    const auto &conds = arm->get_cond();
                    if(!conds || conds->empty()) {
                        has_default = true;
                        legs.emplace_back(nullptr, arm->get_statement());
                        continue;
                    }
                    AST::Node::Ptr match;
                    for(const auto &value : *conds) {
                        const auto &eq =
                            make_eq(clone_subst(cs->get_comp(), env), value->clone(), fn, ln);
                        match = match ? make_lor(match, eq, fn, ln) : eq;
                    }
                    legs.emplace_back(match, arm->get_statement());
                    not_any = conjoin(not_any, make_ulnot(match->clone(), fn, ln), fn, ln);
                }
            }
            for(auto &leg : legs) {
                if(!leg.first) {
                    leg.first = not_any;
                }
            }
            for(const auto &leg : legs) {
                std::vector<Frame> lf = frames;
                push_frame(lf, leg.second);
                if(walk_paths(from, conjoin(guard, leg.first, fn, ln), copy_list(action), lf, env,
                              lapped, states, entry)) {
                    return 1;
                }
            }
            if(!has_default) {
                std::vector<Frame> lf = frames;
                if(walk_paths(from, conjoin(guard, not_any, fn, ln), copy_list(action), lf, env,
                              lapped, states, entry)) {
                    return 1;
                }
            }
            return 0;
        }

        case AST::NodeType::WhileStatement:
        case AST::NodeType::RepeatStatement:
        case AST::NodeType::ForStatement:
        case AST::NodeType::ForeverStatement:
            // A loop the CFG keeps, registered by the collector: fork at
            // its head (§2, §7.2, §7.3).
            return loop_fork(stmt.get(), true, from, guard, action, frames, env, lapped, states,
                             entry);

        case AST::NodeType::Break:
        case AST::NodeType::Continue: {
            // §8: a jump is just an edge. Unwind to the innermost loop the
            // CFG sees: continue takes its back-edge — the step and the
            // test — break continues past it, skipping both.
            std::size_t depth = frames.size();
            while(depth > 0 && !frames[depth - 1].loop) {
                --depth;
            }
            if(depth == 0) {
                LOG_ERROR_N(stmt) << "break/continue outside a loop the CFG sees: nothing "
                                  << "to jump within (ADR-0014 §8)";
                return 1;
            }
            const AST::Node *loop = frames[depth - 1].loop;
            std::vector<Frame> unwound(frames.begin(), frames.begin() + (depth - 1));
            if(stmt->is_node_type(AST::NodeType::Continue)) {
                return loop_fork(loop, false, from, guard, action, unwound, env, lapped, states,
                                 entry);
            }
            return walk_paths(from, guard, action, unwound, env, lapped, states, entry);
        }

        case AST::NodeType::EventStatement: {
            const auto &event = AST::cast_to<AST::EventStatement>(stmt);
            const std::size_t idx = m_wait_index.at(event.get());
            record(idx);
            // Every path reaches a given wait with the same continuation —
            // its syntactic position fixes it — so its outgoing paths are
            // walked once, on first arrival. The environment and the lap
            // set die with the segment: the next one reads committed
            // registers.
            if(states[idx].walked) {
                return 0;
            }
            states[idx].walked = true;
            std::vector<Frame> cont = frames;
            push_frame(cont, event->get_statement());
            return walk_paths(idx, nullptr, std::make_shared<AST::Node::List>(), cont, Env{}, {},
                              states, entry);
        }

        default:
            LOG_ERROR_N(stmt) << "construct not handled by the FSM lowering: "
                              << stmt->get_node_type();
            return 1;
        }
    }

    // The process ends: a one-shot parks in the hold state (§2).
    record(states.size());
    return 0;
}

int ImplicitFsmElaboration::loop_fork(const AST::Node *loop, bool entering, std::size_t from,
                                      const AST::Node::Ptr &guard, const AST::Node::ListPtr &action,
                                      const std::vector<Frame> &frames, const Env &env,
                                      const std::set<const AST::Node *> &lapped,
                                      std::vector<State> &states, std::vector<Transition> &entry)
{
    const auto &info = m_loops.at(loop);
    const auto &anchor = info.cond ? info.cond : info.body;
    const std::string &fn = anchor->get_filename();
    const int ln = anchor->get_line();

    // Entering the body: a lap that reached this head again without
    // crossing a cut point re-enters in zero time (IEEE §9.2.2.1, §9).
    const auto enter_body = [&](const AST::Node::Ptr &leg_guard, AST::Node::ListPtr leg_action,
                                Env leg_env) -> int {
        if(lapped.count(loop)) {
            LOG_ERROR_N(anchor) << "a path through this loop's body reaches the loop head again "
                                << "without crossing a cut point: a zero-delay lap "
                                << "(IEEE 1800-2017 §9.2.2.1, ADR-0014 §9)";
            return 1;
        }
        auto leg_lapped = lapped;
        leg_lapped.insert(loop);
        auto leg_frames = frames;
        push_frame(leg_frames, info.body, loop);
        return walk_paths(from, leg_guard, leg_action, leg_frames, leg_env, leg_lapped, states,
                          entry);
    };
    const auto skip_past = [&](const AST::Node::Ptr &leg_guard, AST::Node::ListPtr leg_action,
                               Env leg_env) -> int {
        return walk_paths(from, leg_guard, leg_action, frames, leg_env, lapped, states, entry);
    };

    switch(info.kind) {
    case LoopInfo::Kind::WHILE: {
        // §7.3: the test re-evaluates at the head, entry and back-edge
        // alike, over the segment's values.
        if(enter_body(conjoin(guard, clone_subst(info.cond, env), fn, ln), copy_list(action),
                      env)) {
            return 1;
        }
        return skip_past(conjoin(guard, make_ulnot(clone_subst(info.cond, env), fn, ln), fn, ln),
                         copy_list(action), env);
    }

    case LoopInfo::Kind::FOREVER:
        // §2: the §7.3 back-edge with no exit test — the only way past it
        // is a break (§8).
        return enter_body(guard, entering ? action : copy_list(action), env);

    case LoopInfo::Kind::REPEAT: {
        const auto cnt_id = [&]() { return AST::to_node(make_id(m_cnt_name, fn, ln)); };
        const auto cnt_zero = [&]() {
            return AST::to_node(make_const(0, static_cast<int>(m_cnt_width), fn, ln));
        };
        if(entering) {
            if(info.count_known) {
                if(info.count_value == 0) {
                    return skip_past(guard, action, env);
                }
                if(info.count_value == 1) {
                    // §7.2: a single pass needs no countdown, but the body
                    // still owns its jumps (§8): it enters as a loop whose
                    // back-edge exits unconditionally.
                    return enter_body(guard, action, env);
                }
                auto leg_action = copy_list(action);
                leg_action->push_back(AST::to_node(make_nba(
                    m_cnt_name,
                    AST::to_node(make_const(static_cast<unsigned int>(info.count_value - 1),
                                            static_cast<int>(m_cnt_width), fn, ln)),
                    fn, ln)));
                return enter_body(guard, leg_action, env);
            }
            // §12.7.2: the count is evaluated once, on entry — captured
            // into the countdown — and a zero count skips the state
            // through the entry guard.
            {
                auto leg_action = copy_list(action);
                leg_action->push_back(AST::to_node(make_nba(
                    m_cnt_name,
                    make_minus(clone_subst(info.cond, env),
                               AST::to_node(make_const(1, static_cast<int>(m_cnt_width), fn, ln)),
                               fn, ln),
                    fn, ln)));
                if(enter_body(conjoin(guard,
                                      make_noteq(clone_subst(info.cond, env), cnt_zero(), fn, ln),
                                      fn, ln),
                              leg_action, env)) {
                    return 1;
                }
            }
            return skip_past(
                conjoin(guard, make_eq(clone_subst(info.cond, env), cnt_zero(), fn, ln), fn, ln),
                copy_list(action), env);
        }
        // Back-edge of the single pass: exit unconditionally — which is
        // also where a continue inside it lands (§12.7.2: no next lap).
        if(info.count_known && info.count_value == 1) {
            return skip_past(guard, copy_list(action), env);
        }
        // Back-edge: the countdown decides, and decrements on the lap.
        {
            auto leg_action = copy_list(action);
            leg_action->push_back(AST::to_node(make_nba(
                m_cnt_name,
                make_minus(cnt_id(),
                           AST::to_node(make_const(1, static_cast<int>(m_cnt_width), fn, ln)), fn,
                           ln),
                fn, ln)));
            if(enter_body(conjoin(guard, make_noteq(cnt_id(), cnt_zero(), fn, ln), fn, ln),
                          leg_action, env)) {
                return 1;
            }
        }
        return skip_past(conjoin(guard, make_eq(cnt_id(), cnt_zero(), fn, ln), fn, ln),
                         copy_list(action), env);
    }

    case LoopInfo::Kind::FOR: {
        // §7.2: the init or the step commits once, and its value is
        // substituted forward within its own segment (§6.1) — which is when
        // the source evaluates the test.
        const auto &value = clone_subst(entering ? info.init_rhs : info.step_rhs, env);
        Env leg_env = env;
        leg_env[info.index] = value;
        auto base = copy_list(action);
        // A prior induced commit to the same index in this segment — the
        // previous loop's step before this one's init — coalesces away,
        // blocking-style: the environment already carries the value its
        // reads needed. An author's commit stays, and §6 flags it.
        for(auto it = base->begin(); it != base->end();) {
            const bool induced =
                m_induced.count(it->get()) &&
                (*it)->is_node_type(AST::NodeType::NonblockingSubstitution) &&
                nba_target(AST::cast_to<AST::NonblockingSubstitution>(*it)) == info.index;
            it = induced ? base->erase(it) : std::next(it);
        }
        const auto &commit = AST::to_node(make_nba(info.index, value->clone(), fn, ln));
        m_induced.insert(commit.get());
        base->push_back(commit);
        if(enter_body(conjoin(guard, clone_subst(info.cond, leg_env), fn, ln), copy_list(base),
                      leg_env)) {
            return 1;
        }
        return skip_past(
            conjoin(guard, make_ulnot(clone_subst(info.cond, leg_env), fn, ln), fn, ln), base,
            leg_env);
    }
    }
    return 1;
}

int ImplicitFsmElaboration::check_wait(const AST::EventStatement::Ptr &event, AST::Sens::Ptr &clock)
{
    const auto &senslist = event->get_senslist();
    if(!senslist || !senslist->get_list() || senslist->get_list()->size() != 1) {
        LOG_ERROR_N(event) << "a marked process waits on exactly one event "
                           << "term: one clock, one edge (ADR-0014 §2)";
        return 1;
    }

    const auto &sens = senslist->get_list()->front();
    if(sens->get_type() != AST::Sens::TypeEnum::POSEDGE &&
       sens->get_type() != AST::Sens::TypeEnum::NEGEDGE) {
        LOG_ERROR_N(event) << "a marked process waits on a clock edge — "
                           << "posedge or negedge, never a level (ADR-0014 §2)";
        return 1;
    }

    if(!sens->get_sig() || !sens->get_sig()->is_node_type(AST::NodeType::Identifier)) {
        LOG_ERROR_N(event) << "a marked process waits on a plain clock signal";
        return 1;
    }

    if(!clock) {
        clock = sens;
        return 0;
    }

    const auto &clock_name = AST::cast_to<AST::Identifier>(clock->get_sig())->get_name();
    const auto &sens_name = AST::cast_to<AST::Identifier>(sens->get_sig())->get_name();
    if(clock_name != sens_name || clock->get_type() != sens->get_type()) {
        LOG_ERROR_N(event) << "cut point over '" << sens_name
                           << "' disagrees with the process clock '" << clock_name
                           << "': one clock, one edge, per process (ADR-0014 §2, §9)";
        return 1;
    }

    return 0;
}

int ImplicitFsmElaboration::check_enable(const std::vector<AST::EventStatement::Ptr> &waits,
                                         AST::Node::Ptr &enable)
{
    std::vector<AST::EventStatement::Ptr> bare;
    std::vector<AST::EventStatement::Ptr> qualified;
    for(const auto &wait : waits) {
        const auto &sens = wait->get_senslist()->get_list()->front();
        if(sens->get_condition()) {
            qualified.push_back(wait);
        } else {
            bare.push_back(wait);
        }
    }

    if(qualified.empty()) {
        enable = nullptr;
        return 0;
    }

    // Some waits qualified and some bare is almost always an oversight, and
    // it is not repairable by guessing: adding or dropping an enable changes
    // the machine. The message names the odd waits out and the ones they
    // disagree with (§5.3). Lines are deduplicated: waits replicated by
    // unrolling share one source line and are one wait to the author.
    if(!bare.empty()) {
        const bool bare_odd = bare.size() <= qualified.size();
        const auto &odd = bare_odd ? bare : qualified;
        const auto &rest = bare_odd ? qualified : bare;
        auto lines_of = [](const std::vector<AST::EventStatement::Ptr> &group) {
            std::set<int> lines;
            for(const auto &wait : group) {
                lines.insert(wait->get_line());
            }
            std::vector<std::string> strs;
            for(int line : lines) {
                strs.push_back(std::to_string(line));
            }
            return std::make_pair(lines.size(), Misc::StringUtils::join(", ", strs));
        };
        const auto &odd_lines = lines_of(odd);
        const auto &rest_lines = lines_of(rest);
        auto describe = [](const std::pair<std::size_t, std::string> &lines) {
            return (lines.first > 1 ? "the waits at lines " : "the wait at line ") + lines.second;
        };
        LOG_ERROR_N(odd.front())
            << describe(odd_lines) << (odd_lines.first > 1 ? " carry" : " carries")
            << (bare_odd ? " no `iff`" : " an `iff`") << " while " << describe(rest_lines)
            << (rest_lines.first > 1 ? " carry" : " carries") << (bare_odd ? " one" : " none")
            << ": a chip enable qualifies every transition or none (ADR-0014 §5.3)";
        return 1;
    }

    const auto &reference = qualified.front()->get_senslist()->get_list()->front()->get_condition();
    for(std::size_t i = 1; i < qualified.size(); ++i) {
        const auto &wait = qualified[i];
        const auto &condition = wait->get_senslist()->get_list()->front()->get_condition();
        if(!reference->is_equal(condition, false)) {
            LOG_ERROR_N(wait)
                << "the wait at line " << wait->get_line() << " carries a different `iff` "
                << "condition than the wait at line " << qualified.front()->get_line()
                << ": a uniform chip enable is one condition for the whole machine — "
                << "gating states differently is a separate feature (ADR-0014 §5.3, §15)";
            return 1;
        }
    }

    enable = reference;
    return 0;
}

int ImplicitFsmElaboration::find_reset(const AST::Module::Ptr &module,
                                       const AST::Pragmalist::Ptr &pragmalist,
                                       std::string &reset_name, bool &active_low)
{
    const auto &hint = get_pragma(pragmalist, "veriparse_reset");
    if(hint) {
        const auto &expr = hint->get_expression();
        if(expr && expr->is_node_type(AST::NodeType::StringConst)) {
            reset_name = AST::cast_to<AST::StringConst>(expr)->get_value();
        } else if(expr && expr->is_node_type(AST::NodeType::Identifier)) {
            reset_name = AST::cast_to<AST::Identifier>(expr)->get_name();
        } else {
            LOG_ERROR_N(pragmalist) << "veriparse_reset names the reset input, "
                                    << "as a string or an identifier (ADR-0014 §5)";
            return 1;
        }
    } else {
        static const std::set<std::string> candidates = {"rst_n", "resetn", "aresetn", "rst",
                                                         "reset"};
        std::vector<std::string> matches;
        const auto &inputs = Analysis::Module::get_input_names(AST::to_node(module));
        for(const auto &name : inputs) {
            if(candidates.count(to_lower(name))) {
                matches.push_back(name);
            }
        }
        if(matches.size() != 1) {
            LOG_ERROR_N(module) << "reset signal neither hinted nor uniquely "
                                << "inferable: " << matches.size()
                                << " candidate input(s) — name it with "
                                << "(* veriparse_reset = \"<port>\" *) (ADR-0014 §5)";
            return 1;
        }
        reset_name = matches.front();
    }

    const auto &lowered = to_lower(reset_name);
    active_low = lowered.size() >= 2 && lowered.compare(lowered.size() - 2, 2, "_n") == 0;
    return 0;
}

int ImplicitFsmElaboration::check_paths(const AST::Node::ListPtr &init_stmts,
                                        const std::vector<State> &states, std::size_t entry_next,
                                        const AST::Node::Ptr &enable)
{
    // Registers of the process: every nonblocking target anywhere in it,
    // the branches an action keeps verbatim included.
    std::set<std::string> process_regs;
    if(collect_targets_list(init_stmts, process_regs)) {
        return 1;
    }
    for(const auto &state : states) {
        for(const auto &transition : state.out) {
            if(collect_targets_list(transition.action, process_regs)) {
                return 1;
            }
        }
    }

    // At most one commit per register on any runtime path through an action
    // (§6) — a branch counts its worst arm, a sequence adds. The error is
    // anchored at the last '<=' to the offending register, the one whose
    // predecessor never takes effect.
    auto commits_ok = [](const AST::Node::ListPtr &stmts) -> int {
        std::map<std::string, int> counts;
        max_commits_list(stmts, counts);
        for(const auto &elt : counts) {
            if(elt.second > 1) {
                AST::Node::Ptr anchor = stmts->front();
                std::function<void(const AST::Node::Ptr &)> find = [&](const AST::Node::Ptr &node) {
                    if(!node) {
                        return;
                    }
                    if(node->is_node_type(AST::NodeType::NonblockingSubstitution)) {
                        if(nba_target(AST::cast_to<AST::NonblockingSubstitution>(node)) ==
                           elt.first) {
                            anchor = node;
                        }
                        return;
                    }
                    for(const auto &child : *node->get_children()) {
                        find(child);
                    }
                };
                for(const auto &stmt : *stmts) {
                    find(stmt);
                }
                LOG_ERROR_N(anchor)
                    << "register '" << elt.first << "' committed twice on one path: "
                    << "the first '<=' never takes effect (ADR-0014 §6)";
                return 1;
            }
        }
        return 0;
    };
    if(!init_stmts->empty() && commits_ok(init_stmts)) {
        return 1;
    }

    // §5.1: the reset branch loads reset values once. Emitted under
    // `if (!rst)` a preamble branch would be re-evaluated on every reset
    // cycle — the source initial evaluates it exactly once — and an arm
    // that skips a register leaves it with no reset value.
    for(const auto &stmt : *init_stmts) {
        if(stmt->is_node_type(AST::NodeType::IfStatement) ||
           stmt->is_node_type(AST::NodeType::CaseStatement)) {
            LOG_ERROR_N(stmt) << "a branch in the preamble: the reset branch loads reset "
                              << "values once, while emitted into the reset arm it would be "
                              << "re-evaluated on every reset cycle (ADR-0014 §5.1)";
            return 1;
        }
    }

    // A preamble read of a process register is a read of the empty entry
    // store: nothing is assigned at reset entry — its own '<=' commits only
    // at the clock edge (§5.1, §6).
    for(const auto &stmt : *init_stmts) {
        std::set<std::string> reads;
        collect_reads(stmt, reads);
        for(const auto &read : reads) {
            if(process_regs.count(read)) {
                LOG_ERROR_N(stmt) << "the preamble reads register '" << read
                                  << "': nothing is assigned at reset entry, so the reset "
                                  << "value would be undefined (ADR-0014 §5.1, §6)";
                return 1;
            }
        }
    }

    // Must-defined registers at each state's entry: what the init segment
    // wrote, then the intersection over every incoming path — a register a
    // branch arm skips is not defined past the merge. Transitions only go
    // forward in source order (no loops yet), so one pass in state order
    // sees every predecessor first.
    std::set<std::string> init_defined;
    must_writes_list(init_stmts, init_defined);

    // The enable is read at every state's entry, the first out of reset
    // included, so a process register it reads must come up with a value
    // the init segment supplies (§5.1, §5.3, §6).
    if(enable) {
        std::set<std::string> reads;
        collect_identifier_names(enable, reads);
        for(const auto &read : reads) {
            if(process_regs.count(read) && !init_defined.count(read)) {
                LOG_ERROR_N(enable)
                    << "the `iff` enable reads register '" << read << "' which no path "
                    << "out of reset assigns: the enable gates every state including "
                    << "the first, and the init segment gives it no value "
                    << "(ADR-0014 §5.1, §5.3, §6)";
                return 1;
            }
        }
    }

    // Back-edges (§7.3) make the state graph cyclic, so the must-defined
    // sets iterate to a fixpoint: they only ever shrink once seeded, so
    // the iteration terminates. A transition's write set never changes:
    // computed once, the fixpoint is pure set arithmetic.
    std::vector<std::vector<std::set<std::string>>> writes_of(states.size());
    for(std::size_t s = 0; s < states.size(); ++s) {
        for(const auto &transition : states[s].out) {
            writes_of[s].emplace_back();
            must_writes_list(transition.action, writes_of[s].back());
        }
    }
    std::vector<std::set<std::string>> defined_in(states.size());
    std::vector<bool> reached(states.size(), false);
    if(entry_next < states.size()) {
        defined_in[entry_next] = init_defined;
        reached[entry_next] = true;
    }
    bool changed = true;
    while(changed) {
        changed = false;
        for(std::size_t s = 0; s < states.size(); ++s) {
            if(!reached[s]) {
                continue;
            }
            for(std::size_t t = 0; t < states[s].out.size(); ++t) {
                const auto &transition = states[s].out[t];
                if(transition.next >= states.size()) {
                    continue;
                }
                std::set<std::string> defined_out = defined_in[s];
                defined_out.insert(writes_of[s][t].begin(), writes_of[s][t].end());
                if(!reached[transition.next]) {
                    defined_in[transition.next] = defined_out;
                    reached[transition.next] = true;
                    changed = true;
                    continue;
                }
                std::set<std::string> kept;
                for(const auto &name : defined_in[transition.next]) {
                    if(defined_out.count(name)) {
                        kept.insert(name);
                    }
                }
                if(kept.size() != defined_in[transition.next].size()) {
                    defined_in[transition.next] = kept;
                    changed = true;
                }
            }
        }
    }

    for(std::size_t s = 0; s < states.size(); ++s) {
        if(!reached[s]) {
            continue;
        }
        for(const auto &transition : states[s].out) {
            if(!transition.action->empty() && commits_ok(transition.action)) {
                return 1;
            }

            // Reads resolve against the entry store (§6.1): the action's own
            // writes commit at the next edge and define nothing here. The
            // guard is a read too — the fork asks the question at entry.
            // Each error is anchored at the statement or guard that reads.
            auto check_reads = [&](const std::set<std::string> &reads,
                                   const AST::Node::Ptr &anchor) -> int {
                for(const auto &read : reads) {
                    if(process_regs.count(read) && !defined_in[s].count(read)) {
                        LOG_ERROR_N(anchor)
                            << "register '" << read << "' is read before every path "
                            << "out of reset assigns it, and the init segment gives "
                            << "it no value (ADR-0014 §5.1, §6)";
                        return 1;
                    }
                }
                return 0;
            };
            if(transition.guard) {
                std::set<std::string> reads;
                collect_identifier_names(transition.guard, reads);
                if(check_reads(reads, transition.guard)) {
                    return 1;
                }
            }
            for(const auto &stmt : *transition.action) {
                std::set<std::string> reads;
                collect_reads(stmt, reads);
                if(check_reads(reads, stmt)) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

int ImplicitFsmElaboration::compile_process(const AST::Module::Ptr &module,
                                            const AST::Node::Ptr &parent,
                                            const AST::Pragmalist::Ptr &pragmalist,
                                            const AST::Initial::Ptr &initial,
                                            const std::string &prefix)
{
    // One state per wait, in source order (§4, §10.1); each wait is checked
    // for shape and clock uniformity as it is collected.
    m_forking.clear();
    m_wait_index.clear();
    m_loops.clear();
    m_induced.clear();
    m_cnt_name = prefix + "_cnt";
    m_cnt_width = 0;
    m_repeat_depth = 0;
    AST::Sens::Ptr clock;
    std::vector<AST::EventStatement::Ptr> waits;
    bool has_wait = false;
    if(collect_body(initial->get_statement(), waits, clock, has_wait)) {
        return 1;
    }

    if(!clock) {
        LOG_ERROR_N(initial) << "(* veriparse_fsm *) on an initial with no wait: "
                             << "there is nothing to compile (ADR-0014 §2, §9)";
        return 1;
    }

    // Size the shared countdown (§15: one per process) over every rolled
    // repeat: $clog2(N) for a folded count, the count signal's declared
    // width otherwise — the capture `cnt <= expr - 1` must hold any value
    // the signal can carry. Rolled for indices are the author's registers:
    // they must exist at module level, with their declared type (§7.2).
    for(const auto &elt : m_loops) {
        const auto &info = elt.second;
        if(info.kind == LoopInfo::Kind::FOR) {
            bool is_input = false;
            const auto &decl = find_declaration(module, info.index, &is_input);
            if(!decl) {
                LOG_ERROR_N(info.cond)
                    << "rolled for index '" << info.index << "' is not a module-level "
                    << "declaration: the induced register takes the index's declared "
                    << "type (ADR-0014 §7.2)";
                return 1;
            }
            // The machine drives the index from its always_ff: an input
            // port or a net cannot take the commits.
            if(is_input || !std::dynamic_pointer_cast<AST::Var>(decl)) {
                LOG_ERROR_N(info.cond)
                    << "rolled for index '" << info.index << "' is not a variable the "
                    << "machine can drive: an input port or a net cannot take the "
                    << "induced register's commits (ADR-0014 §7.2, §9)";
                return 1;
            }
            continue;
        }
        if(info.kind != LoopInfo::Kind::REPEAT) {
            continue;
        }
        if(info.count_known) {
            if(info.count_value >= 2) {
                m_cnt_width =
                    std::max(m_cnt_width, clog2(static_cast<unsigned int>(info.count_value)));
            }
            continue;
        }
        if(!info.cond->is_node_type(AST::NodeType::Identifier)) {
            LOG_ERROR_N(info.cond) << "non-constant repeat count must be a plain signal, so the "
                                   << "countdown can take its declared width — bind the expression "
                                   << "to a named signal first (ADR-0014 §7.2)";
            return 1;
        }
        const auto &count_name = AST::cast_to<AST::Identifier>(info.cond)->get_name();
        const auto &decl = find_declaration(module, count_name);
        unsigned int width = 0;
        if(!decl || declared_width(decl, width)) {
            LOG_ERROR_N(info.cond)
                << "cannot size the countdown for repeat count '" << count_name
                << "': its declaration or packed range is not resolvable (ADR-0014 §7.2)";
            return 1;
        }
        m_cnt_width = std::max(m_cnt_width, width);
    }

    AST::Node::Ptr enable;
    if(check_enable(waits, enable)) {
        return 1;
    }

    std::string reset_name;
    bool active_low = false;
    if(find_reset(module, pragmalist, reset_name, active_low)) {
        return 1;
    }

    // Enumerate the path cover (§C.4): each state's outgoing transitions,
    // plus the entry paths from the top of the process to its first wait.
    std::vector<State> states;
    for(const auto &wait : waits) {
        states.push_back(State{wait, {}, false});
    }
    std::vector<Transition> entry;
    std::vector<Frame> frames;
    push_frame(frames, initial->get_statement());
    if(walk_paths(k_entry, nullptr, std::make_shared<AST::Node::List>(), frames, Env{}, {}, states,
                  entry)) {
        return 1;
    }

    // The statements before the first wait are the reset branch (§5.1): one
    // unconditional path. A branch there whose arms hold cut points would
    // make the state out of reset input-dependent.
    if(entry.size() != 1 || entry.front().guard) {
        LOG_ERROR_N(initial) << "a cut point inside a branch before the first wait: "
                             << "the reset branch cannot fork (ADR-0014 §5.1)";
        return 1;
    }
    const auto &init_stmts = entry.front().action;
    const std::size_t entry_next = entry.front().next;

    if(check_paths(init_stmts, states, entry_next, enable)) {
        return 1;
    }

    const auto &emitted =
        emit(module, clock, enable, reset_name, active_low, init_stmts, states, entry_next, prefix);
    if(!emitted) {
        return 1;
    }

    AST::Node::Ptr pragmalist_node = AST::to_node(pragmalist);
    AST::Node::Ptr parent_node = parent;
    pickup_statements(parent_node, pragmalist_node, emitted);
    return 0;
}

AST::Node::ListPtr ImplicitFsmElaboration::emit(
    const AST::Module::Ptr &module, const AST::Sens::Ptr &clock, const AST::Node::Ptr &enable,
    const std::string &reset_name, bool active_low, const AST::Node::ListPtr &init_stmts,
    const std::vector<State> &states, std::size_t entry_next, const std::string &prefix)
{
    const std::string &fn = module->get_filename();
    const int ln = module->get_line();

    // One state per wait, plus the hold state a one-shot parks in (§2) —
    // omitted when no path ends the process, as none does in a perpetual
    // machine: an unreachable state would still cost encoding width.
    bool hold_needed = entry_next >= states.size();
    for(const auto &state : states) {
        for(const auto &transition : state.out) {
            hold_needed |= transition.next >= states.size();
        }
    }
    const std::size_t nstates = states.size() + (hold_needed ? 1 : 0);
    const unsigned int width = clog2(static_cast<unsigned int>(nstates));

    std::vector<std::string> state_names;
    for(std::size_t i = 0; i < states.size(); ++i) {
        state_names.push_back(prefix + "_state_" + std::to_string(i));
    }
    if(hold_needed) {
        state_names.push_back(prefix + "_hold");
    }
    const std::string state_reg = prefix + "_state";

    // §10: a collision remaining after prefixing is an error, not a rename.
    Analysis::UniqueDeclaration::IdentifierSet declared;
    Analysis::UniqueDeclaration::analyze(AST::to_node(module), declared);
    for(const auto &name : state_names) {
        if(Analysis::UniqueDeclaration::identifier_declaration_exists(name, declared)) {
            LOG_ERROR_N(module) << "generated declaration '" << name
                                << "' collides with an existing one (ADR-0014 §10)";
            return nullptr;
        }
    }
    if(Analysis::UniqueDeclaration::identifier_declaration_exists(state_reg, declared)) {
        LOG_ERROR_N(module) << "generated declaration '" << state_reg
                            << "' collides with an existing one (ADR-0014 §10)";
        return nullptr;
    }

    const auto &result = std::make_shared<AST::Node::List>();

    // localparam [w-1:0] <prefix>_state_<i> = <w>'d<i>;
    for(std::size_t i = 0; i < nstates; ++i) {
        auto type = std::make_shared<AST::ImplicitType>(fn, ln);
        type->set_packed_dims(make_packed_range(width - 1, fn, ln));
        auto param = std::make_shared<AST::Param>(fn, ln);
        param->set_name(state_names[i]);
        param->set_is_local(true);
        param->set_type(type);
        param->set_value(AST::to_node(
            make_const(static_cast<unsigned int>(i), static_cast<int>(width), fn, ln)));
        result->push_back(AST::to_node(param));
    }

    // logic [w-1:0] <prefix>_state;
    auto reg_type = std::make_shared<AST::LogicType>(fn, ln);
    reg_type->set_packed_dims(make_packed_range(width - 1, fn, ln));
    auto reg = std::make_shared<AST::Var>(fn, ln);
    reg->set_name(state_reg);
    reg->set_type(reg_type);
    result->push_back(AST::to_node(reg));

    // logic [w-1:0] <prefix>_cnt; — the shared countdown, when a rolled
    // repeat induced one (§7.2, §15).
    if(m_cnt_width > 0) {
        if(Analysis::UniqueDeclaration::identifier_declaration_exists(m_cnt_name, declared)) {
            LOG_ERROR_N(module) << "generated declaration '" << m_cnt_name
                                << "' collides with an existing one (ADR-0014 §10)";
            return nullptr;
        }
        auto cnt_type = std::make_shared<AST::LogicType>(fn, ln);
        if(m_cnt_width > 1) {
            cnt_type->set_packed_dims(make_packed_range(m_cnt_width - 1, fn, ln));
        }
        auto cnt = std::make_shared<AST::Var>(fn, ln);
        cnt->set_name(m_cnt_name);
        cnt->set_type(cnt_type);
        result->push_back(AST::to_node(cnt));
    }

    // Reset branch: the init segment verbatim, plus the state register
    // going to the first state (§5.1).
    const auto &reset_stmts = std::make_shared<AST::Node::List>();
    for(const auto &stmt : *init_stmts) {
        reset_stmts->push_back(stmt->clone());
    }
    reset_stmts->push_back(
        AST::to_node(make_state_assign(state_reg, state_names[entry_next], fn, ln)));
    auto reset_block = std::make_shared<AST::Block>(reset_stmts, "", fn, ln);

    // case (<state>) arms: one leg per outgoing transition — action plus
    // next state — chained if / else if / else, in enumeration order, the
    // last leg as the bare else since the guards partition by construction
    // (§C.3). The hold state carries no update and stays put.
    const auto &caselist = std::make_shared<AST::Case::List>();
    for(std::size_t i = 0; i < states.size(); ++i) {
        const auto &out = states[i].out;
        auto make_leg = [&](const Transition &transition) {
            const auto &leg_stmts = std::make_shared<AST::Node::List>();
            for(const auto &stmt : *transition.action) {
                leg_stmts->push_back(stmt->clone());
            }
            leg_stmts->push_back(
                AST::to_node(make_state_assign(state_reg, state_names[transition.next], fn, ln)));
            return AST::to_node(std::make_shared<AST::Block>(leg_stmts, "", fn, ln));
        };

        // A state no path reaches — a constant-zero repeat's body — keeps
        // an empty arm, like the hold state.
        if(out.empty()) {
            const auto &conds = std::make_shared<AST::Node::List>();
            conds->push_back(AST::to_node(make_id(state_names[i], fn, ln)));
            auto arm = std::make_shared<AST::Case>(fn, ln);
            arm->set_cond(conds);
            arm->set_statement(AST::to_node(
                std::make_shared<AST::Block>(std::make_shared<AST::Node::List>(), "", fn, ln)));
            caselist->push_back(arm);
            continue;
        }

        AST::Node::Ptr body = make_leg(out.back());
        for(std::size_t j = out.size() - 1; j-- > 0;) {
            // Guards partition by construction: every non-final transition
            // of a multi-way state carries one. Refuse to emit otherwise.
            if(!out[j].guard) {
                LOG_ERROR_N(states[i].wait)
                    << "unguarded transition in a multi-way state: the path cover "
                    << "lost a fork condition — please report this input";
                return nullptr;
            }
            auto chain = std::make_shared<AST::IfStatement>(fn, ln);
            chain->set_cond(out[j].guard->clone());
            chain->set_true_statement(make_leg(out[j]));
            chain->set_false_statement(body);
            body = AST::to_node(chain);
        }

        const auto &conds = std::make_shared<AST::Node::List>();
        conds->push_back(AST::to_node(make_id(state_names[i], fn, ln)));
        auto arm = std::make_shared<AST::Case>(fn, ln);
        arm->set_cond(conds);
        arm->set_statement(body);
        caselist->push_back(arm);
    }
    if(hold_needed) {
        const auto &conds = std::make_shared<AST::Node::List>();
        conds->push_back(AST::to_node(make_id(state_names.back(), fn, ln)));
        auto arm = std::make_shared<AST::Case>(fn, ln);
        arm->set_cond(conds);
        arm->set_statement(AST::to_node(
            std::make_shared<AST::Block>(std::make_shared<AST::Node::List>(), "", fn, ln)));
        caselist->push_back(arm);
    }

    auto case_stmt = std::make_shared<AST::CaseStatement>(fn, ln);
    case_stmt->set_comp(AST::to_node(make_id(state_reg, fn, ln)));
    case_stmt->set_caselist(caselist);

    // if (!rst_n) <reset> else [if (en)] case (...) — the enable gates the
    // case and nothing else: the reset is never gated, so the machine leaves
    // reset whether or not anything is running it (§5.3).
    AST::Node::Ptr else_branch = AST::to_node(case_stmt);
    if(enable) {
        auto gate = std::make_shared<AST::IfStatement>(fn, ln);
        gate->set_cond(enable->clone());
        gate->set_true_statement(else_branch);
        else_branch = AST::to_node(gate);
    }

    AST::Node::Ptr reset_cond = AST::to_node(make_id(reset_name, fn, ln));
    if(active_low) {
        auto ulnot = std::make_shared<AST::Ulnot>(fn, ln);
        ulnot->set_right(reset_cond);
        reset_cond = AST::to_node(ulnot);
    }
    auto guard = std::make_shared<AST::IfStatement>(fn, ln);
    guard->set_cond(reset_cond);
    guard->set_true_statement(AST::to_node(reset_block));
    guard->set_false_statement(else_branch);

    auto clock_sens = std::make_shared<AST::Sens>(fn, ln);
    clock_sens->set_type(clock->get_type());
    clock_sens->set_sig(clock->get_sig()->clone());
    const auto &sens_list = std::make_shared<AST::Sens::List>();
    sens_list->push_back(clock_sens);

    auto always = std::make_shared<AST::AlwaysFF>(fn, ln);
    always->set_senslist(std::make_shared<AST::Senslist>(sens_list, fn, ln));
    always->set_statement(AST::to_node(guard));
    result->push_back(AST::to_node(always));

    return result;
}

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse
