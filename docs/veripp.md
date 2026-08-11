# `veripp` — Verilog Preprocessor

`veripp` runs the Verilog/SystemVerilog preprocessor alone: it expands
`` `include `` and `` `define `` macros, honours `` `ifdef ``/`` `ifndef ``
conditionals, and writes the resulting single compilation unit. All other
veriparse tools embed the same preprocessor, so `veripp` exists for
inspection and pipelines: seeing exactly what the parser will consume, or
feeding a preprocessed file to an external tool.

## Command-line reference

```
Usage: veripp [options] verilog-file [verilog-file ...]

options:
  -h [ --help ]             Produce help message
  -v [ --version ]          Show the version and exit
  -o [ --output ] arg       Output file (default: stdout)
  -I [ --include-dir ] arg  Add directory to `include search path (repeatable)
  -D [ --define ] arg       Predefine a macro as NAME or NAME=BODY (repeatable)
  -U [ --undef ] arg        Cancel a predefine NAME (repeatable)
  --sv                      Enable SystemVerilog mode
  --log arg                 Log to FILE instead of stderr
```

## Example

```sh
# Expand includes and macros, with BOARD defined, and inspect the result
veripp -I rtl/include -D BOARD=de10 rtl/top.sv -o top_pp.sv
```
