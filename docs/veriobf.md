# `veriobf` — Verilog Obfuscator

`veriobf` renames all internal identifiers of a design — signals, instances,
blocks, functions — to meaningless random or hashed names, while leaving the
module's external interface (name, ports, kept parameters) intact. The output
remains valid, simulatable, synthesizable RTL; it is simply much harder to
read.

It is designed to run on a **flattened** design (see
[`veriflat`](veriflat.md)): flattening first removes the hierarchy and
internal module names, so obfuscation then leaves nothing meaningful behind.
The typical IP-delivery pipeline is `veriflat` → `veriobf`.

## Command-line reference

```
Usage: veriobf [options] verilog-file

options:
  -h [ --help ]                 Produce help message
  -v [ --version ]              Show the version and exit
  -o [ --output ] arg           Output file
  -l [ --id-length ] arg (=16)  Maximum length of obfuscated identifiers
  -a [ --hash ]                 Use hashed identifiers instead of random ones
  --sv                          Enable SystemVerilog mode
  -I [ --include-dir ] arg      Add directory to `include search path (repeatable)
  -D [ --define ] arg           Predefine a macro as NAME or NAME=BODY (repeatable)
  -U [ --undef ] arg            Cancel a predefine NAME (repeatable)
  -s [ --seed ] arg (=0)        Seed value
  --log arg                     Log to FILE instead of stderr
```

- `--id-length` bounds the length of the generated identifiers.
- `--hash` derives each new name deterministically from the original by
  hashing, instead of drawing random names — useful when separate files must
  agree on the renaming.
- `--seed` makes the random naming reproducible: the same seed on the same
  input yields the same output.

## Example

```sh
# Flatten, then obfuscate
veriflat --seed 0 -t top -o top_flat.v src/top.v src/sub.v
veriobf --id-length 16 --seed 0 top_flat.v --output top_obf.v
```

## Notes

- Port names and the top module name are preserved — they are the design's
  contract with the outside.
- The license of the output is the license of the *input*: obfuscating a
  proprietary design does not change its licensing (see the note on
  generated output in the top-level README).
