// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include "../../helpers/helpers.hpp"

#include <veriparse/passes/transformations/package_inliner.hpp>
#include <veriparse/importers/yaml_importer.hpp>
#include <veriparse/parser/verilog.hpp>
#include <veriparse/AST/nodes.hpp>
#include <veriparse/AST/node_cast.hpp>

#include <gtest/gtest.h>

using namespace Veriparse;

static TestHelpers test_helpers("lib/test/passes/transformations/testcases/");

// Count the nodes of `type` reachable from `node`.
static int count_nodes(const AST::Node::Ptr &node, AST::NodeType type)
{
    if(!node) {
        return 0;
    }
    int n = node->is_node_type(type) ? 1 : 0;
    AST::Node::ListPtr children = node->get_children();
    for(const AST::Node::Ptr &child : *children) {
        n += count_nodes(child, type);
    }
    return n;
}

// Count value/type references that still carry an unresolved `::` scope.
static int count_scoped_refs(const AST::Node::Ptr &node)
{
    if(!node) {
        return 0;
    }
    int n = 0;
    if(node->is_node_category(AST::NodeType::Identifier)) {
        const auto scope = AST::cast_to<AST::Identifier>(node)->get_scope();
        if(scope && !scope->empty()) {
            n++;
        }
    } else if(node->is_node_type(AST::NodeType::NamedType)) {
        const auto scope = AST::cast_to<AST::NamedType>(node)->get_scope();
        if(scope && !scope->empty()) {
            n++;
        }
    }
    AST::Node::ListPtr children = node->get_children();
    for(const AST::Node::Ptr &child : *children) {
        n += count_scoped_refs(child);
    }
    return n;
}

#define TEST_CORE_SV                                                                               \
    ENABLE_LOGGER;                                                                                 \
                                                                                                   \
    /* Parse the SV file (package + module in one compilation unit) */                             \
    Parser::Verilog verilog;                                                                       \
    verilog.set_sv_mode(true);                                                                     \
    verilog.parse(test_helpers.get_sv_filename(test_name));                                        \
    AST::Node::Ptr source = verilog.get_source();                                                  \
    ASSERT_TRUE(source != nullptr);                                                                \
                                                                                                   \
    /* Apply the pass on the whole source */                                                       \
    test_helpers.render_node_to_verilog_file(source, test_string + "_before.v");                   \
    Passes::Transformations::PackageInliner().run(source);                                         \
    ASSERT_AST_IS_TREE(source); /* pass output must stay a proper tree */                          \
    test_helpers.render_node_to_verilog_file(source, test_string + ".v");                          \
    test_helpers.render_node_to_yaml_file(source, test_string + ".yaml");                          \
    test_helpers.render_node_to_dot_file(source, test_string + ".dot");                            \
                                                                                                   \
    /* Load the reference */                                                                       \
    std::string ref_filename = std::string("refs/package_inliner_") + test_name;                   \
    AST::Node::Ptr source_ref =                                                                    \
        Importers::YAMLImporter().import(test_helpers.get_yaml_filename(ref_filename).c_str());    \
    ASSERT_TRUE(source_ref != nullptr);                                                            \
    test_helpers.render_node_to_verilog_file(source_ref, test_string + "_ref.v");                  \
                                                                                                   \
    /* Check */                                                                                    \
    ASSERT_TRUE(source_ref->is_equal(*source, false))

/* Error cases: the pass must report a non-zero status (undefined package /
 * symbol, multi-wildcard ambiguity) rather than silently mis-resolving. */
#define TEST_ERROR_SV                                                                              \
    ENABLE_LOGGER;                                                                                 \
                                                                                                   \
    Parser::Verilog verilog;                                                                       \
    verilog.set_sv_mode(true);                                                                     \
    verilog.parse(test_helpers.get_sv_filename(test_name));                                        \
    AST::Node::Ptr source = verilog.get_source();                                                  \
    ASSERT_TRUE(source != nullptr);                                                                \
                                                                                                   \
    ASSERT_NE(Passes::Transformations::PackageInliner().run(source), 0)

