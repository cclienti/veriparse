// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include "../../helpers/helpers.hpp"
#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/typedef_inliner.hpp>
#include <veriparse/logger/logger.hpp>
#include <gtest/gtest.h>

using namespace Veriparse;

namespace
{

// A module holding `typedef logic T;` and `assign b = <cast>;` — the cast's
// target is a NamedType "T", package-scoped or not. The scoped form cannot be
// produced by the parser (the cast grammar only admits a bare identifier), so
// the AST is built by hand: it models a package-scoped target that survived
// package inlining unresolved.
AST::Module::Ptr build_cast_module(bool scoped_target)
{
    const auto &tdef = std::make_shared<AST::Typedef>();
    tdef->set_name("T");
    tdef->set_type(std::make_shared<AST::LogicType>());

    const auto &target = std::make_shared<AST::NamedType>();
    target->set_name("T");
    if(scoped_target) {
        const auto &seg = std::make_shared<AST::ScopeName>();
        seg->set_name("pkg");
        const auto &scope = std::make_shared<AST::ScopeName::List>();
        scope->push_back(seg);
        target->set_scope(scope);
    }

    const auto &cast = std::make_shared<AST::TypeCast>();
    cast->set_target(target);
    cast->set_expr(std::make_shared<AST::Identifier>(nullptr, nullptr, "a"));

    const auto &rvalue = std::make_shared<AST::Rvalue>();
    rvalue->set_var(cast);
    const auto &lvalue = std::make_shared<AST::Lvalue>();
    lvalue->set_var(std::make_shared<AST::Identifier>(nullptr, nullptr, "b"));
    const auto &assign = std::make_shared<AST::Assign>();
    assign->set_left(lvalue);
    assign->set_right(rvalue);

    const auto &items = std::make_shared<AST::Node::List>();
    items->push_back(tdef);
    items->push_back(assign);
    const auto &module = std::make_shared<AST::Module>();
    module->set_name("m");
    module->set_items(items);
    return module;
}

} // namespace

// A package-scoped cast target (`pkg::T'(a)`) must be rejected: the scope is
// resolved (and stripped) by PackageInliner before this pass, so a surviving
// one is unresolved — looking up the bare name would silently bind the
// same-named local typedef instead.
TEST(PassesTransformation_TypedefInliner, cast_scoped_target_rejected)
{
    ENABLE_LOGGER;

    const auto &module = build_cast_module(true);
    ASSERT_NE(0, Passes::Transformations::TypedefInliner().run(module));
}

// Control: the identical module with an unscoped target resolves the cast
// through the local typedef and lowers it to a size cast.
TEST(PassesTransformation_TypedefInliner, cast_local_target_lowered)
{
    ENABLE_LOGGER;

    const auto &module = build_cast_module(false);
    ASSERT_EQ(0, Passes::Transformations::TypedefInliner().run(module));

    ASSERT_EQ(std::size_t(1), module->get_items()->size());
    const auto &assign = AST::cast_to<AST::Assign>(module->get_items()->front());
    const auto &var = assign->get_right()->get_var();
    ASSERT_TRUE(var->is_node_type(AST::NodeType::SizeCast));
}
