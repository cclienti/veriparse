// A signed member inside an lvalue's INDEX expression is rvalue context:
// the signed'() re-wrap must survive there (a negative offset must not
// become a large positive index).
module struct_signed_idx0(input logic clk, input logic [2:0] base,
                          input logic [7:0] a, output logic [7:0] y);

  struct packed {
    logic signed [3:0] off;
  } s;

  logic [7:0] mem [8];

  always @(*) s.off = -4'sd1;

  always @(posedge clk)
    mem[base + s.off] = a;

  assign y = mem[0];

endmodule
