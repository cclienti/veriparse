module tdef_err_unknown0(input logic a, output logic b);
  unknown_t x;
  assign x = a;
  assign b = x;
endmodule
