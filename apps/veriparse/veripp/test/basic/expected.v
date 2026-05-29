`line 1 "top.v" 0
`line 1 "widths.vh" 1
`line 2 "top.v" 2
module top(
  input  wire clk,
  input  wire [8-1:0] d,
  input  wire [4-1:0] a,
  output reg  [8-1:0] q
);
  always @(posedge clk) begin
    q <= d & {8{a[0]}};
  end
endmodule
