# ADR-0012 — Implicit-default resolution (`DefaultResolution`)

- **Status**: Proposed — design settled; implementation pending.
- **Date**: 2026-07-25
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
| `module mh12(ref [5:0] x);` | `ref var logic [5:0] x` |
| `module mh14(wire x, y[7:0]);` | both `inout wire logic` |

Note the parser already implements the *inheritance* half for ANSI ports
(`create_ports_decls` threads `last_dir`/`last_net`/…); what is missing is
the first-port seed and the kind rules. This pass completes them rather
than re-implementing inheritance.

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
`inout`, and an `inout` cannot be a variable. The pass reports it rather
than emitting an illegal port. This is the only case where applying a
default *creates* a diagnosable conflict.

## 6. Interactions to keep honest

- **`is_declared_signal`** (duplicated in `analysis/{module,function,task}.cpp`)
  treats `ImplicitNet{ImplicitType}` as *not* a real declaration — the
  "bare header ref still needing a body declaration" case. Once this pass
  runs, that shape no longer exists and the predicate's verdict flips to
  "declared", which is **correct** post-resolution (an `input x` genuinely
  is a fully declared `input wire logic x`). Because the flip is only
  correct *after* the pass, the pass must not run between an analysis that
  relies on the old reading and its consumer; running design-wide before
  `ResolveModule` satisfies this. The three copies are also a standing
  duplication (a shared predicate is the tidy follow-up, out of scope here).
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
| `var` port whose direction defaults to `inout` | §23.2.2.3 | `port 'x': a var port cannot be inout (the omitted direction defaults to inout)` |
| first port has a kind/type but no direction and no predecessor to inherit from | §23.2.2.3 | (already diagnosed by the parser — unchanged) |

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
  (`TransformationBase`), with `run_design(sources)` mirroring
  `NameResolution` and a working single-source `process()` (the test
  harnesses drive passes that way).
- Wired into `apps/veriparse/veriflat/src/main.cpp` immediately after
  `NameResolution().run_design(...)` and before `SynthesizableCheck`,
  per ADR-0009 §7.
- The net-kind mapping is shared with `ModuleIONormalizer` rather than
  copied (one enum switch, one home).

## 10. Phasing (each lands green) & test plan

1. **ADR** (this document).
2. **Types and nets**: `ImplicitType` → `logic`, `ImplicitNet` →
   `` `default_nettype ``, `none` error. Goldens for a module with bare
   vars/nets/params, and per-module directive scoping.
3. **Ports (§23.2.2.3)**: goldens driven by the LRM's `mh0`…`mh14` table,
   the interface-port no-op, and the `var`-inout error.
4. **Pipeline**: veriflat wiring; confirm the flattener goldens are
   unchanged (the pass must be width- and semantics-neutral), and that
   `veridump` output still round-trips as written.