TEST(PassesTransformation_PackageInliner, package_inliner0) { TEST_CORE_SV; }
TEST(PassesTransformation_PackageInliner, package_inliner1) { TEST_CORE_SV; }
TEST(PassesTransformation_PackageInliner, package_inliner2) { TEST_CORE_SV; }
TEST(PassesTransformation_PackageInliner, package_inliner3) { TEST_CORE_SV; }
TEST(PassesTransformation_PackageInliner, package_inliner4) { TEST_CORE_SV; }
// Dependency copy: P::data_t pulls in the W it depends on (ADR-0004 §9.3).
TEST(PassesTransformation_PackageInliner, package_inliner_dep) { TEST_CORE_SV; }
// Package imports package: P2 uses P1::A in B; resolving P2 as a scope makes it
// self-contained, and the module copying P2::B pulls A too (§9.2 + §9.3).
TEST(PassesTransformation_PackageInliner, package_inliner_pkgdep) { TEST_CORE_SV; }
// Precedence: an explicit `import p2::X` shadows the wildcard `import p1::*`, so
// X resolves to p2's X, not p1's (IEEE 1800-2017 §26.5).
TEST(PassesTransformation_PackageInliner, package_inliner_shadow) { TEST_CORE_SV; }
// Lazy wildcard: only the referenced name (USED) is imported; UNUSED is not
// copied in (§26.5 — a wildcard name is imported only when referenced).
TEST(PassesTransformation_PackageInliner, package_inliner_lazy) { TEST_CORE_SV; }
// The synthesizable subset beyond params: an enum typedef, and a function
// (called via a wildcard import, pulling its localparam dependency).
TEST(PassesTransformation_PackageInliner, package_inliner_func) { TEST_CORE_SV; }
// A package task, copied in and its call resolved.
TEST(PassesTransformation_PackageInliner, package_inliner_task) { TEST_CORE_SV; }
// A function arg shadows a same-named package symbol: copying the function must
// NOT pull that package symbol in as a (false) dependency.
TEST(PassesTransformation_PackageInliner, package_inliner_func_shadow) { TEST_CORE_SV; }
// Same, for a task arg — the free-name dependency analysis covers Task too.
TEST(PassesTransformation_PackageInliner, package_inliner_task_shadow) { TEST_CORE_SV; }
// Re-export: P2 re-exports P1, a module imports P2 and uses P1's A (§26.6).
TEST(PassesTransformation_PackageInliner, package_inliner_reexport) { TEST_CORE_SV; }
// A module-local declaration shadows a re-exported name (§26.5 + §26.6).
TEST(PassesTransformation_PackageInliner, package_inliner_reexport_shadow) { TEST_CORE_SV; }
// Explicit `export P1::A` of a wildcard-imported-but-unused name forces its
// import, so a module importing P2 still sees A (§26.6).
TEST(PassesTransformation_PackageInliner, package_inliner_reexport_force) { TEST_CORE_SV; }
// Same declaration via two wildcard paths (import P1::* and P2::* where P2
// re-exports P1::X) is NOT a conflict — origin-dedup resolves it (§26.6).
TEST(PassesTransformation_PackageInliner, package_inliner_reexport_multipath) { TEST_CORE_SV; }
// `export *::*` re-exports everything the package imported (§26.6).
TEST(PassesTransformation_PackageInliner, package_inliner_reexport_star) { TEST_CORE_SV; }
// origin-dedup across an EXPLICIT import re-exported via `export *::*`: a module
// importing both P1 and P2 (which explicitly imports + `*::*`-re-exports P1::X)
// resolves X — its origin is recovered for explicit imports, not just wildcards.
TEST(PassesTransformation_PackageInliner, package_inliner_reexport_star_explicit) { TEST_CORE_SV; }
// Chained re-export P3 <- P2 <- P1: X reaches the module through two re-exports,
// its origin carried transitively (§26.6).
TEST(PassesTransformation_PackageInliner, package_inliner_reexport_chain) { TEST_CORE_SV; }

