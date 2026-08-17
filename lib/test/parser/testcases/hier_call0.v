// Hierarchical task enables and function calls (IEEE 1364-2005 A.6.9, A.8.2):
// one level, several levels, an indexed label, with and without arguments.
// Parse-only coverage — resolution decides separately what is supported.
module hier_call0(input wire [7:0] a, output wire [7:0] w);

  assign w = u.f(a);

  initial begin
    u.t(1);
    top.u.t;
    blk[0].u.t(2);
  end

endmodule
