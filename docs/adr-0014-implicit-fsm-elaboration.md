# ADR-0014 — Implicit-FSM elaboration (`ImplicitFsmElaboration`)

- **Status**: Proposed (2026-08-01, this branch — nothing implemented yet;
  §12 phases each land green).
- **Date**: 2026-08-01
- **Origin**: This ADR formalises `docs/imperative-fsm-design.md`, the
  design note that groomed the approach against two RS232 examples. That
  note's decisions carry over unless stated otherwise; §3 records the one
  it reverses (attributes), and §7.2 settles the bounded-loop question the
  note left whole: unroll uniformly, `veriparse_no_unroll` to keep a loop
  rolled.
- **Scope**: A per-module pass that compiles a **multi-cycle imperative
  process** — an `initial` body carrying its own `@(posedge clk)` controls,
  the style a firmware engineer bit-banging a GPIO would write —
  into an explicit synthesizable **FSMD** (finite state machine with
  datapath): a state register, an `always_ff`, and the per-state actions.
  The input is *already scheduled*: the author placed the edge waits, so
  the pass performs no operation scheduling, resource allocation or
  pipelining. Explicitly **not** in scope: general HLS (§15), multi-clock
  processes, `fork`/`join`, datapath generation, and memory inference
  policy.
- **Home**: a new tool, `verilower`, not `veriflat`. The transformation
  compiles behaviour into structure, which is a different job from
  flattening a hierarchy, and §13 records that it is the first of a family
  — the CFG construction and dataflow checks below are generic, only the
  RTL emission is FSM-specific. The two tools stay separate and **chain**:
  verilower's source-to-source output is a first-class deliverable,
  reviewed and simulated on its own, and `veriflat` consumes it like any
  source. `veriflat --fsm` runs the pass in-process besides — cheaply,
  precisely because §10's default emission is safe in that position too —
  and §10 says why both roads exist.
- **Hard constraint, inherited unchanged**: the source must run as-is in
  any conforming simulator. Anything that would require veriparse to run
  before a behavioural simulation is rejected — this rules out a custom
  block keyword and any "one statement = one cycle" scheme.
- **Normative reference** — IEEE 1800-2017, verified against
  `docs/1800-2017.pdf`. No PDF is committed: the standards and the §C.7
  papers alike are third-party and redistribution-restricted, so `.gitignore`
  refuses them all and a reader supplies their own copies. What the repository
  owes them instead is identification precise enough to fetch — a full
  citation and a DOI per entry (§C.7) — which is why the clause list below
  and §C.7's records are written out rather than left as file names. The
  `docs/imperative-fsm-design.md` note this ADR derives from is *not* a
  publication and is tracked. The clauses used:
  - **§9.2.1** — *"An initial procedure shall execute only once, and its
    activity shall cease when the statement has finished."*
  - **§9.2.2** — *"All forms of always procedures repeat continuously
    throughout the duration of the simulation."*
  - **§9.2.2.1** — *"The general purpose always procedure, because of its
    looping nature, is only useful when used in conjunction with some form
    of timing control. If an always procedure has no control for
    simulation time to advance, it will create a simulation deadlock
    condition."* This is what makes the input form legal and well-defined,
    and it is also the source of the §9 deadlock rejection.
  - **§9.4.2** — event control; *"A posedge shall be detected on the
    transition from 0 to x, z, or 1, and from x or z to 1"*.
  - **§9.4.2.3** — conditional event controls, the `iff` form §5.3 uses to
    carry a chip enable.
  - **§9.3.4** — block names, which §10.1 uses to name states.
  - **§10.4.1 / §10.4.2** — blocking vs nonblocking procedural
    assignments; §6 turns their difference into the storage decision.
  - **§11.6** — expression bit lengths: the context-width rules §6.1's
    materialization test is built on.
  - **§13.3 / §13.4** — tasks and functions: §13.4 forbids time-controlled
    statements in a `function`, so a function can never hold a cut point
    and §9 accepts pure calls on that guarantee; §13.3's argument
    copy-in/out is what gives a multi-cycle task's arguments their
    capture-at-entry meaning when §15's inlining arrives.
  - **§12.7** — loop statements (`for`, `while`, `repeat`, `forever`);
    §12.7.2 evaluates a `repeat` count once, on entry — §7.2's rolled
    countdown implements exactly that.
  - **§5.12** — attributes: *"A mechanism is included for specifying
    properties about objects, statements, and groups of statements in the
    SystemVerilog source that can be used by various tools… without
    standardizing on any particular attributes."* §3 rests on this.
- **Relationship to prior ADRs**:
  - **ADR-0007** owns the synthesizable-subset verdict. This ADR does not
    widen that subset: it *translates* a process out of the unsynthesizable
    region into it, and §9 rejects what it cannot translate. The output
    must satisfy ADR-0007 unchanged.

    **`verilower` therefore runs `SynthesizableCheck` on its output, never on
    its input**, and that is not a detail: the input is a process suspending
    on edge waits, which is precisely what the subset excludes — §1's whole
    premise. Running the check first would reject every design this tool
    exists for. ADR-0007 §1.2 already makes the pass a per-tool opt-in
    (`veriflat` runs it, `veridump`/`veriobf` do not); `verilower` runs it,
    after this pass.

    §9's table is consequently **not** a second copy of ADR-0007's blacklist.
    It answers a different question — *can this marked process be compiled?*
    — at the one point where `SynthesizableCheck` cannot be asked. Where a
    row names something non-synthesizable anywhere (`#delay`, `fork`, a
    system task in a design file), the general verdict stays ADR-0007's and
    grows there additively per its §2.1; this ADR claims only the marked
    process.
  - **ADR-0005 §3.2.1** deferred mixed/nested jump lowering behind a flag
    scheme. `break`/`continue` are nevertheless **in scope here** (§8):
    once states are explicit a jump is an edge in the CFG, which is
    strictly easier than that flag lowering and needs none of it.
  - **ADR-0011** (`StructLowering`) and **ADR-0012**
    (`DefaultResolution`) run before this pass, so every variable §6
    classifies already has an explicit type, net kind, direction and a
    defined bit layout.

    **The two do not arrive by the same route, and `verilower` has to supply
    the second itself.** `StructLowering` is inside `ResolveModule`, so it
    comes free with the pipeline of §10.3. `DefaultResolution` is not: it is
    a whole-design pass invoked from the driver
    (`apps/veriparse/veriflat/src/main.cpp`, `run_design`), alongside
    `NameResolution`. A new tool that only calls `ResolveModule` would run
    this pass over variables that still have no resolved default net kind or
    direction, and §6's classification would be reading fields nobody had
    filled in. `verilower`'s driver must reproduce veriflat's design-level
    preamble before descending into modules; phase 2 lands that driver, not
    just the pass.

## 1. Problem — the scheduled description is not synthesizable, and the RTL is not simulable against it

Writing FSMs by hand is the most tedious part of RTL work: state register,
next-state logic, output decode, all kept in sync manually. C/C++ HLS
solves it but lives outside the Verilog universe — no direct access to
surrounding signals, ceremony at every boundary.

Sequential control logic is naturally written as a *program over time*:

```systemverilog
(* veriparse_fsm *)
initial begin
  done <= 1'b0;
  acc  <= 8'd0;
  @(posedge clk);
  forever begin
    while (!start) @(posedge clk);
    acc <= 8'd1;
    @(posedge clk);
    acc <= acc + 8'd2;
    @(posedge clk);
    done <= 1'b1;
    @(posedge clk);
  end
end
```

This is legal SystemVerilog (§9.2.2.1), simulates anywhere, and reads in
program order. It is also not synthesizable: no flow accepts a process
that suspends on an edge mid-body.

Hand-translating it into an explicit FSM is mechanical, it is where
sequential bugs are actually introduced (an action one cycle early, a
variable that should have been registered), and it costs the property that
makes the behavioural form valuable: **the source stops being the
specification**. The two descriptions drift and nothing checks that they
agree.

> **Appendix A** carries a full RS232 transmitter through the whole
> transformation, source and generated RTL side by side, and **Appendix B**
> adds an I2C byte write for what a second protocol exercises. Reading
> those first is the shortest route into the rules below.

## 2. Decision 1 — the author marks the process; the construct says which kind

`docs/imperative-fsm-design.md` made the construct itself the signal: an
`always` or `initial` carrying edge waits *is* an FSM, no annotation. That
is reversed here, because the shape is not exclusive to hardware intent —
it is also the shape of every testbench ever written:

```systemverilog
initial begin
  cnt = 0;
  repeat (8) @(posedge clk);        // an Initial, EventStatements,
  $display(...); $finish;           // one edge, one signal
end
```

Under a shape-only rule that is a schedulable process. §9 rejects the
system tasks, so *this* block would at least fail loudly — but it would
fail loudly on a file the tool was never asked to touch, which is its own
defect. And the stimulus generator beside it that drives its signals with
plain assignments between edge waits carries nothing for §9 to catch:
`verilower` would compile it into an `always_ff`.
And the hazard is sharpest for exactly the construct this ADR compiles:
nearly every `initial` with edge waits in existence is testbench.

It is also, plainly, the rule §3 asks for applied to itself: *deciding
whether a block should become hardware* is the largest guess in this
design, and exempting it from §3's second rule was inconsistent.

> **A process is compiled only if it carries `(* veriparse_fsm *)`.**
> Anything unmarked is left exactly as it is.

The marker is namespaced by the **project**, not by the tool: it lives in
the user's RTL, which is the most expensive place to churn a name, while
`verilower` is a binary name and the least stable identifier in the system.
The pass lives in the library and nothing stops another front end invoking
it. This is Verilator's convention (`/* verilator lint_off */`) rather
than a per-executable one.

Requiring it costs nothing on the hard constraint: §5.12 attributes are
ignored by any tool that does not recognise them, so the source still runs
as-is, marked or not.

**The construct still carries meaning — just not the opt-in.** Within a
marked process it decides the terminal behaviour, read off §9.2.1/§9.2.2
rather than invented:

| **Marked construct** | **Meaning** | **Terminal behaviour** | **Reset** |
|---|---|---|---|
| `(* veriparse_fsm *) initial begin … @…; forever begin … end end` — **canonical** | perpetual FSM | the `forever` back-edge (§7.3) | preamble is the reset branch; every register it writes is reset (§5.1) |
| `(* veriparse_fsm *) initial begin @… @… end` | one-shot sequence | the last segment enters an added empty **hold state** — a self-loop on the segment itself would replay its action every cycle, where §9.2.1 says the activity has ceased | as above |

**`initial` is the only accepted construct**, and `always` is refused rather
than supported: `always begin BODY end` is exactly `initial forever begin
BODY end` minus the one-time region — two spellings of one thing, one of
them lossy. §15.1 keeps the full argument and the measurement behind it.

The old objection to `initial`, that it reads as testbench, is answered by
the mark rather than by the keyword: nothing is compiled without
`(* veriparse_fsm *)`, so the construct no longer carries that risk on its
own.

**What the `forever` is, and what it is not.** It is only the back-edge:
§7.3 lowers `forever` and a `while` whose condition never folds false to
the same thing — checked, `forever` and `while (1)` produce the same
machine. Nothing rests on the keyword.

What the canonical form actually buys is `initial`'s **one-time region** —
everything above the loop: the reset values above the first wait, then any
number of one-shot startup states, each run exactly once (§5.1 walks the
boundary). It is what `always` cannot express — its preamble runs on every
lap — and the reason §15 refuses the construct rather than merely
discouraging it.

Dropping the loop entirely is legal and is the one-shot row above: without a
back-edge, §9.2.1 ends the process and the machine parks in the hold state.

A marked module item is a **schedulable process** when it is an `Initial`
whose body contains at least one
`EventStatement`, and where every `EventStatement` carries exactly one
`Sens` of the same edge — `posedge` or `negedge`, never a level sensitivity
(`@(sig)`) — over the same single-bit signal, and, when
present, the same `iff` condition on that `Sens`, which is how a chip enable
is written (§5.3). One clock,
one edge, per process; §9 rejects the rest, and a *marked* process that
fails any of these is an error rather than a silent skip — the mark says
the author meant it. So is a mark on anything that cannot be a schedulable
process at all: a module item that is not a process, or an `initial` with
no wait. A module may hold several schedulable processes plus
ordinary RTL; each yields its own independent state register and
`always_ff`.

**No `Always` is ever a candidate**, and the two reasons differ. One with a
**non-empty** `senslist` and no `EventStatement` (`always @(posedge clk)`,
`always_ff`, `always_comb`) is already RTL, left untouched: this
pass does not rewrite existing hardware descriptions. Marked anyway, it
draws a **warning** — *"the mark has no effect: this process is already
RTL"* — and stays untouched. It is the one misplaced mark that warns
rather than errors, because it is the one where nothing is lost: the
process means the same thing with or without the tool, so the author needs
the signal, not a broken build. One with waits in its
body is refused instead — an empty senslist because §15 takes the construct
out of the language this pass reads, and a non-empty one because it is
neither RTL nor compilable, `always @(posedge clk or negedge rst_n) begin …
@(posedge clk); … end` being the shape anyone reaches for on being told the
reset is not in the source, which §5.2 shows cannot work.

Both are errors when marked, never silent skips, and both messages carry the
rewrite: wrap the body in `initial forever` and, for the second, drop the
senslist. Falling through a marked process in silence would leave the author
with no signal at all.

**Discoverability.** An unmarked process of schedulable shape gets one
**informational** note — *"looks like an implicit FSM; mark it with
`(* veriparse_fsm *)` to compile it"* — so a first user is not left
wondering why nothing happened. Info, never a warning: pointing the tool
at a file that also holds a testbench must stay quiet.

## 3. Decision 2 — attributes are optional hints, never load-bearing

`docs/imperative-fsm-design.md` rejected attributes on the grounds of
"zero new syntax to teach" and of keeping the source consumable without
running veriparse first. **That reasoning is reversed here**, because
neither premise holds against §5.12: an attribute *is* standard
SystemVerilog, ignored by any tool that does not recognise it — so
behavioural simulation is untouched and the hard constraint above is fully
preserved — and the mechanism exists in the standard precisely to carry
tool-specific hints. It is also not new syntax to the audience: Vivado,
Quartus and Yosys already steer synthesis this way (`fsm_encoding`,
`ram_style`, `async_reg`).

Every attribute this ADR defines is prefixed `veriparse_`, and
`fsm_encoding` is exactly why: Vivado and Quartus both already define that
name with their own values. An unprefixed `fsm_encoding` here would either
be reinterpreted by them or silently pick up a value meant for them. The
prefix is on the *hints* as well as on the §2 marker, because a convention
that namespaces only some of them leaves a reader unable to tell which are
ours.

The `fsm_` infix is deliberately **not** repeated in the hints: the marker
already says which transformation is being asked for, and §13 expects
others. `veriparse_reset` will mean something to a pipeline lowering;
`veriparse_fsm_reset` would not.

§2's `veriparse_fsm` is a different kind of thing from what follows: it is
the **opt-in**, deciding whether a process is compiled at all. The rules
below govern the **hints** — attributes that steer a compilation the author
has already asked for.

The rule that keeps the note's intent intact is therefore not "no
attributes" but two:

> **1. No attribute is load-bearing for correctness.** The pass must
> produce correct RTL with none of them present. None may change
> behaviour — only implementation choices the source does not express.
>
> **2. Where the pass would have to guess, it asks.** Any heuristic whose
> answer is not obviously right becomes an attribute rather than a
> default. A wrong guess made silently is the failure mode this project
> keeps paying for; an attribute turns it into a decision the author took.

The second rule is why the reset signal is hinted rather than divined
(§5), why rolled-versus-unrolled on a bounded loop is the author's call,
written `veriparse_no_unroll` rather than guessed (§7.2), and why state
names are written rather than generated (§10.1). It
also sets the bar for anything added later: a new heuristic ships as an
attribute first, and only becomes a default once there is evidence the
default is right.

Binding is **structural, not positional**: the parser nests the attributed
item inside `Pragmalist.statements`, so a schedulable process carries its
hints by being that list's element. The generator already round-trips the
form (`render_pragmalist` emits `(* … *)` when the statement list is
non-empty), so nothing is lost on output.

| **Attribute** | **Effect** | **Default when absent** |
|---|---|---|
| `veriparse_fsm` (§2, the opt-in, not a hint) | compiles this process | the process is left untouched |
| `veriparse_reset = "<port>"` | names the reset input | inferred (§5) |
| `veriparse_reset_level = 0\|1` | active level | `1`, or `0` when the name ends in `_n` |
| `veriparse_reset_kind = "sync"\|"async"` | reset flavour of the generated `always_ff` | `"sync"` |
| `veriparse_encoding = "binary"\|"one_hot"\|"gray"` | state encoding | `"binary"` |
| `veriparse_prefix = "<id>"` | prefix for generated declarations; must be **distinct** across a module's marked processes — a hint colliding with another process's hint or ordinal is rejected where the prefix is assigned | `__fsm` (§10), `__fsm<N>` when several processes are marked |
| `veriparse_no_unroll` on a loop | keeps the loop rolled: one state group with an induced countdown or index register (§7.2) | bounded loops unroll |

