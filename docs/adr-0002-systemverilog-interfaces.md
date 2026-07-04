# ADR-0002 — SystemVerilog interfaces (definition, ports, modports, virtual)

- **Status**: Accepted — **implemented** (nodes + scanner + the full §6 grammar
  phasing, with parser/generator round-trip golden tests). The two deferred
  resolutions (§3: instance kind, bare interface port) await the
  interface-resolution pass (§5 / roadmap step 4), and the §7 features remain
  out of scope.
- **Date**: 2026-06-16
- **Scope**: The **core** interface feature set — interface *definition*, interface
  *ports* on modules, *modports* (simple direction views), *modport-qualified*
  ports, *virtual* interfaces, *parameterized* interfaces, and interface *arrays*.
  Explicitly **deferred** to a later ADR (§7): clocking blocks (IEEE §14),
  modport *import/export* of tasks/functions (§25.5/§25.7), and *generic* interface
  ports (`module m(interface i)`).
- **Normative reference** — IEEE 1800-2017, **clause 25 (Interfaces)**, verified
  against `docs/1800-2017.pdf`:
  - §25.3 Interface syntax · §25.4 Ports in interfaces · §25.5 Modports ·
    §25.8 Parameterized interfaces · §25.9 Virtual interfaces ·
    §25.10 Access to interface objects.
- **Relationship to prior ADRs**:
  - **ADR-0001 §10** pre-allocated the three core nodes (`Interface`, `Modport`,
    `InterfaceType` as a `DataType`). This ADR makes them concrete and adds the
    instance-resolution node.
  - **ADR-0003** gains **two new deferred-resolution entries** (instance target
    kind; interface-vs-named-type port) — recorded in that ADR's §4 and §7 table.
  - **Implementation order**: step 3 of the SV-support roadmap — see
    [ADR-0001 §11](adr-0001-type-declaration-split.md).

---

## 1. Principle

An interface is, for the type/declaration model, **two things at once**:

1. a **module-like definition** (`interface … endinterface`) — it has params,
   ports, and a body of items, exactly like `Module`/`Package`; and
2. a **type** — a port or variable can be "of interface `my_if`" (optionally
   restricted to a modport, optionally `virtual`). That is a `DataType`
   (`InterfaceType`), per ADR-0001's "advanced types → `DataType` variants".

The recurring difficulty is that **an interface name is not syntactically
distinguishable** from a module name (in instance position) or a user type name
(in port-type position). True to ADR-0003, the parser commits only where the
syntax is decisive and defers the rest to a resolution pass.

---

## 2. The nodes

### 2.1 `Interface` — the definition (module-like)

Mirrors `Module` (`verilog_ast.yaml:61`), minus nothing essential:

```yaml
Interface:
  properties:
    name: "std::string"
    lifetime: enum(NONE, AUTOMATIC, STATIC)   # `interface automatic …`
    default_nettype: enum(WIRE, …, NONE)      # interfaces may declare nets
  children:
    params: list(Declaration)   # #(...) Param | TypeParam
    ports:  list(Port)          # interface header ports (an interface has ports too)
    items:  list(Node)          # body: nets, vars, Modport, instances, always, …
```

`Modport` nodes live in `items` (a modport is a body item, §25.5). Same shape as
`Module` so every pass that walks module bodies walks interface bodies unchanged.

> **astgen limitation — the duplicated `default_nettype` enum.** astgen generates
> enums **per node** (`AST::Module::Default_nettypeEnum`,
> `AST::Interface::Default_nettypeEnum`); it has no shared/free-standing enum
> support. So `Interface.default_nettype` re-declares the identical 13-value
> net-type list, and the parser bridges the two with a hand-written switch
> (`ParserHelpers::to_interface_nettype`). The three copies (Module schema,
> Interface schema, bridge) must be kept in sync: a value added to only one
> falls through the bridge to `NONE`. Low risk in practice — the IEEE net-type
> keyword set is closed — but the honest fix is shared-enum support in astgen,
> not a third workaround site.

### 2.2 `Modport` — a named direction view (§25.5)

ADR-0001 §10: "a `name` + list of `(port, direction)`". Core = **simple** modport
ports only:

```yaml
Modport:
  properties:
    name: "std::string"
  children:
    ports: list(ModportPort)

ModportPort:
  properties:
    name: "std::string"      # a signal declared in the interface
    direction: enum(INPUT, OUTPUT, INOUT, REF)
```

