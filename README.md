# Veriparse

Veriparse is a Verilog design processing toolkit that provides **flattening** and **obfuscation** of synthesizable Verilog designs. It is intended to help IP vendors protect their designs while still delivering functional, simulatable Verilog netlists.

## Overview

Veriparse takes a hierarchical Verilog design, flattens it into a single module, optionally inlines parameters, eliminates dead code, and obfuscates identifiers. The resulting file is functionally equivalent to the original but is much harder to reverse-engineer.

The toolkit is built around a C++ library (`veriparse_static`) that implements:
- A complete **Verilog parser** (Flex/Bison based)
- An **AST** (Abstract Syntax Tree) framework
- A set of **transformation passes** (constant folding, dead code elimination, loop unrolling, module flattening, obfuscation, etc.)
- **Verilog and YAML generators**

## Tools

### `veriflat` — Verilog Flattener

`veriflat` takes a pre-processed Verilog file and flattens the design hierarchy into a single module. It supports partial parameter inlining: you can specify which parameters to keep (preserve as module parameters) and which to inline (fold as constants).

```
Usage: veriflat [options] verilog-file [verilog-file ...]

options:
  -h [ --help ]            Produce help message
  -v [ --version ]         Show the version and exit
  -o [ --output ] arg      Output file
  -t [ --top-module ] arg  Top module name
  -p [ --param-map ] arg   YAML parameter map
  -e [ --deadcode-end ]    Remove dead code after flatten pass
  -d [ --deadcode-during ] Remove dead code during flatten pass
  -s [ --seed ] arg        Seed value (default: 0)
```

**Parameter map format (YAML):**
- `{}` — inline all parameters (fully flatten)
- `{PARAM_A:}` — keep `PARAM_A` as a module parameter, inline all others
- `'{PARAM_A: null, PARAM_B: null}'` — keep `PARAM_A` and `PARAM_B`, inline all others
- `{PARAM_A: 42}` — override `PARAM_A` with value 42, inline all others

**Example:**
```sh
# Pre-process with iverilog
iverilog -E -g2005 -I src/ src/top.v src/sub.v -o top_pp.v

# Flatten, keeping FIFO_WIDTH as a parameter
veriflat -p '{FIFO_WIDTH:}' --seed 0 --top-module top top_pp.v --output top_flat.v
```

---

### `veriobf` — Verilog Obfuscator

`veriobf` takes a flattened Verilog file and obfuscates all internal identifiers (signals, instances, etc.) using random or hashed names.

```
Usage: veriobf [options] verilog-file

options:
  -h [ --help ]          Produce help message
  -v [ --version ]       Show the version and exit
  -o [ --output ] arg    Output file
  -l [ --id-length ] arg Maximum length of obfuscated identifiers (default: 16)
  -a [ --hash ]          Use hashed identifiers instead of random ones
  -s [ --seed ] arg      Seed value (default: 0)
```

**Example:**
```sh
veriobf --id-length 16 --seed 0 top_flat.v --output top_obf.v
```

---

### `veriparse` — All-in-One Script

A convenience shell script that chains the pre-processing (iverilog), flattening (veriflat), and obfuscation (veriobf) steps into a single command.

```sh
veriparse -o output.v \
          -t top_module \
          -p '{FIFO_WIDTH:}' \
          src/*.v
```

---

## Transformation Passes

The library implements the following transformation passes:

| Pass | Description |
|------|-------------|
| `AnnotateDeclaration` | Annotates declarations with scope information |
| `AnnotateScope` | Annotates AST nodes with scope information |
| `AstReplace` | Generic AST node replacement |
| `BranchSelection` | Selects branches based on constant conditions |
| `ConstantFolding` | Evaluates and folds constant expressions |
| `DeadcodeElimination` | Removes unused signals and assignments |
| `ExpressionEvaluation` | Evaluates expressions at compile time |
| `FunctionEvaluation` | Inlines and evaluates Verilog functions |
| `GenerateRemoval` | Resolves and removes `generate` blocks |
| `LocalparamInliner` | Inlines `localparam` values |
| `LoopUnrolling` | Unrolls `for` loops |
| `ModuleFlattener` | Flattens module hierarchy |
| `ModuleInstanceNormalizer` | Normalizes module instantiations |
| `ModuleIoNormalizer` | Normalizes module I/O declarations |
| `ModuleObfuscator` | Obfuscates internal identifiers |
| `ParameterInliner` | Inlines parameter values |
| `ResolveModule` | Resolves module references |
| `ScopeElevator` | Elevates declarations to the correct scope |
| `VariableFolding` | Folds constant variable assignments |

