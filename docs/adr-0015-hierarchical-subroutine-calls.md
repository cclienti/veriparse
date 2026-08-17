# ADR-0015 — Hierarchical subroutine calls (`HierCallResolution`)

- **Status**: Draft (this branch). Unparks the TODO entry of 2026-08-17
  ("No hierarchical subroutine call") as a targeted exception to the
  ADR-0014 feature freeze, motivated by `verilower`: a handshake helper
  living with the bus it drives — a task declared in an interface, called
  through the interface port — is the firmware idiom the tool targets.
- **Date**: 2026-08-17
- **Scope**: two independent pieces, delivered together:
  1. **Parse** the hierarchical `tf_call` — `bus.ping()`, `bus.gv(x)`,
     `u.t()`, with or without an argument list — in both parse modes,
     building the neutral `AST::Call` (statement position) or
     `AST::FunctionCall` (expression position) with the path in the
     `hier` field that `AST::Call` has carried unused since ADR-0001.
  2. **Resolve** the calls whose root names an interface this module can
     see — an interface **port** or a local interface **instance** — by
     splicing the subroutine into the calling module, the same shape
     `PackageInliner` gives `pkg::t()`. Every other hierarchical call
     (`u.t()` into a module instance) is refused loudly, and parked (§8).
- **Normative reference** — IEEE 1800-2017, verified against
  `docs/1800-2017.pdf`; IEEE 1364-2005 against
  `docs/verilog-std-1364-2005.pdf`:
  - **1800-2017 §A.8.2** — `tf_call ::= ps_or_hierarchical_tf_identifier
    { attribute_instance } [ ( list_of_arguments ) ]` with
    `ps_or_hierarchical_tf_identifier ::= [ package_scope ] tf_identifier
    | hierarchical_tf_identifier`. The hierarchical form is what the
    grammar lacked; the package form is the one it had.
  - **1800-2017 §13.5** — tasks and void functions are called as
    statements; a nonvoid function call may be an operand. §13.4.1
    allows a nonvoid function as a statement, value discarded.
  - **1800-2017 §25.7** — *Tasks and functions in interfaces*: the
    abstraction this ADR serves ("'read' and 'write' can be defined as
    tasks, without reference to any wires, and the master module can
    merely call these tasks"). NOTE — ADR-0014 §6.3/§15 and the TODO
    entry cite this clause as §25.5; §25.5 is *Modports*. The citation
    is corrected here and rules relying on it are unchanged.
  - **1800-2017 §25.10** — access to interface objects by hierarchical
    reference or port reference; a modport restricts port-referenced
    access by listing what is accessible.
  - **1364-2005 §A.6.9** — `task_enable ::= hierarchical_task_identifier
    [ ( expression { , expression } ) ] ;` and **§A.8.2**
    `function_call ::= hierarchical_function_identifier { attribute_instance }
    ( expression { , expression } )` — the hierarchical call is plain
    Verilog too, so the grammar work is not SV-gated.
- **Relationship to prior ADRs**:
  - **ADR-0006 §4.1** put the task-vs-function re-tag in
    `NameResolution`, from the *declared kind of the callee*. A
    hierarchical callee's kind is not lexical knowledge, so
    `NameResolution` keeps its existing bail-out on `hier`; the re-tag
    duty **travels with resolution** (§3.4).
  - **ADR-0004 / `PackageInliner`** is the model: splice the definition
    into the referencing scope, clear the qualifier, let the existing
    machinery take over. §3 transplants that shape to the `.` axis.
  - **ADR-0008** (`InterfaceElaboration`) deliberately excluded
    subroutines from the member set ("not externally accessible", §8).
    This ADR supersedes that row — but by resolving calls *before* the
    flattener dissolves ports, so `InterfaceElaboration` itself is
    untouched (§5.2).
  - **ADR-0014 §6.3/§7.4/§15** — interface ports pass through the FSM
    pass, members are identities, tasks inline per call site. §5.1 shows
    the splice reduces `bus.ping()` to exactly those supported shapes;
    the FSM pipeline needs no new rule.

## 1. Problem — the call is the one missing piece

Declaring a task or function inside an interface parses and survives
every pipeline (ADR-0008 keeps them; the veriflat flattener even prefixes
and splices them correctly). What no grammar production accepts is any
call through a hierarchical name: `task_call` and `function_call` start
from a bare `TK_IDENTIFIER`, plus the `pkg::` scope form. So `bus.ping()`
through an interface port fails to parse, `u.t()` on a module instance
fails identically, and the only qualified call that works is the package
one — which is why a package task can feed an FSM (ADR-0014 §7.4) and an
interface task cannot.

`AST::Call` inherits `Identifier` and has carried a `hier` field since
ADR-0001 §3.3.1; nothing constructs it, and a survey of the passes shows
nothing consumes it beyond two bail-out guards (`NameResolution`,
`ExpressionEvaluation`) and the renderers, which already print
`scope`/`hier`/`name` uniformly for every `Identifier`-category node. The
AST and the generators are ready; the grammar and one resolution pass are
the whole feature.

## 2. Decision 1 — the grammar parses the standard's hierarchical call

`function_call` gains the `scope`-rooted forms (with and without
arguments), building `AST::FunctionCall` with `hier` set — expression
position is function-only, per §13.5. `task_call` gains the same two
plus the parenthesis-free enable (`bus.ping;`, 1364-2005 §A.6.9),
building the **neutral** `AST::Call` exactly as the bare-identifier
productions do: statement position is task-OR-function and the parser
does not resolve names (ADR-0003, ADR-0006).

The `scope` nonterminal is the existing `.`-path builder (`a.`, `a[i].`
segments), so indexed intermediate labels parse — `gen[0].bus.t()` is a
grammar sentence — and resolution, not the parser, decides what is
supported (§3.2). This mirrors how member references already work.

**Measured**: bison 3.8.2 with `-Wcounterexamples` reports **zero
conflicts** on the extended grammar, including the parenthesis-free
enable (the risky overlap: `a.b = x;` vs `a.b;` — distinct lookaheads,
LALR(1) decides). No parse-mode gate: both standards admit the form.

No AST, generator, or importer change: the fields, their YAML
serialization, and their rendering exist since ADR-0001.

## 3. Decision 2 — resolution stays with the definition holder

### 3.1 A new per-module pass: `HierCallResolution`

`NameResolution` resolves what lexical scope can see; a hierarchical
callee lives behind a type (the interface port) or an instance, so its
resolution belongs where the definition is in hand — the per-module
`ResolveModule` pipeline, which the flattener and verilower both run.
`HierCallResolution` takes the design's **interface dictionary** and
runs **unconditionally** between `GenerateRemoval` and
`ImplicitFsmElaboration`:

- after `GenerateRemoval`/`BranchSelection`, so a hierarchical call in a
  discarded generate branch or dead `if` never errors;
- after `LoopUnrolling`, so every cloned call site resolves to the same
  spliced definition (§3.3 makes that idempotent);
- before `ImplicitFsmElaboration`, so the FSM pass sees only shapes it
  already supports.

### 3.2 What resolves: a single, index-free root naming an interface

A call's `hier` must be **one label, not indexed**, and that root must
name, in this module:

- an **interface port** (`bus_if.dev bus` — the ADR-0014 §6.3 set), or
- a **local interface instance** (`bus_if bus();`).

The named subroutine must be a `Task` or `Function` declared in that
interface's body. Everything else is a hard error (§6): a multi-label
path, an indexed root (`bus[i].t()` — no static identity, the ADR-0014
§6.3 argument verbatim), a root naming a module instance (`u.t()`), or a
root naming nothing. The module-instance form stays parked (§8): it is
simulation-only Verilog with no synthesis story, and no pipeline of ours
has a consumer for it.

### 3.3 The splice — `PackageInliner`'s shape on the `.` axis

Per **(root, subroutine)** pair — once, however many call sites,
including sites `LoopUnrolling` cloned — the pass:

1. **clones** the declaration from the interface body, stamping the
   subroutine lifetime from the interface's own, the same freeze
   `PackageInliner` applies when a package item leaves home (a spliced
   item must not inherit the module's default lifetime);
2. **rewrites the body's free references to interface members** — the
   var/net-category names bound in the interface body — into
   hierarchical references rooted at the call's root: `req` becomes
   `bus.req`. Formals and block locals shadow members, as in any scope;
3. **names** the clone `<root>_<name>` (`bus_ping`), uniquified against
   the module's declaration set on collision;
4. **inserts** it at the front of the module's items, and
5. **rewrites the call site**: `hier` cleared, name set to the spliced
   name — and **re-tags** (§3.4).

The body's rewritten member references are exactly the references the
caller could have written itself: in verilower they are ADR-0014 §6.3
identities; in veriflat they are what `rewrite_port_refs` /
`replace_scoped_identifiers` already flatten. The splice invents no new
downstream shape.

**The v1 closure rule.** A body name that is neither a formal, a local,
an interface member, nor a system task/function does not resolve:
interface parameters, localparams, typedefs, enum items, and calls to
sibling interface subroutines are all hard errors naming the offending
identifier (§6). Each needs its own carry — a parameter needs the
*instance's* overrides, which a generic module with an interface port
does not have; a sibling call needs dependency closure — and none is
needed by the motivating idiom. Parked with that analysis (§8).

### 3.4 The re-tag travels with resolution

ADR-0006 §4.1 re-tags a neutral `Call` from the declared kind of the
callee; for a hierarchical call the declared kind is known exactly here,
so this pass performs the identical re-tag at rewrite time:

- callee is a `Task` → the call becomes `AST::TaskCall`. In expression
  position (the parser built `FunctionCall`) this is a hard error — a
  task has no value (§13.5).
- callee is a `Function` → `AST::FunctionCall`; in statement position
  the §13.4.1 discarded-value warning fires unless the return type is
  void — the same message `NameResolution` emits for the bare form.

`NameResolution` keeps its bail-out on `hier` unchanged: a warning-free
skip, because the call now has a resolver downstream.

## 4. Decision 3 — modport visibility is not enforced (documented)

§25.7 makes a modport grant subroutine access with `import` declarations;
ADR-0002 §7 keeps modport `import`/`export` (and `extern` prototypes)
out of the parsed subset, so a source spelling the legal grant cannot be
written. Refusing every call through a modport-qualified port would
refuse the idiom entirely; therefore v1 resolves subroutine calls
through any interface port **without a visibility check**, exactly as
ADR-0008 §15 already defers modport *direction* enforcement on members.
Both belong to the same future row: the day modports are resolved with
the definition in hand, `import` lists are the check to add. Member
visibility through modports (`InterfaceElaboration`'s existing §25.5
check) is unaffected.

## 5. Decision 4 — tool integration

### 5.1 verilower

`verilower` gains the interface dictionary veriflat already collects
(`Analysis::Module::get_interface_dictionary`) and hands it to
`ResolveModule`. Nothing else changes: after `HierCallResolution`, the
spliced `bus_ping` is an ordinary module task — `FsmTaskInliner` finds
it by name, the call-site block stem is `BUS_PING_0` under §10.1 naming,
the body's `bus.req` references are §6.3 member identities, commits to
them are member commits, and a spliced *function* is exactly a
module-level function, taking whatever rules those already have. The
ADR-0014 §6.3 rule "a hierarchical target must be a member of an
interface port" is untouched — the spliced body satisfies it by
construction for ports. For a **local instance** root the FSM pass's
existing checks decide, as they would for hand-written `bus.req`
references; the splice neither widens nor narrows that rule.

### 5.2 veriflat

`ModuleFlattener` already holds the interfaces map and already runs
`ResolveModule` on each module clone **before** the parent dissolves its
interface ports (`bind_interface_ports`). So by the time
`InterfaceElaboration` looks at the body, hierarchical calls are gone:
what remains is a bare call to a spliced local task plus `bus.member`
references — both shapes it flattens today (`rewrite_port_refs` retargets
the member's root onto the actual; `AnnotateDeclaration` prefixes the
spliced declaration and its call sites like any other declaration). Its
member check, which would have rejected `bus.ping` as "no member",
is never consulted for a call — and stays as-is for genuine mistakes
that reach it (§6). No change to `InterfaceElaboration`.

### 5.3 veriobf

Out of scope: the obfuscator is not taught hierarchical calls. A source
that parses only by this ADR's grammar is not a supported veriobf input
until the day that tool runs the resolution pass too (§8).

## 6. Errors — rejected loudly, never silently mis-lowered

Diagnostics name the construct and the fix, never an ADR (house rule);
IEEE citations allowed. The rows:

| Shape | Message names |
|---|---|
| root is a module instance or unknown | the root, and that only an interface port or local interface instance may carry a subroutine call |
| multi-label path `a.b.t()` | the path; one level is the supported form |
| indexed root `bus[i].t()` | the static-identity requirement (same wording family as the FSM §6.3 refusal) |
| callee not declared in the interface | interface name and subroutine name |
| callee is a task, call in expression position | §13.5: a task returns no value |
| nonvoid function as a statement | warning, value discarded (§13.4.1) — same as the bare-call path |
| body name outside the v1 closure (parameter, typedef, enum item, localparam, sibling subroutine) | the identifier and its kind; unsupported through a hierarchical call |

## 7. Validation

- **Parser goldens**: SV interface-port call (`bus.ping(x)`, `bus.gv()`
  in an expression), parenthesis-free enable, 1364 mode hierarchical
  enable, indexed/multi-label paths (parse-only — they must build the
  `hier` faithfully).
- **Pass goldens** (`HierCallResolution` alone): splice through a port;
  through a local instance; two call sites → one splice; formal
  shadowing a member; each §6 error row.
- **verilower**: FSM golden of a handshake task with a wait, called
  twice through the port (stems `X_0`/`X_1`), and a **differential
  cosim** in the `iface_line` mold — the Verilator reference compiles
  the source's interface call natively, the lowered output has none.
- **veriflat**: flatten golden — interface instance + two modules
  calling the same interface task through their ports; the flattened
  output holds two prefixed task copies with correctly retargeted
  member references.

## 8. Not supported (parked)

| Feature | Why parked |
|---|---|
| `u.t()` — subroutine call into a module instance | parses now (the grammar is general); refused at resolution. Simulation-only Verilog: flattening could splice it, but no consumer asked, and the FSM pass must keep refusing storage it does not own (ADR-0014 §6.3) |
| interface parameters / typedefs / enum items / localparams in a called body | needs the instance's parameter overrides or a declaration-closure carry; the generic-module-with-port case has no instance in hand. Splice the closure when a design asks |
| sibling-subroutine calls inside a called body | dependency closure, `PackageInliner::copy_symbol`'s recursion on the `.` axis — mechanical once wanted |
| modport `import`/`export`, `extern` prototypes, `extern forkjoin` (§25.7) | not in the parsed subset (ADR-0002 §7); visibility enforcement lands with modport resolution (§4) |
| virtual interfaces | dynamic binding; with ADR-0008's row |
| named/default arguments on hierarchical calls | same call-grammar row as ADR-0014 §15 |
| veriobf over hierarchical calls | the obfuscator does not run the resolution pass (§5.3) |
