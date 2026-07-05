// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/passes/transformations/name_resolution.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

int NameResolution::run_design(const std::vector<AST::Node::Ptr> &sources)
{
    m_design.clear();
    m_scopes.clear();

    int ret = 0;
    for(const AST::Node::Ptr &source : sources) {
        ret += index_design(source);
    }
    if(ret) {
        return ret;
    }

    for(const AST::Node::Ptr &source : sources) {
        ret += resolve_source(source);
    }
    return ret;
}

int NameResolution::process(AST::Node::Ptr node, AST::Node::Ptr parent)
{
    (void)parent;
    m_design.clear();
    m_scopes.clear();

    int ret = index_design(node);
    if(ret) {
        return ret;
    }
    return resolve_source(node);
}

int NameResolution::index_design(const AST::Node::Ptr &source)
{
    if(!source) {
        return 0;
    }

    switch(source->get_node_type()) {
    case AST::NodeType::Source:
        return index_design(AST::to_node(AST::cast_to<AST::Source>(source)->get_description()));

    case AST::NodeType::Description: {
        const auto &definitions = AST::cast_to<AST::Description>(source)->get_definitions();
        int ret = 0;
        if(definitions) {
            for(const AST::Node::Ptr &def : *definitions) {
                ret += index_design(def);
            }
        }
        return ret;
    }

    case AST::NodeType::Module:
    case AST::NodeType::Interface: {
        const ScopeTable::SymbolKind kind = ScopeTable::classify(source);
        std::string name;
        if(kind == ScopeTable::SymbolKind::MODULE) {
            name = AST::cast_to<AST::Module>(source)->get_name();
        } else {
            name = AST::cast_to<AST::Interface>(source)->get_name();
        }
        const auto it = m_design.find(name);
        if(it != m_design.end()) {
            LOG_ERROR_N(source) << "design element '" << name << "' is defined more than once";
            return 1;
        }
        m_design[name] = DesignEntry{kind, source};
        return 0;
    }

    default:
        return 0;
    }
}

int NameResolution::resolve_source(const AST::Node::Ptr &source) { return walk(source, nullptr); }

void NameResolution::add_scope_item(ScopeTable &table, const AST::Node::Ptr &item)
{
    ScopeTable::for_each_bound_name(
        item, [&table](const std::string &name, const AST::Node::Ptr &denoted) {
            table.add_local(name, denoted);
        });
}

void NameResolution::add_scope_items(ScopeTable &table, const AST::Node::ListPtr &items)
{
    if(items) {
        for(const AST::Node::Ptr &item : *items) {
            add_scope_item(table, item);
        }
    }
}

const ScopeTable::Binding *NameResolution::lookup(const std::string &name) const
{
    for(auto it = m_scopes.rbegin(); it != m_scopes.rend(); ++it) {
        const ScopeTable::Binding *binding = it->lookup(name);
        if(binding) {
            return binding;
        }
    }
    return nullptr;
}

int NameResolution::walk_in_scope(ScopeTable table, const AST::Node::Ptr &node)
{
    m_scopes.push_back(std::move(table));

    int ret = 0;
    const AST::Node::ListPtr children = node->get_children();
    for(const AST::Node::Ptr &child : *children) {
        ret += walk(child, node);
    }

    m_scopes.pop_back();
    return ret;
}

ScopeTable NameResolution::build_definition_scope(const AST::Declaration::ListPtr &params,
                                                  const AST::Port::ListPtr &ports,
                                                  const AST::Node::ListPtr &items)
{
    ScopeTable table;
    if(params) {
        for(const AST::Declaration::Ptr &param : *params) {
            add_scope_item(table, param);
        }
    }
    if(ports) {
        for(const AST::Port::Ptr &port : *ports) {
            add_scope_item(table, port->get_decl());
        }
    }
    add_scope_items(table, items);
    return table;
}

ScopeTable NameResolution::build_subroutine_scope(const std::string &name,
                                                  const AST::Node::Ptr &self,
                                                  const AST::Arg::ListPtr &args,
                                                  const AST::Node::ListPtr &statements)
{
    ScopeTable table;
    // The subroutine's own name is bound inside its body (the legacy return
    // variable of a function, and the callee of a recursive call).
    table.add_local(name, self);
    if(args) {
        for(const AST::Arg::Ptr &arg : *args) {
            add_scope_item(table, arg);
        }
    }
    add_scope_items(table, statements);
    return table;
}

