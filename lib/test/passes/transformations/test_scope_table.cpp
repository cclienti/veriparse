// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/scope_table.hpp>
#include <gtest/gtest.h>

using namespace Veriparse;
using Passes::Transformations::ScopeTable;

using Kind = ScopeTable::SymbolKind;

TEST(PassesTransformation_ScopeTable, classify_kinds)
{
    EXPECT_EQ(Kind::UNKNOWN, ScopeTable::classify(nullptr));
    EXPECT_EQ(Kind::UNKNOWN, ScopeTable::classify(std::make_shared<AST::Identifier>()));

    EXPECT_EQ(Kind::VALUE, ScopeTable::classify(std::make_shared<AST::Var>()));
    EXPECT_EQ(Kind::VALUE, ScopeTable::classify(std::make_shared<AST::WireNet>()));
    EXPECT_EQ(Kind::VALUE, ScopeTable::classify(std::make_shared<AST::Param>()));
    EXPECT_EQ(Kind::VALUE, ScopeTable::classify(std::make_shared<AST::Arg>()));
    EXPECT_EQ(Kind::VALUE, ScopeTable::classify(std::make_shared<AST::Member>()));
    EXPECT_EQ(Kind::VALUE, ScopeTable::classify(std::make_shared<AST::Genvar>()));
    EXPECT_EQ(Kind::VALUE, ScopeTable::classify(std::make_shared<AST::EnumItem>()));

    EXPECT_EQ(Kind::TYPE, ScopeTable::classify(std::make_shared<AST::Typedef>()));
    EXPECT_EQ(Kind::TYPE, ScopeTable::classify(std::make_shared<AST::TypeParam>()));

    EXPECT_EQ(Kind::FUNCTION, ScopeTable::classify(std::make_shared<AST::Function>()));
    EXPECT_EQ(Kind::TASK, ScopeTable::classify(std::make_shared<AST::Task>()));
    EXPECT_EQ(Kind::MODULE, ScopeTable::classify(std::make_shared<AST::Module>()));
    EXPECT_EQ(Kind::INTERFACE, ScopeTable::classify(std::make_shared<AST::Interface>()));
}

TEST(PassesTransformation_ScopeTable, lookup_precedence_and_kind)
{
    ScopeTable table;

    // Wildcard first, then explicit, then local — precedence must not depend
    // on insertion order.
    table.add_wildcard_import("x", "P1", std::make_shared<AST::Task>(), "P1");
    table.add_explicit_import("x", "P2", std::make_shared<AST::Function>());
    table.add_local("x", std::make_shared<AST::Typedef>());

    const ScopeTable::Binding *b = table.lookup("x");
    ASSERT_NE(nullptr, b);
    EXPECT_EQ(ScopeTable::Origin::Local, b->origin);
    EXPECT_EQ(Kind::TYPE, b->kind);

    table.add_explicit_import("y", "P2", std::make_shared<AST::Function>());
    b = table.lookup("y");
    ASSERT_NE(nullptr, b);
    EXPECT_EQ(ScopeTable::Origin::ExplicitImport, b->origin);
    EXPECT_EQ(Kind::FUNCTION, b->kind);

    table.add_wildcard_import("z", "P1", std::make_shared<AST::Task>(), "P1");
    b = table.lookup("z");
    ASSERT_NE(nullptr, b);
    EXPECT_EQ(ScopeTable::Origin::WildcardImport, b->origin);
    EXPECT_EQ(Kind::TASK, b->kind);

    EXPECT_EQ(nullptr, table.lookup("unbound"));
}

TEST(PassesTransformation_ScopeTable, wildcard_ambiguity_on_use)
{
    ScopeTable table;

    // Two wildcard paths to the SAME defining package: not a conflict (§26.6).
    table.add_wildcard_import("a", "P1", std::make_shared<AST::Param>(), "P0");
    table.add_wildcard_import("a", "P2", std::make_shared<AST::Param>(), "P0");
    bool ambiguous = true;
    const ScopeTable::Binding *b = table.lookup("a", &ambiguous);
    ASSERT_NE(nullptr, b);
    EXPECT_FALSE(ambiguous);
    EXPECT_EQ(Kind::VALUE, b->kind);

    // Two wildcard paths to DIFFERENT declarations: ambiguous on use.
    table.add_wildcard_import("c", "P1", std::make_shared<AST::Param>(), "P1");
    table.add_wildcard_import("c", "P2", std::make_shared<AST::Param>(), "P2");
    ambiguous = false;
    EXPECT_EQ(nullptr, table.lookup("c", &ambiguous));
    EXPECT_TRUE(ambiguous);

    // A local shadows even an ambiguous wildcard pair.
    table.add_local("c", std::make_shared<AST::Var>());
    ambiguous = true;
    b = table.lookup("c", &ambiguous);
    ASSERT_NE(nullptr, b);
    EXPECT_FALSE(ambiguous);
    EXPECT_EQ(Kind::VALUE, b->kind);
}
