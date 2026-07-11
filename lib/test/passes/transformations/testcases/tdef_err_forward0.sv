// A forward typedef referenced before (here: without) its completing
// definition is a hard error.
module tdef_err_forward0(input logic a, output logic b);
  typedef enum fw_t;
  fw_t x;
  assign b = a;
endmodule
