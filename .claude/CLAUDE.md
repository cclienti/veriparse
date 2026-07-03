# CLAUDE.md

Guidance for working in **Veriparse** — a source-to-source transformation toolkit
for synthesizable Verilog / SystemVerilog (flattening, parameter inlining, dead-code
elimination, loop unrolling, obfuscation). C++17 library (`veriparse_static`) plus the
`veriflat` / `veridump` / `veriobf` CLI tools.

## Build & test (always via the Makefile / conda env)

The dev environment is a local conda env at `build/env`; never invoke `cmake`/`ctest`
directly without it. Wrappers run through `micromamba run`:

```sh
make dev-env     # create build/env (one-time; reads conda/recipe-release/meta.yaml)
make dev-cmake   # configure Ninja build in build/ (RelWithDebInfo, mold linker)
make dev-build   # build lib + tools + tests
make dev-test                       # ctest -L unittest  (default)
make dev-test CTEST_LABELS=cosim    # or: make dev-test-cosim  (Verilator)
```

- Tests are labeled `unittest` (GoogleTest) and `cosim` (Verilator-built model).
- `ctest` needs `VERIPARSE_SOURCE_ROOT` set — `make dev-test` does this; if running
  `ctest` by hand from `build/`, export it yourself.
- Single cosim binary: `./build/apps/veriparse/test_cosim_<name>` (supports `--gtest_filter`).
- Windows uses `dev.ps1` (same `meta.yaml` as source of truth); don't edit one without the other.

## Code generation — the AST is generated

AST node classes, the YAML/dot generators, the importers, and their tests are
**generated** from `lib/tools/ASTGen/verilog_ast.yaml`. Do not hand-edit generated
files under the `AST_GEN_PATHS` (see Makefile). Instead edit the yaml/templates, then:

```sh
make ast-gen     # regenerate (clang-format on by default)
make ast-check   # CI guard: fails if committed generated AST is stale vs the yaml
```

Commit the regenerated files together with the yaml change.

## Layout

- `lib/src/parser/` — Flex/Bison Verilog + SystemVerilog grammar.
- `lib/src/passes/` — transformation & analysis passes (see `docs/passes.md`).
- `lib/src/AST/`, `lib/src/generators/`, `lib/src/importers/` — **generated**.
- `apps/veriparse/{veriflat,veriobf}/` — CLI tools; `apps/.../test/` integration tests.
- `docs/` — ADRs and design specs (see below).

## Style

- C++17. `clang-format` is enforced by `scripts/pre-commit` (`make install-hooks`).
  Match surrounding code; run `clang-format -i <file>` before committing `.cpp`/`.hpp`.
- Comments describe code intent only — never reference the dev workflow, review process,
  `make dev-test`, `VERIPARSE_SOURCE_ROOT`, or similar meta.

## Specs & ADRs — the standard is the source of truth

Design decisions live in `docs/` as numbered ADRs (`adr-NNNN-*.md`) plus design notes
(e.g. `passes.md`, the passes reference). Browse `docs/` for the current set before
starting work that touches types, interfaces, parser resolution, or packages — and add a
new ADR rather than reworking a decision silently.

Normative references, pinned by parse mode (clause/annex numbers shift between editions):
- SystemVerilog → **IEEE 1800-2017** (`docs/1800-2017.pdf`)
- Verilog → **IEEE 1364-2005** (`docs/verilog-std-1364-2005.pdf`)

At the slightest doubt about what is legal, defer to the standard (both Annex A BNF and
the clause prose), never to memory or an existing comment.

## Commits

Commit messages **must follow [Conventional Commits](https://www.conventionalcommits.org/)**:
`type(scope): summary` (`feat`, `fix`, `refactor`, `test`, `docs`, `revert`, …; scope =
`parser`, `passes`, `veriflat`, `scanner`, `ast`, `generators`, …; `!` after the scope for
breaking changes). Keep the summary imperative and under ~72 chars.

Two rules for every commit:

1. **No `Co-Authored-By: Claude` / "Generated with Claude Code" trailers, and no
   `claude.ai/code` session URLs** — not in commit messages, not in PR bodies — even when
   the tooling suggests appending them. Instead put just the model name/version on its
   own line, e.g. `Model: claude-opus-4-8`.
2. **Reference the relevant ADR or spec at the end of the message body** (not in the
   subject line), citing the ADR by its `docs/` filename, and cite the IEEE clause when
   the change implements a specific rule. Use a dedicated trailer, e.g.
   `Refs: docs/adr-0001-type-declaration-split.md §3.6` or `Refs: IEEE 1800-2017 §26.3`.

Example:

```
feat(passes): enforce package compilation order in run_units

PackageInliner now resolves compilation units in declaration order so
forward references across $unit imports fail loudly instead of silently
binding to a later definition.

Refs: ADR-0001 §3.6, IEEE 1800-2017 §26.3
Model: claude-opus-4-8
```
