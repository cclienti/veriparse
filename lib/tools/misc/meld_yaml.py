#!/usr/bin/env python

import sys
import os
import tempfile
import yaml
import argparse


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Indent a YAML file or stdin')
    parser.add_argument('infiles', type=str, nargs='+', help='YAML file')

    args = parser.parse_args()

    to_compare = []

    for f in args.infiles:
        basename = "_" + os.path.basename(f)

        try:
            yamltree = yaml.load(open(f).read())
        except IOError:
            sys.stderr.write("Cannot open file '{}'\n".format(f))
            sys.exit(1)

        to_compare.append(tempfile.NamedTemporaryFile(prefix=".", suffix=basename))
        yaml.dump(yamltree, to_compare[-1], default_flow_style=False)
    to_meld = [f.name for f in to_compare]

    os.execvp("meld", ['meld'] + to_meld)