The three reset hints govern the whole reset branch, which under §5 is the
init segment plus the state register.

## 4. Decision 3 — the state model: cut at every `EventStatement`

The target is a **FSMD** — a finite state machine with datapath: control
states, primary inputs, storage variables, primary outputs. That is the
standard model for what this pass produces, and §11 leans on it, so the
vocabulary below is the one that literature uses rather than one invented
here.

Flatten the process body into a control-flow graph whose **cut points** are
exactly the `EventStatement` nodes. A **path** between two cut points with
no intervening cut point is what that literature calls a path of the FSMD's
*path cover*; this ADR calls it a **segment**, and the two are the same
thing.

- A **segment** is a maximal run of statements between two consecutive cut
  points: the action of one state, executed in one clock cycle.
- An `EventStatement` means *everything preceding it in program order has
  taken effect in the current state; execution resumes in the successor
  segment after the edge.* It emits no logic — it is consumed as the
  transition.
- The statements **before the first** cut point form the **init segment**,
  which is not an ordinary state: §5 turns it into the reset branch.
- `IfStatement`/`CaseStatement` fork the graph; loops close a back-edge
  (§7); `break`/`continue` are edges (§8). A branch whose arms contain no
  cut point stays a plain conditional inside one segment's action and
  costs no state.
- Branches of unequal length are **not padded**: each takes the cycles it
  needs and the last state of each arm transitions to the same merge state.

**The dispatch idiom.** A `case` whose selector is a register the process
itself steers is legal input and needs no rule beyond the ones above:

```systemverilog
forever begin
  case (state)                    // dispatch on the ENTRY value (§6.1)
    IDLE: begin … state <= S0; end
    S0:   begin …                 // arms may hold cut points and
            @(posedge clk);       // take several cycles
            … state <= IDLE;
          end
  endcase
  @(posedge clk);
end
```

The dispatch reads the selector's entry value (§6.1 rule 4), §6's commit
rule governs its updates — the new value is seen at the *next* dispatch,
after a wait, never in the same cycle — and §6/§9 force a reset value into
the preamble, since the first lap reads the selector before writing it. It
is how a **computed jump** is written in structured control: the loop
closes on a single dispatch point and the branch on data reaches any arm,
which is the shape a command processor with multi-cycle handlers wants.

Two consequences are worth knowing before reaching for it. With no cut
point inside the arms and one closing wait, the whole lap is one segment,
the `case` is if-converted (§C.3), and the output is exactly the classic
`always_ff` FSM the author could have written directly — faithful, and
nothing gained. With cut points inside the arms, the generated machine
carries **two** state registers — `__fsm_state` for the position inside
the arms, the author's selector for the next dispatch — overlapping
without being redundant: correct by construction, not minimal. v1 leaves
it that way; §15 records the v2 flattening that fuses them.

## 5. Decision 4 — reset: the init segment, with the signal inferred or hinted

The init segment (§4) executes at time zero in simulation — exactly when
reset is asserted in a real system — and supplies the **reset branch** of
the generated `always_ff`, with the state register reset to the first
segment's state.

**That works because an `initial` preamble runs exactly once** (§9.2.1), so
the reset values are one piece of source with one job. An `always` preamble
runs at time zero *and again on every wrap-around*, so it cannot also be a
set of reset values — §15.1 carries the argument and the measurement.

The convention is not invented here — SystemC's `SC_CTHREAD` reaches the
same reset-preamble shape from C++, which §14 records as the strongest
argument available for it.

The reset *signal* cannot be derived from behavioural source, so:

1. if `veriparse_reset` is present (§3), it names the port;
2. otherwise the pass looks for exactly one single-bit module **input**
   whose name matches `rst_n`, `resetn`, `aresetn`, `rst`, `reset`
   (case-insensitive), taking its active level from the `_n` suffix;
3. if there is no match, or more than one, that is an **error** naming the
   candidates and pointing at `veriparse_reset`.

Rule (2) keeps the note's "no pragma required" property true for ordinary
designs — both RS232 examples in §11 declare `rst_n` and need no attribute
— while (3) refuses to guess. An unresettable state register is a
synthesis defect, so silence is not an option in either direction.

### 5.1 `initial` — the preamble is the reset branch

It runs once, so it is the reset action and nothing else: the generated
`always_ff` opens with `if (!rst_n)` carrying the preamble's assignments
verbatim, the state register going to the first segment's state. Every
register the preamble writes gets a reset value; no state is spent on it
and the machine is doing useful work on the first cycle out of reset.
Appendix A relies on exactly that, and §A.3's "eleven states, not twelve"
is this rule paying for itself.

It is well-defined for free. A preamble that *reads* a variable it has not
itself assigned would be reading an uninitialised value at time zero, and
§6 already makes that an error — so an `initial` reset branch cannot
accidentally depend on state that reset does not establish.

The preamble needs a cut point after it before the loop writes the same
registers again — §6's commit rule, nothing more — but **where that cut point
sits is free**: before the `forever`, or as the first statement of its body.
The two are the same thing once the loop closes, and neither spelling is
preferred. What is *not* free is the boundary between reset values and the
rest, and it is easy to write past by accident:

```systemverilog
initial begin
  p;                     // before the first cut point → the reset branch
  @(posedge clk);
  q;                     // after it, still before the forever → NOT reset
  forever begin … end
end
```

`q` does run exactly once — measured, not assumed — but as the action of the
state the first cut point enters, not as a reset value: it lands one cycle
after reset instead of during it, and it costs a state, because that state
differs from the one the lap re-enters (whose action is the `forever` body's
head alone). Everything meant as a reset value goes **above** the first
wait. What sits between the two is a one-time startup step — there may be
several, each costing one state — which is a different thing and
occasionally the thing wanted.

```systemverilog
(* veriparse_fsm *)                            // rst_n inferred, no hint needed
initial begin
  tick <= 1'b0;  n <= 8'd0;                    // preamble — runs ONCE
  @(posedge clk);
  forever begin
    begin : COUNT  n <= n + 8'd1; tick <= 1'b0; @(posedge clk); end
    begin : TICK   tick <= 1'b1;                @(posedge clk); end
  end
end
```

```systemverilog
localparam [0:0] COUNT = 1'd0, TICK = 1'd1;
logic [0:0] __fsm_state;

always_ff @(posedge clk)
  if (!rst_n) begin
    tick        <= 1'b0;                       // the preamble, verbatim:
    n           <= 8'd0;                       // BOTH registers get a reset
    __fsm_state <= COUNT;
  end
  else case (__fsm_state)
    COUNT: begin n <= n + 8'd1; tick <= 1'b0; __fsm_state <= TICK;  end
    TICK:  begin tick <= 1'b1;                __fsm_state <= COUNT; end
  endcase
```

Two states, no cycle spent on the preamble, and the `forever` back-edge
(§7.3) is what makes it perpetual — the wrap segment re-enters `COUNT`,
whose action it already is, so it costs nothing. Run against its source
under Verilator with reset released before the first edge, the FSM matches
**sample for sample, at zero offset**: the reset branch puts every register
where time zero left the reference. A longer hold changes only the §11
alignment offset — the reference keeps running while the FSM is held, so it
leads by exactly the hold.

### 5.2 Why the reset cannot be written in the source at all

**Nothing outside a suspended multi-wait process can re-enter it.** That one
property is why the reset values have to come from the preamble, why the
reference is unresettable (§11), and why the two familiar spellings below
both fail. It is stated once here; everything else points at it.

*As an `if` at the top* — `if (!rst_n) … else …` opening the process. A
multi-wait process is entered **once** and runs forever, suspending at each
wait, so the `if` executes **once**, at time zero, not every cycle. To be
tested each cycle it would have to enclose every wait, and its `else` would
then have to resume at whichever point the process had reached, which
program order does not provide.

*As a sensitivity list* — `always @(posedge clk or negedge rst_n) begin …
@(posedge clk); … end` (§2 makes it an error). `always @(E) S` is
`always begin @(E); S; end`: the senslist is a wait *statement at the top of
the loop*, not a guard on the body, so while the body is suspended on an
internal wait nothing is waiting on `E` and the edge is lost. Measured
rather than reasoned — a reset pulse delivered mid-body moves neither
`always @(negedge rst_n)` nor `always @(posedge clk or negedge rst_n)`;
both run the body to its end regardless. The second is worse than useless:
it *enters* on whichever event comes first, normally the clock, then
re-enters and replays its "reset" every time the body completes. Verilog
has no preemption of a running process except `disable`, which §9 forbids.

What the familiar shape actually is, is a **single-cycle** process:
`always_ff @(posedge clk or negedge rst_n) if (!rst_n) … else …` re-enters
from the top every cycle precisely because it has no internal waits. §2
leaves it untouched — it is this pass's *output* form, not its input.

**Sync versus async** is likewise not observable in the behavioural source
— seeing an asynchronous reset would require it in the sensitivity of
*every* wait. It is a property of the generated flop, which is why it is
`veriparse_reset_kind` and not something the source is asked to express.

### 5.3 Chip enable — in the source, never an attribute

A clock enable qualifies every transition: while it is low the machine
holds. Nothing above expresses that, and the gap is real — most real
designs have one.

**It cannot be an attribute, and the reason generalises.** The reset hints
work because they annotate a *signal*, never a value: what the registers
come up holding is decided by the source's own init segment (§5.1), so the
attribute changes nothing the behavioural source does. An enable changes **when the machine
advances**, which is observable behaviour. Annotate it, and the reference
keeps advancing while `en` is low where the generated FSM holds — the
differential cosim of §11 fails, and teaching the reference about the
attribute would break the hard constraint that the source runs as-is.

So it is written, using the notation the standard already provides
(§9.4.2.3, conditional event control):

```systemverilog
(* veriparse_fsm *)
initial forever begin
  @(posedge clk iff en);
  ...
  @(posedge clk iff en);
  ...
end
```

`@(posedge clk iff en)` means *wait for a posedge of clk at which `en`
holds*, and maps onto `always_ff @(posedge clk) if (en)` directly. It
simulates: Verilator 5.050 accepts it, so the behavioural reference honours
the enable too and the cosim exercises it.

**The reset branch goes outside the enable, not inside it.** The mapping
above says where the `case` goes; taken as the whole shape of the process it
would nest §5's synchronous reset under `if (en)` and give a state register
that cannot be reset while the enable is low — a machine that comes out of
reset only if something else is already running it. So the emission is

```systemverilog
always_ff @(posedge clk)
  if (!rst_n)   <reset action>       // never gated
  else if (en)  case (state) … endcase
```

and for `veriparse_reset_kind = "async"` the question does not arise: the
reset is in the sensitivity list and already outside everything.

**The cosim cannot arbitrate this one, and it is worth saying so where the
rule is stated.** Both nestings were built and compared against the
reference: the one with the reset placed *inside* the enable reports
**fewer** divergences, not more. Not because it is better hardware — because
the reference does not read the reset signal at all (§11), so a machine that
resets less agrees with it more readily. The measurement answers a different
question from the one asked.

**Synthesis answers the right one.** Through Yosys, an 8-bit register with a
synchronous reset and an enable:

| | **`if (!rst_n) … else if (en) …`** | **`if (en) begin if (!rst_n) … end`** |
|---|---|---|
| generic cell | `$sdffe` | `$sdffce` |
| `synth_xilinx` | 8 × `FDRE`, 8 × `INV` | 8 × `FDRE`, **8 × `LUT2`** |
| `synth_ice40` | 8 × `SB_DFFESR` | 8 × `SB_DFFESR`, **1 × `SB_LUT4`** |

The flop primitives on both targets carry a synchronous reset that is *not*
gated by the clock enable, so "reset only while enabled" has to be built out
of fabric — a LUT per bit on Xilinx. Reset outside the enable is what the
primitive already does, and costs nothing beyond the inverter on `rst_n`.
That is the reason the rule is normative rather than stylistic, and phase 3
pins the emitted shape with a structural golden.

Putting the two together — the §5.1 machine with `iff en` on each of its
two waits — the emission becomes:

```systemverilog
localparam [0:0] COUNT = 1'd0, TICK = 1'd1;
logic [0:0] __fsm_state;

always_ff @(posedge clk)
  if (!rst_n) begin                       // outside the enable: the machine
    tick        <= 1'b0;                  // leaves reset whether or not
    n           <= 8'd0;                  // anything is running it
    __fsm_state <= COUNT;
  end
  else if (en) case (__fsm_state)         // one gate, every transition
    COUNT: begin n <= n + 8'd1; tick <= 1'b0; __fsm_state <= TICK;  end
    TICK:  begin tick <= 1'b1;                __fsm_state <= COUNT; end
  endcase
```

The enable costs exactly one `else if`: it qualifies the `case` and nothing
else, because §2's uniformity means there is one condition to apply rather
than one per state. Run against its source with `en` toggling irregularly
over 300 cycles, the two agree on every cycle — the reference honours the
enable through `iff`, so this half of the emission *is* differentially
checkable, unlike the reset placement above.

It needs **no new concept**: §2 already requires every wait in a process to
carry the same edge on the same signal. The rule extends to *and the same
`iff` condition, if any*, and the enable falls out of the uniformity that
is already there.

A text macro puts that uniformity beyond the reach of oversight:
`` `define COMMIT @(posedge clk iff en) `` written once and used at every
cut point — `veripp` expands it, the reference simulates the expansion, so
the hard constraint holds and the mismatch rows below become unwritable.
Worth doing in any process with more than a few waits; the v2 task
inlining of §15 is the scoped version of the same idea.

**Disagreement is an error** — waits some of which carry a condition and
some of which do not, or which carry different ones. Two conditions are the
same when their expressions are structurally equal after the passes that
ran before this one, so `en` and `en` agree while `en` and `en == 1'b1` do
not; that is a coarse test and deliberately so, since the alternative is
proving expressions equivalent.

**The two disagreements are one rule but not one mistake, and the
diagnostic must tell them apart.** *Some waits qualified and some bare* is
overwhelmingly an oversight — the author added `iff en` while editing and
missed one — and it is the dangerous one, because the obvious repair a tool
might make silently is to gate the missing wait too, which would change the
machine. So the message names the odd wait out by line and lists the ones
that disagree with it, rather than reporting a generic mismatch: *"`@(posedge
clk)` at line 41 carries no `iff`, while the waits at lines 33, 37 and 45
carry `iff en`"*. *Different conditions* is likelier to be deliberate, and
its message points at §15's per-state enable rather than at a typo.

Neither is ever repaired by guessing. An enable is observable behaviour
(above), so inserting or dropping one changes what the source means, and
§3 rule 1 leaves no attribute with which to say "I meant it".

Worth being accurate about *why* it is refused, though: a machine whose
states are gated differently is not nonsense. It is a state-dependent clock
enable, and it is occasionally what someone wants — an idle state that
samples regardless of `en`, say. It is refused because it is a **different
feature** with its own per-state enable logic, not because it has no
meaning, and §15 records it as such rather than as an impossibility.

**Parser prerequisite**: veriparse does not currently parse `iff` — a
conditional field on **`Sens`**, not on `Senslist`. Annex A.6.5 binds the
qualifier to each event term, not to the list:

```
event_expression ::= [ edge_identifier ] expression [ iff expression ]
                   | event_expression or event_expression | …
```

and §9.2.2.4's own example is `always_ff @(posedge clock iff reset == 0 or
posedge reset)`, where one term carries a condition and the other does not.
A field on `Senslist` could not represent that: it would either fail to
parse or hoist one term's condition onto every term, silently changing the
sensitivity. Putting it on `Sens` costs nothing here — §2 requires exactly
one `Sens` per wait anyway — and is the shape that does not have to be
redone the first time a multi-term `iff` appears. It joins the §7.2
`LoopUnrolling` guard and the §10.1 `ScopeElevator` guard in phase 1.

## 6. Decision 5 — the author says which is which: `<=` is a register, `=` is combinational

**Rule.** The two assignment forms mean in the source exactly what they mean
in the RTL this pass emits, and the author picks:

- **`x <= v` is a register.** The value is committed at the next edge, so
  `x` survives the cycle and becomes a flop in the generated `always_ff`.
- **`x = v` is combinational.** It names a value *within* one cycle, and it
  must not outlive it: reads of `x` see the new value immediately (§10.4.1),
  which is what makes the imperative style read in program order.

An earlier draft inferred this instead — every target classified by liveness,
`=` accepted throughout and silently promoted to `<=` wherever a chain
crossed a cut point. That is a guess about intent where the language already
has a notation for it, and §3 rule 2 says a guess becomes a question. The
author writes what they mean, and the pass **checks** rather than divines.

**And the check is scoping, not liveness**, which is what makes the rule
cheap to state and impossible to get subtly wrong: an `=` target must be
**declared in a scope that contains no cut point**. Then every read of it is
in the same segment as every write by construction, it cannot outlive the
cycle, and whether the declaration is static or automatic stops mattering.
SystemVerilog gives the notation without ceremony — §9.3.1, *"an unnamed
block creates a new hierarchy scope only if it directly contains a block item
declaration"*, and Annex A makes the label optional:

```systemverilog
@(posedge clk);
begin
  logic [8:0] sum;
  sum = a + b;              // combinational, and it dies at this `end`
  q  <= sum[7:0];           // registered, and it crosses the next edge