int NameResolution::walk(const AST::Node::Ptr &node, const AST::Node::Ptr &parent)
{
    if(!node) {
        return 0;
    }

    switch(node->get_node_type()) {
    case AST::NodeType::Module: {
        const auto &module = AST::cast_to<AST::Module>(node);
        return walk_in_scope(
            build_definition_scope(module->get_params(), module->get_ports(), module->get_items()),
            node);
    }

    case AST::NodeType::Interface: {
        const auto &interface = AST::cast_to<AST::Interface>(node);
        ++m_interface_depth;
        const int ret =
            walk_in_scope(build_definition_scope(interface->get_params(), interface->get_ports(),
                                                 interface->get_items()),
                          node);
        --m_interface_depth;
        return ret;
    }

    case AST::NodeType::Package:
        return walk_in_scope(
            build_definition_scope(nullptr, nullptr, AST::cast_to<AST::Package>(node)->get_items()),
            node);

    case AST::NodeType::Function: {
        const auto &function = AST::cast_to<AST::Function>(node);
        return walk_in_scope(build_subroutine_scope(function->get_name(), node,
                                                    function->get_args(),
                                                    function->get_statements()),
                             node);
    }

    case AST::NodeType::Task: {
        const auto &task = AST::cast_to<AST::Task>(node);
        return walk_in_scope(build_subroutine_scope(task->get_name(), node, task->get_args(),
                                                    task->get_statements()),
                             node);
    }

    case AST::NodeType::Block: {
        ScopeTable table;
        add_scope_items(table, AST::cast_to<AST::Block>(node)->get_statements());
        return walk_in_scope(std::move(table), node);
    }

    case AST::NodeType::ParallelBlock: {
        // fork/join declares a scope exactly like a begin/end block.
        ScopeTable table;
        add_scope_items(table, AST::cast_to<AST::ParallelBlock>(node)->get_statements());
        return walk_in_scope(std::move(table), node);
    }

    case AST::NodeType::GenerateStatement: {
        // Items declared directly in a generate region (no begin/end) live in
        // their own scope.
        ScopeTable table;
        add_scope_items(table, AST::cast_to<AST::GenerateStatement>(node)->get_items());
        return walk_in_scope(std::move(table), node);
    }

    case AST::NodeType::Call: {
        // Exact type only: FunctionCall/TaskCall are already resolved forms.
        // Arguments first (they may hold casts / type() operands to re-tag);
        // the re-tag then replaces this node in `parent`.
        int ret = 0;
        const AST::Node::ListPtr children = node->get_children();
        for(const AST::Node::Ptr &child : *children) {
            ret += walk(child, node);
        }
        retag_call(AST::cast_to<AST::Call>(node), parent);
        return ret;
    }

    case AST::NodeType::Modport:
        // §25.5: a modport is only legal inside an interface body.
        if(m_interface_depth == 0) {
            LOG_ERROR_N(node) << "modport '" << AST::cast_to<AST::Modport>(node)->get_name()
                              << "' outside an interface (IEEE 1800-2017 25.5)";
            return 1;
        }
        return 0;

    case AST::NodeType::Instancelist: {
        retag_instances(AST::cast_to<AST::Instancelist>(node));
        int ret = 0;
        const AST::Node::ListPtr children = node->get_children();
        for(const AST::Node::Ptr &child : *children) {
            ret += walk(child, node);
        }
        return ret;
    }

    case AST::NodeType::Port: {
        int ret = resolve_port(AST::cast_to<AST::Port>(node));
        const AST::Node::ListPtr children = node->get_children();
        for(const AST::Node::Ptr &child : *children) {
            ret += walk(child, node);
        }
        return ret;
    }

    case AST::NodeType::TypeCast: {
        // Children first: the re-tag replaces this node in `parent`, which
        // must not happen under a child still being walked.
        int ret = 0;
        const AST::Node::ListPtr children = node->get_children();
        for(const AST::Node::Ptr &child : *children) {
            ret += walk(child, node);
        }
        retag_typecast(AST::cast_to<AST::TypeCast>(node), parent);
        return ret;
    }

    case AST::NodeType::TypeOpExpr: {
        int ret = 0;
        const AST::Node::ListPtr children = node->get_children();
        for(const AST::Node::Ptr &child : *children) {
            ret += walk(child, node);
        }
        retag_typeop(AST::cast_to<AST::TypeOpExpr>(node), parent);
        return ret;
    }

    default: {
        int ret = 0;
        // §25.9 applies to every declaration role (Var, Net family, Arg,
        // Param, Member, Typedef, …): an interface is not a data type.
        if(node->is_node_category(AST::NodeType::Declaration)) {
            ret += check_interface_as_data_type(AST::cast_to<AST::Declaration>(node));
        }
        const AST::Node::ListPtr children = node->get_children();
        for(const AST::Node::Ptr &child : *children) {
            ret += walk(child, node);
        }
        return ret;
    }
    }
}

