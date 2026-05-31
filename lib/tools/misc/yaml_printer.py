#!/usr/bin/python
# SPDX-License-Identifier: LGPL-3.0-or-later
# Copyright (C) 2013-2026 Christophe Clienti
"""Beautify a YAML file"""

import sys
import argparse
import yaml


def main():
    """Main procedure"""

    parser = argparse.ArgumentParser(description="Indent a YAML file or stdin")
    parser.add_argument("infile", type=str, nargs="?", help="YAML file")

    args = parser.parse_args()

    if args.infile is None:
        yamlstr = sys.stdin.read()
    else:
        try:
            yamlstr = open(args.infile).read()
        except IOError:
            sys.stderr.write("Cannot open file '{}'\n".format(args.infile))
            sys.exit(1)

    # sort_keys=False is essential: a "beautify" must not reorder keys.
    # YAML key order is semantically significant for some files (notably
    # lib/tools/ASTGen/verilog_ast.yaml, where field order drives the
    # generated AST constructor/member order). Python 3.7+ dicts and the
    # standard yaml.Loader already preserve document order on load, so this
    # round-trips order-faithfully without a custom ordered loader.
    print(
        yaml.dump(
            yaml.load(yamlstr, Loader=yaml.Loader),
            default_flow_style=False,
            sort_keys=False,
            Dumper=yaml.Dumper,
        )
    )


if __name__ == "__main__":
    main()