end
```

An `=` to a variable whose declaring scope spans a cut point — other
than a module-level one — is an **error** naming the target and pointing
at `<=`. Nothing is silently promoted, and a reader can tell a register
from a wire by looking at the declaration rather than by running the
analysis in their head. A **module-level** `=` target is the one
exception, and it is not a loophole in the liveness rule but a third
storage class with its own contract: a *decoded output*, §6.2, where the
`=` on every path becomes the per-state value of an emitted
`always_comb`.

**There is no exception, and the `for` step is not one.** In a clocked
system an `=` that crosses an edge has no meaning to give it, so the rule is
absolute for the author's variables. A `for` index is not one of those: it
is loop control the construct owns. Unrolled — the §7.2 default — each
replicated iteration carries it as a constant and the blocking step never
reaches the FSM at all; rolled (`veriparse_no_unroll`, or a bound that does
not fold), it becomes an **induced variable** (§7.2, §C.3) — a register of
the index's declared type, stepped by the construct — like the countdown
that implements a rolled `repeat`: storage by construction, outside the
liveness question by definition.

An author who wants a counting shape the construct does not express — a
custom re-arm point, a selection order the stride must survive — writes the
register themselves (§7.3 shows the idiom), and then it is an ordinary
variable taking `<=` like any other. What they may not do is write `=` to
something that has to survive the edge — there is nothing for the pass to
build out of that.

**Every `<=` must be committed**, and that is the rule the discipline needs
to be worth anything. A nonblocking assignment schedules a value for the
next edge; if a second `<=` to the same register runs before that edge, the
first never takes effect — it was written and never committed. So **on any
one path through a segment there is at most one `<=` per register**, which
in §C.1's terms is simply that `s_p` holds one update per target rather
than the last of several. Two of them on one path is an **error**.

The rule is not about where the wait sits. A `forever` body may open with
its `@(posedge clk)` or close with one — the loop makes those the same
thing — and neither spelling is preferred. What matters is only that a cut
point separates any two writes to the same register.

**The idiom is to guard, not to separate.** A write that belongs to only
one path goes under the condition that selects that path, so each path
commits once; inserting a wait between two unconditional writes also obeys
the rule but says something different about the machine, and usually not
what was meant. Appendix A shows the guarded form.

This is stricter than the standard, which defines the double write (§10.4.2:
the last in a time step wins) rather than forbidding it. The reason for
being stricter is that the definition is not what simulators deliver: an
earlier RS232 transmitter that closed a frame with an unconditional
`busy <= 1'b0` and opened the next with `busy <= 1'b1` in the same cycle was
measured to give opposite answers on Verilator and on Icarus/ModelSim — the
same file, the same testbench. A source whose meaning depends on that is not
a specification, which is what §1 asks it to be.

A register of the process **read before assigned** on the first path out of
reset takes its value from the init segment — the rule is about storage the
process itself writes; inputs and signals driven by other processes are
simply reads. If it has none, the behavioural source
would propagate `x` while the synthesized register comes up at whatever
reset gives it, and the two disagree exactly where debugging is hardest —
so the pass says so, and loudly: the init segment is the reset branch
(§5.1), a value it cannot supply is unrecoverable, and there is no later
lap to make the read meaningful. It is an **error**.

### 6.1 Within-segment semantics — translation is substitution, not rewriting

§6 makes the author name each form, so the pass never has to rewrite one
into the other. What it must still do is **substitute**, because a blocking
write and a nonblocking one disagree on what a *later statement in the same
segment* sees (§10.4.1 vs §10.4.2), and the emitted RTL has only the
nonblocking form to say it with.

After §6 and §7.2, two kinds of blocking write reach a segment. An `=` to
a scope-local temporary never survives the cycle, so it is substituted
into its uses and vanishes — no flip-flop, only the value it named. And a
**rolled loop's induced init and step** (§7.2) are blocking by the
construct's own contract while the index is a register: a read *after*
the step in the same segment must see the stepped value, so the pass
substitutes it forward. Emitted naively — `data[__fsm_i]` where the
source read the incremented index — every bit goes out one position late,
a bug that simulates plausibly; the substitution is where that hazard
dies. An unrolled iteration has neither problem: its index is a constant.

**Rule.** A segment is straight-line code executed in one cycle, so the
pass evaluates it symbolically and emits, per target, a single nonblocking
assignment carrying that target's **final value at the end of the
segment**. Segment-local `=` targets vanish entirely into the values they
name. Two environments are threaded through the segment:

- the **pre-edge** environment — the value every register held when the
  state was entered. Reads of a target that has only been written by `<=`
  in this segment resolve here, which is exactly §10.4.2: the update is
  not visible until the NBA region;
- the **blocking** environment — updated in place by each `=`, so
  subsequent reads in the same segment see the new value, per §10.4.1.

Four consequences worth stating, because each is a place a
straightforward implementation goes wrong:

1. **Repeated assignment.** Several `=` to one local collapse to the last
   after substitution — that is the whole point of a combinational name.
   Several `<=` to one register on one path do **not** collapse: §6 makes
   them an error, because the first was never committed. Branches are not
   repetition — `if (c) x <= a; else x <= b;` is one write per path.
2. **Partial targets.** `bits[i] <= rx;` updates one slice and leaves the
   rest holding, and a scope-local `=` can do the same. The environments
   and `s_p` therefore track **per-slice** values, not whole variables; a
   whole-variable model would silently drop the surviving bits. RS232 RX
   depends on this.
3. **Mixing the two forms on one target is an error**, and §6 makes it one
   twice over: `x = …` and `x <= …` reaching the same variable is a race in
   the source *and* a contradiction about whether `x` is a wire or a flop.
   Reject rather than pick.
4. **Sampling point.** Signals a segment *reads* resolve to the values
   sampled at the edge that entered the state, which is precisely
   `always_ff` RHS sampling. This is what makes the two descriptions agree
   whenever the signal is driven by nonblocking assignment — the ordinary
   RTL case. A signal driven by a *blocking* assignment from another
   process is already a race in the behavioural source, and the pass does
   not invent a resolution for it.

The rule also explains why §6's discipline is safe: a target whose
final value never leaves the segment was only ever a name for a
subexpression, so substitution removes it entirely and no flip-flop is
inferred.

**Substituted in the analysis; materialized on the way out when the
language demands it.** The analysis always consumes the temp: paths are
characterised as `(R_p, s_p)` over entry values, which is what merging
paths, building guards, if-converting branches (§C.3) and the §C.6
self-check all compare — a value still naming a temp cannot be compared
across paths. The temp's *scope* never survives either: §10.3 sits
downstream of `ScopeElevator`, nothing after that point handles
block-scoped declarations — the same argument §10 makes against emitting a
`typedef enum` — and 1364 has no unnamed-block declarations at all.

What does come back, when needed, is the *name*: emission re-materializes
a substituted value as a **module-level continuous assignment** —
`wire [8:0] __fsm_t_sum = a + b;`, `__fsm`-prefixed and collision-checked
like every §10 declaration, read inside the case arm. It is correct by the
same sampling argument as everything else — the wire is a pure function of
registers and inputs, so an `always_ff` RHS reading it at the edge sees
exactly what the inline expression would have produced — it works
identically in the 1364 mode (`wire` + `assign`, no cast needed, the
declared width doing the truncating), and it is the shape `WireSplit`, the
folding passes and the flattener already own. Three conditions call for
it, and the first two are not cosmetic:

- **the substituted form is unprintable** — `t = a + b; q <= t[7:0];`
  substitutes to `(a + b)[7:0]`, and a part-select applies to a name,
  never to an expression;
- **the declared width was doing work** — with `logic [8:0] t`, the
  `a + b` truncates at 9 bits *because the declaration says so*, while
  substituted inline it evaluates at the surrounding context width
  (§11.6) and can differ in the carry corner. SystemVerilog could patch
  that with a `SizeCast`; 1364 cannot, and one rule for both modes beats
  two. The test is exact: materialize when the substituted expression's
  context width would differ from the temp's declared width;
- **readability**, at the emitter's discretion — a value read many times
  may deserve a name even where inline is legal; §15 keeps this purely
  cosmetic half as the optional part.

The v1 emitter resolves the three conditions by construction rather than
by analysis: **every value materializes** as a wire typed by the
temporary's declaration — dims and signing, keyword widths included — so
the declared-width and unprintable-select conditions can never be
mis-judged, and identical expressions share a wire only under identical
declared types, since the type *is* the truncation. The one value that
never earns a wire is a **constant**, folded and truncated to the declared
width at substitution time — the fold tier done eagerly and exactly. The
inline-alias economy this forgoes is precisely what the
`ConstantFolding`/`VariableFolding` running right after (§10.3) exist to
recover.

The same mechanism, applied *across* states, is a lightweight resource
sharing: two case arms reading one materialized wire present the
synthesizer a single network, and states being mutually exclusive, nothing
ever contends. It shares only *identical* expressions — naming, not
binding: one operator serving different operands behind a state-driven mux
is HLS allocation, the thing §C.2 forbids — and §15 records it as a
hint-enabled v2 mode.

None of it disturbs folding: a temp whose value the veriparse machinery
*can* fold stays substituted, and the `ConstantFolding`/`VariableFolding`
that §10.3 runs right after this pass collapse it to the constant — an
unrolled copy's index arithmetic never earns a wire. Materialization is
for the values that remain expressions.

This is not a local invention. It is how the FSMD literature characterises
a path: by the condition under which it is taken and by what it leaves in
each variable, both read against the values held on entry — which is the
rule above, in other words. **Appendix C** gives it its notation and builds
the algorithm and the data structures on it.

### 6.2 Module-level `=` — combinational output decode

A module-level variable assigned by `=` inside the marked process is a
**decoded output**. The process may mix the two forms freely — that is the
point: the behavioural model reads in program order with both — and the
lowering splits them by discipline: every `<=` stays in the `always_ff`
with the transitions, every module-level `=` moves into one
**`always_comb` over the state register**, the classic Moore output
decode. One signal, one discipline: a target taking both `=` and `<=`
in the process is an error, and a decoded output joins the §9.2.2.4
multi-driver check like any register.

**The value model is the arriving convention.** In the source, a segment's
`=` executes at the edge that opens the segment, and the value holds until
some later segment reassigns it — so the value visible during a cycle is
the one assigned by the path that **arrived** at the current state. That
is exactly what a hand-written Moore machine's decode arm says: *in* state
`S`, the output has the value the design gave it *entering* `S`. The arm
for `S` is therefore built from the paths arriving at `S`: agreeing
arrivals collapse to one assignment; disagreeing ones form a guarded tree
over their residual guards; infeasible arrivals were already pruned
(§C.4). The init segment is the arrival into the entry state and becomes
the comb's own reset branch, mirroring §5 — read as a level, since a comb
has no edge for `veriparse_reset_kind` to matter to — and its values
double as the `default` arm, so unreachable encodings latch nothing.

**Totality is the anti-latch check and the §9 gate.** Every path between
two consecutive cut points — the init segment included — must assign every
decoded output; the last `=` on a path binds, blocking semantics. A path
that skips one *holds* in the source and *tracks* in the emitted comb, so
the skip is exactly where the two forms part ways — rejected, naming the
path. A rolled `repeat`'s lap is such a path by construction (the lap
carries no user statement) and fails the check; §7.3's `while` idiom is
the spelling that re-asserts per lap.

**Stability is the coherency check, and it is not negotiable.** The
emitted arm re-evaluates the assigned expression *during* the cycle, over
post-edge register values; the source evaluated it once, at the opening
edge, over entry values. The two agree if and only if no operand of the
value expression — after §6.1 substitution, so temporaries are transparent
— and no operand of a residual arrival guard is a register **committed by
its own arriving path**. Violations are rejected naming the register and
the commit. The trap this closes is easy to write and silently off by one:

```systemverilog
while (i < 8) begin
  tx = data[i];             // reads entry i = v … but the same lap
  i <= i + 1;               // commits i = v+1 at the same edge, so an
  (* veriparse_no_unroll *) // emitted `tx = data[i]` would track v+1
  repeat (BAUD_DIV) @(posedge clk);
end                         // for the whole period the source shows v
```

There is no rewrite that saves this shape as a comb decode — a §6.1
temporary substitutes to the same operand — and the honest answer is that
a **self-advancing expression decode stays a register**: write `tx <= …`
with §6's v1 placement, or wait for the three-process emission (§15),
whose sequential next-state half is where such a value belongs. What
passes the check trivially is what the feature exists for: the constant
and quasi-constant Moore outputs — `busy`, `valid`, `ready`, state-typed
flags — and expressions over registers the arriving paths leave alone.

A module **input** is rejected wherever a decode value or a tree guard
reads it, and this is a measurement, not a style rule: the §15 draft
admitted inputs under an edge-synchronous caveat, and the phase-9 bench
refuted it — an input legitimately changes *on* the arrival edge, the
`always_ff` sampled it before that edge to choose the transition, and the
emitted arm re-reads it after, so the two disagree for the whole arrived
cycle whenever the input flipped. The fix is the message: register the
input, and the register is stable for the cycle by §9.2.2.4.

One reduction keeps the stability rule from over-firing: a tree's guards
are the paths' full conjunctions, and the conjuncts **common to every leg
of a group** — the way into the fork, typically a §7.3 wait-state's exit
condition over an input — hold on every leg and discriminate nothing, so
they are dropped before the tree is built and before stability judges the
guards. Only the differing conjuncts — the fork condition itself — must
be stable.

**Mixing rules inside one segment come from §6.1 unchanged.** A later
read in the same segment — by another `=`, by a `<=` right-hand side, by
a fork condition — sees the decoded output's new value by substitution; a
`<=` never feeds back into its own segment (entry values, §6). A read
*before* the segment's own `=`, or across a cut point, needs no new rule
either, unlike §6.1 temporaries: module-level storage holds, both sides
show the previous arrival's value, and the emitted form agrees because
the `always_ff` samples the comb at the closing edge — which shows
exactly the current cycle's arm.

**Emission.** One `always_comb` per process, placed with the machine it
decodes: after the state `localparam`s and register declarations, before
the `always_ff`:

```systemverilog
always_comb
  if (!rst_n) begin
    busy = 1'b0;            // the init segment's `=` values
  end
  else
    case (__fsm_state)
      __fsm_IDLE:   busy = 1'b0;
      __fsm_RUN_0:  busy = 1'b1;
      ...
      default:      busy = 1'b0;   // the init values again
    endcase