namespace
{
// The NamedType forms this pass may reinterpret: unscoped (a scoped name
// belongs to the package inliner).
bool is_bare_named_type(const AST::DataType::Ptr &type)
{
    if(!type || !type->is_node_type(AST::NodeType::NamedType)) {
        return false;
    }
    const auto &scope = AST::cast_to<AST::NamedType>(type)->get_scope();
    return !scope || scope->empty();
}

AST::InterfaceType::Ptr make_interface_type(const AST::NamedType::Ptr &named)
{
    auto type = std::make_shared<AST::InterfaceType>(named->get_filename(), named->get_line());
    type->set_name(named->get_name());
    return type;
}
} // namespace

const NameResolution::DesignEntry *
NameResolution::design_lookup_unshadowed(const std::string &name, const AST::Node *self) const
{
    // A declaration does not shadow the type reference of its OWN header
    // (`module m(my_if my_if)`, `my_if my_if;`): only a binding to another
    // node hides the design-level name.
    const ScopeTable::Binding *binding = lookup(name);
    if(binding && binding->decl.get() != self) {
        return nullptr;
    }
    const auto it = m_design.find(name);
    return (it == m_design.end()) ? nullptr : &it->second;
}

void NameResolution::retag_instances(const AST::Instancelist::Ptr &instancelist)
{
    const auto it = m_design.find(instancelist->get_module());
    if(it == m_design.end() || it->second.kind != ScopeTable::SymbolKind::INTERFACE) {
        return;
    }

    const AST::Instance::ListPtr instances = instancelist->get_instances();
    if(!instances) {
        return;
    }

    for(AST::Instance::Ptr &slot : *instances) {
        if(!slot || !slot->is_node_type(AST::NodeType::Instance)) {
            continue;
        }
        auto tagged =
            std::make_shared<AST::InterfaceInstance>(slot->get_filename(), slot->get_line());
        tagged->set_module(slot->get_module());
        tagged->set_name(slot->get_name());
        tagged->set_array(slot->get_array());
        tagged->set_parameterlist(slot->get_parameterlist());
        tagged->set_portlist(slot->get_portlist());
        slot = tagged;
    }
}

