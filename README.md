# Veriparse

Veriparse is a **source-to-source transformation** toolkit for synthesizable Verilog and SystemVerilog designs. Its transformations always produce valid, human-readable RTL — never a gate-level netlist — so the output drops into any simulation or synthesis flow. Three capabilities headline it:

- **Imperative FSM elaboration** (`verilower`): write a multi-cycle sequential behaviour as a program — an `initial` process with its own `@(posedge clk)` waits, loops, and `break`/`continue` — and compile it into the explicit, synthesizable `always_ff`/`case` state machine a designer would have written by hand. The behavioural source and the lowered machine are validated against the *same* testbench.
- **Flattening & elaboration** (`veriflat`): flatten a hierarchical design into a single module, with partial parameter inlining, constant folding, loop unrolling, and generate resolution — also useful to retarget generic, parametric designs (complex ROM/LUT initialization written as functions or generate loops) into plain RTL that FPGA toolchains with limited synthesis support accept.
- **Obfuscation** (`veriobf`): rename all internal identifiers of a (flattened) design so IP vendors can deliver functional, simulatable netlists that are hard to reverse-engineer.

The toolkit is built around a C++ library (`veriparse_static`) that implements:
- A complete **Verilog / SystemVerilog parser** (Flex/Bison based), supporting a significant subset of the synthesizable languages (generate blocks, `always_ff`/`always_comb`, `logic` types, packages, interfaces, packed structs, enums, typedefs, …)
- An **AST** (Abstract Syntax Tree) framework
- A set of **transformation passes** (constant folding, dead code elimination, loop unrolling, module flattening, FSM elaboration, obfuscation, etc.)
- **Verilog and YAML generators**

## Tools

Each tool is summarized here; its full documentation — command-line
reference, supported constructs, examples — lives in `docs/`.

### `verilower` — Imperative FSM Elaboration

`verilower` compiles a `(* veriparse_fsm *)`-marked multi-cycle `initial`
process into an explicit synthesizable state machine. Statements run in
order, `@(posedge clk)` marks each clock boundary; the tool cuts the process
at the waits and emits the `always_ff`/`case` machine — state register,
encoding, reset branch (inferred from the assignments before the first
wait), optional chip enable (`@(posedge clk iff en)`), and state names
derived from the source's block labels:

```systemverilog
(* veriparse_fsm *)
initial begin
   done <= 1'b0;              // init segment = the reset values
   acc  <= 8'd0;
   @(posedge clk iff en);     // each wait ends a state
   acc <= 8'd1;
   @(posedge clk iff en);
   acc <= acc + 8'd2;
   @(posedge clk iff en);
   done <= 1'b1;
end
```

Loops unroll by default; `(* veriparse_no_unroll *)` keeps a loop rolled
with an induced counter. Alongside the RTL, `verilower` writes a JSON state
map and a graphviz view of the compiled machines. The behavioural input
simulates as-is (Verilator ≥ 5.050 `--timing`), so input and output are
validated against the **same testbench** by differential cosimulation.
Anything the model cannot lower exactly is a hard error citing the rule.

**Full documentation: [docs/verilower.md](docs/verilower.md)** — design
rationale: [docs/adr-0014-implicit-fsm-elaboration.md](docs/adr-0014-implicit-fsm-elaboration.md).

### `veriflat` — Verilog Flattener

`veriflat` flattens a hierarchical design into a single self-contained
module, resolving parameters, enums, typedefs, packed structs, constant
functions, and generate blocks along the way. Partial parameter inlining
lets you choose which top-module parameters stay parameters and which fold
as constants:

```sh
# Flatten, keeping FIFO_WIDTH as a parameter (preprocessing is built in)
veriflat -p '{FIFO_WIDTH:}' --seed 0 --top-module top -I src/ src/top.v src/sub.v --output top_flat.v
```

With `--fsm`, the flattener also runs verilower's imperative-FSM
elaboration on each instance at its own parameters — the road for marked
modules instantiated with different parameter overrides.

**Full documentation: [docs/veriflat.md](docs/veriflat.md)** — including the
parameter-map format and a worked generate-loop flattening example.

### `veriobf` — Verilog Obfuscator

`veriobf` renames all internal identifiers of a (typically flattened) design
to random or hashed names, preserving the external interface — the
IP-delivery back end of the `veriflat → veriobf` pipeline:

```sh
veriobf --id-length 16 --seed 0 top_flat.v --output top_obf.v
```

**Full documentation: [docs/veriobf.md](docs/veriobf.md)**

### `veridump` — AST Dumper