```

The §10.2 state map lists each decoded output with its per-state value,
the same way transitions carry their actions.

## 7. Decision 6 — loops

### 7.1 Loops with no cut point

Untouched. `LoopUnrolling` and the folding passes already own them.

### 7.2 Bounded loops *containing* a cut point — unrolled, uniformly

A bounded loop whose body contains a cut point **unrolls into consecutive
states**, and the rule is the same for `for` and `repeat`: replication is
what bounded iteration already means everywhere else in veriparse (§7.1),
and a wait in the body changes what a copy *costs* — a state instead of a
statement — not what the construct means. There is no heuristic on the
loop body and no asymmetry between the two constructs. Appendix A's
`DATA` loop yields `DATA_0 … DATA_7`, each copy reading its bit at a
constant index.

That uniformity **reverses this ADR twice over**, and the record matters.
The first draft compiled both forms to counters; a revision then split
them — `repeat` counting, `for` unrolling — on the argument that a
`repeat` body has no index, so its iterations are indistinguishable and a
countdown is a free implementation choice. True as far as it goes, but
the split was the RS232 example steering the rule: `repeat (BAUD_DIV)` at
`BAUD_DIV = 100` made a hundred states look absurd, and the absurdity was
quietly promoted into a per-construct default — a heuristic on the body,
which is what §3 rule 2 exists to forbid. One meaning for bounded loops;
the state count is the author's decision, and it has a notation:

> **`(* veriparse_no_unroll *)` on a loop keeps it rolled.** It follows
> every §3 rule: `veriparse_`-prefixed, no `fsm_` infix — it instructs
> `LoopUnrolling`, not this pass, which is §13's decoupling — and it is a
> pure implementation choice, the rolled and unrolled machines agreeing
> cycle for cycle, so §3 rule 1 holds. It is §3 rule 2 in the flesh:
> rolled-versus-unrolled is exactly a question the pass would otherwise
> have guessed.

**The rolled lowering** — taken by a `veriparse_no_unroll` loop, whose
bound may therefore be non-constant. The mark is the only road in: a
bounded loop whose bound stops folding *without* the mark is a §9 error,
not a silent rolled compile (§8 records why the once-tried repair lost):

- **`repeat (expr)` becomes one state with a countdown register.** IEEE
  §12.7.2 evaluates the count once, on entry — so a non-constant `expr`
  is *captured at entry* into the countdown by the standard's own
  semantics, and a zero count skips the state through an entry guard.
  For a constant `N` the countdown holds `N-1 … 0` in `$clog2(N)` bits —
  except at `N = 1`, where `$clog2(1)` is 0 and `logic [$clog2(N)-1:0]`
  is the illegal range `[-1:0]`: a single wait needs no countdown at all,
  so **`N = 1` induces no counter** and the width rule applies from
  `N = 2` up. Appendix A.2's `logic [$clog2(BAUD_DIV)-1:0] __fsm_cnt;` is
  the form for a `BAUD_DIV` of two or more, not a template to expand
  blindly.
- **A `for` becomes its body's states plus an induced index register**,
  honouring the construct's full contract, because the body may read the
  index. The register takes the index's **declared type** — an `int i` is
  a 32-bit signed flop; an author who wants four bits declares
  `logic [3:0] i` — the init assigns it on entry, the stop condition is
  the back-edge guard, re-evaluated each lap over entry values (which is
  when the source evaluates it: in zero time, at the same cut point), and
  the increment expression commits once per iteration, whatever it is.
  The blocking init and step are substituted forward within their own
  segment (§6.1).

On a loop with **no** cut point in its body the hint is inert — the loop
runs in zero time and there is no state to save — and the pass **errors**,
telling the author to drop the hint and let the loop unroll. An earlier
revision had it warn and unroll regardless, the §2 inert-mark rule applied
to a hint; but the unrolling would have to be this pass's own — the shared
unroller honoured the hint and is already behind us — and this section
says twice over that the pass carries no unrolling machinery of its own.
An error keeps that true, and the fix is deleting one attribute.

Two more contract points the rolled lowering pins down, both loud errors
rather than silent choices (§3 rule 2):

- **A non-constant repeat count is a plain signal.** The capture
  `cnt <= expr - 1` must hold any value the count can carry, so the
  countdown takes the count signal's **declared width** — the same rule as
  the `for` index's declared type. An arbitrary expression has no declared
  width to take; the author binds it to a named signal first, and the
  binding is one line.
- **A rolled `for`'s index is a module-level declaration.** The pass
  consumes the process, so a declaration inside it does not survive to
  carry the induced register; "the register takes the index's declared
  type" needs a declaration that outlives the lowering. §7.3's
  explicit-counter idiom already writes it that way.

**Nested rolled repeats each own their depth's countdown.** One register
per repeat-nesting depth (`__fsm_cnt`, `__fsm_cnt2`, ...), sized over the
counting repeats at that depth: sequential timers at one depth still share
— they re-initialise on entry — while an inner timer's reload leaves the
outer's remaining count alone, which is what makes the 2D-scan shape
(`repeat (ROWS) begin ... repeat (COLS) @(posedge clk); ... end`) compile
without hand-writing either counter. A repeat that induces no counter (a
folded count of 0 or 1) consumes no depth.

**Prerequisite change, with its own commit and test:** `LoopUnrolling`
honours `(* veriparse_no_unroll *)`, leaving the marked loop rolled. Two
things about that guard are easy to get wrong and are therefore normative
here:

- **It must still recurse into the body.** `LoopUnrolling::unroll` returns
  from `install_unrolled` on a `For`/`Repeat` node, so a guard that simply
  returns without unrolling never visits the body — and an unrollable loop
  *nested inside* a kept one would stay rolled, in exactly the shape §7.1
  and §C.4 step 1 assume is already gone. The guard skips the unrolling of
  *this* loop and keeps descending.
- **It is unconditional, not restricted to marked processes**, and that is
  a deliberate change to a shared pass. Restricting it would mean teaching
  `LoopUnrolling` about `veriparse_fsm`, which is the coupling §13 exists
  to avoid. The blast radius is the author's own ask: a kept loop in
  ordinary RTL survives to the output, where the downstream flow will say
  what it thinks of it — nothing that was working changes without the
  attribute. The test says so rather than the reader assuming it.

Loops without the attribute keep unrolling exactly as today — a bounded
`for` with a cut point arrives at the CFG builder as replicated copies
with `data[3]`-style constant reads, `LoopUnrolling` substituting the
index and uniquifying each copy's declarations and scopes, so the pass
needs no unrolling machinery of its own. A replicated group's size is
visible in the state map (§10.2), and an unusually large expansion draws
a §2-style informational note pointing at `veriparse_no_unroll` — the
`BAUD_DIV = 100` timer is almost always meant rolled.

### 7.3 Data-dependent loops containing a cut point

`while (cond) …` and `forever …` become
a real **back-edge**: the segment ending at the loop's cut point
re-evaluates `cond` and either re-enters the loop's first segment or
proceeds to its successor. The canonical form

```systemverilog
while (!start) @(posedge clk);
```

is a single state that holds itself while `!start` — the wait state, and
the construct that makes this style worth having.

A loop containing **no** cut point and no static exit inside a schedulable
process is rejected: §9.2.2.1 names it a simulation deadlock, and it has no
hardware meaning either.

**The explicit-counter idiom.** `veriparse_no_unroll` buys the induced
register with the construct's own contract; a counting shape outside that
contract — a custom re-arm point, MSB-first selection, an exit the stride
must survive — is written by hand: the author declares the register, and
the loop becomes data-dependent, which is exactly this section:

```systemverilog
logic [3:0] i;                              // 4 bits: the exit value 8 must
…                                           // be representable, or `i < 8`
initial begin                               // never fails
  tx <= 1'b1; busy <= 1'b0; i <= 4'd0;      // reset value: §6 requires one
  @(posedge clk);
  forever begin
    …
    begin : DATA
      while (i < 8) begin
        tx <= data[i[2:0]];                 // entry value — no substitution
        i  <= i + 4'd1;                     // committed once per path (§6)
        (* veriparse_no_unroll *)
        repeat (BAUD_DIV) @(posedge clk);
      end
    end
    begin : STOP
      tx <= 1'b1; i <= 4'd0;                // re-arm HERE, not only in reset
      (* veriparse_no_unroll *)
      repeat (BAUD_DIV) @(posedge clk);
    end
  end
end
```

Two details are load-bearing, and both are decisions the tool would
otherwise have taken silently, which is why §3 rule 2 wants them in the
source. The register is **4 bits, not 3**: a 3-bit `i` wraps and the exit
test never fails. And the re-arm sits **in `STOP`, not only in the
preamble**: reset it once and the second, back-to-back frame enters `DATA`
with `i == 8` and skips it entirely — precisely the frame-chaining case
§A.3 records as the one that catches translation bugs.

## 8. Decision 7 — `break` and `continue` are CFG edges

`continue` transitions to the first state of the innermost enclosing loop;
`break` transitions to the first state *after* it. Both are in scope for
v1 — the note's RS232 RX example depends on `continue`, and once states are
explicit a jump is just an edge, needing none of the flag lowering
ADR-0005 §3.2.1 deferred. The loops in question are the ones the CFG still
sees — `while`, `forever`, a rolled `repeat` or `for` (§7.2); in an
unrolled `for` the jumps are `LoopUnrolling`'s business, lowered across
the unrolled sequence as
today (ADR-0005 §3.2). A `break`/`continue` in a loop containing **no**
cut point is not this pass's business and is left to the existing passes.

**That delegation can fail, and §7.1 and §7.2 assume it did not.**
`LoopUnrolling` refuses the mixed and nested jump forms ADR-0005 §3.2.1
deferred — it warns and leaves the loop intact rather than unrolling it
wrongly. So a loop it refused — with or without a cut point in the body —
inside a marked process reaches the CFG builder still rolled, in a shape
§7.1 or §7.2 expected to be gone.

What the pass does with such a survivor has changed twice, and the second
change is the one that holds. The draft had it **notice and refuse**,
because at drafting time a surviving loop had no correct lowering. Once
the §7.2 rolled forms and this section's jump edges landed, a correct
lowering existed — the shapes the unroller refuses, back-edges and mixed
`break`/`continue` included, compile exactly — and for a while the pass
**repaired**: compiled the survivor rolled with a warning pointing at
`(* veriparse_no_unroll *)`. But the repair breaks §7.2's contract in the
one way that section calls out as the trap: *all bounded loops unroll
uniformly*, and a warning is not a contract. The failure mode is a bound
that stops folding after the fact — a `--param-map` keep on the parameter
behind it, a rewrite that unbinds it — silently trading N unrolled states
for one rolled group: cycle-for-cycle correct, structurally a different
machine than every constant-bound build of the same source. So a
surviving bounded loop **with** a cut point is a §9 **error** naming both
exits — mark it `(* veriparse_no_unroll *)` to compile it rolled, or make
the bound constant — and the rolled lowering runs only where the mark
says the author chose it. A survivor **without** a cut point still has
nothing this pass can do and errors per §9's zero-delay row.

## 9. Errors — rejected loudly, never silently mis-lowered

References are marked `IEEE §…` for a clause of IEEE 1800-2017 and `ADR §…`
for a section of this document; the two numbering schemes overlap and an
unmarked column has already been misread once.

| **Condition** | **Why** | **Reference** |
|---|---|---|
| cut points over different signals, mixed `POSEDGE`/`NEGEDGE`, or a level sensitivity (`@(sig)`) in one process | multi-clock scheduling is out of scope, and a level wait is no clock at all; picking one would mis-compile | IEEE §9.4.2 |
| cut points **some carrying an `iff` and some bare** | almost always an oversight, and not repairable by guessing: adding or dropping an enable changes the machine. Names the odd wait out and the ones it disagrees with (ADR §5.3) | IEEE §9.4.2.3 |
| cut points carrying **different** `iff` conditions | v1 takes a *uniform* enable, which is what a chip enable is; gating states differently is a separate feature, not a variant of this one (ADR §5.3, §15) | IEEE §9.4.2.3 |
| a marked `Always` whose body holds an `EventStatement` | `always` is not compiled (ADR §15): with an empty senslist it is a degenerate spelling of `initial forever`, and with a non-empty one it is neither RTL nor compilable, the senslist being a wait at the top of the loop that cannot preempt a suspended body (ADR §5.2). The message carries the rewrite | IEEE §9.2.1, §9.2.2 |
| `DelayStatement` (`#d`) in a schedulable process | simulation timing with no static hardware meaning | — |
| a system **task** in a marked process (`$display`, `$finish`, `$fatal`, `$monitor`, …) | no hardware meaning; reaching one means the mark landed on testbench code | IEEE §20.2, §20.10 |
| a system **function** outside the constant/query subset below | same reason, minus the ones every synthesis flow accepts | IEEE §20 |
| `WaitStatement` / level-sensitive control | not an edge; no boundary to cut at | IEEE §9.4.3 |
| `fork`/`join` | concurrent control flow the state model cannot express: one control position per machine. Write cooperating marked processes instead; §15 records the v2 decomposition that would write them for you | IEEE §9.3.2, ADR §15 |
| `disable` | abortive control flow the state model cannot express | IEEE §9.6.2 |
| cut point inside a called `task` | not visible in the process body; v1 does not inline to find it (§15). A `function` can never hold one — IEEE §13.4 forbids time-controlled statements there | IEEE §13.4, ADR §15 |
| a `function` called in a marked process that writes non-local state | expression position is no place for a side effect: the `(R_p, s_p)` model would miss the write silently. **Pure functions are accepted** and pass through to the output as the ordinary combinational calls they are | IEEE §13.4 |
| loop with no cut point and no static exit | zero-delay infinite loop — deadlock, and no hardware | IEEE §9.2.2.1 |
| a path through a kept loop's body that reaches the head again without crossing a cut point | the same zero-delay lap, hidden in one arm of a branch: the loop has cut points, that path has none | IEEE §9.2.2.1 |
| `(* veriparse_no_unroll *)` on a loop without a cut point | the hint is inert — the loop runs in zero time, there is no state to save — and honouring it would need unrolling machinery this pass deliberately lacks (ADR §7.2); the fix is deleting the attribute | ADR §7.2 |
| `break`/`continue` outside a loop the CFG sees | nothing to jump within: in an unrolled loop the jumps were the unroller's business (ADR §8), and stray ones have no target | ADR §8 |
| a rolled `for` missing init, test or step, or whose init and step assign different registers | the lowering honours the construct's full contract on one index register; half a contract is a different construct | ADR §7.2 |
| a bounded loop with a cut point the unroller left behind, **without** `veriparse_no_unroll` | rolled is opt-in: a bound that stopped folding (a `--param-map` keep, an unresolvable expression) or a jump shape the unroller refuses must not change the state count in silence — mark the loop rolled or make the bound constant (ADR §7.2, §8) | ADR §7.2, §8 |
| a non-constant repeat count that is not a plain signal | the countdown takes the count signal's declared width (ADR §7.2); an arbitrary expression has none to take — bind it to a named signal first | ADR §7.2 |
| a rolled `for` whose index is not a module-level declaration, or not a **variable** the machine can drive | the pass consumes the process, so an in-process declaration cannot survive to carry the induced register — and an input port or a net cannot take its commits (ADR §7.2) | ADR §7.2, IEEE §9.2.2.4 |
| a constant repeat count that folds **negative**, or beyond 2^32 | a loop cannot execute a negative number of times and tools disagree on what one means — §12.7.2 gives only x/z a meaning (zero) — so it is almost always a parameterization off-by-N; and no countdown the lowering sizes holds 2^32 laps | IEEE §12.7.2, ADR §7.2 |
| a system call outside the constant/query subset in a fork or loop **condition**, or in a rolled `for`'s init/step | the walk forks on conditions, reuses them across guards, and prunes their contradictions (§C.4) — every one of those moves assumes the condition reads stably within its zero-time segment, which `$random` and its kin break | ADR §C.4, IEEE §20 |
| the mark on an item that is not a process, or on an `initial` with no wait | the mark says the author meant it, and there is nothing to compile | ADR §2 |
| a hierarchical reference into a marked process (`COUNT.cnt_tmp` from outside it, or across its labels) | the referent is consumed by the lowering — no state block survives to the output for the reference to bind to. Caught at scope elevation, where the reference would otherwise be silently rewritten to a name that no longer resolves | ADR §10.1 |
| reset signal neither hinted nor uniquely inferable | an unresettable state register is a synthesis defect | ADR §5 |
| a register of the process read before assignment out of reset, with no init value | source and RTL disagree where it is hardest to debug, and the reset branch cannot supply the value | ADR §5.1, §6 |
| the preamble reads a register of the process | a read of the empty entry store: nothing is assigned at reset entry — the preamble's own `<=` commits only at the clock edge — so the reset value would be undefined | ADR §5.1, §6 |
| a branch in the preamble, cut point inside it or not | the reset branch loads reset values once; a fork there would make the state out of reset input-dependent, and even a cut-point-free branch emitted under the reset arm is re-evaluated on every reset cycle where the source evaluates it exactly once — and an arm that skips a register leaves it with no reset value | ADR §5.1 |
| `casex`/`casez` in a marked process | wildcard matching is not lowered in v1 (ADR §15): the fork guard would need wildcard-match semantics, and exact `==` would silently change which arm runs | IEEE §12.5.1, ADR §15 |
| a `case` with more than one `default` arm | the grammar admits it, IEEE allows at most one, and the guard construction has no condition to give a second one | IEEE §12.5 |
| a case item with x/z bits, in a `case` holding cut points | plain-`case` item matching is case equality, but the fork guard is built with logical `==`, which such an item never satisfies. A cut-point-free `case` stays verbatim in its action and keeps its semantics | IEEE §12.5 |
| a `=` temporary shadowing a module-level declaration or an enclosing temporary | substitution binds by name (§6.1): honouring the shadow would silently hijack values across scopes — rename it; §15 records the v2 alpha-renaming that lifts the restriction | ADR §6, §15 |
| one signal taking both `=` and `<=` in the process | one signal, one discipline (§6.2): a target cannot be a register and a decoded output at once | ADR §6.2 |
| a path between two cut points that skips a decoded output | totality (§6.2): the skip is where the source *holds* and the comb *tracks* — a rolled lap cannot re-assert, §7.3's `while` spells the per-lap form | ADR §6.2 |
| a decoded output's value or arrival guard reading a register committed by its own arriving path | stability (§6.2): the arm re-evaluates over post-edge values where the source read entry values — the emitted decode would be off by one commit | ADR §6.2 |
| a target written by two schedulable processes, or by one and any other process | IEEE §9.2.2.4: *"Variables on the left-hand side of assignments within an `always_ff` procedure … shall not be written to by any other process."* The source is merely a race; the **output would not conform**, which is the stronger reason to refuse. The check sees processes, continuous assigns, generate blocks, the tasks other processes call, and — when the driver supplies the parsed modules, as verilower does — **instance output and inout ports**, scope-aware; an instantiated module missing from the map is a black box and skipped | IEEE §9.2.2.4 |

