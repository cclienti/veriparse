# Veriparse Passes Reference

## Architecture

All passes follow the same pattern: they inherit `TransformationBase` and implement a `process(node, parent)` method that is called recursively via `recurse_in_childs()`. Entry point is `run(node)`.

Analysis passes are static utility classes (no base class) that extract information from an AST without modifying it.

---

## Analysis Passes

### `Dimensions`
Analyzes signal and array dimensions within a module.

- Extracts packed/unpacked dimension info (`msb`, `lsb`, `width`, `is_big`, `is_packed`) for each signal.
- Produces a `DimMap` (signal name → `DimList`) via `analyze_decls()`.
- Can also analyze expression dimensions (`analyze_expr()`) and generate a declaration AST node from a `DimList` (`generate_decl()`).

**Diagnostic contract.** A width that does not fold is an *answer*, not an
anomaly: a parametric module has no constant width until its parameters are
inlined, so the extraction predicates report nothing and `analyze_decls()`
simply leaves the declaration out of the map (traced at debug level). Saying
what could not be sized, and why it mattered, belongs to the caller that needed
the width — `StructLowering` names the struct member, `TypedefInliner` the
alias, `TypeParamInliner` the type parameter, `ModuleInstanceNormalizer` the
port. A caller must therefore distinguish *absent because unmeasurable* from
*absent because undeclared* before blaming either.

**An empty `DimList` is a 1-bit value.** `extract_arrays()` drops width-1
dimensions, so an expression `analyze_expr()` has no rule for contributes the
same empty list a genuine 1-bit value does — and that reading is right far more
often than it is wrong: an operator result usually *is* one bit, and
`Concat`/`Repeat` compose the total from it correctly. Failing instead would
reject `{8{a & b}}`, whose width is unambiguous.

The consequence is that a caller cannot trust a width to be *measured*, only to
be *plausible*. So a caller that must act on it checks the width it ends up
with against what the construct allows, rather than against nothing:
`ModuleInstanceNormalizer` applies IEEE 1364-2005 §7.1.6 to instance arrays —
equal to the port connects to each instance, port×N part-selects per instance,
and "too many or too few bits to connect to all the instances shall be
considered an error". Skipping that last case is how a 24-bit value came to be
truncated into an 8-bit wire that every element then read, with the flatten
reporting success.

---

### `Module`
Queries the module-level AST for declarations and structure.

Key static getters:
- **Modules:** `get_module_nodes`, `get_module_dictionary`
- **Ports/IO:** `get_port_nodes`, `get_iodir_nodes`, `get_input_nodes`, `get_output_nodes`, `get_inout_nodes`
- **Parameters:** `get_parameter_nodes`, `get_localparam_nodes`
- **Variables:** `get_variable_nodes`, `get_variable_nodes_within_module`
- **Functions/Tasks:** `get_function_nodes`, `get_function_dictionary`, `get_task_nodes`
- **Calls:** `get_functioncall_nodes`, `get_taskcall_nodes`, `get_systemcall_nodes`
- **Instances:** `get_instance_nodes`
- **Values:** `get_lvalue_nodes`, `get_rvalue_nodes`, `get_rvalue_identifier_nodes`

---

### `Function`
Queries inside a `Function` node (stops at `Module` and `Task` boundaries).

- `get_iodir_nodes` / `get_iodir_names`
- `get_variable_nodes` / `get_variable_names`
- `is_like_automatic()` — returns true if the function can be treated as automatic

---

### `Task`
Queries inside a `Task` node (stops at `Module` and `Function` boundaries).

- `get_iodir_nodes` / `get_iodir_names`
- `get_variable_nodes` / `get_variable_names`

---

### `FunctionCall`
Extracts identifier nodes/names from a `FunctionCall` argument list.

---

### `Instance`
Extracts identifier and port argument nodes/names from an `Instance`.

---

### `SystemCall`
Extracts identifier argument nodes/names from a `SystemCall`.

---

### `TaskCall`
Extracts identifier argument nodes/names from a `TaskCall`.

---

### `Lvalue`
Analyzes an `Lvalue` node to separate true lvalue identifiers from rvalue identifiers embedded in lvalue expressions (e.g. index `i` in `p[i]`).

- `get_identifier_nodes` / `get_identifier_names` — all identifiers (lvalue + embedded rvalue)
- `get_lvalue_nodes` / `get_lvalue_names` — true lvalue identifiers only
- `get_rvalue_nodes` / `get_rvalue_names` — embedded rvalue identifiers only

---

### `UniqueDeclaration`
Collects all declared identifiers (I/O, variables, instances, tasks, functions) into a set, and provides helpers to generate unique identifiers (with optional random suffix).

---

