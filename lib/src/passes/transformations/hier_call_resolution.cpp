// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/passes/transformations/hier_call_resolution.hpp>
#include <veriparse/passes/transformations/interface_elaboration.hpp>
#include <veriparse/passes/transformations/name_resolution.hpp>
#include <veriparse/passes/transformations/scope_table.hpp>
#include <veriparse/passes/transformations/splice_utils.hpp>
#include <veriparse/logger/logger.hpp>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

namespace
{

/// The subroutine declared under `name` among `items`, descending into bare
/// generate regions — semantically transparent (IEEE 1800-2017 §27.3), so a
/// task they hold belongs to the interface itself. Named or conditional
/// generate blocks open their own scope and are deliberately not searched.
AST::Node::Ptr find_subroutine_in(const AST::Node::ListPtr &items, const std::string &name)
{
    if(!items) {
        return nullptr;
    }
    for(const AST::Node::Ptr &item : *items) {
        if(item->is_node_type(AST::NodeType::Task) &&
           AST::cast_to<AST::Task>(item)->get_name() == name) {
            return item;
        }
        if(item->is_node_type(AST::NodeType::Function) &&
           AST::cast_to<AST::Function>(item)->get_name() == name) {
            return item;
        }
        if(item->is_node_type(AST::NodeType::GenerateStatement)) {
            const auto &found =
                find_subroutine_in(AST::cast_to<AST::GenerateStatement>(item)->get_items(), name);
            if(found) {
                return found;
            }
        }
    }
    return nullptr;
}

AST::Node::Ptr find_subroutine(const AST::Interface::Ptr &iface, const std::string &name)
{
    return find_subroutine_in(iface->get_items(), name);
}

/// What kind of name a declaration binds, for the closure error messages.
/// ScopeTable::classify is deliberately not reused: its VALUE bucket blends
/// members with parameters, the very distinction these messages exist for.
std::string describe_kind(const AST::Node::Ptr &denoted)
{
    if(!denoted) {
        return "a declaration";
    }
    if(denoted->is_node_type(AST::NodeType::EnumItem)) {
        return "an enum item";
    }
    if(denoted->is_node_type(AST::NodeType::Param)) {
        return AST::cast_to<AST::Param>(denoted)->get_is_local() ? "a localparam" : "a parameter";
    }
    if(denoted->is_node_type(AST::NodeType::Typedef) ||
       denoted->is_node_type(AST::NodeType::TypeParam)) {
        return "a type name";
    }
    if(denoted->is_node_type(AST::NodeType::Task) ||
       denoted->is_node_type(AST::NodeType::Function)) {
        return "a subroutine";
    }
    return "a declaration";
}

/// Member and non-member names one interface binds, from the shared
/// enumeration (`InterfaceElaboration::for_each_binding`): members are the
/// §25.10 objects a subroutine body may reference and a splice rewrites;
/// every other bound name goes to `kinds` so a reference to it fails with
/// its kind named.
void collect_interface_names(const AST::Interface::Ptr &iface, std::set<std::string> &members,
                             std::map<std::string, std::string> &kinds)
{
    InterfaceElaboration::for_each_binding(
        iface, [&members, &kinds](const std::string &name, const AST::Node::Ptr &denoted) {
            if(!denoted) {
                return;
            }
            if(InterfaceElaboration::is_member_decl(denoted)) {
                members.insert(name);
            } else {
                kinds.emplace(name, describe_kind(denoted));
            }
        });
}

/// Names the immediate items of one scope declare. Non-recursive: a nested
/// block owns its own names, which is what makes shadowing per-scope.
std::set<std::string> scope_names(const AST::Node::ListPtr &items)
{
    std::set<std::string> names;
    if(items) {
        for(const AST::Node::Ptr &item : *items) {
            ScopeTable::for_each_bound_name(
                item, [&names](const std::string &name) { names.insert(name); });
        }
    }
    return names;
}

} // namespace

bool HierCallResolution::RewriteScope::is_bound(const std::string &name) const
{
    for(const auto &scope : scopes) {
        if(scope.count(name) != 0) {
            return true;
        }
    }
    return false;
}

HierCallResolution::HierCallResolution(const Analysis::Module::InterfacesMap &interfaces_map)
    : m_interfaces_map(interfaces_map)
{
}

