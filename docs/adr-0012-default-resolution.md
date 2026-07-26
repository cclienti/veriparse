# ADR-0012 — Implicit-default resolution (`DefaultResolution`)

- **Status**: Implemented (2026-07-26, this branch: `DefaultResolution`
  pass, §3.1 parser relaxation, veriflat wiring; §3.2/§5 stay
  parser-gated and §8 deferred).
- **Date**: 2026-07-25 (amended 2026-07-26: §2.1 parse-mode gating, §2.2
  parameter carve-out, §3.1 parser relaxation scope, §5 parser-gating)
- **Scope**: A design-wide pass that **replaces every deferred implicit
  default with what the standard says it means**: `ImplicitType` → `logic`,
  `ImplicitNet` → the prevailing `` `default_nettype ``, and the §23.2.2.3
  port direction / port kind defaults. After it, a declaration's type, net
  kind and direction are all explicit, so downstream passes stop
  re-deriving them. Explicitly **not** in scope: effective subroutine
  lifetime (ADR-0006 §8's fourth bullet — same family, but gated on a
  parser gap, §8 below), `interconnect` ports (not parsed), and any
  symbol-table work (this pass needs none).
- **Normative reference** — IEEE 1800-2017, verified against
  `docs/1800-2017.pdf`:
  - **§6.8** — *"If a data type is not specified or if only a range and/or
    signing is specified, then the data type is implicitly declared as
    logic."*
  - **§22.8** — `` `default_nettype `` values `wire|tri|tri0|tri1|wand|
    triand|wor|trior|trireg|uwire|none`; absent directive ⇒ `wire`; with
    `none`, *"all nets shall be explicitly declared. If a net is not
    explicitly declared, an error is generated."*
  - **§23.2.2.3** — the port rules quoted in full in §3.
- **Relationship to prior ADRs**:
  - **ADR-0003 §4.1/§4.2** modelled the two deferrals ("implicit is
    explicit": the parser *records the absence*, it does not fabricate a
    `LogicType`) and named this pass as their resolver. This ADR closes
    those two rows.
  - **ADR-0006 §8** scoped the pass and required it to need no symbol
    table. **ADR-0009 §7** already pinned its position: *design-wide,
    right after `NameResolution`*, before the per-module `ResolveModule`
    pipeline. Both are honoured unchanged.
  - **ADR-0002/ADR-0006 §4.2** left an interface port deliberately
    directionless; §4 below keeps that distinction intact.

## 1. Problem — the defaults are re-derived, or not applied at all

Three deferrals from ADR-0003 are still open in the AST, and the code
compensates ad hoc:

- `Analysis::Dimensions::integral_base` already treats `ImplicitType` as a
  1-bit unsigned vector — i.e. *as if* it were `logic` — so widths are
  right by accident of a special case, not because the type says so.
