// Packed union members must share one width (§7.3.1).
module struct_err_union_width0(input logic [7:0] a, output logic [7:0] b);
  union packed {
    logic [7:0] raw;
    logic [3:0] nib;
  } u;
  always @(*) u.raw = a;
  assign b = u.raw;
endmodule
