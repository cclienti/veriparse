// A body `parameter type` legally follows the typedef its default names
// (§6.18): it reduces in place, never hoisted past its reference.
module tdef_tp_order0(input logic [7:0] a, output logic [7:0] b);

  logic [7:0] r;
  typedef logic [7:0] my_t;
  parameter type T = my_t;

  T v;
  assign r = a;
  assign v = r;
  assign b = v;

endmodule
