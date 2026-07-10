# ADR-0008 — Interface elaboration (`InterfaceElaboration`)

- **Status**: Accepted — **implemented** (all §10 phases, incl. the ANSI-port
  unpacked-dimension grammar §5 required; dut-vs-flat cosim equivalence green).
- **Date**: 2026-07-09
- **Scope**: The elaboration step that **lowers SystemVerilog interfaces to plain
  synthesizable signals** during module flattening — the feature ADR-0006 §8
  deferred ("flattening a design *through* an interface … own ADR when tackled").
  Covers: scalar interface instances, per-instance parameterization, interface
  header ports, modport-qualified and bare interface ports on modules, interface
  body logic (continuous assigns / always blocks), and **interface arrays**.
  Explicitly **not supported** (§8): nested interface instances, tasks/functions
  in interfaces, generic interface ports, modport expressions, modport direction
  enforcement (visibility only in v1).
- **Normative reference** — IEEE 1800-2017, verified against `docs/1800-2017.pdf`:
  **§25.3** (interface syntax/instantiation; module-in-interface prohibition),
  **§25.3.2** (interface port access: vars `ref`, nets `inout` — reference, not
  copy), **§25.4** (interface header ports), **§25.5** (modports; header vs
  connection selection; both ⇒ identical), **§25.8** (parameterized interfaces),
  **§25.10** (access to interface objects; modport restricts the port-reference
  path), **§23.3.3.4** (interface port connection rules), **§23.3.3.5**
  (arrayed instance connection).
- **Relationship to prior ADRs**:
  - **ADR-0002** provided the nodes (`Interface`, `Modport`/`ModportPort`,
    `InterfaceType`, `InterfaceInstance`) and stated the missing piece: "only
    elaboration/flattening that *connects through* an interface needs new logic".
  - **ADR-0006** provided the tags this pass consumes (`Instance →
    InterfaceInstance`, bare port → `InterfaceType`) and sketched this design in
    its §8: field-by-field replacement, locating the instance behind each
    connection to bind parameter values. §8 also records the `match_scope`
    index-blindness gap that §5 below closes for interface labels.
  - **ADR-0007**: virtual interfaces are already rejected before flattening;
    this pass never sees one.
  - **Implementation order**: step 5 of the SV-support roadmap
    ([ADR-0001 §11](adr-0001-type-declaration-split.md)); depends on steps 3
    and 4.

---

## 1. Decision 1 — flattener-side elaboration, own class

`Passes::Transformations::InterfaceElaboration` is a separate class **invoked
from `ModuleFlattener`** (the organizational pattern of
`ModuleInstanceNormalizer` inside `ResolveModule`): testable on its own, but
running at flatten time, not as a design-wide pre-pass.

**Rejected: standalone pre-flatten pass** (`InterfaceInliner::run_design` after
`NameResolution`). It looks cleaner — erase interfaces design-wide, flattener
stays interface-free — but it must solve two problems the flattener has already
solved:

1. **Per-connection parameterization.** A module with an interface port takes
   its signal widths from the *connected instance*'s parameterization (§25.8):
   `memMod mem8(sb8)` / `memMod mem16(sb16)` need two differently-sized
   expansions of one `memMod`. A pre-pass must clone per-parameterization module
   specializations — duplicating exactly the per-instance cloning the flattener
   does natively.
2. **No-modport port semantics.** An expanded interface port without a modport
   has `ref`/`inout` access (§25.3.2), which the flattener's `bind()` rejects
   (inout unsupported). Flatten-time aliasing (§3) sidesteps ports entirely.

## 2. Decision 2 — interfaces become pseudo-modules

`AST::Interface{name, lifetime, default_nettype; params, ports, items}` is
field-for-field the shape of `AST::Module` (ADR-0002 §2.1 made this deliberate).
At preparation time, each `Interface` definition is **transplanted into a
genuine `AST::Module`** — after its `Modport` items are extracted (validated,
then stripped) and its body instances are rejected (§8) — and merged into the
*effective* `ModulesMap` the flattener consumes.