int NameResolution::resolve_port(const AST::Port::Ptr &port)
{
    const AST::Declaration::Ptr decl = port->get_decl();
    if(!decl) {
        return 0;
    }

    // Deferred bare form: Port{ decl: Arg{ type: NamedType } } (ADR-0003 §4.4).
    if(decl->is_node_type(AST::NodeType::Arg) && is_bare_named_type(decl->get_type())) {
        const auto &arg = AST::cast_to<AST::Arg>(decl);
        const auto &named = AST::cast_to<AST::NamedType>(decl->get_type());
        const std::string &name = named->get_name();

        const ScopeTable::Binding *binding = lookup(name);
        if(binding && binding->decl.get() == static_cast<const AST::Node *>(decl.get())) {
            // A port named like its own type (`module m(my_if my_if)`): the
            // port's own binding is not a type reference.
            binding = nullptr;
        }
        if(binding) {
            if(binding->kind == ScopeTable::SymbolKind::TYPE) {
                // The other side of the ambiguity: a typed net port whose
                // omitted direction stays unresolved (the §23.2.2.3
                // default/inheritance is not fabricated).
                auto net =
                    std::make_shared<AST::ImplicitNet>(decl->get_filename(), decl->get_line());
                net->set_name(arg->get_name());
                net->set_unpacked_dims(arg->get_unpacked_dims());
                net->set_type(arg->get_type());
                port->set_decl(net);
                LOG_WARNING_N(port) << "typed port '" << arg->get_name()
                                    << "' has no direction; the IEEE 1800-2017 23.2.2.3 "
                                       "default is left to a later pass";
                return 0;
            }
            LOG_WARNING_N(port) << "port type '" << name
                                << "' does not name a type or an interface; left unresolved";
            return 0;
        }

        const DesignEntry *entry = design_lookup_unshadowed(name, decl.get());
        if(entry && entry->kind == ScopeTable::SymbolKind::INTERFACE) {
            if(named->get_packed_dims() && !named->get_packed_dims()->empty()) {
                LOG_ERROR_N(port) << "an interface port type takes no packed dimensions";
                return 1;
            }
            decl->set_type(make_interface_type(named));
            return 0;
        }

        LOG_WARNING_N(port) << "port type '" << name << "' is unresolved";
        return 0;
    }

    // Inherited-direction form that proves to be an interface (ADR-0002 §3.1):
    // Port{ dir, decl: ImplicitNet{ NamedType } } — an interface port admits no
    // direction (A.1.3), so the §23.2.2.3 inheritance is undone.
    if(decl->is_node_type(AST::NodeType::ImplicitNet) && is_bare_named_type(decl->get_type())) {
        const auto &named = AST::cast_to<AST::NamedType>(decl->get_type());
        const DesignEntry *entry = design_lookup_unshadowed(named->get_name(), decl.get());
        if(!entry || entry->kind != ScopeTable::SymbolKind::INTERFACE) {
            return 0;
        }
        if(named->get_packed_dims() && !named->get_packed_dims()->empty()) {
            LOG_ERROR_N(port) << "an interface port type takes no packed dimensions";
            return 1;
        }
        auto arg = std::make_shared<AST::Arg>(decl->get_filename(), decl->get_line());
        arg->set_name(decl->get_name());
        arg->set_direction(AST::Arg::DirectionEnum::NONE);
        arg->set_unpacked_dims(AST::cast_to<AST::Net>(decl)->get_unpacked_dims());
        arg->set_type(make_interface_type(named));
        port->set_direction(AST::Port::DirectionEnum::NONE);
        port->set_decl(arg);
    }

    return 0;
}

void NameResolution::retag_typecast(const AST::TypeCast::Ptr &cast, const AST::Node::Ptr &parent)
{
    const AST::DataType::Ptr target = cast->get_target();
    if(!target || !target->is_node_type(AST::NodeType::NamedType) || !parent) {
        return;
    }
    const auto &named = AST::cast_to<AST::NamedType>(target);

    if(named->get_scope() && !named->get_scope()->empty()) {
        LOG_WARNING_N(cast) << "scoped cast target '" << named->get_name()
                            << "' left unresolved (package scopes are resolved by the "
                               "package inliner)";
        return;
    }

    // A dimensioned named type (`my_t [3:0]'(x)`) can only be a type cast.
    if(named->get_packed_dims() && !named->get_packed_dims()->empty()) {
        return;
    }

    const ScopeTable::Binding *binding = lookup(named->get_name());
    if(!binding) {
        LOG_WARNING_N(cast) << "cast target '" << named->get_name()
                            << "' is unresolved (type or size)";
        return;
    }

    switch(binding->kind) {
    case ScopeTable::SymbolKind::VALUE: {
        // `WIDTH'(x)` with WIDTH a value: a size cast, not a type cast.
        auto sizecast = std::make_shared<AST::SizeCast>(cast->get_filename(), cast->get_line());
        auto size = std::make_shared<AST::Identifier>(named->get_filename(), named->get_line());
        size->set_name(named->get_name());
        sizecast->set_size(size);
        sizecast->set_expr(cast->get_expr());
        parent->replace(cast, sizecast);
        return;
    }

    case ScopeTable::SymbolKind::TYPE:
        return;

    default:
        LOG_WARNING_N(cast) << "cast target '" << named->get_name()
                            << "' names neither a type nor a value; left unresolved";
        return;
    }
}

