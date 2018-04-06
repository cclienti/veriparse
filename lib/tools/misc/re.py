#!/usr/bin/env python

import re
import sys

f = open(sys.argv[1])
sin = f.read()
f.close()

# r1 = re.compile(r"[ ]*width:\n[ ]*Width: null\n", re.MULTILINE)
# stmp = re.sub(r1, "", sin)

# r2 = re.compile(r"width:\n([ ]*)  Width:", re.MULTILINE)
# sout = re.sub(r2, "widths:\n\g<1>- Width:", stmp)

r2 = re.compile(r"          paramlist:\n            Paramlist: null\n", re.MULTILINE)
stmp = re.sub(r2, "", sin)

r2 = re.compile(r"          portlist:\n            Portlist: null\n", re.MULTILINE)
sout = re.sub(r2, "", stmp)

# sys.stdout.write(sout)
open(sys.argv[1], "w").write(sout)
