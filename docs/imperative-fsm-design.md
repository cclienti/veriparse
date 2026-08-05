# Imperative-style FSM elaboration — design notes

## Motivation

Writing FSMs in Verilog by hand is the most tedious part of RTL development —
you write the state register, the next-state logic, the output decode, and
keep them in sync. C/C++-based HLS tools solve this but introduce friction:
the HLS code lives outside the Verilog universe, has no direct access to
surrounding signals, and forces ceremony at every boundary (AXI wrappers,
function-call interfaces, etc.).

The proposal: keep the entire design in Verilog/SystemVerilog and add a
single veriparse pass that compiles imperative-looking sequential code —
the kind of code a firmware engineer would naturally write to bit-bang a
GPIO — into an explicit synthesizable FSM. The source remains a valid SV
that any standard simulator runs as-is; veriparse becomes a synthesis-only
transformation.

## Scope

**In scope:**

- Multi-cycle imperative sequencing inside one block.
- `if`/`else`, `while`, `for` with constant bounds, `forever`.
- `continue` / `break` inside loops.
- Live-variable analysis to lift cross-state variables into registers.
- Automatic state register allocation, encoding, and next-state logic.
- Reading and writing module-level signals at wait points.

**Explicitly out of scope (general HLS territory):**

- Resource scheduling and allocation.
- Pipelining, II tuning.
- Datapath generation from arithmetic expressions.
- Loops whose iteration count is computed from a non-constant expression
  evaluated dynamically (e.g. `repeat (some_signal)`), unless the count
  is captured synchronously at loop entry.
- Tasks/functions containing wait points.
- Memory inference policy (BRAM vs register vs distributed).
- Two concurrent statements in the same imperative block.

## Hard constraint: simulability

The source must be runnable as-is by any standard SV simulator. Any pattern
that breaks the contract with the simulator is rejected. This rules out
"1 statement = 1 cycle" implicit schemes — §10.4 makes them silently
diverge from the synthesised behaviour.

## Source syntax: `always` and `initial`, the construct is the intent

The pass detects two source patterns. **The choice of construct *is* the
signalling**, no new keyword or attribute is required.

| Source construct | Meaning | Generated terminal behaviour |
|---|---|---|
| `always begin @... @... end` | Perpetual FSM | After last state, transition back to first state |
| `initial begin @... @... end` | One-shot sequence | Last state self-loops (terminal); optional `done` strobe |
| `initial forever begin ... end` | Equivalent to `always` | Same as `always`, redundant but accepted |

Both forms are standard SV and simulate natively. The synthesiser-rejected
parts (`initial` with `@`, `always` containing `@` events that suspend the
process) are what veriparse rewrites.

Rationale for *not* using a custom keyword or attribute:

- Zero new syntax to teach.
- The source is consumable by any SV tool for simulation without
  running veriparse first.
- The construct itself communicates intent — `initial` for one-shot
  sequences, `always` for perpetual FSMs — without ambiguity.