- `ModuleIONormalizer::create_default_net_type_variable` applies
  `` `default_nettype `` — but only for **non-ANSI** body-declared ports
  whose backing declaration is missing. An ANSI `input x` is never
  resolved, and neither is an implicit net introduced anywhere else.
- The §23.2.2.3 **first-port** default is implemented nowhere: the parser
  seeds its inheritance state with `NONE`, so a leading directionless port
  either errors ("missing port direction qualifier") or is emitted with
  `direction == NONE`. `NameResolution` explicitly hands one shape forward
  with a warning (*"the IEEE 1800-2017 23.2.2.3 default is left to a later
  pass"*).

Each consumer therefore has to know the defaults, and any consumer that
forgets silently gets a different answer.

## 2. Decision 1 — resolve types and net kinds, design-wide

`DefaultResolution` runs over each source after `NameResolution`, walking
module and interface bodies (both carry `default_nettype`; an interface
value maps through the existing translation helper):

1. **`ImplicitType` → `logic`** (§6.8), preserving `signing` and
   `packed_dims` verbatim — the replacement is a `LogicType` carrying the
   same two fields, so widths and signedness are unchanged by construction
   (which is what keeps `Dimensions` honest). This applies wherever an
   `ImplicitType` sits: variables, nets, parameters, ports, subroutine
   return types and args.
2. **`ImplicitNet` → the prevailing net type** (§22.8), reusing the
   existing 12-way mapping. With `` `default_nettype none `` in effect the
   net cannot be created: **hard error** (§22.8's own wording), replacing
   today's `LOG_ERROR` + null-return in the normalizer.

Both rewrites are context-local; no symbol table, per ADR-0006 §8.

### 2.1 Parse-mode gating — 1364 in, 1364 out

`logic` is not a 1364-2005 keyword, so materializing it in a plain-Verilog
design would make veriflat's output invalid for strict Verilog consumers.
The pass therefore takes the parse mode:

- **SV mode**: both rewrites run, as above.
- **Verilog mode**: only **`ImplicitNet` → net kind** runs (its result,
  e.g. `input wire [3:0] x`, is legal 1364-2005). `ImplicitType` is left
  in place — `Analysis::Dimensions` already reads it as the 1-bit
  unsigned vector it denotes, and no 1364 spelling of the resolved type
  exists. One `ImplicitType` shape thus stays alive in Verilog mode; that
  is the deliberate price of mode-faithful output.

### 2.2 Parameters — the §6.20.2 carve-out

§6.8's "implicit means logic" does **not** hold for value parameters.
IEEE 1800-2017 §6.20.2: a parameter with **no type and no range** takes
*the type and range of the final value assigned to it* (32-bit-or-wider
logic vector for an unsized integral, `real` for a real expression) — so
rewriting `parameter W = 8` to a dimension-less `logic` would silently
make it 1-bit. Hence:

- `Param` (and localparam) with an `ImplicitType` **carrying packed
  dims** — §6.20.2's "range but no type" case — resolves to a
  `LogicType` with the same signing and dims (the range is fixed by the
  declaration and unaffected by overrides).
- A **range-less** `Param` `ImplicitType` (with or without a signing)
  is left untouched: its type genuinely comes from its final value,
  which is `ParameterInliner`/`ConstantFolding` territory, not a
  syntactic default.

## 3. Decision 2 — the §23.2.2.3 port rules, in full

The older ADRs summarised this as "first-port `inout`". The clause is
richer, and the extra rules are exactly the ones that decide **net vs
variable** — so v1 implements all of it. For the **first** ANSI port:

- direction omitted → `inout`;
- data type omitted → `logic`;
- port kind omitted → a net of the default net type for `input`/`inout`;
  for `output` it depends on **how the data type was written**: omitted or
  *implicit* (signing and/or packed dims only) → net; an **explicit**
  `data_type` → **variable**. A `ref` port is always a variable.

For **subsequent** ports: if direction, kind and type are *all* omitted,
all three are inherited from the previous port; otherwise the direction
alone inherits, the kind follows the rules above, and an omitted type
defaults to `logic`.

The distinction the output hinges on — *explicit* vs *implicit* data type —
is already in the AST: an implicit data type is exactly an `ImplicitType`
node (ADR-0003 §4.1), and the parser records nothing else. The LRM's own
examples are the acceptance criteria:

| Source | §23.2.2.3 result |
|---|---|
| `module mh0 (wire x);` | `inout wire logic x` |
| `module mh3 ([5:0] x);` | `inout wire logic [5:0] x` |
| `module mh8 (output x);` | `output wire logic x` |
| `module mh10(output signed [5:0] x);` | `output wire logic signed [5:0] x` |
| `module mh11(output integer x);` | `output var integer x` |
| `module mh12(ref [5:0] x);` | `ref var logic [5:0] x` *(parser-gated, §3.2)* |
| `module mh14(wire x, y[7:0]);` | both `inout wire logic` |

In the resolved AST, "the port kind is a variable" is expressed by the
declaration **node type** (a `Var` instead of a `Net`), with `is_var`
false — the `var` keyword was not written, and not spelling it keeps the
render legal in both language modes (`output integer x` is its own
resolved form; 1364-2005 has no `var`). The LRM comments above are
semantic, not a rendering contract.

### 3.1 Parser prerequisite — admitting the directionless forms

The mh table is unreachable today: `create_ports_decls` classifies any
leading directionless port as the non-ANSI form and errors with "missing
port direction qualifier", and a *subsequent* directionless port carrying
attributes is rejected the same way (only the fully-bare all-inherit form
was admitted). §23.2.2.3 says otherwise on both counts. This ADR
therefore also relaxes `create_ports_decls`, **in SV mode only** (in
1364-2005, ANSI port declarations always start with a direction, so the
old diagnostics stand in Verilog mode):

- the non-ANSI branch is taken only when the first port is **fully
  bare** (no direction, kind, signing, dims, or type — §23.2.2.3's own
  criterion); otherwise the list is ANSI and a directionless first port
  is emitted with `direction == NONE` for this pass to seed;
- a subsequent port with attributes but no direction inherits **only
  the direction** (its kind and type are its own, per the clause);
  the fully-bare form keeps inheriting all three, unchanged;
- unpacked dimensions are admitted on plain ANSI ports (`wire x,
  y[7:0]`) and land on the declaration's `unpacked_dims` — they are
  never inherited (§23.2.2.3 requires them repeated per port).

Note the parser already implements the *inheritance* half for ANSI ports
(`create_ports_decls` threads `last_dir`/`last_net`/…); this ADR
completes the missing admission cases there and puts the first-port seed
and the kind rules in the pass.

### 3.2 Still parser-gated — `var` and `ref` port kinds

`var` is not accepted in a port list and `ref`/`const ref` is not a port
direction token, so the mh rows needing them (mh4, mh6/7/9 of the LRM's
own numbering, mh12, mh17/18) stay syntax errors. Admitting them is a
grammar feature of its own (and `ref` ports are outside the
synthesizable subset anyway); the kind rules below are written so those
forms slot in when the grammar lands.

## 4. Decision 3 — `direction == NONE` is overloaded; disambiguate by declaration

After `NameResolution`, a `NONE` direction means one of two things:

- an **interface port** — legitimately directionless (A.1.3; `NameResolution`
  deliberately *undoes* the inherited direction for one), or
- an **unresolved** port awaiting this pass.

They are told apart by the declaration, not the direction: an interface
port's decl carries an `InterfaceType` (the existing
`Analysis::Module::get_port_interface_type` answers this). **Interface
ports are left untouched** — direction, kind and type all stay as they
are. Applying an `inout` default to one would contradict ADR-0002.