void NameResolution::retag_typeop(const AST::TypeOpExpr::Ptr &typeop, const AST::Node::Ptr &parent)
{
    const AST::Node::Ptr expr = typeop->get_expr();
    if(!expr || !expr->is_node_type(AST::NodeType::Identifier) || !parent) {
        return;
    }
    const auto &identifier = AST::cast_to<AST::Identifier>(expr);
    if(identifier->get_hier() || (identifier->get_scope() && !identifier->get_scope()->empty())) {
        return;
    }

    const ScopeTable::Binding *binding = lookup(identifier->get_name());
    if(!binding) {
        LOG_WARNING_N(typeop) << "type() operand '" << identifier->get_name()
                              << "' is unresolved (type or expression)";
        return;
    }

    if(binding->kind == ScopeTable::SymbolKind::TYPE) {
        // `type(my_t)`: the operand is a type name, not an expression.
        auto typeoptype =
            std::make_shared<AST::TypeOpType>(typeop->get_filename(), typeop->get_line());
        auto named =
            std::make_shared<AST::NamedType>(identifier->get_filename(), identifier->get_line());
        named->set_name(identifier->get_name());
        typeoptype->set_type(named);
        parent->replace(typeop, typeoptype);
    }
}

int NameResolution::check_interface_as_data_type(const AST::Declaration::Ptr &decl)
{
    if(!is_bare_named_type(decl->get_type())) {
        return 0;
    }
    const auto &named = AST::cast_to<AST::NamedType>(decl->get_type());
    const DesignEntry *entry = design_lookup_unshadowed(named->get_name(), decl.get());
    if(entry && entry->kind == ScopeTable::SymbolKind::INTERFACE) {
        LOG_ERROR_N(decl) << "interface '" << named->get_name()
                          << "' used as a data type requires 'virtual' (IEEE 1800-2017 25.9)";
        return 1;
    }
    return 0;
}

void NameResolution::retag_call(const AST::Call::Ptr &call, const AST::Node::Ptr &parent)
{
    // Hierarchical calls (u1.t()) belong to elaboration, not to this pass.
    if(call->get_hier()) {
        return;
    }

    // A scoped call (pkg::t()) only reaches this pass un-inlined; its bare
    // name must not be looked up in the lexical stack (it could bind an
    // unrelated local), so it stays neutral.
    if(call->get_scope() && !call->get_scope()->empty()) {
        LOG_WARNING_N(call) << "scoped call '" << call->get_name()
                            << "' left unresolved (package scopes are resolved by "
                               "the package inliner)";
        return;
    }

    if(!parent) {
        return;
    }

    const ScopeTable::Binding *binding = lookup(call->get_name());
    if(!binding) {
        LOG_WARNING_N(call) << "unresolved statement call to '" << call->get_name() << "'";
        return;
    }

    switch(binding->kind) {
    case ScopeTable::SymbolKind::TASK: {
        auto taskcall = std::make_shared<AST::TaskCall>(call->get_filename(), call->get_line());
        taskcall->set_name(call->get_name());
        taskcall->set_args(call->get_args());
        parent->replace(call, taskcall);
        return;
    }

    case ScopeTable::SymbolKind::FUNCTION: {
        // Legal as a statement; the §13.4.1 discarded-value warning applies
        // only when there IS a value — a void function has none.
        const auto &function = AST::cast_to<AST::Function>(binding->decl);
        const AST::DataType::Ptr return_type = function->get_return_type();
        if(!return_type || !return_type->is_node_type(AST::NodeType::VoidType)) {
            LOG_WARNING_N(call) << "function '" << call->get_name()
                                << "' called as a statement; its return value is discarded";
        }
        auto functioncall =
            std::make_shared<AST::FunctionCall>(call->get_filename(), call->get_line());
        functioncall->set_name(call->get_name());
        functioncall->set_args(call->get_args());
        parent->replace(call, functioncall);
        return;
    }

    default:
        LOG_WARNING_N(call) << "'" << call->get_name()
                            << "' is not a task or function; statement call left unresolved";
        return;
    }
}

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse
