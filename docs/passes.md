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
- Skips `EnumDef` subtrees to preserve typedef declarations intact.

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
Statically unrolls `for` loops whose bounds and step are constant.

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

### `ModuleFlattener`
Flattens a module hierarchy by inlining all sub-module instances.

- Requires a `ModulesMap` of all available modules.
- For each instance: inlines parameter/localparam values, renames declarations to avoid collisions, replaces port connections with assignments.
- Handles `defparam`.
- Optionally runs dead-code elimination after flattening.
- Builds an instance tree (`TreeNode`) recording the full hierarchy.
- Calls `ResolveModule` internally for each sub-module before inlining.

---

### `ResolveModule`
High-level pass that applies the full resolution pipeline to a single module.

Exact pipeline order:

```
ModuleIONormalizer
ParameterInliner
LocalparamInliner
ConstantFolding
EnumElaboration       ← SV: fill auto-increment enum values
EnumInliner           ← SV: replace enum names with IntConstN
ScopeElevator
LoopUnrolling
BranchSelection
GenerateRemoval
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
ModuleFlattener                    ← flattens hierarchy
  └─ per sub-module: ResolveModule ← full resolution pipeline
DeadcodeElimination                ← optional (--deadcode-end)
WireSplit                          ← post-flatten cleanup
VerilogGenerator                   ← emit output file
```
