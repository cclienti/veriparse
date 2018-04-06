#!/usr/bin/env python

import sys
import yaml
import argparse


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Indent a YAML file or stdin')
    parser.add_argument('infile', type=str, nargs='?', help='YAML file')

    args = parser.parse_args()

    if args.infile is None:
        yamlstr = sys.stdin.read()
    else:
        try:
            yamlstr = open(args.infile).read()
        except IOError:
            sys.stderr.write("Cannot open file '{}'\n".format(args.infile))
            sys.exit(1)

    print(yaml.dump(yaml.load(yamlstr), default_flow_style=False))