Everything downstream is then the **existing, untouched instance path**, which
resolves each interface **clone-first, inline-second** — the clone is fully
elaborated against its instantiation context *before* its items enter the
module:

1. **Clone per instance** — the flattener clones the pseudo-module for each
   `InterfaceInstance` (per-instance parameterization is structural, §25.8).
2. **Parametrization from the instantiating module** — the instance's `#(...)`
   actuals were already folded to constants by the *parent's* own
   `ResolveModule` run (parent parameters inline first); instance-value-wins
   binding applies them to the clone.
3. **Full resolution on the clone** — the nested flatten runs the whole
   `ResolveModule` pipeline on it: `ParameterInliner(paramlist_inst)` →
   `LocalparamInliner` → `ConstantFolding` (which evaluates constant
   `function`s via `ExpressionEvaluation`/`FunctionEvaluation`) →
   `EnumElaboration`/`EnumInliner` → `ScopeElevator` → `LoopUnrolling` →
   `BranchSelection` → `GenerateRemoval` → `ConstantFolding` →
   `VariableFolding`. Widths, enum members, generate regions and constant
   functions inside the interface body therefore elaborate exactly as they do
   in a module body — with no interface-specific code.
4. **Only then inline** — `instancename_signal` declaration prefixing,
   header-port `bind()` (§25.4 — an interface's own `input logic clk` binds
   like any module input), instance-tree registration, and item splicing into
   the instantiating module.

Two §25 obligations fall out structurally:

- **body logic is emitted once per interface instance** (the instance is inlined
  once, wherever it is declared), and
- **every user of the instance aliases the same spliced signals** (§3).

**Rejected: generalizing the pass family to accept `Interface` roots.** Every
`ResolveModule` sub-pass and `Analysis::Module` helper exact-matches
`NodeType::Module`; widening them all is a broad ripple for zero benefit.
**Rejected: transplant-and-back.** Flattening *consumes* the interface — nothing
downstream ever needs the `Interface` node again; veriflat renders only the
flattened top module.

One flattener-side caveat: the recursive flatten of a pseudo-module interface
runs with **dead-code elimination disabled** — interface members have no readers
*inside* the interface, so eliminating there would strip the very signals the
connected modules are about to reference. veriflat's final `--deadcode-end`
pass still prunes genuinely unused members once all references are materialized.

## 3. Decision 3 — connection by aliasing, not copies

A member access `bus.field` is `Identifier{name: field, hier: [bus]}` (there is
no member-access node; ADR-0003 §4.3 hier axis). The lowering exploits that
representation end to end:

- **Owning module**: once the `InterfaceInstance` is inlined as a pseudo-module
  (§2), it has an instance-tree entry `(my_if, bus)`, so the flattener's
  *existing* `replace_scoped_identifiers` resolves `bus.field → bus_field` with
  no new code.
- **Connected child** `mod u(.p(bus))`: when the flattener processes instance
  `u` — after the child's recursive flatten and `u_` declaration prefixing,
  **before `bind()`** — `InterfaceElaboration::bind_interface_ports`:
  1. decodes the actual (§7 errors otherwise): bare instance `bus`, an
     enclosing module's own interface port (chaining), connection-modport form
     `bus.mp`, or array element `v[k]`;
  2. checks interface-type identity (§23.3.3.4) and merges modports (§4);
  3. rewrites every child-body identifier whose first hier label is `p` to base
     `bus`, applying member existence + modport visibility per reference; a
     multi-label path `p.a.b` is a hard error (no nested paths in v1);
  4. **drops** the interface `Port` from the child's port list and the
     `PortArg` from the instance — `bind()` must never see them (it clones every
     port declaration unconditionally; an undropped interface `Arg` would leak
     a bogus declaration into the flat output, and no assign is wanted:
     connection is aliasing, per §25.3.2, not a copy).

  The rewritten `bus.field` refs then resolve in the parent exactly as the
  owning module's own refs do. Two modules connected to one instance therefore
  reference the **same** `bus_*` signals — the §25.3.2 semantics.