**System functions are not system tasks, and the row above must not be
written as if they were.** `$clog2`, `$bits`, `$size`, `$left`/`$right`,
`$signed`/`$unsigned` (IEEE §20.5–§20.8) are elaboration-time or purely
combinational, every synthesis flow accepts them, `ExpressionEvaluation`
already folds three of them, and Appendix A.2's pre-fold listing writes
`logic [$clog2(BAUD_DIV)-1:0] __fsm_cnt;`. The §20.9 bit-vector
functions — `$countones`, `$countbits`, `$onehot`, `$onehot0`,
`$isunknown` — are the same kind of stable, synthesizable read and join
the subset. Rejecting any of them would have the pass refuse on input a
construct its own listings write on output, and §3 rule 1 leaves no
attribute to escape with. They are therefore **accepted**; a system
function outside that subset is rejected by the second row.

## 10. Generated form, naming, and pass placement

Output is a plain `always_ff` with a `case (state)`, one **`localparam` per
state**, a vector state register, the induced countdown and index
registers of §7.2's rolled lowering, and the `wire` temporaries §6.1's
emitter materializes when substitution cannot be printed — accepted by
every downstream tool.

**Not a `typedef enum` — and the honest argument is narrower than
"never".** The obvious emission is `typedef enum logic [3:0] {…}
__fsm_state_t;`, and three things rule it out as the *default*. First,
verilower's own pipeline: §10.3 puts this pass after `EnumElaboration`,
`EnumInliner` and `TypedefInliner` have run, and nothing re-runs them — an
enum created at that point survives into the `ConstantFolding`,
`VariableFolding` and `DeadcodeElimination` that follow the slot, none of
which was ever taught it: the mid-pipeline breakage ADR-0009 was written
to remove, reintroduced from the far end. Second, deployment: the pass is
a library pass any front end may invoke (§2), including
`veriflat --fsm` running it in-process ahead of the flattener — which has
no `Typedef` or `Enum` case at all — so the default emission must be safe
in *every* pipeline position, not just verilower's. Third, output
language: `typedef enum` does not exist in 1364-2005, and one emitter
serving both modes beats two.

What the argument is **not** is a claim that enums in output are broken.
Across tools, through serialization, they are fine: verilower writes a
file, `veriflat` re-parses it, and its own ADR-0009 machinery inlines the
enum like any user-written one — the breakage was always about
mid-pipeline creation within one run, never about enums in source. §15
therefore keeps enum emission as an opt-in style for the chained SV
workflow, where it buys something real: enum states display symbolically
in every simulator natively, no state-map filter needed. The default
stays what every pipeline position and both output modes understand:

```systemverilog
localparam [3:0] __fsm_WAIT_SEND = 4'd0;
…
logic [3:0] __fsm_state;
```

`localparam` is the right carrier rather than a bare literal:
`LocalparamInliner` runs at the *head* of `ResolveModule`, so a localparam
introduced here is never inlined away, and the generated RTL stays
readable. Written without a data type it is also `local_parameter_declaration`
in IEEE 1364-2005 §A.2.1.1, so the state constants survive a Verilog output
target unchanged — the state *register* follows whatever net kind the mode
already gives every other declaration, and the process is emitted as plain
`always @(posedge clk)` there, `always_ff` being SV-only. The symbolic decode a waveform needs
comes from the state map (§10.2), which has to be emitted anyway since the
encoding is not recoverable from the RTL.

Each `localparam` carries the source line of its segment so a waveform is
readable.

**The output is post-resolution, and the listings show the pre-fold form.**
§10.3's slot means every pass ahead of it has already run: parameters
inlined, constants folded — which is precisely what makes `repeat
(BAUD_DIV)` a constant bound in the first place. The module `verilower`
writes is therefore **de-parametrized**, like everything the resolution
pipeline emits: its `BAUD_DIV` is `100` everywhere and the countdown is
declared `logic [6:0]`. Appendix A.2 keeps the parameter symbolic because a
listing is for reading; the structural golden pins the folded form.

**The unit of compilation is the module the command line names.** The
output holds exactly that module; other modules in the input serve as
context — packages, name resolution, instance references — and are absent
from it. Each marked module is its own run, and a marked process outside
the selected module draws a **warning** naming the run that compiles it
(§2: the mark is never skipped in silence). The run's parameterization is
the module's defaults, overridden per parameter by `--param-map` — the
`veriflat` map, same YAML, same semantics: `{N: 42}` overrides, `{N:}`
keeps `N` a parameter of the output. A kept parameter the machine's
*structure* depends on is an **error**: behind a bounded loop's bound it
trips §8's refusal (the loop no longer unrolls, and rolled is opt-in);
behind a `veriparse_no_unroll` countdown it fails §7.2's sizing (a
parameter is not a plain signal with a declared width). Only a `while`
condition (§7.3) reads a kept parameter legally — the data-dependent form
never depended on the value. A kept parameter only the datapath reads
survives into the output, so a lowered machine can stay parametric where
lowering does not need the value. What the map cannot do is compile a marked module
instantiated elsewhere **with per-instance overrides** — one CLI
parameterization per run. Per-instance resolution is the flattener's
job, and **`veriflat --fsm`** does it: the flattener already resolves
each instance's clone with that instance's parameters, so enabling the
§10.3 slot inside that resolution compiles every marked instantiation at
its own parameterization — two instances of one module get separately
sized machines, names uniquified by the flattening as any declaration
is. Under the flag the ADR-0007 subset check keeps its input
verdict — its rules in force are mode-independent (a virtual interface
is illegal in any mode, and the check passes a marked process's edge
waits; a future rule a marked input may legitimately trip must gate on
the mode, not the call site) — and gains an output verdict on the
flattened result, verilower's placement, so what the lowering emitted
is vetted too. Without the flag nothing changes,
and per §2 nothing is silent: a marked design flattens as-is — attribute
included, so a chained verilower still sees it — with an info note
naming `--fsm` as the road not taken.

Generated declarations take the `veriparse_prefix` (default `__fsm`), settling
the note's open question on collisions: readable in a waveform, safe
against user identifiers. A module holding several marked processes gets an
ordinal in the default — `__fsm0`, `__fsm1`, in source order — because §2
allows the plural and one shared prefix would make the allowed case collide
by construction. A collision remaining after prefixing is an
error, not a silent rename.

### 10.1 Naming the states — SystemVerilog block labels

A generated name (`__fsm_state_47`) is useless in a waveform and the pass
cannot invent a good one, so §3 rule 2 applies: the author writes it. The
notation is the **block label** the language already has (§9.3.4), not an
attribute — it is legal, simulable, and *names the code* instead of
annotating it. `Block.scope` already carries it through the parser, so
there is no grammar work.

```systemverilog
@(posedge clk);
begin : IDLE
  ...
end
```

Placing the label **after** the wait is what makes it work: the block then
delimits the segment instead of overlapping it by accident.

**The rule is about the graph, not about cut points.** A label names the
states of the sub-CFG it delimits: one state takes the name, several take
it as a stem with an index. Saying instead "a label names one state when
it contains no cut point" is tempting and wrong — it rejects the commonest
shape there is:

```systemverilog
begin : START
  tx <= 1'b0;
  (* veriparse_no_unroll *)
  repeat (BAUD_DIV) @(posedge clk);   // a cut point, inside the label
end
```

That block is **one** state — the rolled counter state of §7.2, the wait
being how
it exits — and every phase of Appendix A's transmitter is written this way:
A.1 carries one label per phase, and they are where A.2's `__fsm_WAIT_SEND`,
`__fsm_START`, `__fsm_STOP` and the `__fsm_DATA_k` family come from. Under the graph rule
it just works, with no special case: a rolled `repeat` and a `while` each
yield one state, so their label names it.

A label over a sub-CFG of **several** states gives them the stem with an
ordinal:

```systemverilog
begin : SEND_BIT
  sda_out <= byte_out[nbit - 4'd1];
  (* veriparse_no_unroll *) repeat (T_LOW)  @(posedge clk);
  scl     <= 1'b1;
  (* veriparse_no_unroll *) repeat (T_HIGH) @(posedge clk);
end
```

yields `SEND_BIT_0`, `SEND_BIT_1`. Nested labels compose outward-in, and
that is the way out of the ordinals when they are not good enough: Appendix
B's bit loop labels the body `BIT` and its two halves `LOW` and `HIGH`, so
the states come out `BIT_LOW` and `BIT_HIGH` rather than `BIT_0`/`BIT_1` —
which is what one wants for a protocol phase, and why the composition rule
earns its place.

Replication (§7.2) needs no rule of its own: Appendix A's `DATA` label
wraps the eight unrolled copies of its loop body, and the per-copy scopes
`LoopUnrolling` already uniquifies give the counter states their ordinals —
`DATA_0 … DATA_7`.

Naming is incremental: an unlabelled segment keeps an ordinal, so the
states that matter get names and the rest do not.

**The init segment is not a state and cannot be named.** It is the reset
branch (§5.1), so a label on it would point at nothing. That is the
fold-away case below with one difference worth a distinct diagnostic: a
folded state could have existed, whereas this one never could, so the pass
**warns** and points at §5.1 rather than silently dropping the name.

**Prerequisite change, with its own commit and test: `ScopeElevator` must
keep the label.** As it stands the naming scheme above cannot work at all,
and it is worth being exact about why rather than discovering it in phase 8.
`ScopeElevator` (`lib/src/passes/transformations/scope_elevator.cpp`, the
`Block` case) splices a named block into its parent whenever that parent is
a `Block` — it renames the block's declarations to `<scope>__<name>` to keep
them unique, records the mapping, then calls `pickup_statements` and the
`Block` node, with its `scope` string, is gone. §10.3 places this pass
*after* `ScopeElevator`, and every labelled block in this ADR — the `IDLE`
example above, `START`, `SEND_BIT`, every phase of Appendix A — is nested
inside its process's outer `begin`/`end`. All of them would be spliced away
before the CFG is built, so every state would fall back to its ordinal and
the author would get `__fsm_state_0/1/2…`, the exact waveform this section
exists to prevent, with no diagnostic saying the labels were dropped.

The guard mirrors §7.2's and covers **every block of the marked process**,
not the labelled ones alone. A named block still has its declarations
**renamed** exactly as today — that is what keeps two `int i` in two
segments apart, and the emission needs it when a segment's registers are
promoted to module level — but the splice is skipped, leaving the `Block`
node and its `scope` in place. An **unnamed** block is never spliced:
under §6 it can hold only `=`-temporaries, which §6.1's substitution
dissolves into values, and its boundary is precisely what §6's scoping
check reads, so it must survive to the pass. (One nuance, pinned by the
test: an enclosing label's rename sweep still prefixes declarations in
its whole subtree, surviving unnamed blocks included — harmless, since
the rename is consistent and the scope boundary is what matters.)
Everything outside a marked process,
named or not, elevates as it does today. Each kept block is consumed by
this pass along with the rest of the process body, so no downstream pass
ever sees a surviving block and the invariant `ScopeElevator` exists to
establish is unbroken.

**Two things to check in phase 2 rather than assume.** A named block is a
*scope* (§9.3.4), so declarations inside it are local — which is exactly
what §6's scoping check reads — and the interaction with the renaming above
has to be exercised before labels are advertised. And a label whose states all fold
away leaves a name pointing at nothing; the state map (§10.2) must simply
not list it, rather than emit an entry with no encoding.

### 10.2 The state map

The encoding is an implementation choice the source does not contain, so
it must be emitted, not reverse-engineered from the RTL. `verilower` writes
a **JSON state map** beside the output: per process, the state variable's
name and width, then per state its encoded value, its name, and the source
line its segment starts at.

**It also records what the reset does, and that is not a convenience.** §1
rests on the source being the specification, and reset is the one thing the
source does not state: nothing in the behavioural text names `rst_n`, says
which registers it clears, or to what. A reviewer would have to re-derive it
from §5's rule, the construct in use, and an inventory of what the preamble
writes. So the map carries it — the signal, its active level and kind, and
the list of registers the reset branch actually writes:

The list is everything the author put **above the first cut point**, plus
the state register the pass adds implicitly — the author never writes that
one, and §5.1 is where it comes from. A register the design expected to come
up reset and that is absent from the list is exactly what this makes
reviewable.

JSON is the canonical form, and no **viewer** format is generated from
inside the tool. Waveform viewers each want their own file — GTKWave a
translate filter, Surfer its own translator — and that knowledge already
lives in `wavedisp`, which drives all of them from one description and
already has `radix='symbolic'`. Emitting viewer formats here would
duplicate it and guarantee drift; emitting JSON lets `wavedisp` consume it
(`scripts/fsm_wavedisp.py` is the bridge).

One derived output is generated beside it, because it is review material,
not viewer plumbing: `--fsm-dot` prints a **graphviz** view of each
machine from the same report the JSON serialises — states as circles, the
reset-entry state as a double circle, guards on the edges. The register
updates stay off the edges unless `--fsm-dot-values` asks, so the picture
reads as a state graph rather than a listing. Sharing the report is the
rule that keeps the two from drifting: the dot printer consumes exactly
the structure the JSON records, never the AST.

### 10.3 Placement in the `ResolveModule` pipeline

```
… StructLowering → ScopeElevator → LoopUnrolling → BranchSelection
  → GenerateRemoval → ImplicitFsmElaboration
  → ConstantFolding → VariableFolding → DeadcodeElimination → …
```

after `GenerateRemoval` so the CFG is over real control flow rather than
generate structure; after `LoopUnrolling` — which §7.2 teaches the
`veriparse_no_unroll` hint — so §7.1 loops and unrolled §7.2 groups are
already flat; and **before** the second
`ConstantFolding`/`VariableFolding`/`DeadcodeElimination` so the generated
FSM is folded and cleaned by the passes that already own that work, which
is the note's "before flatten/dead-code keeps the output clean".

The slot is downstream of every pass that normalises declarations, which is
what forces the `localparam` form of §10 and the `ScopeElevator` guard of
§10.1 — two bills the position inherits rather than chooses, both due in
phase 2. Paying them is still cheaper than moving the pass earlier: ahead of
`ScopeElevator` the body would hold rolled §7.1 loops and generate
structure, and the CFG would be built over the wrong thing.

Two further interactions to **verify in phase 2 rather than assume**:
`VariableFolding` and `DeadcodeElimination` both already walk
`WhileStatement`, and neither was written with a suspended process in mind.
If either mishandles a body containing an `EventStatement`, the fix belongs
in that pass with its own test.

## 11. Validation — differential cosimulation against the same testbench

The property motivating the design is that **the input is simulable**, so
the golden model is free.

- Verilator 5.050 — the version the dev environment resolves to today;
  `conda/environment.yml` lists `verilator` without a pin — elaborates the
  behavioural form under `--timing`; the generated FSM needs no such flag.
  The floor the Makefile rule that writes `conda/environment.yml` must emit
  is **`verilator >=5.050`, and it is semantic, not cosmetic**: earlier 5.x
  versions execute `<=` in an `initial` process as `=` — their own
  `INITIALDLY` warning says so — which corrupts the golden model twice
  over. The reference's outputs move to the *active* region of the edge
  (measured on 5.046 as a systematic one-region lead at every bit boundary
  against a same-edge comparator), and a segment reading a register it has
  itself `<=`-written sees the new value where IEEE §10.4.2 delivers the
  old one. 5.050 implements the standard's semantics. The floor is the fix,
  not the sampling point: the comparator stays on **posedge**, reading the
  values NBA-committed at the previous edge — the one convention that also
  survives a post-P&R netlist in the DUT slot, where mid-cycle sampling
  assumes settling margins real delays can violate.
- A cosim test builds **both** and drives them from **one** testbench,
  comparing outputs cycle by cycle — the harness shape `veriflat` already
  uses, with the behavioural source in the reference slot.
- **Reset is asserted at time zero only; the testbench must not pulse it
  again mid-run.** The reference cannot honour one — §5.2's property, not a
  weakness of the plan — so a mid-run reset would report a divergence that
  is not a defect, and the harness simply does not issue one. The §11.1
  equivalence statement reads "from the release of reset onwards", which is
  all it ever claimed.