### `SynthesizableCheck`
Rejects constructs outside the synthesizable RTL subset before flattening
(ADR-0007). Blacklist model; first entry: virtual interfaces (IEEE 1800-2017
§25.9). Opt-in per tool: `veriflat` runs it, `veridump`/`veriobf` stay
permissive.

---

### `Search<blacklist...>` (template base)
Internal template base for all analysis passes. Implements recursive AST walking with a configurable blacklist of node types to stop at. `StandardSearch` stops at `Function` and `Task`.

---

## Transformation Passes

### `TransformationBase`
Abstract base class. Provides:
- `run(node)` — entry point, calls `process(node, nullptr)`
- `recurse_in_childs(node)` — calls `process()` on each child
- `pickup_statements()` — replaces a node in its parent with a list of statements, merging blocks as needed

---

### `ConstantFolding`
Evaluates constant expressions in-place using `ExpressionEvaluation`.

- For every non-`Constant` node, attempts to evaluate the expression.
- If successful, replaces the node with the folded constant.
- Accepts an optional `FunctionMap` to evaluate function calls.
- Folds explicit enum item values (e.g. `A = WIDTH-1`) so that `EnumElaboration` can read them as `IntConstN`.

---

### `EnumElaboration`
Resolves enum item ordinal values in-place. **Must run after `ConstantFolding`.**

- Walks every `EnumDef` node in declaration order.
- If an item has an explicit `IntConstN` value, that value becomes the running counter base.
- If an item has no value, injects an unsized `IntConstN` with the current counter value.
- Increments the counter after each item.
- Errors if an explicit value is not yet an `IntConstN` (i.e. `ConstantFolding` was not run first).

---

### `EnumInliner`
Replaces enum item name identifiers with their resolved `IntConstN` constants throughout the AST. **Must run after `EnumElaboration`.**

- Collects all `EnumDef` item name → `IntConstN` mappings in a single pass.
- Replaces every simple `Identifier` (no scope qualifier) whose name matches an enum item with a clone of the constant.
- Skips `EnumDef` subtrees — the typedef declarations they preserve are consumed and dropped by `TypedefInliner`.

---

### `TypeParamInliner`
Reduces every type parameter (`parameter type T`, ADR-0010) to a `Typedef` — the instantiation actual (matched by name in the same ParamArg list `ParameterInliner` consumes) or the declared default — so `TypedefInliner` performs all substitution. **Must run before `ParameterInliner`.**

- Header formals splice after the leading typedef run at the body head; body `localparam type` reduces in place.
- Errors: no default and no override (§6.20.3), a non-type actual for a type formal, an override of a `localparam type`.
- A bare-identifier type actual (`.T(word_t)`) is resolved to a concrete type in the *parent's* scope by `TypedefInliner` (one-namespace rule, §3.13); a type actual for a value formal errors in `ParameterInliner`.

---

### `TypedefInliner`
Substitutes every user-defined type name with its underlying data type and drops the typedef declarations (ADR-0009). **Must run after `EnumInliner`.**

- Lexical typedef bindings: nested scopes (generate regions, begin/end blocks) shadow; body references must follow their typedef (IEEE 1800-2017 §6.18); header ports/params resolve against the whole module scope (package/unit typedefs are spliced at the body head).
- Substitutes a clone of the aliased type at every `NamedType` use (decl types, cast targets, `type()` operands); chains collapse eagerly; an array typedef's unpacked dims append after the declaration's own.
- After the pass no `Typedef` item and no `NamedType` reference remains, so `Dimensions` and the flattener only see concrete types.

---

### `StructLowering`
Lowers packed struct/union declaration types to their equivalent packed vectors and rewrites member accesses to constant part-selects (ADR-0011). **Must run after `TypedefInliner`.**

- First member at the MSBs (IEEE 1800-2017 §7.2.1); a packed union overlays equal-width members on the full range (§7.3.1); nested aggregates fold to absolute offsets; a directly enclosing bit/part-select folds into the member offset; signed members re-wrap in `signed'()` in expression position.
- Lexical scopes (module/interface, blocks, generate regions, subroutine bodies); identifiers whose root binds to no lowered declaration stay untouched (interface accesses, hierarchical references).
- Errors: unpacked aggregates, tagged unions, unknown members, non-integral or non-constant-width members, union width mismatches.

---

### `BranchSelection`
Evaluates `if`/`case` conditions that are constant after folding and replaces the branch node with the selected branch's statements.

- Depends on `ExpressionEvaluation`.
- Accepts an optional `FunctionMap`.

---

### `GenerateRemoval`
Removes `generate`/`endgenerate` wrapper blocks when their content can be safely inlined (i.e. the generate block contains only items, no parameterized loops or conditionals that remain unresolved).

---

### `LoopUnrolling`
Statically unrolls `for` and `repeat` loops whose bounds/counts are constant.