- **Chained interface ports** (`a` passes its own port down: `.p(p2)`) rewrite
  label `p → p2` and resolve one level up; the recursion terminates at the level
  that owns the `InterfaceInstance`.

Ordering guarantees (why the rewrite is sound): interface port names are `Arg`
declarations, which neither `AnnotateDeclaration` nor `AnnotateScope` renames,
so the label `p` is still intact after prefixing; rewriting *after* prefixing
means a stale label fails loudly (no match) instead of silently binding wrong.
References stay symbolic (`bus.field`) until `replace_scoped_identifiers` runs
last, so sibling ordering inside a module is irrelevant.

### 3.1 Hier-safety hardening (prerequisite fixes)

Two existing utilities rewrite identifiers **by leaf name with no `hier`
guard**, which becomes a live corruption path once hier-carrying member refs
flow through flattening:

- `AnnotateDeclaration::annotate_names`: a child local named `field` being
  prefixed to `u_field` would also rename the leaf of `p.field`;
- `ASTReplace::replace_identifier`: `ParameterInliner` inlining child parameter
  `DATA` would substitute into `p.DATA`.

Both gain a skip-when-`hier`-set guard (recursion into children preserved —
`HierLabel.loop` index expressions must still be renamed); the
constant/variable-folding passes are audited for the same pattern. Recorded
here as closing part of the ADR-0006 §8 gap list.

## 4. Decision 4 — modports v1: visibility only

- The modport named by a port type (`my_if.mp p`, header) or a connection
  (`.p(bus.mp)`) must **exist** on the interface — checked here for the first
  time anywhere (ADR-0006 only checked `Modport` placement).
- Header and connection both naming one ⇒ the names must be **identical**
  (§25.5); none ⇒ all members accessible; one ⇒ that modport's member list is
  the **visible set**, and access through the port to an unlisted member is a
  hard error (§25.10 restricts precisely the port-reference path).
