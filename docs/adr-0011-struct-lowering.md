# ADR-0011 — Packed struct/union lowering (`StructLowering`)

- **Status**: Proposed — design settled; implementation pending.
- **Date**: 2026-07-25
- **Scope**: A transformation pass that **lowers every packed struct/union
  declaration type to its equivalent packed vector** and **rewrites member
  accesses to part-selects**, so no `StructType`/`UnionType` and no
  member-access reference survives into flattening. Explicitly **not** in
  scope: unpacked structs/unions (rejected loudly, §7), tagged unions,
  assignment patterns (`'{...}`, not parsed today), the `type()` operator
  over struct types, and struct-typed subroutine arguments beyond what the
  vector lowering gives for free.
- **Normative reference** — IEEE 1800-2017, verified against
  `docs/1800-2017.pdf`: **§7.2.1** (packed structures; *"the first member
  specified is the most significant and subsequent members follow in
  decreasing significance"*; a packed structure is a vector — any member or
  the whole is usable as an integral value), **§7.3.1** (packed unions: all
  members shall be the same size; the union is a vector of that width),
  **§7.4.1–7.4.2** (packed dimensions/integral equivalence).
- **Relationship to prior ADRs**:
  - **ADR-0009** substitutes struct *typedefs* with faithful anonymous
    clones: after `TypedefInliner`, every struct-typed declaration carries a
    concrete `StructType` in its `type` slot — this pass's input. Enum and
    named-type members are already lowered/substituted there (the ADR-0009
    walk recurses into `StructType.members`, each a `Declaration`).
  - **ADR-0010** reduces type parameters to typedefs, so a struct bound via
    `parameter type` also arrives here as a concrete decl type.
  - **ADR-0008 §4.3**: member access `s.f` is `Identifier{name: f,
    hier: [s]}` — no member-access node. Interface accesses share this shape;
    the rewrite rule (§4) distinguishes them by what the root label resolves
    to, and this pass runs inside `ResolveModule` where interface instances
    are not yet spliced (their accesses resolve later, in the flattener).
  - **ADR-0009 §8**: type-identity erasure (matching vs anonymous types) is
    already an accepted semantic narrowing; lowering structs to vectors is
    the same family.

## 1. Problem — structs parse but cannot flatten

`StructType`/`UnionType` parse and substitute (ADR-0009), but `Dimensions`
does not model them, the flattener cannot splice them, and a member access
`s.hi` reaches `LoopUnrolling`/`ScopeElevator` as an unresolved hierarchical
reference. Any RTL using a packed struct — ubiquitous in bus/protocol IP —
fails to flatten. A packed struct is, by §7.2.1, *already* a vector with
named slices: the lowering is purely mechanical.

## 2. Decision 1 — lower to vectors, rewrite accesses, drop the types

A new `StructLowering` pass runs in `ResolveModule` **after
`TypedefInliner`** (decl types concrete, parameters/constants folded, enums
lowered) and **before `ScopeElevator`/`LoopUnrolling`** (no member access
survives to confuse hierarchical-name handling). Per module/interface clone:

1. **Layout.** For each declaration whose `type` is a packed `StructType`,
   compute the member layout recursively: member width = product of its
   packed dims × base width (atoms per §6.11; nested packed structs
   recurse). First member at the MSBs (§7.2.1). The declaration's type is
   replaced by `logic [W-1:0]` (signed iff the struct is declared `signed`),
   and the layout `{member path → (msb, lsb, signed)}` is bound to the
   declaration's name in a lexical scope table (module body, generate
   blocks, subroutine bodies — the ADR-0009 scope discipline).