- **The comparison is aligned, not simultaneous.** The reset branch carries
  the preamble, so the FSM leaves reset already in the reference's time-zero
  state — but the reference started at time zero regardless, so it is ahead
  by however long reset was held. The two agree value for value at a
  constant offset equal to that hold, and the harness aligns on it rather
  than comparing raw sample indices. Measured at holds of 0, 1, 2, 3, 5 and
  7 cycles: the superposition is exact at the hold every time.
- A testbench that instead **holds every input at its idle value until
  reset releases** makes the offset unobservable — the reference's state is
  then invariant over the hold — and may compare raw sample indices. Either
  convention is sound; the harness states which one it uses rather than
  aligning by accident.
- The two RS232 examples from the design note are the primary corpus: TX
  exercises `forever`, `while`, rolled `repeat` timers
  (`veriparse_no_unroll`), an unrolled `for` and a shared countdown; RX
  adds input sampling and `continue`. With Appendix B's explicit-counter
  loop they cover every row of §7 and §8.
- Structural goldens (YAML + generated Verilog) pin the encoding and the
  state decode, in the existing house style.

### 11.1 What has to be argued, and what does not

Verifying a high-level synthesiser's scheduling is a research problem with
a literature of its own: the source and the scheduled design are both
FSMDs, but scheduling *moves operations across cycle boundaries*, so the
two have different cut points. Establishing that they agree needs a path
cover and a non-trivial **bisimulation relation** between their states —
that is what path-based equivalence checking does, and why it is hard.

**None of that applies here, and the reason is the whole point of the
design.** The author writes the cut points; the pass never moves an
operation across one. Source and generated FSMD therefore have *the same
control structure*, and the bisimulation relation is the **identity on
states** — by construction, not by proof.

**One honest gap, found by reading the sources rather than summarising
them.** That literature defines a *computation* as a finite walk from the
reset state back to itself with no intermediary return, and equivalence
between computations characterised that way. **The line falls between
perpetual and one-shot**, which since §15 is the only distinction left: an
`initial … forever` matches the model, being the non-terminating outer loop
from reset the definition assumes, while a **one-shot** `initial` does not —
it parks in the §2 hold state and never returns to reset, so a
"computation" in that sense does not exist for it. The identity-bisimulation argument still holds
state by state, but the equivalence statement for a one-shot has to be
phrased differently — agreement on every path up to entering the terminal
state — and this ADR does not yet do that. It is a hole in the argument,
not in the transformation, and it belongs to whoever writes phase 6.

What remains to be argued is strictly smaller: that the transformation
*within* one segment is faithful. That is exactly §6.1's substitution
rule — final value per target, pre-edge environment for reads of
nonblocking-assigned variables, per-slice tracking for partial writes.
Restating the obligation this way is worth more than any amount of extra
testing: it says which single page of reasoning the correctness of the
pass rests on. §C.6 notes that the same framing makes a path-by-path
self-check cheap, since there is no bisimulation left to search for.

### 11.2 Coverage as the adequacy criterion

Agreeing on a directed testbench is evidence, not proof. What turns it
into something defensible is knowing *what was exercised*, so the harness
measures **structural coverage of the CFG** and the plan requires it to be
complete:

- every segment entered;
- every CFG edge taken, including each arm of every branch;
- every back-edge (§7.3) both taken and not taken;
- every `break`/`continue` edge (§8) taken;
- for a counter state (§7.2), the first and last iteration.

The reset branch sits outside this list and is reported as such rather than
counted covered: it is exercised once, at time zero, and never again — §11
forbids a second pulse — so it is pinned by the structural golden.

Uncovered items are reported, not silently tolerated. Constrained-random
stimulus over the process inputs runs on top of the directed cases to
reach what they miss.

This is what makes the §11.1 argument operational: the identity
bisimulation says the control structures match, and full edge coverage
says the datapath transformation was exercised on every one of them.

## 12. Phasing (each lands green) & test plan

1. **Prerequisites**, each its own commit and test — the §7.2
   `LoopUnrolling` guard (honour `(* veriparse_no_unroll *)`: the marked
   loop stays rolled, still recursing into its body); the §10.1
   `ScopeElevator` guard (keep every block of a marked process: named ones
   renamed, unnamed ones unspliced); and the §5.3 grammar
   addition for `iff` (conditional event control, IEEE §9.4.2.3): a
   condition field on **`Sens`**, with parser and generator round-trip, the
   round-trip covering the multi-term form
   `@(posedge clk iff en or posedge rst)` that a field on `Senslist` could
   not represent.
2. **Straight-line.** A one-shot `initial`, no branches, no loops, no
   `forever`: cut, segment, state register, `always_ff`, the §5.1 reset
   branch and the terminal hold state. Golden + a 3-state cosim, aligned on
   the offset §11 defines. Lands the
   §11 harness with its §11.2 coverage baseline — every segment entered,
   every CFG edge taken — the §10 interaction checks, and the `verilower`
   driver —
   including the design-level `NameResolution`/`DefaultResolution` preamble
   the pass depends on and `ResolveModule` does not supply, plus
   `SynthesizableCheck` on the output.
3. **Chip enable.** Phase 1 only parses `iff`; this is where it becomes a
   machine. The §2 uniformity check across the waits, the §5.3 emission —
   reset outside the enable, one gate over the whole `case` — and the
   structural golden that pins that nesting. It comes before branches and
   loops because it fixes the shape of the `always_ff` they will fill, and
   it is small once the straight-line emission exists. The golden wants a
   **synthesis check beside it**: §5.3's rule is normative on measured cell
   counts, so the check that produced them belongs in CI, and
   `conda/environment.yml` should carry `yosys` for the same reason §11 asks
   it to carry a Verilator floor.
4. **Branches** — `IfStatement`/`CaseStatement` with cut points in one or
   more arms, unequal lengths, merge state.
5. **Loops** — §7.2 unrolled groups and the rolled countdown/index
   lowerings (`veriparse_no_unroll`, non-constant bounds), then §7.3
   back-edges, then §8 jumps, and the §11.2 coverage instrumentation grows
   to the constructs that finally need it: back-edges taken *and* not taken,
   `break`/`continue` edges, and the first and last iteration of a counter
   state.
6. **Perpetual** — the `initial … forever` form (§2) on the §7.3 back-edge,
   and the multi-cycle one-time prologue above the loop. RS232 TX and RX
   land as cosim tests here — TX is itself perpetual, so its cosim cannot
   arrive earlier.
7. **Diagnostics** — the whole §9 table, one `TEST_ERROR_SV` each.
8. **Hints and the state map** — the §3 table beyond `veriparse_reset`
   (encoding, prefix), the §10.1 naming decision
   including the init-segment diagnostic, and the §10.2 JSON, with a `wavedisp`
   description consuming it end to end.

9. **Combinational output decode** (§6.2) — decoded-output collection and
   the `=`/`<=` discipline check; totality over the path cover (init
   segment and rolled laps included); stability against per-arrival commit
   sets; arm construction with arrival merging and the comb reset/default
   branches; emission and state-map extension. Golden tests per shape, a
   `TEST_ERROR_SV` per new §9 row, and a differential cosim whose bench
   drives a decoded `busy`/`valid` pair through branches, a rolled timer
   (§7.3 spelling), and a perpetual wrap — the tracking-vs-holding window
   is exactly what the cycle-sampled comparison probes.

**Why the perpetual form lands late.** Phase 2 compiles a one-shot
`initial` — the smallest vehicle for the cut/segment/emit machinery, with no
back-edge to close — and the `forever` that makes a machine perpetual
arrives only in phase 6, on the §7.3 loop lowering the phases before it
build. Nothing earlier should be shipped as ready for a design, since almost
every real FSM is perpetual.

## 13. Beyond FSMs — what the structure has to allow

`verilower` is named for the operation the family shares — **lowering**
behaviour into structure — not for this pass. The FSM lowering is
the first behavioural transformation, and others will want the same front
half, so the split is structural from the start:

- **generic** — building the CFG from a suspended process, cutting it at
  the timing controls, running §6's dataflow checks across the cuts, and
  the within-segment substitution of §6.1. None of this mentions state
  registers;
- **FSM-specific** — state allocation and encoding, the `always_ff` and
  `case` emission, the state map.

A later transformation that is not an FSM — a pipeline, say — reuses the
first and replaces the second. Writing the two halves as one class now
would guarantee the second transformation reimplements the first, which is
the mistake this ADR can avoid for free.

## 14. Related work

Positioning, so that later choices can be argued against something.

- **SystemC `SC_CTHREAD`** is the closest living relative: a thread
  suspended by a clock, `wait()` calls spanning cycles, a reset preamble,
  synthesised into an FSM by commercial HLS (Catapult, Stratus). The
  design here converges on the same shape from Verilog instead of C++,
  which is where the value is: the source stays Verilog, so it stays
  simulable by the tools already in the flow. §5.1's reset convention — a
  one-time preamble supplying the reset values — is theirs, arrived at
  independently — and for the same reason, a clocked thread being entered
  once rather than restarted every lap.
- **HLS proper** (Catapult, Stratus; CIRCT's Calyx and Handshake dialects
  in the open-source world) *schedules*: it decides which operation goes
  in which cycle, statically or dynamically. That is the part deliberately
  excluded here — and §11.1 shows the exclusion is what makes the
  correctness argument cheap. This pass is not a step towards HLS; it is
  the case where scheduling has already been done by hand.
- **Alternative languages** — Bluespec, Amaranth, Chisel, Clash, Spade —
  answer the same need by replacing Verilog. The proposition here is
  orthogonal: keep the RTL in Verilog, keep it simulable, and add one
  transformation.
- **Path-based equivalence checking / FSMD bisimulation** is the framework
  §11.1 borrows from, developed to verify HLS scheduling. It is cited for
  its vocabulary and for the argument that our obligation is the easy
  special case, not because this pass performs formal verification.
- Nothing found in the open-source Verilog ecosystem performs this
  source-to-source lowering, which is the gap this fills.

## 15. Not supported (v1 deferrals)

