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

namespace
{

// A module holding an array typedef (`typedef logic [7:0] mem_t [4];`) plus a
// declaration of the given kind whose type slot references it. Neither shape
// is producible by the parser today (`nettype` has no grammar; TypeParamInliner
// reduces every TypeParam before this pass runs), so the AST is built by hand:
// it models a dims-less declaration kind reaching the array-typedef merge.
AST::Module::Ptr build_array_alias_module(const AST::Declaration::Ptr &decl)
{
    const auto &tdef = std::make_shared<AST::Typedef>();
    tdef->set_name("mem_t");
    const auto &elem = std::make_shared<AST::LogicType>();
    const auto &range = std::make_shared<AST::RangeDim>();
    range->set_left(std::make_shared<AST::IntConstN>(10, -1, true, 7));
    range->set_right(std::make_shared<AST::IntConstN>(10, -1, true, 0));
    const auto &pdims = std::make_shared<AST::Dimension::List>();
    pdims->push_back(range);
    elem->set_packed_dims(pdims);
    tdef->set_type(elem);
    const auto &size = std::make_shared<AST::SizeDim>();
    size->set_size(std::make_shared<AST::IntConstN>(10, -1, true, 4));
    const auto &udims = std::make_shared<AST::Dimension::List>();
    udims->push_back(size);
    tdef->set_unpacked_dims(udims);

    const auto &ref = std::make_shared<AST::NamedType>();
    ref->set_name("mem_t");
    decl->set_type(ref);

    const auto &items = std::make_shared<AST::Node::List>();
    items->push_back(tdef);
    items->push_back(decl);
    const auto &module = std::make_shared<AST::Module>();
    module->set_name("m");
    module->set_items(items);
    return module;
}

} // namespace

// An array typedef in a NetTypeDecl's type slot must be rejected: unpacked
// dims are meaningless on a nettype declaration, and the base unpacked_dims
// slot it would silently absorb them into has no consumer there.
TEST(PassesTransformation_TypedefInliner, array_alias_nettypedecl_rejected)
{
    ENABLE_LOGGER;

    const auto &decl = std::make_shared<AST::NetTypeDecl>();
    decl->set_name("nt");
    const auto &module = build_array_alias_module(decl);
    ASSERT_NE(0, Passes::Transformations::TypedefInliner().run(module));
}

// Same for a TypeParam surviving to this pass (it cannot, in the standard
// pipeline — TypeParamInliner reduces them first — but the guard must not
// depend on pass ordering).
TEST(PassesTransformation_TypedefInliner, array_alias_typeparam_rejected)
{
    ENABLE_LOGGER;

    const auto &decl = std::make_shared<AST::TypeParam>();
    decl->set_name("T");
    const auto &module = build_array_alias_module(decl);
    ASSERT_NE(0, Passes::Transformations::TypedefInliner().run(module));
}

// Control: the identical module with a Var absorbs the alias dims onto the
// declaration (the ADR-0009 §5 merge).
TEST(PassesTransformation_TypedefInliner, array_alias_var_merged)
{
    ENABLE_LOGGER;

    const auto &decl = std::make_shared<AST::Var>();
    decl->set_name("v");
    const auto &module = build_array_alias_module(decl);
    ASSERT_EQ(0, Passes::Transformations::TypedefInliner().run(module));

    ASSERT_EQ(std::size_t(1), module->get_items()->size());
    const auto &var = AST::cast_to<AST::Var>(module->get_items()->front());
    ASSERT_TRUE(var->get_unpacked_dims());
    ASSERT_EQ(std::size_t(1), var->get_unpacked_dims()->size());
    ASSERT_TRUE(var->get_type()->is_node_type(AST::NodeType::LogicType));
}
