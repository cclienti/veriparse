# ADR-0014 — Implicit-FSM elaboration (`ImplicitFsmElaboration`)

- **Status**: Proposed (2026-08-01, this branch — nothing implemented yet;
  §12 phases each land green).
- **Date**: 2026-08-01
- **Origin**: This ADR formalises `docs/imperative-fsm-design.md`, the
  design note that groomed the approach against two RS232 examples. That
  note's decisions carry over unless stated otherwise; §3 records the one
  it reverses (attributes) and §7.2 the one it corrects (bounded loops).
- **Scope**: A per-module pass that compiles a **multi-cycle imperative
  process** — an `always`/`initial` body carrying its own `@(posedge clk)`
  controls, the style a firmware engineer bit-banging a GPIO would write —
  into an explicit synthesizable **FSMD** (finite state machine with
  datapath): a state register, an `always_ff`, and the per-state actions.
  The input is *already scheduled*: the author placed the edge waits, so
  the pass performs no operation scheduling, resource allocation or
  pipelining. Explicitly **not** in scope: general HLS (§15), multi-clock
  processes, `fork`/`join`, datapath generation, and memory inference
  policy.
- **Home**: a new tool, `vericomp`, not `veriflat`. The transformation
  compiles behaviour into structure, which is a different job from
  flattening a hierarchy, and §13 records that it is the first of a family
  — the CFG construction and liveness analysis below are generic, only the
  RTL emission is FSM-specific.
- **Hard constraint, inherited unchanged**: the source must run as-is in
  any conforming simulator. Anything that would require veriparse to run
  before a behavioural simulation is rejected — this rules out a custom
  block keyword and any "one statement = one cycle" scheme.
- **Normative reference** — IEEE 1800-2017, verified against
  `docs/1800-2017.pdf`:
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
  - **§10.4.1 / §10.4.2** — blocking vs nonblocking procedural
    assignments; §6 turns their difference into the storage decision.
  - **§12.7** — loop statements (`for`, `while`, `repeat`, `forever`).
  - **§5.12** — attributes: *"A mechanism is included for specifying
    properties about objects, statements, and groups of statements in the
    SystemVerilog source that can be used by various tools… without
    standardizing on any particular attributes."* §3 rests on this.
- **Relationship to prior ADRs**:
  - **ADR-0007** owns the synthesizable-subset verdict. This ADR does not
    widen that subset: it *translates* a process out of the unsynthesizable
    region into it, and §9 rejects what it cannot translate. The output
    must satisfy ADR-0007 unchanged.
  - **ADR-0005 §3.2.1** deferred mixed/nested jump lowering behind a flag
    scheme. `break`/`continue` are nevertheless **in scope here** (§8):
    once states are explicit a jump is an edge in the CFG, which is
    strictly easier than that flag lowering and needs none of it.
  - **ADR-0011** (`StructLowering`) and **ADR-0012**
    (`DefaultResolution`) run before this pass, so every variable §6
    classifies already has an explicit type, net kind, direction and a
    defined bit layout.

## 1. Problem — the scheduled description is not synthesizable, and the RTL is not simulable against it

Writing FSMs by hand is the most tedious part of RTL work: state register,
next-state logic, output decode, all kept in sync manually. C/C++ HLS
solves it but lives outside the Verilog universe — no direct access to
surrounding signals, ceremony at every boundary.

Sequential control logic is naturally written as a *program over time*:

```systemverilog
always begin
  @(posedge clk);
  done <= 1'b0;
  acc  <= 8'd0;
  while (!start) @(posedge clk);
  acc <= 8'd1;
  @(posedge clk);
  acc <= acc + 8'd2;
  @(posedge clk);
  done <= 1'b1;
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

## 2. Decision 1 — the construct is the intent

The pass detects two source patterns; no keyword and no *required*
annotation participates in the decision.

| Source construct | Meaning | Terminal behaviour |
|---|---|---|
| `always begin @… @… end` | perpetual FSM | last segment transitions back to the first |
| `initial begin @… @… end` | one-shot sequence | last segment self-loops (terminal state) |
| `initial forever begin … end` | perpetual | the `forever` back-edge (§7.3) makes it so; accepted, redundant |

This is exactly the §9.2.1 / §9.2.2 distinction, so the terminal behaviour
is read off the standard rather than invented.

An `Always` with a **non-empty** `senslist` (`always @(posedge clk)`,
`always_ff`, `always_comb`) is already RTL and is **left untouched** — this
pass does not rewrite existing hardware descriptions.

A module item is a **schedulable process** when it is an `Always` with an
empty `senslist`, or an `Initial`, whose body contains at least one
`EventStatement`, and where every `EventStatement` carries exactly one
`Sens` of the same edge type over the same single-bit signal. One clock,
one edge, per process; §9 rejects the rest. A module may hold several
schedulable processes plus ordinary RTL; each yields its own independent
state register and `always_ff`.

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
(§5), why the unroll-or-count choice on a bounded loop is the author's
(§7.2), and why state names are written rather than generated (§10.1). It
also sets the bar for anything added later: a new heuristic ships as an
attribute first, and only becomes a default once there is evidence the
default is right.

Binding is **structural, not positional**: the parser nests the attributed
item inside `Pragmalist.statements`, so a schedulable process carries its
hints by being that list's element. The generator already round-trips the
form (`render_pragmalist` emits `(* … *)` when the statement list is
non-empty), so nothing is lost on output.

| Attribute | Effect | Default when absent |
|---|---|---|
| `fsm_reset = "<port>"` | names the reset input | inferred (§5) |
| `fsm_reset_level = 0\|1` | active level | `1`, or `0` when the name ends in `_n` |
| `fsm_reset_kind = "sync"\|"async"` | reset flavour of the generated `always_ff` | `"sync"` |
| `fsm_encoding = "binary"\|"one_hot"\|"gray"` | state encoding | `"binary"` |
| `fsm_prefix = "<id>"` | prefix for generated declarations | `__fsm` (§10) |
| `fsm_unroll` on a bounded loop | unroll into consecutive states instead of a counter | counter (§7.2) |
| `fsm_state = "<name>"` | names the state a segment becomes | ordinal (§10.1) |

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
- The statements **before the first** cut point form the **init segment**
  (§5), not an ordinary state.
- `IfStatement`/`CaseStatement` fork the graph; loops close a back-edge
  (§7); `break`/`continue` are edges (§8). A branch whose arms contain no
  cut point stays a plain conditional inside one segment's action and
  costs no state.
- Branches of unequal length are **not padded**: each takes the cycles it
  needs and the last state of each arm transitions to the same merge state.

## 5. Decision 4 — reset: the init segment, with the signal inferred or hinted

The init segment (§4) executes once at time zero in simulation — exactly
when reset is asserted in a real system — and becomes the **reset branch**
of the generated `always_ff`, with the state register reset to the first
segment. One piece of source means the right thing in both worlds.

This convention is not invented here. A SystemC `SC_CTHREAD` — a thread
suspended by a clock, carrying `wait()` calls that span cycles, and
synthesised into an FSM by commercial HLS — is written with exactly this
shape: a **reset preamble** ahead of the first `wait()`. Two designs
arriving independently at the same convention is the strongest argument
available for it.

The reset *signal* cannot be derived from behavioural source, so:

1. if `fsm_reset` is present (§3), it names the port;
2. otherwise the pass looks for exactly one single-bit module **input**
   whose name matches `rst_n`, `resetn`, `aresetn`, `rst`, `reset`
   (case-insensitive), taking its active level from the `_n` suffix;
3. if there is no match, or more than one, that is an **error** naming the
   candidates and pointing at `fsm_reset`.

Rule (2) keeps the note's "no pragma required" property true for ordinary
designs — both RS232 examples in §11 declare `rst_n` and need no attribute
— while (3) refuses to guess. An unresettable state register is a
synthesis defect, so silence is not an option in either direction.

## 6. Decision 5 — storage from liveness; `=` in the source is not a mistake

**Rule.** A variable assigned in one segment and read in a segment
reachable from it *across at least one cut point* is live across an edge
and becomes a **register**. A variable whose every definition-to-use chain
stays within one segment is a combinational temporary local to that
segment's action. Loop counters and `repeat` countdowns (§7) are registers
by construction.

The imperative style writes `=` throughout — it is the natural form inside
`initial`, and both RS232 examples use it exclusively. That is **not** a
defect to warn about. The pass translates:

- assignments to **module-level signals** and to **cross-state registers**
  become nonblocking (`<=`) in the generated `always_ff`, which is what
  gives the FSM the same cycle-by-cycle behaviour the source had under
  §10.4.2;
- assignments to **pure intra-state temporaries** stay blocking inside
  their state's action, where §10.4.1 straight-line semantics are exactly
  what is wanted.

A source that already uses `<=` is unaffected: its targets are live across
a cut by construction and land in the same place.

A variable **read before assigned** on the first path out of reset takes
its value from the init segment. If it has none, that is an error, not an
`x`: the behavioural source would propagate `x` while the synthesized
register would come up at whatever reset gives it, and the two would
disagree exactly where debugging is hardest.

### 6.1 Within-segment semantics — translation is substitution, not rewriting

"Translate `=` into `<=`" is the right slogan and the wrong algorithm.
Rewriting each assignment in place inverts the source's meaning, because
§10.4.1 and §10.4.2 differ precisely on what a *later statement in the
same segment* sees:

```systemverilog
acc = acc + 1;
q   = acc;        // blocking: q takes the NEW acc
```

emitted naively as `acc <= acc + 1; q <= acc;` gives `q` the **old** `acc`
— off by one cycle, and silent.

**Rule.** A segment is straight-line code executed in one cycle, so the
pass evaluates it symbolically and emits, per target, a single nonblocking
assignment carrying that target's **final value at the end of the
segment**. The example above yields `acc <= acc + 1; q <= acc + 1;`. Two
environments are threaded through the segment:

- the **pre-edge** environment — the value every register held when the
  state was entered. Reads of a target that has only been written by `<=`
  in this segment resolve here, which is exactly §10.4.2: the update is
  not visible until the NBA region;
- the **blocking** environment — updated in place by each `=`, so
  subsequent reads in the same segment see the new value, per §10.4.1.

Four consequences worth stating, because each is a place a
straightforward implementation goes wrong:

1. **Repeated assignment.** Several `=` to one target collapse to the last
   one *after* substitution. Several `<=` to one target also keep the
   last, per the §10.4.2 ordering within a time step.
2. **Partial targets.** `data[i] = rx;` updates one slice and leaves the
   rest holding. The environments therefore track **per-slice** values,
   not whole variables; a whole-variable model would silently drop the
   surviving bits. RS232 RX depends on this.
3. **Mixing the two forms on one target inside a process is an error.**
   `x = …` and `x <= …` reaching the same variable is a race in the
   source, so there is no meaning to preserve — reject rather than pick.
4. **Sampling point.** Signals a segment *reads* resolve to the values
   sampled at the edge that entered the state, which is precisely
   `always_ff` RHS sampling. This is what makes the two descriptions agree
   whenever the signal is driven by nonblocking assignment — the ordinary
   RTL case. A signal driven by a *blocking* assignment from another
   process is already a race in the behavioural source, and the pass does
   not invent a resolution for it.

The rule also explains why §6's classification is safe: a target whose
final value never leaves the segment was only ever a name for a
subexpression, so substitution removes it entirely and no flip-flop is
inferred.

## 7. Decision 6 — loops

### 7.1 Loops with no cut point

Untouched. `LoopUnrolling` and the folding passes already own them.

### 7.2 Bounded loops *containing* a cut point — counter, not unroll

`repeat (N) @(posedge clk);` with `N` constant becomes **one state with a
countdown register**, exiting at zero. A `for` with constant bounds whose
body contains a cut point becomes a **group of states plus an
iteration-counter register**, with a loop edge until the bound is reached.

Neither is unrolled. This **corrects** the placement sketch in
`docs/imperative-fsm-design.md`, which put the pass after `LoopUnrolling`
while also specifying counters: `LoopUnrolling` handles `RepeatStatement`
and `ForStatement` today, so on the note's own RS232 TX example
`repeat (BAUD_DIV) @(posedge clk)` with `BAUD_DIV = 100` it would emit
**100 states** instead of one state and a counter.

**Prerequisite change, with its own commit and test:** `LoopUnrolling`
must skip any loop whose body contains an `EventStatement`, leaving it
intact for this pass. Loops without cut points keep unrolling exactly as
today. That single guard makes the ordering in §11 safe.

**Which of the two the author gets is theirs to choose** (§3 rule 2). A
counter is right for `repeat (BAUD_DIV)`; four states in a row may well
beat one state plus an index register for `for (i = 0; i < 4; i++)`, and
nothing in the source says which. `fsm_unroll` on the loop selects
unrolling; absent it, the counter is the default because it is the choice
that cannot explode.

### 7.3 Data-dependent loops containing a cut point

`while (cond) …`, `forever …`, and a `for` whose bound does not fold become
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

## 8. Decision 7 — `break` and `continue` are CFG edges

`continue` transitions to the first state of the innermost enclosing loop;
`break` transitions to the first state *after* it. Both are in scope for
v1 — the note's RS232 RX example depends on `continue`, and once states are
explicit a jump is just an edge, needing none of the flag lowering
ADR-0005 §3.2.1 deferred. A `break`/`continue` in a loop containing **no**
cut point is not this pass's business and is left to the existing passes.

## 9. Errors — rejected loudly, never silently mis-lowered

| Condition | Why | Clause |
|---|---|---|
| cut points over different signals, or mixed `POSEDGE`/`NEGEDGE`, in one process | multi-clock scheduling is out of scope; picking one would mis-compile | §9.4.2 |
| `DelayStatement` (`#d`) in a schedulable process | simulation timing with no static hardware meaning | — |
| `WaitStatement` / level-sensitive control | not an edge; no boundary to cut at | §9.4.3 |
| `fork`/`join`, `disable` | concurrent or abortive control flow the state model cannot express | §9.3.2 |
| cut point inside a called `function`/`task` | not visible in the process body; v1 does not inline to find it | §13 |
| loop with no cut point and no static exit | zero-delay infinite loop — deadlock, and no hardware | §9.2.2.1 |
| reset signal neither hinted nor uniquely inferable | an unresettable state register is a synthesis defect | §5 |
| variable read before assignment out of reset, with no init value | source and RTL disagree where it is hardest to debug | §6 |
| a target written by two schedulable processes, or by one and any other process | §9.2.2.4: *"Variables on the left-hand side of assignments within an `always_ff` procedure … shall not be written to by any other process."* The source is merely a race; the **output would not conform**, which is the stronger reason to refuse | §9.2.2.4 |
| two concurrent statements in one imperative block | outside the sequential model | — |

