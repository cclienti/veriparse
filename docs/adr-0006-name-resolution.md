# ADR-0006 — Broad name-resolution pass (`NameResolution`)

- **Status**: Accepted — **implemented** (all §7 phases; the §8 items stay
  deferred with their named future homes).
- **Date**: 2026-07-04
- **Scope**: The symbol-aware pass that resolves the parser deferrals catalogued in
  [ADR-0003](adr-0003-parser-ambiguity-deferred-resolution.md) by generalizing the
  `ScopeTable` seam of [ADR-0004 §6](adr-0004-package-import-resolution.md):
  `Call → {FunctionCall, TaskCall}` (with the ADR-0003 §6 statement-call grammar
  fix), `Instance → InterfaceInstance`, `NamedType → InterfaceType`,
  `TypeCast{NamedType} → SizeCast`, `TypeOpExpr → TypeOpType`, plus the §25.5
  modport-placement check deferred by [ADR-0002 §5.1](adr-0002-systemverilog-interfaces.md).
- **Normative reference** — same as ADR-0001, per parse mode:
  - SystemVerilog mode → **IEEE 1800-2017** (`docs/1800-2017.pdf`);
  - Verilog mode → **IEEE 1364-2005** (`docs/verilog-std-1364-2005.pdf`).
  Clauses that drive decisions here: **§13.4.1** (functions callable as
  statements), **§23.2.2.3** (port direction defaulting/inheritance), **§25.3–25.5,
  §25.9** (interfaces/modports), **§26.4–26.6** (import visibility — already
  resolved upstream by `PackageInliner`).
- **Relationship to prior ADRs**:
  - **ADR-0003** is the input contract: its §7 table lists what stays unresolved
    after parsing. This pass resolves rows 3.1–3.4 and 4.4; it does **not** touch
    rows 4.1/4.2 (implicit type/net defaults — see §8) or 4.3/4.5 (the `::` axis —
    already consumed by `PackageInliner`).
  - **ADR-0004 §6** promised the `ScopeTable` generalization implemented here:
    "asks the binding's *kind* rather than copying it".
  - **ADR-0002 §5** specifies the interface-resolution duties (instance re-tag,
    bare-port promotion); §5.1 defers the modport-placement check to this pass;
    §3.1 defers the directed-port-that-is-an-interface rewrite to this pass.
  - **Implementation order**: step 4 of the SV-support roadmap — see
    [ADR-0001 §11](adr-0001-type-declaration-split.md). Both prerequisites
    (steps 2 and 3) are done.

---

## 1. Principle

> The parser recorded, honestly, what syntax could not decide (ADR-0003 §1). This
> pass owns the *other half* of that contract: one symbol-aware sweep that asks,
> for each deferred node, **what does this name declare?** — and re-tags the node
> to its precise form. It never guesses: a name it cannot classify leaves the
> neutral node in place (with a diagnostic), exactly as conservative as the parser
> that emitted it.

Two structural choices follow:

1. **One pass, one walk.** All the deferrals need the same two ingredients — a
   *design-level* index (module vs interface definitions) and a *lexical* scope
   stack (what a name binds to here). Building them once and applying every
   re-tag in a single sweep avoids N passes × N walks and keeps the resolution
   order self-consistent.
2. **Run after `PackageInliner`.** Downstream of step 2 every module/interface is
   self-contained: imports are materialized as local declarations and `::` scopes
   are gone. `NameResolution` therefore needs **no package logic at all** — only
   locals and design-level definitions. (Run standalone on un-inlined input it
   still works; imported names simply stay unresolved-with-diagnostic.)

---

## 2. `SymbolKind` — the classifier the seam was waiting for

`ScopeTable::Binding` (scope_table.hpp) gains a **kind**:

```cpp
enum class SymbolKind {
    UNKNOWN,    // not a symbol-declaring node — callers treat as unbound
    VALUE,      // Var, Net, Param (value), Arg, Genvar, EnumItem — a name with a value
    TYPE,       // Typedef, TypeParam — a name that denotes a data type
    FUNCTION,   // Function declaration
    TASK,       // Task declaration
    MODULE,     // design-level Module definition
    INTERFACE,  // design-level Interface definition
};
```

Classification is **syntactic on the declaration node** (`node_kind(decl)`): the
declaration's node type fully determines the kind — no inference, no
type-checking. `EnumItem` names count as `VALUE` (they are constants in scope,
§6.19). The existing `Binding` fields (`decl`, `package`, `origin`,
`defining_package`) are unchanged; `PackageInliner` keeps compiling with kind
recorded but unused (it copies declarations; it never needed the kind).

---

## 3. The two tables

### 3.1 Design index (global, one per run)