// Enum members are package symbols (§6.19): a wildcard import binds an
// enumerator referenced WITHOUT naming its typedef (the containing typedef is
// the copied unit).
TEST(PassesTransformation_PackageInliner, package_inliner_enum0) { TEST_CORE_SV; }
// Qualified enumerator refs (pkg::A, pkg::B) plus the typedef itself: the
// declaration is copied exactly once (every bound name deduped on copy).
TEST(PassesTransformation_PackageInliner, package_inliner_enum1) { TEST_CORE_SV; }

// Same-declaration diamond via an EXPLICIT typedef import re-exported by P:
// importing P::* and Q::* and referencing an enumerator of that typedef is NOT
// ambiguous — both paths reach Q's declaration (§26.6, ADR-0004 §9.5).
TEST(PassesTransformation_PackageInliner, package_inliner_enum_diamond)
{
    ENABLE_LOGGER;

    Parser::Verilog verilog;
    verilog.set_sv_mode(true);
    verilog.parse(test_helpers.get_sv_filename(test_name));
    AST::Node::Ptr source = verilog.get_source();
    ASSERT_TRUE(source != nullptr);

    ASSERT_EQ(Passes::Transformations::PackageInliner().run(source), 0);
    ASSERT_AST_IS_TREE(source);
    EXPECT_EQ(count_scoped_refs(source), 0); // PED_A bound, not left dangling
}

// Ranged enumerators (§6.19.2): `enum {V[3]}` declares V0..V2 and `{W[6:4]}`
// declares W6..W4 in the enclosing scope — those generated names, not the bare
// base name, are what an import binds.
TEST(PassesTransformation_PackageInliner, package_inliner_enum_ranged)
{
    ENABLE_LOGGER;

    Parser::Verilog verilog;
    verilog.set_sv_mode(true);
    verilog.parse(test_helpers.get_sv_filename(test_name));
    AST::Node::Ptr source = verilog.get_source();
    ASSERT_TRUE(source != nullptr);

    ASSERT_EQ(Passes::Transformations::PackageInliner().run(source), 0);
    ASSERT_AST_IS_TREE(source);
    EXPECT_EQ(count_scoped_refs(source), 0); // per_pkg::PER_W5 resolved

    // Both referenced typedefs must be copied into the module (packages are
    // stripped after resolution, so any remaining Typedef is a copied one).
    EXPECT_EQ(count_nodes(source, AST::NodeType::Typedef), 2);
}

// A copied declaration's SIBLING bound name (the enum typedef name behind an
// imported enumerator) collides with a module-local declaration: localizing
// would splice a duplicate `e_t` — hard error, not a silent clobber.
TEST(PassesTransformation_PackageInliner, package_inliner_enum_sibling_shadow) { TEST_ERROR_SV; }
// Same collision through two explicit imports: materializing P::PEEC_A binds
// its typedef name `e_t`, which the later `import Q::e_t` must not silently
// rebind to (nor be shadowed by) — hard error.
TEST(PassesTransformation_PackageInliner, package_inliner_enum_explicit_clobber) { TEST_ERROR_SV; }
// A copied symbol's same-package dependency (an enumerator) is shadowed by a
// module-local enumerator: inlining `localparam W = PEDS_A;` would rebind it to
// the local's value — hard error, not a silent wrong constant.
TEST(PassesTransformation_PackageInliner, package_inliner_enum_dep_shadow) { TEST_ERROR_SV; }
// Two package items binding the same name (a localparam and an enumerator) is a
// duplicate declaration in the package scope (§6.19/§26.2) — diagnosed at
// collect, not resolved last-wins.
TEST(PassesTransformation_PackageInliner, package_inliner_enum_dup) { TEST_ERROR_SV; }

TEST(PassesTransformation_PackageInliner, package_inliner_undef_pkg) { TEST_ERROR_SV; }
TEST(PassesTransformation_PackageInliner, package_inliner_undef_sym) { TEST_ERROR_SV; }
TEST(PassesTransformation_PackageInliner, package_inliner_ambiguous) { TEST_ERROR_SV; }
TEST(PassesTransformation_PackageInliner, package_inliner_collision) { TEST_ERROR_SV; }
// Re-export of a name the package never imported is an error (§26.6).
TEST(PassesTransformation_PackageInliner, package_inliner_reexport_err) { TEST_ERROR_SV; }