## 10. Generated form, naming, and pass placement

Output is plain `always_ff` with an explicit state enum and a
`case (state)`, plus the inferred counter and index registers — accepted by
every downstream tool. Enumerators carry the source line so a waveform is
readable.

Generated declarations take the `fsm_prefix` (default `__fsm`), settling
the note's open question on collisions: readable in a waveform, safe
against user identifiers. A collision remaining after prefixing is an
error, not a silent rename.

### 10.1 Naming the states

A generated name (`__fsm_state_47`) is useless in a waveform, and the pass
has no way to invent a good one — §3 rule 2 applies, so the author writes
it. Two candidate spellings, to settle before phase 2:

- **an attribute** on the segment's first statement, uniform with every
  other hint here;
- **a SystemVerilog block label**, `begin : IDLE`, which is already legal,
  already simulable, and names the code rather than annotating it.

The label is the nicer notation and costs no new syntax, but block scope
and segment boundaries are different things — a labelled block can hold
several cut points, or none. The likely answer is to accept a label *when
the block is exactly one segment* and fall back to the attribute
otherwise; that has to be checked against a real design before it goes in.

Unnamed states keep an ordinal, so naming is incremental: name the four
that matter and leave the rest.

### 10.2 The state map

The encoding is an implementation choice the source does not contain, so
it must be emitted, not reverse-engineered from the RTL. `vericomp` writes
a **JSON state map** beside the output: per process, the state variable's
name and width, then per state its encoded value, its name, and the source
line its segment starts at.