- Lint tools warn that this construct isn't synthesisable; that's
  correct and the right signal to the developer ("this needs the
  veriparse pass before synthesis").

## Validation by example: RS232 transmit and receive

The design was groomed against two concrete examples, both written in the
naive "firmware engineer bit-banging a GPIO" style. The mapping to FSM is
mechanical and unambiguous for every construct that appears.

### Example 1 — RS232 TX (perpetual FSM)

```sv
module rs232_tx #(parameter int BAUD_DIV = 100) (
  input  logic clk, rst_n,
  input  logic [7:0] data,
  input  logic send,
  output logic tx, busy
);
  initial begin
    tx = 1'b1; busy = 1'b0;
    @(posedge clk);
    forever begin
      while (!send) @(posedge clk);
      busy = 1'b1;
      tx = 1'b0; repeat (BAUD_DIV) @(posedge clk);
      for (int i = 0; i < 8; i = i + 1) begin
        tx = data[i];
        repeat (BAUD_DIV) @(posedge clk);
      end
      tx = 1'b1; repeat (BAUD_DIV) @(posedge clk);
      busy = 1'b0;
    end
  end
endmodule
```

Elaborates to a 5-state FSM (`S_INIT`, `S_WAIT_SEND`, `S_START`, `S_DATA`,
`S_STOP`) with one shared baud-counter and one bit-index register. The
`forever` reboucles `S_STOP` → `S_WAIT_SEND`.

### Example 2 — RS232 RX (perpetual FSM with input sampling and `continue`)

```sv
module rs232_rx #(
  parameter int BAUD_DIV = 100,
  parameter int HALF_BIT = BAUD_DIV / 2
) (
  input  logic clk, rst_n,
  input  logic rx,
  output logic [7:0] data,
  output logic valid, framing_error
);
  initial begin
    valid = 1'b0; framing_error = 1'b0;
    @(posedge clk);
    forever begin
      while (rx) @(posedge clk);
      repeat (HALF_BIT) @(posedge clk);
      if (rx) continue;
      for (int i = 0; i < 8; i = i + 1) begin
        repeat (BAUD_DIV) @(posedge clk);
        data[i] = rx;
      end
      repeat (BAUD_DIV) @(posedge clk);
      framing_error = (rx == 1'b0);
      valid = 1'b1;
      @(posedge clk);
      valid = 1'b0;
    end
  end
endmodule
```

Elaborates to a 5-state FSM (`S_WAIT_IDLE`, `S_HALF_BIT`, `S_DATA_WAIT`,
`S_STOP_WAIT`, `S_STROBE`). The `continue` becomes a transition back to
`S_WAIT_IDLE`. The output strobe `valid = 1; @clk; valid = 0;` becomes the
two-state `S_STROBE` pair.

## Catalogue of source constructs and their FSM mapping

Derived from the examples above. Every construct has a deterministic,
mechanical translation.

| Source construct | FSM mapping |
|---|---|
| `@(posedge clk);` | Cycle boundary → next state |
| `while (cond) @(posedge clk);` | State self-loops on `!cond`, exits on `cond` true |
| `repeat (N) @(posedge clk);` | State with countdown counter, exit on `cnt == 0` |
| `for (i = 0; i < N; i = i + 1) { body with @ }` | Group of states with iteration counter `i`; loop edge until `i >= N` |
| `if (cond) ... else ...` | Branch in next-state logic of the state containing the `if` |
| `forever begin ... end` | Last state of body reboucles to first state |
| `continue` in a loop | Transition to first state of innermost enclosing loop |
| `break` in a loop | Transition to first state *after* innermost enclosing loop |
| `initial begin ... end` *(no forever)* | Last state self-loops (terminal); optionally emit a `done` strobe |
| Read from module input (`x = signal;` after a wait) | NBA reading the current value of `signal` in the post-wait state |
| Write to module output (`signal = expr;` between waits) | NBA `signal <= expr` in the corresponding state |
| `signal = 1; @clk; signal = 0;` | Two-state pulse pattern (one-cycle strobe) |

## Conventions that emerged

These are the unambiguous defaults the pass commits to. None of them
require user pragmas in v1.

| Question | Convention |
|---|---|
| Blocking `=` vs non-blocking `<=` in the source | Source uses `=` (natural for `initial`); pass translates to `<=` for all module-level signals and cross-state registers. Pure intra-state temporaries can remain blocking inside their generated state. |
| Reset behaviour | Reset is taken from the `@(posedge clk or negedge rst_n)` slot of the wrapper module. The pass synthesises `if (!rst_n) state <= FIRST_STATE; outputs <= init_values` and embeds the init phase of the source as the post-reset state. |
| Variable lifetime across states | Live-variable analysis: any variable written in state N and read in state M ≠ N becomes a register. Loop counters and `repeat` countdown counters always become registers. |
| Counter sharing | A single `baud_cnt` (or equivalent) is shared across all `repeat` and timed loops at v1, re-initialised on every entry. A future pass may split for routing wins. |
| `if (cond) ... else ...` with different branch lengths | No padding. Each branch takes the cycles it needs; the next-state logic of the last state of each branch transitions to the same merge state. |
| `for` with constant bound, body has no waits | Unroll. |
| `for` with constant bound, body has waits | Generate a loop FSM with an iteration counter register. |
| `data[i] = rx;` — when is `rx` sampled? | At the clock edge where the enclosing state transitions out, i.e. the standard NBA semantics. |
| `framing_error = (rx == 0);` after a wait | Computed combinationally from the just-sampled input value, written as NBA. |

## Implementation: where it fits in the veriparse pipeline

A new pass — `ImplicitFsmElaboration` — placed in the transformation
chain:

1. `AnnotateDeclaration`, `AnnotateScope`
2. `ParameterInliner`, `LocalparamInliner`
3. `GenerateRemoval`, `LoopUnrolling`
4. **`ImplicitFsmElaboration`** ← new
5. `ConstantFolding`, `VariableFolding`
6. `ModuleFlattener`, `ScopeElevator`
7. `DeadcodeElimination`

Placement after generate/loop unrolling lets the FSM pass see fully-resolved
bodies for the unroll-able loops; placement before flatten/dead-code keeps
the output clean.

Output is plain `always_ff` with an explicit state enum and a `case(state)`,
plus the inferred counter/iteration-index registers. Every downstream tool
accepts the result.

## Effort estimate

For a v1 covering everything in the catalogue above (which is sufficient for
the RS232 TX and RX examples, and by extension for most non-pipelined FSMs):

**~1.5–2 weeks** of focused work. Considerably less than full HLS because we
explicitly punt scheduling and datapath generation, and because the
construct choice (`always` vs `initial`) makes the perpetual-vs-one-shot
decision trivial.

## Open questions

- **State encoding policy:** binary, one-hot, gray? Default to binary, allow
  override via attribute on the source block?
- **Output decoding:** Moore-style (output is a function of `state` only,
  i.e. registered) or Mealy (output is combinational on `state` + inputs)?
  Default Moore (matches the NBA `signal <= expr` semantics of the source),
  Mealy reachable via a separate `assign` outside the FSM block.
- **Counter sharing:** when does separating the shared counter pay off in
  terms of routing? Probably a post-v1 optimisation pass.
- **Naming of generated registers:** `state`, `baud_cnt`, `bit_idx` look
  reasonable but collide with user code. Use a generated prefix (`__fsm_state`,
  etc.)? Trade-off between readable output and collision safety.

## Rejected alternatives

For reference, two earlier candidates that were considered and dropped:

**Option A — `always` with explicit `@(posedge clk)` everywhere** (no
implicit cycle boundaries). Functionally identical to the chosen
construct-driven scheme, just more verbose. Dropped because the
`always`-vs-`initial` distinction already communicates perpetual-vs-one-shot
cleanly without requiring extra ceremony.

**Option B — custom block keyword `veriparse_fsm`** with sparse syntax
(implicit per-statement clocking). Dropped because it would break standard
SV simulability — the source would have to be passed through veriparse
even to run a behavioural simulation. The current scheme keeps veriparse
on the synthesis path only.

## Relation to full HLS

This pass is *not* a stepping stone to full HLS. Scheduling, allocation, and
pipelining are deliberately excluded — they're an order of magnitude more
work and drag in concerns (memory models, interface protocols, pragma
languages) that the FSM-only scope avoids. If full HLS becomes desired
later, this pass is a useful foundation but not a prefix of it.
