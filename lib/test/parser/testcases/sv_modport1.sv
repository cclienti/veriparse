interface sv_modport1;

  logic a;
  logic b;

  modport m0 (input a, output b), m1 (inout a, inout b);

endinterface
