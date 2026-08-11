# `veridump` — AST Dumper

`veridump` parses a Verilog or SystemVerilog file and dumps its abstract
syntax tree in **YAML** or **Graphviz DOT** format. It is the fastest way to
see exactly what the parser understood — for debugging a construct, scripting
over a design's structure, or rendering the tree for documentation.

The YAML output round-trips: the library's YAML importer rebuilds the same
AST from it, so a dump can serve as a golden reference in tests or as an
exchange format between tools.

## Command-line reference

```
Usage: veridump [options] verilog-file [verilog-file ...]

options:
  -h [ --help ]                Produce help message
  -v [ --version ]             Show the version and exit
  -o [ --output ] arg          Output file
  -f [ --format ] arg (=yaml)  Output format: yaml or dot
  --sv                         Enable SystemVerilog mode
  -I [ --include-dir ] arg     Add directory to `include search path (repeatable)
  -D [ --define ] arg          Predefine a macro as NAME or NAME=BODY (repeatable)
  -U [ --undef ] arg           Cancel a predefine NAME (repeatable)
  --log arg                    Log to FILE instead of stderr
```

Preprocessing is built in (`-I`, `-D`, `-U`); `--sv` selects the
SystemVerilog grammar.

## Examples

```sh
# Dump AST as YAML
veridump --sv -f yaml -o design.yaml design.sv

# Dump AST as DOT and render with graphviz
veridump --sv -f dot -o design.dot design.sv
dot -Tsvg design.dot -o design.svg
```

A rendered example (the `nbit_reg` module from the
[`veriflat` documentation](veriflat.md)):

![AST example](images/nbit_reg_ast_before.svg)

## Notes

- Every node carries its source `filename` and `line`, so a YAML dump can be
  grepped to locate constructs in large designs.
- The AST node set is generated from
  `lib/tools/ASTGen/verilog_ast.yaml` — that file is the authoritative
  reference for node types and their attributes.