| **Feature** | **v1 behavior** | **Future home** |
|---|---|---|
| Mealy outputs | Moore only — for a nonblocking target the equivalence with the source is exact segment by segment, whereas Mealy moves the observable timing inside the cycle | user-written `assign` outside the block today; a v2 rule if that proves insufficient |
| Three-process emission (state register / next state / output decode) | one `always_ff` with a `case` (§10), plus the §6.2 `always_comb` for decoded outputs — the output-decode third, landed | v2 for the remaining split. The preferred style for synthesis and for reading, but the segment model puts the transition and the registered outputs in the same process, so the split is not the mechanical one it looks like — it needs its own decision; §6.2's stability analysis says where a self-advancing value must land when it comes |
| Output encoding (outputs carried by the state encoding itself) | not attempted | v2, as an `veriparse_encoding` value beside binary/one-hot/gray |
| `typedef enum` state emission | `localparam` only (§10) — the one form safe in-process, in any front end, and in both output modes | v2 opt-in emission style, SV output only, for the chained workflow: sound because a consumer re-parses and the ADR-0009 machinery re-resolves, and it buys native symbolic state display in any simulator. An in-process integration keeps `localparam` |
| Counter splitting | one shared countdown per repeat-nesting **depth**, re-initialised on entry: sequential rolled `repeat` timers share their depth's register, nested ones each own their depth's (`cnt`, `cnt2`, ...), and a rolled `for` keeps its own index | further splitting (one per site) is a post-v1 optimisation, if routing says so |
| Dispatch-idiom machine (§4) carrying two state registers — the control position plus the author's selector | correct by construction, not minimal; nothing merges them | v2 **selector specialization**: a register assigned only constant labels and read only in guards against constants (Yosys `fsm_detect`'s criterion) folds into the control state by reachable-pair splitting — statechart flattening, done reachability-driven to avoid the product blowup. Restructures control, so §11.1's identity no longer holds across it: ships off by default with a §C.6-style path-by-path check under the recorded (position, selector) → flat-state relation |
| Cross-state expression sharing (CSE over the whole process, `wire` per distinct subexpression) | §6.1's emitter materializes a `wire` only where the language forces it — unprintable selects, width-bearing declarations — and folds a value away when the machinery can; sharing and cosmetic naming are not attempted | v2, behind a process-level `veriparse_share` hint (§3: implementation-only, states are mutually exclusive so nothing contends). Shares *identical* expressions only — naming, not binding; one operator with state-muxed operands is the HLS allocation the row below refuses. Evidence first, per §3: measured cell counts, §5.3-style, before it may ever become a default |
| Multiple clocks or mixed edges | hard error (§9) | needs a CDC model, own ADR |
| `casex`/`casez` holding cut points, and x/z case items in a forking `case` | hard error (§9) — the fork guards are exact `==` | v2, if a design asks: wildcard-match guard expressions (`casez` semantics per item), the same if-conversion with a different comparison |
| Temporary **shadowing** — a `=` temporary named after a module-level declaration (a register, a port, a rolled `for`'s index) or after an enclosing temporary | hard error (§6, §9), with the one-word fix: rename. Legal SystemVerilog, but substitution and every by-name check (the environment, `check_temp_reads`, the §9.2.2.4 exclusion) bind by name — honouring the shadow silently would hijack values across scopes | v2: **alpha-rename** temporaries to unique names at collection — the `ScopeElevator` rename discipline, applied inside the process — after which the by-name machinery never sees two scopes share a name and the shadowed forms (the `fsm_temp_err9` shape included) compile as written |
| `fork`/`join` — concurrent control flow inside one marked process | hard error (§9): the model rests on **one control position** — a single state register over one sequential CFG — and `fork` creates several at once. The supported spelling today is the decomposition the language already has: **two marked processes in one module**, each its own machine, rendezvousing through registers (`while (!other_done) @(posedge clk);`), the §9.2.2.4 check keeping their write sets disjoint | v2, as mechanized decomposition — not the product automaton, which explodes and is unreadable, but **one sub-machine per branch** with generated done flags and the `join` lowered to the rendezvous wait on all of them: exactly the cooperating-processes idiom, written by the pass instead of the author. `join_any`/`join_none` are further semantics on top and stay out until a design asks |
| Cut point inside a called `task` (multi-cycle sub-sequence) | hard error (§9); a pure `function` is accepted, IEEE §13.4 keeping it cut-point-free by construction | v2 `TaskInliner` before the cut walk: a task spanning waits is a *reusable sub-sequence* — Appendix B's `LOW`/`HIGH` pairs, called four times. IEEE §13.3's copy-in gives an `input` argument defined capture-at-entry semantics — an induced register per call — with per-call-site state naming (§10.1 composition, `LoopUnrolling`-style uniquification), copy-out at task end, recursion rejected |
| Resource sharing, scheduling, pipelining, datapath generation | none — the author's edges *are* the schedule; the shared-wire CSE above is the structural subset that needs no allocation | out of scope by construction; this pass is not a prefix of full HLS |
| Memory inference policy (BRAM vs registers) | none | orthogonal |
| Per-state clock enable (waits gated by different conditions) | hard error (§9); v1 takes one uniform enable | per-state enable logic, once a design asks for it |
| A marked `always` process | hard error (§9), with the one-line rewrite in the message | see below — not planned, because there is nothing left for it to add |
| Reset asserted again mid-run | out of scope: nothing can re-enter a suspended multi-wait process from outside (§5.2) | would need a restartable reference, which the input form cannot express |

### 15.1 Why `always` is refused rather than supported

It was specified and then removed, so the reasoning is kept here rather than
rediscovered. `always` is not a second construct: **`always begin BODY end`
is exactly `initial forever begin BODY end`** — checked cycle for cycle — so
supporting it would be two spellings of one thing, and the rewrite is one
line the error message can carry.

It is also the lossy spelling, and that is what settles it. The reset values
come from the region *above* the loop (§5.1), and an `always` has no such
region: its preamble sits inside the lap and runs again on every
wrap-around — measured rather than assumed, `always begin n = n+100;
@(posedge clk); n = n+1; end` reaching 403 after three edges, four
preambles against three increments.
Compiled anyway, it would need the preamble to become a once-entered **init
state** — costing a cycle out of reset, resetting only the state register,
and leaving every other inferred register to come up unreset — plus its own
naming rule, its own coverage case, and a second branch in the algorithm.
An `initial` gives all of it for free, and gives a one-time prologue of any
length besides (§2), which `always` cannot express at all.

A v2 has nothing to gain by adding it back. What would be worth revisiting
is the opposite: whether the terminal `initial` (no loop) earns its place,
since §11.1 records that the equivalence argument does not yet cover it.

## Appendix A — Worked example: RS232 transmitter

Every rule above, on one design. The source is the TX from
`docs/imperative-fsm-design.md`; the RTL beside it is what this ADR
specifies the pass must produce, so it doubles as the acceptance criterion
for phase 6.

### A.1 Before — the behavioural source

```systemverilog
module rs232_tx #(parameter int BAUD_DIV = 100) (
  input  logic       clk, rst_n,
  input  logic [7:0] data,
  input  logic       send,
  output logic       tx, busy
);
  (* veriparse_fsm *)
  initial begin
    tx <= 1'b1; busy <= 1'b0;                  // init segment  -> reset branch
    @(posedge clk);
    forever begin                              // back-edge
      begin : WAIT_SEND
        if (!send) begin                       // guarded, so each path commits
          busy <= 1'b0;                        // `busy` once (§6): `busy` only
          while (!send) @(posedge clk);        // falls if we really stop
        end
      end
      busy <= 1'b1;
      begin : START
        tx <= 1'b0;
        (* veriparse_no_unroll *)                      // rolled: one counter
        repeat (BAUD_DIV) @(posedge clk);              // state, not BAUD_DIV
      end
      begin : DATA
        for (int i = 0; i < 8; i = i + 1) begin        // unrolled (§7.2):
          tx <= data[i];                               // 8 states, `i` a
          (* veriparse_no_unroll *)                    // constant per copy
          repeat (BAUD_DIV) @(posedge clk);
        end
      end
      begin : STOP
        tx <= 1'b1;
        (* veriparse_no_unroll *)
        repeat (BAUD_DIV) @(posedge clk);
      end
    end
  end
endmodule
```

### A.2 After — the generated FSM

```systemverilog
module rs232_tx #(parameter int BAUD_DIV = 100) (
  input  logic       clk, rst_n,
  input  logic [7:0] data,
  input  logic       send,
  output logic       tx, busy
);
  localparam [3:0] __fsm_WAIT_SEND = 4'd0;   // no typedef and no enum at
  localparam [3:0] __fsm_START     = 4'd1;   // this point in the pipeline
  localparam [3:0] __fsm_DATA_0    = 4'd2;   // — see §10
  localparam [3:0] __fsm_DATA_1    = 4'd3;
  localparam [3:0] __fsm_DATA_2    = 4'd4;   // DATA_0..DATA_7: the `for`
  localparam [3:0] __fsm_DATA_3    = 4'd5;   // unrolled, one counter state
  localparam [3:0] __fsm_DATA_4    = 4'd6;   // per copy (§7.2), named by the
  localparam [3:0] __fsm_DATA_5    = 4'd7;   // DATA label (§10.1)
  localparam [3:0] __fsm_DATA_6    = 4'd8;
  localparam [3:0] __fsm_DATA_7    = 4'd9;
  localparam [3:0] __fsm_STOP      = 4'd10;

  logic [3:0]                  __fsm_state;
  logic [$clog2(BAUD_DIV)-1:0] __fsm_cnt;    // symbolic for reading — the
                                             // emitted module is
                                             // de-parametrized (§10)
  always_ff @(posedge clk) begin
    if (!rst_n) begin
      tx          <= 1'b1;              // the init segment, verbatim
      busy        <= 1'b0;
      __fsm_state <= __fsm_WAIT_SEND;
    end
    else case (__fsm_state)

      __fsm_WAIT_SEND:                  // the `while (!send)` test: holds
        if (send) begin                 // while idle. A frame requested during
          busy        <= 1'b1;          // the stop bit never reaches here — it
          tx          <= 1'b0;          // is taken by __fsm_STOP directly
          __fsm_cnt   <= BAUD_DIV - 1;
          __fsm_state <= __fsm_START;
        end

      __fsm_START:
        if (__fsm_cnt == 0) begin
          tx          <= data[0];       // constant index — unrolling already
          __fsm_cnt   <= BAUD_DIV - 1;  // substituted it (§7.2, §6.1)
          __fsm_state <= __fsm_DATA_0;
        end
        else __fsm_cnt <= __fsm_cnt - 1;

      __fsm_DATA_0:
        if (__fsm_cnt == 0) begin
          tx <= data[1]; __fsm_cnt <= BAUD_DIV - 1; __fsm_state <= __fsm_DATA_1;
        end
        else __fsm_cnt <= __fsm_cnt - 1;

      // __fsm_DATA_1 … __fsm_DATA_6: the same arm, each loading the next
      // constant bit — data[2] … data[7]

      __fsm_DATA_7:
        if (__fsm_cnt == 0) begin
          tx <= 1'b1; __fsm_cnt <= BAUD_DIV - 1; __fsm_state <= __fsm_STOP;
        end
        else __fsm_cnt <= __fsm_cnt - 1;

      __fsm_STOP:                       // decides in the cycle that ends the
        if (__fsm_cnt == 0) begin       // stop bit, so a frame already
          if (send) begin               // requested starts on THIS edge with
            busy        <= 1'b1;        // no idle cycle — and `busy`, guarded
            tx          <= 1'b0;        // in the source, is written once on
            __fsm_cnt   <= BAUD_DIV - 1;// each path (§6, §A.3)
            __fsm_state <= __fsm_START;
          end
          else begin
            busy        <= 1'b0;
            __fsm_state <= __fsm_WAIT_SEND;
          end
        end
        else __fsm_cnt <= __fsm_cnt - 1;

    endcase
  end
endmodule
```

### A.3 What the example pins

- **Eleven states, not twelve, and that is `initial` earning its keep.**
  `docs/imperative-fsm-design.md` counted an `S_INIT`. Under §5.1 the init
  segment is the *reset branch*, not a state, so it disappears — one fewer
  state, one fewer cycle out of reset, and `tx`/`busy` come up at known
  values with no cycle of operation needed. It is the concrete form of what
  §15 says an `always` cannot do.
- **`data[3]`, not `data[__fsm_i]` — no index register at all.** This is
  §7.2 doing real work. An earlier draft compiled the loop to one `DATA`
  state plus an induced index, and the emission then had to substitute the
  blocking `for` step forward — `data[__fsm_i + 3'd1]`, one token away from
  the off-by-one `data[__fsm_i]` that transmits every bit one position late
  and simulates plausibly, exactly what hand-translation gets wrong.
  Unrolling dissolves the hazard: each copy reads its bit at a constant
  index, and the machine mirrors the source's control structure, which is
  what keeps §11.1's bisimulation the identity. A hand-written counter
  machine over an `idx` register is I/O-equivalent — and is not this pass's
  output; an author who wants it writes §7.3's idiom and gets it as source,
  not as a guess.
- **`busy <= 1'b0` sits under a guard, and that is §6 at work.** Written
  unconditionally at the end of the lap it would land in the same cycle as
  the next frame's `busy <= 1'b1` and never be committed — measured, that
  source gives opposite answers on Verilator and on Icarus/ModelSim.
  `if (!send)` makes each path commit `busy` exactly once: the chaining path
  writes only `1'b1`, the idle path writes `1'b0` and then waits. The
  decision therefore stays in the cycle that ends the stop bit, which is why
  `__fsm_STOP` branches on `send` rather than handing over unconditionally.
- **One shared countdown.** `repeat (BAUD_DIV)` — three textual sites,
  each kept rolled by `veriparse_no_unroll` (§7.2), ten counter states
  after the `for` unrolls around them — reuses one `__fsm_cnt`, re-loaded
  on entry to each state (§13 of the design note). Splitting it is a
  post-v1 optimisation.

A.1 and A.2 have been run against each other under Verilator 5.050 —
`BAUD_DIV = 4`, one frame, a back-to-back pair with `send` held across the
boundary, then random stimulus, 721 compared cycles — and agree on `tx` and
`busy` on every one from the release of reset. That is the §11 harness in
miniature, and it is worth saying that it was actually run: an earlier
form of the `__fsm_STOP` arms passes a single-frame test and fails the
back-to-back one, which is exactly the class of bug the appendix exists to
pin. Phase 6 lands it as a test rather than as a claim.

## Appendix B — I2C byte write: what a second protocol adds

RS232 exercises every rule, but only one at a time. An I2C master phase is
the useful second example because its counted loop is written as §7.3's
explicit-counter idiom — a body holding **two** cut points, so the loop is
a two-state lap for any byte width — and because it samples an input to
decide what happens next.

### B.1 Before

```systemverilog
logic [3:0] nbit;                                       // the author's counter

(* veriparse_fsm *)
initial begin
  scl <= 1'b1; sda_out <= 1'b1; sda_oe <= 1'b1;         // reset branch: an
  ack <= 1'b0; nbit <= 4'd8;                            // idle bus, not X
  @(posedge clk);
  forever begin
    begin : IDLE
      while (!go) @(posedge clk);
    end

    begin : START
      sda_out <= 1'b0;                                  // START condition
      (* veriparse_no_unroll *)
      repeat (T_HD_STA) @(posedge clk);
    end
    scl <= 1'b0;

    begin : BIT                                         // §7.3's idiom: two
      while (nbit != 4'd0) begin                        // cut points, so a
        begin : LOW                                     // two-state lap
          sda_out <= byte_out[nbit - 4'd1];             // MSB first — both
          nbit    <= nbit - 4'd1;                       // reads see the
          (* veriparse_no_unroll *)                     // entry value (§6.1)
          repeat (T_LOW)  @(posedge clk);
        end
        begin : HIGH
          scl <= 1'b1;
          (* veriparse_no_unroll *)
          repeat (T_HIGH) @(posedge clk);
        end
        scl <= 1'b0;
      end
    end
    nbit <= 4'd8;                                       // re-arm: next byte

    begin : ACK                                         // same shape again,
      begin : LOW                                       // so the same two
        sda_oe <= 1'b0;                                 // inner labels
        (* veriparse_no_unroll *)
        repeat (T_LOW)  @(posedge clk);
      end
      begin : HIGH
        scl <= 1'b1;
        (* veriparse_no_unroll *)
        repeat (T_HIGH) @(posedge clk);
      end
    end
    ack    <= ~sda_in;                                    // sample the slave
    scl    <= 1'b0;
    sda_oe <= 1'b1;
  end
end
```

### B.2 After — the control graph

Written as the state graph rather than full RTL; the emission follows §A.2
mechanically.

| **State** | **Action** | **Leaves when** | **To** |
|---|---|---|---|
| `IDLE` | — | `go` | `START` |
| `START` | `sda_out<=0`, load `T_HD_STA` | count 0 | `BIT_LOW` |
| `BIT_LOW` | `scl<=0`, `sda_out<=byte_out[nbit-1]`, `nbit<=nbit-1`, load `T_LOW` | count 0 | `BIT_HIGH` |
| `BIT_HIGH` | `scl<=1`, load `T_HIGH` | count 0 and `nbit!=0` | `BIT_LOW` |
| | | count 0 and `nbit==0` | `ACK_LOW` (`nbit<=8` re-armed) |
| `ACK_LOW` | `scl<=0`, `sda_oe<=0`, load `T_LOW` | count 0 | `ACK_HIGH` |
| `ACK_HIGH` | `scl<=1`, load `T_HIGH` | count 0 and `!go` | `IDLE` (`ack<=~sda_in`, `scl<=0`, `sda_oe<=1`) |
| | | count 0 and `go` | `START` (same writes, plus `sda_out<=0`, load `T_HD_STA`) |

The second `ACK_HIGH` row is the back-to-back byte — §A.2's `__fsm_STOP`
shape again: `IDLE` is a `while`, so it costs no cycle when `go` is already
high at the edge that ends the acknowledge.

Four things it adds over RS232:

- **The explicit-counter idiom, in the field.** The bit loop declares its
  own `nbit` and tests it in a `while` (§7.3), so its two cut points give a
  two-state lap — `BIT_LOW`/`BIT_HIGH` — for any byte width, where the
  unrolled-`for` alternative (§7.2) would spend sixteen states. A
  `veriparse_no_unroll` `for` would also give a two-state lap, with an
  induced up-index; the hand-written register is what buys the MSB-first
  down-count and the re-arm point. The width
  (4 bits: the count starts at 8, which three bits cannot hold) and the
  re-arm for the next byte are the author's visible decisions, which is §3
  rule 2's point. The state names come from §10.1's composition rule — the
  body labelled `BIT`, its halves `LOW` and `HIGH` — the case ordinals
  would have served badly.
- **A preamble that is the bus specification.** The reset branch releases
  both lines: out of reset the master presents an idle bus rather than `X`.
  An earlier draft of this example had an empty preamble — legal under
  §5.1, and wrong as I2C.
- **An input sampled at a known edge.** `ack <= ~sda_in` reads `sda_in` as
  of the edge leaving `ACK_HIGH` — §6.1's sampling point, and the reason
  the behavioural source and the FSM agree on *when* the slave is read.
- **A three-signal bus model.** SDA is written as `sda_out`/`sda_oe`/
  `sda_in` rather than an `inout`, because the flattener does not support
  `inout` ports (`Inout port not supported during flattening`). That is a
  veriparse limitation, not an I2C one, and this example is a good reason
  to lift it.

## Appendix C — Data structures and algorithm

Written out for review. §C.1 is what the literature settles, §C.2 what it
warns against, and the rest is design that follows from them.

### C.1 What the FSMD literature gives us

Two things, and the second is the useful one.

**The target model.** An FSMD is an ordered tuple — control states, primary
inputs, storage variables, primary outputs, a transition function and an
action function. That is the shape to build, and it is worth naming because
it says what the intermediate representation must carry: control and data
*together*, not a control graph with the assignments left in the AST.

**The path characterisation.** A *path* is a segment in the sense of §4 —
a run from one cut point to the next with none in between. The literature
characterises each one by two things, and the whole appendix is built on
that pair, so it is worth setting up slowly.

Take the state a path leaves. On entry it holds a **store**: the value of
every storage variable at that moment, before anything in the path runs.
Everything a path does is expressed against that store, and only against
it. Two questions then say all there is to say about the path:

- ***When* is it taken?* Several paths leave one state — a branch, a loop
  test, a counter reaching zero — and exactly one is taken per cycle. The
  answer is a predicate over the inputs and the entry store, written
  **`R_p`** (`R` for the *condition* under which path `p` runs).
- ***What* does it do?* One expression per storage variable, giving its
  value on exit in terms of the entry store. That is the new store, written
  **`s_p`**.

So a path is the pair `(R_p, s_p)`: a guard and a store update, both read
against the state's entry values.

**Two places where this is a specialisation of the general model, not the
model itself**, and being explicit about them is what makes the borrowing
honest.

The literature's data transformation is `r_p = <s_p, O_p>`: the store
update *and* an ordered list `O_p` of the values sent to output ports along
the path, order and multiplicity included, because in the general model an
output is an event. Here an output is a level signal held by a register, so
only its final value can be observed and it is already an element of
`s_p` — `O_p` is empty and the pair collapses to `s_p` alone. §C.3 and §C.6
are written for that collapsed form.

The general model also indexes inputs by occurrence — the `i`-th read of
port `P` along a path is a distinct value `P_i` — because a path there can
span several samples. Here a path is exactly one clock cycle, so every read
of an input along it yields the same value, sampled at the edge that
entered the state. That is §6.1's sampling point, and it is a consequence
of where we put the cut points rather than an assumption.

Concretely, the `BIT_HIGH` state of Appendix B has three paths out, and
writing them down is the fastest way to see what the notation buys:

| | **`R_p`** | **`s_p`** |
|---|---|---|
| still counting | `cnt != 0` | `cnt := cnt - 1` |
| next bit | `cnt == 0 && nbit != 0` | `scl := 0`, `sda_out := byte_out[nbit-1]`, `nbit := nbit - 1`, `cnt := T_LOW-1` |
| byte done | `cnt == 0 && nbit == 0` | `scl := 0`, `sda_oe := 0`, `nbit := 8`, `cnt := T_LOW-1` |

Read the middle row against the entry store, which is the only way it may
be read: `nbit` there is the value *on entry*, so `byte_out[nbit - 1]` and
`nbit := nbit - 1` are both in terms of it — the bit selected and the
decrement never see each other, which is what nonblocking assignment means
and what a hand translation forgets first. The notation does not merely
describe the rule; it makes stating it wrongly awkward.

**The validity condition, and why we meet it.** A path cover is a *finite*
set of paths such that any computation is a concatenation of them, so the
cut points must be chosen to make paths finite — every loop has to contain
at least one. The literature obtains that from two rules: the reset state
is a cut point, and so is any state with more than one outward transition,
which "cut each loop of the FSMD in at least one cutpoint, because each
internal loop has an exit point".

Our rule is different — a cut point at every clock edge — so the obligation
has to be discharged separately. It is, by construction: §7.1 hands every
loop *without* a cut point to the passes that unroll or fold it, so every
loop still standing when the CFG is built contains an `EventStatement`,
hence a cut point. The condition holds for a different reason than theirs,
and the ADR should say so rather than inherit the conclusion.

Our cover is also strictly **finer** than the minimal one: cutting at every
edge rather than only at branch points makes every path exactly one clock
cycle. That is not incidental — it is what makes `s_p` simply "the state's
action", what makes inputs single-valued along a path, and ultimately what
makes §11.1's bisimulation the identity.

That is the payoff. `(R_p, s_p)` is precisely what §6.1 describes in prose:
the pre-edge environment is the entry store, the "final value per target"
is `s_p`, and the branch conditions collected along the way are `R_p`. The
substitution rule is therefore not a local invention to be defended on its
own — it is the standard characterisation of an FSMD path, and writing the
algorithm in those terms makes §11.1's argument mechanical rather than
rhetorical.

### C.2 What not to build

HLS builds a **CDFG** and schedules it: operations carry mobility, and the
scheduler decides which cycle each lands in. Building that here would be a
category error. The schedule is an input, fixed by the author's edge waits,
and every cut point is immovable. The representation is therefore a
**control-flow graph with fixed cut points**, never a dataflow graph with
freedom to move operations across them.

Concretely: no ASAP/ALAP, no resource constraints, no mobility intervals,
no list scheduling. If any of those appear in the implementation, the
design has drifted.

### C.3 Data structures

```
StateId       = index into the state table

Transition {                     // one path of the path cover
  Expr                guard;     // R_p, over entry values
  vector<Update>      updates;   // s_p, one per assigned slice
  StateId             next;
}

State {
  StateId             id;
  string              name;      // from the block label (§10.1), else ordinal
  SourceLoc           loc;       // for the waveform and the state map
  vector<Transition>  out;       // in source order; first match wins
}

Update {                         // an element of s_p
  Slice               target;    // variable + bit range, or dynamic
  Expr                value;     // over ENTRY values, never over updates;
}                                // may be a conditional — see below

Slice {
  string              name;
  optional<Range>     bits;      // absent = whole variable
  bool                dynamic;   // index not constant — see §C.5
}
```

Two remarks on shape.

`Transition::updates` holds expressions over **entry** values by
construction. That is the invariant the whole design rests on: if a value
in `updates` ever refers to another update, §6.1 has been violated and the
emission is wrong. It is cheap to assert and worth asserting.

`State::out` is ordered and first-match-wins, which is what lets the
emission be an `if/else if` chain rather than requiring the guards to be
proven disjoint.

**`Update::value` may be a conditional expression, and has to be.** §4 says
a branch whose arms hold no cut point "stays a plain conditional inside one
segment's action and costs no state"; the flat `vector<Transition>` above is
where that promise is kept or lost. If every branch forked the path list, a
segment holding *k* independent cut-point-free `if`s would produce 2^*k*
transitions — each with a full conjunctive guard and its own copy of the
updates the arms agree on — where the source has *k* conditionals. Eight of
them is 256 case-arm branches instead of eight nested `if`s, and the output
stops resembling A.2. So a cut-point-free branch is **if-converted into the
value**: `x` written in one arm and not the other yields the single update
`x := c ? e : x`, and the transition count stays proportional to the cut
points actually reachable from the state, never to the branches between
them. The emitter is then free to factor a guard shared by several updates
back out into an `if`, which is what A.2's nested form shows; that is a
readability choice on the way out, not a second representation.

**Where the v1 implementation stands relative to this shape.** Phases 2–4
keep a transition's action as the verbatim statement run — nonblocking
assignments plus the cut-point-free branches, held as statements — which is
exactly the factored form the previous paragraph lets the emitter print,
stored directly instead of if-converted and re-factored. The transition
count is the same (a cut-point-free branch never forks the walk), the
entry-value invariant holds by nonblocking semantics (every right-hand side
reads entry values), and the §6 checks do the per-arm merging on the
statement form: worst-arm addition for the commit count, arm intersection
for the must-defined sets. When §6.1's blocking temporaries landed, the
two-layer environment arrived without converting the actions: the walk
substitutes a `=` value — or its materialized wire's name — into every
later expression *before* it enters an action or a guard, so the statement
form stays fully resolved over entry values and the flat `Update` list
remains unnecessary. The conversion to this section's literal shape is now
motivated only by the §C.6 self-check and the v2 rows that compare values
across paths, and belongs to whichever lands first.

Alongside them, two side tables:

- **induced variables** — the countdowns and index registers §7.2's rolled
  lowering creates, the only storage the pass invents. They are storage by
  construction, never subject to the liveness question, and they must be
  distinguishable from the author's variables when the state map and the
  prefix (§10) are produced;
- **the environment**, during construction only: a map from `Slice` to
  `Expr`, in two layers — the entry values and the blocking updates, which
  is §6.1's two environments made concrete.

### C.4 The algorithm

1. **Normalise.** The existing pipeline (§10.3) runs first: parameters
   inlined, generates removed, structs lowered, loops without cut points
   unrolled. The process body reaching step 2 is structured statements over
   concrete types.

2. **Build the CFG, cutting at every `EventStatement`.** Structured
   statements map directly: `if`/`case` fork, sequences chain. Every
   bounded loop without `veriparse_no_unroll` is already gone —
   `LoopUnrolling` replicated it with the index substituted per copy
   (§7.2) — so the loops still standing are: a rolled `repeat`, **one
   state plus an induced countdown** (a constant count of 1 induces none:
   one wait, one state); a rolled or non-constant-bound `for`, **its
   body's states plus an induced index register** honouring the
   construct's contract (§7.2); `while`/`forever`, a back-edge on the
   loop condition; and `break`/`continue`, an edge to the loop's exit or
   head (§8).

   The init segment is set aside as the reset action (§5) and is not a
   state. A one-shot process additionally gets the §2 **hold state**
   appended: an empty state the last segment enters, self-looping with no
   updates.

3. **Enumerate the path cover.** For each state, walk forward to the next
   cut points, collecting branch conditions. A walk forks only at a branch
   **whose arms contain a cut point**; a cut-point-free branch does not
   fork it and is if-converted into the updates instead (§C.3), which is
   what keeps the path count proportional to the reachable cut points
   rather than exponential in the conditionals between them. Each distinct
   walk is one `Transition`. Loops do not multiply paths here — their
   back-edge ends a path like any other cut point, which is what keeps the
   enumeration finite without any bound on iterations.

   **A structurally contradictory guard prunes its path.** The
   enumeration is syntactic, so it also names paths no execution takes:
   A.1's `WAIT_SEND` forks at `if (!send)` and again at its
   `while (!send)`, and the leg that takes the `if` yet skips the `while`
   carries `!send && send` — where `busy` commits twice, tripping §6 on
   the ADR's own example if the path is kept. A contradictory guard is
   the empty set: the walk drops the leg at the fork, before judging or
   emitting anything, and the remaining legs still partition — the
   removed piece was empty, which is also why the last leg may still be
   printed as the bare `else`.

   The screen runs where the guard grows — each condition joins conjunct
   by conjunct, a compound test flattened into its parts — and knows the
   complement shapes the walk itself produces (structural equality, the
   §5.3 criterion, throughout): `X` against `!X`, a negated conjunction
   against its individually-present parts, the countdown's `cnt == 0`
   against `cnt != 0`, and two equalities pinning one expression to
   different folded constants. A conjunct the guard already carries is
   not conjoined again, so a test forked twice along one path reads once
   in the output; a constant `while` test folds at the loop head, which
   is what makes `while (1)` and `forever` the same machine (§2). All of
   it assumes a condition reads stably within its zero-time segment,
   which is why an impure call in condition position is rejected (§9) —
   pruning a "contradiction" between two evaluations of `$random` would
   delete a transition the source can take.

4. **Symbolically execute each path** to get `(R_p, s_p)`. One forward
   sweep with the two environments: a blocking assignment updates the
   blocking layer, a nonblocking one records an `Update` and leaves the
   entry layer alone, and a read resolves against the blocking layer if it
   has the slice, the entry layer otherwise. `R_p` accumulates the branch
   conditions taken, resolved the same way.

5. **Check the §6 discipline over the CFG.** There is no classification
   step — §6 already names every target: `<=` a register, `=` a scope-local
   temporary, the latter checked by scoping and dissolved by step 4. What
   needs the graph is read-before-assign: a forward reaching-definitions
   pass from the reset state verifies that every register read on some
   path out of reset is written by the init segment or preceded by a
   commit — otherwise the §9 error. The one-commit rule is per-path and
   already enforced while step 4 builds `s_p`. Induced countdowns skip the
   check.

6. **Allocate and encode states**, taking names from §10.1 and the encoding
   from `veriparse_encoding`, and emitting one `localparam` per state
   rather than an enum (§10).

7. **Emit** the `always_ff` and one `case` arm per state whose body is its
   transitions' guards and updates. The reset branch carries the init
   segment's updates plus the state register. Write the state map (§10.2).

Steps 2–5 are the generic half of §13; only 6 and 7 are FSM-specific.

### C.5 The cases that will bite

**A dynamic slice.** `data[i] = rx` with `i` a register cannot be tracked
per-slice — the environment does not know which bits changed. The write is
still emitted correctly (a dynamic bit-select assignment is ordinary RTL);
what breaks is a *later read of `data` in the same path*, which blocking
semantics say must observe the write. The rule: a dynamic write marks the
variable unknown in the blocking layer, and a subsequent read of it in the
same path is an error rather than a guess. Rare, and refusing it costs
nothing next to getting it silently wrong.

**A temp's declared width.** `logic [8:0] t; t = a + b; q <= t + c;` —
substituted inline, `a + b` evaluates at the surrounding context width
instead of truncating at 9 bits (§11.6), and the carry corner differs.
§6.1's materialization test is exact — a `wire` when context width and
declared width disagree — and the trap is that the two agree on almost
every stimulus that was not written to catch the corner.

**Reading an induced variable.** The counter is a register, so a guard
comparing it (`cnt == 0`) is over the entry value — the same rule as
everything else, but easy to get wrong by treating induced variables as
loop counters in the compiler's own sense.

**A path with no updates.** A pure wait state produces an empty `updates`
and a guard that is just the loop condition — the §2 hold state is all of
that with a constant-true guard. Both must reach emission intact rather
than be dropped as empty.

**Nested labels and folded states.** §10.1's two open items land here: a
state whose updates all fold away must disappear from the state table *and*
from the state map, not linger with an encoding.

### C.6 A self-check the structure makes cheap

Because a path is `(R_p, s_p)` and §11.1 establishes the bisimulation is
the identity, source and generated FSMD can be compared **path by path**:
same endpoints, same `R_p`, same `s_p`. Re-extracting the pair from the
generated RTL and diffing against the one the pass computed is translation
validation in the literature's own sense, at a fraction of its cost —
there is no bisimulation to search for.

Whether that lands as a debug mode or as a test-only harness is open. It is
recorded here because it is a property of the chosen representation, and
choosing a different one would forfeit it.

### C.7 References

The FSMD model and the path characterisation §C.1 borrows from are not
folklore. The entries below are split by **how far they were actually
read**, because the ADR's claims rest unevenly on them and an earlier draft
of this section got the mapping wrong: it presented the four PDFs held in
`docs/` as though they were the first entries of a list they are not in, so
"the definitions are read from the first two" pointed at two papers nobody
had opened. What §C.1 quotes comes from the four below.

None of them is committed (`.gitignore`): they are third-party and
redistribution-restricted, so each is identified by a full citation and,
where one exists, a DOI.

**Read in full text.** Four papers by the Karfa / Sarkar / Mandal group at
IIT Kharagpur, working copies in `docs/`. The definitions quoted in §C.1 —
the FSMD tuple `<Q, q0, I, V, O, f, h>`, the condition of execution `R_p`,
the data transformation `r_p = <s_p, O_p>`, the path cover, the computation
as a finite walk from the reset state, and the two cutpoint selection rules
with their justification *"cut each loop of the FSMD in at least one
cutpoint, because each internal loop has an exit point"* — are read from
these, and chiefly from the first two.

1. **C. Karfa, C. Mandal, D. Sarkar, S. R. Pentakota**, *"A Formal
   Verification Method of Scheduling in High-level Synthesis"*, 7th
   International Symposium on Quality Electronic Design (ISQED), 2006.
   <https://doi.org/10.1109/ISQED.2006.10> — `docs/ISQED06-eqck.pdf`.
   The FSMD definition as an ordered tuple, cutpoints, computations as
   concatenations of paths, the condition of execution and the data
   transformation: the `(R_p, s_p)` characterisation §C.1 and §C.3 are
   built on.

2. **C. Reade, C. Karfa, D. Sarkar, C. Mandal**, *"Hand-in-hand
   Verification of High-level Synthesis"*, GLSVLSI '07, 2007.
   <https://doi.org/10.1145/1228784.1228911> — `docs/glsvlsi07.pdf`.
   The tuple written out as `<Q, q0, I, V, O, f, h>`, the *computation*
   defined as a finite walk from the reset state back to itself — the
   definition §11.1 records as not covering an `initial` process — and the
   cutpoint selection rules §C.1 discharges differently.

3. **C. Karfa, D. Sarkar, C. Mandal**, *"Verification of Data-path and
   Controller Generation Phase of High-level Synthesis"*, ADCOM 2007 —
   `docs/karfa-adcom07.pdf`. The phase *after* scheduling: recovering the
   register-transfer operations from generated RTL and checking them
   against the scheduled FSMD. That recovery is the shape of §C.6's
   path-by-path self-check.

4. **C. Karfa, K. Banerjee, D. Sarkar, C. Mandal**, *"Equivalence Checking
   of Array-Intensive Programs"*, IEEE Computer Society Annual Symposium on
   VLSI (ISVLSI), 2011. <https://doi.org/10.1109/ISVLSI.2011.24> —
   `docs/ISVLSI-2011-A-IEEE.pdf`. What it costs to reason about indexed
   storage across paths, which is why §C.5 *refuses* a dynamic slice read
   rather than solving it.

**Read as bibliographic records and abstracts only.** Not held locally and
not opened; cited for positioning, and to be checked before any argument is
made to lean on them — the misattribution above is what that caution is for.

5. **C. Karfa, D. Sarkar, C. Mandal, P. Kumar**, *"An Equivalence-Checking
   Method for Scheduling Verification in High-Level Synthesis"*, IEEE
   Transactions on Computer-Aided Design of Integrated Circuits and
   Systems, vol. 27, no. 3, pp. 556–569, 2008.
   <https://ieeexplore.ieee.org/abstract/document/4391074>
   — the journal treatment of entry 1 by the same group; the fuller account
   to read first if §C.4 turns out to need more than entry 1 gives.

6. **T. Li, Y. Guo, W. Liu, M. Tang**, *"Translation validation of
   scheduling in high level synthesis"*, GLSVLSI '13, pp. 101–106, 2013.
   <https://doi.org/10.1145/2483028.2483070>
   — translation validation of a scheduler, the expensive problem §11.1
   explains this pass does *not* have. **This is not `glsvlsi07.pdf`**,
   which is entry 2; conflating the two is the error this section corrects.

7. **K. Banerjee, D. Sarkar, C. Mandal**, *"Deriving bisimulation relations
   from path based equivalence checkers"*, Formal Aspects of Computing,
   vol. 29, no. 2, pp. 365–379, 2017.
   <https://doi.org/10.1007/s00165-016-0406-y>
   — the bridge between path-based checking and bisimulation. §11.1 claims
   the bisimulation here is the identity; this is the frame in which that
   claim means something, and it is the one entry §11.1 would need read in
   full before the claim is called proved rather than argued.

8. **P. Coussy, D. D. Gajski, M. Meredith, A. Takach**, *"An Introduction
   to High-Level Synthesis"*, IEEE Design & Test of Computers, 2009.
   <http://masters.donntu.org/2018/fknt/khvishuk/library/article10.htm>
   — scheduling and binding as the two phases of HLS, and the CDFG that
   §C.2 deliberately refuses to build. The FSMD model is Gajski's; see also
   *Embedded System Design: Modeling, Synthesis, Verification*, Springer,
   2009, chapter "Finite State Machine with Datapath".

9. **CIRCT**, *Handshake Dialect Rationale*.
   <https://circt.llvm.org/docs/Dialects/Handshake/RationaleHandshake/>
   — the open-source point of comparison in §14: dataflow circuits with
   static or dynamic scheduling, lowered from MLIR.

10. SystemC `SC_CTHREAD` as commercial HLS treats it — a clocked thread with
    `wait()` spanning cycles, a reset preamble, synthesised into an FSM —
    documented by the vendors rather than in a paper:
    <https://semiwiki.com/eda/cadence/8080-update-on-systemc-for-high-level-synthesis/>
    and <https://www.cadence.com/en_US/home/tools/digital-design-and-signoff/synthesis/stratus-high-level-synthesis.html>.
    §5's reset convention and §14's positioning rest on this.

**Not found, and worth recording as such**: no open-source tool performing
this source-to-source lowering on Verilog, and no confirmable documentation
of Synopsys Behavioral Compiler's cycle-fixed mode — mentioned in early
discussion, deliberately absent from this ADR because it could not be
sourced.
