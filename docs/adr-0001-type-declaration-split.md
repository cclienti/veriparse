# ADR-0001 — Type / Declaration split (systematic AST redesign)

- **Status**: Accepted — all §9 decisions settled; ready for the implementation plan.
- **Date**: 2026-06-09
- **Scope**: *everything* that touches types and declarations. Nothing is off the
  table (the software has no users yet). The goal is a **single, systematic,
  complete** model, so that the representation of a type is never reinvented per
  construct again — and **with no out-of-scope** (§10: every advanced construct
  already has a home).
- **Normative reference** — the **single source of truth** for this work, per
  parse mode:
  - SystemVerilog mode → **IEEE 1800-2017** (`docs/1800-2017.pdf`);
  - Verilog mode → **IEEE 1364-2005** (`docs/verilog-std-1364-2005.pdf`).

  Pinning the *edition* matters because clause/annex numbers shift between
  versions (we cite `§6.7.1`, `A.2.2.1`, …); 1800-2017 is chosen as it is the LRM
  we hold and can verify citations against (1800-2023 exists but is not on hand).
  At the *slightest* doubt — about a construct, a constraint, or what is legal —
  defer to the standard, never to memory or to an existing code comment. And check
  **both** the BNF (Annex A) **and** the prose of the relevant clause: they are
  complementary, and a constraint often lives only in the prose. (Case in point:
  Annex A.2.2.1 grammatically allows `reg` after a net type, but §6.7.1 prose adds
  the lexical rule that forbids `wire reg`/`tri reg` — the grammar alone would
  have misled us.)

  The **type/declaration core** (Annex A.2) is stable across 1800-2012 / 2017 /
  2023 — version differences are **additive** (new features), not breaking — so
  this redesign is not version-fragile, and **no per-version mode** is needed: a
  single `sv_mode` targeting 1800-2017 accepts the superset. The additive
  `DataType`/`Declaration` model absorbs any later version the same way (§10).

---

## 1. Problem

Today, **every place that declares something reinvents "how to represent a
type"**. Inventory of the competing representations:

| Site | Current representation of the type |
|---|---|
| Variable (`int x`) | the type node **IS** the variable: `Int`/`Logic`/`Reg`… inherit `Variable`, carry `name`/`lengths`/`right` |
| Net (`wire logic w`) | a `Wire`/`Tri`/`Supply` node (kind = node) + a `type:` child = `Identifier{"logic"}` |
| Port (`input logic [7:0] p`) | `Ioport{ first: IODir(name,sign,widths), second: Variable }` — type spread across `IODir` + a `Variable` (duplicated name) |
| Parameter (`parameter int N`) | `Parameter{ type: enum(INTEGER/REAL/LOGIC/INT/…), sign, widths, value }` — type = **ad-hoc enum** |
| Function return | `Function{ rettype: enum, retsign, retwidths, rettype_ref: Identifier }` — yet another ad-hoc repr |
| Struct member | `StructMember{ type: Node, lengths, right }` |
| Cast (`int'(x)`) | `Cast{ type: Identifier{"int"} }` |
| Typedef (`typedef int it`) | `Typedef{ def: Int{name:""} }` — an **empty-named** descriptor (the `h` bug) |

Consequences:
- **5+ ways** to say "the type is `logic [7:0] signed`" depending on context.
- A type on its own (typedef, member, return, cast) = a variable node **with no
  variable** → an empty `name` that leaks (cf. the `h` obfuscator bug).
- Any new SV feature (interfaces, `var`, `const`, named types, …) forces
  re-wiring the type at *every* site. Hence "redoing it over and over."

**Root cause**: the **type / declaration fusion**. A type node carries the
variable `name`, its unpacked dims, its init, its direction, its net-kind… The
"type" does not exist as a standalone entity.

---

## 2. Principles of the new model

1. **Two orthogonal abstractions.**
   - **`DataType`** — a description of a type, **nameless**, with no unpacked
     dims, no init, no direction, no net-kind, no qualifier. Sealed hierarchy.
     Carries *only* what is intrinsic to the type.
   - **Declaration / binding** — binds a *name* (and a role) to a `DataType`,
     plus site-specific extras: unpacked dims, init/default, direction,
     qualifiers (`const`/`var`/lifetime), net strength/delays. *(The net-type —
     wire/tri/supply/… — is the declaration **node type**, not an extra; cf. §4.)*

2. **A single `DataType` everywhere.** Variable, net, port, task/function arg,
   return, parameter, struct member, typedef, cast, operand of `type()` — all
   reference the **same** `DataType`. Zero ad-hoc repr.

3. **Packed dimensions belong to the type** (`data_type ::= … {
   packed_dimension }`). Unpacked dimensions belong to the **declaration**
   (`name { unpacked_dimension }`).

4. **Scope (`pkg::`) is a property of the *name reference*** — on `NamedType`
   (type ref) and `Identifier` (value ref). No separate scope node.

5. **Implicit is explicit**: no type written ⇒ `ImplicitType` (never a fabricated
   `Logic`). Resolving the default (net⇒logic 4-state, var⇒logic) is a **pass**,
   not a fact baked into the AST. *(This removes the pure-Verilog objection:
   `wire w` ⇒ `WireNet{ type: ImplicitType }`, we do not inject `logic`.)*

6. **Built-in is a concrete node, named is `NamedType`**; a type slot never
   holds an `Identifier` (a value node). A resolution pass turns `NamedType`
   (and `ImplicitType`) into a concrete type.

7. **Lean nodes, explicit role.** A `DataType` carries only the type; a
   declaration carries only the binding. Passes access `decl.name` and
   `decl.type` uniformly.

---

## 3. The `DataType` hierarchy (nameless)