JSON is the canonical form and nothing else is generated from inside the
tool. Waveform viewers each want their own file — GTKWave a translate
filter, Surfer its own translator — and that knowledge already lives in
`wavedisp`, which drives all of them from one description and already has
`radix='symbolic'`. Emitting viewer formats here would duplicate it and
guarantee drift; emitting JSON lets `wavedisp` consume it.

### 10.3 Placement in the `ResolveModule` pipeline

```
… StructLowering → ScopeElevator → LoopUnrolling → BranchSelection
  → GenerateRemoval → ImplicitFsmElaboration
  → ConstantFolding → VariableFolding → DeadcodeElimination → …
```

after `GenerateRemoval` so the CFG is over real control flow rather than
generate structure; after `LoopUnrolling` — made safe by the §7.2 guard —
so §7.1 loops are already flat; and **before** the second
`ConstantFolding`/`VariableFolding`/`DeadcodeElimination` so the generated
FSM is folded and cleaned by the passes that already own that work, which
is the note's "before flatten/dead-code keeps the output clean".

Two interactions to **verify in phase 2 rather than assume**:
`VariableFolding` and `DeadcodeElimination` both already walk
`WhileStatement`, and neither was written with a suspended process in mind.
If either mishandles a body containing an `EventStatement`, the fix belongs
in that pass with its own test.