- Every `ModportPort` name must be a member the interface declares (§25.5:
  "shall be declared by the same interface"; a modport "shall not implicitly
  declare new ports") — validated at `prepare()`.

**Deferred: direction enforcement** (writing a member the modport declares
`input`). The rewrite walk (§3 step 3) visits every access with lvalue context
recoverable, so v2 adds the check in place — named future home:
`InterfaceElaboration::bind_interface_ports`. Until then a direction violation
lowers to a multiple-driver conflict that Verilator/synthesis reports.

## 5. Decision 5 — interface arrays

- **Instance arrays** `my_if v[3:0]()`: `ModuleInstanceNormalizer::split_array`
  already splits arrays of pseudo-module instances into scalars `v0…v3` — free
  once §2 puts interfaces in the map.
- **Connection** (§23.3.3.5): an arrayed child `u[3:0](.p(v))` distributes
  element-wise — the normalizer wraps the bare interface actual per element
  (`.p(v[k])` for element `k`, reusing the array-split index formula); an
  already-indexed actual passes through. v1 restriction: formal/actual ranges
  must be **identical** (checked per index; a missing scalar is a
  range-mismatch error).
- **Arrayed interface ports** `my_if p[3:0]` on a module: a reference
  `p[i].field` carries the index in `HierLabel.loop`; after unroll/folding the
  loop is constant and the label folds to `v<idx>`. **Grammar note
  (implemented here)**: no ANSI port form carried `{unpacked_dimension}`
  before this ADR (ADR-0006 §4.2 had assumed the node model implied parse
  support) — the `portname` productions (bare-typed, package-scoped and
  modport-qualified) gained a dims variant per A.1.3 `ansi_port_declaration`,
  carried on the `Arg`; other ANSI port forms and task/function arguments
  reject the dims loudly rather than dropping them.
- **Index-aware `match_scope`**: the flattener's hier matching is name-only
  today (ADR-0006 §8 known gap). The label key becomes
  `name + decimal(eval(loop))` via `ExpressionEvaluation`; a non-constant index
  no longer matches (existing keep-hierarchical behavior). This fixes plain
  instance arrays (`u[2].sig`) as a standalone bugfix, not just interfaces.
- A hierarchical reference used as an interface actual must not resolve through
  an arrayed instance (§25.3) — hard error.

## 6. What the pass produces

For a module `m` with `my_if #(.W(8)) bus(.clk(clk));`, children connected via
`.p(bus)` / `.p(bus.mp)`, after full flattening:

- one set of `bus_*` declarations at `W=8` widths (typedef'd/enum members
  resolved by the pseudo-module's own `ResolveModule` run);
- the interface's body logic once, prefixed `bus_*`;
- `assign bus_clk = clk;` from header-port `bind()`;
- every `bus.field` / child `p.field` reference rewritten to `bus_field`;
- no `Interface` definitions, no `InterfaceInstance`, no `InterfaceType` ports
  anywhere in the output.

The **top module** must not itself have interface ports (nothing exists above
to own the instance) — hard error, and the veriflat `--top-module` may not name
an interface.

## 7. Error catalogue (all hard errors, located and clause-cited)

| Condition | Clause |
|---|---|
| Modport name not declared by the interface (header or connection) | §25.5 |
| Header and connection modports differ | §25.5 |
| `ModportPort` names a non-member | §25.5 |
| Access through modport-qualified port to unlisted member | §25.10 |
| Access to a member the interface does not declare (incl. external `p.method()` — subroutines are not members, §8) | §25.10 |
| Interface port left unconnected / null actual | §23.3.3.4 |
| Actual's interface type ≠ formal's named interface type | §23.3.3.4 |
| Module instantiated inside an interface body | §25.3 |
| Nested interface instance (interface in interface) | §8 deferral |
| Top module has interface ports / top names an interface | — |
| Array range mismatch (formal vs actual), non-constant index on an interface label | §23.3.3.5 |
| Hierarchical interface actual through arrayed instance | §25.3 |
| Multi-label member path `p.a.b` | §8 deferral |

## 8. Not supported (v1 deferrals — rejected loudly, never silently mis-lowered)

| Feature | Clause | v1 behavior | Future home |
|---|---|---|---|
| **Nested interface instances** | §25.3 | **hard error** at `prepare()`: "nested interfaces not supported" | fast-follow, falls out of the recursion (see below) |
| **External** access to interface subroutines (`p.method()`, `bus.method()`; modport import/export) | §25.7 | **internal** `function`/`task` declarations in the body are *allowed* — they ride the module-body machinery (constant ones fold away via `FunctionEvaluation`; survivors splice legally as `bus_fname`); only *external* access through a port or instance is rejected (a subroutine name is not in the member set — §3 rewrite errors "not a member"); modport import/export entries not parsed (ADR-0002 §7) | member-set entry + call-label rewrite in `bind_interface_ports` |
| Generic interface port (`module m(interface i)`) | §25.3.3 | not parsed (ADR-0002 §7) | resolve at connection, then §3 |
| Modport expressions (`.a(expr)`) | §25.5.4 | not parsed (ADR-0002 §7) | substitution in the §3 rewrite |
| Modport direction enforcement | §25.5 | visibility only (§4) | v2 check in `bind_interface_ports` |
| Non-identical array ranges in connection | §23.3.3.5 | range-mismatch error | index remapping |
| Virtual interfaces | §25.9 | already rejected pre-flatten (ADR-0007) | — |

### 8.1 Nested interfaces — deferred as a fast-follow, not a redesign

The pseudo-module recursion (§2) is precisely what makes nesting tractable; it
is deferred for scope, not difficulty. The cost decomposes as:

1. **Owner-side access** (`link.rx.data` where `link_if` instantiates
   `channel_if rx()`) — nearly free: a nested instance inside a pseudo-module
   body is inlined by the same recursion, its subtree splices into the instance
   tree, and `replace_scoped_identifiers`/`match_scope` already walk
   multi-label paths (`u1.u2.sig` works today). Parameter chaining
   (`channel_if #(W)` referencing the outer interface's param) folds before the
   nested bind. Changes: `prepare()` admits `InterfaceInstance` body items
   (still rejecting module instances, §25.3) and threads the deadcode-off flag.
2. **Child-port rewrite** (`p.tx.valid` through a bare interface port) —
   moderate: rewrite only the *first* hier label, keep the tail; the flat
   member set becomes a structural table (members + nested-instance map) and
   validation walks the labels.
3. **Nested interface as an actual** (`mod u(.c(p.tx))`) — the trickiest:
   actual decode for a one-label sub-interface path, and chaining that
   substitutes one label with two.

Modports need **no** extra work: §25.5 requires modport entries to be names
declared by the same interface, so a nested member can never be modport-listed
in our subset (that would need modport expressions, §25.5.4) — the existing
"not in modport" error is already the correct behavior for nested paths through
a modport-qualified port.

## 9. Pass placement & structure

- **Class**: `Passes::Transformations::InterfaceElaboration`, files
  `interface_elaboration.{hpp,cpp}` beside `module_flattener.*`. Holds a
  `Design` prepared once: pseudo-modules, per-interface modport sets,
  per-interface member-name sets (names are parameter-independent).
- **`Analysis::Module`** gains `InterfacesMap` +
  `get_interface_dictionary()` (mirror of the module dictionary; a name
  colliding between a module and an interface is a hard error, §3.13).
- **`ModuleFlattener`** ctor gains a defaulted `const InterfacesMap&`
  (existing callers unchanged); merges pseudo-modules into its map; threads the
  shared `Design` through its per-instance recursion; calls
  `bind_interface_ports` between declaration prefixing and `bind()`; disables
  dead-code elimination for the interface recursion (§2).
- **veriflat** builds the interface dictionary alongside the module map and
  passes it in; `veridump`/`veriobf` are untouched (they neither flatten nor
  need lowering).

## 10. Phasing (each lands green) & test plan

1. **Hier-safety groundwork** (§3.1 guards + §5 index-aware `match_scope`) —
   standalone bugfix value; unit tests + a flattener golden with `u[2].sig`
   refs into a split instance array.
2. **Prep + interface instances** — dictionary, `prepare()`, pseudo-module
   flatten of owning-module member refs. Goldens: scalar instance, header
   port + body always, two parameterizations of one interface, body-assign
   emitted once per instance. Error tests: module-in-interface, nested
   interface, top-with-interface-port, unknown interface.
3. **Interface ports on children** — the §3 rewrite. Goldens: bare port with
   two modules sharing one bus (proves aliasing: a single `bus_*` set),
   header-modport, connection-modport, chained ports, child output driving a
   member. Error tests: the §7 modport/connection rows.
4. **Arrays** (§5). Goldens: instance array + direct indexed refs, arrayed
   child element-wise, arrayed interface port. Error tests: the §7 array rows.
5. **End-to-end + docs** — a cosim design (parameterized valid/ready handshake
   interface, modports, body logic; producer/consumer/top; dut-vs-flat
   equivalence via the shared Verilator harness; plus an array variant);
   `docs/passes.md` updated (this pass, and the missing
   PackageInliner/NameResolution/SynthesizableCheck entries); status flip here
   and in the ADR-0006 §8 gap list.

Unit tests use an SV-mode variant of the `test_module_flattener` macro
(pattern of `TEST_CORE_SV`/`TEST_ERROR_SV` in `test_name_resolution.cpp`);
goldens stay `refs/module_flattener_<name>.yaml`.