**Discrimination by node type** (approach 2, no `kind` enum): each type is a
distinct node, like the current `Logic`/`Reg`/`Bit`/`Int`. `signing` and
`packed_dims` are on the **base** `DataType` (decision #2 = (a)).

```
DataType (abstract)   { signing: enum(NONE,SIGNED,UNSIGNED) ; packed_dims: list(Dimension) }
  # integer vector (signing + packed relevant)
  ├── LogicType  ├── RegType  ├── BitType                            # logic / reg / bit
  # integer atom (signing relevant ; packed inert)
  ├── ByteType ├── ShortintType ├── IntType ├── LongintType
  ├── IntegerType ├── TimeType                                       # byte/shortint/int/longint/integer/time
  # non-integer (signing + packed inert)
  ├── RealType ├── ShortrealType ├── RealtimeType
  ├── StringType ├── ChandleType ├── EventType                       # real/…/string/chandle/event
  # named (packed relevant ; signing inert)
  ├── NamedType      { name, scope: list(ScopeName) }                # my_t, pkg::T, A::B::T [..]
  # aggregates
  ├── StructType     { is_packed: bool ; members: list(Member) }     # struct {..} [..]
  ├── UnionType      { is_packed: bool ; members: list(Member) }     # union  {..} [..]
  ├── EnumType       { base: DataType ; items: list(EnumItem) }      # enum base {..} [..]
  # others
  ├── ImplicitType                                                   # (no keyword)
  ├── TypeOpExpr     { expr }                                        # type(expr)       — SV operator
  ├── TypeOpType     { type: DataType }                              # type(data_type)  — SV operator
  └── VoidType                                                       # void return
```

`is_packed` (bool, `struct packed` layout) is distinct from `packed_dims` (the
`[..]`, on the base). `signing`/`packed_dims` are inert (NONE/[]) on the types
where they do not apply (atom→packed, named→signing, real→both) — zero cost,
uniform access (cf. #2).

Cross-cutting decisions:

### 3.1 `signing` — tri-state
`enum Signing { NONE, SIGNED, UNSIGNED }` (not a `bool`). We **preserve** what was
written; the default depends on the kind (vector ⇒ unsigned, atom ⇒ signed) and
is resolved by a pass. The current `bool sign` loses the "written or not" info
and conflates it with the default.

### 3.2 `packed_dims` / `signing` — placement (SETTLED: on the base)
Both on the `DataType` base (empty list / `NONE` when not applicable). Reason:
`signing` and `packed_dims` have applicability sets that **cross** (atom→signing
without packed; named→packed without signing), so no shared intermediate fits;
and uniform access `type.signing` / `type.packed_dims` simplifies passes. The
YAML is identical to the alternative for every legal input (empty list omitted) —
the only gain is the uniform C++ API.

### 3.3 `NamedType`
Takes over the current `CustomType` (`name`), **+ `packed_dims`** (the `[..]`
after a named type belong to the type) and **+ `scope: list(ScopeName)`**. Rename
`CustomType → NamedType` for clarity ("reference to a named type"). A resolution
pass replaces it with the concrete underlying type.

`scope` is an ordered **scope list** (`SETTLED`), uniform for `pkg::T`,
`$unit::T`, and the **nested class scope** `A::B::T` (`class_type ::= id {:: id
…}`): `ScopeName { name, params: list(ParamArg) }` — one segment per `::` step;
`params` carries a parameterized class scope (`A#(8)::T`), null for
package/`$unit`. Empty scope = unscoped. Replaces the flat `package` string —
which only held one level. The **value side** (`Identifier`, `FunctionCall`,
`TaskCall`) takes the same `scope: list(ScopeName)`.

#### 3.3.1 Two scope axes — SV `::` resolution vs Verilog `.` hierarchy (SETTLED)

These are **two orthogonal axes** and must not be conflated:

- **SV `::` scope-resolution** — `pkg::x`, `$unit::x`, `A::B::T`, `A#(8)::T`. A
  *compile-time namespace qualifier* on a name; applies to **types and values
  alike**. Modelled by `scope: list(ScopeName)`.
- **Verilog `.` hierarchical reference** — `top.u1.sig`, `genblk[i].sig`. A *path
  through the instance/generate hierarchy*; a **value-side-only** axis. Modelled
  by the existing `IdentifierScope` child.

The original draft (§3.3/§5.10/§6) said "`Identifier` takes `scope:
list(ScopeName)`" but **missed a name clash**: `Identifier` *already* has a child
literally named `scope` — the hierarchical `IdentifierScope`. The two axes are
genuinely different, so they get genuinely different fields.

**Resolution (SETTLED):** the field name `scope` is reserved for the **SV `::`**
axis *everywhere* (type ref and value ref); the misnamed hierarchical node/child
is renamed to its honest name:

- `IdentifierScope → HierName`, `IdentifierScopeLabel → HierLabel`;
- `Identifier.scope` (the hierarchy child) → `Identifier.hier: HierName`.

So a value ref carries the two axes as orthogonal fields:
`Identifier { name, scope: list(ScopeName) (::), hier: HierName (.) }`, and
`scope` now means the **same thing** on `NamedType` (type) and on
`Identifier`/`FunctionCall`/`TaskCall` (value). `FunctionCall`/`TaskCall` gain
`scope` (the `::` axis); a hierarchical *call* (`u1.t()`) is not modelled yet
(additive later).

### 3.4 Aggregates
`StructType`/`UnionType` carry `is_packed`, `signing`, `members:[Member]`,
`packed_dims`; `UnionType` also `is_tagged` (`union tagged`). `EnumType` carries
`base:DataType` (instead of the current `base_type` enum — a real `DataType`),
`items:[EnumItem]`, `packed_dims`. Members become **declarations** `Member`
(cf. §4), not an ad-hoc `StructMember`.

`EnumItem { name, value, range: Dimension }` — the SV `enum { A[3:0], B }` form
names a *range* of members; `range` reuses a **`Dimension`** (`enum {A[4]}` →
`SizeDim(4)`, `enum {A[3:0]}` → `RangeDim(3,0)`), null for the plain form. (Grammar:
`enum_name_declaration ::= enum_identifier [ [ N [ : N ] ] ] [ = expr ]`.)

### 3.5 `ImplicitType`
`signing` + `packed_dims`, no keyword. Used by `var a`, `var [3:0] a`, `wire
[3:0] w` (net with no data_type), an untyped parameter. The default (logic) is
resolved by a pass.

### 3.6 `type()` operator — `TypeOpExpr` / `TypeOpType` (node per form)
`type(expr)` → `TypeOpExpr{ expr }` ; `type(data_type)` → `TypeOpType{ type:
DataType }`. **Two nodes** (no heterogeneous `arg` slot), consistent with
decision #8 (Cast). Distinct from `NamedType`. Not parsed yet; the model provides
for it.

### 3.7 `VoidType`
For `function void f(...)` / the absence of a return. Avoids a special case on the
function.

### 3.8 Dimensions — `Dimension` (replaces `Width`/`Length`), **node per form**
Discrimination by **node type** (consistent with approach 2 / decision B), no
`kind` enum. Used for packed (`DataType.packed_dims`) and unpacked
(`Declaration.unpacked_dims`) — the context decides.

```
Dimension (abstract)
  ├── RangeDim   { left, right }      # [L:R]  range ; asc/desc order preserved
  ├── SizeDim    { size }             # [N]    size, ≡ [0:N-1] ; round-trips "[4]"
  ├── UnsizedDim { }                  # []     dynamic array
  ├── QueueDim   { bound }            # [$] | [$:N]  (bound null for [$])
  └── AssocDim   { index_type }       # [type] | [*]  (index_type: DataType, null for '*')
```

Exact distinctions (round-trip): `[4]`→`SizeDim`, `[0:3]`→`RangeDim(0,3)`,
`[3:0]`→`RangeDim(3,0)`. Gone is the `lsb=null` convention for `[N]`.

Validation **by context** (not by node type):
- **packed** (`logic [7:0]`, `my_t [3:0]`, `bit []`): `RangeDim`/`UnsizedDim`
  only.
- **unpacked**: all forms.

⇒ removes `Width` and `Length`; queues/dynamic/associative arrays come in with no
new node (just wire up the grammar).

---

## 4. The declaration model

Common base + role nodes (lean, dispatch by type; decision #1 = B, the "fat
single node" was rejected):

```
Declaration (abstract)   { name,  type: DataType }
  ├── Var       { unpacked_dims, init, is_var, is_const, lifetime }              # data_declaration
  ├── Net       (abstract — node per net-type, see below)                       # net_declaration (signing -> on the DataType)
  ├── Port      { direction, decl: (Var | Net) }                                # ANSI port (dir + declaration)
  ├── Arg       { direction, unpacked_dims, default, is_var }                    # task/function arg
  ├── Param     { is_local, value, unpacked_dims }                              # parameter / localparam (value; unpacked = array param)
  ├── TypeParam { name, type }                                                  # parameter type T = …
  ├── Typedef   { name, type, unpacked_dims, fwd_kind }                         # typedef (type null + fwd_kind set = forward typedef)
  ├── Member    { unpacked_dims, init }                                         # struct/union member
  └── Genvar    { }                                                             # genvar (compile-time, NO type slot — always integer)
```

- `name` + `type` on the base ⇒ uniform access everywhere. *(For `Port`, name/type
  are delegated to `decl`: `port.name ≡ port.decl.name`, `port.type ≡
  port.decl.type`.)*
- `Net` is **node-per-net-type** (`WireNet`/`TriNet`/…/`UserNet`/`ImplicitNet`,
  see the Net hierarchy below), **not** a `net_kind` enum (cf. #6) — same reason
  as `DataType` node-per-type (the family includes a *named* member, the user
  `nettype`). A net's `signing` and type are on the `DataType` (`ImplicitType`
  if not written). The old `Wire`/`Tri`/`Supply`/`Net` (conflated with the
  variable) are gone; these are clean *declaration* nodes carrying `type:
  DataType`.
- `direction: enum{ INPUT, OUTPUT, INOUT, REF, CONST_REF }` (all keyword tags ⇒
  enum is fine here, unlike net-types).
- `Port` wraps a direction + an inner declaration (`Net` or `Var`): a port *is* a
  directed declaration. No name duplication.

> The "bound value" gets a **name distinct per semantics** (intentional, not on
> the base): `init` (initial value, Var/Member), `cont_assign` (**continuous**
> assign of a net), `default` (argument default), `value` (a parameter's value).
> `Genvar` is compile-time and untyped — possibly to be pulled out of the
> `Declaration` hierarchy.

Notes (from the IEEE A.2 cross-check):
- **`Net.strength`** (node per form, no kind enum): `Strength` (abstract) →
  `DriveStrength{ s0, s1 }` / `ChargeStrength{ charge }`; `Net.strength: Strength`
  (null = none). E.g. `wire (strong0,weak1) w` / `trireg (small) c`. Plus the
  `is_vectored` / `is_scalared` flags (`wire vectored [3:0] w`).
- **`Typedef`** carries unpacked dims (`typedef logic [7:0] mem_t [256]`) and a
  **forward** form: **`type == null` marks a forward typedef** (a normal typedef
  always has a `type`); `fwd_kind: enum{ NONE, ENUM, STRUCT, UNION, CLASS,
  INTERFACE_CLASS }` is the optional kind hint (`typedef class C;`, `typedef enum
  E;`, or bare `typedef foo_t;` with `fwd_kind:NONE`).
- **`Param.unpacked_dims`**: array parameters (`parameter int N [4]`).
- **rand** (`rand`/`randc`) is a class-context qualifier on `Member`/`Var` —
  modelled as a property in §10, not a base field.

### The `Net` hierarchy (node per net-type — decision #6 = B)
```
Net (abstract : Declaration)   { unpacked_dims, cont_assign, strength: Strength,
                                 is_vectored, is_scalared, ldelay, rdelay }   # type: DataType from the base
  # the 12 IEEE net_types (keyword tags -> nodes, like LogicType/IntType)
  ├── WireNet  ├── TriNet  ├── Tri0Net  ├── Tri1Net  ├── TriandNet  ├── TriorNet
  ├── TriregNet ├── WandNet ├── WorNet  ├── UwireNet ├── Supply0Net ├── Supply1Net
  # distinct net forms
  ├── InterconnectNet                                  # interconnect [3:0] io;
  ├── UserNet                                          # user nettype: my_net_t w;  (type = NamedType(my_net_t))
  └── ImplicitNet                                      # no net keyword written (port default; resolve via default_nettype)
```
`UserNet` carries the nettype reference in the inherited `type` (a `NamedType`),
exactly as `NamedType` does for types — that named member is *why* nets are
node-per-type, not an enum. `ImplicitNet` is the no-keyword case (a default net
in a port, parallel to `ImplicitType`), the only `Net` that appears without an
explicit keyword. The net keyword stays at the edges (parser in / generator out),
out of the AST.

---

## 5. Construct-by-construct mapping

Everything listed, mapped explicitly. Format: `source` → target AST.

### 5.1 Variables
```
int x;                  Var{ name:x, type: IntType }
logic [7:0] y;          Var{ name:y, type: LogicType(packed:[7:0]) }
var a;                  Var{ name:a, type: ImplicitType, is_var:true }
var [3:0] a = 0;        Var{ name:a, type: ImplicitType(packed:[3:0]), init:0, is_var:true }
const var int c = 5;    Var{ name:c, type: IntType, init:5, is_var:true, is_const:true }
static int s = 2;       Var{ name:s, type: IntType, init:2, lifetime:STATIC }
int arr [4];            Var{ name:arr, type: IntType, unpacked_dims:[4] }
my_t [3:0] v [2];       Var{ name:v, type: NamedType(my_t, packed:[3:0]), unpacked_dims:[2] }
```
⇒ removes `DataModifier`, `ImplicitType`-as-Variable, `CustomTypeVar`.

### 5.2 Nets (wire / tri / supply)
```
wire w;                 WireNet{ name:w, type: ImplicitType }
wire [3:0] b;           WireNet{ name:b, type: ImplicitType(packed:[3:0]) }
wire logic [3:0] x;     WireNet{ name:x, type: LogicType(packed:[3:0]) }
tri logic r;            TriNet{ name:r, type: LogicType }   # only `logic` is a valid explicit net data type (§6.7.1)
wire my_t m;            WireNet{ name:m, type: NamedType(my_t) }
supply0 s;              Supply0Net{ name:s, type: ImplicitType }
wire #(2,3) d = e;      WireNet{ name:d, type: ImplicitType, cont_assign:e, ldelay/rdelay }
wire (strong0,weak1) g; WireNet{ name:g, strength: DriveStrength(strong0,weak1), type: ImplicitType }
trireg (small) c;       TriregNet{ name:c, strength: ChargeStrength(small), type: ImplicitType }
interconnect [3:0] ic;  InterconnectNet{ name:ic, type: ImplicitType([3:0]) }
my_net_t un;            UserNet{ name:un, type: NamedType(my_net_t) }   # user-defined nettype
```
⇒ the old `Wire`/`Tri`/`Supply0`/`Supply1`/`Net` (conflated with the variable)
are replaced by clean declaration nodes. **Pure Verilog intact**: `wire w` ⇒
`WireNet{ type: ImplicitType }`, no `logic` injected.

> Net data-type rules (§6.7.1), two independent constraints:
> 1. **4-state**: a net's data type must be 4-state — `logic`, a 4-state named
>    type (`wire addressT w;`), a packed struct/union of 4-state, etc. `bit`
>    (2-state) is rejected.
> 2. **Lexical**: a net-type keyword **directly followed by `reg`** is in error
>    (`tri reg r;`, `wire reg w;`). `reg` *is* otherwise a valid net data type —
>    with lexical elements in between (`wire (strong0,weak1) reg w;`) or via a
>    typedef. So `RegType` is reachable as a net `type`, just not adjacent to the
>    keyword.
>
> `reg`/`bit` are of course fully valid **variable** types (`reg x;` ⇒ `Var{
> type: RegType }`, `bit b;` ⇒ `Var{ type: BitType }`). These are checks, not
> AST-model constraints — the `DataType` slot itself accepts any type.

> **`interconnect` restrictions (§6.7.1) are enforced by the grammar.** The
> production is `interconnect implicit_data_type [ # delay_value ] name {dims}` —
> no data-type keyword, no strength, a *single* delay, no inline assignment — so
> `interconnect logic …` / a strength / `#(1,2,3)` / `= e` are **syntax errors**,
> and `InterconnectNet`'s inherited `strength`/`cont_assign`/second-delay are
> simply never populated. Only the two *usage* restrictions — driving an
> interconnect with a continuous `assign`, or reading it in a procedural context —
> are cross-statement and need a **semantic pass**.

### 5.3 Module ports (ANSI and non-ANSI)
```
input logic [7:0] p;    Port{ direction:INPUT,  decl: ImplicitNet{ type: LogicType([7:0]), name:p } }   # neither wire nor var written -> ImplicitNet (resolved)
output wire o;          Port{ direction:OUTPUT, decl: WireNet{ type: ImplicitType, name:o } }
input var int q;        Port{ direction:INPUT,  decl: Var{ is_var:true, type: IntType, name:q } }       # var written -> Var
output my_t r;          Port{ direction:OUTPUT, decl: ImplicitNet{ type: NamedType(my_t), name:r } }
inout [3:0] io;         Port{ direction:INOUT,  decl: ImplicitNet{ type: ImplicitType([3:0]), name:io } }
```
Non-ANSI (`module m(a); input a; wire a;`): `Port{ name:a }` (name only) + separate
`Net`/`Var` items; a pass merges them. ⇒ replaces `Ioport` +
`IODir`/`Input`/`Output`/`Inout`. Direction is no longer a type node (`IODir`
carried sign+widths!), it is a property of the port.

> Net-vs-var nuance of a port (default kind by direction §23.2.2.3): this is a
> **semantic resolution**, not a parsing decision. The AST keeps what was written:
> `ImplicitNet` when no net keyword (and `Var` when `var` is written); a pass
> resolves the concrete net-type (via `default_nettype`) or net-vs-var default.

### 5.4 Function
```
function automatic logic [7:0] f(input int a, output bit b);
  …
endfunction
                        Function{ name:f, lifetime:AUTOMATIC,
                                  return_type: LogicType([7:0]),
                                  args: [ Arg{dir:INPUT, type:IntType, name:a},
                                          Arg{dir:OUTPUT, type:BitType, name:b} ],
                                  statements: … }

function void g(); …    Function{ name:g, return_type: VoidType, … }
function my_t h(); …    Function{ name:h, return_type: NamedType(my_t), … }
function f2(); …        Function{ name:f2, return_type: ImplicitType, … }   # implicit 1-bit
```
⇒ removes `rettype`(enum) + `retsign` + `retwidths` + `rettype_ref` → **a single**
`return_type: DataType`. Function ports become `Arg`.

### 5.5 Task
```
task automatic t(input int a, ref logic [3:0] b); …
                        Task{ name:t, lifetime:AUTOMATIC,
                              args: [ Arg{dir:INPUT, type:IntType, name:a},
                                      Arg{dir:REF, type:LogicType([3:0]), name:b} ],
                              statements: … }
```
No return. `args` = `Arg`. (Today `automatic: bool` → `lifetime` enum for
consistency with the rest.)

### 5.6 Parameters / localparams
Two roles: **value parameter** (`Param`) and **type parameter** (`TypeParam`).
`Param` reuses the same `type: DataType` as everything else; `value` is the
default (**optional** — null on a param-port overridden at instantiation).

```
parameter int N = 8;             Param{ name:N, type: IntType,        value:8, is_local:false }
localparam logic [3:0] M = 4'h5; Param{ name:M, type: LogicType([3:0]), value:.., is_local:true }
parameter N = 8;                 Param{ name:N, type: ImplicitType,   value:8 }          # untyped (type from value)
parameter [7:0] W = 4;           Param{ name:W, type: ImplicitType(packed:[7:0]), value:4 }   # range, no keyword
parameter signed S = -1;         Param{ name:S, type: ImplicitType(signing:SIGNED), value:-1 }
parameter int P;                 Param{ name:P, type: IntType,        value: null }      # param-port without default
parameter type T = int;          TypeParam{ name:T, type: IntType }                      # type-param
parameter type U;                TypeParam{ name:U, type: null }                         # type-param without default
localparam my_t L = e;           Param{ name:L, type: NamedType(my_t), value:e, is_local:true }
parameter int M [4] = '{…};      Param{ name:M, type: IntType, unpacked_dims:[4], value:'{…} }   # array parameter
```

⇒ removes the `Parameter.type` enum + `sign` + `widths` → **a single** `type:
DataType` (signing/range/named/implicit for free). `Localparam` = `Param{
is_local:true }` (no separate node).

**Placement** (structurally unchanged): `Param`/`TypeParam` appear in
`Module.params` (the `#(...)` list of param-ports), in the `items`
(localparam/parameter in the module body), and in `Package.items`.
`Module.params` becomes `list(Param|TypeParam)` (via the `Declaration` base).

**Param-adjacent** (distinct from a parameter *declaration*, kept as-is):
`ParamArg` (instantiation override `m #(.N(4)) u`), `Defparam` (`defparam`
override). `specparam` (a `specify`-block parameter) → a `Specparam{ name, value }`
node once we parse `specify` (same pattern, additive).

### 5.7 Struct / Union
```
struct packed signed { logic [7:0] a; my_t b; } s;
                        Var{ name:s, type: StructType{ is_packed:true, signing:SIGNED,
                               members: [ Member{ name:a, type: LogicType([7:0]) },
                                          Member{ name:b, type: NamedType(my_t) } ] } }

union { int x; bit [3:0] y; } u;
                        Var{ name:u, type: UnionType{ members:[ Member{x:int}, Member{y:bit[3:0]} ] } }
```
⇒ `StructUnionDef`/`StructDef`/`Union` → `StructType`/`UnionType` (`DataType`s).
`StructMember` → `Member` (a declaration). The aggregate **no longer wraps** the
variable: `Var{ type: StructType }`, gone is the empty `CustomTypeVar`.

### 5.8 Enum
```
typedef enum logic [1:0] { A, B=2, C } state_t;
                        Typedef{ name:state_t,
                          type: EnumType{ base: LogicType([1:0]),
                                          items: [ EnumItem{A}, EnumItem{B, value:2}, EnumItem{C} ] } }

enum { OP[4], NOP } e;  Var{ name:e, type: EnumType{ base: ImplicitType,
                          items: [ EnumItem{OP, range: SizeDim(4)}, EnumItem{NOP} ] } }   # OP0..OP3, then NOP
```
⇒ `EnumDef.base_type`(enum) → `EnumType.base`(DataType). `EnumItem` gains a
`range` (the `[N]`/`[N:M]` member-range form).

### 5.9 Typedef
```
typedef int it;                 Typedef{ name:it,    type: IntType }
typedef logic [3:0] nibble;     Typedef{ name:nibble,type: LogicType([3:0]) }
typedef my_t [3:0] arr_t;       Typedef{ name:arr_t, type: NamedType(my_t, packed:[3:0]) }
typedef logic [7:0] mem_t [256];Typedef{ name:mem_t, type: LogicType([7:0]), unpacked_dims:[256] }   # unpacked dims on the typedef name
typedef struct {…} s_t;         Typedef{ name:s_t,   type: StructType{…} }
typedef pkg::T other_t;         Typedef{ name:other_t, type: NamedType(T, scope:[pkg]) }
typedef A::B::T nested_t;       Typedef{ name:nested_t, type: NamedType(T, scope:[A, B]) }   # nested class scope
typedef class C;                Typedef{ name:C, type: null, fwd_kind: CLASS }    # forward typedef
typedef enum E;                 Typedef{ name:E, type: null, fwd_kind: ENUM }     # forward typedef
```
⇒ `Typedef.def: Node` (empty-named descriptor) → `type: DataType`. **No more empty
names** ⇒ the `h` bug fixed at the root (the obfuscator guard becomes unnecessary,
but harmless).

### 5.10 Scope — SV `::` resolution vs Verilog `.` hierarchy (two axes, cf. §3.3.1)
- **SV `::` scope-resolution** — `scope: list(ScopeName)`, `ScopeName{ name,
  params }`:
  - **type** ref: `NamedType{ name, scope }` (`pkg::T`, `$unit::T`, `A::B::T`,
    `A#(8)::T`).
  - **value** ref: `Identifier{ name, scope }` (`pkg::sig`, `A::B::x`);
    `FunctionCall`/`TaskCall` take the same `scope`.
  - Replaces the old single-level `package` string (which could not express
    `A::B::x`).
- **Verilog `.` hierarchical reference** — `hier: HierName` (renamed from
  `IdentifierScope`): `top.u1.sig`, `genblk[i].sig`, a path through the
  instance/generate hierarchy. Value-side-only, **orthogonal** to `::`.
- `import pkg::*` / `import pkg::x`: `Import` (existing, kept — SV imports are
  single-level, so its flat `package` + `symbol` suffices).

A resolution pass (PackageInliner) consumes the `::` `scope` list (instead of the
old flat `package` string).

### 5.11 Cast — node per form
A cast's target (`casting_type`) is broader than a type: type, signing, or size.
Instead of a heterogeneous slot, **a node per form** (each slot well-typed):
```
int'(x)                 TypeCast{ target: IntType, expr:x }
logic'(x)               TypeCast{ target: LogicType, expr:x }
my_t'(x)                TypeCast{ target: NamedType(my_t), expr:x }
signed'(x)              SigningCast{ signing: SIGNED, expr:x }
unsigned'(x)            SigningCast{ signing: UNSIGNED, expr:x }
8'(x)                   SizeCast{ size: 8, expr:x }
WIDTH'(x)               SizeCast{ size: Identifier(WIDTH), expr:x }
const'(x)               ConstCast{ expr:x }
```
```
Cast (abstract)   { expr }
  ├── TypeCast    { target: DataType }
  ├── SizeCast    { size: <const expr> }
  ├── SigningCast { signing: enum(SIGNED, UNSIGNED) }
  └── ConstCast   { }                                     # const'(x)
```
⇒ replaces the current `Cast{ type, expr }` (PR #14). `TypeCast.target` is
*always* a `DataType`; no heterogeneous type-ish slot left in the AST.

### 5.12 Name references & subroutine calls — `Identifier → Call → {FunctionCall, TaskCall}`

A subroutine call is just **a scoped/hierarchical name that is invoked** — so the
calls *inherit* `Identifier` rather than re-spelling `name`/`scope`/`hier`
(killing the same duplication §1 indicts, now for value refs):

```
Identifier        { name, scope: list(ScopeName), hier: HierName }   # value name ref
  └── Call         { args: list(Node) }                              # an invoked name
        ├── FunctionCall                                             # known function
        └── TaskCall                                                 # known task
```

So `call.name`/`call.scope`/`call.hier` come for free, and
`is_node_category(Identifier)` makes every name pass (rename, scope resolution)
handle a call's name uniformly; `is_node_category(Call)` means "any call".

**Task vs function is NOT syntactic (SETTLED, against the standard).** The SV BNF
`tf_call ::= ps_or_hierarchical_**tf**_identifier …` is *task-or-function*; and
§13.4.1 makes a function legally callable **as a statement** (`void'(f())`, or
with a warning). So a bare statement call `f();` cannot be classified without a
symbol table. What *is* known at parse time is **position**:

```
x = f(a)        FunctionCall   # expression position ⇒ necessarily a function
f(a);           Call           # statement ⇒ task-or-void-fn, UNRESOLVED
void'(f(a));    TypeCast{ target: VoidType, expr: FunctionCall }   # known function
```

`TaskCall` (and refining a `Call` to `FunctionCall`) is the job of a future
**symbol-aware resolution pass** — same "resolve later" pattern as
`NamedType`/`ImplicitType`. The bare parser never guesses "task". Rendering is
**position-driven** (the trailing `;` comes from statement context, not the node
type), so `Call`/`FunctionCall`/`TaskCall` all round-trip as `name(args)`.

`SystemCall` (`$display`) stays **outside** this hierarchy: a `$`-name is not a
scoped identifier (no `scope`/`hier`), so it keeps its own `syscall` string.

**`Disable.dest`** is an `Identifier` (a `disable top.u1.blk;` target is a
possibly-hierarchical name), not a flat string.

---

## 6. Node delta (removed / renamed / new)

**Removed** (merged into the new model):
`DataModifier`, `CustomTypeVar`, `Net`, `Wire`, `Tri`, `Supply0`, `Supply1`,
`Logic`(-as-net), `Ioport`, `IODir`, `Input`, `Output`, `Inout`, `Localparam`
(→ `Param.is_local`), `StructMember`(→ `Member`), `StructUnionDef`/`StructDef`/
`Union`(→ `StructType`/`UnionType`), `EnumDef`(→ `EnumType`).

**Variables-as-types → bare DataTypes**: `Integer`,`Real`,`Byte`,`Shortint`,
`Int`,`Longint`,`Shortreal`,`Realtime`,`Bit`,`Reg`,`Logic` ⇒ become `DataType`s
(`LogicType`/`RegType`/`BitType`/`IntType`/`RealType`/…), **without**
`name`/`lengths`/`right`. Declarations go through `Var`/`Net`/…

**Renamed**: `CustomType → NamedType`.

**New**: `DataType` (base) + variants `LogicType`/`RegType`/`BitType` (vector),
`ByteType`/`ShortintType`/`IntType`/`LongintType`/`IntegerType`/`TimeType` (atom),
`RealType`/`ShortrealType`/`RealtimeType`/`StringType`/`ChandleType`/`EventType`,
`StructType`, `UnionType`, `EnumType`, `ImplicitType`(reworked, nameless),
`TypeOpExpr`/`TypeOpType`, `VoidType` ; `Declaration` (base) + `Var`, `Net`
(abstract) + `WireNet`/`TriNet`/`Tri0Net`/`Tri1Net`/`TriandNet`/`TriorNet`/
`TriregNet`/`WandNet`/`WorNet`/`UwireNet`/`Supply0Net`/`Supply1Net` +
`InterconnectNet`/`UserNet`/`ImplicitNet`,
`Port`, `Arg`, `Param`, `TypeParam`, `Member`, `NetTypeDecl` (user `nettype`),
(`Typedef`/`Genvar` kept but relifted). `Cast` split into
`TypeCast`/`SizeCast`/`SigningCast`/`ConstCast`. `Strength` (base) +
`DriveStrength`/`ChargeStrength` (net strengths). `ScopeName` (a `::` scope
segment, used by the scope list). `Call` (a `Call : Identifier { args }` base for
subroutine calls, cf. §5.12). (Advanced, §10: `Interface`/`Class`/`Program` +
`InterfaceType`/`ClassType`/`CovergroupType` + `Modport`/`Constraint`.)

**Modified (value side)**: `Identifier`, `FunctionCall`, `TaskCall` — their flat
`package` string → `scope: list(ScopeName)` (the SV `::` axis; cf. §3.3.1/§5.10,
handles nested `A::B::x` and parameterized `A#(8)::x`). `FunctionCall`/`TaskCall`
no longer carry their own `name`/`scope`/`args`: they **inherit `Call : Identifier`**
(§5.12), so a call *is* an invoked name ref. `Disable.dest`: `string` →
`Identifier` (hierarchical disable targets).

**Renamed (hierarchical axis, cf. §3.3.1)**: `IdentifierScope → HierName`,
`IdentifierScopeLabel → HierLabel`, and `Identifier.scope` (the hierarchical
child) → `Identifier.hier` — freeing `scope` to mean the SV `::` axis everywhere
(type and value).

**Dimensions**: `Width` + `Length` → **`Dimension`** (node per form:
`RangeDim`/`SizeDim`/`UnsizedDim`/`QueueDim`/`AssocDim`, cf. §3.8).

**Kept as-is**: most of the expression layer (operators, constants,
`Indirect`/`Partselect`/`Pointer`, `Concat`, `AssignmentPattern`, …) — *except*
the scope of value refs (see Modified) and `Cast` (split, see New); the statement
layer (Always/If/Case/For/Block/Assign/…), `Instance`/`PortArg`/`ParamArg`,
`Package`/`Import`, `Senslist`/`Sens`.

---

## 7. Benefits

- **One type = one representation**, everywhere. A new feature (interface, class,
  `type()`…) is added **once** in `DataType`, and every site benefits. No more
  "redoing it."
- **Zero empty names**: type descriptors no longer have a `name` slot.
- **Pure Verilog preserved** (implicit ⇒ `ImplicitType`, no `logic` injected).
- **Simpler passes**: uniform `decl.name` / `decl.type`; `var`/`const`/`direction`
  are properties; net-type and data-type dispatch by **node type** (the existing
  `is_node_category`/`cast_to` idiom) — no ad-hoc discriminator enums, no old
  conflated nodes.
- **Fidelity**: tri-state signing, explicit implicit → exact round-trip.
- **Resolution-aligned**: `NamedType`/`ImplicitType` + a single resolution pass
  (the current `CustomTypeVar` comment already promises it).

---

## 8. Migration & sequencing — big-bang, parser tests first, passes last

This is a *big-bang* on the AST core. We do **not** keep the old and new type
systems coexisting (the review flagged that coexistence as the main risk; we
remove the risk by removing the coexistence). The branch stays red between
batches — fine on a dedicated feature branch with no users.

1. **Freeze the spec** (this doc) — §9 decisions settled.
2. **Big-bang the schema.** Rewrite `verilog_ast.yaml` to the new
   `DataType`/`Declaration` model in one shot; regenerate. The whole tree goes
   red (parser, generators, importer, passes, every golden). Expected and
   accepted.
   - **Sort out the obsolete generated AST.** `astgen` does **not** delete files
     for nodes removed from the yaml: the stale per-node files (`wire.hpp/.cpp`,
     `tri.*`, `supply0.*`, `datamodifier.*`, `customtypevar.*`, `ioport.*`,
     `iodir.*`, `localparam.*`, `structmember.*`, `enumdef.*`, …) linger and the
     CMake `GLOB_RECURSE` would still compile them against removed base classes.
     Delete the obsolete generated files (and any hand-written code referencing
     the removed nodes), then reconfigure CMake.
3. **Parser green first, in batches.** Rewrite the bison rules + `ParserHelpers`
   to build the new AST, and bring the **parser tests** green **one coherent set
   at a time, one commit per batch**. Each batch = a group of parser testcases +
   their regenerated goldens. Suggested order:
   variables → nets → ports → parameters/localparams → function/task →
   struct/union/enum/typedef → cast. The generators (yaml/dot/verilog) and the
   importer are part of getting these round-trip/golden tests green.
4. **Passes last.** Only once the parser produces the new AST and **all parser
   tests are green**, fix the transformation/analysis passes (they consume the
   AST): one commit per pass-group (obfuscator, dimensions, flattener,
   io-normalizer, …). Their golden references regenerate against the new AST.
5. **Cosim at the very end.** The end-to-end cosim tests (parse → transform →
   emit → verilator) are fixed **last**, after parser + passes are green — the
   final acceptance gate.
6. **Resolution pass** (`ImplicitType`/`NamedType`/defaults) optional, when a
   consumer needs it.

Golden strategy: mechanical regeneration (the test `_parsed.yaml`), compared with
`is_equal(attrs=false)`. The churn is massive but expected and safe (1 type = 1
form).

> **SV-mode gating is unchanged.** The Verilog-vs-SystemVerilog distinction stays
> in the **lexer**: the scanner's `m_sv_mode` flag (set via
> `Parser::Verilog::set_sv_mode`, also on the preprocessor) gates the SV keywords
> (`logic`/`int`/`var`/`const`/`always_ff`/…). The new SV `DataType` nodes
> (`LogicType`/`IntType`/…, `Var`'s `is_var`/`is_const`, …) are therefore produced
> **only under `sv_mode`**; in Verilog-2005 mode those words remain identifiers.
> The big-bang touches the AST schema and the grammar actions, **not** this
> keyword-gating mechanism — it builds on it as-is.

> **Where constraints live — and the YAML-import gap.** Parse-time constraints
> (the grammar + parser semantic checks: `tri reg`, `const` without initializer,
> the `interconnect` restrictions, the net 4-state rule, …) are **not** enforced
> on an AST built by any *other* path. The **YAML importer** — and programmatic
> construction, and transformation passes — build nodes directly, bypassing every
> parser check, so a YAML-imported tree can be arbitrarily invalid. The
> node-per-type model makes **many** illegal states unrepresentable by
> construction (a `Var` has no `direction`; a `TypeCast.target` is always a
> `DataType`; net-type is a node, not a free enum), but a residual set stays
> representable-yet-illegal (e.g. an `InterconnectNet` carrying a `strength`). If
> we ever need to *guarantee* validity regardless of source, that calls for a
> dedicated **AST-invariant validation pass** runnable on any tree. Out of scope
> for the split itself — noted because the redesign neither introduces nor closes
> this gap, and the cleaner model makes such a validator much easier to write
> (fewer cases, all local).

---

## 9. Decisions — all settled

1. **Declaration: role nodes vs single fat node** — **SETTLED: B** (role nodes,
   veriparse idiom, illegal states impossible).
2. **`packed_dims`/`signing`: base vs intermediate** — **SETTLED: on the
   `DataType` base** (crossing applicability ⇒ no clean intermediate; uniform
   access). Built-ins as **node-per-type** (`LogicType`/`IntType`/…), no `kind`
   enum — consistent with `NamedType` and decision B.
3. **`Port`: wrapper vs flat vs property** — **SETTLED: (i) wrapper** `Port{
   direction, decl: Var|Net }` (DRY, distinct role, no `direction` inert
   elsewhere).
4. **Rename `CustomType → NamedType`** — **SETTLED: `NamedType`** (clarity).
5. **`signing`: tri-state vs bool** — **SETTLED: tri-state**
   `{NONE,SIGNED,UNSIGNED}` (round-trip fidelity; default resolved by a pass).
6. **Net types: `net_kind` enum vs node-per-type** — **SETTLED: node-per-net-type**
   (`Net` abstract → `WireNet`/`TriNet`/…/`Supply1Net`, + `InterconnectNet` /
   `UserNet` / `ImplicitNet`). Same criterion as `DataType`: the net family
   includes a *named* member (the user `nettype`), so it is **nodes**, not a
   `net_kind` enum — covers the 12 IEEE `net_type` keywords + `interconnect` +
   user nettype + the no-keyword default (`ImplicitNet`). `Module.default_nettype`
   stays an **enum** of net-type keywords + `NONE` (a config tag, not a node;
   removes the INTEGER/REAL/REG legacy).
7. **`lifetime` everywhere vs where allowed** — **SETTLED: (b) where allowed** —
   only `Var`/`Task`/`Function`/`Module`/`Package` (no common base; no
   uniform-access benefit to offset the inertness).
8. **`Cast`** — **SETTLED: node per form** `TypeCast{target:DataType}` /
   `SizeCast{size}` / `SigningCast{signing}`. Removes the heterogeneous slot;
   replaces the current `Cast{type,expr}` (PR #14).
9. **`EnumType.base`: `DataType` vs restricted enum** — **SETTLED: `DataType`**
   (handles packed + named bases).
10. **`Dimension`** — **SETTLED: node per form** (`RangeDim`/`SizeDim`/
    `UnsizedDim`/`QueueDim`/`AssocDim`), replaces `Width`/`Length`.

### Design points clarified (post-review)
- **Bound value naming**: `init` / `cont_assign` / `default` / `value` kept
  **distinct** (different semantics; not on the base).
- **`type()` operator**: split `TypeOpExpr` / `TypeOpType` (node per form).
- **`Net` continuous assign**: `cont_assign` (not `init`).
- **`Genvar`**: no type slot (always integer, compile-time).

---

## 10. Complete coverage — no out-of-scope

**No SV type/declaration construct is excluded from the model.** The
implementation (grammar) is phased, but the `DataType`/`Declaration` AST is
designed to host *everything*: adding a construct = **wire up the grammar + fill
an existing node**, never a redesign. Inventory of where each lives.

### Already in the model (only the parsing remains to wire up)
- `type(expr)` / `type(data_type)` — type operator → `TypeOpExpr` / `TypeOpType`
  (§3.6).
- `string` / `chandle` / `event` → `StringType` / `ChandleType` / `EventType` (§3).
- `[]` (dynamic) / `[$]` (queue) / `[type]` / `[*]` (associative) → `UnsizedDim` /
  `QueueDim` / `AssocDim` (§3.8).

### Interfaces / modports
- `Interface` — module-like definition (`name`, `params`, `ports`, `items`),
  alongside `Module`/`Package`.
- `Modport` — a named direction view inside an interface: `name` + list of
  `(port, direction)`.
- `InterfaceType { name, params, modport, is_virtual }` — a **`DataType`** variant
  (`params` = `parameter_value_assignment`, e.g. `virtual my_if#(8)`):
  - interface port `module m(my_if.mp i)` → `Port{ decl: Var{ type:
    InterfaceType(my_if, modport:mp) } }` ;
  - `virtual interface my_if v;` → `Var{ type: InterfaceType(my_if,
    is_virtual:true) } }`.

### Classes
- `Class` — module-like definition: `name`, `params`, `extends`(base + args),
  `items` (properties, methods, constraints).
- `ClassType { name, params }` — a **`DataType`** variant for handles: `MyClass
  obj;`, `MyCls#(int) o;`.
- **methods** = `Function`/`Task` (in `Class.items`) + class qualifiers.
- **properties** = `Var`/`Member` + class qualifiers.

### Class qualifiers / randomization (= properties, like const/var)
- access: `enum access { NONE, LOCAL, PROTECTED }` on members/methods.
- method: `is_virtual`, `is_pure`, `is_static` (bool).
- randomization: `enum rand_mode { NONE, RAND, RANDC }` on `Var`/`Member`.
- `Constraint { name, body }` — a constraint block in `Class.items`.

### Programs / other scopes
- `Program` (and `Checker`) — module-like definitions, same patterns as
  `Module`/`Interface`/`Class`/`Package`.

### User-defined nettypes, covergroup types, and the loose ends
- **`nettype`** (`nettype data_type my_net_t [with f];`) → a `NetTypeDecl{ name,
  type: DataType, resolver }` declaration. A net using it: `UserNet{ type:
  NamedType(my_net_t) }` (cf. §4/§5.2). Concretely it is a `Declaration` subtype
  (`name`/`type` from the base) with `resolver: Identifier` (the optional
  resolution-function ref, null absent). The node is realized **now** in the
  big-bang schema — small and fully specified, like `Strength`/`TypeOp*`/the
  `Dimension` variants — while its *grammar* is wired up later (§8 phasing).
- **`covergroup` as a type** (`ps_covergroup_identifier` in `data_type`) →
  a `CovergroupType { name }` `DataType` variant (covergroup *definition* is a
  module-like node when we get there).
- **`interconnect`** net → `InterconnectNet` (already in the Net hierarchy, §4).
- **explicit ANSI port** `module m(.p(expr))` (`[direction] . port_id ( [expr] )`)
  → `Port{ name:p, expr }` (a named connection, not a typed declaration).
- **typedef of an interface-instance member type** (`typedef intf.T myT;`) →
  `Typedef{ type: <interface-member type ref> }`. Rare.

> Principle: **advanced types → `DataType` variants**; **advanced declarations →
> existing `Declaration` roles + properties**; **definitions (interface/class/
> program) → module-like nodes**; **qualifiers (rand, local, virtual…) →
> properties**. Every addition is additive. The split big-bang is done **once**;
> after that, the types/declarations AST is never redone again.

---

## 11. Implementation order across the SV-support ADRs

ADR **numbers are chronological identifiers, not a build order** — they are never
renumbered. This section is the single authoritative **dependency / implementation
order** for the SV-support program; each ADR back-references it from its header.
Build top to bottom — each step is independently shippable and lands green.

| # | Step | ADR | Status | Depends on |
|---|---|---|---|---|
| 1 | **Type / Declaration split** — the `DataType`/`Declaration` node model everything else consumes | this ADR (0001) | **done** (PR #15) | — |
| 2 | **Package / import resolution** — `PackageInliner`, narrow; introduces the reusable `ScopeTable` (scope-build + name lookup) | [ADR-0004](adr-0004-package-import-resolution.md) | **done** | 1 |
| 3 | **Interface grammar** — `interface`/`modport`/interface ports/`virtual`; decisive cases emit `InterfaceType`, bare ports ride the `NamedType` path | [ADR-0002](adr-0002-systemverilog-interfaces.md) | design done; grammar TODO | 1 |
| 4 | **Broad name-resolution pass** — generalizes `ScopeTable` (step 2) to all scope kinds; resolves the deferrals catalogued in [ADR-0003](adr-0003-parser-ambiguity-deferred-resolution.md): `Call`→`Function/TaskCall`, `NamedType`→`InterfaceType`, `TypeCast`→`SizeCast`, `TypeOpExpr`→`TypeOpType`, scoped/imported names | [ADR-0003](adr-0003-parser-ambiguity-deferred-resolution.md) + [ADR-0004 §6](adr-0004-package-import-resolution.md) | TODO | 2 (seam), 3 (for interface-resolution) |

Notes:
- **[ADR-0003](adr-0003-parser-ambiguity-deferred-resolution.md)** is a *living
  catalogue*, not a single step: it records every parser deferral, and steps 2–4
  resolve those entries. Feature ADRs append to it (interfaces added §3.4/§4.4;
  package/import added §4.5).
- **Steps 2 and 3 are independent** (different subsystems) and may land in either
  order; both only need step 1.
- The **statement-call gap** (ADR-0003 §6 — grammar emits `TaskCall`, never the
  neutral `Call`) is folded into step 4 (or fixed standalone earlier — it is local).