> **Deferred (§7)**: modport *expressions* (`.a(expr)`, §25.5.4) and modport
> *import/export* of subroutines (§25.5/§25.7). Both are additive: an
> import/export entry becomes a future `ModportImport`/`ModportExport` item in
> `Modport.ports`; a modport expression adds an `expr` child to `ModportPort`.
> Nothing here is redesigned to add them.

### 2.3 `InterfaceType` — the type form (a `DataType`, §25.4/§25.9)

```yaml
InterfaceType:
  inherit: DataType
  properties:
    name: "std::string"       # the interface name
    modport: "std::string"    # restricting modport, "" if none
    is_virtual: "bool"        # the `virtual` keyword (§25.9)
  children:
    params: list(ParamArg)    # #(...) value assignment, e.g. virtual my_if#(8)
```

Used for:
- **modport-qualified port** `module m(my_if.mp i)` →
  `Port{ decl: Arg{ type: InterfaceType{ name:"my_if", modport:"mp" } } }`;
- **virtual interface** `virtual my_if v;` →
  `Var{ type: InterfaceType{ name:"my_if", is_virtual:true } }`;
- **parameterized** `virtual my_if#(8) v;` → the `params` child carries `#(8)`.

**Parse-time vs deferred** (this is the ADR-0003 boundary, §3 below):
`InterfaceType` is emitted by the parser **only when the syntax is decisive** —
i.e. a `virtual` keyword, or a `.modport` suffix in port position. A *bare*
`module m(my_if i)` is **not** decisive (could be a `typedef`) and is parsed as
`NamedType{my_if}`, promoted to `InterfaceType` later.

### 2.4 `InterfaceInstance` — the resolved instance tag (Decision 2)

`my_if u(...)` is syntactically a module instantiation. The parser keeps emitting
the neutral `Instancelist`/`Instance` (`verilog_ast.yaml:751,758`) with the target
held as the `module` string. A resolution pass that knows `my_if` is an interface
**rewrites the neutral `Instance` to `InterfaceInstance`** — a pure tag, exactly
like `Call → TaskCall`:

```yaml
InterfaceInstance:
  inherit: Instance        # name/module/array/parameterlist/portlist unchanged
```

The neutral `Instance.module` (and `Instancelist.module`) is the shared target the
pass classifies; rendering is identical, so the rewrite round-trips. (A
`ProgramInstance` peer is the same future move when programs land — not in scope
here.)

---

## 3. Construct-by-construct mapping

| Source | Parser emits | Resolved form | Decisive at parse? |
|---|---|---|---|
| `interface my_if; … endinterface` | `Interface{…}` | — | yes |
| `modport mp(input a, output b);` | `Modport{ ports:[…] }` | — | yes |
| `module m(my_if.mp i)` | `Port{ decl: Arg{ type: InterfaceType{name,modport} } }` | — | **yes** (`.mp` is decisive — Decision 4) |
| `module m(my_if i)` | `Port{ decl: Arg{ type: NamedType{my_if} } }` | `… InterfaceType{my_if}` | **no** — deferred (Decision 3) |
| `virtual my_if v;` | `Var{ type: InterfaceType{is_virtual:true} }` | — | **yes** (`virtual` is decisive) |
| `virtual my_if#(8) v;` | `… InterfaceType{ params:[#(8)] }` | — | yes |
| `my_if u(...)` (instance) | `Instance{ module:"my_if" }` (neutral) | `InterfaceInstance` | **no** — deferred (Decision 2) |
| `my_if u[4](...)` (array) | `Instance{ array: Dimension }` | `InterfaceInstance` | array decisive; kind deferred |

The two **deferred** rows are exactly the new ADR-0003 entries (§4 there).

### 3.1 Bare-port deferral vs §23.2.2.3 direction inheritance (implemented nuance)

The bare row above holds only where no ANSI direction is inheritable. IEEE
1800-2017 §23.2.2.3 says a subsequent ANSI port with a data type but **no
direction inherits the previous port's direction** — so in
`module m(input logic a, my_t b)` the port `b` is a legal *directed* named-type
port (`input my_t b`), and emitting the directionless `Arg` form there would
silently drop the direction. `create_ports_decls` therefore splits on context:

- **`.modport` written** → decisive `InterfaceType` port, always (interface
  ports take no direction; the suffix cannot be a typedef port).
