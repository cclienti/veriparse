# `verilower` — Imperative FSM Elaboration

`verilower` compiles a **multi-cycle imperative process** — an `initial` block
that suspends on its own clock waits — into an explicit, synthesizable finite
state machine. The author writes sequential behaviour as a *program*: statements
run in order, `@(posedge clk)` marks each clock boundary, loops repeat work,
`break`/`continue` shape the control flow. The tool emits the `always_ff` /
`case` machine a designer would have written by hand — with the state register,
the encoding, the reset branch, and the state names derived from the source.

The behavioural input and the lowered output are validated against the **same
testbench**: the input form simulates as-is in any event-driven simulator that
supports timing controls (Verilator ≥ 5.050 with `--timing`, or any commercial
simulator), so equivalence can be established by differential cosimulation
rather than by trust.

The full design rationale — every decision, its alternatives, and the
equivalence argument — is **ADR-0014**
([adr-0014-implicit-fsm-elaboration.md](adr-0014-implicit-fsm-elaboration.md)).
This document is the user-facing reference: what to write, and what it
produces.

---

## Quick start

Input — a five-step accumulator, gated by a chip enable:

```systemverilog
module enable_line (input logic clk, input logic rst_n, input logic en,
                    output logic done, output logic [7:0] acc);

   (* veriparse_fsm *)
   initial begin
      done <= 1'b0;              // init segment = the reset values
      acc  <= 8'd0;
      @(posedge clk iff en);     // each wait ends a state
      acc <= 8'd1;
      @(posedge clk iff en);
      acc <= acc + 8'd2;
      @(posedge clk iff en);
      acc <= acc + 8'd5;
      @(posedge clk iff en);
      acc <= acc ^ 8'h0f;
      @(posedge clk iff en);
      done <= 1'b1;
   end
endmodule
```

Compile:

```sh
verilower --sv -t enable_line -o enable_line_fsm.sv enable_line.sv
```

Output — the machine, written the way a designer would:

```systemverilog
module enable_line (input wire logic clk,
                    input wire logic rst_n,
                    input wire logic en,
                    output logic done,
                    output logic [7:0] acc);

  localparam [2:0] __fsm_state_0 = 3'd0;
  localparam [2:0] __fsm_state_1 = 3'd1;
  localparam [2:0] __fsm_state_2 = 3'd2;
  localparam [2:0] __fsm_state_3 = 3'd3;
  localparam [2:0] __fsm_state_4 = 3'd4;
  localparam [2:0] __fsm_hold = 3'd5;
  logic [2:0] __fsm_state;

  always_ff @(posedge clk)
    if(!rst_n) begin
      done <= 1'b0;
      acc <= 8'd0;
      __fsm_state <= __fsm_state_0;
    end
    else
      if(en)
        case(__fsm_state)
          __fsm_state_0: begin
            acc <= 8'd1;
            __fsm_state <= __fsm_state_1;
          end
          __fsm_state_1: begin
            acc <= acc + 8'd2;
            __fsm_state <= __fsm_state_2;
          end
          __fsm_state_2: begin
            acc <= acc + 8'd5;
            __fsm_state <= __fsm_state_3;
          end
          __fsm_state_3: begin
            acc <= acc ^ 8'hf;
            __fsm_state <= __fsm_state_4;
          end
          __fsm_state_4: begin
            done <= 1'b1;
            __fsm_state <= __fsm_hold;
          end
          __fsm_hold: begin
          end
        endcase

endmodule
```

