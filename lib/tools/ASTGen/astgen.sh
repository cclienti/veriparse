#!/bin/sh

ipython3 --pdb ./astgen.py -- -t templates -i ../../include/veriparse -c ../../src verilog_ast.yaml
