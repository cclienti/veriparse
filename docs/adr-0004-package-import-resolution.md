# ADR-0004 — Package / import resolution (`PackageInliner`)

- **Status**: Accepted — implemented (grammar + `PackageInliner`, incl. re-export
  and the §9 refinements); the broad name-resolution engine of §6 remains future work.
- **Date**: 2026-06-16
- **Scope**: The semantic pass that **resolves SystemVerilog `package` / `import`**
  and produces self-contained output — `PackageInliner`. Covers: explicit
  (`import pkg::sym;`) and wildcard (`import pkg::*;`) imports, scoped references
  (`pkg::T`, `pkg::f()`, `$unit::x`), the §26 search/precedence rules, and stripping
  `package`/`import` from the output. **Deliberately narrow** (per the scope
  decision): this is *not* a general symbol-binding engine. It is the first client
  of a `ScopeTable` seam (§6) that a future broad name-resolution pass will
  generalize to drive the other deferrals in ADR-0003.
- **Normative reference** — IEEE 1800-2017, verified against `docs/1800-2017.pdf`:
  §26 (Packages), esp. **§26.3** (referencing data in packages / compilation
  order), **§26.4–26.6** (import, precedence, search), §3.12 (compilation-order
  exceptions), §6.18 (user-defined type must precede use).
- **Relationship to prior ADRs**:
  - **ADR-0001** gives the node model this pass consumes: scoped refs are
    `Identifier`/`NamedType` carrying **`scope: list(ScopeName)`** (the flat
    `package` string and `ScopedRef`/`CustomVariable`/`rettype_ref` are **gone**).
  - **ADR-0003** catalogues the parser deferrals this pass resolves — explicit
    `pkg::T` (§4.3) and the new unqualified-via-wildcard entry (§4.5).
  - This ADR is the **sole, current record** of the `package`/`import` design.
    (Earlier scratch notes — which predated ADR-0001 and named retired nodes —
    have been removed.) The SV preprocessor is a separate, already-implemented
    workstream (`lib/src/parser/preprocessor/`), unrelated to this pass.
  - **Implementation order**: step 2 of the SV-support roadmap (introduces the
    `ScopeTable` seam that step 4 generalizes) — see
    [ADR-0001 §11](adr-0001-type-declaration-split.md).

---

## 1. Why a pass, not text substitution

`import` is **not** `` `include ``-style text substitution: it resolves a named
symbol inside parsed `package … endpackage` syntax via *scoped name lookup*. So it
lives in the AST, as a transformation pass with the same shape as the existing
`ParameterInliner` / `FunctionEvaluation` / `ModuleFlattener`. The parser already
emits the raw material (PR A, done): `Package{name, items}`, `Import{package,
symbol}` (`symbol == "*"` ⇒ wildcard), and every scoped reference left
**unresolved** as an `Identifier`/`NamedType` carrying a non-empty `scope`.

---

## 2. Decisions (carried from the design notes, re-affirmed)

1. **Wildcard imports are lazy; explicit imports are eager** (revised — the
   original "wildcard is eager" decision was reversed). Per §26.5/§26.6 a wildcard
   name is *actually imported* only when **referenced**, whereas an explicit
   `import pkg::x;` is actually imported regardless of use. So `import pkg::*;`
   copies only the names the scope references (on use, in `rewrite_refs`), while
   `import pkg::x;` is materialized unconditionally. The original eager-wildcard
   choice (justified as "simpler") proved both *less* correct (it over-imported,
   over-exported, and broke §26.5 precedence — fixed) and not simpler: lazy is the
   standard's own model and `rewrite_refs` already resolves references, so it falls
   out naturally. Bonus: no reliance on `DeadcodeElimination` to remove unused
   imports, and far less output bloat.
2. **Packages share the input list.** Package files are passed on the command line
   alongside module files — no `--package` flag. Cross-file resolution happens in
   the pass.
3. **Synthesizable subset only.** Resolvable package items: `parameter`,
   `localparam`, `typedef`, `enum`, constant `function`, `task`. Out of scope:
   `class`, `covergroup`, `sequence`, `property`, virtual interfaces.
4. **Re-export — implemented** (reverses the original "rejected" decision).
   `export pkg::sym;` / `export pkg::*;` / `export *::*;` (§26.6) parse to a new
   `Export` AST node; `fold_exports()` adds the re-exported names — taken from the
   package's `contents` (what it actually imported) — into its interface
   `symbols`, in declaration order so chained re-exports see the full interface;
   the export nodes are then stripped. An explicit `export pkg::name` of a name
   the package never imported is a hard error. **Known deviation** (§9.5):
   importing both a package and its re-exporter (the same original declaration)
   currently false-flags ambiguity, where §26.6 says it should not — origin-dedup
   is a follow-up. Cycles remain forbidden (§26).
5. **Output is stripped.** Flattened output contains no `package`/`endpackage` and
   no `import` — the same way modules are flattened today.

---

## 3. Ordering & search order (normative — drives the design)

"Declared before use" is a **hard requirement** here, and that ordering *is* the
lookup order:

- **Packages** — §26.3: *"The compilation of a package shall precede the
  compilation of scopes in which the package is imported."* §3.12 lists this as one
  of only **two** exceptions to compilation-order-insignificance. A reference can
  never point at a not-yet-seen package.
- **Types** — §6.18: a user-defined type must be defined before use; the forward
  `typedef` form is for `class` handle coupling, not the synthesizable subset. So
  `typedef base_t alias_t;` always has `base_t` earlier; alias chains resolve in
  declaration order.

**Consequence: no forward references, no cycles** → the pass is a simple
**collect-then-resolve** (§5). A reference whose target is absent from the table is
a **hard error** ("undefined package/type"), which also catches the illegal
forward-reference case.

### Precedence / shadowing (§26.4–26.6)

When resolving an **unqualified** name in a scope that has imports:
1. a **local** declaration shadows any import;
2. an **explicit** import (`import pkg::x`) shadows a **wildcard** (`import pkg::*`);
3. two wildcards offering the *same* name is **not** an error until the name is
   **actually referenced** (§26.6) — only then is the ambiguity a hard error.

This precedence is the lookup order of the per-scope `ScopeTable` (§6).

---

## 4. What the pass resolves (post-ADR-0001 node model)

| Source | Parsed as (unresolved) | After the pass |
|---|---|---|
| `pkg::C` (value) | `Identifier{name:C, scope:[pkg]}` | the package's `localparam`/`param` value (inlined) |
| `pkg::T` (type) | `NamedType{name:T, scope:[pkg]}` | resolved type (typedef/enum/struct), copied in |
| `pkg::f(...)` | `FunctionCall{scope:[pkg]}` | the package's `function`, copied in |
| `pkg::t(...)` | `TaskCall{scope:[pkg]}` | the package's `task`, copied in |
| `$unit::x` | `…{scope:[$unit]}` | compilation-unit declaration |
| unqualified `x` under `import pkg::*` | `Identifier{name:x, scope:[]}` | bound to `pkg::x` by precedence (§3) — **ADR-0003 §4.5** |

`Call`/`FunctionCall`/`TaskCall` inherit `Identifier`, so they all carry `scope`
uniformly — one resolution path covers values, types, and calls.

---

## 5. Algorithm — collect-then-resolve

1. **Collect.** One sweep over all inputs builds the symbol tables: package name →
   its synthesizable items; plus the `$unit` and per-file scopes. (No forward refs,
   so one sweep suffices.)
2. **Resolve.** A second sweep, per importing scope:
   - build that scope's `ScopeTable` (locals + explicit imports + wildcard sets,
     in the §3 precedence order);
   - rewrite each unresolved `scope`-bearing ref (and each unqualified name bound by
     a wildcard) to point at — and copy in — the target declaration;
   - eagerly copy wildcard-imported items into the scope.
3. **Strip.** Delete all `Import` and `Package` nodes.
4. `DeadcodeElimination` (existing) prunes unused copied declarations.

**Sequencing**: after parse, **before** `ParameterInliner` / `ModuleFlattener`.
A named type that crosses a package boundary needs resolving before the type/decl
consumers run — so package type resolution either runs here or cooperates with the
typedef-resolution step (§7), sharing the same `ScopeTable`.

---

## 6. The reusable seam (why "narrow" is not "throwaway")

The one piece worth factoring out is **scope construction + name lookup**, as a
`ScopeTable` component:

- `ScopeTable` = an ordered set of bindings (name → declaration) for one scope,
  built from locals + imports in §3 precedence order, with a `lookup(name)` that
  returns the binding or flags an on-use ambiguity.
- `PackageInliner` is its **first and only client today** — it builds a table per
  importing scope and resolves against it.

**Future (broad name resolution).** ADR-0003's other deferrals — `Call →
Function/TaskCall`, `NamedType → InterfaceType`, `TypeCast → SizeCast`,
`TypeOpExpr → TypeOpType` — all need the *same* "what does this name bind to?"
lookup, just over more scope kinds (module / generate / interface / function-locals)
and asking the binding's *kind* rather than copying it. That future pass
**generalizes `ScopeTable`** to all scope kinds and becomes the engine those
resolutions consume. Designing the seam now (not the engine) keeps this ADR narrow
while avoiding a rebuild later. *Explicitly out of scope here.*

---

## 7. Relationship to typedef resolution

Named-type resolution (`NamedType → concrete DataType`, the typedef/enum/struct a
name aliases) is a sibling concern: it consumes the **same `ScopeTable`** and obeys
the same §6.18 ordering. It may run as its own step (a `TypedefResolution`) or be
merged into the collect-then-resolve here — but a package that exports a typedef
used by a module means the two must share tables and the package step cannot run
strictly after a module-only typedef step. Treated as a cooperating pass, detailed
when its grammar/pass lands.

---

## 8. Out of scope (future ADRs / sessions)

- The **broad name-resolution engine** that generalizes `ScopeTable` (§6) and drives
  the remaining ADR-0003 deferrals.
- Non-synthesizable package members (`class`, `covergroup`, …).
- Hierarchical (`.`) path resolution (`hier` axis) — elaboration concern, unrelated
  to `::`.

---

## 9. Compilation units, package-body resolution, and dependencies (implemented)

Refinements made while implementing the pass; they supersede the single-global-sweep
sketch in §5 where they differ.

### 9.1 Per-unit collect/resolve in compilation order
A `package` name is **global** across the design (§26.2), but `import`, `$unit` and
macros are scoped to **one compilation unit** (one source file). So the pass splits:
- `collect(source)` indexes a source's packages into the shared registry
  (accumulates across sources);
- `resolve(source)` resolves that source's scopes against the shared packages plus
  *that source's own* top-level imports / `$unit`.

`run_units(sources)` drives them **in command-line order**, collect-then-resolve per
unit before the next — so a unit may reference a package from an **earlier** unit but
not a **later** one (§26.3). A qualified reference to an unknown package (undeclared,
or whose unit is compiled later) is a **hard error**. The veriflat app preprocesses
and parses **each file in isolation** (one file = one unit; macros do not flow across
files), then calls `run_units`.

### 9.2 Package bodies are resolved as scopes
A package body is structurally a scope like a module body (items + its own imports),
so the **same** scope-resolution machinery runs on packages too — *before* modules,
in dependency order. This makes a package that **imports and uses** another
self-contained:

```
package P1; localparam A = 5; endpackage
package P2; import P1::*; localparam B = A * 2; endpackage
//  P2 after resolution:  localparam A = 5; localparam B = A * 2;   (A pulled in)
```

Without this, a module that copies `P2::B` would inherit a dangling `A` — resolving
only module scopes is **not** sufficient (the `A`→`P1::A` context lives in P2's
import, which module resolution never sees).

### 9.3 Transitive intra-package dependency copy
Copying a single package symbol must also copy the same-package symbols it
**transitively depends on**. A qualified `P::data_t` where `typedef logic[W-1:0]
data_t` must pull `W` too, or `[W-1:0]` dangles. (Wildcard import already pulls
everything via the eager copy; this closes the explicit/qualified single-symbol
case.)

### 9.4 Interface vs contents
Once §9.2/§9.3 pull dependencies **into** a package, two sets diverge and must be
kept distinct:
- **interface** — what `import P::*` / `P::x` exposes = P's own declarations
  **plus re-exports** (§2.4);
- **contents** — everything physically in P, including dependencies pulled in for
  P's own internal use.

`export` writes to the *interface*; the transitive copy reads the *contents*. Without
the split, P would over-export every dependency it imported.

### 9.5 Re-export same-declaration origin (origin-dedup — implemented)
§26.6: *"import of a declaration by way of multiple exported paths does not cause
conflicts"* — the same original declaration reached via two paths is not a
conflict. Keying ambiguity by *source-package name* would wrongly reject a scope
that imports both a package and its re-exporter (e.g. `import P1::*; import P2::*;`
where P2 re-exports P1's `x`). So each interface symbol records its **defining
package** (`PackageEntry.origin`; carried across a re-export fold, transitively for
chained re-exports), and `ScopeTable::lookup` treats multiple wildcard paths as a
conflict only when their defining packages differ. Same origin → resolves; truly
different declarations (`Pa::x` vs `Pb::x`) → still ambiguous on use.

(A related deviation — re-exporting the *full* wildcard set rather than only the
referenced names — was fixed by making wildcard imports lazy (§2.1): a package now
imports, and therefore re-exports, only the names it references.)

### 9.6 Enum members are package symbols (implemented)

An enum declared in a package binds its **enumerators** in the package scope
(§6.19), so `import P::*` + a bare `S_A`, and the qualified `P::S_A`, must both
resolve — previously the symbol index recorded only each item's *own* name, so
the wildcard form silently left `S_A` dangling in the output and the qualified
form hard-errored ("no synthesizable symbol"). Symbol indexing now enumerates
**every name an item binds** via the shared `ScopeTable::for_each_bound_name`
(the ADR-0006 classifier seam): an enumerator maps to its **containing
declaration**, which stays the unit of copy — referencing `S_A` copies the whole
typedef. On copy, *all* of the copied item's bound names are registered in the
scope's dedup map, so a later reference to the typedef itself or to a sibling
enumerator does not clone the declaration twice. The same enumeration also
registers genvars and enumerators as scope locals, so they now shadow imports
per §26.4 like any other local declaration.