A `map<string, DesignEntry{kind: MODULE|INTERFACE, node}>` built from every
`Description.definitions` across all sources — the same shape as the existing
`Analysis::Module::get_module_dictionary()`, extended to interfaces. Names are
global (§3.13); a duplicate definition name is a hard error.

### 3.2 Lexical scope stack (innermost-out lookup)

A stack of `ScopeTable`s pushed/popped during the walk. Scopes and what they
contribute:

| Scope | Contributes |
|---|---|
| Module / Interface body | ports (their inner decl names), params, body `Declaration`s, `Function`/`Task` names, `Genvar`s, `EnumItem`s of anonymous enums |
| Function / Task | args (`Arg`), return name (the function's own name, legacy form), local decls |
| Named / unnamed `Block`, generate blocks | local decls, genvars |

Lookup is nearest-scope-first. The table for a scope is built from **all** its
items before its bodies are walked — which deliberately does **not** diagnose
the §6.18 type-before-use ordering rule (a user-defined type must be defined
before it is used — the same reading ADR-0004 §3 applies to packages). An
out-of-order typedef reference therefore *resolves* here; flagging it is a
validation concern for the future checker family (§8), not a resolution
concern.

---

## 4. Resolutions performed (the ADR-0003 rows, made concrete)

### 4.1 Statement calls — grammar emits neutral `Call`; pass re-tags (ADR-0003 §3.1 + §6)

**Grammar change (part of this step):** the statement-position `task_call` rule
(`verilog_parser.yy:5585`, used at `:5756`) stops fabricating `TaskCall` and
builds the **neutral `Call`** the schema always intended — in **all six**
productions (bare `t;`, `t();`, `t(args);`, and their three
`package_scope`-prefixed forms), not just the unscoped ones. Expression position keeps emitting
`FunctionCall` (a task cannot be an operand — still decisive).

**Pass re-tag** for a statement `Call` — skipped if `hier` **or `scope`** is
set. `hier`: hierarchical calls are not modelled (ADR-0001 §3.3.1). `scope`: a
`pkg::t();` only reaches this pass un-inlined (standalone run, §1); looking up
its *bare* name in the lexical stack could bind an unrelated local and re-tag
the call against the wrong symbol — so scoped calls are left neutral, exactly
like every other name this pass cannot own:

| `name` binds to | Result |
|---|---|
| `TASK` | `TaskCall` |
| `FUNCTION` | `FunctionCall` (legal as a statement, §13.4.1 — plus a warning diagnostic, as the standard suggests when the value is discarded) |
| anything else / unbound | left as neutral `Call` + warning (never a guess) |

**Ripple:** conservative call handling must match the **`Call` base category**
(`is_node_category(NodeType::Call)`) so it covers `Call`/`TaskCall`/
`FunctionCall` uniformly — and most of it **already does**: `VariableFolding`
(give-up trigger + never-fold-a-task-actual, ADR-0005 §3.1.1),
`ModuleObfuscator` and `AnnotateDeclaration` all match the category today;
`DeadcodeElimination` has no call-specific matching at all (it tracks uses via
the `Identifier` category, which `Call` inherits). The remaining true ripple is
**`Analysis::Module::get_taskcall_nodes` / `get_taskcall_names`**
(`module.cpp:238–250`): they exact-match `NodeType::TaskCall` and would return
empty for every statement call after the grammar change — re-point them at the
`Call` category (and audit their callers and `test_taskcall`). Golden churn:
every parser/pass golden holding a statement-position `TaskCall` regenerates to
`Call` — mechanical, expected (rendering is identical, so no `.v` output
changes).

### 4.2 Interface re-tags (ADR-0002 §5, §3.1)

**Precedence rule:** the lexical scope stack is consulted **first**; only a
name the stack does not bind is checked against the design index. A local
`typedef X` legally shadows a design-level `interface X` for a type reference,
so "is `X` an interface?" is never asked before "what does `X` bind to here?".
(The design index is not a lexical binding — it is the fallback namespace of
design elements.)

- `Instance`/`Instancelist` whose `module` names an `INTERFACE` →
  **`InterfaceInstance`** (pure tag; fields cloned as-is; rendering identical).
  **Ripple — exact-type matchers must not miss the subclass:**
  `AnnotateDeclaration` (`annotate_declaration.cpp:73,148`) and
  `UniqueDeclaration` (`unique_declaration.cpp:53`) test
  `is_node_type(Instance)`, and `Analysis::Module::get_instance_nodes`
  (`module.cpp:435`) exact-matches — all three must match the `Instance`
  **category** (or explicitly include `InterfaceInstance`), else a flattened /
  obfuscated module skips renaming an interface instance's declaration while
  its references are renamed. Note the tag is a stepping stone: interface
  *elaboration* (§8) will eventually replace the instance by its individual
  fields — this pass only guarantees the instance is *identifiable*.
- Port `decl: Arg{ type: NamedType{X} }` (the deferred bare form) where `X`
  resolves to an `INTERFACE` → `Arg.type` becomes **`InterfaceType{X}`** (no
  modport, not virtual — those were decisive at parse time, ADR-0002 §4). Only
  the type child changes; the `Port` wrapper, `Arg` name and any
  `unpacked_dims` (an interface **array** port, `my_if p [3:0]`) survive
  untouched. `packed_dims` on that `NamedType` → hard error (an interface type
  takes no packed dims).
- Directed port `decl: ImplicitNet{ type: NamedType{X} }` where `X` resolves to
  an `INTERFACE` (the ADR-0002 §3.1 inherited-direction case) → rewritten to
  the interface-port form
  `Port{ direction NONE, decl: Arg{ type: InterfaceType{X} } }`, carrying the
  name and `unpacked_dims` onto the `Arg` — the §23.2.2.3 inheritance applied
  to what turned out to be an interface port is undone, since an interface port
  admits no direction (A.1.3: `interface_port_header` has no `port_direction`).
- `Var{ type: NamedType{X} }` / `Typedef` target / `Arg` in tf where `X`
  resolves to an `INTERFACE` → **hard error**: an interface as a data type
  requires `virtual` (§25.9); a non-virtual interface variable is not a
  declaration but an instantiation, which the grammar already parses separately.
- Bare port `Port{ decl: Arg{ type: NamedType{X} } }` where `X` binds to a
  `TYPE` (typedef) — the other side of the ambiguity: the port is a **typed net
  port** whose omitted direction §23.2.2.3 resolves by default/inheritance —
  which this pass, like the parser, does **not** fabricate (future
  default-resolution pass, cf. §8). Rewritten to
  `Port{ direction NONE, decl: ImplicitNet{ type: NamedType{X} } }` — the
  `Port` wrapper, name and `unpacked_dims` survive — plus a diagnostic noting
  the still-unresolved direction default.

### 4.3 Modport placement check (ADR-0002 §5.1)

A `Modport` whose enclosing definition is not an `Interface` → **hard error**
(§25.5). Pure validation; no rewrite.

### 4.4 Cast disambiguation (ADR-0003 §3.2)

`TypeCast{ target: NamedType{N}, expr }` with unscoped, undimensioned `N`:

| `N` binds to | Result |
|---|---|
| `VALUE` (param/localparam/genvar/enum item) | **`SizeCast{ size: Identifier{N}, expr }`** |
| `TYPE` | stays `TypeCast` (now known-correct) |
| unbound | left + warning |

### 4.5 `type()` operand (ADR-0003 §3.3)

`TypeOpExpr{ expr: <bare Identifier N> }`: `N` binds to `TYPE` →
**`TypeOpType{ type: NamedType{N} }`**; binds to `VALUE` → stays `TypeOpExpr`
(known-correct); unbound → left + warning. Any non-bare-identifier operand is
untouched (already an expression).

---

## 5. Pass placement & structure

- **Class**: `Passes::Transformations::NameResolution : TransformationBase`,
  files `name_resolution.{hpp,cpp}` beside `package_inliner.*`. Entry:
  `run_design(const std::vector<AST::Node::Ptr> &sources)` (builds the design
  index across sources, then walks each) — mirroring
  `PackageInliner::run_units`; the single-node `process()` resolves against
  whatever index one source provides.
- **veriflat pipeline** (main.cpp): `PackageInliner::run_units` →
  **`NameResolution::run_design`** → module dictionary → `ModuleFlattener` → … .
  Re-tagging before flattening keeps the flattener consuming precise nodes (and
  is where a future interface *elaboration* would slot).
- **`ScopeTable`**: already lives in its own translation unit
  (`scope_table.{hpp,cpp}`); it gains `SymbolKind kind` on `Binding` and a
  `classify(decl)` helper. `PackageInliner` behaviour unchanged (the kind is
  recorded, unused there).

---

## 6. Error / diagnostic policy

The pass distinguishes **illegal** from **unresolvable**:

- *Illegal* (standard violation, certain): interface used as a non-virtual data
  type; packed dims on an interface port type; modport outside an interface;
  duplicate design-level name → **hard error** (pass returns non-zero).
- *Unresolvable* (name unbound — e.g. running without `PackageInliner`, or a
  black-box reference): **leave the neutral node, emit a warning**. Neutral nodes
  are already handled conservatively downstream (ADR-0005 §3.1.1 gives up on
  them), so an unresolved name degrades gracefully, never silently wrongly.

---

## 7. Phasing (each lands green)

1. **`ScopeTable` generalization** — `SymbolKind` on `Binding` + `classify()`;
   `PackageInliner` untouched behaviourally. Unit tests on the table itself.
2. **Statement-call neutralization** — grammar emits `Call` (all five
   productions); the `Analysis::Module` taskcall helpers re-pointed at the
   `Call` category (most passes already match it, §4.1); goldens regenerate
   (TaskCall → Call in yamls, `.v` output identical).
3. **`NameResolution` core** — design index + scope stack + `Call` re-tagging
   (§4.1). Pass unit tests (testcases + refs, per lib/test/passes conventions).
4. **Interface re-tags + modport check** (§4.2/§4.3). Resolves the two ADR-0003
   entries added by ADR-0002.
5. **Cast / type() disambiguation** (§4.4/§4.5).
6. **veriflat integration** — pipeline slot + an end-to-end cosim case with an
   interface-using design (as far as flattening supports it today).

ADR-0003's §7 table statuses flip to "resolved by `NameResolution`" as each
phase lands, and the ADR-0001 §11 step-4 row is re-pointed at this ADR when it
is accepted.

---

## 8. Out of scope (kept deferred, with their future homes)

- **Implicit defaults** (ADR-0003 §4.1/§4.2: `ImplicitType`→`logic`,
  `ImplicitNet`→`` `default_nettype ``, §23.2.2.3 first-port `inout`) — a
  context-local *default-resolution* pass; needs no symbol table, and baking
  defaults early would violate the round-trip fidelity the AST guarantees today.
- **Effective subroutine lifetime** (§13.3.1/§13.4.2) — same default-resolution
  family: a `Function`/`Task` with `lifetime: NONE` inherits the enclosing
  `module`/`interface`/`package` default (itself `NONE` ⇒ **static**). The AST
  already round-trips all of these tri-state (that is why `Interface.lifetime`
  exists); nothing *resolves* them yet. This matters before evaluation /
  flattening ever relies on lifetime semantics: a **static** subroutine's locals
  persist across calls and are shared, so inlining or folding one as if its
  locals were fresh is unsound in general. (`FunctionEvaluation` is safe today
  only because a *constant* function is evaluated as elaboration does — fresh
  frame per call, §13.4.3.) The future pass tags every subroutine with its
  effective lifetime (e.g. resolving `NONE` to the inherited value) so
  `FunctionEvaluation`/`ModuleFlattener` can check `AUTOMATIC` explicitly
  instead of assuming it. **Known parser gap to close first** (small, standalone
  fix): the module header lifetime is modelled (`Module.lifetime`) but not
  parsed — `moduledef` has no lifetime slot, so `module automatic m;`
  (A.1.2: `module_keyword [ lifetime ] module_identifier`, §23.2.1) is a syntax
  error today and `Module.lifetime` is always `NONE`, while `package` and
  `interface` already parse theirs. Same three-production nonterminal as
  `interface_lifetime`; a prerequisite for resolving inherited lifetimes.
- **Hierarchical (`.`) resolution** (`hier` axis, `u1.t()` calls, `disable`
  targets). Partially exists today, and stays where it is:
  `ModuleFlattener::replace_scoped_identifiers` resolves **downward
  instance-path references at flatten time** — it matches an identifier's
  `hier` label list against the instance tree built while inlining and rewrites
  `u1.u2.sig` to the flattened prefixed name (`u1_u2_sig`); a path into a kept
  (not-flattened) instance fails the match and correctly stays hierarchical in
  the output. That mechanism is inherently coupled to the instance tree, which
  only exists **during** flattening — so it cannot move into this pass, which
  runs before the flattener on the still-folded hierarchy. `NameResolution`
  leaves `hier`-carrying nodes untouched (cf. §4.1: a call with `hier` set is
  skipped). Known gaps of the flatten-time mechanism, recorded here as
  deferrals, not silent behavior: the leaf signal name is appended without
  existence-checking; label matching is name-only, ignoring a `HierLabel`
  index (`u[2].sig` into instance arrays / generate loops); matching is
  top-relative only (a hierarchical ref written *inside* a child module, and
  upward/`$root` references, are unsupported). Fixing those is
  `ModuleFlattener` work, out of scope here.
- **Interface elaboration** (flattening a design *through* an interface) — a
  `ModuleFlattener`-side feature that consumes this pass's tags; own ADR when
  tackled. Its shape is already known and non-trivial: the flattener will
  replace an interface **field by field** — each signal the interface declares
  becomes a flattened net/var, and every `port.field` access is rewritten to
  it. Doing that requires **locating the interface instance** behind each
  connection to bind its parameter values (a `my_if #(.W(16)) u` and a
  `my_if #(.W(8)) v` produce different field widths from the same definition)
  — which is exactly why this pass tags `InterfaceInstance` and preserves its
  `parameterlist` untouched.
- **Expression-position `FunctionCall` validation** (name actually a function?)
  — diagnostics-only concern for a later validation pass (ADR-0005 §3.6 family).