int HierCallResolution::process(AST::Node::Ptr node, AST::Node::Ptr parent)
{
    (void)parent;
    if(!node) {
        return 0;
    }
    if(node->is_node_type(AST::NodeType::Module)) {
        return process_module(AST::cast_to<AST::Module>(node));
    }
    return recurse_in_childs(node);
}

int HierCallResolution::process_module(const AST::Module::Ptr &module)
{
    std::list<std::pair<AST::Call::Ptr, AST::Node::Ptr>> calls;
    collect_calls(module, nullptr, calls);
    if(calls.empty()) {
        return 0;
    }

    m_roots.clear();
    m_splices.clear();
    m_declared.clear();
    if(collect_roots(module)) {
        return 1;
    }
    if(Analysis::UniqueDeclaration::analyze(module, m_declared)) {
        return 1;
    }

    for(const auto &entry : calls) {
        if(resolve_call(module, entry.first, entry.second)) {
            return 1;
        }
    }
    return 0;
}

int HierCallResolution::collect_roots(const AST::Module::Ptr &module)
{
    const auto &ports = module->get_ports();
    if(ports) {
        for(const AST::Port::Ptr &port : *ports) {
            const auto &decl = port->get_decl();
            if(!decl || !decl->is_node_type(AST::NodeType::Arg)) {
                continue;
            }
            const auto &arg = AST::cast_to<AST::Arg>(decl);
            const auto &type = arg->get_type();
            if(type && type->is_node_type(AST::NodeType::InterfaceType) &&
               !AST::cast_to<AST::InterfaceType>(type)->get_is_virtual()) {
                m_roots.emplace(arg->get_name(),
                                AST::cast_to<AST::InterfaceType>(type)->get_name());
            }
        }
    }
    return 0;
}

void HierCallResolution::collect_calls(const AST::Node::Ptr &node, const AST::Node::Ptr &parent,
                                       std::list<std::pair<AST::Call::Ptr, AST::Node::Ptr>> &calls)
{
    if(!node) {
        return;
    }
    const auto &children = node->get_children();
    if(children) {
        for(const AST::Node::Ptr &child : *children) {
            collect_calls(child, node, calls);
        }
    }
    // Post-order: a call nested in another call's arguments resolves before
    // its enclosing call site is replaced, so the recorded parent is live
    // when it is used.
    if(node->is_node_category(AST::NodeType::Identifier) &&
       (node->is_node_type(AST::NodeType::Call) || node->is_node_type(AST::NodeType::TaskCall) ||
        node->is_node_type(AST::NodeType::FunctionCall))) {
        const auto &call = AST::cast_to<AST::Call>(node);
        if(call->get_hier()) {
            calls.emplace_back(call, parent);
        }
    }
}

int HierCallResolution::resolve_call(const AST::Module::Ptr &module, const AST::Call::Ptr &call,
                                     const AST::Node::Ptr &parent)
{
    const auto &hier = call->get_hier();
    const auto &labels = hier->get_labellist();
    if(!labels || labels->empty()) {
        LOG_ERROR_N(call) << "hierarchical call to '" << call->get_name() << "' has an empty path";
        return 1;
    }
    if(labels->size() != 1) {
        LOG_ERROR_N(call) << "hierarchical call to '" << call->get_name()
                          << "' crosses more than one level; the supported form is a "
                          << "subroutine of an interface port of this module";
        return 1;
    }

    const AST::HierLabel::Ptr &root_label = labels->front();
    if(root_label->get_loop()) {
        LOG_ERROR_N(call) << "hierarchical call root '" << root_label->get_name()
                          << "' is indexed: it names a different target per evaluation, "
                          << "which subroutine resolution cannot tell apart";
        return 1;
    }

    const std::string &root = root_label->get_name();
    const auto root_it = m_roots.find(root);
    if(root_it == m_roots.end()) {
        LOG_ERROR_N(call) << "cannot call '" << root << "." << call->get_name() << "': '" << root
                          << "' does not name a non-virtual interface port of this module "
                          << "(calling a subroutine of a module instance or of a local "
                          << "interface instance is not supported)";
        return 1;
    }

    const auto iface_it = m_interfaces_map.find(root_it->second);
    if(iface_it == m_interfaces_map.end()) {
        LOG_ERROR_N(call) << "cannot call '" << root << "." << call->get_name()
                          << "': the definition of interface '" << root_it->second
                          << "' is not available";
        return 1;
    }
    const AST::Interface::Ptr &iface = iface_it->second;

    const AST::Node::Ptr subroutine = find_subroutine(iface, call->get_name());
    if(!subroutine) {
        LOG_ERROR_N(call) << "interface '" << iface->get_name() << "' declares no task or "
                          << "function '" << call->get_name() << "'";
        return 1;
    }

    AST::Node::Ptr decl;
    if(splice(module, root, iface, subroutine, decl)) {
        return 1;
    }

    const std::string display = root + "." + call->get_name();

    if(decl->is_node_type(AST::NodeType::Task)) {
        if(call->is_node_type(AST::NodeType::FunctionCall)) {
            LOG_ERROR_N(call) << "task '" << display
                              << "' called in expression position: a task returns no value "
                              << "(IEEE 1800-2017 13.5)";
            return 1;
        }
        const auto &spliced_name = AST::cast_to<AST::Task>(decl)->get_name();
        if(!NameResolution::retag_statement_call(call, parent, decl, spliced_name, display)) {
            LOG_ERROR_N(call) << "failed to rewrite the call to '" << display << "'";
            return 1;
        }
        return 0;
    }

    const auto &function = AST::cast_to<AST::Function>(decl);
    if(call->is_node_type(AST::NodeType::FunctionCall)) {
        call->set_name(function->get_name());
        call->set_hier(nullptr);
        return 0;
    }

    if(!NameResolution::retag_statement_call(call, parent, decl, function->get_name(), display)) {
        LOG_ERROR_N(call) << "failed to rewrite the call to '" << display << "'";
        return 1;
    }
    return 0;
}