- Evaluates loop bounds via `ExpressionEvaluation`.
- Renames loop-body variables with a unique suffix per iteration to avoid collisions.
- Tracks scope renaming via `ScopeMap` and fixes scoped identifiers after unrolling.
- Accepts an optional `FunctionMap`.

---

### `ParameterInliner`
Inlines `parameter` values into the module body.

- Accepts an optional `ParamArg::ListPtr` (instantiation overrides); falls back to default parameter values.
- Resolves inter-parameter dependencies (O(n²) substitution).
- Removes parameter declarations after inlining.

---

### `LocalparamInliner`
Same as `ParameterInliner` but for `localparam` declarations.

- Resolves inter-localparam dependencies.
- Removes localparam declarations after inlining.

---

### `VariableFolding`
Propagates constant values of local variables through a procedural block.

- Maintains a `StateMap` (variable name → current constant value).
- Replaces rvalues with their folded constant when the state is known.
- Handles `if`, `for`, `while`, `repeat` statements.
- Accepts an optional `FunctionMap`.

---

### `ExpressionEvaluation`
Core expression evaluator (not a pass — used by other passes).

- Evaluates an expression tree to a constant (`IntConstN` or `FloatConst`) recursively.
- Supports a `ReplaceMap` to substitute identifiers with constants.
- Supports a `FunctionMap` to evaluate function calls.
- Handles system calls (`$clog2`, `$unsigned`, `$signed`).

---

### `ASTReplace`
Utility (not a pass) that walks an AST and replaces all `Identifier` nodes matching a given name with a clone of a replacement node.

Variants:
- Single name replace
- Bulk replace via `ReplaceMap`
- Works on `Node::Ptr` or `Node::ListPtr`

---

### `AnnotateDeclaration`
Renames all declared identifiers in a module using a regex search/replace pattern.

- Optionally ignores I/O port declarations.
- Applies the rename consistently across all uses in the module.

---

### `AnnotateScope`
Renames all scope labels (named block names) in a module using a regex search/replace pattern.

---

### `ScopeElevator`
Removes unnecessary named or unnamed `begin`/`end` blocks by lifting their contents to the enclosing scope.

- Handles scoped identifiers — renaming variables that were declared in the removed block to avoid collisions.
- Maintains a scope stack to correctly resolve multi-level nesting.

---

### `ModuleIONormalizer`
Normalizes module port declarations into a canonical form.

- Merges `Ioport` pairs (port + variable declaration) by removing redundant inline declarations.
- Handles `parameter` ports.
- Creates default net-type variables for ports that lack an explicit variable declaration.

---

### `ModuleInstanceNormalizer`
Normalizes module instantiations.

- Splits instance lists (multiple instances in one `Instancelist`) into individual instances.
- Splits array instances into scalar instances.
- Converts positional port connections to named connections.
- Converts positional parameter overrides to named overrides.
- Handles `defparam` statements.
- Replaces complex port expressions with intermediate wire declarations + assignments.

---

### `PackageInliner`
Resolves SystemVerilog `package`/`import` (ADR-0004): collect-then-resolve per
compilation unit in command-line order (§26.3), lazy wildcard imports, eager
explicit imports, re-export folding with origin dedup (§26.6), transitive
same-package dependency copy. Output is package/import-free. Entry:
`run_units(sources)`.

---

### `NameResolution`
Symbol-aware resolution of the parser deferrals (ADR-0006): design index +
lexical scope stack; re-tags `Call → Function/TaskCall`,
`Instance → InterfaceInstance`, bare interface ports → `InterfaceType`,
`TypeCast → SizeCast`, `TypeOpExpr → TypeOpType`; validates modport placement
(§25.5) and interface-as-data-type (§25.9). Entry: `run_design(sources)`.

---