`veridump` parses a design and dumps its AST as YAML (round-trippable
through the library's importer) or Graphviz DOT — for debugging,
visualization, and scripting:

```sh
veridump --sv -f dot -o design.dot design.sv && dot -Tsvg design.dot -o design.svg
```

**Full documentation: [docs/veridump.md](docs/veridump.md)**

### `veripp` — Verilog Preprocessor

`veripp` runs the built-in preprocessor alone (`` `include ``,
`` `define ``, conditionals) and writes the resulting compilation unit —
useful to inspect exactly what the parser will consume:

```sh
veripp -I rtl/include -D BOARD=de10 rtl/top.sv -o top_pp.sv
```

**Full documentation: [docs/veripp.md](docs/veripp.md)**

---

## Transformation Passes

The library implements its transformations as composable passes over the
AST: resolution and inlining (parameters, localparams, enums, typedefs,
structs, type parameters, packages, interfaces, defaults), structure
(loop unrolling, branch selection, generate removal, module flattening,
instance/IO normalization, scope elevation), evaluation (constant folding,
function evaluation, variable folding), FSM elaboration (ADR-0014), dead
code elimination, and obfuscation.

**Full documentation: [docs/passes.md](docs/passes.md)** — every analysis
and transformation pass, with its contracts and the exact `ResolveModule`
pipeline order. Design decisions behind the passes are recorded as ADRs in
[docs/](docs/).

---

## Project Structure

```
veriparse/
├── apps/
│   └── veriparse/
│       ├── veriflat/       # veriflat tool
│       ├── verilower/      # verilower tool
│       ├── veriobf/        # veriobf tool
│       ├── veridump/       # veridump tool
│       ├── veripp/         # veripp tool
│       └── test/           # Integration & cosimulation tests
├── cmake/                  # CMake modules and common settings
├── conda/                  # Conda build and dev environment
├── docs/                   # Tool documentation, passes reference, ADRs
├── lib/
│   ├── include/            # Public headers
│   ├── src/                # Library source code
│   │   ├── AST/            # AST node definitions
│   │   ├── generators/     # Verilog/YAML generators
│   │   ├── importers/      # YAML importer
│   │   ├── logger/         # Logging utilities
│   │   ├── misc/           # Miscellaneous utilities
│   │   ├── parser/         # Verilog parser (Flex/Bison)
│   │   └── passes/         # Transformation passes
│   ├── test/               # Unit tests
│   └── tools/
│       └── ASTGen/         # AST code generator (Python)
```

---

## Dependencies

| Dependency | Version | Purpose |
|------------|---------|---------|
| GCC / Clang | C++17 | Compiler |
| CMake | ≥ 3.10 | Build system |
| Flex | 2.6.4 | Verilog scanner |
| Bison | 3.8.2 | Verilog parser |
| Boost | 1.85.0 | Program options, filesystem, logging |
| yaml-cpp | 0.8.0 | YAML parameter map parsing |
| GMP / GMPXX | 6.3.0 | Arbitrary precision arithmetic |
| GoogleTest | 1.17.0 | Unit testing |
| Verilator | ≥ 5.050 | Cosim test suite (build-time); the floor is semantic — earlier versions mis-schedule initial-block `<=` under `--timing` |
| Yosys | ≥ 0.10 | Synthesis checks in the verilower test suite (dev/CI only, optional — skipped when absent; conda-forge has no yosys for osx-arm64 or win-64) |

---

## Building

Veriparse uses a **Conda-based development environment** managed via the `Makefile`.

### 1. Install micromamba

On Fedora/RHEL:

    sudo dnf install micromamba

On other distributions, use the official installer:

    "${SHELL}" <(curl -L micro.mamba.pm/install.sh)

### 2. Create the Development Environment

    make dev-env


This creates a conda environment named `veriparse-dev` with all required dependencies fetched from `conda-forge`.

To use a different mamba implementation (e.g. full `mamba`):

    make dev-env MAMBA=mamba

### 3. Configure with CMake

    make dev-cmake

This runs CMake and generates build files in `build/`.

### 4. Build

    make dev-build

This compiles the library, tools, and tests using all available CPU cores.

### 5. Run Tests

Tests are organized into two labeled groups:

| Label | Description |
|-------|-------------|
| `unittest` | C++ unit tests (GoogleTest) |
| `cosim` | Verilator-built C++ model of the design (optionally piped through veriflat), driven by a GoogleTest harness |

    # Run unit tests (default)
    make dev-test

    # Run only unit tests
    make dev-test CTEST_LABELS=unittest

    # Run the cosim suite
    make dev-test-cosim

You can also run ctest directly from the build directory:

    cd build
    ctest -L unittest               # C++ unit tests
    ctest -L cosim                  # cosim tests
    ctest                           # everything

### 6. Test Coverage

    make dev-coverage

Configures and builds a separate `build-coverage/` tree (`CMAKE_BUILD_TYPE=Coverage`:
`-O0 --coverage`, GCC/gcov only — not available on the Windows/ClangCL build), runs
the `unittest` suite, and generates a [gcovr](https://gcovr.com/) report: a summary
printed to the terminal plus an HTML report at `build-coverage/coverage/index.html`.
Test sources themselves (`lib/test/`, `apps/*/test/`) are excluded from the report.

Run against the cosim suite instead with:

    make dev-coverage COVERAGE_CTEST_LABELS=cosim

### 7. Clean Up

    make dev-clean

This removes the build directories (`build/`, `build-coverage/`) and the conda environment.

### On Windows

Windows uses `dev.ps1` instead of the Makefile (PowerShell-native,
no MSYS/git-bash dependency). It shares `conda/recipe-release/meta.yaml`
as the single source of truth for the dev environment. Prerequisite:
[micromamba](https://mamba.readthedocs.io/en/latest/installation/micromamba-installation.html)
on PATH (e.g. `winget install Anaconda.Micromamba`).

    .\dev.ps1 env        # generate environment.yml + create build\env
    .\dev.ps1 cmake      # configure with VS17 2022 + ClangCL
    .\dev.ps1 build      # build the active configuration
    .\dev.ps1 test       # run ctest -L unittest
    .\dev.ps1 test-cosim # run ctest -L cosim (no-op on Windows: verilator
                         # is not on conda-forge for win-64)
    .\dev.ps1 all        # env -> cmake -> build -> test
    .\dev.ps1 clean      # remove build\

In Visual Studio or VS Code: open the repo folder, the included
`CMakePresets.json` lets the IDE configure and build directly once
`.\dev.ps1 env` has populated `build\env`.

---

## Running a Single Cosim Test

The cosim binaries live under `build/apps/veriparse/test_cosim_*` and can
be invoked directly (useful for `--gtest_filter`, attaching a debugger,
or running with extra Verilator runtime flags):

    ./build/apps/veriparse/test_cosim_dclkfifolut_flat_inlined
    ./build/apps/veriparse/test_cosim_hynoc_router_5p_dut --gtest_filter='*Multicast*'

---

## AST Code Generation

The AST node classes are generated by a Python tool located in `lib/tools/ASTGen/`. If you modify the AST node definitions, regenerate the AST files with:

```sh
cd lib/tools/ASTGen
python astgen.py
```

---

## Continuous Integration & Releases

Veriparse uses two GitHub Actions workflows:

| Workflow | Trigger | What runs |
|----------|---------|-----------|
| `ci.yml` | Push / PR on `master`, weekly schedule, manual | Build + unit tests on `linux-64` (via `make dev-*`) and `win-64` (via `.\dev.ps1`) on every push/PR. `osx-64` + `osx-arm64` join on push to master, weekly schedule, and when manually requested. |
| `release.yml` | Published GitHub release, manual | Conda packages for `linux-64`, `osx-64`, `osx-arm64`, and `win-64` (`win-64` is built with `conda/recipe-release/bld.bat` using VS17 2022 + ClangCL). |

`release.yml` exposes `include_macos` and `include_windows` toggles for
`workflow_dispatch` runs so you can spot-check a single platform
without burning runner minutes on the others (macOS runners cost ~10×
Linux). On a published release, all four platforms always run and their
`.conda` artifacts are attached to the release page.

| How `release.yml` was triggered | linux-64 | osx-64 / osx-arm64 | win-64 | Uploads to a GitHub Release |
|---------------------------------|:--------:|:-------------------:|:------:|:---------------------------:|
| `gh workflow run release.yml`                                                 | ✓ | ✓ | ✓ | no  |
| `gh workflow run release.yml -f include_macos=false -f include_windows=true`  | ✓ | — | ✓ | no  |
| GitHub Release published (`gh release create vX.Y.Z`)                         | ✓ | ✓ | ✓ | yes |

In short:

- **Day-to-day iteration**: just `git push` — CI verifies `linux-64` + `win-64` cheaply.
- **Before tagging a release**: run `gh workflow run release.yml` for a full four-platform smoke test (artifacts uploaded to the run page, not attached to a release).
- **Actual release**: `gh release create vX.Y.Z ...` builds all four platforms and attaches the `.conda` packages to the release page.

---

## License


> **Relicensing notice:** All prior versions and commits of this repository
> are retroactively relicensed under LGPLv3. See [RELICENSING.md](RELICENSING.md)
> for details.

Veriparse is distributed under the **GNU Lesser General Public License v3 (LGPLv3)**.
See [LICENSE](LICENSE) for the full license text.

### Note on Generated Output

As is conventional for compilers and code transformation tools, the license of
the output generated by veriparse is determined solely by the license of the
**input Verilog files**. Processing a proprietary Verilog design with veriparse
does **not** impose the LGPLv3 on the resulting output — the output retains the
original license of the input files.

### Third-Party Licenses

- **GMP / GMPXX**: LGPLv3 (dynamically linked)