- **Bare named type, no preceding direction** (first port, or following another
  directionless typed port) → the deferred `Arg{ NamedType }` form of the table;
  a bare name after such a port inherits the same form (`my_if i0, i1`).
- **Bare named type after a directional port** → a normal named-type port with
  the **inherited direction** (`Port{ dir, decl: ImplicitNet{ NamedType } }`).
  If the name later resolves to an interface, the resolution pass rewrites it —
  the `NamedType` is preserved, so no information is lost; this is the
  conservative reading because the §23.2.2.3 inheritance rule is the
  standard-mandated interpretation of the *typed-port* syntax, while the
  interface reading remains recoverable.

---

## 4. Why the deferrals are unavoidable (not laziness)

- **Instance kind** (`my_if u(...)`): IEEE §25.3 gives interface instantiation the
  *same* `module_instantiation` syntax as a module. The definition of `my_if` may
  be in another file or an imported package — unknowable to a single-pass,
  symbol-table-free grammar. Committing to `InterfaceInstance` at parse time would
  require the very parse-time symbol table the architecture rejects (ADR-0003 §1).
- **Bare interface port** (`module m(my_if i)`): identical syntax to a named-type
  port `module m(my_type i)`. §25.4 vs a `typedef` port are disambiguated only by
  what `my_if` resolves to. Hence `NamedType` now, promote later — the same
  pattern as `TypeCast` covering `SizeCast` (ADR-0003 §3.2).

What **is** decisive and resolved now (Decision 4): the `.modport` suffix and the
`virtual` keyword are interface-only syntax, so `my_if.mp i` and `virtual my_if v`
go straight to `InterfaceType`.

---

## 5. Passes affected

- **New: interface-resolution pass** (the pass ADR-0003 calls for):
  - tag neutral `Instance`/`Instancelist` whose `module` is an interface →
    `InterfaceInstance`;
  - promote port/var `NamedType{X}` → `InterfaceType{X}` when `X` resolves to an
    interface (carry over any modport/virtual already set — they won't be, by
    construction, since those cases were decisive at parse time).
- **Existing body-walking passes** (io-normalizer, dimensions, flattener, …) need
  the `Interface` body shape = `Module` body shape (§2.1) so they require **no
  interface-specific code** to traverse an interface; only elaboration/flattening
  that *connects through* an interface needs new logic (out of scope here).

### 5.1 Known permissiveness — modport placement is not enforced (deferred)

Because the interface body reuses the shared module item grammar (§2.1), the
grammar accepts a `modport` in **any** body that uses those items — including a
plain `module`, where §25.5 forbids it (previously a parse error, since
`modport` was not a keyword). This is the project's standard permissive-parser
stance (cf. ADR-0005 §2.2/§3.6: placement rules are semantic checks, not
grammar): rejecting it would mean forking the item grammar per container for
one construct. **Deferred**: the §25.5 placement check ("modport only inside an
interface") belongs to the future validation / interface-resolution pass
(roadmap step 4), which must reject a `Modport` whose enclosing definition is
not an `Interface`.

---

## 6. Grammar phasing (per ADR-0001 §8)

The AST above is the contract; grammar wiring is incremental and lands green at
each step:

1. `interface … endinterface` definition + body reuse → `Interface`.
2. `modport` declaration → `Modport`/`ModportPort`.
3. interface ports on modules: decisive `my_if.mp i` → `InterfaceType`; bare
   `my_if i` rides the existing `NamedType` port path (no new grammar).
4. `virtual` interface type → `InterfaceType{is_virtual}`.
5. parameterized + arrays reuse the existing `params`/`array` machinery.

No step touches the type/declaration core; each is "wire grammar + fill an
existing node" (ADR-0001 §10).

---

## 7. Deferred to a later ADR

| Feature | Clause | Why deferred | Future home (additive) |
|---|---|---|---|
| Clocking blocks | §14 | Orthogonal (event/timing), own complexity | `Clocking` body item |
| Modport import/export methods | §25.5, §25.7 | Pulls in subroutine prototypes | `ModportImport`/`ModportExport` in `Modport.ports` |
| Modport expressions `.a(expr)` | §25.5.4 | Rare | `expr` child on `ModportPort` |
| Generic interface port `interface i` | §25.3 | Resolves only at elaboration | `GenericInterfaceType` / flag on `Port` |

> Principle (ADR-0001 §10 restated): interfaces are **module-like definitions +
> a `DataType` variant + existing declaration roles**. Every deferred feature is
> an additive node or field — the model is not redone to admit them.