2. **Rewrite.** Every `Identifier{name: f, hier: [v, m1, ...]}` whose root
   label `v` binds to a lowered declaration rewrites to a `Partselect` of
   `v` with the constant `[msb:lsb]` of the member path `m1...f`. A root
   label's `loop` index (array-of-structs element, `bank[i].f`) becomes a
   `Pointer` under the part-select, exactly as a hand-written
   `bank[i][7:4]` parses. A path that names no member is a hard error. An
   identifier whose root binds to nothing in the table is left untouched —
   interface accesses and genuine hierarchical references resolve
   downstream, as today.
3. **Drop.** After the pass no `StructType`/`UnionType` remains in the
   module. Whole-struct reads/writes/comparisons need no rewrite at all:
   the declaration *is* a vector now.

A signed member reads unsigned through a part-select (§11.5.1). Where the
member was declared signed, the rewrite wraps the select in
`signed'(...)` in expression (rvalue) position — the same `SigningCast`
lowering ADR-0009 uses for signed typedef casts; lvalue positions keep the
bare select (assignment targets carry no signedness semantics).

## 3. Decision 2 — packed unions share the machinery

A packed `UnionType` lowers identically with every member at offset
`[W-1:0]`: §7.3.1 requires all members the same width, which the pass
checks (differing widths are a hard error, not a truncation). Tagged unions
are out of the synthesizable subset and rejected.

## 4. Decision 3 — struct-typed ports and subroutine args

A port or ANSI arg whose type lowers becomes a plain vector port/arg — the
flattener and `FunctionEvaluation` need nothing new. Member accesses on the
*child* side rewrite inside the child's own `ResolveModule` run; on the
*parent* side, an actual is a whole-vector connection. This mirrors how
ADR-0009 made typedef'd ports concrete before binding.

## 5. Not supported (rejected loudly or unchanged)

| Feature | v1 behavior | Future home |
|---|---|---|
| unpacked `struct`/`union` declaration type | hard error `unpacked struct/union is not synthesizable here` (today: silently mis-handled downstream) | field-splitting pass (per-member variables + access rewrite) |
| `union tagged` | hard error | — |
| assignment patterns `'{...}` | not parsed today — unchanged | grammar + this pass (constant concat lowering) |
| struct literal in a cast (`T'{...}`) | not parsed today — unchanged | with assignment patterns |
| member select on a function *call* result (`f(x).m`) | not parsed today — unchanged | needs expression typing |
| `$bits` over a struct type | whatever `$bits` support exists today; after lowering the operand is a vector | — |

## 6. Errors

| Condition | Clause | Message shape |
|---|---|---|
| unpacked struct/union as a declaration type | §7.2 | `unpacked struct/union is not synthesizable here` |
| packed union members of differing widths | §7.3.1 | `packed union 'v': members have differing widths` |
| member path names no member | §7.2 | `'v' has no member 'f'` |
| member width not constant after folding | §7.2.1 | `struct member 'f': the width is not constant` |
| tagged union | §7.3.2 | `tagged union is not synthesizable` |

## 7. Pass placement & structure

- `lib/src/passes/transformations/struct_lowering.{cpp,hpp}` — the pass
  (`TransformationBase`), wired into `ResolveModule` after `TypedefInliner`,
  before `ScopeElevator`.
- Width computation reuses `Analysis::Dimensions::extract_dimension` for
  packed dims and the §6.11 atom widths (the ADR-0009 cast table).
- The scope table mirrors `TypedefInliner`'s (lexical stack; declaration
  order within a body).

## 8. Phasing (each lands green) & test plan

1. **ADR** (this document).
2. **Layout + decl lowering + access rewrite** — packed structs only:
   goldens for flat member offsets (`s.hi`/`s.lo`), whole-struct assign,
   nested structs, struct-typed ports across flattening, array-of-structs
   element access; error catalogue rows 1/3/4.
3. **Unions + signed members** — packed union goldens, width-mismatch and
   tagged-union errors, `signed'` rvalue wrapping.
4. **Cosim** — a Verilator-checked design routing a packed protocol struct
   through a flattened hierarchy (header fields packed/unpacked at both
   ends), native DUT vs veriflat output.