## 11. Validation — differential cosimulation against the same testbench

The property motivating the design is that **the input is simulable**, so
the golden model is free.

- Verilator 5.050 (the pinned toolchain) elaborates the behavioural form
  under `--timing`; the generated FSM needs no such flag.
- A cosim test builds **both** and drives them from **one** testbench,
  comparing outputs cycle by cycle — the harness shape `veriflat` already
  uses, with the behavioural source in the reference slot.
- The two RS232 examples from the design note are the primary corpus: TX
  exercises `forever`, `while`, `repeat`, a `for` with a cut point and a
  shared counter; RX adds input sampling and `continue`. Between them they
  cover every row of §7 and §8.
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

What remains to be argued is strictly smaller: that the transformation
*within* one segment is faithful. That is exactly §6.1's substitution
rule — final value per target, pre-edge environment for reads of
nonblocking-assigned variables, per-slice tracking for partial writes.
Restating the obligation this way is worth more than any amount of extra
testing: it says which single page of reasoning the correctness of the
pass rests on.

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

Uncovered items are reported, not silently tolerated. Constrained-random
stimulus over the process inputs runs on top of the directed cases to
reach what they miss.

This is what makes the §11.1 argument operational: the identity
bisimulation says the control structures match, and full edge coverage
says the datapath transformation was exercised on every one of them.

## 12. Phasing (each lands green) & test plan

1. **Prerequisite** — the §7.2 `LoopUnrolling` guard, own commit and test.
2. **Straight-line.** `Always`, no branches, no loops: cut, segment, state
   register, `always_ff`, reset (§5). Golden + a 3-state cosim. Lands the
   §11 harness and the §10 interaction checks.
3. **Branches** — `IfStatement`/`CaseStatement` with cut points in one or
   more arms, unequal lengths, merge state.
4. **Loops** — §7.2 counters, then §7.3 back-edges, then §8 jumps. RS232 TX
   becomes a cosim test here.
5. **`Initial`** — one-shot with the terminal state; RS232 RX joins.
6. **Diagnostics** — the whole §9 table, one `TEST_ERROR_SV` each.
7. **Hints and the state map** — the §3 table beyond `fsm_reset`
   (encoding, prefix, `fsm_unroll`), the §10.1 naming decision, and the
   §10.2 JSON, with a `wavedisp` description consuming it end to end.

## 13. Beyond FSMs — what the structure has to allow

`vericomp` is named for a family, not for this pass. The FSM lowering is
the first behavioural transformation, and others will want the same front
half, so the split is structural from the start:

- **generic** — building the CFG from a suspended process, cutting it at
  the timing controls, computing liveness across the cuts, and the
  within-segment substitution of §6.1. None of this mentions state
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
  simulable by the tools already in the flow. §5's reset convention is
  theirs, arrived at independently.
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

