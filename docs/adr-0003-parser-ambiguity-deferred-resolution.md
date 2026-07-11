# ADR-0003 — Parser ambiguity & deferred semantic resolution (neutral nodes / base-class fallbacks)

- **Status**: **Closed** (2026-07-11) — the inventory is resolved. Every Family-A
  neutral node is re-typed by `NameResolution` (ADR-0006) and every deferred
  reference is bound by `PackageInliner` (ADR-0004) or at flatten time
  (ADR-0006 §8 / ADR-0008), except the two **implicit defaults** (§4.1/§4.2),
  which remain deliberately unresolved in the AST and are handed off to the
  future *default-resolution* pass (ADR-0006 §8). The catalogue is frozen: a
  feature ADR that introduces a new deferred resolution documents it in its own
  text rather than appending here.
- **Date**: 2026-06-16
- **Scope**: Every place where the **grammar** cannot pick the *exact* AST node
  because the choice is **not a syntactic property** — task vs function, size vs
  named type, type vs expression, implicit defaults, unresolved scope. The parser
  emits a **neutral / base node** (or a deliberately under-committed subclass) and
  a **later semantic pass** refines it. This ADR is the catalogue of those points,
  so the boundary "syntactic now / semantic later" is recorded in one place and
  not re-litigated per construct.
- **Normative reference** — same as ADR-0001, per parse mode:
  - SystemVerilog mode → **IEEE 1800-2017** (`docs/1800-2017.pdf`);
  - Verilog mode → **IEEE 1364-2005** (`docs/verilog-std-1364-2005.pdf`).
- **Relationship to ADR-0001** — ADR-0001 built the `DataType` / `Declaration`
  hierarchies. Several of those hierarchies exist *specifically* so the parser can
  emit a base when it cannot disambiguate (`Identifier → Call → {FunctionCall,
  TaskCall}`; `ImplicitType`/`ImplicitNet` as first-class "no keyword written"
  nodes). This ADR documents how the parser *uses* that headroom.
- **Living document (until closure)** — feature ADRs appended entries here as
  they introduced new deferred resolutions. **ADR-0002 (interfaces)** added §3.4
  (`Instance` → `InterfaceInstance`) and §4.4 (`NamedType` → `InterfaceType`);
  **ADR-0004 (package/import)** added §4.5 (unqualified name under a wildcard
  import). With the closure (see Status) no further entries are appended.
- **Implementation order** — this catalogue was resolved by steps 2–4 of the
  SV-support roadmap (the broad name-resolution pass is step 4); see
  [ADR-0001 §11](adr-0001-type-declaration-split.md).

---

## 1. Principle

> **The parser commits to what the grammar can prove. Everything that needs a
> symbol table, a type, a constant fold, or hierarchy elaboration is left to a
> semantic pass — represented honestly as a neutral node, never guessed.**

Two failure modes this avoids:

1. **Fabricating precision** — emitting `TaskCall` for `f();` when `f` might be a
   function, or fabricating a `LogicType` where the source wrote no type keyword.
   A wrong-but-specific node is harder to fix downstream than an honest neutral
   one (cf. ADR-0001 §1: the empty-named `Logic` "h bug").
2. **Inventing a parse-time symbol table** — the grammar has no scope/type
   knowledge and must not grow one.

So the AST carries a small, explicit vocabulary of "**not yet resolved**" states.

---

## 2. Two families of unresolved node

- **Family A — base-class / neutral-node fallback.** Two distinct constructs share
  one syntax; the parser emits the **base** (or the only legal-in-this-position
  subclass) and a pass re-types it. *Catalogue: §3.*
- **Family B — deferred reference / default.** The parser knows the node *kind* but
  not what it *refers to* or *defaults to* (a name's package, a hierarchical path,
  an implicit type/nettype). The node is correct; a field is left for a pass to
  consume. *Catalogue: §4.*

---

## 3. Family A — base-class / neutral-node fallbacks

### 3.1 Subroutine call — `Call` → `{FunctionCall, TaskCall}`

- **Ambiguous syntax**: `f(args)` / `f()`. The SV BNF `tf_call` is task-**or**-
  function; §13.4.1 even lets a function be **called as a statement**
  (`void'(f())`, or bare with a warning). Task-vs-function is **not** recoverable
  from syntax — only from the declared kind of `f`.