// Multi-source: a package defined in one file, referenced from a module in
// another (the veriflat model — global collect, per-unit resolve). Proves a
// package name crosses files while a top-level import does not.
TEST(PassesTransformation_PackageInliner, package_inliner_multi)
{
    ENABLE_LOGGER;

    Parser::Verilog va;
    va.set_sv_mode(true);
    va.parse(test_helpers.get_sv_filename("package_inliner_multi_a"));
    AST::Node::Ptr src_a = va.get_source();
    ASSERT_TRUE(src_a != nullptr);

    Parser::Verilog vb;
    vb.set_sv_mode(true);
    vb.parse(test_helpers.get_sv_filename("package_inliner_multi_b"));
    AST::Node::Ptr src_b = vb.get_source();
    ASSERT_TRUE(src_b != nullptr);

    // Units in compilation order: file A (defines xpkg) before file B (uses it).
    // run_units collects-then-resolves each in turn, so B sees xpkg from A.
    Passes::Transformations::PackageInliner inliner;
    ASSERT_EQ(inliner.run_units({src_a, src_b}), 0);
    ASSERT_AST_IS_TREE(src_a);
    ASSERT_AST_IS_TREE(src_b);

    // Golden on unit B: `xpkg::XW` (package from file A) is inlined here.
    test_helpers.render_node_to_verilog_file(src_b, test_string + ".v");
    test_helpers.render_node_to_yaml_file(src_b, test_string + ".yaml");
    test_helpers.render_node_to_dot_file(src_b, test_string + ".dot");
    std::string ref_filename = std::string("refs/package_inliner_") + test_name;
    AST::Node::Ptr ref =
        Importers::YAMLImporter().import(test_helpers.get_yaml_filename(ref_filename).c_str());
    ASSERT_TRUE(ref != nullptr);
    ASSERT_TRUE(ref->is_equal(*src_b, false));
}

// Compilation order (§26.3): a unit may use a package from an EARLIER unit, not
// a later one. Same two files, opposite order — the reference resolves only when
// the package's unit precedes the module's unit.
TEST(PassesTransformation_PackageInliner, package_inliner_order)
{
    ENABLE_LOGGER;

    // Correct order: the package unit (order_b) before the user unit (order_a).
    {
        Parser::Verilog vb;
        vb.set_sv_mode(true);
        vb.parse(test_helpers.get_sv_filename("package_inliner_order_b"));
        AST::Node::Ptr src_b = vb.get_source();
        Parser::Verilog va;
        va.set_sv_mode(true);
        va.parse(test_helpers.get_sv_filename("package_inliner_order_a"));
        AST::Node::Ptr src_a = va.get_source();
        ASSERT_TRUE(src_a != nullptr && src_b != nullptr);

        Passes::Transformations::PackageInliner inliner;
        ASSERT_EQ(inliner.run_units({src_b, src_a}), 0);
        EXPECT_EQ(count_scoped_refs(src_a), 0); // latepkg::LW resolved
    }

    // Wrong order: the user unit before the package unit — a hard error, since
    // the reference is to a package not yet (and, in order, not legally) visible.
    {
        Parser::Verilog va;
        va.set_sv_mode(true);
        va.parse(test_helpers.get_sv_filename("package_inliner_order_a"));
        AST::Node::Ptr src_a = va.get_source();
        Parser::Verilog vb;
        vb.set_sv_mode(true);
        vb.parse(test_helpers.get_sv_filename("package_inliner_order_b"));
        AST::Node::Ptr src_b = vb.get_source();
        ASSERT_TRUE(src_a != nullptr && src_b != nullptr);

        Passes::Transformations::PackageInliner inliner;
        ASSERT_NE(inliner.run_units({src_a, src_b}), 0); // latepkg::LW unknown here
    }
}