int HierCallResolution::splice(const AST::Module::Ptr &module, const std::string &root,
                               const AST::Interface::Ptr &iface, const AST::Node::Ptr &subroutine,
                               AST::Node::Ptr &decl)
{
    const bool is_task = subroutine->is_node_type(AST::NodeType::Task);
    const std::string sub_name = is_task ? AST::cast_to<AST::Task>(subroutine)->get_name()
                                         : AST::cast_to<AST::Function>(subroutine)->get_name();

    const std::string key = root + "." + sub_name;
    const auto memo = m_splices.find(key);
    if(memo != m_splices.end()) {
        decl = memo->second;
        return 0;
    }

    AST::Node::Ptr clone = subroutine->clone();
    SpliceUtils::stamp_subroutine_lifetime(clone, iface->get_lifetime() ==
                                                      AST::Interface::LifetimeEnum::AUTOMATIC);

    std::set<std::string> members;
    std::map<std::string, std::string> kinds;
    collect_interface_names(iface, members, kinds);

    RewriteScope ctx;
    ctx.root = root;
    ctx.iface_name = iface->get_name();
    ctx.sub_name = sub_name;
    ctx.members = &members;
    ctx.kinds = &kinds;

    // The subroutine's own scope: its name (a function's return variable),
    // its formals, and its body-level declarations. Block-scoped names join
    // and leave the stack as rewrite_body enters and leaves each block.
    std::set<std::string> own;
    own.insert(sub_name);
    const AST::Arg::ListPtr args = is_task ? AST::cast_to<AST::Task>(clone)->get_args()
                                           : AST::cast_to<AST::Function>(clone)->get_args();
    if(args) {
        for(const AST::Arg::Ptr &arg : *args) {
            own.insert(arg->get_name());
        }
    }
    const AST::Node::ListPtr statements =
        is_task ? AST::cast_to<AST::Task>(clone)->get_statements()
                : AST::cast_to<AST::Function>(clone)->get_statements();
    const auto body_names = scope_names(statements);
    own.insert(body_names.begin(), body_names.end());
    ctx.scopes.push_back(own);

    if(rewrite_body(clone, ctx)) {
        return 1;
    }

    std::string spliced_name = root + "_" + sub_name;
    if(Analysis::UniqueDeclaration::identifier_declaration_exists(spliced_name, m_declared)) {
        spliced_name = Analysis::UniqueDeclaration::get_unique_identifier(spliced_name, m_declared);
    } else {
        m_declared.insert(spliced_name);
    }

    if(is_task) {
        AST::cast_to<AST::Task>(clone)->set_name(spliced_name);
    } else {
        AST::cast_to<AST::Function>(clone)->set_name(spliced_name);
    }

    auto items = module->get_items();
    if(!items) {
        items = std::make_shared<AST::Node::List>();
        module->set_items(items);
    }
    items->push_front(clone);

    m_splices.emplace(key, clone);
    m_spliced = true;
    decl = clone;
    return 0;
}

