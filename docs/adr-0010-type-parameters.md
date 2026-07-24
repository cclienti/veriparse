# ADR-0010 — Type parameters (`parameter type`) lower to typedefs

- **Status**: Accepted — implemented (grammar, `TypeParamInliner`, parent-side
  identifier-actual resolution, error catalogue).
- **Date**: 2026-07-23
- **Scope**: Parse `parameter type T = data_type` in module/interface
  parameter ports and `localparam type` in bodies, accept type actuals at
  instantiation, and **reduce every bound type parameter to a `Typedef`**
  spliced at the head of the body — so `TypedefInliner` (ADR-0009) performs
  all substitution and no `TypeParam` survives into flattening. Explicitly
  **not** in scope: the `type()` operator (§6.23), `$bits`/type query system
  functions, `parameter type` on classes, and type-parameter *comparison*
  semantics (§6.22 matching — same caveat as ADR-0009 §8).
- **Normative reference** — IEEE 1800-2017, verified against
  `docs/1800-2017.pdf`: **§6.20.3** (type parameters; *"a type parameter that
  is not overridden shall have a default data type"* — an unoverridden type
  parameter without a default is an error), **§23.10** (overriding module
  parameters; type assignments in `parameter_value_assignment`, A.4.1.1
  `param_expression ::= mintypmax_expression | data_type | $`), **§3.13 /
  §6.18** (one declaration namespace per scope: a name cannot denote both a
  value and a type — the disambiguation rule §4 relies on).
- **Relationship to prior ADRs**:
  - **ADR-0001** modelled `TypeParam : Declaration` (inherited `type` = the
    default `DataType`, null when no default; `is_local` for
    `localparam type`). The node exists; only the grammar and the passes are
    missing.
  - **ADR-0009** gives the landing zone: once a `TypeParam` is bound to a
    concrete type, it is *exactly* a typedef — collision handling, chained
    aliases, array-typedef dims, enum lowering and cast lowering all come for
    free by reducing to `Typedef` before `TypedefInliner` runs.
  - **ADR-0004** package resolution rewrites scoped *value* references in
    instantiation actuals (`.T(pkg::word_t)` parses as a scoped `Identifier`);
    after `PackageInliner` the copied typedef is local, so the parent-side
    rule of §4 applies unchanged.

## 1. Problem — parameterized IP is type-generic

The toolkit inlines typedefs (ADR-0009) but rejects `parameter type`, so any
IP of the form

```systemverilog
module fifo #(parameter type T = logic [7:0], parameter DEPTH = 16)
  (input T din, output T dout, ...);
endmodule

fifo #(.T(logic [15:0]), .DEPTH(32)) u0 (...);
```

fails at parse. Everything downstream (substitution, flattening, width
resolution) already exists for typedefs; the missing 10% is binding the
actual to the formal.

## 2. Decision 1 — reduce to `Typedef`, reuse ADR-0009 wholesale

A new `TypeParamInliner` pass runs inside `ResolveModule`, **before
`ParameterInliner`**. For each `TypeParam` in the module's parameter ports:

1. **Bind.** The bound type is the instantiation actual when one targets the
   formal (by name or position, from the same `ParamArg` list
   `ParameterInliner` consumes), else the declared default. An overriding
   actual that is not a data type is an error; an unoverridden `TypeParam`
   with no default is an error (§6.20.3). A `localparam type` (`is_local`)
   accepts no override — same rule value `localparam`s already follow.
2. **Reduce.** The `TypeParam` is removed from the header (or body) and a
   `Typedef` with the same name and the bound type is spliced **after the
   leading typedef run at the body head** — a formal's default may reference
   a package/unit typedef `PackageInliner` spliced there, and
   `TypedefInliner` registers in declaration order (§6.18); header ports and
   parameters see the result either way through ADR-0009 §2's
   whole-module-scope refinement. A body `localparam type` reduces in place,
   keeping its §6.18 position. (Implementation notes: `ModuleIONormalizer`
   hoists body *overridable* type params to the header first, exactly as it
   does value params — §6.20.1; positional actuals bind across value and
   type formals interleaved, so the instance normalizer and the flattener
   consult the combined `get_parameter_decl_nodes` list.)
3. **Drop.** After the pass no `TypeParam` remains; `TypedefInliner`
   (running later, unchanged) substitutes every use.

`ParameterInliner` iterates `Param` nodes and never sees `TypeParam`
(distinct node types), so ordering is a safety property, not a code change:
type params must be gone before value-param expressions referencing sized
types could need them.

## 3. Decision 2 — grammar

- **Formals**: `parameter type T` and `parameter type T = data_type` join the
  module/interface parameter-port grammar; `localparam type` joins body
  items. The default rides the inherited `type` slot (null = no default).
  `type` is already a token (`TK_TYPE`, SV-mode lexer-gated like
  `TK_TYPEDEF`).