---

## Project Structure

```
veriparse/
├── apps/
│   └── veriparse/
│       ├── veriflat/       # veriflat tool
│       ├── veriobf/        # veriobf tool
│       ├── scripts/        # veriparse convenience script
│       └── test/           # Integration tests
├── cmake/                  # CMake modules and common settings
├── conda/                  # Conda build and dev environment
│   └── Makefile            # Main build entry point
├── external/               # External dependency build scripts
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
└── licenses/               # License files
```

---

## Dependencies

| Dependency | Version | Purpose |
|------------|---------|---------|
| GCC / Clang | C++17 | Compiler |
| CMake | ≥ 3.9 | Build system |
| Flex | any | Verilog scanner |
| Bison | any | Verilog parser |
| Boost | ≥ 1.72 | Program options, filesystem, logging |
| yaml-cpp | 0.6.3 | YAML parameter map parsing |
| GMP / GMPXX | 6.2.0 | Arbitrary precision arithmetic |
| GoogleTest | 1.10.0 | Unit testing |
| iverilog | any | Verilog pre-processing (runtime) |

---

## Building

Veriparse uses a **Conda-based development environment** managed via the `conda/Makefile`.

### 1. Install Miniconda

```sh
wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh
bash Miniconda3-latest-Linux-x86_64.sh -b -p $HOME/miniconda
eval "$($HOME/miniconda/bin/conda shell.bash hook)"
```

### 2. Create the Development Environment

```sh
cd conda
make dev-env
```

This creates a Conda environment named `veriparse-dev` with all required dependencies.

### 3. Configure with CMake

```sh
make dev-cmake
```

This runs CMake and generates build files in `conda/build/`.

### 4. Build

```sh
make dev-build
```

This compiles the library, tools, and tests using all available CPU cores.

### 5. Run Tests

```sh
make dev-test
```

This runs the full test suite using CTest (unit tests + integration tests).

### 6. Clean Up

```sh
make dev-clean
```

This removes the build directory and the Conda environment.

---

## Running the Integration Tests Manually

Integration tests use [Icarus Verilog](http://iverilog.icarus.com/) for simulation. Make sure `iverilog` and `vvp` are available in your `PATH`:

```sh
# Debian/Ubuntu
apt install iverilog

# Fedora/RHEL
yum install iverilog
```

To run a single test manually:

```sh
cd conda/build/apps/veriparse/test/dclkfifolut/project0

make -f /path/to/veriparse/apps/veriparse/test/dclkfifolut/project0/Makefile \
    VERIFLAT=/path/to/conda/build/apps/veriparse/veriflat/veriflat \
    VERIOBF=/path/to/conda/build/apps/veriparse/veriobf/veriobf \
    clean iverilog_check veriflat_check veriobf_check
```

Each test runs three checks:
- `iverilog_check` — simulate the original design
- `veriflat_check` — simulate the flattened design
- `veriobf_check` — simulate the obfuscated design

---

## AST Code Generation

The AST node classes are generated by a Python tool located in `lib/tools/ASTGen/`. If you modify the AST node definitions, regenerate the AST files with:

```sh
cd lib/tools/ASTGen
python astgen.py
```

---

## License

Veriparse is distributed under the **GNU Lesser General Public License v3 (LGPLv3)**.
See [licenses/LICENSE](licenses/LICENSE) for the full license text.

### Note on Generated Output

As is conventional for compilers and code transformation tools, the license of
the output generated by veriparse is determined solely by the license of the
**input Verilog files**. Processing a proprietary Verilog design with veriparse
does **not** impose the LGPLv3 on the resulting output — the output retains the
original license of the input files.

### Third-Party Licenses

- **GMP / GMPXX**: [LGPLv3](licenses/gnugmp-lgplv3.txt)
- **OpenSSL / SSLeay**: [licenses/license-openssl-ssleay.txt](licenses/license-openssl-ssleay.txt)