- **Model** (`verilog_ast.yaml:122-131`): `Call : Identifier { args }` is the
  **neutral** node; `FunctionCall`/`TaskCall` inherit it and add nothing — they are
  pure *tags*. Rendering is position-driven, so all three round-trip as
  `name(args)` identically.
- **Intended discrimination**:
  - call in **expression position** (or under a void cast) → `FunctionCall`
    (necessarily a function — a task cannot be an operand);
  - bare **statement** call → neutral `Call` (unresolved);
  - **known-task** form a symbol-aware pass produces → `TaskCall`.
- **Parser reality (current, a known gap — see §6)**: expression position emits
  `FunctionCall` (`verilog_parser.yy:5044-5071`); **statement** position emits
  `TaskCall` via the `task_call` rule (`:5326-5366`, used at `:5455`/`:5468`). The
  neutral `Call` base is **defined in the schema but not yet emitted** by the
  grammar.
- **Resolved by**: `NameResolution` (ADR-0006 §4.1) re-types `Call` to the
  callee's declared kind. *Done* (the parser gap above is also closed — §6).

### 3.2 Cast `IDENT'(x)` — `TypeCast` covers an unresolvable size

- **Ambiguous syntax**: `WIDTH'(x)`. With an **identifier** before the tick, a
  size cast (`WIDTH` a constant ⇒ `SizeCast`) is **indistinguishable** from a
  named-type cast (`WIDTH` a type ⇒ `TypeCast{ NamedType }`).
- **Parser rule** (`verilog_parser.yy:3107-3123`):
  - `intnumber '(...)` → `SizeCast{ size }` (a literal constant — unambiguous);
  - `TK_IDENTIFIER '(...)` → `TypeCast{ target: NamedType(IDENT) }`, with the
    comment: *"`WIDTH'(x)` with an identifier is indistinguishable from a
    named-type cast and stays a TypeCast, resolved later."*
- **Model**: `Cast → {TypeCast, SizeCast, SigningCast, ConstCast}`, node per form
  (`verilog_ast.yaml:507-526`). `TypeCast` is the **conservative** choice (a named
  type), reinterpreted as `SizeCast` if the name proves to be a constant.
- **Resolved by**: `NameResolution` (ADR-0006 §4.4) classifies the leading
  identifier (constant vs type) and rewrites `TypeCast{NamedType}` → `SizeCast`
  when it is a value. *Done.*

### 3.3 `type()` operator — `TypeOpExpr` covers an unresolvable type name

- **Ambiguous syntax**: `type(my_t)` (SV §6.23). The operand may be an
  **expression** (`type(x+1)`) or a **type name** (`type(my_t)`); a bare
  identifier reads as either.
- **Parser rule** (`verilog_parser.yy:1911-1927`):
  - `type( expression )` → `TypeOpExpr{ expr }` — *"a bare `type(my_t)` reads as
    an expression too → TypeOpExpr (the named-type case is resolved later, like
    Call/NamedType)."*
  - `type( data_type )` → `TypeOpType{ type }` — only when the operand is a
    **keyword-led** data type (`logic`/`int`/`struct`/…), which *is* syntactic.
- **Model**: `TypeOpExpr` / `TypeOpType`, both `DataType` (`verilog_ast.yaml:269-277`).
  `TypeOpExpr` is the neutral catch-all.
- **Resolved by**: `NameResolution` (ADR-0006 §4.5) rewrites
  `TypeOpExpr{Identifier}` → `TypeOpType{NamedType}` when the identifier is a
  type. *Done.*

### 3.4 Instantiation — `Instance` → `InterfaceInstance` (ADR-0002)

- **Ambiguous syntax**: `my_if u(...)`. IEEE §25.3 gives an interface instance the
  **same** `module_instantiation` syntax as a module (and a program). Module vs
  interface vs program is the *declared kind* of the target name — not syntactic;
  the definition may even be in another file or an imported package.
- **Parser**: emits the neutral `Instancelist`/`Instance`, target held as the
  `module` string (`verilog_ast.yaml:751,758`) — unchanged from today.
- **Model**: `InterfaceInstance : Instance`, a pure tag (like `FunctionCall`/
  `TaskCall` over `Call`); rendering is identical, so the rewrite round-trips.
