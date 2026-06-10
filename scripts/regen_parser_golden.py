#!/usr/bin/env python3
# SPDX-License-Identifier: LGPL-3.0-or-later
# Copyright (C) 2013-2026 Christophe Clienti
#
# Regenerate parser-test golden YAML from the `<Test>_parsed.yaml` files a test
# run drops in the repo root. The golden is the parsed AST re-emitted block-style
# with sorted keys and the testcase path normalized to the form the committed
# goldens use (`../../test/parser/testcases/<name>.v`). `is_equal(attrs=false)`
# ignores filename/line, so the path is cosmetic — normalized only for a clean,
# stable diff.
#
# Usage:
#   python3 scripts/regen_parser_golden.py var0 var1 ...      # specific cases
#   python3 scripts/regen_parser_golden.py --all             # every *_parsed.yaml
#
# Run the parser tests first (they write the *_parsed.yaml), VERIFY the parses
# are correct, then regenerate.

import sys
import glob
import os
import re
import yaml

PARSED_GLOB = "VerilogParserTest.{name}_parsed.yaml"
GOLDEN_DIR = "lib/test/parser/testcases"
SRC_PREFIX = "lib/test/parser/testcases/"
GOLDEN_PREFIX = "../../test/parser/testcases/"


def normalize_paths(obj):
    """Rewrite testcase filenames to the committed-golden form, in place."""
    if isinstance(obj, dict):
        for k, v in obj.items():
            if k == "filename" and isinstance(v, str) and v.startswith(SRC_PREFIX):
                obj[k] = GOLDEN_PREFIX + v[len(SRC_PREFIX):]
            else:
                normalize_paths(v)
    elif isinstance(obj, list):
        for e in obj:
            normalize_paths(e)


def regen(name):
    parsed = PARSED_GLOB.format(name=name)
    if not os.path.exists(parsed):
        sys.stderr.write("missing {} (run the parser test first)\n".format(parsed))
        return False
    with open(parsed) as f:
        tree = yaml.safe_load(f)
    normalize_paths(tree)
    out = os.path.join(GOLDEN_DIR, name + ".yaml")
    with open(out, "w") as f:
        yaml.dump(tree, f, default_flow_style=False, sort_keys=True)
    sys.stderr.write("regenerated {}\n".format(out))
    return True


def main():
    args = sys.argv[1:]
    if not args:
        sys.stderr.write(__doc__)
        return 1
    if args == ["--all"]:
        names = [re.match(r"VerilogParserTest\.(.+)_parsed\.yaml", os.path.basename(p)).group(1)
                 for p in glob.glob("VerilogParserTest.*_parsed.yaml")]
    else:
        names = args
    ok = all(regen(n) for n in sorted(set(names)))
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
