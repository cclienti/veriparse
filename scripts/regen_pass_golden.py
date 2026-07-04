#!/usr/bin/env python3
# SPDX-License-Identifier: LGPL-3.0-or-later
# Copyright (C) 2013-2026 Christophe Clienti
#
# Regenerate transformation-pass golden YAML from the `<Suite>.<case>.yaml`
# files a test run drops in the build test dir. Each pass test renders its
# transformed AST to `<Suite>.<case>.yaml`, then compares it (via is_equal,
# attrs=false) against `refs/<prefix>_<case>.yaml`. The golden is that rendered
# AST re-emitted block-style in document order (sort_keys=False, matching
# lib/tools/misc/yaml_printer.py), the testcase path normalized to the committed
# form (`../test/passes/transformations/testcases/<name>.v`).
# `is_equal(attrs=false)` ignores filename/line, so the path is cosmetic.
#
# Run the pass tests first (they write the `<Suite>.<case>.yaml`), VERIFY the
# transformed output is correct (eyeball the rendered `.v`), THEN regenerate.
#
# Usage:
#   python3 scripts/regen_pass_golden.py --all            # every existing ref
#   python3 scripts/regen_pass_golden.py module_io_normalizer_module0 ...

import sys
import os
import glob
import yaml

BUILD_DIR = "build/lib/test"
REFS_DIR = "lib/test/passes/transformations/testcases/refs"
PATH_MARKER = "/lib/test/"
PATH_REPLACEMENT = "../test/"

# ref-prefix -> gtest suite name (the `<Suite>` in the produced file name).
PREFIX_TO_SUITE = {
    "annotate_declaration": "PassesTransformation_AnnotateDeclaration",
    "annotate_scope": "PassesTransformation_AnnotateScope",
    "ast_replace": "PassesTransformation_ASTReplace",
    "branch_selection": "PassesTransformation_BranchSelection",
    "constant_folding": "PassesTransformation_ConstantFolding",
    "deadcode_elimination": "PassesTransformation_Deadcode",
    "enum_elaboration": "PassesTransformation_EnumElaboration",
    "enum_inliner": "PassesTransformation_EnumInliner",
    "function_evaluation": "PassesTransformation_FunctionEvaluation",
    "generate_removal": "PassesTransformation_GenerateRemoval",
    "localparam_inliner": "PassesTransformation_LocalparamInliner",
    "loop_unrolling": "PassesTransformation_LoopUnrolling",
    "module_flattener": "PassesTransformation_ModuleFlattener",
    "module_instance_normalizer": "PassesTransformation_ModuleInstanceNormalizer",
    "module_io_normalizer": "PassesTransformation_ModuleIONormalizer",
    "module_obfuscator": "PassesTransformation_ModuleObfuscator",
    "name_resolution": "PassesTransformation_NameResolution",
    "package_inliner": "PassesTransformation_PackageInliner",
    "parameter_inliner": "PassesTransformation_ParameterInliner",
    "resolve_module": "PassesTransformation_ResolveModule",
    "scope_elevator": "PassesTransformation_ScopeElevator",
    "variable_folding": "PassesTransformation_VariableFolding",
    "wire_split": "PassesTransformation_WireSplit",
}


def normalize_paths(obj):
    """Rewrite testcase filenames to the committed-golden form, in place."""
    if isinstance(obj, dict):
        for k, v in obj.items():
            if k == "filename" and isinstance(v, str) and PATH_MARKER in v:
                obj[k] = PATH_REPLACEMENT + v.split(PATH_MARKER, 1)[1]
            else:
                normalize_paths(v)
    elif isinstance(obj, list):
        for e in obj:
            normalize_paths(e)


def split_ref(ref_basename):
    """`module_io_normalizer_module0` -> (prefix, case) using the longest known
    prefix that matches."""
    for prefix in sorted(PREFIX_TO_SUITE, key=len, reverse=True):
        if ref_basename.startswith(prefix + "_"):
            return prefix, ref_basename[len(prefix) + 1:]
    return None, None


def regen(ref_basename):
    prefix, case = split_ref(ref_basename)
    if prefix is None:
        sys.stderr.write("unknown pass prefix for {}\n".format(ref_basename))
        return False
    produced = os.path.join(BUILD_DIR, "{}.{}.yaml".format(PREFIX_TO_SUITE[prefix], case))
    if not os.path.exists(produced):
        sys.stderr.write("missing {} (run the pass test first)\n".format(produced))
        return False
    with open(produced) as f:
        tree = yaml.safe_load(f)
    normalize_paths(tree)
    out = os.path.join(REFS_DIR, ref_basename + ".yaml")
    with open(out, "w") as f:
        yaml.dump(tree, f, default_flow_style=False, sort_keys=False)
    sys.stderr.write("regenerated {}\n".format(out))
    return True


def main():
    args = sys.argv[1:]
    if not args:
        sys.stderr.write(__doc__)
        return 1
    if args == ["--all"]:
        names = [os.path.splitext(os.path.basename(p))[0]
                 for p in glob.glob(os.path.join(REFS_DIR, "*.yaml"))]
    else:
        names = args
    ok = all(regen(n) for n in sorted(set(names)))
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