- **Actuals**: `parameter_value_assignment` alternatives gain a `data_type`
  form for the *keyword-headed* types (`logic [7:0]`, `int`, packed
  struct/enum, …) — unambiguous, since an expression cannot start with a type
  keyword. A **bare identifier actual stays an expression** (`Identifier`):
  `.T(word_t)` and `.W(P)` are lexically identical, and the parser cannot
  know the formal's kind. The identifier case resolves in the parent's scope
  at substitution time (§4).

## 4. Decision 3 — identifier actuals resolve in the parent's scope

`.T(word_t)` names a type visible **at the instantiation site** (§23.10 — the
actual is evaluated in the instantiating scope). By §3.13 a name cannot
denote both a value and a type in one scope, so the rule is unambiguous:

> In the parent's `TypedefInliner` walk, a `ParamArg` whose value is a bare
> unscoped `Identifier` that **resolves to a typedef alias in scope** is
> rewritten to a clone of the alias's concrete type. Identifiers that resolve
> to nothing stay untouched (they are value-param actuals referencing
> parent parameters, already inlined or folded by other passes).

Scoped actuals (`.T(pkg::word_t)`) need nothing new: `PackageInliner` copies
the package typedef into the parent and strips the scope before
`TypedefInliner` runs, reducing to the bare-identifier case.

The child never sees an identifier actual for a type formal: by the time the
flattener recurses with the parent's `ParamArg` list, every type actual is a
concrete `DataType` (keyword form from the parser, identifier form from the
rule above). A `ParamArg` targeting a `TypeParam` whose value is still an
expression is therefore an error the child reports loudly.

## 5. Decision 4 — parent-side dims fold before the child binds

A keyword actual's packed dims are expressions in the parent
(`.T(logic [W-1:0])`). They live inside the parent's body, so the parent's
own `ParameterInliner`/`ConstantFolding` (which traverse instance param args
generically) fold them to constants before `ResolveModule` recurses into the
child clone. `TypeParamInliner` refuses a bound type whose packed dims are
not constant — the same "alias width is not constant" shape ADR-0009's cast
lowering uses.

## 6. Interfaces

Interface headers carry the same `params: list(Declaration)` slot, and
interface pseudo-modules elaborate through the same `ResolveModule`
recursion (ADR-0008 §2). `TypeParamInliner` treats both roots identically —
no extra machinery, mirroring ADR-0009.

## 7. Not supported (rejected loudly or unchanged)

| Feature | v1 behavior | Future home |
|---|---|---|
| `type()` operator as actual or default (`.T(type(x))`, §6.23) | not parsed — unchanged | own ADR (needs expression typing) |
| `$bits`/type query functions over a type param | whatever `$bits` support exists today; the reduced typedef makes the type concrete first | with `$bits` support |
| `parameter type` in classes / virtual interfaces | out of the synthesizable subset | — |
| `defparam` on a type parameter | illegal per §23.10.1 — rejected | — |
| unpacked-dim'd type actual (`.T(logic [7:0] [4])` in dims-less position) | inherits ADR-0009 §5 array-typedef rules after reduction | — |

## 8. Errors

| Condition | Clause | Message shape |
|---|---|---|
| unoverridden `TypeParam` with no default | §6.20.3 | `type parameter 'T' has no default and no override` |
| override targets a `localparam type` | §6.20.4 | `'T' is a localparam type and cannot be overridden` |
| actual for a type formal is not a data type (expression survived to the child) | §23.10 | `override of type parameter 'T' is not a type` |
| actual for a *value* formal is a data type | §23.10 | `override of parameter 'X' with a type` |
| bound type's packed dims not constant | §6.20.2 | `type parameter 'T': the bound type's width is not constant` |

## 9. Pass placement & structure

- `lib/src/passes/transformations/type_param_inliner.{cpp,hpp}` — the pass
  (`TransformationBase`, constructed with the same `ParamArg::ListPtr` as
  `ParameterInliner`), wired into `ResolveModule` **first among the
  inliners** (before `ParameterInliner`).
- Parent-side identifier-actual rule (§4): a `ParamArg` case in
  `TypedefInliner::substitute`.
- Grammar: parameter-port and body rules for `parameter type` /
  `localparam type`; `data_type` alternatives in the instantiation
  param-actual rules.

## 10. Phasing (each lands green) & test plan

1. **Grammar + AST round-trip** — parse/generate tests for formals with and
   without defaults, `localparam type`, keyword actuals (`sv_type_param0`).
2. **Reduction** — `TypeParamInliner` alone: default binding, named/ordered
   overrides, error catalogue (§8).
3. **End-to-end flatten** — golden `module_flattener` tests: default used,
   keyword override, parent-typedef identifier override (§4), scoped
   `pkg::T` override, type param typing child ports, chained
   parameterization (parent passes its own type param down).
4. **Cosim** — a Verilator-checked design whose datapath width comes from a
   type override.