## 5. Decision 4 — `var` with a defaulted `inout` is an error

The LRM makes `module mh4 (var x);` an error: the direction defaults to
`inout`, and an `inout` cannot be a variable. This is the only case
where applying a default *creates* a diagnosable conflict — but it is
**parser-gated** (§3.2: `var` does not parse in a port list), so no v1
code path can reach it. Recorded here so the check lands together with
the `var` port grammar.

## 6. Interactions to keep honest

- **`is_declared_signal`** (duplicated in `analysis/{module,function,task}.cpp`)
  treats `ImplicitNet{ImplicitType}` as *not* a real declaration — the
  "bare header ref still needing a body declaration" case. Once this pass
  runs, that shape no longer exists and the predicate's verdict flips to
  "declared", which is **correct** post-resolution (an `input x` genuinely
  is a fully declared `input wire logic x`). Because the flip is only
  correct *after* the pass, the pass must not run between an analysis that
  relies on the old reading and its consumer; running design-wide before
  `ResolveModule` satisfies this. The three copies were also a standing
  duplication — consolidated on this branch into the shared predicate in
  `analysis/declaration_helpers.hpp`.
- **Backed non-ANSI direction declarations stay untouched.** A body
  `input [7:0] x;` whose signal is also declared by a standalone body
  `reg [7:0] x;` (or any Var/Net) is a *reference* to that declaration,
  not an implicit-net creation: resolving its placeholder would fabricate
  a second declaration of `x`, and under `` `default_nettype none `` it
  would hard-error a perfectly legal design (§22.8 penalizes nets that
  are "not explicitly declared" — this one is). The pass therefore
  resolves a body direction declaration's `ImplicitNet` only when **no**
  standalone body Var/Net of the same name exists in the module — a
  single name-set sweep over the items, not a symbol table. The backed
  placeholder keeps its `ImplicitNet{ImplicitType}` shape, so
  `is_declared_signal`'s "still needs a body declaration" reading stays
  exact for it.
- **`ModuleIONormalizer`** applies the default net type itself for the
  non-ANSI case. After this pass that branch is unreachable — its
  `needs_default_nettype` predicate can no longer be true. v1 keeps the
  branch (the normalizer must stay correct when run standalone, as the
  tests do) but the ADR records it as dead in the veriflat pipeline; the
  shared mapping moves to one place so the two cannot diverge.
- **Round-trip fidelity** is deliberately given up *by this pass only*:
  `veridump`/`veripp` do not run it, so the parse-only paths keep printing
  the source as written (ADR-0003's reason for deferring in the first
  place).

## 7. Errors

| Condition | Clause | Message shape |
|---|---|---|
| implicit net needed while `` `default_nettype none `` is in effect | §22.8 | `implicit net 'x' but `default_nettype none is in effect` |
| `var` port whose direction defaults to `inout` | §23.2.2.3 | *(parser-gated, §3.2/§5 — lands with the `var` port grammar)* |
| first port has a kind/type but no direction (Verilog mode; SV mode admits it per §3.1) | 1364-2005 §12.3.4 | (diagnosed by the parser — unchanged in Verilog mode) |

## 8. Not in scope — effective subroutine lifetime

ADR-0006 §8 bundles "effective lifetime" into the same family, and it
belongs in this pass eventually. It is deferred here because it has a
**parser prerequisite**: `Module.lifetime` is modelled but not parsed
(`module automatic m;` is a syntax error today, while `package` and
`interface` parse theirs), so "inherit the enclosing default" has nothing
to inherit *from* for modules. Sequence: close the parser gap (small,
standalone — the same three-production nonterminal as
`interface_lifetime`), then extend this pass to tag every `Function`/`Task`
with its effective lifetime so `FunctionEvaluation`/`ModuleFlattener` can
check `AUTOMATIC` explicitly instead of assuming it.

## 9. Pass placement & structure

- `lib/src/passes/transformations/default_resolution.{cpp,hpp}` — the pass
  (`TransformationBase`), constructed with the parse mode (§2.1), with
  `run_design(sources)` mirroring `NameResolution` and a working
  single-source `process()` (the test harnesses drive passes that way).
- Wired into `apps/veriparse/veriflat/src/main.cpp` immediately after
  `NameResolution().run_design(...)` and before `SynthesizableCheck`,
  per ADR-0009 §7.
- The net-kind mapping is shared with `ModuleIONormalizer` rather than
  copied (one enum switch, one home).

## 10. Phasing (each lands green) & test plan

1. **ADR** (this document).
2. **Types and nets**: `ImplicitType` → `logic` (SV mode, §2.1/§2.2),
   `ImplicitNet` → `` `default_nettype `` (both modes), `none` error.
   Goldens for a module with bare vars/nets/params, and per-module
   directive scoping.
3. **Parser relaxation (§3.1)**: the directionless ANSI admissions, with
   parser goldens for the newly-parsing mh forms and tests that the
   Verilog-mode diagnostics are unchanged.
4. **Ports (§23.2.2.3)**: goldens driven by the LRM's `mh0`…`mh16` rows
   that parse (§3.2 lists the gated ones) and the interface-port no-op.
5. **Pipeline**: veriflat wiring; confirm the flattener goldens are
   unchanged (the pass must be width- and semantics-neutral), and that
   `veridump` output still round-trips as written.
