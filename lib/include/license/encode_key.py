#!/usr/bin/env python

import sys
import random
import argparse
import time


def read_key(key_filename):
    """return Keyfile dumped into a string"""
    try:
        key_f = open(key_filename, 'rb')
        key_file = key_f.read()

    except IOError:
        sys.stderr.write('could not read %s!\n' % key_filename)
        sys.exit(1)

    return key_file


def build_c_string(string):
    string_split = string.splitlines()
    c_const = "\\\n"
    for i in range(len(string_split)):
        c_const += "\t\t"
        c_const += '"%s\\n"' % string_split[i]
        if i != len(string_split) - 1:
            c_const += ' \\\n'

    return c_const


def generate_c_string(key, cpp_template_filename, pattern):
    try:
        f = open(cpp_template_filename, 'rb')
        cpp_template = f.read()

    except IOError:
        sys.stderr.write('could not read %s!\n' % cpp_template_filename)
        sys.exit(1)

    key = key.splitlines()
    key_len = len(key)
    hidden_key = ''
    key_array_init = "{\n\t\t\t"

    for line_index in range(key_len):
        xkey = random.randint(1, 255)
        hidden_key += 'DEFINE_HIDDEN_STRING(%s_%d, %d, ' % (pattern, line_index, xkey)
        for c in key[line_index]:
            hidden_key += '(\'%s\')' % c
        hidden_key += '(\'\\n\'))\n\n'
        key_array_init += "create_%s_%d()" % (pattern, line_index)
        if line_index == key_len - 1:
            key_array_init += "\n\t\t}"
        else:
            key_array_init += ",\n\t\t\t"

    cpp_template = cpp_template.replace("{{hidden_key}}", hidden_key)
    cpp_template = cpp_template.replace("{{hidden_name}}", pattern)
    cpp_template = cpp_template.replace("{{hidden_array_init}}", key_array_init)

    sys.stdout.write(cpp_template)


def main():
    parser = argparse.ArgumentParser(description='Convert a key file to a cpp header')
    parser.add_argument('key', type=str, help='key file to hide')
    parser.add_argument('cpp_template', type=str, help='C++ template file')
    parser.add_argument('pattern', type=str, help='pattern to replace in the template')
    args = parser.parse_args()

    key = read_key(args.key)
    sys.stdout.write("// File generated the %s, do not edit!\n" % time.strftime("%Y-%m-%d %H:%M:%S"))
    sys.stdout.write("// Command: %s\n\n" % " ".join(sys.argv))
    generate_c_string(key, args.cpp_template, args.pattern)


if __name__ == '__main__':
    main()