| Feature | v1 behavior | Future home |
|---|---|---|
| Mealy outputs | Moore only — for a nonblocking target the equivalence with the source is exact segment by segment, whereas Mealy moves the observable timing inside the cycle | user-written `assign` outside the block today; a v2 rule if that proves insufficient |
| Three-process emission (state register / next state / output decode) | one `always_ff` with a `case` (§10) | v2. The preferred style for synthesis and for reading, but the segment model puts the transition and the registered outputs in the same process, so the split is not the mechanical one it looks like — it needs its own decision |
| Output encoding (outputs carried by the state encoding itself) | not attempted | v2, as an `fsm_encoding` value beside binary/one-hot/gray |
| Counter splitting | one shared countdown per process, re-initialised on entry | post-v1 optimisation, if routing says so |
| Multiple clocks or mixed edges | hard error (§9) | needs a CDC model, own ADR |
| Cut point inside a `function`/`task` | hard error (§9) | subroutine inlining before the cut walk |
| Loop count from a non-constant expression not captured at entry | hard error | capture-at-entry lowering |
| Resource sharing, scheduling, pipelining, datapath generation | none — the author's edges *are* the schedule | out of scope by construction; this pass is not a prefix of full HLS |
| Memory inference policy (BRAM vs registers) | none | orthogonal |

## Appendix A — Worked example: RS232 transmitter

Every rule above, on one design. The source is the TX from
`docs/imperative-fsm-design.md`; the RTL beside it is what this ADR
specifies the pass must produce, so it doubles as the acceptance criterion
for phase 4.

### A.1 Before — the behavioural source

```systemverilog
module rs232_tx #(parameter int BAUD_DIV = 100) (
  input  logic       clk, rst_n,
  input  logic [7:0] data,
  input  logic       send,
  output logic       tx, busy
);
  initial begin
    tx = 1'b1; busy = 1'b0;                    // init segment  -> reset branch
    @(posedge clk);
    forever begin                              // back-edge
      while (!send) @(posedge clk);            // wait state
      busy = 1'b1;
      tx = 1'b0; repeat (BAUD_DIV) @(posedge clk);   // counter state
      for (int i = 0; i < 8; i = i + 1) begin        // counter + index
        tx = data[i];
        repeat (BAUD_DIV) @(posedge clk);
      end
      tx = 1'b1; repeat (BAUD_DIV) @(posedge clk);
      busy = 1'b0;
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
  typedef enum logic [1:0] {
    __fsm_WAIT_SEND, __fsm_START, __fsm_DATA, __fsm_STOP
  } __fsm_state_t;

  __fsm_state_t                __fsm_state;
  logic [$clog2(BAUD_DIV)-1:0] __fsm_cnt;
  logic [2:0]                  __fsm_i;

  always_ff @(posedge clk) begin
    if (!rst_n) begin
      tx          <= 1'b1;              // the init segment, verbatim
      busy        <= 1'b0;
      __fsm_state <= __fsm_WAIT_SEND;
    end
    else case (__fsm_state)

      __fsm_WAIT_SEND:
        if (send) begin                 // exit and the statements after the
          busy      <= 1'b1;            // loop are ONE segment, so `busy = 0`
          tx        <= 1'b0;            // and `busy = 1` collapse per §6.1
          __fsm_cnt <= BAUD_DIV - 1;
          __fsm_state <= __fsm_START;
        end
        else busy <= 1'b0;

      __fsm_START:
        if (__fsm_cnt == 0) begin
          tx        <= data[0];
          __fsm_i   <= 3'd0;
          __fsm_cnt <= BAUD_DIV - 1;
          __fsm_state <= __fsm_DATA;
        end
        else __fsm_cnt <= __fsm_cnt - 1;

      __fsm_DATA:
        if (__fsm_cnt == 0) begin
          if (__fsm_i == 3'd7) begin
            tx        <= 1'b1;
            __fsm_cnt <= BAUD_DIV - 1;
            __fsm_state <= __fsm_STOP;
          end
          else begin
            __fsm_i   <= __fsm_i + 3'd1;
            tx        <= data[__fsm_i + 3'd1];   // NOT data[__fsm_i] — see §A.3
            __fsm_cnt <= BAUD_DIV - 1;
          end
        end
        else __fsm_cnt <= __fsm_cnt - 1;

      __fsm_STOP:
        if (__fsm_cnt == 0) __fsm_state <= __fsm_WAIT_SEND;
        else __fsm_cnt <= __fsm_cnt - 1;

    endcase
  end
endmodule
```

### A.3 What the example pins