### `DefaultResolution`
Implicit-default resolution (ADR-0012): replaces every deferred implicit
default with what the standard says it means — `ImplicitType` → `logic` with
signing/packed dims carried verbatim (§6.8; SV mode only, so 1364-2005 designs
render as 1364-2005; value parameters follow §6.20.2: only a ranged implicit
type resolves), `ImplicitNet` → a net of the prevailing `` `default_nettype ``
(§22.8, `none` is a hard error), and the §23.2.2.3 ANSI port defaults
(first-port `inout`, direction inheritance, output-with-explicit-data-type and
`ref` ports become variables). Interface ports and backed non-ANSI direction
declarations stay untouched. Context-local, no symbol table. Entry:
`run_design(sources)`.

---

### `InterfaceElaboration`
Lowers SystemVerilog interfaces to plain signals during flattening (ADR-0008).
Each interface definition transplants into a pseudo-module (modports validated
and stripped) that joins the flattener's modules map, so instances elaborate
through the regular per-instance clone → parametrize → resolve → inline path —
body logic once per instance, per-instance parameterization (§25.8). A child's
interface port dissolves by hier-label aliasing: `port.member` references
retarget onto the connected instance's flattened signals (never copies,
§25.3.2) with modport-visibility checks (§25.10); interface arrays connect
element-wise (§23.3.3.5). Invoked from `ModuleFlattener`; not a standalone
pass.

---

### `ImplicitFsmElaboration`
Compiles a `(* veriparse_fsm *)`-marked multi-cycle `initial` process into an
explicit synthesizable FSM (ADR-0014). The user-facing reference is
[verilower.md](verilower.md).

- Cuts the process at every `@(posedge clk)` and walks the path cover:
  each path between two consecutive cut points becomes a guarded transition,
  with infeasible paths pruned structurally.
- Init segment → reset branch (signal/level/kind inferred or hinted);
  uniform `iff` conditions → a single chip enable.
- Bounded loops arrive unrolled (`LoopUnrolling`); `(* veriparse_no_unroll *)`
  loops get an induced countdown or drive the author's index register, one
  counter per nesting depth. `break`/`continue` are CFG edges;
  `forever`/`while` form real back-edges.
- Blocking temporaries (`=` on in-process declarations) materialize as typed
  wires substituted at their uses.
- State names derive from block labels; encodings: binary, one-hot, gray.
- Everything outside the supported subset is a hard error citing the
  governing rule (ADR-0014 §9): impure calls, multi-driver registers,
  zero-delay loops, multiple clocks, `fork`/`join`, tasks holding cut
  points, …
- Fills an `FsmReport` (states, transitions, reset, per-process) that
  `verilower` serializes as JSON and graphviz.
- Opt-in: only runs when enabled in `ResolveModule` (the `verilower` driver
  does; `veriflat` leaves it off).

---

### `ModuleFlattener`
Flattens a module hierarchy by inlining all sub-module instances.

- Requires a `ModulesMap` of all available modules.
- For each instance: inlines parameter/localparam values, renames declarations to avoid collisions, replaces port connections with assignments.
- Handles `defparam`.
- Optionally runs dead-code elimination after flattening.
- Builds an instance tree (`TreeNode`) recording the full hierarchy; the
  hierarchical-reference matching is index-aware (`u[2].sig` → `u2_sig`).
- Calls `ResolveModule` internally for each sub-module before inlining.
- Elaborates interfaces through `InterfaceElaboration` (ADR-0008): interface
  instances flatten as pseudo-modules, interface ports dissolve by aliasing.

---

### `ResolveModule`
High-level pass that applies the full resolution pipeline to a single module.

Exact pipeline order:

```
ModuleIONormalizer
TypeParamInliner      ← SV: reduce type parameters to typedefs (ADR-0010)
ParameterInliner
LocalparamInliner
ConstantFolding
EnumElaboration       ← SV: fill auto-increment enum values
EnumInliner           ← SV: replace enum names with IntConstN
TypedefInliner        ← SV: substitute typedefs with concrete types (ADR-0009)
StructLowering        ← SV: packed structs/unions to vectors (ADR-0011)
ScopeElevator
LoopUnrolling
BranchSelection
GenerateRemoval
ImplicitFsmElaboration ← optional, opt-in (ADR-0014); on in verilower, off in veriflat
ConstantFolding       ← second pass after branch/generate removal
VariableFolding
DeadcodeElimination   ← optional
ModuleInstanceNormalizer
```

---

### `DeadcodeElimination`
Removes unused declarations and dead statements.

- Collects all identifier references in the module.
- Removes statements that drive signals never read (iterates to fixed point).
- Removes declarations of signals that are never referenced.
- Removes empty blocks, empty statements, and empty pragma attributes.

---

### `WireSplit`
Splits inline wire declarations with initializers:

```verilog
wire [N:0] x = expr;
```
into separate declaration and continuous assignment:
```verilog
wire [N:0] x;
assign x = expr;
```
Applied **after** `ModuleFlattener` in `veriflat` (not inside `ResolveModule`) to fix forward-reference issues in the final flat output.

---

### `ModuleObfuscator`
Renames all local identifiers (variables, instances, named blocks, tasks, functions) to random strings.

- Configurable identifier length.
- Optionally uses a hash-based naming scheme instead of random.
- Preserves port names (I/O) unchanged.

---

## `veriflat` Top-Level Pipeline

```
PackageInliner (run_units)         ← package/import resolution per unit
NameResolution (run_design)        ← re-tags the parser deferrals
DefaultResolution (run_design)     ← resolves the implicit defaults
SynthesizableCheck                 ← rejects non-synthesizable constructs
ModuleFlattener                    ← flattens hierarchy (incl. interfaces
  └─ per sub-module: ResolveModule    via InterfaceElaboration)
DeadcodeElimination                ← optional (--deadcode-end)
WireSplit                          ← post-flatten cleanup
VerilogGenerator                   ← emit output file
```