The reset signal (`rst_n`, active-low, synchronous), the chip enable (`en`),
the state count, and the terminal hold state were all **inferred from the
source** — nothing was configured. Every inference can be overridden by an
attribute ([Attribute reference](#attribute-reference)).

---

## Command-line reference

```
Usage: verilower [options] verilog-file [verilog-file ...]

options:
  -h [ --help ]             Produce help message
  -v [ --version ]          Show the version and exit
  -o [ --output ] arg       output
  -t [ --top-module ] arg   top-module
  --suffix arg              Append to the emitted module's name, so the output
                            can sit beside its source in one testbench
  --sv                      Enable SystemVerilog mode
  -I [ --include-dir ] arg  Add directory to `include search path (repeatable)
  -D [ --define ] arg       Predefine a macro as NAME or NAME=BODY (repeatable)
  -U [ --undef ] arg        Cancel a predefine NAME (repeatable)
  -s [ --seed ] arg (=0)    Seed value
  --log arg                 Log to FILE instead of stderr
  --state-map arg           Write the ADR-0014 state map (JSON) to FILE;
                            default <output>.fsm.json
  --fsm-dot arg             Write a graphviz view of the compiled machines to FILE
  --fsm-dot-values          Label the graphviz edges with the register updates too
```

`verilower` runs the same design preamble as `veriflat` (preprocessing,
parameter/localparam inlining, enum/typedef/struct lowering, generate
resolution) before the FSM pass, and checks the *output* for
synthesizability. `--suffix` renames the emitted module (e.g.
`enable_line` → `enable_line_fsm`) so the behavioural source and the lowered
result can be instantiated side by side in one differential testbench.

The `-t` module is compiled at its **default parameterization**. A marked
module instantiated elsewhere with parameter overrides is reported, not
silently compiled — resolving each instantiation with its own parameters is
the flattener's job (a planned `veriflat --fsm` mode, ADR-0014 §15).

---

## The input contract

A process is compiled if and only if it is marked:

```systemverilog
(* veriparse_fsm *)
initial begin
   ...
end
```

- **`initial` only.** A perpetual machine is written `initial forever begin … end`;
  the statements *above* the loop are the one-time prologue that becomes the
  reset branch. A marked `always` is rejected with the one-line rewrite in the
  message (`always begin B end` ≡ `initial forever begin B end`, but `always`
  has no prologue region to take reset values from — ADR-0014 §15.1).
- **One clock, one edge.** Every wait in the process names the same
  `@(posedge clk)` (or `negedge`); mixing clocks or edges is an error.
- **Unmarked processes are untouched.** The pass is strictly opt-in;
  a design with no `(* veriparse_fsm *)` mark passes through unchanged.
- **Several marked processes** in one module each become their own machine,
  with distinct generated names (`__fsm…`, `__fsm2…`, …). Their register
  write sets must be disjoint — writing one register from two processes
  (or from a process and an `assign`/instance output) is a multi-driver
  error, checked structurally.

A marked process either compiles **exactly** or fails **loudly** with the
source line and the governing rule — there is no silent approximation
(the error catalogue is ADR-0014 §9).

---

## Supported constructs

| Construct | Produces |
|---|---|
| `@(posedge clk)` | a state boundary: statements between two consecutive waits become one state's action |
| `@(posedge clk iff en)` | the same boundary, plus a machine-wide chip enable folded around the `case` — every wait must carry the same `iff` condition |
| statements before the first wait (the *init segment*) | the reset branch: those `<=` right-hand sides are the reset values |
| `<=` to a module-level signal | a register, assigned in the state where the statement sits |
| `=` to a local variable (declared in-process) | a combinational temporary: a `wire` per assigned expression, substituted at its uses ([Blocking temporaries](#blocking-temporaries)) |
| `if` / `else` around waits | a control-flow fork: one transition per path, guarded by the condition's **entry value** |
| `case` around waits | the same fork, one guarded transition per item (`==` against each label); items must be x/z-free |
| bounded `for` / `repeat` / `while` | **unrolled** — each iteration contributes its own states |
| `(* veriparse_no_unroll *) repeat (N) …` | one rolled state group with an induced countdown register (`__fsm_cnt`); `N` may be non-constant — it is captured at loop entry (IEEE 1800-2017 §12.7.2) |
| `(* veriparse_no_unroll *) for (i = …) …` | one rolled state group driving the author's own index register `i` (module-level, its declared type), honouring init/test/step |
| nested rolled loops | one countdown per nesting **depth** (`__fsm_cnt`, `__fsm_cnt2`, …), each re-initialised on entry; sequential loops at the same depth share their depth's register |
| `while (cond)` with a wait inside | a genuine back-edge: the loop's last state re-evaluates `cond` and re-enters or proceeds — `while (!start) @(posedge clk);` is the canonical wait-state |
| `forever` / `while (1)` | a perpetual machine: the last state loops to the first in-loop state, and no hold state is emitted |
| `break` / `continue` | CFG edges: exit or re-enter the innermost loop, from any nesting of `if`/`case` |
| `begin : LABEL … end` | state names: `__fsm_LABEL`, nested labels composed outward-in (`__fsm_BIT_LOW`), ordinals when one label spans several states ([Naming](#naming-the-states)) |
| calls to **pure** functions | inlined combinational logic; purity is checked (only inputs read, only locals written, no static locals, callees pure) |
| pure system functions (`$clog2`, `$bits`, `$size`, `$countones`, `$onehot`, …) | evaluated or passed through as combinational logic |
| parameters, `localparam`, enum/typedef/struct types | resolved by the preamble passes before lowering, as in `veriflat` |

Everything outside this table is a **hard error with a cited rule**, never a
silent mis-lowering — see [What is rejected](#what-is-rejected).

---

## Reset

The statements **before the first wait** form the init segment: they execute
before the machine can be suspended, so their assignments are the reset
values. `verilower` turns them into the reset branch of the `always_ff`:

- the **reset signal** is inferred (or named with `veriparse_reset`);
- the **active level** defaults to high, or low when the name ends in `_n`
  (`veriparse_reset_level` overrides);
- the **flavour** defaults to synchronous; `veriparse_reset_kind = "async"`
  adds the reset edge to the sensitivity list.

Every register written by the process must have a reset value — an
assignment in the init segment. The state register is reset alongside them,
to the entry state.

The reset cannot be *spelled* in the source (an `if (!rst_n)` inside the
process would be part of the program, not a reset), which is why it is
inferred from the init segment — ADR-0014 §5.2.

## Chip enable

A chip enable is written **in the source**, on the waits:

```systemverilog
@(posedge clk iff en);
```

All waits must agree on the condition; the machine is then wrapped in a
single `if (en)` inside the clocked branch — the idiom synthesis tools map
to a clock-enable flip-flop (checked in CI: yosys maps the quick-start
example to `$sdffe` cells). Different conditions on different waits
(per-state enables) are rejected in v1.

---

## Loops

**Default: bounded loops unroll.** Every iteration contributes its own
states, uniformly, with no heuristic on the body — a `repeat (4)` around one
wait yields four states.

**`(* veriparse_no_unroll *)` keeps a loop rolled.** The loop becomes one
state group re-entered under an induced counter:

```systemverilog
q <= d;
(* veriparse_no_unroll *)
repeat (4) @(posedge clk);
done <= 1'b1;
```

lowers to a countdown register sized for the count, initialised on entry and
tested on the back-edge:

```systemverilog
  logic [1:0] __fsm_cnt;
  ...
        __fsm_state_0: begin
          q <= d;
          __fsm_cnt <= 2'd3;
          __fsm_state <= __fsm_state_1;
        end
        __fsm_state_1: if(__fsm_cnt != 2'd0) begin
          __fsm_cnt <= __fsm_cnt - 2'd1;
          __fsm_state <= __fsm_state_1;
        end
        else begin
          done <= 1'b1;
          __fsm_state <= __fsm_state_2;
        end
```

- A rolled `repeat (N)` accepts a **non-constant** `N`: the count is captured
  at loop entry, per IEEE 1800-2017 §12.7.2.
- A rolled `for` drives the **author's own index register** (declared at
  module level) with its declared type, honouring the init, test, and step
  expressions — so the body can read the index, e.g. `data[i]`.
- **Nesting**: each nesting depth owns one countdown (`__fsm_cnt`,
  `__fsm_cnt2`, …), sized for the largest count at that depth; sequential
  rolled loops at the same depth share it. Rolled and unrolled loops mix
  freely with `break`/`continue`.
- A `while` (or a hand-written counting shape that needs a custom re-arm
  point or exit) is a **data-dependent loop**: its condition is re-evaluated
  on the back-edge, no counter is induced. This is the escape hatch when the
  rolled `repeat`/`for` contract does not fit — see ADR-0014 §7.3.

A loop with a cut point on **every** path through its body advances the
clock each lap. A lap that can complete with no wait (e.g. `break` skipping
the only wait, leaving a zero-delay path) is rejected as a zero-delay loop.

---

## Blocking temporaries

Inside a marked process, `=` and `<=` are **distinguished on purpose**
(ADR-0014 §6): `<=` writes a register; `=` names an intermediate value.

A blocking temporary is declared *in the process*, in a block containing no
wait, and lives until the block ends:

```systemverilog
begin
   logic [8:0] sum;
   sum   = a + b + carry_in;   // one wire: __fsm_t_sum = a + b + carry_in
   acc   <= sum[7:0];          // reads become the wire
   carry <= sum[8];
end
@(posedge clk);
```

Each assigned expression is **materialized as a `wire`** typed exactly by the
temporary's declaration (width, signedness), and every read is substituted
with it — so a 9-bit `sum` keeps its carry bit no matter what context it is
read in. Identical expressions assigned to same-typed temporaries share one
wire; constant right-hand sides are folded and truncated to the declared
width instead of materialized.

Rules the checker enforces:

- a temporary must be **assigned before read**, on every path;
- its scope must not contain a wait (its value cannot cross a clock edge —
  a value that must survive a cycle is a register: use `<=`);
- `=` to a **module-level** signal is an error in v1 (combinational output
  decode is a planned v2 rule — ADR-0014 §15);
- a temporary may not **shadow** a module-level name or an enclosing
  temporary — rename it (alpha-renaming is planned for v2).

---

## Naming the states

State names are `localparam`s built from **block labels** — the language's
own naming construct, preserved by the tool:

```systemverilog
begin : IDLE
   r <= d;
end
begin : COUNT
   q <= q + 8'd1;
   @(posedge clk);
   q <= q + 8'd2;
   @(posedge clk);
end
begin : BIT
   begin : LOW  ... end
   begin : HIGH ... end
end
```

produces

```systemverilog
  localparam [2:0] __fsm_IDLE = 3'd0;
  localparam [2:0] __fsm_COUNT_0 = 3'd1;
  localparam [2:0] __fsm_COUNT_1 = 3'd2;
  localparam [2:0] __fsm_BIT_LOW = 3'd3;
  localparam [2:0] __fsm_BIT_HIGH = 3'd4;
```

- a label spanning several states gets ordinals (`_0`, `_1`, …);
- nested labels compose outward-in (`BIT` + `LOW` → `__fsm_BIT_LOW`);
- unlabeled states fall back to positional names (`__fsm_state_5`);
- the terminal hold state (emitted only when the process can end) is
  `__fsm_hold`.

The prefix is `__fsm` (then `__fsm2`, `__fsm3`, … for further marked
processes in the same module), or `veriparse_prefix` per process. A label
or hint colliding with an existing module-level name is an error, not a
silent rename.

## Encodings

`veriparse_encoding` selects the state encoding: `"binary"` (default),
`"one_hot"`, or `"gray"`. The encoding changes the `localparam` values and
the state register width; the machine's shape is identical.

---

## Attribute reference

Attributes are **hints, never load-bearing** (ADR-0014 §3): each one either
names what would be inferred, or selects among correct alternatives. A design
with no attribute except the `veriparse_fsm` mark compiles with defaults.

| Attribute | On | Effect | Default when absent |
|---|---|---|---|
| `veriparse_fsm` | the `initial` process | compiles this process (the opt-in) | process left untouched |
| `veriparse_reset = "<port>"` | the process | names the reset input | inferred (see [Reset](#reset)) |
| `veriparse_reset_level = 0\|1` | the process | reset active level | `1`, or `0` when the name ends in `_n` |
| `veriparse_reset_kind = "sync"\|"async"` | the process | reset flavour of the generated `always_ff` | `"sync"` |
| `veriparse_encoding = "binary"\|"one_hot"\|"gray"` | the process | state encoding | `"binary"` |
| `veriparse_prefix = "<id>"` | the process | prefix for generated declarations; must be distinct across a module's marked processes | `__fsm`, `__fsm<N>` |
| `veriparse_no_unroll` | a `repeat`/`for` loop | keeps the loop rolled (induced countdown / index) | bounded loops unroll |

A hint that cannot take effect (naming a port that does not exist, a
`veriparse_no_unroll` on a loop with no cut point, …) is an **error**, not a
warning — an inert attribute in the source would misdocument the design.

---

## Machine reports: state map, graphviz, waveforms

Alongside the lowered RTL, `verilower` documents what it built:

- **State map (JSON)** — always written to `<output>.fsm.json` (or
  `--state-map FILE`): per process, the state variable, width, encoding,
  entry state, the inferred reset (signal, level, kind, reset registers),
  every state with its value and source line, and every transition with its
  guard and action. This is the machine in machine-readable form — for
  scripting, debug overlays, or documentation.

```json
{
  "processes": [
    {
      "module": "enable_line",
      "state_variable": "__fsm_state",
      "width": 3,
      "encoding": "binary",
      "entry": "__fsm_state_0",
      "reset": {"signal": "rst_n", "active_level": 0, "kind": "sync",
                "registers": ["done", "acc", "__fsm_state"]},
      "states": [
        {"name": "__fsm_state_0", "value": 0, "line": 8},
        ...
      ],
      "transitions": [
        {"from": "__fsm_state_0", "to": "__fsm_state_1",
         "guard": "", "action": "acc <= 8'd1"},
        ...
      ]
    }
  ]
}
```

- **Graphviz** — `--fsm-dot FILE` renders the machines as a state diagram
  (one cluster per process, entry state double-circled; `--fsm-dot-values`
  adds the register updates to the edges):

  ```sh
  verilower --sv -t enable_line -o out.sv --fsm-dot out.dot enable_line.sv
  dot -Tsvg out.dot -o out.svg
  ```

- **Waveform display** — `scripts/fsm_wavedisp.py` converts the state map
  into a [wavedisp](https://github.com/cclienti/wavedisp) description, so a
  waveform viewer shows symbolic state names instead of encoded values.

---

## What is rejected

The pass draws a hard line: inside the marked process, everything it accepts
lowers exactly, and everything else names its rule. The categories
(the full table, with one test per row, is ADR-0014 §9):

- **scheduling constructs with no hardware meaning**: delays (`#`), waits on
  data (`wait`), event triggers, multiple clocks or mixed edges, `fork`/`join`
  (write two marked processes rendezvousing through registers instead — the
  v2 plan mechanizes exactly that decomposition);
- **`always` marked** (rewrite as `initial forever` — the message says so);
- **impure calls**: tasks containing cut points, functions with side effects,
  non-pure system calls (`$random`, `$display`, `$fopen`, …);
- **§6 violations**: `=` to a module-level signal, a register committed twice
  on one path, a temporary read before assignment or crossing a wait,
  shadowing;
- **structural hazards**: zero-delay loops, a register also driven by
  another process / `assign` / instance output (multi-driver, checked
  across generate blocks and task calls), case items with x/z in a forking
  `case`;
- **inert hints** (see [Attribute reference](#attribute-reference)).

Infeasible paths are pruned before checking: a path guarded by `send` and
`!send` at once is not walked, so it can neither create a state nor trigger
a false diagnostic.

The v1 deferral list — what is rejected today with a planned future rule
(combinational output decode, task inlining, `fork`/`join` decomposition,
`casez` guards, temporary shadowing, Mealy outputs, …) — is ADR-0014 §15.

---

## Validation

Every feature is backed by a differential cosimulation: the behavioural
source and the lowered machine run under **the same testbench**, and their
outputs are compared cycle by cycle (the reference leads by the reset
latency; the harness aligns them). The shipped suite covers straight lines,
enables, branches, rolled and nested counters, mixed loop nests, jumps,
temporaries, and complete designs (RS232 TX/RX, an RMII MAC receiver
benched three ways: behavioural vs lowered vs the original hand-written
FSM).

Simulating the *behavioural input* requires timing-control support:
**Verilator ≥ 5.050 with `--timing`** — earlier Verilators execute
initial-block `<=` immediately (no NBA delay) and corrupt the reference.
The *lowered output* is plain synchronous RTL and runs anywhere, including
2-state and synthesis flows.