int HierCallResolution::rewrite_body(const AST::Node::Ptr &node, RewriteScope &ctx)
{
    if(!node) {
        return 0;
    }

    if(node->is_node_category(AST::NodeType::Identifier)) {
        if(!node->is_node_type(AST::NodeType::Identifier)) {
            // Call, TaskCall or FunctionCall: nothing a nested call could
            // legally reach is carried by the splice (v1 closure).
            const auto &nested = AST::cast_to<AST::Call>(node);
            LOG_ERROR_N(node) << "'" << ctx.iface_name << "." << ctx.sub_name << "' calls '"
                              << nested->get_name() << "': a call inside an interface "
                              << "subroutine is not supported through a hierarchical call";
            return 1;
        }

        const auto &id = AST::cast_to<AST::Identifier>(node);
        if(id->get_hier()) {
            LOG_ERROR_N(node) << "'" << ctx.iface_name << "." << ctx.sub_name
                              << "' holds a hierarchical reference to '" << id->get_name()
                              << "', which a spliced subroutine cannot carry";
            return 1;
        }
        if(id->get_scope() && !id->get_scope()->empty()) {
            LOG_ERROR_N(node) << "'" << ctx.iface_name << "." << ctx.sub_name
                              << "' holds an unresolved scoped reference to '" << id->get_name()
                              << "'";
            return 1;
        }

        const std::string &name = id->get_name();
        if(ctx.is_bound(name)) {
            return 0;
        }
        if(ctx.members->count(name) != 0) {
            auto label = std::make_shared<AST::HierLabel>(id->get_filename(), id->get_line());
            label->set_name(ctx.root);
            auto labels = std::make_shared<AST::HierLabel::List>();
            labels->push_back(label);
            auto hier = std::make_shared<AST::HierName>(id->get_filename(), id->get_line());
            hier->set_labellist(labels);
            id->set_hier(hier);
            return 0;
        }
        const auto kind = ctx.kinds->find(name);
        if(kind != ctx.kinds->end()) {
            LOG_ERROR_N(node) << "'" << ctx.iface_name << "." << ctx.sub_name << "' references '"
                              << name << "', " << kind->second << " of the interface: only "
                              << "members (nets and variables) are supported through a "
                              << "hierarchical call";
            return 1;
        }
        LOG_ERROR_N(node) << "'" << ctx.iface_name << "." << ctx.sub_name << "' references '"
                          << name << "', which interface '" << ctx.iface_name
                          << "' does not declare";
        return 1;
    }

    if(node->is_node_type(AST::NodeType::NamedType)) {
        LOG_ERROR_N(node) << "'" << ctx.iface_name << "." << ctx.sub_name
                          << "' uses the type name '"
                          << AST::cast_to<AST::NamedType>(node)->get_name()
                          << "': interface-local types are not supported through a "
                          << "hierarchical call";
        return 1;
    }

    // A `disable` target names a block or task (IEEE 1800-2017 §9.6.2) —
    // the block namespace, never a member, even when a label shares a
    // member's spelling. After the splice both live in the clone, so the
    // name stands as written.
    if(node->is_node_type(AST::NodeType::Disable)) {
        const auto &dest = AST::cast_to<AST::Disable>(node)->get_dest();
        if(dest && dest->is_node_category(AST::NodeType::Identifier) &&
           AST::cast_to<AST::Identifier>(dest)->get_hier()) {
            LOG_ERROR_N(node) << "'" << ctx.iface_name << "." << ctx.sub_name
                              << "' disables a hierarchical target, which a spliced "
                              << "subroutine cannot carry";
            return 1;
        }
        return 0;
    }

    if(node->is_node_type(AST::NodeType::Block)) {
        ctx.scopes.push_back(scope_names(AST::cast_to<AST::Block>(node)->get_statements()));
        int ret = 0;
        const auto &children = node->get_children();
        if(children) {
            for(const AST::Node::Ptr &child : *children) {
                if(rewrite_body(child, ctx)) {
                    ret = 1;
                    break;
                }
            }
        }
        ctx.scopes.pop_back();
        return ret;
    }

    const auto &children = node->get_children();
    if(children) {
        for(const AST::Node::Ptr &child : *children) {
            if(rewrite_body(child, ctx)) {
                return 1;
            }
        }
    }
    return 0;
}

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse
