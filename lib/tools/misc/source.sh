#!/bin/bash

function veriparse_meld_yaml {
   meld_yaml="$HOME/src/veriparse/lib/tools/misc/meld_yaml.py"
   $meld_yaml ${1} ${2}
}

function veriparse_new_ref {
   yaml_printer="$HOME/src/veriparse/lib/tools/misc/yaml_printer.py"
   $yaml_printer ${1} > ${2}
}