- **Four states, not five.** `docs/imperative-fsm-design.md` counted an
  `S_INIT`. Under §5 the init segment is the *reset branch*, not a state,
  so it disappears — one fewer state and one fewer cycle out of reset.
  A consequence of the convention worth seeing before committing to it.
- **`data[__fsm_i + 3'd1]`, not `data[__fsm_i]`.** This is §6.1 doing real
  work. In the source, `i = i + 1` is a blocking assignment, so
  `tx = data[i]` on the next iteration reads the *incremented* i. In the
  FSM, `__fsm_i` is a register updated by a nonblocking assignment, so it
  still holds the old value in this cycle. The index must be substituted
  forward. Emitting `data[__fsm_i]` transmits every bit one position late
  — a bug that simulates plausibly and is exactly what hand-translation
  gets wrong.
- **`busy` collapses.** The `busy = 1'b0` that ends the previous byte and
  the `busy = 1'b1` that starts the next are in the same segment; the
  substitution rule keeps the last one on each path, giving the `if/else`
  shown rather than two conflicting assignments.
- **One shared countdown.** `repeat (BAUD_DIV)` in three different places
  reuses `__fsm_cnt`, re-loaded on entry to each state (§13 of the design
  note). Splitting it is a post-v1 optimisation.

## Appendix B — I2C byte write: what a second protocol adds

RS232 exercises every rule, but only one at a time. An I2C master phase is
the useful second example because its loop body holds **two** cut points,
so §7.2 produces a *group* of states rather than one, and because it
samples an input to decide what happens next.

### B.1 Before

```systemverilog
always begin
  @(posedge clk);
  while (!go) @(posedge clk);

  sda_out <= 1'b0;  repeat (T_HD_STA) @(posedge clk);   // START condition
  scl     <= 1'b0;

  for (int i = 7; i >= 0; i = i - 1) begin              // two cut points in
    sda_out <= byte_out[i];                             // one loop body
    repeat (T_LOW)  @(posedge clk);
    scl <= 1'b1;
    repeat (T_HIGH) @(posedge clk);
    scl <= 1'b0;
  end

  sda_oe <= 1'b0;                                       // release for ACK
  repeat (T_LOW)  @(posedge clk);
  scl <= 1'b1;
  repeat (T_HIGH) @(posedge clk);
  ack    <= ~sda_in;                                    // sample the slave
  scl    <= 1'b0;
  sda_oe <= 1'b1;
end
```

### B.2 After — the control graph

Written as the state graph rather than full RTL; the emission follows §A.2
mechanically.

| State | Action | Leaves when | To |
|---|---|---|---|
| `IDLE` | — | `go` | `START` |
| `START` | `sda_out<=0`, load `T_HD_STA` | count 0 | `BIT_LOW` |
| `BIT_LOW` | `scl<=0`, `sda_out<=byte_out[i]`, load `T_LOW` | count 0 | `BIT_HIGH` |
| `BIT_HIGH` | `scl<=1`, load `T_HIGH` | count 0 and `i>0` | `BIT_LOW` (i−1) |
| | | count 0 and `i==0` | `ACK_LOW` |
| `ACK_LOW` | `scl<=0`, `sda_oe<=0`, load `T_LOW` | count 0 | `ACK_HIGH` |
| `ACK_HIGH` | `scl<=1`, load `T_HIGH` | count 0 | `IDLE` (`ack<=~sda_in`, `sda_oe<=1`) |

Three things it adds over RS232:

- **A loop body spanning two states.** `BIT_LOW`/`BIT_HIGH` are one
  iteration; the back-edge returns to `BIT_LOW` with the index
  decremented. §7.2's "group of states plus an iteration counter" is this,
  and RS232 never shows it because its body holds a single cut point.
- **An input sampled at a known edge.** `ack <= ~sda_in` reads `sda_in` as
  of the edge leaving `ACK_HIGH` — §6.1's sampling point, and the reason
  the behavioural source and the FSM agree on *when* the slave is read.
- **A three-signal bus model.** SDA is written as `sda_out`/`sda_oe`/
  `sda_in` rather than an `inout`, because the flattener does not support
  `inout` ports (`Inout port not supported during flattening`). That is a
  veriparse limitation, not an I2C one, and this example is a good reason
  to lift it.