- **Resolved by**: `NameResolution` (ADR-0006 §4.2) re-tags `Instance` →
  `InterfaceInstance` when `module` resolves to an interface. *Done* — and
  consumed downstream by `InterfaceElaboration` (ADR-0008).

---

## 4. Family B — deferred reference / default

### 4.1 No type keyword — `ImplicitType`

- **Source**: `var a`, `wire [3:0] w`, `parameter N = 10` — signing/packed-dims
  may be present, but **no type keyword** is written.
- **Model**: `ImplicitType : DataType` (`verilog_ast.yaml:203`). ADR-0001's
  "implicit is explicit": the parser **records the absence**, it does not fabricate
  a `LogicType`.
- **Resolved by**: context-dependent default (IEEE 1800-2017 §6.8 — variable
  defaults, parameter inherited defaults). *Open — handed off to the future
  default-resolution pass (ADR-0006 §8); deliberately unresolved in the AST to
  preserve round-trip fidelity.*

### 4.2 No net keyword — `ImplicitNet`

- **Source**: a net introduced with no net-type keyword.
- **Model**: `ImplicitNet : Net` (`verilog_ast.yaml:382`).
- **Resolved by**: the prevailing `` `default_nettype `` directive (defaults to
  `wire`). *Open — same hand-off as §4.1 (default-resolution pass,
  ADR-0006 §8).*

### 4.3 Unresolved scope / hierarchy — `NamedType` & `Identifier` carry `scope` / `hier`

- **Source**: `pkg::T`, `$unit::T`, `A::B::T` (SV `::` scope); `top.u1.sig`,
  `genblk[i].sig` (Verilog `.` hierarchy).
- **Model** (`verilog_ast.yaml:107-118`, `214-219`): the **same `::` axis** for
  types (`NamedType.scope`) and values (`Identifier.scope`); the `.` hierarchy is
  the orthogonal value-only `hier` field. The node kind is correct — the parser
  just cannot say what the name *binds to* (`scoped_ref` →
  `Identifier{ scope }`, `verilog_parser.yy:2927-2935`).
- **Resolved by**: `PackageInliner` (ADR-0004) consumes `scope` — *done*;
  `hier` resolves at flatten time
  (`ModuleFlattener::replace_scoped_identifiers`, index-aware since ADR-0008)
  — *done*, with the residual gaps recorded in ADR-0006 §8 (no leaf
  existence-check; top-relative matching only).

### 4.4 Bare interface port — `NamedType` covers `InterfaceType` (ADR-0002)

- **Source**: `module m(my_if i)`. A port whose type is a bare identifier is
  **indistinguishable** from a named-type (`typedef`) port `module m(my_type i)` —
  §25.4 vs a user type are told apart only by what `my_if` binds to.
- **Parser**: the port `decl` gets `type: NamedType{my_if}` — the conservative
  choice (same as a normal named-type port; no new grammar path).
- **Model**: `InterfaceType : DataType` (ADR-0002 §2.3). Note the **decisive**
  cases bypass this deferral and emit `InterfaceType` directly at parse time: a
  `.modport` suffix (`my_if.mp i`) or the `virtual` keyword (`virtual my_if v`)
  are interface-only syntax (ADR-0002 §4, Decision 4).
- **Resolved by**: `NameResolution` (ADR-0006 §4.2) promotes `NamedType{X}` →
  `InterfaceType{X}` when `X` resolves to an interface. *Done.*

### 4.5 Unqualified name under a wildcard import (ADR-0004)

- **Source**: a bare `x` in a scope with `import pkg::*;`. Where §4.3 covers an
  *explicitly* scoped `pkg::T`, here the reference carries **no scope** — `x` is
  parsed as `Identifier{name:x, scope:[]}`, syntactically identical to a
  locally-declared `x`.
- **Why not syntactic**: whether `x` is local or imported from `pkg` (or ambiguous
  between two wildcards) depends on what is in scope — IEEE §26.4–26.6 precedence,
  not grammar. The parser cannot attribute the package.
- **Resolved by**: `PackageInliner` (ADR-0004) binds the unqualified name by the §26
  precedence order (local shadows explicit-import shadows wildcard; multi-wildcard
  collision is an error only *on use*). *Done.*

---

## 5. Explicitly **not** a split — bracketed indexing

For completeness, one case that *looks* like it belongs here but does not:
`a[i]` (bit-select vs array-index) and `a[m:l]` / `a[b+:w]` / `a[b-:w]`
(`verilog_parser.yy` `pointer` / `partselect` rules → `Pointer` / `Partselect` /
`PartselectPlus/MinusIndexed`, `verilog_ast.yaml:535-557`).

These are distinguished **syntactically** (the colon / `+:` / `-:` decides the
node), so the parser commits with no neutral base. The remaining bit-select-vs-
array-index distinction is a property of the operand's *type*, **not** represented
as separate AST nodes — `Pointer` is the single home for `[expr]`, and no pass
re-types it. Listed here so it is not mistaken for an open ambiguity.

---

## 6. Known gap (statement calls) — CLOSED by ADR-0006

The schema's design intent (§3.1) — a **bare statement call is the neutral
`Call`** — is now the grammar's behavior: every `task_call` production builds
`Call`, and `NameResolution` (ADR-0006 §4.1) promotes it to
`TaskCall`/`FunctionCall` from the callee's declared kind. Kept for the record:
before the fix the grammar fabricated `TaskCall` in statement position, so a
statement call to a function was silently mis-tagged.

---

## 7. Summary

| # | Ambiguous source | Neutral / base node emitted | Resolves to | Why not syntactic | Resolving pass (status) | Refs |
|---|---|---|---|---|---|---|
| 3.1 | `f(args)` call | `Call` | `FunctionCall` / `TaskCall` | task-vs-fn = declared kind, §13.4.1 | `NameResolution` *(done, ADR-0006 §4.1)* | `yaml:122-131` |
| 3.2 | `WIDTH'(x)` | `TypeCast{ NamedType }` | `SizeCast` | const-vs-type of `WIDTH` | `NameResolution` *(done, ADR-0006 §4.4)* | `yaml:507-521` |
| 3.3 | `type(my_t)` | `TypeOpExpr` | `TypeOpType` | type-vs-expr of operand, §6.23 | `NameResolution` *(done, ADR-0006 §4.5)* | `yaml:269-277` |
| 3.4 | `my_if u(...)` instance | `Instance` (neutral) | `InterfaceInstance` | module-vs-iface = declared kind, §25.3 | `NameResolution` *(done, ADR-0006 §4.2)* | ADR-0002 §2.4 |
| 4.1 | `var a` (no type kw) | `ImplicitType` | concrete `DataType` | context default, §6.8 | default resolution *(open — handed off, ADR-0006 §8)* | `yaml:203`, `yy:975-993` |
| 4.2 | net, no net kw | `ImplicitNet` | concrete `Net` | `` `default_nettype `` | default resolution *(open — handed off, ADR-0006 §8)* | `yaml:382` |
| 4.3 | `pkg::T`, `top.u1.sig` | `NamedType`/`Identifier` + `scope`/`hier` | bound type / signal | needs symbol table / elaboration | `::` `PackageInliner` *(done)*; `.` flatten-time (ADR-0006 §8) | `yaml:107-118,214-219` |
| 4.4 | `module m(my_if i)` port | `NamedType{my_if}` | `InterfaceType` | iface-vs-typedef = declared kind, §25.4 | `NameResolution` *(done, ADR-0006 §4.2)* | ADR-0002 §2.3 |
| 4.5 | unqualified `x` under `import pkg::*` | `Identifier{scope:[]}` | bound `pkg::x` | local-vs-import precedence, §26.4–26.6 | `PackageInliner` *(done)* | ADR-0004 |
| 5 | `a[i]` / `a[m:l]` | `Pointer` / `Partselect` *(syntactic, no split)* | — | bit-vs-array is type-only, not an AST split | — | `yaml:535-557` |

> **Principle restated**: the parser's job is to record *syntax faithfully*,
> including the syntactic fact "**this is ambiguous / implicit / unresolved**" as a
> first-class node. Re-typing those nodes is the job of the semantic passes —
> now built (`NameResolution`, `PackageInliner`, flatten-time hier resolution).
> The to-do list this ADR once was is closed; only the two implicit-default rows
> (4.1/4.2) remain, by design, for the future default-resolution pass
> (ADR-0006 §8). The **principle itself stays normative** for any new grammar:
> commit to what syntax proves, defer the rest as an honest neutral node.
