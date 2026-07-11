# ADR-0009 — Typedef inlining & enum-type lowering (`TypedefInliner`)

- **Status**: Proposed — design settled; implementation pending.
- **Date**: 2026-07-11
- **Scope**: A transformation pass that **substitutes every user-defined type
  name (`typedef`) with its underlying data type** and **lowers residual enum
  decl types to their base type**, so no `Typedef` item and no `NamedType`
  reference survives into flattening. Also closes the grammar gap that rejects
  a compilation-unit-scope `typedef`. Explicitly **not** in scope: making
  struct/union types *transformable* (they substitute faithfully but stay
  unsupported downstream), typedefs inside task/function bodies (not parsed
  today, unchanged), and type parameters (`parameter type`).
- **Normative reference** — IEEE 1800-2017, verified against `docs/1800-2017.pdf`:
  **§6.18** (user-defined types; *"the declaration of a user-defined data type
  shall precede any reference to its type_identifier"*; `type_declaration ::=
  typedef data_type type_identifier { variable_dimension } ;` from A.2.1.3),
  **§6.19** (enumerations), **§6.22.1** (matching types — the equivalence
  semantics this pass knowingly does not preserve, §8), **§26.3** (compilation
  units, for the `$unit`-scope typedef).
- **Relationship to prior ADRs**:
  - **ADR-0001** modelled `Typedef : Declaration` (inherited `type` = aliased
    `DataType`, `type == null` = forward typedef with `fwd_kind`,
    `unpacked_dims` for `typedef logic [7:0] mem_t [256]`) and `NamedType`.
  - **ADR-0003 §4.4 / ADR-0006 §4.2** already split the one ambiguous
    `NamedType` case: a bare interface port re-tags to `InterfaceType`. Every
    `NamedType` that survives `NameResolution` therefore denotes a typedef (or
    a dangling name — an error this pass surfaces).
  - **ADR-0004** inlines package typedefs into consuming modules
    (`PackageInliner` copies the `Typedef` item and strips the `::` scope);
    this pass consumes those copies like local ones.
  - **ADR-0008** excluded typedefs from interface member sets; interfaces
    elaborate as pseudo-modules through `ResolveModule`, so this pass covers
    interface-body typedefs with no extra work.

---

## 1. Problem — typedefs are carried opaquely and break flattening

Nothing in the pipeline ever substitutes a `NamedType` use with its `Typedef`
definition. Four defects follow (all reproduced through `veriflat --sv`,
Verilator-verified):

1. **Name collision.** A child's `typedef logic [7:0] t` splices next to the
   top's `typedef logic [3:0] t`: `AnnotateDeclaration` prefixes variables but
   never renames `Typedef` nodes or the `NamedType` references to them —
   *Duplicate declaration of TYPEDEF 't'*.
2. **Use before declaration.** The flattener's port-bind declarations
   (`nib_t u_o;`) are emitted before the child's spliced `typedef` item —
   illegal per §6.18.
3. **Silently wrong netlist.** `Analysis::Dimensions` has no `NamedType`
   knowledge: a typedef-typed actual on an instance array is treated as a
   scalar. `nib_t av` (4 bits) driving `u[3:0]` produces `wire u_i; assign
   u_i = av;` — a 4→1-bit truncation with **every element driven from
   `av[0]`**. Only a downstream width warning hints at it.
4. **Enumerator duplication** (direct enums, same family): an `enum {IDLE,
   RUN} st;` inside a child instantiated twice renders two enum decls both
   declaring `IDLE`/`RUN` in the flat module — *Duplicate declaration of enum
   value*. (`EnumInliner` already replaced the *references* with constants;
   the *declarations* still ride along.)

Plus one front-end gap: a `typedef` at compilation-unit scope is a syntax
error — `definitions` only admits module/package/interface/import, while §26.3
allows any `package_item` between design elements.

The root cause is one and the same: **type sugar survives into a phase that
only understands concrete types**. The fix follows the `EnumInliner`
precedent — resolve the sugar to what it denotes, then drop the declaration.

## 2. Decision 1 — substitute and drop

`TypedefInliner` walks a module (or interface pseudo-module) clone:

1. **Bind.** Build the lexical scope of `Typedef` items with `ScopeTable`
   (which already classifies `Typedef` as `SymbolKind::TYPE`), so nested
   scopes (generate blocks, named blocks) shadow correctly and §6.18
   before-use order is enforced at the binding step.
2. **Substitute.** Every `NamedType` whose name binds to a `Typedef` is
   replaced by a **clone of the aliased `DataType`** — at *every* type
   position: `Var`/net/`Arg`/`Param`/`Member` decl types, `Typedef` chains,
   `TypeCast` targets (`nib_t'(x)`), `TypeOpType` operands, function return
   types. Chains (`typedef a b; typedef b c;`) resolve transitively; a cycle
   (expressible through forward typedefs) is a hard error. A forward typedef
   (`type == null`) is satisfied by its completing definition in scope; a
   forward declaration never completed but referenced is a hard error. A
   `NamedType` that binds to nothing is a hard error (it can no longer be an
   interface name — `NameResolution` already re-tagged those).
3. **Drop.** All `Typedef` items are removed. After the pass the module
   contains no `Typedef` and no `NamedType`.

Substituting a clone (not sharing the node) preserves the no-node-sharing tree
invariant, and keeps parameter-dependent dims per-use: at the chosen placement
(§3) parameters are already inlined, so `typedef logic [W-1:0] word_t` clones
as a constant-folded range.

## 3. Decision 2 — placement: inside `ResolveModule`, after `EnumInliner`

`TypedefInliner` runs in the `ResolveModule` pipeline, immediately after
`EnumInliner`:

```
...
ConstantFolding
EnumElaboration
EnumInliner
TypedefInliner        ← this ADR
ScopeElevator
LoopUnrolling
...
ModuleInstanceNormalizer
```

- **After `ParameterInliner`/`ConstantFolding`**: aliased dims are constant at
  substitution, so `Dimensions` sees fully-resolved ranges — and each module
  *clone* substitutes with its own parameter binding (a `my_pkg #(.W(8))` vs
  `#(.W(16))` problem never arises; same per-instance argument as ADR-0008).
- **After `EnumInliner`**: every enumerator *reference* is already a constant,
  which is what makes the enum-type lowering (§4) a pure type rewrite.
- **Before `ModuleInstanceNormalizer`** (and therefore before the flattener's
  bind and instance-array split): defect 3 disappears because the normalizer
  never sees a `NamedType`.
- Lexical binding (§2) makes generate-scoped typedefs work before
  `GenerateRemoval` runs: each `NamedType` resolves against its nearest
  enclosing scope.

Interfaces need nothing extra: their pseudo-modules elaborate through the same
`ResolveModule` recursion (ADR-0008 §2).

## 4. Decision 3 — enum types lower to their base type

After `EnumInliner`, an `EnumType` declaration is only a *shape*: its items
are dead weight that re-declares enumerators at every splice (defect 4). So:

- a `NamedType` aliasing an `EnumType` substitutes the enum's **base** data
  type (`enum logic [1:0] {…}` → `logic [1:0]`), not the `EnumType` clone;
- a **direct** decl type that is still an `EnumType` (`enum {IDLE, RUN} st;`)
  is likewise rewritten to its base type in the same walk.

An enum with no explicit base defaults to `int` (§6.19); the existing
`EnumElaboration`/`EnumInliner` machinery already normalizes item values, so
the lowering never loses information the netlist needs. After the pass the
flat output is enumerator-free — defect 4 is fixed for typedef'd *and* direct
enums by one rule.

## 5. Decision 4 — unpacked-dimension merge

`typedef data_type type_identifier { variable_dimension }` (A.2.1.3) makes the
alias an **array type**. When such an alias types a declaration that itself
carries unpacked dims, the declaration's dims vary slowest (§6.18 composition):

```systemverilog
typedef logic [7:0] mem_t [256];
mem_t m [4];        // ≡ logic [7:0] m [4][256]
```

Substitution moves the typedef's `unpacked_dims` onto the *declaration*,
appended **after** the declaration's own dims. A typedef-with-dims used in a
position that has no unpacked-dims slot (a `TypeCast` target, a packed
context) is a hard error rather than a silent drop.

## 6. Decision 5 — compilation-unit-scope typedef (grammar)

`definitions` gains a `typedef_decl` alternative (mirroring how `import_decl`
is already spliced at unit scope). The unit-scope typedef joins the `$unit`
pseudo-package that `PackageInliner` already builds (`build_unit_scope`), so
visibility follows §26.3 compilation-unit rules with **no new machinery**: by
the time `TypedefInliner` runs, the unit-scope typedef has been copied into
its consuming modules like any package typedef.

## 7. Ordering relative to the default-resolution pass (ADR-0006 §8)

The future *default-resolution* pass (implicit defaults: `ImplicitType` →
context default, `ImplicitNet` → `` `default_nettype ``, first-port `inout`)
and this pass are **independent**: a typedef alias is never an implicit type
(the grammar requires an explicit `data_type`), and default resolution never
creates a `NamedType`. Neither consumes the other's output. Convention when
both exist: default resolution runs **first** (design-wide, right after
`NameResolution`, where the per-module `default_nettype` context lives), and
`TypedefInliner` stays per-module in `ResolveModule`; but nothing breaks if
the order is swapped. Recorded here so the implicit-default work does not
re-open the question.

## 8. Semantics knowingly changed — type equivalence (§6.22.1)

§6.22.1(b)/(d) make a typedef *matching* with its definition and with itself
across uses, while (c) makes two textually identical **anonymous** types
non-matching. Substitution replaces one typedef'd type with N anonymous
clones — under strict LRM type checking, `AB_t AB1; AB_t AB2;` (matching)
becomes two anonymous structs (non-matching). Veriparse performs **no type
equivalence checking** and emits a flat netlist where only bit widths matter,
so this is recorded as an accepted, deliberate semantic narrowing — the same
family as `EnumInliner` erasing enum identity. If a type checker ever lands,
it must run **before** this pass.

## 9. Not supported (rejected loudly or unchanged)

| Feature | v1 behavior | Future home |
|---|---|---|
| `parameter type` (type parameters, §6.20.3) | not parsed today — unchanged (the `TypeParam` node exists in the schema, like `Module.lifetime` before ADR-0006 §8; only the grammar is missing) | own ADR (interacts with `ParameterInliner`) |
| typedef inside task/function bodies | not parsed today — unchanged | grammar + this pass's scope walk |
| `typedef iface.type_t t;` (interface-based typedef, A.2.1.3 second form) | not parsed today — unchanged | after ADR-0008 v2 items |
| struct/union **transformation** support | substitution is a faithful clone; downstream passes reject/ignore structs exactly as before | own ADR (needs `Dimensions` + flatten support) |

## 10. Errors

| Condition | Clause | Message shape |
|---|---|---|
| `NamedType` binds to nothing | §6.18 | `'X' does not name a type` |
| reference before the typedef's declaration point | §6.18 | `'X' is referenced before its declaration` |
| typedef cycle (`typedef a b; typedef b a;`) | §6.18 | `typedef 'X' is circular` |
| forward typedef never completed but used | §6.18 | `forward typedef 'X' is never defined` |
| typedef-with-unpacked-dims in a dims-less position | §6.18/A.2.1.3 | `array typedef 'X' is not legal here` |

## 11. Pass placement & structure

- `lib/src/passes/transformations/typedef_inliner.{cpp,hpp}` — the pass
  (`TransformationBase`), wired into `ResolveModule` after `EnumInliner`.
- Grammar: `definitions` gains `typedef_decl` (+ `pragmalist` variant if
  needed); `PackageInliner::build_unit_scope` picks it up.
- `docs/passes.md` gains the pass entry; `EnumInliner`'s "skips EnumDef
  subtrees" note gains a pointer here (the preserved typedef declarations are
  now consumed and dropped by this pass).

## 12. Phasing (each lands green) & test plan

1. **Grammar**: unit-scope `typedef` (§6) — parser golden tests
   (`sv_unit_typedef0`), plus a `$unit` visibility test through
   `PackageInliner`.
2. **Core substitution** (§2): module-local typedefs on vars/nets/args/casts;
   chains; shadowing in generate blocks; every defect-1/2 repro from §1 as a
   `ModuleFlattener` golden test (`tdef_collision0`, `tdef_port0`,
   `tdef_pkg0`).
3. **Dimensions correctness** (§3): the defect-3 instance-array repro
   (`tdef_array0`) — golden test asserting per-element `av[k]` connections;
   cosim variant (flat-vs-dut equivalence, Verilator).
4. **Enum lowering** (§4): typedef'd enum and direct enum through a
   twice-instantiated child (`tdef_enum0`, `enum_dup0`) — flat output is
   enumerator-free and Verilator-clean.
5. **Unpacked-dims merge** (§5): `mem_t m [4]` golden test; the dims-less
   position error test.
6. **Error catalogue** (§10): one `TEST_ERROR_SV` per row.
